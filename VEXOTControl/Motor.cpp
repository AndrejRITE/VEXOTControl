#include "Motor.h"
/* Motor */

DeviceHandle::DeviceHandle(const char* name, Motor* owner)
	: device(open_device(name)), m_Owner(owner)
{
	if (m_Owner) m_Owner->SetActiveDevice(device);
}

DeviceHandle::~DeviceHandle()
{
	if (m_Owner) m_Owner->SetActiveDevice(device_undefined);
	close_device(&device);
}

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

auto Motor::SetRange
(
	const float first_motor_position,
	const float second_motor_position
) -> void
{
	auto& settings = *m_MotorSettings;

	settings.hasValidRange = false;

	const float ratio = settings.stepsPerMMRatio;

	if (!std::isfinite(first_motor_position) ||
		!std::isfinite(second_motor_position) ||
		!std::isfinite(ratio) ||
		ratio <= 0.0f)
	{
		return;
	}

	// Some stages can report their borders in reverse order.
	const float minimumMotorPosition =
		std::min(first_motor_position, second_motor_position);

	const float maximumMotorPosition =
		std::max(first_motor_position, second_motor_position);

	const float motorRange =
		maximumMotorPosition - minimumMotorPosition;

	if (motorRange <= 0.0f)
		return;

	settings.minMotorPos = minimumMotorPosition;
	settings.maxMotorPos = maximumMotorPosition;

	// The old motorRange / 2 calculation was only correct when min == 0.
	settings.middleMotorPos =
		minimumMotorPosition + motorRange / 2.0f;

	settings.motorRange = motorRange;
	settings.hasValidRange = true;

	UpdateStageRange();
}

auto Motor::GoCenter() -> bool
{
	if (!m_MotorSettings->hasValidRange)
		return false;

	DeviceHandle device(m_DeviceName.get(), this);
	if (!device.isValid())
		return false;

	if (!Check(command_move_calb(
		device,
		m_MotorSettings->middleMotorPos,
		&m_StandaSettings->calibration)))
	{
		return false;
	}

	if (!Check(command_wait_for_stop(device, 100)))
		return false;

	std::this_thread::sleep_for(
		std::chrono::milliseconds(wait_delay_milliseconds)
	);

	if (!UpdateStatusAndCalibration(device))
		return false;

	UpdateCurrentPosition();
	return true;
}

// Refactored GoHomeAndZero
auto Motor::GoHomeAndZero() -> bool
{
	DeviceHandle device(m_DeviceName.get(), this);
	if (!device.isValid()) return false;

	if (!Check(command_homezero(device))) return false;
	if (!Check(command_wait_for_stop(device, 100))) return false;
	std::this_thread::sleep_for(std::chrono::milliseconds(wait_delay_milliseconds));
	if (!UpdateStatusAndCalibration(device)) return false;

	UpdateCurrentPosition();
	return true;
}

auto Motor::GoToPos(const float stage_position) -> bool
{
	if (!std::isfinite(stage_position))
		return false;

	DeviceHandle device(m_DeviceName.get(), this);
	if (!device.isValid())
		return false;

	if (!Check(get_status_calb(
		device,
		&m_StandaSettings->calb_state,
		&m_StandaSettings->calibration)))
	{
		return false;
	}

	// Only perform software validation when valid controller borders
	// were actually obtained.
	if (m_MotorSettings->hasValidRange)
	{
		if (stage_position < m_MotorSettings->minStagePos ||
			stage_position > m_MotorSettings->maxStagePos)
		{
			return false;
		}
	}

	const float motorPosition =
		stage_position *
		m_MotorSettings->stepsPerMMRatio;

	if (!std::isfinite(motorPosition))
		return false;

	if (!Check(command_move_calb(
		device,
		motorPosition,
		&m_StandaSettings->calibration)))
	{
		return false;
	}

	if (!Check(command_wait_for_stop(device, 100)))
		return false;

	std::this_thread::sleep_for(
		std::chrono::milliseconds(wait_delay_milliseconds)
	);

	if (!UpdateStatusAndCalibration(device))
		return false;

	UpdateCurrentPosition();
	return true;
}

auto Motor::Stop() -> bool
{
	if (!m_DeviceMutex) return false;

	std::lock_guard<std::mutex> lock(*m_DeviceMutex);

	// Nothing currently open for this motor => nothing in motion to stop.
	if (m_ActiveDevice == device_undefined) return false;

	return command_stop(m_ActiveDevice) == result_ok;
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
	Motor* motor = FindMotorBySerial(motor_sn);

	if (!motor)
		return error_position;

	if (!motor->GoHomeAndZero())
		return error_position;

	return motor->GetDeviceActualStagePos();
}

float MotorArray::GoMotorCenter(const std::string& motor_sn)
{
	Motor* motor = FindMotorBySerial(motor_sn);

	if (!motor)
		return error_position;

	if (!motor->GoCenter())
		return error_position;

	return motor->GetDeviceActualStagePos();
}

float MotorArray::GoMotorToAbsPos(const std::string& motor_sn, float abs_pos)
{
	if (auto* motor = FindMotorBySerial(motor_sn))
	{
		if (!motor->GoToPos(abs_pos))
			return error_position;

		return motor->GetDeviceActualStagePos();
	}
	return error_position;
}

