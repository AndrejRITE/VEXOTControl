#include "Ketek.h"

#include <cmath>
#include <fstream>
#include <limits>
#include <sstream>

namespace
{
    auto GenCrc16CcittFalse(const uint8_t* data, std::size_t length) -> uint16_t
    {
        uint16_t crc = 0xFFFF;

        for (std::size_t i = 0; i < length; ++i)
        {
            crc ^= static_cast<uint16_t>(data[i]) << 8;

            for (int bit = 0; bit < 8; ++bit)
            {
                if ((crc & 0x8000U) != 0U)
                {
                    crc = static_cast<uint16_t>((crc << 1U) ^ 0x1021U);
                }
                else
                {
                    crc = static_cast<uint16_t>(crc << 1U);
                }
            }
        }

        return crc;
    }

    auto ReadFloat32LE(const uint8_t* src) -> float
    {
        static_assert(sizeof(float) == 4, "Unexpected float size");

        uint32_t raw =
            (static_cast<uint32_t>(src[0])) |
            (static_cast<uint32_t>(src[1]) << 8) |
            (static_cast<uint32_t>(src[2]) << 16) |
            (static_cast<uint32_t>(src[3]) << 24);

        float value{};
        std::memcpy(&value, &raw, sizeof(value));
        return value;
    }

    auto BuildRequest(uint8_t command) -> std::array<uint8_t, 32>
    {
        std::array<uint8_t, 32> req{};
        req[0] = command;

        const uint16_t crc = GenCrc16CcittFalse(req.data(), 30);
        req[30] = static_cast<uint8_t>(crc >> 8);     // high byte
        req[31] = static_cast<uint8_t>(crc & 0xFF);   // low byte

        return req;
    }
}

