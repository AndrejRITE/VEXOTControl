#include "cSettings.h"

cSettings::cSettings(wxWindow* parent_frame, const wxString& defaultMotorsIPAddress) 
	: wxDialog(
		parent_frame, 
		wxID_ANY, 
		"Settings", 
		wxDefaultPosition, 
		wxDefaultSize, 
		wxDEFAULT_DIALOG_STYLE), m_DefaultMotorsIPAddress(defaultMotorsIPAddress)
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
	*msg = "Capturing " + wxString::Format(wxT("%i"), m_Progress->current_capture + 1) + " of " + wxString::Format(wxT("%i"), m_Progress->whole_captures_num) + " images";
	*prgrs = 100.f * (float)(m_Progress->current_capture + 1) / (float)m_Progress->whole_captures_num;
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
	ReadInitializationFile();
	InitComponents();
	LoadWorkStationFiles();

	SetMotorStepsPerMM();

	//IterateOverConnectedCameras();
	//ReadXMLFile();
	CreateSettings();
	BindControls();
	UpdateMotorsAndCameraTXTCtrls();



	if (!m_PhysicalMotors->AreAllMotorsInitialized())
	{
		wxString title = "Motors initialization error";
		wxString motors{};

		auto motor_arr = m_PhysicalMotors->GetUninitializedMotors();
		for (const auto& motor : motor_arr)
		{
			motors += wxString::Format(wxT("%i"), motor);
			motors += '\n';
		}

		wxMessageBox(
			wxT
			(
				"Not all motors initialized correctly\nUninitialized motors:\n" + motors
			),
			title,
			wxICON_ERROR);

	}
	//SelectMotorsAndRangesOnWXChoice();
}

void cSettings::CreateSettings()
{
	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	wxBoxSizer* ms_sizer = new wxBoxSizer(wxVERTICAL);
	
	CreateIPAddressSection(ms_sizer);
	CreateMotorsSelection(ms_sizer);
	
	main_sizer->Add(ms_sizer, 1, wxEXPAND);

	auto btnSizer = CreateButtonSizer(wxOK | wxCANCEL | wxOK_DEFAULT);
	main_sizer->Add(btnSizer, 0, wxALIGN_RIGHT | wxALL, 5);

	SetSizerAndFit(main_sizer);
}

auto cSettings::CreateDetectorPage(wxWindow* parent, const wxSize& txtCtrlSize, const int& topOffset) -> wxWindow*
{
	auto page = new wxPanel(parent);
	auto sizerPage = new wxBoxSizer(wxVERTICAL);

	/* X */
	wxSizer* const det_x_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&X");
	/* Serial Number */
	{
		wxSizer* const sn_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&S/N");

		m_Motors->m_Detector[0].motor = new wxTextCtrl(
			page, 
			SettingsVariables::ID::MOT_DET_X_MOTOR_TXT_CTRL, 
			wxT("None"),
			wxDefaultPosition, 
			txtCtrlSize,
			wxTE_CENTRE | wxTE_READONLY
		);
		
		m_Motors->m_Detector[0].motor->SetValue(GetSelectedMotorSerialNumberFromMotorSettings(0));
		//m_Motors->m_Detector[0].motors->SetSelection(0);

		sn_static_box_sizer->Add(m_Motors->m_Detector[0].motor);

		det_x_static_box_sizer->Add(sn_static_box_sizer);
	}
	/* Steps/mm */
	det_x_static_box_sizer->AddSpacer(2);
	{
		wxSizer* const range_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Steps/mm");

		m_Motors->m_Detector[0].steps_per_mm = new wxStaticText(
			page,
			SettingsVariables::ID::MOT_DET_X_STEPS_PER_MM_ST_TEXT,
			wxT("None"), 
			wxDefaultPosition, 
			wxDefaultSize, 
			wxALIGN_CENTRE_HORIZONTAL);

		range_static_box_sizer->AddStretchSpacer();
		range_static_box_sizer->Add(m_Motors->m_Detector[0].steps_per_mm, 0, wxTOP, topOffset);
		range_static_box_sizer->AddStretchSpacer();

		det_x_static_box_sizer->Add(range_static_box_sizer, 0, wxEXPAND);
	}
	sizerPage->Add(det_x_static_box_sizer, 0, wxEXPAND);

	/* Y */
	wxSizer* const det_y_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Y");
	/* Serial Number */
	{
		wxSizer* const sn_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&S/N");

		m_Motors->m_Detector[1].motor = new wxTextCtrl(
			page, 
			SettingsVariables::ID::MOT_DET_Y_MOTOR_TXT_CTRL, 
			wxT("None"),
			wxDefaultPosition, 
			txtCtrlSize,
			wxTE_CENTRE | wxTE_READONLY
		);
		
		m_Motors->m_Detector[1].motor->SetValue(GetSelectedMotorSerialNumberFromMotorSettings(1));
		sn_static_box_sizer->Add(m_Motors->m_Detector[1].motor);

		det_y_static_box_sizer->Add(sn_static_box_sizer);
	}
	/* Steps/mm */
	det_y_static_box_sizer->AddSpacer(2);
	{
		wxSizer* const range_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Steps/mm");

		m_Motors->m_Detector[1].steps_per_mm = new wxStaticText(
			page,
			SettingsVariables::ID::MOT_DET_Y_STEPS_PER_MM_ST_TEXT,
			wxT("None"), 
			wxDefaultPosition, 
			wxDefaultSize, 
			wxALIGN_CENTRE_HORIZONTAL);

		range_static_box_sizer->AddStretchSpacer();
		range_static_box_sizer->Add(m_Motors->m_Detector[1].steps_per_mm, 0, wxTOP, topOffset);
		range_static_box_sizer->AddStretchSpacer();

		det_y_static_box_sizer->Add(range_static_box_sizer, 0, wxEXPAND);
	}
	sizerPage->Add(det_y_static_box_sizer, 0, wxEXPAND);

	page->SetSizer(sizerPage);
	return page;
}

