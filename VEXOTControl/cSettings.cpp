#include "cSettings.h"

#include "wx/stdpaths.h"
#include "wx/filename.h"

cSettings::cSettings
(
	wxWindow* parent_frame,
	const wxString& defaultWorkStation,
	const wxString& defaultMotorsIPAddress
) 
	: 
	wxDialog(
		parent_frame, 
		wxID_ANY, 
		"Settings", 
		wxDefaultPosition, 
		wxDefaultSize, 
		wxDEFAULT_DIALOG_STYLE), 
	workStation(defaultWorkStation),
	m_DefaultMotorsIPAddress(defaultMotorsIPAddress)
{
	CreateMainFrame();
	InitDefaultStateWidgets();
	CenterOnScreen();
}

bool cSettings::IsCapturingFinished() const
{
	return m_Progress->is_finished;
}

void cSettings::ProvideProgressInfo(wxString* msg, int* prgrs)
{
	const int current = m_Progress->current_capture.load();
	const int whole = m_Progress->whole_captures_num.load();

	*msg = "Capturing " + wxString::Format(wxT("%i"), current + 1) + " of " + wxString::Format(wxT("%i"), whole) + " images";
	*prgrs = whole > 0
		? std::clamp(static_cast<int>(100.f * (float)(current + 1) / (float)whole), 0, 100)
		: 0;
}

void cSettings::ResetCapturing()
{
	m_Progress->is_finished = false;
}

void cSettings::SetCurrentProgress(const int& curr_capturing_num, const int& whole_capturing_num)
{
	m_Progress->current_capture = curr_capturing_num;
	m_Progress->whole_captures_num = whole_capturing_num;
	m_Progress->is_finished = curr_capturing_num >= whole_capturing_num ? true : false;
}

auto cSettings::GetSelectedCamera() const -> wxString
{
	return m_KETEK->selected_device_str;
}

void cSettings::CreateMainFrame()
{
	InitComponents();
	LoadWorkStationFiles();

	SetMotorStepsPerMM();

	//IterateOverConnectedCameras();
	//ReadXMLFile();
	CreateSettings();
	BindControls();
	UpdateMotorsAndCameraTXTCtrls();



	ReportMotorInitializationProblems();
	//SelectMotorsAndRangesOnWXChoice();
}

void cSettings::ReportMotorInitializationProblems()
{
	if (!m_PhysicalMotors || m_PhysicalMotors->AreAllMotorsInitialized())
		return;

	const wxString title = "Motors initialization error";

	const auto& initError = m_PhysicalMotors->GetInitializationError();
	if (!initError.empty())
	{
		wxMessageBox
		(
			"The motor controllers could not be initialized:\n\n" +
			wxString::FromUTF8(initError),
			title,
			wxICON_ERROR
		);
		return;
	}

	wxString motors{};

	auto motor_arr = m_PhysicalMotors->GetUninitializedMotors();
	for (const auto& motor : motor_arr)
	{
		motors += motor ? wxString::Format(wxT("%u"), motor) : wxString("(unknown serial number)");
		motors += '\n';
	}

	wxMessageBox(
		"Not all motors initialized correctly\nUninitialized motors:\n" + motors,
		title,
		wxICON_ERROR);
}

namespace
{
	auto MutedSettingsColour() -> wxColour
	{
		return wxSystemSettings::GetAppearance().IsDark() ? wxColour(175, 175, 175) : wxColour(95, 95, 95);
	}

	auto MakeSettingsLabel(wxWindow* parent, const wxString& text) -> wxStaticText*
	{
		auto* label = new wxStaticText(parent, wxID_ANY, text);
		label->SetForegroundColour(MutedSettingsColour());
		return label;
	}

	// A bordered card with a bold title, optional right-aligned status text
	// and a content sizer.
	auto MakeSettingsCard(wxWindow* parent, const wxString& title, wxStaticText** statusOut, wxSizer** contentOut) -> wxPanel*
	{
		auto* card = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME | wxTAB_TRAVERSAL);
		auto* sizer = new wxBoxSizer(wxVERTICAL);
		const int pad = card->FromDIP(10);

		auto* header = new wxBoxSizer(wxHORIZONTAL);
		auto* titleLabel = new wxStaticText(card, wxID_ANY, title);
		{
			wxFont font = titleLabel->GetFont();
			font.MakeBold();
			font.SetPointSize(font.GetPointSize() + 1);
			titleLabel->SetFont(font);
		}
		header->Add(titleLabel, 0, wxALIGN_CENTER_VERTICAL);
		header->AddStretchSpacer();

