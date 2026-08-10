#pragma once
#ifndef KETEK_H
#define KETEK_H

#include "handel.h"
#include "handel_errors.h"
#include "md_generic.h"

#include <array>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <nlohmann/json.hpp>

class Ketek
{
public:
    struct AcquisitionValue
    {
        std::string name;
        double value{};
        bool required{ true };
    };

    struct Configuration
    {
        int schemaVersion{ 1 };

        std::filesystem::path configurationFilePath;
        std::filesystem::path handelIniPath{ "KETEK.ini" };
        std::filesystem::path logFilePath{ "handel.log" };

        int logLevel{ MD_DEBUG };
        int channel{ 0 };

        std::string expectedSerialNumber;

        double dynamicRangeKeV{ 12.0 };

        bool persistToDevice{ false };

        std::vector<AcquisitionValue> acquisitionValues;
    };

public:
    Ketek(
        const std::string& deviceSN,
        const std::filesystem::path& configurationFilePath
    );

    auto InitializeDevice(
        const std::string& deviceSN,
        const std::filesystem::path& configurationFilePath
    ) -> bool;

    auto IsDeviceInitialized() const -> bool
    {
        return m_HandelInitialized &&
            !m_DeviceSerialNumber.empty();
    }

    auto CaptureData(
        int exposure,
        unsigned long* mca,
        bool* continueCapturing
    ) -> bool;

    auto DeInitializeDevice() -> bool;

    ~Ketek();

    // Actual configuration getters
    auto GetDataSize() const -> unsigned long
    {
        return static_cast<unsigned long>(m_nMCA);
    }

    // keV per MCA bin
    auto GetBinSize() const -> double
    {
        return m_BinSizeKeV;
    }

    auto GetBinSizeEV() const -> double
    {
        return m_BinSizeKeV * 1000.0;
    }

    auto GetBinWidth() const -> double
    {
        return m_BinWidth;
    }

    auto GetDynamicRangeKeV() const -> double
    {
        return m_DynamicRangeKeV;
    }

    auto GetPolarity() const -> double
    {
        return m_Polarity;
    }

    auto GetThreshold() const -> double
    {
        return m_TriggerThreshold;
    }

    auto GetSerialNumber() const -> std::string
    {
        return m_DeviceSerialNumber;
    }

    auto GetGain() const -> double
    {
        return m_Gain;
    }

    auto GetPeakingTime() const -> double
    {
        return m_PeakingTime;
    }

    auto GetBaselineAverageLength() const -> double
    {
        return m_BaselineAverageLength;
    }

    auto GetFineGainTrim() const -> double
    {
        return m_FineGainTrim;
    }

    auto GetBaselineThreshold() const -> double
    {
        return m_BaselineThreshold;
    }

    auto GetEnergyThreshold() const -> double
    {
        return m_EnergyThreshold;
    }

    auto GetGENSET() const -> unsigned short
    {
        return m_GENSET;
    }

    auto GetPARSET() const -> unsigned short
    {
        return m_PARSET;
    }

    auto GetUSBVersion() const -> unsigned long
    {
        return m_USBVersion;
    }

    auto GetBoardTemperature() const -> double
    {
        return m_BoardTemperature;
    }

    auto GetSDDTemperature() const -> double
    {
        return m_SDDTemperature;
    }

    auto GetHotSideTemperature() const -> double
    {
        return m_HotSideTemperature;
    }

    auto GetThermistor1Temperature() const -> double
    {
        return m_Thermistor1Temperature;
    }

    auto GetThermistor2Temperature() const -> double
    {
        return m_Thermistor2Temperature;
    }

    auto GetTargetTemperature() const -> double
    {
        return m_TargetTemperature;
    }

    auto IsTemperatureReady() const -> bool
    {
        return m_TemperatureReady;
    }

    auto GetLastError() const -> const std::string&
    {
        return m_LastError;
    }

private:
    static auto CHECK_ERROR(int status) -> bool
    {
        return status == XIA_SUCCESS;
    }

    auto LoadConfiguration
    (
        const std::filesystem::path& configurationFilePath,
        Configuration& configuration
    ) -> bool;

    auto ValidateConfiguration
    (
        const Configuration& configuration
    ) -> bool;

    auto ApplyAcquisitionConfiguration
    (
        const Configuration& configuration
    ) -> bool;

    auto ReadBackAcquisitionConfiguration() -> bool;

    auto SetAcquisitionValue
    (
        const AcquisitionValue& acquisitionValue
    ) -> bool;

    auto TryReadAcquisitionValue
    (
        const char* name,
        double& destination
    ) -> bool;

    auto UpdateDerivedValues() -> bool;

    auto RequestTemperature() -> bool;

    auto SetError(std::string message) -> bool;

    auto SendPassthrough32
    (
        const std::array<uint8_t, 32>& send,
        std::array<uint8_t, 32>& receive
    ) -> bool;

    auto LoadPeakingTimes() -> bool;

    auto FindParsetForPeakingTime
    (
        double peakingTime,
        unsigned short& parset
    ) const -> bool;

private:
    Configuration m_Configuration{};

    std::string m_DeviceSerialNumber{};
    std::string m_LastError{};

    static constexpr double MaxMcaChannels = 8192.0;

    double m_nMCA{ 8192.0 };
    double m_BinWidth{ 1.0 };

    // Calculated value, not independently configured.
    double m_BinSizeKeV{ 0.0015 };
    double m_DynamicRangeKeV{ 12.0 };

    double m_TriggerThreshold{ 30.0 };
    double m_BaselineThreshold{ 0.0 };
    double m_EnergyThreshold{ 0.0 };

    double m_Polarity{ 1.0 };
    double m_Gain{ 15.675 };
    double m_FineGainTrim{ 1.321 };
    double m_PeakingTime{ 0.5 };
    double m_BaselineAverageLength{ 512.0 };

    unsigned short m_Ignored{ 0 };

    double m_BoardTemperature{};
    double m_SDDTemperature{};
    double m_HotSideTemperature{};
    double m_Thermistor1Temperature{};
    double m_Thermistor2Temperature{};
    double m_TargetTemperature{};

    bool m_TemperatureReady{ false };

    double m_CurrentGENSET{};
    double m_CurrentPARSET{};

    unsigned short m_GENSET{};
    unsigned short m_PARSET{};

    unsigned long m_USBVersion{};

    unsigned short m_NumberPeakingTimes{};
    std::unique_ptr<double[]> m_PeakingTimes{};

    unsigned short m_NumberFippis{};

    bool m_HandelInitialized{ false };
};

#endif