auto cSettings::CreateOpticsPage(wxWindow* parent, const wxSize& txtCtrlSize, const int& topOffset) -> wxWindow*
{
	auto page = new wxPanel(parent);
	auto sizerPage = new wxBoxSizer(wxVERTICAL);

	wxSizer* const xyz_sizer = new wxBoxSizer(wxVERTICAL);
	{
		/* X */
		wxSizer* const opt_x_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&X");
		/* Serial Number */
		{
			wxSizer* const sn_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&S/N");

			m_Motors->m_Optics[0].motor = new wxTextCtrl(
				page,
				SettingsVariables::ID::MOT_OPT_X_MOTOR_TXT_CTRL,
				wxT("None"),
				wxDefaultPosition,
				txtCtrlSize,
				wxTE_CENTRE | wxTE_READONLY
				//m_Motors->unique_motors[0]
			);

			m_Motors->m_Optics[0].motor->SetValue(GetSelectedMotorSerialNumberFromMotorSettings(1));
			//m_Motors->m_Optics[0].motors->SetSelection(0);

			sn_static_box_sizer->Add(m_Motors->m_Optics[0].motor);

			opt_x_static_box_sizer->Add(sn_static_box_sizer);
		}
		/* Steps/mm */
		opt_x_static_box_sizer->AddSpacer(2);
		{
			wxSizer* const range_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Steps/mm");

			m_Motors->m_Optics[0].steps_per_mm = new wxStaticText(
				page,
				SettingsVariables::ID::MOT_OPT_X_STEPS_PER_MM_ST_TEXT,
				wxT("None"),
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL);

			range_static_box_sizer->AddStretchSpacer();
			range_static_box_sizer->Add(m_Motors->m_Optics[0].steps_per_mm, 0, wxTOP, topOffset);
			range_static_box_sizer->AddStretchSpacer();

			opt_x_static_box_sizer->Add(range_static_box_sizer, 0, wxEXPAND);
		}
		xyz_sizer->Add(opt_x_static_box_sizer, 0, wxEXPAND);
		xyz_sizer->AddSpacer(2);
		xyz_sizer->AddStretchSpacer();

		/* Y */
		wxSizer* const opt_y_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Y");
		/* Serial Number */
		{
			wxSizer* const sn_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&S/N");

			m_Motors->m_Optics[1].motor = new wxTextCtrl(
				page,
				SettingsVariables::ID::MOT_OPT_Y_MOTOR_TXT_CTRL,
				wxT("None"),
				wxDefaultPosition,
				txtCtrlSize,
				wxTE_CENTRE | wxTE_READONLY
				//m_Motors->unique_motors[0]
			);

			m_Motors->m_Optics[1].motor->SetValue(GetSelectedMotorSerialNumberFromMotorSettings(2));
			//m_Motors->m_Optics[1].motors->SetSelection(0);

			sn_static_box_sizer->Add(m_Motors->m_Optics[1].motor);

			opt_y_static_box_sizer->Add(sn_static_box_sizer);
		}
		/* Steps/mm */
		opt_y_static_box_sizer->AddSpacer(2);
		{
			wxSizer* const range_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Steps/mm");

			m_Motors->m_Optics[1].steps_per_mm = new wxStaticText(
				page,
				SettingsVariables::ID::MOT_OPT_Y_STEPS_PER_MM_ST_TEXT,
				wxT("None"),
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL);

			range_static_box_sizer->AddStretchSpacer();
			range_static_box_sizer->Add(m_Motors->m_Optics[1].steps_per_mm, 0, wxTOP, topOffset);
			range_static_box_sizer->AddStretchSpacer();

			opt_y_static_box_sizer->Add(range_static_box_sizer, 0, wxEXPAND);
		}
		xyz_sizer->Add(opt_y_static_box_sizer, 0, wxEXPAND);
		xyz_sizer->AddSpacer(2);
		xyz_sizer->AddStretchSpacer();

		/* Z */
		wxSizer* const opt_z_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Z");
		/* Serial Number */
		{
			wxSizer* const sn_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&S/N");

			m_Motors->m_Optics[2].motor = new wxTextCtrl(
				page,
				SettingsVariables::ID::MOT_OPT_Z_MOTOR_TXT_CTRL,
				wxT("None"),
				wxDefaultPosition,
				txtCtrlSize,
				wxTE_CENTRE | wxTE_READONLY
				//m_Motors->unique_motors[0]
			);

			m_Motors->m_Optics[2].motor->SetValue(GetSelectedMotorSerialNumberFromMotorSettings(3));
			//m_Motors->m_Optics[2].motors->SetSelection(0);

			sn_static_box_sizer->Add(m_Motors->m_Optics[2].motor);

			opt_z_static_box_sizer->Add(sn_static_box_sizer);
		}
		/* Steps/mm */
		opt_z_static_box_sizer->AddSpacer(2);
		{
			wxSizer* const range_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Steps/mm");

			m_Motors->m_Optics[2].steps_per_mm = new wxStaticText(
				page,
				SettingsVariables::ID::MOT_OPT_Z_STEPS_PER_MM_ST_TEXT,
				wxT("None"),
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL);

			range_static_box_sizer->AddStretchSpacer();
			range_static_box_sizer->Add(m_Motors->m_Optics[2].steps_per_mm, 0, wxTOP, topOffset);
			range_static_box_sizer->AddStretchSpacer();

			opt_z_static_box_sizer->Add(range_static_box_sizer, 0, wxEXPAND);
		}
		xyz_sizer->Add(opt_z_static_box_sizer, 0, wxEXPAND);
	}

	sizerPage->Add(xyz_sizer, 0, wxEXPAND);

	wxSizer* const pitch_yaw_sizer = new wxBoxSizer(wxVERTICAL);
	{
		/* Pitch */
		wxSizer* const opt_pitch_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Pitch");
		/* Serial Number */
		{
			wxSizer* const sn_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&S/N");

			m_Motors->m_Optics[3].motor = new wxTextCtrl(
				page,
				SettingsVariables::ID::MOT_OPT_PITCH_MOTOR_TXT_CTRL,
				wxT("None"),
				wxDefaultPosition,
				txtCtrlSize,
				wxTE_CENTRE | wxTE_READONLY
				//m_Motors->unique_motors[0]
			);

			m_Motors->m_Optics[3].motor->SetValue(GetSelectedMotorSerialNumberFromMotorSettings(4));
			//m_Motors->m_Optics[0].motors->SetSelection(0);

			sn_static_box_sizer->Add(m_Motors->m_Optics[3].motor);

			opt_pitch_static_box_sizer->Add(sn_static_box_sizer);
		}
		/* Steps/mm */
		opt_pitch_static_box_sizer->AddSpacer(2);
		{
			wxSizer* const range_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Steps/mm");

			m_Motors->m_Optics[3].steps_per_mm = new wxStaticText(
				page,
				SettingsVariables::ID::MOT_OPT_PITCH_STEPS_PER_MM_ST_TEXT,
				wxT("None"),
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL);

			range_static_box_sizer->AddStretchSpacer();
			range_static_box_sizer->Add(m_Motors->m_Optics[3].steps_per_mm, 0, wxTOP, topOffset);
			range_static_box_sizer->AddStretchSpacer();

			opt_pitch_static_box_sizer->Add(range_static_box_sizer, 0, wxEXPAND);
		}
		pitch_yaw_sizer->AddStretchSpacer();
		pitch_yaw_sizer->Add(opt_pitch_static_box_sizer, 0, wxEXPAND);
		pitch_yaw_sizer->AddSpacer(2);

		/* Yaw */
		wxSizer* const opt_yaw_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Yaw");
		/* Serial Number */
		{
			wxSizer* const sn_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&S/N");

			m_Motors->m_Optics[4].motor = new wxTextCtrl(
				page,
				SettingsVariables::ID::MOT_OPT_YAW_MOTOR_TXT_CTRL,
				wxT("None"),
				wxDefaultPosition,
				txtCtrlSize,
				wxTE_CENTRE | wxTE_READONLY
			);

			m_Motors->m_Optics[4].motor->SetValue(GetSelectedMotorSerialNumberFromMotorSettings(5));

			sn_static_box_sizer->Add(m_Motors->m_Optics[4].motor);

			opt_yaw_static_box_sizer->Add(sn_static_box_sizer);
		}
		/* Steps/mm */
		opt_yaw_static_box_sizer->AddSpacer(2);
		{
			wxSizer* const range_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Steps/mm");

			m_Motors->m_Optics[4].steps_per_mm = new wxStaticText(
				page,
				SettingsVariables::ID::MOT_OPT_YAW_STEPS_PER_MM_ST_TEXT,
				wxT("None"),
				wxDefaultPosition,
				wxDefaultSize,
				wxALIGN_CENTRE_HORIZONTAL);

			range_static_box_sizer->AddStretchSpacer();
			range_static_box_sizer->Add(m_Motors->m_Optics[4].steps_per_mm, 0, wxTOP, topOffset);
			range_static_box_sizer->AddStretchSpacer();

			opt_yaw_static_box_sizer->Add(range_static_box_sizer, 0, wxEXPAND);
		}
		pitch_yaw_sizer->Add(opt_yaw_static_box_sizer, 0, wxEXPAND);
		pitch_yaw_sizer->AddStretchSpacer();

		sizerPage->Add(pitch_yaw_sizer, 0, wxEXPAND);
	}

	page->SetSizer(sizerPage);
	return page;
}