		if (statusOut)
		{
			*statusOut = new wxStaticText(card, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
			header->Add(*statusOut, 0, wxALIGN_CENTER_VERTICAL);
		}

		sizer->Add(header, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

		auto* content = new wxBoxSizer(wxVERTICAL);
		sizer->Add(content, 1, wxEXPAND | wxALL, pad);

		card->SetSizer(sizer);
		*contentOut = content;
		return card;
	}

	// Read-only value that reads like text (no edit box look) but keeps the
	// wxTextCtrl the rest of the dialog writes to.
	auto MakeValueField(wxWindow* parent, int id, const wxSize& size, long align) -> wxTextCtrl*
	{
		auto* field = new wxTextCtrl(parent, id, "None", wxDefaultPosition, size, align | wxTE_READONLY | wxBORDER_NONE);
		field->SetBackgroundColour(parent->GetBackgroundColour());
		return field;
	}
}

void cSettings::CreateSettings()
{
	// Which workstation is selected, and its detector (kept from the old
	// device section: other code relies on initialized_work_station_num).
	wxString deviceManufacturer{ "None" };
	{
		const std::map<SettingsVariables::DeviceManufacturers, wxString> deviceToString =
		{
			{ SettingsVariables::DeviceManufacturers::KETEK, "KETEK" }
		};

		for (auto i{ 0 }; i < m_WorkStations->work_stations_count; ++i)
		{
			if (m_WorkStations->work_station_data[i].workStationName == m_WorkStations->initialized_work_station)
			{
				const auto it = deviceToString.find(m_WorkStations->work_station_data[i].deviceManufacturer);
				deviceManufacturer = it != deviceToString.end() ? it->second : wxString("None");
				m_KETEK->selected_device_str = m_WorkStations->work_station_data[i].selectedDeviceInDataFile;
				m_WorkStations->initialized_work_station_num = i;
				break;
			}
		}
	}

	auto* mainSizer = new wxBoxSizer(wxVERTICAL);
	const int gap = FromDIP(8);
	const int outer = FromDIP(12);

	/* Connection */
	{
		wxSizer* content{};
		wxPanel* card = MakeSettingsCard(this, "Connection", nullptr, &content);

		auto* grid = new wxFlexGridSizer(2, FromDIP(3), FromDIP(14));
		grid->AddGrowableCol(0, 1);
		grid->AddGrowableCol(1, 1);

		grid->Add(MakeSettingsLabel(card, "Motor controller address"));
		grid->Add(MakeSettingsLabel(card, "Workstation"));

		m_IPAddressTextCtrl = std::make_unique<wxTextCtrl>
			(
				card, wxID_ANY, m_DefaultMotorsIPAddress, wxDefaultPosition,
				FromDIP(wxSize(170, -1)), wxTE_LEFT
			);
		m_IPAddressTextCtrl->SetHint("Blank for USB-connected motors");
		m_IPAddressTextCtrl->SetToolTip
		(
			"IP address of the Standa network controller. Leave it blank to "
			"use only motors connected directly over USB/COM. OK reconnects."
		);
		grid->Add(m_IPAddressTextCtrl.get(), 1, wxEXPAND);

		m_WorkStations->work_station_choice = new wxChoice
		(
			card, SettingsVariables::ID::WORK_STATION_CHOICE, wxDefaultPosition,
			FromDIP(wxSize(170, -1)), m_WorkStations->all_work_station_array_str
		);
		m_WorkStations->work_station_choice->SetSelection(m_WorkStations->initialized_work_station_num);
		m_WorkStations->work_station_choice->SetToolTip("Defines which motor drives each axis and which detector is used");
		grid->Add(m_WorkStations->work_station_choice, 1, wxEXPAND);

		grid->Add(MakeSettingsLabel(card, "Leave blank for USB-connected motors."));
		grid->Add(MakeSettingsLabel(card, "Defines which motor drives each axis."));

		content->Add(grid, 0, wxEXPAND);
		mainSizer->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, outer);
	}