auto Ketek::InitializeDevice
(
    const std::string& deviceSN,
    const std::filesystem::path& configurationFilePath
) -> bool
{
    m_LastError.clear();

    if (IsDeviceInitialized())
    {
        if (!DeInitializeDevice())
            return false;
    }

    Configuration configuration;

    if (!LoadConfiguration(
        configurationFilePath,
        configuration))
    {
        return false;
    }

    if (!ValidateConfiguration(configuration))
        return false;

    m_Configuration = std::move(configuration);
    m_DynamicRangeKeV =
        m_Configuration.dynamicRangeKeV;

    std::string logPath =
        m_Configuration.logFilePath.string();

    std::string iniPath =
        m_Configuration.handelIniPath.string();

    int status = xiaInitHandel();
    if (status != XIA_SUCCESS)
    {
        return SetError(
            "xiaInitHandel failed; Handel status = " +
            std::to_string(status)
        );
    }

    m_HandelInitialized = true;

    status = xiaSetLogLevel(m_Configuration.logLevel);
    if (status != XIA_SUCCESS)
    {
        xiaExit();

        return SetError(
            "xiaSetLogLevel failed; Handel status = " +
            std::to_string(status)
        );
    }

    status = xiaSetLogOutput(logPath.data());
    if (status != XIA_SUCCESS)
    {
        xiaExit();

        return SetError(
            "xiaSetLogOutput failed; Handel status = " +
            std::to_string(status)
        );
    }

    status = xiaLoadSystem(
        const_cast<char*>("handel_ini"),
        iniPath.data()
    );

    if (status != XIA_SUCCESS)
    {
        xiaExit();
        xiaCloseLog();

        return SetError(
            "xiaLoadSystem failed for '" +
            iniPath +
            "'; Handel status = " +
            std::to_string(status)
        );
    }

    xiaSetIOPriority(MD_IO_PRI_HIGH);

    status = xiaStartSystem();
    if (status != XIA_SUCCESS)
    {
        xiaExit();
        xiaCloseLog();

        return SetError(
            "xiaStartSystem failed; Handel status = " +
            std::to_string(status)
        );
    }

    // Serial number
    {
        std::array<char, 17> serialNumber{};

        status = xiaBoardOperation(
            m_Configuration.channel,
            const_cast<char*>("get_serial_number"),
            serialNumber.data()
        );

        if (status != XIA_SUCCESS)
        {
            DeInitializeDevice();

            return SetError(
                "Could not read KETEK serial number; Handel status = " +
                std::to_string(status)
            );
        }

        serialNumber.back() = '\0';
        m_DeviceSerialNumber = serialNumber.data();

        status = xiaBoardOperation(
            m_Configuration.channel,
            const_cast<char*>("get_usb_version"),
            &m_USBVersion
        );

        if (status != XIA_SUCCESS)
        {
            DeInitializeDevice();

            return SetError(
                "Could not read KETEK USB version; Handel status = " +
                std::to_string(status)
            );
        }
    }

    const std::string expectedSerial =
        !deviceSN.empty()
        ? deviceSN
        : m_Configuration.expectedSerialNumber;

    if (!expectedSerial.empty() &&
        m_DeviceSerialNumber != expectedSerial)
    {
        const std::string actualSerial =
            m_DeviceSerialNumber;

        DeInitializeDevice();

        return SetError(
            "Connected KETEK serial number '" +
            actualSerial +
            "' does not match expected serial number '" +
            expectedSerial +
            "'."
        );
    }

    if (!LoadPeakingTimes())
    {
        DeInitializeDevice();
        return false;
    }

    if (!ApplyAcquisitionConfiguration(m_Configuration))
    {
        DeInitializeDevice();
        return false;
    }

    if (!ReadBackAcquisitionConfiguration())
    {
        DeInitializeDevice();
        return false;
    }

    if (m_Configuration.persistToDevice)
    {
        status = xiaBoardOperation(
            m_Configuration.channel,
            const_cast<char*>("save_genset"),
            &m_GENSET
        );

        if (status != XIA_SUCCESS)
        {
            DeInitializeDevice();

            return SetError(
                "save_genset failed; Handel status = " +
                std::to_string(status)
            );
        }

        status = xiaBoardOperation(
            m_Configuration.channel,
            const_cast<char*>("save_parset"),
            &m_PARSET
        );

        if (status != XIA_SUCCESS)
        {
            DeInitializeDevice();

            return SetError(
                "save_parset failed; Handel status = " +
                std::to_string(status)
            );
        }
    }

    status = xiaBoardOperation(
        m_Configuration.channel,
        const_cast<char*>("get_temperature"),
        &m_BoardTemperature
    );

    if (status != XIA_SUCCESS)
    {
        DeInitializeDevice();

        return SetError(
            "get_temperature failed; Handel status = " +
            std::to_string(status)
        );
    }

    if (!RequestTemperature())
    {
        DeInitializeDevice();

        return SetError(
            "Detector initialized, but temperature readout failed."
        );
    }

    return true;
}

Ketek::Ketek(const std::string& deviceSN, const std::filesystem::path& configurationFilePath)
{
    InitializeDevice(deviceSN, configurationFilePath);
}