auto cSettings::CreateAuxPage(wxWindow* parent, const wxSize& txtCtrlSize, const int& topOffset) -> wxWindow*
{
	auto page = new wxPanel(parent);
	auto sizerPage = new wxBoxSizer(wxVERTICAL);

	wxSizer* const aux_x_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&X");

	/* Serial Number */
	{
		wxSizer* const sn_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&S/N");

		m_Motors->m_Aux[0].motor = new wxTextCtrl(
			page,
			SettingsVariables::ID::MOT_AUX_X_MOTOR_TXT_CTRL,
			wxT("None"),
			wxDefaultPosition,
			txtCtrlSize,
			wxTE_CENTRE | wxTE_READONLY
		);

		m_Motors->m_Aux[0].motor->SetValue(GetSelectedMotorSerialNumberFromMotorSettings(SettingsVariables::AUX_X));

		sn_static_box_sizer->Add(m_Motors->m_Aux[0].motor);
		aux_x_static_box_sizer->Add(sn_static_box_sizer);
	}

	/* Steps/mm */
	aux_x_static_box_sizer->AddSpacer(2);
	{
		wxSizer* const steps_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Steps/mm");

		m_Motors->m_Aux[0].steps_per_mm = new wxStaticText(
			page,
			SettingsVariables::ID::MOT_AUX_X_STEPS_PER_MM_ST_TEXT,
			wxT("None"),
			wxDefaultPosition,
			wxDefaultSize,
			wxALIGN_CENTRE_HORIZONTAL
		);

		steps_static_box_sizer->AddStretchSpacer();
		steps_static_box_sizer->Add(m_Motors->m_Aux[0].steps_per_mm, 0, wxTOP, topOffset);
		steps_static_box_sizer->AddStretchSpacer();

		aux_x_static_box_sizer->Add(steps_static_box_sizer, 0, wxEXPAND);
	}

	sizerPage->Add(aux_x_static_box_sizer, 0, wxEXPAND);
	sizerPage->AddStretchSpacer();

	page->SetSizer(sizerPage);
	return page;
}