	/* Motor assignment */
	{
		wxSizer* content{};
		wxPanel* card = MakeSettingsCard(this, "Motor assignment", &m_MotorSummaryText, &content);

		auto* grid = new wxFlexGridSizer(5, FromDIP(5), FromDIP(14));
		grid->AddGrowableCol(4, 1);

		const std::pair<const char*, long> headings[] =
		{
			{ "Group", wxALIGN_LEFT }, { "Axis", wxALIGN_LEFT }, { "Serial", wxALIGN_RIGHT },
			{ "Steps/mm", wxALIGN_RIGHT }, { "Status", wxALIGN_LEFT }
		};
		for (const auto& [text, align] : headings)
			grid->Add(MakeSettingsLabel(card, text), 0, align);

		const wxSize serialSize(GetTextExtent("00000000").GetWidth() + FromDIP(6), -1);

		const auto addRow = [&](const wxString& group, const wxString& axis, SettingsVariables::MotorSettings& ui, int serialId, int stepsId)
			{
				auto* groupLabel = new wxStaticText(card, wxID_ANY, group);
				{
					wxFont font = groupLabel->GetFont();
					font.MakeBold();
					groupLabel->SetFont(font);
				}
				grid->Add(groupLabel, 0, wxALIGN_CENTER_VERTICAL);
				grid->Add(new wxStaticText(card, wxID_ANY, axis), 0, wxALIGN_CENTER_VERTICAL);

				ui.motor = MakeValueField(card, serialId, serialSize, wxTE_RIGHT);
				grid->Add(ui.motor, 0, wxALIGN_CENTER_VERTICAL);

				ui.steps_per_mm = new wxStaticText(card, stepsId, "None", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
				grid->Add(ui.steps_per_mm, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);

				ui.status = new wxStaticText(card, wxID_ANY, wxEmptyString);
				grid->Add(ui.status, 0, wxALIGN_CENTER_VERTICAL);
			};

		using ID = SettingsVariables::ID;
		addRow("Detector", "X", m_Motors->m_Detector[0], ID::MOT_DET_X_MOTOR_TXT_CTRL, ID::MOT_DET_X_STEPS_PER_MM_ST_TEXT);
		addRow("", "Y", m_Motors->m_Detector[1], ID::MOT_DET_Y_MOTOR_TXT_CTRL, ID::MOT_DET_Y_STEPS_PER_MM_ST_TEXT);
		addRow("Optics", "X", m_Motors->m_Optics[0], ID::MOT_OPT_X_MOTOR_TXT_CTRL, ID::MOT_OPT_X_STEPS_PER_MM_ST_TEXT);
		addRow("", "Y", m_Motors->m_Optics[1], ID::MOT_OPT_Y_MOTOR_TXT_CTRL, ID::MOT_OPT_Y_STEPS_PER_MM_ST_TEXT);
		addRow("", "Z", m_Motors->m_Optics[2], ID::MOT_OPT_Z_MOTOR_TXT_CTRL, ID::MOT_OPT_Z_STEPS_PER_MM_ST_TEXT);
		addRow("", "Pitch", m_Motors->m_Optics[3], ID::MOT_OPT_PITCH_MOTOR_TXT_CTRL, ID::MOT_OPT_PITCH_STEPS_PER_MM_ST_TEXT);
		addRow("", "Yaw", m_Motors->m_Optics[4], ID::MOT_OPT_YAW_MOTOR_TXT_CTRL, ID::MOT_OPT_YAW_STEPS_PER_MM_ST_TEXT);
		addRow("Aux", "X", m_Motors->m_Aux[0], ID::MOT_AUX_X_MOTOR_TXT_CTRL, ID::MOT_AUX_X_STEPS_PER_MM_ST_TEXT);

		content->Add(grid, 0, wxEXPAND);
		content->Add(MakeSettingsLabel(card, "Status is from the last connection. OK reconnects with the address above."), 0, wxTOP, gap);

		mainSizer->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, outer);
	}

