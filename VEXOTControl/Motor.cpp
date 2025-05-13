#include "Motor.h"
/* Motor */

Motor::Motor()
{
	m_MotorSettings = std::make_unique<MotorVariables::Settings>();
	m_StandaSettings = std::make_unique<StandaVariables::C_Settings>();
}

auto Motor::SetDeviceName(const char* device_name) -> void
{
	if (!device_name) return;
	std::string name(device_name);
	m_DeviceName = std::make_unique<char[]>(name.size() + 1);
	std::memcpy(m_DeviceName.get(), name.c_str(), name.size() + 1);
}

auto Motor::SetRange(const float min_motor_deg, const float max_motor_deg) -> void
{
	const float motor_range = max_motor_deg - min_motor_deg;
	const float ratio = m_MotorSettings->stepsPerMMRatio;

	// Motor coordinates
	m_MotorSettings->minMotorPos = min_motor_deg;
	m_MotorSettings->middleMotorPos = motor_range / 2.f;
	m_MotorSettings->maxMotorPos = max_motor_deg;
	m_MotorSettings->motorRange = motor_range;

	// Stage coordinates
	m_MotorSettings->minStagePos = min_motor_deg / ratio;
	m_MotorSettings->middleStagePos = m_MotorSettings->middleMotorPos / ratio;
	m_MotorSettings->maxStagePos = max_motor_deg / ratio;
	m_MotorSettings->stageRange = motor_range / ratio;
}

// Refactored GoCenter
auto Motor::GoCenter() -> bool
{
	DeviceHandle device(m_DeviceName.get());
	if (!device.isValid()) return false;

	if (!Check(command_move_calb(device, m_MotorSettings->middleMotorPos, &m_StandaSettings->calibration))) return false;
	if (!Check(command_wait_for_stop(device, 100))) return false;
	std::this_thread::sleep_for(std::chrono::milliseconds(wait_delay_milliseconds));
	if (!UpdateStatusAndCalibration(device)) return false;

	UpdateCurrentPosition();
	return true;
}

// Refactored GoHomeAndZero
auto Motor::GoHomeAndZero() -> bool
{
	DeviceHandle device(m_DeviceName.get());
	if (!device.isValid()) return false;

	if (!Check(command_homezero(device))) return false;
	if (!Check(command_wait_for_stop(device, 100))) return false;
	std::this_thread::sleep_for(std::chrono::milliseconds(wait_delay_milliseconds));
	if (!UpdateStatusAndCalibration(device)) return false;

	UpdateCurrentPosition();
	return true;
}

// Refactored GoToPos
auto Motor::GoToPos(const float stage_position) -> bool
{
	DeviceHandle device(m_DeviceName.get());
	if (!device.isValid()) return false;

	if (!Check(get_status_calb(device, &m_StandaSettings->calb_state, &m_StandaSettings->calibration))) return false;
	if (stage_position < m_MotorSettings->minStagePos || stage_position > m_MotorSettings->maxStagePos) return false;

	float motor_position = stage_position * m_MotorSettings->stepsPerMMRatio;
	if (!Check(command_move_calb(device, motor_position, &m_StandaSettings->calibration))) return false;
	if (!Check(command_wait_for_stop(device, 100))) return false;
	std::this_thread::sleep_for(std::chrono::milliseconds(wait_delay_milliseconds));
	if (!UpdateStatusAndCalibration(device)) return false;

	UpdateCurrentPosition();
	return true;
}

/* MotorArray */
MotorArray::MotorArray(const std::string ipAddress)
{
	InitAllMotors(ipAddress.c_str());
}

auto MotorArray::FillNames() -> void
{
	m_NamesOfMotorsWithRanges.clear();  // Prevent accumulation on repeated calls

	for (const auto& motor : m_MotorsArray)
	{
		const auto& serial = motor.GetDeviceSerNum();
		const auto& range = motor.GetDeviceRange();

		m_NamesOfMotorsWithRanges.emplace(serial, range);
	}
}

float MotorArray::GetActualStagePos(const std::string& motor_sn) const
{
	if (const Motor* motor = FindMotorBySerial(motor_sn))
	{
		return motor->GetDeviceActualStagePos();
	}
	return error_position;
}

auto MotorArray::MotorHasSerialNumber(const std::string& motor_sn) const -> bool
{
	return FindMotorBySerial(motor_sn) != nullptr;
}

float MotorArray::GoMotorHome(const std::string& motor_sn)
{
	if (Motor* motor = FindMotorBySerial(motor_sn))
	{
		motor->GoHomeAndZero();
		return motor->GetDeviceActualStagePos();
	}
	return error_position;
}

float MotorArray::GoMotorCenter(const std::string& motor_sn)
{
	if (Motor* motor = FindMotorBySerial(motor_sn))
	{
		motor->GoCenter();
		return motor->GetDeviceActualStagePos();
	}
	return error_position;
}