auto cSettings::CreateDeviceSection(wxWindow* parent, wxSizer* sizer) -> void
{
	auto txtCtrlSize = wxSize(140, 24);

	wxString deviceManufacturer{ "None" };

	std::map<SettingsVariables::DeviceManufacturers, wxString> deviceToString = { { SettingsVariables::DeviceManufacturers::KETEK, "KETEK" } };

	for (auto i{ 0 }; i < m_WorkStations->work_stations_count; ++i)
	{
		if (m_WorkStations->work_station_data[i].workStationName == m_WorkStations->initialized_work_station)
		{
			deviceManufacturer = deviceToString[m_WorkStations->work_station_data[i].deviceManufacturer];
			m_KETEK->selected_device_str = m_WorkStations->work_station_data[i].selectedDeviceInDataFile;
			m_WorkStations->initialized_work_station_num = i;
			break;
		}
	}

	wxSizer* const deviceSizer = new wxStaticBoxSizer(wxVERTICAL, parent, "&Device");
	{
		const auto nameSizer = new wxBoxSizer(wxHORIZONTAL);
		{
			nameSizer->Add(new wxStaticText(parent, wxID_ANY, "Manufacturer:"), 0, wxALIGN_CENTER_VERTICAL);
			nameSizer->AddSpacer(5);
			nameSizer->Add(new wxStaticText(parent, wxID_ANY, deviceManufacturer), 0, wxALIGN_CENTER_VERTICAL);
		}

		deviceSizer->Add(nameSizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 2);
		deviceSizer->AddSpacer(5);

		const auto idSizer = new wxBoxSizer(wxHORIZONTAL);
		{
			idSizer->Add(new wxStaticText(parent, wxID_ANY, "ID:"), 0, wxALIGN_CENTER_VERTICAL);
			idSizer->AddSpacer(5);

			m_KETEK->device = new wxTextCtrl
			(
				parent,
				SettingsVariables::ID::DEVICE_TXT_CTRL,
				wxT("None"),
				wxDefaultPosition,
				txtCtrlSize,
				wxTE_CENTRE | wxTE_READONLY
			);

			m_KETEK->device->SetValue(m_KETEK->selected_device_str);

			idSizer->Add(m_KETEK->device, 0, wxALIGN_CENTER_VERTICAL);
		}

		deviceSizer->Add(idSizer, 0, wxALIGN_CENTER);
	}

	sizer->Add(deviceSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 2);
}