	/* Detector */
	{
		wxSizer* content{};
		wxPanel* card = MakeSettingsCard(this, "Detector", &m_DetectorStatusText, &content);

		auto* grid = new wxFlexGridSizer(2, FromDIP(3), FromDIP(14));
		grid->AddGrowableCol(0, 1);
		grid->AddGrowableCol(1, 1);

		grid->Add(MakeSettingsLabel(card, "Manufacturer"));
		grid->Add(MakeSettingsLabel(card, "Serial"));

		m_DeviceManufacturerText = new wxStaticText(card, wxID_ANY, deviceManufacturer);
		grid->Add(m_DeviceManufacturerText, 0, wxALIGN_CENTER_VERTICAL);

		m_KETEK->device = MakeValueField(card, SettingsVariables::ID::DEVICE_TXT_CTRL, wxSize(GetTextExtent("UDXD0000000000000").GetWidth() + FromDIP(8), -1), wxTE_LEFT);
		m_KETEK->device->SetValue(m_KETEK->selected_device_str);
		grid->Add(m_KETEK->device, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL);

		content->Add(grid, 0, wxEXPAND);
		mainSizer->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, outer);
	}

	auto* buttons = CreateButtonSizer(wxOK | wxCANCEL | wxOK_DEFAULT);
	mainSizer->Add(buttons, 0, wxALIGN_RIGHT | wxALL, outer);

	SetSizerAndFit(mainSizer);
	SetDetectorStatus(m_DetectorStatus, m_DetectorStatusOk);
	UpdateMotorStatusLabels();
}

void cSettings::UpdateMotorStatusLabels()
{
	const bool dark = wxSystemSettings::GetAppearance().IsDark();
	const wxColour ok = dark ? wxColour(90, 200, 120) : wxColour(20, 130, 60);
	const wxColour bad = dark ? wxColour(240, 110, 110) : wxColour(190, 30, 30);
	const wxColour muted = MutedSettingsColour();

	int assigned = 0, connected = 0;

	const auto update = [&](SettingsVariables::MotorSettings& ui)
		{
			if (!ui.status || !ui.motor)
				return;

			const wxString serial = ui.motor->GetValue().Trim().Trim(false);
			wxString text{};
			wxColour colour{};

			if (serial.IsEmpty() || serial.CmpNoCase("None") == 0)
			{
				text = "Not assigned";
				colour = muted;
			}
			else
			{
				++assigned;
				const bool found = m_PhysicalMotors && m_PhysicalMotors->MotorHasSerialNumber(serial.ToStdString());
				if (found)
					++connected;

				text = found ? wxString(L"\u25CF Connected") : wxString(L"\u25CF Not found");
				colour = found ? ok : bad;
			}

			if (ui.status->GetLabel() != text)
				ui.status->SetLabel(text);
			ui.status->SetForegroundColour(colour);
			ui.status->Refresh();
		};

	for (int i = 0; i < 2; ++i) update(m_Motors->m_Detector[i]);
	for (int i = 0; i < 5; ++i) update(m_Motors->m_Optics[i]);
	update(m_Motors->m_Aux[0]);

	if (m_MotorSummaryText)
	{
		m_MotorSummaryText->SetLabel
		(
			assigned == 0
			? wxString("no motors assigned")
			: wxString::Format("%d of %d connected", connected, assigned)
		);
		m_MotorSummaryText->SetForegroundColour(assigned > 0 && connected < assigned ? bad : muted);
		m_MotorSummaryText->Refresh();
	}

	Layout();
}

void cSettings::SetDetectorStatus(const wxString& text, bool ok)
{
	m_DetectorStatus = text;
	m_DetectorStatusOk = ok;

	if (!m_DetectorStatusText)
		return;

	const bool dark = wxSystemSettings::GetAppearance().IsDark();
	m_DetectorStatusText->SetLabel(text);
	m_DetectorStatusText->SetForegroundColour
	(
		ok ? (dark ? wxColour(90, 200, 120) : wxColour(20, 130, 60)) : MutedSettingsColour()
	);
	m_DetectorStatusText->Refresh();
	Layout();
}

void cSettings::InitDefaultStateWidgets()
{
}

void cSettings::InitComponents()
{
	m_WorkStations = std::make_unique<SettingsVariables::WorkStations>();
	m_WorkStations->initialized_work_station = workStation;
	m_Motors = std::make_unique<SettingsVariables::MotorSettingsArray>();
	m_KETEK = std::make_unique<SettingsVariables::MeasurementDevice>();
	m_PhysicalMotors = std::make_unique<MotorArray>(m_DefaultMotorsIPAddress.ToStdString());
}

void cSettings::BindControls()
{
	m_WorkStations->work_station_choice->Bind(wxEVT_CHOICE, &cSettings::OnWorkStationChoice, this);
}

