#pragma once
#ifndef MOTOR_H
#define MOTOR_H
#include <memory>
#include <algorithm>
#include <map>
#include <vector>
#include <thread>
#include <chrono>
#include <string>
#include <filesystem>
#include <mutex>
#include <ximc.h>

namespace MotorVariables
{
	struct Settings
	{
		float motorPos{};
		float minMotorPos{}, middleMotorPos{}, maxMotorPos{};
		float stagePos{};
		float minStagePos{}, middleStagePos{}, maxStagePos{};
		float motorRange{}, stageRange{};
		float stepsPerMMRatio{ 800.f }; 
	};
}

namespace StandaVariables
{
	struct C_Settings
	{
		result_t result{};
		calibration_t calibration{};
		status_t state{};
		status_calb_t calb_state{};
	};
}

class Motor;

// Helper RAII wrapper for device management. If an owner Motor is given,
// also registers/clears the open handle on that Motor for the duration -
// see Motor::SetActiveDevice - so Stop() (likely called from another
// thread) can find and interrupt it, even on an early-return exit path.
// Constructor/destructor are defined in Motor.cpp rather than inline here,
// since Motor is only forward-declared at this point in the header - an
// inline body here would try to call a member of an incomplete type.
class DeviceHandle 
{
public:
	explicit DeviceHandle(const char* name, Motor* owner = nullptr);
	~DeviceHandle();

	operator device_t() const { return device; }
	bool isValid() const { return device >= 0; }

	DeviceHandle(const DeviceHandle&) = delete;
	DeviceHandle& operator=(const DeviceHandle&) = delete;

private:
	device_t device;
	Motor* m_Owner{ nullptr };
};


class Motor final
{
public:
	Motor();

	/* Getters */
	auto GetDeviceSerNum() const -> unsigned int {return m_SerNum; };
	
	auto GetDeviceRange() const -> float { return m_MotorSettings->stageRange; }

	auto GetDeviceActualStagePos() const -> float { return m_MotorSettings->stagePos; }

	/* Setters */
	auto SetDeviceName(const char* device_name) -> void;

	auto SetSerNum(unsigned int s_n) -> void { m_SerNum = s_n; };

	auto SetResult(result_t result) -> void { m_StandaSettings->result = result; }

	auto SetCalibration(calibration_t calibration) -> void { m_StandaSettings->calibration = calibration; }

	auto SetState(status_t state) -> void { m_StandaSettings->state = state; }

	auto SetCalbState(status_calb_t calb_state) -> void { m_StandaSettings->calb_state = calb_state; }

	auto SetRange(const float min_motor_deg, const float max_motor_deg) -> void;

	auto SetStepsPerMMRatio(const int stepsPerMMRatio) -> void 
	{ 
		m_MotorSettings->stepsPerMMRatio = static_cast<float>(stepsPerMMRatio); 
		UpdateStageRange(); 
		UpdateCurrentPosition(); 
	};

	auto UpdateCurrentPosition() -> void
	{
		m_MotorSettings->motorPos = static_cast<float>(m_StandaSettings->state.CurPosition);
		m_MotorSettings->stagePos = m_MotorSettings->motorPos / m_MotorSettings->stepsPerMMRatio;
	}

	auto GoCenter() -> bool;

	auto GoHomeAndZero() -> bool;

	auto GoToPos(const float stage_position) -> bool;

	// Interrupts whatever move is currently in progress on this motor, if
	// any. Safe to call from a different thread than the one performing
	// the move - it sends command_stop() on the same open device handle
	// that move is using (tracked via DeviceHandle/SetActiveDevice).
	auto Stop() -> bool;

	// Records/clears the device handle currently in use by a move, so
	// Stop() (likely called from another thread) knows what to send
	// command_stop to. Thread-safe. Used internally by DeviceHandle.
	auto SetActiveDevice(device_t dev) -> void
	{
		if (!m_DeviceMutex) return;
		std::lock_guard<std::mutex> lock(*m_DeviceMutex);
		m_ActiveDevice = dev;
	}

	/* Move constructor */
	Motor(Motor&& other) noexcept 
		: m_MotorSettings(std::move(other.m_MotorSettings)), 
		m_StandaSettings(std::move(other.m_StandaSettings)), 
		m_DeviceName(std::move(other.m_DeviceName)),
		m_SerNum(other.m_SerNum),
		m_DeviceMutex(std::move(other.m_DeviceMutex)),
		m_ActiveDevice(other.m_ActiveDevice)
	{
		other.m_MotorSettings = nullptr;
		other.m_StandaSettings = nullptr;
		other.m_DeviceName = nullptr;
		other.m_SerNum = 0;
		other.m_ActiveDevice = device_undefined;
	};