void cSettings::CreateIPAddressSection(wxBoxSizer* panel_sizer)
{
	wxPanel* mainPanel = new wxPanel(this);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	auto ipSizer = new wxStaticBoxSizer(wxHORIZONTAL, mainPanel, "&IP Address");
	{
		m_IPAddressTextCtrl = std::make_unique<wxTextCtrl>
			(
				mainPanel, 
				wxID_ANY, 
				m_DefaultMotorsIPAddress, 
				wxDefaultPosition, 
				wxDefaultSize, 
				wxTE_CENTRE
			);

		ipSizer->AddStretchSpacer();
		ipSizer->Add(m_IPAddressTextCtrl.get(), 0, wxALIGN_CENTER);
		ipSizer->AddStretchSpacer();
	}
	mainSizer->Add(ipSizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 2);

	mainPanel->SetSizer(mainSizer);
	panel_sizer->Add(mainPanel, 0, wxEXPAND);
}

void cSettings::CreateMotorsSelection(wxBoxSizer* panel_sizer)
{	
	wxPanel* mainPanel = new wxPanel(this);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	/* Work Station */
	wxSizer* const work_station_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, mainPanel, "&Workstation");
	{
		auto work_station_txt_ctrl_size = wxSize(120, 24);
		m_WorkStations->work_station_choice = new wxChoice
		(
			mainPanel, 
			SettingsVariables::ID::WORK_STATION_CHOICE, 
			wxDefaultPosition, 
			work_station_txt_ctrl_size,
			m_WorkStations->all_work_station_array_str
		);
		m_WorkStations->work_station_choice->SetSelection(m_WorkStations->initialized_work_station_num);

		work_station_static_box_sizer->Add(m_WorkStations->work_station_choice, 1, wxEXPAND | wxLEFT | wxRIGHT, 5);
	}
	mainSizer->Add(work_station_static_box_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 2);

	/* Motors */
	auto motorTxtCtrlSize = wxSize(150, 24);
	int topOffsetStaticText{ 5 };

	auto size = wxSize(16, 16);
	auto imgList = new wxImageList(size.x, size.y);

	int detectorImgIndex{}, opticsImgIndex{}, auxImgIndex{};

	// Detector
	{
		auto bitmap = wxART_CAMERA;
		auto client = wxART_CLIENT_MATERIAL_FILLED;
		auto color = wxColour(128, 0, 255);

		auto bmp = wxMaterialDesignArtProvider::GetBitmap(bitmap, client, size, color);
		detectorImgIndex = imgList->Add(bmp);
	}

	// Optics
	{
		auto bitmap = wxART_CIRCLE_HINT;
		auto client = wxART_CLIENT_FLUENTUI_FILLED;
		auto color = wxColour(255, 128, 128);

		auto bmp = wxMaterialDesignArtProvider::GetBitmap(bitmap, client, size, color);
		opticsImgIndex = imgList->Add(bmp);
	}

	// Aux
	{
		auto bitmap = wxART_CYCLONE;
		auto client = wxART_CLIENT_MATERIAL_ROUND;
		auto color = wxColour(163, 73, 164);

		auto bmp = wxMaterialDesignArtProvider::GetBitmap(bitmap, client, size, color);
		auxImgIndex = imgList->Add(bmp);
	}

	m_MotorsNotebook = new wxNotebook(mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

	m_MotorsNotebook->AssignImageList(imgList);

	m_MotorsNotebook->AddPage
	(
		CreateDetectorPage(m_MotorsNotebook, motorTxtCtrlSize, topOffsetStaticText),
		"Detector",
		true,
		detectorImgIndex
	);

	m_MotorsNotebook->AddPage
	(
		CreateOpticsPage(m_MotorsNotebook, motorTxtCtrlSize, topOffsetStaticText),
		"Optics",
		false,
		opticsImgIndex
	);

	m_MotorsNotebook->AddPage
	(
		CreateAuxPage(m_MotorsNotebook, motorTxtCtrlSize, topOffsetStaticText),
		"Aux",
		false,
		auxImgIndex
	);

	mainSizer->Add(m_MotorsNotebook, 0, wxEXPAND | wxLEFT | wxRIGHT, 2);

	CreateDeviceSection(mainPanel, mainSizer);

	mainSizer->AddSpacer(5);
	mainSizer->AddStretchSpacer();

	mainPanel->SetSizer(mainSizer);
	panel_sizer->Add(mainPanel, 1, wxEXPAND);
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
}