void cSettings::UpdateRangesTextCtrls()
{	
	wxString find_string{}, current_range_text{};
	uint8_t current_index{};

	/* Set the corresponding range for selected motors */
	for (auto motor{ 0 }; motor < m_MotorsCount; ++motor)
	{
		if (motor < 2)
		{
			find_string = m_Motors->unique_motors[0][current_index];
			if (find_string == "None")
			{
				m_Motors->m_Detector[motor].steps_per_mm->SetLabel("None");
				continue;
			}
			current_index = m_Motors->xml_all_motors[0].Index(find_string);
			current_range_text = m_Motors->xml_all_motors[1].Item(current_index);

			m_Motors->m_Detector[motor].steps_per_mm->SetLabel(current_range_text);
			/* Update current_selection index of range */
			current_index = m_Motors->unique_motors[1].Index(current_range_text);
		}
		else
		{
			find_string = m_Motors->unique_motors[0][current_index];
			if (find_string == "None")
			{
				m_Motors->m_Optics[motor - 1].steps_per_mm->SetLabel("None");
				continue;
			}
			current_index = m_Motors->xml_all_motors[0].Index(find_string);
			current_range_text = m_Motors->xml_all_motors[1].Item(current_index);

			m_Motors->m_Optics[motor - 1].steps_per_mm->SetLabel(current_range_text);
			/* Update current_selection index of range */
			current_index = m_Motors->unique_motors[1].Index(current_range_text);
		}
	}
}

auto cSettings::OnWorkStationChoice(wxCommandEvent& evt) -> void
{
	const auto curr_selected_work_station = m_WorkStations->work_station_choice->GetSelection();
	m_WorkStations->initialized_work_station_num = curr_selected_work_station;

	SetMotorStepsPerMM();
	UpdateMotorsAndCameraTXTCtrls(curr_selected_work_station);

	Layout();
	Refresh();
	Update();
}

auto cSettings::UpdateMotorsAndCameraTXTCtrls(const short selected_work_station) -> void
{
	if (selected_work_station > -1)
	{
		m_WorkStations->initialized_work_station_num = selected_work_station;
	}
	else
	{
		m_WorkStations->work_station_choice->SetSelection(m_WorkStations->initialized_work_station_num);
	}

	if (m_WorkStations->work_stations_count == 0 ||
		m_WorkStations->initialized_work_station_num >= m_WorkStations->work_stations_count ||
		m_WorkStations->initialized_work_station_num >= m_WorkStations->all_work_station_array_str.GetCount())
	{
		// No usable workstation file was found (the loader has already
		// logged why); leave the motor fields empty instead of indexing
		// into empty arrays.
		return;
	}

	m_WorkStations->initialized_work_station =
		m_WorkStations->all_work_station_array_str[m_WorkStations->initialized_work_station_num];

	const auto& ws = m_WorkStations->work_station_data[m_WorkStations->initialized_work_station_num];

	auto setMotorUi =
		[&](SettingsVariables::MotorSettings& ui, const int motorName)
		{
			const auto motorSN = GetSelectedMotorSerialNumberFromMotorSettings(motorName);

			ui.motor->SetValue(motorSN);
			ui.motor_sn = motorSN;

			auto it = ws.motorsStepsPerMM.find(motorSN);
			if (it != ws.motorsStepsPerMM.end())
				ui.steps_per_mm->SetLabel(wxString::Format(wxT("%i"), it->second));
			else
				ui.steps_per_mm->SetLabel("None");
		};

	setMotorUi(m_Motors->m_Detector[0], SettingsVariables::DETECTOR_X);
	setMotorUi(m_Motors->m_Detector[1], SettingsVariables::DETECTOR_Y);

	setMotorUi(m_Motors->m_Optics[0], SettingsVariables::OPTICS_X);
	setMotorUi(m_Motors->m_Optics[1], SettingsVariables::OPTICS_Y);
	setMotorUi(m_Motors->m_Optics[2], SettingsVariables::OPTICS_Z);
	setMotorUi(m_Motors->m_Optics[3], SettingsVariables::OPTICS_PITCH);
	setMotorUi(m_Motors->m_Optics[4], SettingsVariables::OPTICS_YAW);

	setMotorUi(m_Motors->m_Aux[0], SettingsVariables::AUX_X);

	m_KETEK->device->SetValue(ws.selectedDeviceInDataFile);
	m_KETEK->selected_device_str = ws.selectedDeviceInDataFile;

	if (m_DeviceManufacturerText)
		m_DeviceManufacturerText->SetLabel(ws.deviceManufacturer == SettingsVariables::DeviceManufacturers::KETEK ? "KETEK" : "None");

	UpdateMotorStatusLabels();
}