auto Ketek::CaptureData(const int exposure, unsigned long* const mca, bool * const continueCapturing) -> bool
{
    if (!IsDeviceInitialized()) return false;
    if (!mca || !continueCapturing) return false;

    const int detChan = m_Configuration.channel;

    /* Start a run w/ the MCA cleared */
    auto status = xiaStartRun(0, 0);
    if (!CHECK_ERROR(status)) return false;

    auto exposureTime = abs(exposure);
    // Started run. Sleeping for exposure
    {
        auto startCheckingTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();

        double deltaTime{};
        /* 1. Wait till the Hardware Trigger signal or till the end of Waiting Time */
        do
        {
            currentTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>
                (currentTime - startCheckingTime).count() / 1'000.0;
            if (!*continueCapturing)
            {
                xiaStopRun(detChan);
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (deltaTime < (double)exposureTime);
    }

    status = xiaStopRun(detChan);
    if (!CHECK_ERROR(status)) return false;

    /* Prepare to read out MCA spectrum */
    unsigned long mcaLen{};
    status = xiaGetRunData(detChan, (char*)"mca_length", &mcaLen);
    if (!CHECK_ERROR(status)) return false;

    if (mcaLen > (unsigned long)m_nMCA) return false;

    /* If you don't want to dynamically allocate memory here,
     * then be sure to declare mca as an array of length 8192,
     * since that is the maximum length of the spectrum.
     */
    status = xiaGetRunData(0, (char*)"mca", (void*)mca);
    if (!CHECK_ERROR(status)) return false;

    return RequestTemperature();
}

auto Ketek::DeInitializeDevice() -> bool
{
    if (!m_HandelInitialized)
        return true;

    const int status = xiaExit();

    xiaCloseLog();

    m_HandelInitialized = false;
    m_DeviceSerialNumber.clear();

    if (status != XIA_SUCCESS)
    {
        return SetError(
            "xiaExit failed; Handel status = " +
            std::to_string(status)
        );
    }

    return true;
}

Ketek::~Ketek()
{
    DeInitializeDevice();
}

auto Ketek::RequestTemperature() -> bool
{
    if (!IsDeviceInitialized())
    {
        return true;
    }

    // 0x10: board live info -> THERM_1, THERM_2
    {
        const auto request = BuildRequest(0x10);
        std::array<uint8_t, 32> response{};

        if (!SendPassthrough32(request, response))
        {
            return false;
        }

        // Basic validation
        if (response[0] != 0x10 || response[1] != 0xFF)
        {
            return false;
        }

        // Verify response CRC
        const uint16_t crc_rx =
            (static_cast<uint16_t>(response[30]) << 8) |
            static_cast<uint16_t>(response[31]);

        const uint16_t crc_calc = GenCrc16CcittFalse(response.data(), 30);
        if (crc_rx != crc_calc)
        {
            return false;
        }

        // Per manual page 23/24:
        // byte 12..15 = THERM_1 (float32)
        // byte 16..19 = THERM_2 (float32)
        m_Thermistor1Temperature = static_cast<double>(ReadFloat32LE(&response[12]));
        m_Thermistor2Temperature = static_cast<double>(ReadFloat32LE(&response[16]));
    }

    // 0x11: VIAMP live info -> SDD_TEMP, HOT_SIDE, RDY
    {
        const auto request = BuildRequest(0x11);
        std::array<uint8_t, 32> response{};

        if (!SendPassthrough32(request, response))
        {
            return false;
        }

        if (response[0] != 0x11 || response[1] != 0xFF)
        {
            return false;
        }

        const uint16_t crc_rx =
            (static_cast<uint16_t>(response[30]) << 8) |
            static_cast<uint16_t>(response[31]);

        const uint16_t crc_calc = GenCrc16CcittFalse(response.data(), 30);
        if (crc_rx != crc_calc)
        {
            return false;
        }

        // Per manual page 24:
        // byte 12..15 = SDD_TEMP (float32)
        // byte 16     = RDY (bool)
        // byte 20..23 = HOT_SIDE (float32)
        m_SDDTemperature = static_cast<double>(ReadFloat32LE(&response[12]));
        m_TemperatureReady = (response[16] != 0);
        m_HotSideTemperature = static_cast<double>(ReadFloat32LE(&response[20]));
    }

    // 0x30: chip temperature readout -> current and target temperature
    {
        const auto request = BuildRequest(0x30);
        std::array<uint8_t, 32> response{};

        if (!SendPassthrough32(request, response))
        {
            return false;
        }

        if (response[0] != 0x30 || response[1] != 0xFF)
        {
            return false;
        }

        const uint16_t crc_rx =
            (static_cast<uint16_t>(response[30]) << 8) |
            static_cast<uint16_t>(response[31]);

        const uint16_t crc_calc = GenCrc16CcittFalse(response.data(), 30);
        if (crc_rx != crc_calc)
        {
            return false;
        }

        // Per manual page 26:
        // byte 2..5  = TTEMP_CUR (float32)
        // byte 6..9  = TTEMP_DEF (float32)
        // byte 10    = RDY (bool)
        //m_BoardTemperature = static_cast<double>(ReadFloat32LE(&response[2]));
        m_TargetTemperature = static_cast<double>(ReadFloat32LE(&response[6]));
        m_TemperatureReady = (response[10] != 0);
    }

    return true;
}

auto Ketek::SetError(std::string message) -> bool
{
    m_LastError = std::move(message);
    return false;
}

auto Ketek::SendPassthrough32(const std::array<uint8_t, 32>& send, std::array<uint8_t, 32>& receive) -> bool
{
    int sendLen =
        static_cast<int>(send.size());

    int receiveLen =
        static_cast<int>(receive.size());

    void* value[4] =
    {
        const_cast<uint8_t*>(send.data()),
        &sendLen,
        receive.data(),
        &receiveLen
    };

    const int status = xiaBoardOperation(
        m_Configuration.channel,
        const_cast<char*>("passthrough"),
        value
    );

    if (status != XIA_SUCCESS)
        return false;

    return receiveLen ==
        static_cast<int>(receive.size());
}

auto Ketek::LoadPeakingTimes() -> bool
{
    int status = xiaBoardOperation(
        m_Configuration.channel,
        const_cast<char*>("get_number_pt_per_fippi"),
        &m_NumberPeakingTimes
    );

    if (status != XIA_SUCCESS)
    {
        return SetError(
            "get_number_pt_per_fippi failed; Handel status = " +
            std::to_string(status)
        );
    }

    if (m_NumberPeakingTimes == 0)
    {
        return SetError(
            "KETEK reported zero available peaking times."
        );
    }

    m_PeakingTimes =
        std::make_unique<double[]>(m_NumberPeakingTimes);

    status = xiaBoardOperation(
        m_Configuration.channel,
        const_cast<char*>("get_current_peaking_times"),
        m_PeakingTimes.get()
    );

    if (status != XIA_SUCCESS)
    {
        return SetError(
            "get_current_peaking_times failed; Handel status = " +
            std::to_string(status)
        );
    }

    return true;
}

auto Ketek::FindParsetForPeakingTime(double peakingTime, unsigned short& parset) const -> bool
{
    if (!m_PeakingTimes || m_NumberPeakingTimes == 0)
        return false;

    constexpr double epsilon = 1e-9;

    for (unsigned short i = 0;
        i < m_NumberPeakingTimes;
        ++i)
    {
        if (std::abs(m_PeakingTimes[i] - peakingTime) <= epsilon)
        {
            parset = i;
            return true;
        }
    }

    return false;
}

auto Ketek::LoadConfiguration(const std::filesystem::path& configurationFilePath, Configuration& configuration) -> bool
{
    std::ifstream input(configurationFilePath);
    if (!input.is_open())
    {
        return SetError(
            "Cannot open KETEK configuration file: " +
            configurationFilePath.string()
        );
    }

    nlohmann::json root;

    try
    {
        input >> root;
    }
    catch (const nlohmann::json::exception& exception)
    {
        return SetError(
            "Invalid JSON in KETEK configuration file '" +
            configurationFilePath.string() +
            "': " +
            exception.what()
        );
    }

    if (!root.is_object())
    {
        return SetError("KETEK configuration root must be a JSON object.");
    }

    configuration = Configuration{};
    configuration.configurationFilePath = configurationFilePath;

    try
    {
        configuration.schemaVersion =
            root.value("schema_version", 1);

        configuration.handelIniPath =
            root.value("handel_ini_path", std::string("KETEK.ini"));

        if (root.contains("log"))
        {
            const auto& log = root.at("log");

            configuration.logFilePath =
                log.value("file", std::string("handel.log"));

            const std::string level =
                log.value("level", std::string("debug"));

            if (level == "error")
                configuration.logLevel = MD_ERROR;
            else if (level == "warning")
                configuration.logLevel = MD_WARNING;
            else if (level == "info")
                configuration.logLevel = MD_INFO;
            else if (level == "debug")
                configuration.logLevel = MD_DEBUG;
            else
                return SetError("Unsupported KETEK log level: " + level);
        }

        if (root.contains("device"))
        {
            const auto& device = root.at("device");

            configuration.channel =
                device.value("channel", 0);

            configuration.expectedSerialNumber =
                device.value(
                    "expected_serial_number",
                    std::string{}
                );
        }

        if (root.contains("calibration"))
        {
            const auto& calibration = root.at("calibration");

            configuration.dynamicRangeKeV =
                calibration.value("dynamic_range_keV", 12.0);
        }

        if (!root.contains("apply") || !root.at("apply").is_object())
        {
            return SetError(
                "KETEK configuration must contain an 'apply' object."
            );
        }

        const auto& apply = root.at("apply");

        configuration.persistToDevice =
            apply.value("persist_to_device", false);

        if (!apply.contains("acquisition_values") ||
            !apply.at("acquisition_values").is_array())
        {
            return SetError(
                "'apply.acquisition_values' must be an array."
            );
        }

        for (const auto& item : apply.at("acquisition_values"))
        {
            if (!item.is_object())
            {
                return SetError(
                    "Each acquisition value must be an object."
                );
            }

            AcquisitionValue value;

            value.name = item.at("name").get<std::string>();
            value.value = item.at("value").get<double>();
            value.required = item.value("required", true);

            configuration.acquisitionValues.push_back(
                std::move(value)
            );
        }
    }
    catch (const nlohmann::json::exception& exception)
    {
        return SetError(
            "Invalid KETEK configuration structure: " +
            std::string(exception.what())
        );
    }

    // Resolve relative paths against the JSON file directory.
    const auto baseDirectory =
        configurationFilePath.parent_path();

    if (configuration.handelIniPath.is_relative())
    {
        configuration.handelIniPath =
            baseDirectory / configuration.handelIniPath;
    }

    if (configuration.logFilePath.is_relative())
    {
        configuration.logFilePath =
            baseDirectory / configuration.logFilePath;
    }

    return true;
}

auto Ketek::ValidateConfiguration(const Configuration& configuration) -> bool
{
    if (configuration.schemaVersion != 1)
    {
        return SetError(
            "Unsupported KETEK configuration schema version: " +
            std::to_string(configuration.schemaVersion)
        );
    }

    if (configuration.channel < 0)
    {
        return SetError("KETEK channel cannot be negative.");
    }

    if (!std::filesystem::exists(configuration.handelIniPath))
    {
        return SetError(
            "Handel initialization file does not exist: " +
            configuration.handelIniPath.string()
        );
    }

    if (!std::isfinite(configuration.dynamicRangeKeV) ||
        configuration.dynamicRangeKeV <= 0.0)
    {
        return SetError(
            "dynamic_range_keV must be a finite positive number."
        );
    }

    if (configuration.acquisitionValues.empty())
    {
        return SetError(
            "No KETEK acquisition values were configured."
        );
    }

    for (const auto& acquisitionValue :
        configuration.acquisitionValues)
    {
        if (acquisitionValue.name.empty())
        {
            return SetError(
                "An acquisition value has an empty name."
            );
        }

        if (!std::isfinite(acquisitionValue.value))
        {
            return SetError(
                "Acquisition value '" +
                acquisitionValue.name +
                "' is not finite."
            );
        }
    }

    auto findValue =
        [&configuration](const std::string& name)
        -> std::optional<double>
        {
            const auto it = std::find_if(
                configuration.acquisitionValues.begin(),
                configuration.acquisitionValues.end(),
                [&name](const AcquisitionValue& value)
                {
                    if (value.required) return value.name == name;
                }
            );

            if (it == configuration.acquisitionValues.end())
                return std::nullopt;

            return it->value;
        };

    const auto numberOfBins =
        findValue("number_mca_channels");

    const auto binWidth =
        findValue("mca_bin_width");

    if (numberOfBins)
    {
        if (*numberOfBins < 1.0 ||
            *numberOfBins > MaxMcaChannels ||
            std::floor(*numberOfBins) != *numberOfBins)
        {
            return SetError(
                "number_mca_channels must be an integer in [1, 8192]."
            );
        }
    }

    if (binWidth)
    {
        if (*binWidth < 1.0 ||
            std::floor(*binWidth) != *binWidth)
        {
            return SetError(
                "mca_bin_width must be a positive integer."
            );
        }
    }

    if (numberOfBins &&
        binWidth &&
        (*numberOfBins * *binWidth > MaxMcaChannels))
    {
        return SetError(
            "number_mca_channels * mca_bin_width exceeds 8192. "
            "This would create an unusable high-energy region."
        );
    }

    const auto peakingTime =
        findValue("peaking_time");

    const auto parset =
        findValue("parset");

    if (peakingTime && parset)
    {
        return SetError(
            "Configure either peaking_time or parset, not both."
        );
    }

    return true;
}

auto Ketek::ApplyAcquisitionConfiguration(const Configuration& configuration) -> bool
{
    //
    // 1. Select PARSET first.
    //
    const auto peakingTimeIt =
        std::find_if(
            configuration.acquisitionValues.begin(),
            configuration.acquisitionValues.end(),
            [](const AcquisitionValue& value)
            {
                return value.name == "peaking_time";
            }
        );

    if (peakingTimeIt != configuration.acquisitionValues.end())
    {
        unsigned short parset{};

        if (!FindParsetForPeakingTime(
            peakingTimeIt->value,
            parset))
        {
            return SetError(
                "No PARSET corresponds to requested peaking time " +
                std::to_string(peakingTimeIt->value) +
                " us."
            );
        }

        double parsetValue =
            static_cast<double>(parset);

        const int status = xiaSetAcquisitionValues(
            configuration.channel,
            const_cast<char*>("parset"),
            &parsetValue
        );

        if (status != XIA_SUCCESS)
        {
            return SetError(
                "Failed to select PARSET " +
                std::to_string(parset) +
                " for peaking time " +
                std::to_string(peakingTimeIt->value) +
                " us; Handel status = " +
                std::to_string(status)
            );
        }
    }

    //
    // 2. Apply all other requested acquisition values.
    //
    for (const auto& acquisitionValue :
        configuration.acquisitionValues)
    {
        if (acquisitionValue.name == "peaking_time")
            continue;

        // PARSET was derived from peaking_time above.
        // Do not allow another generic "parset" entry to override it.
        if (acquisitionValue.name == "parset")
            continue;

        if (!SetAcquisitionValue(acquisitionValue))
            return false;
    }

    //
    // 3. Apply DSP parameter changes to hardware.
    //
    const int status = xiaBoardOperation(
        configuration.channel,
        const_cast<char*>("apply"),
        &m_Ignored
    );

    if (status != XIA_SUCCESS)
    {
        return SetError(
            "KETEK apply operation failed; Handel status = " +
            std::to_string(status)
        );
    }

    return true;
}

auto Ketek::ReadBackAcquisitionConfiguration() -> bool
{
    if (!TryReadAcquisitionValue(
        "number_mca_channels",
        m_nMCA))
    {
        return SetError(
            "Could not read back number_mca_channels."
        );
    }

    if (!TryReadAcquisitionValue(
        "mca_bin_width",
        m_BinWidth))
    {
        return SetError(
            "Could not read back mca_bin_width."
        );
    }

    if (!TryReadAcquisitionValue(
        "trigger_threshold",
        m_TriggerThreshold))
    {
        return SetError(
            "Could not read back trigger_threshold."
        );
    }

    if (!TryReadAcquisitionValue("gain", m_Gain))
    {
        return SetError("Could not read back gain.");
    }

    // Optional readbacks. Their availability depends on the driver.
    TryReadAcquisitionValue(
        "polarity",
        m_Polarity
    );

    TryReadAcquisitionValue(
        "peaking_time",
        m_PeakingTime
    );

    TryReadAcquisitionValue(
        "baseline_average",
        m_BaselineAverageLength
    );

    TryReadAcquisitionValue(
        "fine_gain",
        m_FineGainTrim
    );

    TryReadAcquisitionValue(
        "baseline_threshold",
        m_BaselineThreshold
    );

    TryReadAcquisitionValue(
        "energy_threshold",
        m_EnergyThreshold
    );

    if (!TryReadAcquisitionValue(
        "genset",
        m_CurrentGENSET))
    {
        return SetError("Could not read back genset.");
    }

    if (!TryReadAcquisitionValue(
        "parset",
        m_CurrentPARSET))
    {
        return SetError("Could not read back parset.");
    }

    m_GENSET =
        static_cast<unsigned short>(m_CurrentGENSET);

    m_PARSET =
        static_cast<unsigned short>(m_CurrentPARSET);

    return UpdateDerivedValues();
}

auto Ketek::SetAcquisitionValue(const AcquisitionValue& acquisitionValue) -> bool
{
    if (!acquisitionValue.required) return true;

    double value = acquisitionValue.value;

    const int status = xiaSetAcquisitionValues(
        m_Configuration.channel,
        const_cast<char*>(acquisitionValue.name.c_str()),
        &value
    );

    if (status == XIA_SUCCESS)
        return true;

    std::ostringstream message;
    message
        << "xiaSetAcquisitionValues failed for '"
        << acquisitionValue.name
        << "' with value "
        << acquisitionValue.value
        << "; Handel status = "
        << status;

    if (const char* errorText = xiaGetErrorText(status))
    {
        message << " (" << errorText << ')';
    }

    if (acquisitionValue.required)
        return SetError(message.str());

    // Optional settings are recorded in Handel's log and skipped.
    // Replace this with your application's logging mechanism if needed.
    return true;
}

auto Ketek::TryReadAcquisitionValue(const char* name, double& destination) -> bool
{
    double value{};

    const int status = xiaGetAcquisitionValues(
        m_Configuration.channel,
        const_cast<char*>(name),
        &value
    );

    if (status != XIA_SUCCESS)
        return false;

    if (!std::isfinite(value))
        return false;

    destination = value;
    return true;
}

auto Ketek::UpdateDerivedValues() -> bool
{
    if (!std::isfinite(m_nMCA) ||
        !std::isfinite(m_BinWidth) ||
        !std::isfinite(m_DynamicRangeKeV))
    {
        return SetError(
            "Cannot calculate bin size from non-finite values."
        );
    }

    if (m_nMCA <= 0.0 ||
        m_BinWidth <= 0.0 ||
        m_DynamicRangeKeV <= 0.0)
    {
        return SetError(
            "Cannot calculate bin size from non-positive values."
        );
    }

    if (m_nMCA * m_BinWidth > MaxMcaChannels)
    {
        return SetError(
            "Active MCA configuration exceeds the supported "
            "number-of-bins/bin-width product."
        );
    }

    // XIA equation:
    // eV/bin = Dynamic Range [keV] * MCA Bin Width / 8000
    //
    // Dividing by 8000 directly yields keV/bin:
    m_BinSizeKeV =
        (m_DynamicRangeKeV * m_BinWidth) / 8000.0;

    return true;
}