void cSettings::OnRefreshBtn(wxCommandEvent& evt)
{
	wxBusyCursor busy_cursor{};

	m_Motors->m_Detector[0].steps_per_mm->SetLabel("None");

	for (int i = 0; i < 5; ++i)
		m_Motors->m_Optics[i].steps_per_mm->SetLabel("None");

	m_Motors->m_Aux[0].steps_per_mm->SetLabel("None");
}

void cSettings::OnOkBtn(wxCommandEvent& evt)
{
	if (
		!CheckIfThereIsCollisionWithMotors() && 
		CheckIfUserSelectedAllRangesForAllSelectedMotors() && 
		CheckIfUserSelectedAllMotorsForAllSelectedRanges())
	{
		Hide();
		RewriteInitializationFile();
	}
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
	std::string fileNameWithPath{};

	m_WorkStations->work_stations_count = 0;

	auto desiredExtension = std::string(".json");

	for (const auto& entry : std::filesystem::directory_iterator(m_WorkStationFilePath.ToStdString()))
	{
		auto isNotInitializationFile = entry.path().filename() != wxFileName(m_InitializationFilePath).GetFullName().ToStdString();

		if (entry.is_regular_file() && entry.path().extension() == desiredExtension && isNotInitializationFile)
		{
			++m_WorkStations->work_stations_count;
		}
	}
	m_WorkStations->work_station_data = std::make_unique<SettingsVariables::WorkStationData[]>(m_WorkStations->work_stations_count);

	auto i{ 0 };
	for (const auto& entry : std::filesystem::directory_iterator(m_WorkStationFilePath.ToStdString())) 
	{
		auto isNotInitializationFile = entry.path().filename() != wxFileName(m_InitializationFilePath).GetFullName().ToStdString();

		if (entry.is_regular_file() && entry.path().extension() == desiredExtension && isNotInitializationFile)
		{
			fileNameWithPath = m_WorkStationFilePath.ToStdString() + entry.path().filename().string();

			ReadWorkStationFile(fileNameWithPath, i);

			++i;
		}
	}
}