void cSettings::OnRefreshBtn(wxCommandEvent& evt)
{
	wxBusyCursor busy_cursor{};

	m_Motors->m_Detector[0].steps_per_mm->SetLabel("None");

	for (int i = 0; i < 5; ++i)
		m_Motors->m_Optics[i].steps_per_mm->SetLabel("None");

	m_Motors->m_Aux[0].steps_per_mm->SetLabel("None");
}

bool cSettings::CheckIfThereIsCollisionWithMotors()
{
	auto raise_exception_msg = []() 
	{
		wxString title = "Motor selection error";
		wxMessageBox(
			wxT
			(
				"There is a collision of motors"
				"\nYou have selected minimum 2 axis on the same motor ID"
				"\nPlease, change selection and try again"
			),
			title,
			wxICON_ERROR);
	};
	return false;
}

bool cSettings::CheckIfUserSelectedAllRangesForAllSelectedMotors()
{
	return true;
}

bool cSettings::CheckIfUserSelectedAllMotorsForAllSelectedRanges()
{	
	auto raise_exception_msg = []() 
	{
		wxString title = "Motor selection error";
		wxMessageBox(
			wxT
			(
				"You didn't select motors for all selected ranges"
				"\nPlease, select motors for all selected ranges and try again"
			),
			title,
			wxICON_ERROR);
	};
	return true;
}

void cSettings::OnCancelBtn(wxCommandEvent& evt)
{
	Hide();
}

unsigned int cSettings::FindSerialNumber
(
	const uint8_t selection_number, 
	const SettingsVariables::MotorSettings* motor_settings
) const
{
	wxString find_string{};
	find_string = m_Motors->unique_motors[0][selection_number];
	return (unsigned int)wxAtoi(find_string);
}

auto cSettings::CompareXMLWithConnectedDevices()
{
	auto raise_exception_msg = []() 
	{
		wxString title = "Device enumeration error";
		wxMessageBox(
			wxT
			(
				"Data file is not correct!"
				"\nData from file don't correspond with connected devices"
			),
			title,
			wxICON_ERROR);
	};


	auto physical_motors = m_PhysicalMotors->GetNamesWithRanges();

	unsigned short serial_numbers_in_xml = m_Motors->unique_motors_map.size();
	m_Motors->unique_motors_map.clear();
	for (const auto& motor : physical_motors)
	{
		m_Motors->unique_motors_map.emplace(motor);
	}

	auto default_state_of_motors = [&]()
	{
		m_Motors->xml_all_motors[0].Clear();
		m_Motors->xml_all_motors[1].Clear();
		m_Motors->xml_selected_motors[0].Clear();
		m_Motors->xml_selected_motors[1].Clear();

		wxString motor_sn{}, motor_range{};
		std::map<unsigned int, float>::iterator phys_mot_iter = physical_motors.begin();
		for (auto motor{ 0 }; motor < m_MotorsCount; ++motor)
		{
			if (motor < physical_motors.size())
			{
				motor_sn = wxString::Format(wxT("%i"), phys_mot_iter->first);
				motor_range = wxString::Format(wxT("%.2f"), phys_mot_iter->second);
				++phys_mot_iter;
			}
			else
			{
				motor_sn = "None";
				motor_range = "None";
			}
			m_Motors->xml_all_motors[0].Add(motor_sn);
			m_Motors->xml_all_motors[1].Add(motor_range);
		}
	};

	if (serial_numbers_in_xml != m_Motors->unique_motors_map.size() || serial_numbers_in_xml == 0)
	{
		m_Motors->unique_motors_map = physical_motors;
		default_state_of_motors();
		raise_exception_msg();
	}
}