float MotorArray::GoMotorOffset(const std::string& motor_sn, float offset)
{
	Motor* motor = FindMotorBySerial(motor_sn);

	if (!motor)
		return error_position;

	const float currentPosition =
		motor->GetDeviceActualStagePos();

	const float requestedPosition =
		currentPosition + offset;

	if (!std::isfinite(requestedPosition))
		return error_position;

	if (!motor->GoToPos(requestedPosition))
		return error_position;

	return motor->GetDeviceActualStagePos();
}

auto MotorArray::SetStepsPerMMForTheMotor(const std::string& motor_sn, int stepsPerMM) -> void
{
	if (stepsPerMM <= 0) return;

	if (Motor* motor = FindMotorBySerial(motor_sn))
		motor->SetStepsPerMMRatio(stepsPerMM);
}

bool MotorArray::StopMotor(const std::string& motor_sn)
{
	if (Motor* motor = FindMotorBySerial(motor_sn))
		return motor->Stop();

	return false;
}

bool MotorArray::StopAll()
{
	// Deliberately does not short-circuit: every motor currently mid-move
	// should get a stop command, not just the first one found.
	bool stoppedAny = false;
	for (auto& motor : m_MotorsArray)
		stoppedAny |= motor.Stop();

	return stoppedAny;
}

Motor* MotorArray::FindMotorBySerial(const std::string& motor_sn)
{
	int serial_num{};
	try {
		serial_num = std::stoi(motor_sn);
	}
	catch (...) {
		return nullptr;
	}

	for (auto& motor : m_MotorsArray)
	{
		if (motor.GetDeviceSerNum() == serial_num)
			return &motor;
	}
	return nullptr;
}


const Motor* MotorArray::FindMotorBySerial(const std::string& motor_sn) const
{
	int serial_num{};
	try {
		serial_num = std::stoi(motor_sn);
	}
	catch (...) {
		return nullptr;
	}

	for (const auto& motor : m_MotorsArray)
	{
		if (motor.GetDeviceSerNum() == serial_num)
			return &motor;
	}
	return nullptr;
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

	std::string standaAddress = ip_address;

	// Be defensive in case InitAllMotors() is called from somewhere other
	// than cSettings::ReadInitializationFile().
	const auto firstNonWhitespace =
		standaAddress.find_first_not_of(" \t\r\n");

	if (firstNonWhitespace == std::string::npos)
	{
		standaAddress.clear();
	}
	else
	{
		const auto lastNonWhitespace =
			standaAddress.find_last_not_of(" \t\r\n");

		standaAddress = standaAddress.substr
		(
			firstNonWhitespace,
			lastNonWhitespace - firstNonWhitespace + 1
		);
	}

	int probeFlags = ENUMERATE_PROBE;
	std::string enumerationHints;

	// An empty address intentionally means direct USB/COM enumeration only.
	if (!standaAddress.empty())
	{
		probeFlags |= ENUMERATE_NETWORK;
		enumerationHints = "addr=" + standaAddress;
	}

	device_enumeration_t devenum_c =
		enumerate_devices
		(
			probeFlags,
			enumerationHints.c_str()
		);

	if (!devenum_c)
		return false;

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
	edges_settings_calb_t edges_settings_calb_c{};
	stage_information_t stage_information_c{};
	unsigned int device_sn{};
	for (int i = 0; i < names_count; ++i)
	{
		m_MotorsArray.emplace_back(Motor());
		m_MotorsArray[i].SetResult(result_c);

		strcpy(device_name, get_device_name(devenum_c, i));
		m_MotorsArray[i].SetDeviceName(device_name);
		device_c = open_device(device_name);

		if (device_c == device_undefined)
		{
			appendUnitializedMotor(0, i);
			continue;
		}

		get_serial_number(device_c, &device_sn);
		m_MotorsArray[i].SetSerNum(device_sn);

		if ((result_c = get_status(device_c, &state_c)) != result_ok)
		{
			appendUnitializedMotor(device_sn, i);
			close_device(&device_c);
			continue;
		}

		// The device_t device parameter in this function is a C pointer, unlike most library functions that use this parameter
		if ((result_c = set_correction_table(device_c, correction_table)) != result_ok)
		{
			appendUnitializedMotor(device_sn, i);
			close_device(&device_c);
			continue;
		}

		calibration_c.A = 1;
		calibration_c.MicrostepMode = MICROSTEP_MODE_FULL;

		m_MotorsArray[i].SetCalibration(calibration_c);

		/* Get Status */
		if ((result_c = get_status_calb(device_c, &state_calb_c, &calibration_c)) != result_ok)
		{
			appendUnitializedMotor(device_sn, i);
			close_device(&device_c);
			continue;
		}
		m_MotorsArray[i].SetState(state_c);

		result_c = get_edges_settings_calb(
			device_c,
			&edges_settings_calb_c,
			&calibration_c
		);

		if (result_c == result_ok)
		{
			m_MotorsArray[i].SetRange(
				edges_settings_calb_c.LeftBorder,
				edges_settings_calb_c.RightBorder
			);
		}

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
