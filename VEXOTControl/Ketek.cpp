#include "Ketek.h"

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

    auto SendPassthrough32(const std::array<uint8_t, 32>& send,
        std::array<uint8_t, 32>& receive) -> bool
    {
        int send_len = static_cast<int>(send.size());
        int receive_len = static_cast<int>(receive.size());

        void* value[4] =
        {
            const_cast<uint8_t*>(send.data()),
            &send_len,
            receive.data(),
            &receive_len
        };

        const int status = xiaBoardOperation(0, (char*)"passthrough", value);
        if (status != XIA_SUCCESS)
        {
            return false;
        }

        return receive_len == 32;
    }
}

auto Ketek::InitializeDevice(const std::string deviceSN) -> bool
{
    if (IsDeviceInitialized()) DeInitializeDevice();
    /* Setup logging here */
    // Configuring the Handel log file
    xiaSetLogLevel(MD_DEBUG);
    xiaSetLogOutput((char*)"handel.log");

    // Loading the .ini file
    auto status = xiaInit((char*)m_InitializationFilePath.c_str());
    if (!CHECK_ERROR(status)) return false;

    xiaSetIOPriority(MD_IO_PRI_HIGH);

    status = xiaStartSystem();
    if (!CHECK_ERROR(status)) return false;

    // Serial Number
    {
        char serialNumber[17];
        status = xiaBoardOperation(0, (char*)"get_serial_number", &serialNumber);
        serialNumber[16] = '\0';
        m_DeviceSerialNumber = std::string(serialNumber);

        status = xiaBoardOperation(0, (char*)"get_usb_version", &m_USBVersion);

        if (m_DeviceSerialNumber != deviceSN) return false;
    }

    /* Modify some acquisition values */
    status = xiaSetAcquisitionValues(0, (char*)"number_mca_channels", &m_nMCA);
    if (!CHECK_ERROR(status)) return false;

    status = xiaSetAcquisitionValues(0, (char*)"mca_bin_width", (void*)&m_BinWidth);
    if (!CHECK_ERROR(status)) return false;

    status = xiaSetAcquisitionValues(0, (char*)"trigger_threshold", &m_Thresh);
    if (!CHECK_ERROR(status)) return false;

    status = xiaSetAcquisitionValues(0, (char*)"polarity", &m_Polarity);
    if (!CHECK_ERROR(status)) return false;

    status = xiaSetAcquisitionValues(0, (char*)"gain", &m_Gain);
    if (!CHECK_ERROR(status)) return false;

    /* Apply changes to parameters */
    status = xiaBoardOperation(0, (char*)"apply", (void*)&m_Ignored);

    /* Save the settings to the current GENSET and PARSET */
    status = xiaGetAcquisitionValues(0, (char*)"genset", &m_CurrentGENSET);
    if (!CHECK_ERROR(status)) return false;

    status = xiaGetAcquisitionValues(0, (char*)"parset", &m_CurrentPARSET);
    if (!CHECK_ERROR(status)) return false;

    m_GENSET = (unsigned short)m_CurrentGENSET;
    m_PARSET = (unsigned short)m_CurrentPARSET;

    status = xiaBoardOperation(0, (char*)"save_genset", &m_GENSET);
    if (!CHECK_ERROR(status)) return false;

    status = xiaBoardOperation(0, (char*)"save_parset", &m_PARSET);
    if (!CHECK_ERROR(status)) return false;

    status = xiaBoardOperation(0, (char*)"get_temperature", &m_BoardTemperature);
    if (!CHECK_ERROR(status)) return false;

    status = xiaBoardOperation(0, (char*)"get_number_pt_per_fippi", &m_NumberPeakingTimes);
    if (!CHECK_ERROR(status)) return false;

    m_PeakingTimes = std::make_unique<double[]>(m_NumberPeakingTimes);

    status = xiaBoardOperation(0, (char*)"get_current_peaking_times", m_PeakingTimes.get());
    if (!CHECK_ERROR(status)) return false;

    /* Read out number of fippis to pre-allocate peaking time array */
    status = xiaBoardOperation(0, (char*)"get_number_of_fippis", &m_NumberFippis);
    if (!CHECK_ERROR(status)) return false;

    m_PeakingTimes = std::make_unique<double[]>(m_NumberPeakingTimes * m_NumberFippis);

    status = xiaBoardOperation(0, (char*)"get_peaking_times", m_PeakingTimes.get());
    if (!CHECK_ERROR(status)) return false;

    return RequestTemperature();
}

auto Ketek::CaptureData(const int exposure, unsigned long* const mca, bool * const continueCapturing) -> bool
{
    if (!IsDeviceInitialized()) return false;
    if (!mca) return false;

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
                xiaStopRun(0);
                return false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        } while (deltaTime < (double)exposureTime);
    }

    status = xiaStopRun(0);
    if (!CHECK_ERROR(status)) return false;

    /* Prepare to read out MCA spectrum */
    unsigned long mcaLen{};
    status = xiaGetRunData(0, (char*)"mca_length", &mcaLen);
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
    if (!IsDeviceInitialized()) return true;

	auto status = xiaExit();
    if (!CHECK_ERROR(status)) return false;
    xiaCloseLog();
    m_DeviceSerialNumber = "";

    return true;
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