auto cSettings::LoadWorkStationFiles() -> void
{
	m_WorkStations->work_stations_count = 0;
	m_WorkStations->work_station_data.reset();
	m_WorkStations->all_work_station_array_str.Clear();

	const std::filesystem::path relativeDirectory(m_WorkStationFilePath.ToStdWstring());
	const auto desiredExtension = std::string(".json");

	// Lists the workstation files in one folder (empty if it can't be read).
	const auto listWorkStationFiles = [&](const std::filesystem::path& directory)
		{
			std::vector<std::filesystem::path> found{};

			std::error_code ec{};
			std::filesystem::directory_iterator it(directory, ec), endIt{};
			if (ec)
				return found;

			for (; it != endIt; it.increment(ec))
			{
				if (ec)
					break;

				const auto& path = it->path();
				const bool isNotLegacyInitializationFile = path.filename() != "init.json";

				std::error_code fileEc{};
				if (it->is_regular_file(fileEc) && path.extension() == desiredExtension && isNotLegacyInitializationFile)
					found.push_back(path);
			}

			return found;
		};

	// Same lookup as always: src\ in the working directory. Only if that has
	// no workstation files (e.g. a shortcut with a different "Start in"
	// folder) fall back to src\ next to the executable. The exe folder is
	// not preferred because build folders can hold their own, incomplete
	// src\ (bin\x64\Debug\src has none, bin\x64\Release\src only some).
	const wxFileName executable(wxStandardPaths::Get().GetExecutablePath());
	const auto exeRelativeDirectory =
		std::filesystem::path(executable.GetPath().ToStdWstring()) / relativeDirectory;

	std::vector<std::filesystem::path> files = listWorkStationFiles(relativeDirectory);
	if (files.empty())
		files = listWorkStationFiles(exeRelativeDirectory);

	if (files.empty())
	{
		std::error_code cwdEc{};
		wxLogError
		(
			"No workstation files (*.json) were found in:\n%s\nor\n%s",
			wxString(exeRelativeDirectory.wstring()),
			wxString((std::filesystem::current_path(cwdEc) / relativeDirectory).wstring())
		);
		return;
	}

	// Keep only files that parse completely, so the station names shown in
	// the list and the data array stay index-aligned.
	std::vector<SettingsVariables::WorkStationData> parsed{};
	wxString problems{};

	for (const auto& file : files)
	{
		SettingsVariables::WorkStationData data{};
		wxString error{};

		if (ReadWorkStationFile(file, data, error))
			parsed.push_back(std::move(data));
		else
			problems += wxString(file.filename().wstring()) + ": " + error + "\n";
	}

	m_WorkStations->work_stations_count = static_cast<unsigned short>(parsed.size());
	m_WorkStations->work_station_data =
		std::make_unique<SettingsVariables::WorkStationData[]>(parsed.size());

	for (std::size_t i = 0; i < parsed.size(); ++i)
	{
		m_WorkStations->all_work_station_array_str.Add(parsed[i].workStationName);
		m_WorkStations->work_station_data[i] = std::move(parsed[i]);
	}

	if (!problems.IsEmpty())
	{
		wxLogWarning
		(
			"Some workstation files were skipped because they could not be read:\n\n%s",
			problems
		);
	}
}

auto cSettings::ReadWorkStationFile
(
	const std::filesystem::path& filePath,
	SettingsVariables::WorkStationData& data,
	wxString& error
) -> bool
{
	std::ifstream fileStream(filePath);
	if (!fileStream.is_open())
	{
		error = "the file could not be opened";
		return false;
	}

	try
	{
		nlohmann::json j;
		fileStream >> j;

		if (!j.is_object())
		{
			error = "the file does not contain a JSON object";
			return false;
		}

		if (!j.contains("station") || !j["station"].is_string() || j["station"].get<std::string>().empty())
		{
			error = "the \"station\" name is missing";
			return false;
		}

		const SettingsVariables::MotorManufacturers fallback =
			j.contains("motor_manufacturer")
			? SettingsVariables::ParseVendor(j.at("motor_manufacturer").get<std::string>())
			: SettingsVariables::MotorManufacturers::STANDA;

		const auto readMotors = [&](const char* section)
			{
				if (!j.contains(section))
					return;

				const auto& motors = j.at(section);
				if (!motors.is_array())
					throw std::runtime_error(std::string("\"") + section + "\" must be a list of motors");

				for (const auto& motor : motors)
				{
					// at() throws a descriptive error for a missing field; get<>
					// throws for a value of the wrong type.
					const std::string sn = motor.at("SerialNumber").get<std::string>();
					const int stepsPerMM = motor.at("StepsPerMM").get<int>();

					// Empty serial numbers and non-positive StepsPerMM were
					// accepted before and are ignored later
					// (SetStepsPerMMForTheMotor skips values <= 0), so they
					// don't invalidate the whole file.

					const SettingsVariables::MotorManufacturers vendor =
						motor.contains("Manufacturer")
						? SettingsVariables::ParseVendor(motor.at("Manufacturer").get<std::string>())
						: fallback;

					data.motorVendorBySN.emplace(wxString(sn), vendor);
					data.selectedMotorsInDataFile.Add(wxString(sn));
					data.motorsStepsPerMM.insert(std::make_pair(wxString(sn), stepsPerMM));
				}
			};

		readMotors("detector");
		readMotors("optics");
		readMotors("aux");

		if (j.contains("device"))
			data.selectedDeviceInDataFile = wxString(j.at("device").get<std::string>());

		if (j.contains("device_manufacturer"))
		{
			const wxString deviceManufacturerStr = wxString(j.at("device_manufacturer").get<std::string>());
			if (deviceManufacturerStr.Lower() == "ketek")
				data.deviceManufacturer = SettingsVariables::DeviceManufacturers::KETEK;
		}

		data.workStationName = wxString(j.at("station").get<std::string>());
	}
	catch (const std::exception& e)
	{
		error = wxString::FromUTF8(e.what());
		return false;
	}

	return true;
}