auto cSettings::ReadWorkStationFile(const std::string& fileName, int fileNum) -> void
{
	std::ifstream fileStream(fileName);
	if (!fileStream.is_open())
		return;

	nlohmann::json j;
	try {
		fileStream >> j;
	}
	catch (...) {
		return;  // Malformed JSON
	}

	// Detector
	if (j.contains("detector")) {
		for (const auto& motor : j["detector"]) {
			const std::string sn = motor["SerialNumber"];
			const int stepsPerMM = motor["StepsPerMM"];

			SettingsVariables::MotorManufacturers fallback =
				(j.contains("motor_manufacturer") ? SettingsVariables::ParseVendor(j["motor_manufacturer"].get<std::string>())
					: SettingsVariables::MotorManufacturers::STANDA);

			SettingsVariables::MotorManufacturers v =
				(motor.contains("Manufacturer") ? SettingsVariables::ParseVendor(motor["Manufacturer"].get<std::string>())
					: fallback);

			m_WorkStations->work_station_data[fileNum].motorVendorBySN.emplace(wxString(sn), v);
			m_WorkStations->work_station_data[fileNum].selectedMotorsInDataFile.Add(wxString(sn));
			m_WorkStations->work_station_data[fileNum].motorsStepsPerMM.insert(std::make_pair(wxString(sn), stepsPerMM));
		}
	}

	// Optics
	if (j.contains("optics")) {
		for (const auto& motor : j["optics"]) {
			const std::string sn = motor["SerialNumber"];
			const int stepsPerMM = motor["StepsPerMM"];

			SettingsVariables::MotorManufacturers fallback =
				(j.contains("motor_manufacturer") ? SettingsVariables::ParseVendor(j["motor_manufacturer"].get<std::string>())
					: SettingsVariables::MotorManufacturers::STANDA);

			SettingsVariables::MotorManufacturers v =
				(motor.contains("Manufacturer") ? SettingsVariables::ParseVendor(motor["Manufacturer"].get<std::string>())
					: fallback);

			m_WorkStations->work_station_data[fileNum].motorVendorBySN.emplace(wxString(sn), v);
			m_WorkStations->work_station_data[fileNum].selectedMotorsInDataFile.Add(wxString(sn));
			m_WorkStations->work_station_data[fileNum].motorsStepsPerMM.insert(std::make_pair(wxString(sn), stepsPerMM));
		}
	}
	// Aux
	if (j.contains("aux")) {
		for (const auto& motor : j["aux"]) {
			const std::string sn = motor["SerialNumber"];
			const int stepsPerMM = motor["StepsPerMM"];

			SettingsVariables::MotorManufacturers fallback =
				(j.contains("motor_manufacturer") ? SettingsVariables::ParseVendor(j["motor_manufacturer"].get<std::string>())
					: SettingsVariables::MotorManufacturers::STANDA);

			SettingsVariables::MotorManufacturers v =
				(motor.contains("Manufacturer") ? SettingsVariables::ParseVendor(motor["Manufacturer"].get<std::string>())
					: fallback);

			m_WorkStations->work_station_data[fileNum].motorVendorBySN.emplace(wxString(sn), v);
			m_WorkStations->work_station_data[fileNum].selectedMotorsInDataFile.Add(wxString(sn));
			m_WorkStations->work_station_data[fileNum].motorsStepsPerMM.insert(std::make_pair(wxString(sn), stepsPerMM));
		}
	}

	// Device
	if (j.contains("device")) {
		m_WorkStations->work_station_data[fileNum].selectedDeviceInDataFile = wxString(j["device"].get<std::string>());
	}

	// Device Manufacturer
	if (j.contains("device_manufacturer")) {
		const wxString deviceManufacturerStr = wxString(j["device_manufacturer"].get<std::string>());
		if (deviceManufacturerStr.Lower() == "ketek")
			m_WorkStations->work_station_data[fileNum].deviceManufacturer = SettingsVariables::DeviceManufacturers::KETEK;
	}

	// Station
	if (j.contains("station")) {
		const wxString stationName = wxString(j["station"].get<std::string>());
		m_WorkStations->work_station_data[fileNum].workStationName = stationName;
		m_WorkStations->all_work_station_array_str.Add(stationName);
	}
}