	/* Move assignment */
	auto& operator=(Motor&& other) noexcept
	{
		m_MotorSettings.reset(other.m_MotorSettings.release());
		m_StandaSettings.reset(other.m_StandaSettings.release());
		m_DeviceName.reset(other.m_DeviceName.release());
		m_SerNum = other.m_SerNum;
		m_DeviceMutex = std::move(other.m_DeviceMutex);
		m_ActiveDevice = other.m_ActiveDevice;
		other.m_ActiveDevice = device_undefined;
		return *this;
	};

private:
	// Helper function to check command result
	inline bool Check(result_t result)
	{
		return result == result_ok;
	}

	// Helper function to update status and calibration
	bool UpdateStatusAndCalibration(DeviceHandle& device)
	{
		if (!Check(get_status(device, &m_StandaSettings->state))) return false;
		if (!Check(get_status_calb(device, &m_StandaSettings->calb_state, &m_StandaSettings->calibration))) return false;
		return true;
	}

	auto UpdateStageRange() -> void 
	{
		/* Min position */
		m_MotorSettings->minStagePos = m_MotorSettings->minMotorPos / m_MotorSettings->stepsPerMMRatio;

		/* Middle position */
		m_MotorSettings->middleStagePos = m_MotorSettings->middleMotorPos / m_MotorSettings->stepsPerMMRatio;

		/* Max position */
		m_MotorSettings->maxStagePos = m_MotorSettings->maxMotorPos / m_MotorSettings->stepsPerMMRatio;

		/* Set Whole Motor Range */
		m_MotorSettings->stageRange = m_MotorSettings->motorRange / m_MotorSettings->stepsPerMMRatio;
	};


private:
	std::unique_ptr<MotorVariables::Settings> m_MotorSettings{};
	std::unique_ptr<StandaVariables::C_Settings> m_StandaSettings{};
	//int m_StepsPerMM{ 800 }; 
	std::unique_ptr<char[]> m_DeviceName{};
	unsigned int m_SerNum{};
	const long long wait_delay_milliseconds{ 500 };

	// Heap-allocated so Motor stays movable (std::mutex itself is not).
	// Guards m_ActiveDevice, which Stop() reads from a different thread
	// than the one currently blocked in command_wait_for_stop.
	std::unique_ptr<std::mutex> m_DeviceMutex{ std::make_unique<std::mutex>() };
	device_t m_ActiveDevice{ device_undefined };
};

class MotorArray final
{
public:
	MotorArray(const std::string ipAddress);

	auto InitAllMotors(const std::string ip_address) -> bool;

	auto FillNames() -> void;

	/* Getter */
	auto GetNamesWithRanges() const -> std::map<unsigned int, float> { return m_NamesOfMotorsWithRanges; }

	float GetActualStagePos(const std::string& motor_sn) const;
	auto MotorHasSerialNumber(const std::string& motor_sn) const -> bool;

	/* Setter */
	float GoMotorHome(const std::string& motor_sn);
	float GoMotorCenter(const std::string& motor_sn);
	float GoMotorToAbsPos(const std::string& motor_sn, float abs_pos);
	float GoMotorOffset(const std::string& motor_sn, float offset);

	// Interrupts an in-progress move. Safe to call from the UI thread while
	// a move is running on a background thread - command_stop() is fast
	// and non-blocking.
	bool StopMotor(const std::string& motor_sn);
	bool StopAll();

	auto AreAllMotorsInitialized() const -> bool { return !m_UninitializedMotors.size(); };
	auto GetUninitializedMotors() const -> std::vector<unsigned int> { return m_UninitializedMotors; };

	auto SetStepsPerMMForTheMotor(const std::string& motor_sn, int stepsPerMM) -> void;

	Motor* FindMotorBySerial(const std::string& motor_sn);
	const Motor* FindMotorBySerial(const std::string& motor_sn) const;

private:
	std::vector<Motor> m_MotorsArray{};
	std::map<unsigned int, float> m_NamesOfMotorsWithRanges{};
	const float error_position = 0.0f;

	std::vector<unsigned int> m_UninitializedMotors{};
};

#endif