void cSettings::UpdateUniqueArray()
{
	m_Motors->unique_motors[0].Clear();
	m_Motors->unique_motors[1].Clear();
	m_Motors->unique_motors[0].Add("None");
	m_Motors->unique_motors[1].Add("None");

	for (const auto& motor : m_Motors->unique_motors_map)
	{
		m_Motors->unique_motors[0].Add(wxString::Format(wxT("%i"), motor.first));
		m_Motors->unique_motors[1].Add(wxString::Format(wxT("%.2f"), motor.second));
	}
}

void cSettings::SelectMotorsAndRangesFromXMLFile()
{
	auto raise_exception_msg = []() 
	{
		wxString title = "Data file error";
		wxMessageBox(
			wxT
			(
				"Data file is not correct!"
				"\nSelected motors data don't correspond to all motors data"
			),
			title,
			wxICON_ERROR);
	};
}

auto cSettings::GetSelectedMotorSerialNumberFromMotorSettings(const int motorName) const -> wxString
{
	if (!m_WorkStations)
		return "None";

	if (m_WorkStations->initialized_work_station_num >= m_WorkStations->work_stations_count)
		return "None";

	const auto& motors =
		m_WorkStations->work_station_data[m_WorkStations->initialized_work_station_num].selectedMotorsInDataFile;

	if (motorName < 0 || motorName >= static_cast<int>(motors.size()))
		return "None";

	return motors[motorName];
}

void cSettings::SetMotorStepsPerMM()
{
	if (!m_WorkStations || !m_PhysicalMotors)
		return;

	if (m_WorkStations->initialized_work_station_num >= m_WorkStations->work_stations_count)
		return;

	const auto& ws =
		m_WorkStations->work_station_data[m_WorkStations->initialized_work_station_num];

	for (const auto& [motorSN, stepsPerMM] : ws.motorsStepsPerMM)
	{
		if (motorSN.IsEmpty() || motorSN == "None")
			continue;

		if (stepsPerMM <= 0)
			continue;

		m_PhysicalMotors->SetStepsPerMMForTheMotor(motorSN.ToStdString(), stepsPerMM);
	}

	m_PhysicalMotors->FillNames();
}

auto cSettings::SetStepsPerMMForTheMotor(const std::string& motor_sn, int stepsPerMM) -> void
{
	if (stepsPerMM <= 0) return;

	if (Motor* motor = m_PhysicalMotors->FindMotorBySerial(motor_sn))
		motor->SetStepsPerMMRatio(stepsPerMM);
}

int cSettings::ShowModal()
{
	UpdateMotorStatusLabels();

	const auto retCode = wxDialog::ShowModal();

	if (retCode == wxID_OK)
	{
		m_DefaultMotorsIPAddress =
			m_IPAddressTextCtrl->GetValue();

		m_DefaultMotorsIPAddress.Trim(true);
		m_DefaultMotorsIPAddress.Trim(false);

		// Release the old controllers before enumerating them again.
		m_PhysicalMotors.reset();
		m_PhysicalMotors =
			std::make_unique<MotorArray>
			(
				m_DefaultMotorsIPAddress.ToStdString()
			);

		SetMotorStepsPerMM();
		ReportMotorInitializationProblems();
	}

	Hide();

	return retCode;
}