auto cSettings::ReadInitializationFile() -> void
{
	auto isValidIP = [](const std::string& ip) -> bool
		{
			std::regex ipPattern(R"(^(\d{1,3}\.){3}\d{1,3}$)");

			if (!std::regex_match(ip, ipPattern))
				return false;

			std::stringstream ss(ip);
			std::string octet;

			while (std::getline(ss, octet, '.'))
			{
				try
				{
					const int value = std::stoi(octet);

					if (value < 0 || value > 255)
						return false;
				}
				catch (...)
				{
					return false;
				}
			}

			return true;
		};

	std::ifstream initializationFile(m_InitializationFilePath.ToStdString());
	if (!initializationFile.is_open())
	{
		wxLogError("Cannot open initialization file: %s", m_InitializationFilePath);
		return;
	}

	nlohmann::json initializationJson;

	try
	{
		initializationFile >> initializationJson;
	}
	catch (const nlohmann::json::parse_error& e)
	{
		wxLogError("Initialization file contains malformed JSON: %s", e.what());
		return;
	}

	if (!initializationJson.contains("work_station") || !initializationJson["work_station"].is_string())
	{
		wxLogError("\"work_station\" is missing or invalid inside the initialization file.");
		return;
	}

	if (!initializationJson.contains("standa_ip") || !initializationJson["standa_ip"].is_string())
	{
		wxLogError("\"standa_ip\" is missing or invalid inside the initialization file.");
		return;
	}

	const std::string workStationName = initializationJson["work_station"].get<std::string>();
	const std::string desiredIP = initializationJson["standa_ip"].get<std::string>();

	if (workStationName.empty())
	{
		wxLogError("\"work_station\" inside the initialization file is empty.");
		return;
	}

	if (!isValidIP(desiredIP))
	{
		wxLogError("\"standa_ip\" inside the initialization file doesn't contain a valid IP address.");
		return;
	}

	workStation = wxString(workStationName);
	m_DefaultMotorsIPAddress = wxString(desiredIP);
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

auto cSettings::RewriteInitializationFile() -> void
{
	nlohmann::json initializationJson;

	{
		std::ifstream initializationFile(m_InitializationFilePath.ToStdString());

		if (initializationFile.is_open())
		{
			try
			{
				initializationFile >> initializationJson;
			}
			catch (const nlohmann::json::parse_error& e)
			{
				wxLogError("Initialization file contains malformed JSON: %s", e.what());
				return;
			}
		}
	}

	initializationJson["work_station"] = m_WorkStations->initialized_work_station.ToStdString();
	initializationJson["standa_ip"] = m_DefaultMotorsIPAddress.ToStdString();

	std::ofstream outputFile(m_InitializationFilePath.ToStdString());
	if (!outputFile.is_open())
	{
		wxLogError("Cannot write initialization file: %s", m_InitializationFilePath);
		return;
	}

	outputFile << initializationJson.dump(4);
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
	auto retCode = wxDialog::ShowModal();

	if (retCode == wxID_OK)
	{
		m_DefaultMotorsIPAddress = m_IPAddressTextCtrl->GetValue();
		m_PhysicalMotors = std::make_unique<MotorArray>(m_DefaultMotorsIPAddress.ToStdString());
		SetMotorStepsPerMM();

		RewriteInitializationFile();
	}

	Hide();

	return retCode;
}