float MotorArray::GoMotorToAbsPos(const std::string& motor_sn, float abs_pos)
{
	if (auto* motor = FindMotorBySerial(motor_sn))
	{
		motor->GoToPos(abs_pos);
		return motor->GetDeviceActualStagePos();
	}
	return error_position;
}

float MotorArray::GoMotorOffset(const std::string& motor_sn, float offset)
{
	if (Motor* motor = FindMotorBySerial(motor_sn))
	{
		float current_pos = motor->GetDeviceActualStagePos();
		float new_pos = current_pos + offset;

		// Clamp check within valid range
		auto range_max = motor->GetDeviceRange();
		decltype(range_max) range_min{};

		if (new_pos < range_min || new_pos > range_max)
			return current_pos; // Out of range: return current position without moving

		motor->GoToPos(new_pos);
		return motor->GetDeviceActualStagePos();
	}

	return error_position;
}

auto MotorArray::SetStepsPerMMForTheMotor(const std::string& motor_sn, int stepsPerMM) -> void
{
	if (stepsPerMM <= 0) return;

	if (Motor* motor = FindMotorBySerial(motor_sn))
		motor->SetStepsPerMMRatio(stepsPerMM);
}

auto MotorArray::InitAllMotors(const std::string ip_address) -> bool
{
	auto appendUnitializedMotor = [&](const unsigned int motorSN, const int motorNum) 
		{
			m_UninitializedMotors.push_back(motorSN);
			m_MotorsArray[motorNum].SetSerNum(0);
		};

	m_UninitializedMotors.clear();

	const char* correction_table = "table.txt";
	// Checking whether table.txt is inside the working directory
	{
		auto currPath = std::filesystem::current_path();
		if (!std::filesystem::exists(currPath / correction_table)) return false;
	}

	result_t result_c;
	result_c = set_bindy_key("keyfile.sqlite");
	if (result_c != result_ok) return false;

	device_enumeration_t devenum_c;

#ifdef _DEBUG
	const int probe_flags = ENUMERATE_PROBE;
	const char* enumerate_hints = "addr=";
	devenum_c = enumerate_devices(probe_flags, enumerate_hints);
#else
	const int probe_flags = ENUMERATE_PROBE | ENUMERATE_NETWORK;
	std::string eh = std::string("addr=") + ip_address;
	const char* enumerate_hints = eh.c_str();
	//const char* enumerate_hints = "addr=10.0.0.134";
	devenum_c = enumerate_devices(probe_flags, enumerate_hints);
#endif // _DEBUG
	if (!devenum_c) return false;

	int names_count = get_device_count(devenum_c);

	/* Here we need to clear motor list */
	m_MotorsArray.clear();
	m_MotorsArray.reserve(names_count);

	char device_name[256];
	device_t device_c;
	status_t state_c;
	status_calb_t state_calb_c;
	emf_settings_t emfSettings{};
	calibration_t calibration_c;
	stage_settings_t stage_settings_c{};
	edges_settings_calb_t edges_settings_calb_c;
	stage_information_t stage_information_c{};
	unsigned int device_sn{};
	for (int i = 0; i < names_count; ++i)
	{
		m_MotorsArray.emplace_back(Motor());
		m_MotorsArray[i].SetResult(result_c);

		strcpy(device_name, get_device_name(devenum_c, i));
		m_MotorsArray[i].SetDeviceName(device_name);
		device_c = open_device(device_name);
		get_serial_number(device_c, &device_sn);
		m_MotorsArray[i].SetSerNum(device_sn);

		if ((result_c = get_status(device_c, &state_c)) != result_ok)
		{
			appendUnitializedMotor(device_sn, i);
			continue;
		}

		// The device_t device parameter in this function is a C pointer, unlike most library functions that use this parameter
		if ((result_c = set_correction_table(device_c, correction_table)) != result_ok)
		{
			appendUnitializedMotor(device_sn, i);
			continue;
		}

		calibration_c.A = 1;
		calibration_c.MicrostepMode = MICROSTEP_MODE_FULL;
		//calibration_c.MicrostepMode = MICROSTEP_MODE_FRAC_256;

		m_MotorsArray[i].SetCalibration(calibration_c);

		/* Get Status */
		if ((result_c = get_status_calb(device_c, &state_calb_c, &calibration_c)) != result_ok)
		{
			appendUnitializedMotor(device_sn, i);
			continue;
		}
		m_MotorsArray[i].SetState(state_c);

		get_edges_settings_calb(device_c, &edges_settings_calb_c, &calibration_c);
		m_MotorsArray[i].SetRange(edges_settings_calb_c.LeftBorder, edges_settings_calb_c.RightBorder);

		m_MotorsArray[i].UpdateCurrentPosition();

		close_device(&device_c);
	}
	
	std::sort(m_MotorsArray.begin(), m_MotorsArray.end(), [](Motor& left, Motor& right)
		{
			return (left.GetDeviceSerNum() < right.GetDeviceSerNum());
		});

	free_enumerate_devices(devenum_c);
	FillNames();

	return true;
}
