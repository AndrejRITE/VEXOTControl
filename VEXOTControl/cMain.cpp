#include "cMain.h"

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
EVT_MENU(MainFrameVariables::ID::MENUBAR_FILE_OPEN, cMain::OnOpenMCAFile)
EVT_CLOSE(cMain::OnExit)
EVT_MENU(MainFrameVariables::ID::MENUBAR_FILE_QUIT, cMain::OnExit)
EVT_MENU(MainFrameVariables::ID::RIGHT_DEVICE_SINGLE_SHOT_BTN, cMain::OnSingleShotCameraImage)
EVT_MENU(MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN, cMain::OnStartStopLiveCapturingMenu)
EVT_MENU(MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN, cMain::OnStartStopMeasurementMenu)
EVT_MENU(MainFrameVariables::ID::MENUBAR_WINDOW_ENABLE_DARK_MODE, cMain::OnEnableDarkMode)
EVT_MENU(MainFrameVariables::ID::MENUBAR_EDIT_SETTINGS, cMain::OnOpenSettings)
EVT_MENU(MainFrameVariables::ID::MENUBAR_TOOLS_CURSOR_OVERLAY, cMain::OnCursorOverlayCheck)
EVT_MENU(MainFrameVariables::ID::MENUBAR_WINDOW_FULLSCREEN, cMain::OnFullScreen)
EVT_MENU(MainFrameVariables::ID::MENUBAR_HELP_APPS_VERSION, cMain::OnApplicationVersion)
EVT_MENU(MainFrameVariables::ID::MENUBAR_HELP_OPEN_LOG_FOLDER, cMain::OnOpenLogFolder)
EVT_MAXIMIZE(cMain::OnMaximizeButton)

/* Detector X */
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_SC_DET_X_ABS_TE_CTL, cMain::OnEnterTextCtrlDetectorXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_SET_BTN, cMain::OnSetDetectorXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_DEC_BTN, cMain::OnDecrementDetectorXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_INC_BTN, cMain::OnIncrementDetectorXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_CENTER_BTN, cMain::OnCenterDetectorX)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_HOME_BTN, cMain::OnHomeDetectorX)

/* Detector Y */
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_SC_DET_Y_ABS_TE_CTL, cMain::OnEnterTextCtrlDetectorYAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_Y_SET_BTN, cMain::OnSetDetectorYAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_Y_DEC_BTN, cMain::OnDecrementDetectorYAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_Y_INC_BTN, cMain::OnIncrementDetectorYAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_Y_CENTER_BTN, cMain::OnCenterDetectorY)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_Y_HOME_BTN, cMain::OnHomeDetectorY)

/* Optics X */
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_SC_OPT_X_ABS_TE_CTL, cMain::OnEnterTextCtrlOpticsXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_X_SET_BTN, cMain::OnSetOpticsXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_X_DEC_BTN, cMain::OnDecrementOpticsXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_X_INC_BTN, cMain::OnIncrementOpticsXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_X_CENTER_BTN, cMain::OnCenterOpticsX)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_X_HOME_BTN, cMain::OnHomeOpticsX)
/* Optics Y */
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_SC_OPT_Y_ABS_TE_CTL, cMain::OnEnterTextCtrlOpticsYAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Y_SET_BTN, cMain::OnSetOpticsYAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Y_DEC_BTN, cMain::OnDecrementOpticsYAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Y_INC_BTN, cMain::OnIncrementOpticsYAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Y_CENTER_BTN, cMain::OnCenterOpticsY)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Y_HOME_BTN, cMain::OnHomeOpticsY)
/* Optics Z */
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_SC_OPT_Z_ABS_TE_CTL, cMain::OnEnterTextCtrlOpticsZAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Z_SET_BTN, cMain::OnSetOpticsZAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Z_DEC_BTN, cMain::OnDecrementOpticsZAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Z_INC_BTN, cMain::OnIncrementOpticsZAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Z_CENTER_BTN, cMain::OnCenterOpticsZ)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_Z_HOME_BTN, cMain::OnHomeOpticsZ)
/* Optics Pitch */
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_ABS_TE_CTL, cMain::OnEnterTextCtrlOpticsPitchAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_SET_BTN, cMain::OnSetOpticsPitchAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_DEC_BTN, cMain::OnDecrementOpticsPitchAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_INC_BTN, cMain::OnIncrementOpticsPitchAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_CENTER_BTN, cMain::OnCenterOpticsPitch)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_HOME_BTN, cMain::OnHomeOpticsPitch)
/* Optics Yaw */
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_SC_OPT_YAW_ABS_TE_CTL, cMain::OnEnterTextCtrlOpticsYawAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_YAW_SET_BTN, cMain::OnSetOpticsYawAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_YAW_DEC_BTN, cMain::OnDecrementOpticsYawAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_YAW_INC_BTN, cMain::OnIncrementOpticsYawAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_YAW_CENTER_BTN, cMain::OnCenterOpticsYaw)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_OPT_YAW_HOME_BTN, cMain::OnHomeOpticsYaw)

/* Aux X */
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_SC_AUX_X_ABS_TE_CTL, cMain::OnEnterTextCtrlAuxXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_AUX_X_SET_BTN, cMain::OnSetAuxXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_AUX_X_DEC_BTN, cMain::OnDecrementAuxXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_AUX_X_INC_BTN, cMain::OnIncrementAuxXAbsPos)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_AUX_X_CENTER_BTN, cMain::OnCenterAuxX)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_AUX_X_HOME_BTN, cMain::OnHomeAuxX)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_STOP_ALL_BTN, cMain::OnStopAllMotors)

/* Camera */
EVT_CHOICE(MainFrameVariables::ID::RIGHT_CAM_MANUFACTURER_CHOICE, cMain::ChangeCameraManufacturerChoice)
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_CAM_EXPOSURE_TXT_CTL, cMain::ExposureValueChanged)
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_DEVICE_MIN_RANGE_TXT_CTL, cMain::OnMinRangeKEVChanged)
EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_DEVICE_MAX_RANGE_TXT_CTL, cMain::OnMaxRangeKEVChanged)
EVT_BUTTON(MainFrameVariables::ID::RIGHT_DEVICE_SINGLE_SHOT_BTN, cMain::OnSingleShotCameraImage)
EVT_TEXT(MainFrameVariables::ID::RIGHT_CAM_CROSS_HAIR_POS_X_TXT_CTRL, cMain::OnXPosCrossHairTextCtrl)
EVT_TEXT(MainFrameVariables::ID::RIGHT_CAM_CROSS_HAIR_POS_Y_TXT_CTRL, cMain::OnYPosCrossHairTextCtrl)
EVT_TOGGLEBUTTON(MainFrameVariables::ID::RIGHT_CAM_CROSS_HAIR_SET_POS_TGL_BTN, cMain::OnSetPosCrossHairTglBtn)
/* Set Out Folder */
EVT_BUTTON(MainFrameVariables::ID::RIGHT_MT_OUT_FLD_BTN, cMain::OnSetOutDirectoryBtn)
/* First Stage */
EVT_CHOICE(MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_CHOICE, cMain::OnFirstStageChoice)
/* Second Stage */
EVT_CHOICE(MainFrameVariables::ID::RIGHT_MT_SECOND_STAGE_CHOICE, cMain::OnSecondStageChoice)
/* Start Capturing */
EVT_TOGGLEBUTTON(MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN, cMain::OnStartStopCapturingButton)
/* Start\Stop Live Capturing */
EVT_TOGGLEBUTTON(MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN, cMain::OnStartStopLiveCapturingTglBtn)

/* Live Capturing */
EVT_THREAD(MainFrameVariables::ID::THREAD_LIVE_CAPTURING, cMain::LiveCapturingThread)
/* Worker Thread */
EVT_THREAD(MainFrameVariables::ID::THREAD_MAIN_CAPTURING, cMain::WorkerThreadEvent)
/* Progress */
EVT_THREAD(MainFrameVariables::ID::THREAD_PROGRESS_CAPTURING, cMain::UpdateProgress)

EVT_THREAD(MainFrameVariables::ID::THREAD_EXPOSURE_PROGRESS, cMain::UpdateExposureProgress)
EVT_THREAD(MainFrameVariables::ID::THREAD_EXPOSURE_FINISHED, cMain::FinishExposureProgress)
EVT_THREAD(MainFrameVariables::ID::THREAD_MOTOR_MOVE_FINISHED, cMain::OnMotorMoveFinished)
wxEND_EVENT_TABLE()

cMain::cMain(const wxString& title_)
	: wxFrame(NULL, wxID_ANY, title_), m_AppName(title_)
{
	wxArtProvider::Push(new wxMaterialDesignArtProvider);

	AppLog::Initialize
	(
		std::filesystem::path(wxStandardPaths::Get().GetUserLocalDataDir().ToStdWstring()) / L"logs"
	);
	AppLog::Info
	(
		"App",
		"VEXOTControl " + std::to_string(MAJOR_VERSION) + "." + std::to_string(MINOR_VERSION) +
		" started. Settings file: " + GetInitializationFilePath().utf8_string()
	);

	if (!LoadAndMigrateConfigurationEarly())
	{
		wxLogError
		(
			"Application configuration could not be loaded. "
			"Default hardware settings will be used."
		);
	}

	CreateMainFrame();
	InitDefaultStateWidgets();

	SetIcon(logo_xpm);
	SetMinSize(FromDIP(wxSize(800, 600)));

	InitializeAppearanceFromSystemAndConfig();

	RestoreWindowLayout();
	UpdateStatusFields();

	// Remember the normal (not maximized) size for the next start.
	Bind(wxEVT_SIZE, [this](wxSizeEvent& e)
		{
			if (!IsIconized())
				m_WasMaximized = IsMaximized();

			if (!IsMaximized() && !IsIconized() && IsShown())
				m_NormalWindowRect = GetRect();
			e.Skip();
		});
	Bind(wxEVT_MOVE, [this](wxMoveEvent& e)
		{
			if (!IsMaximized() && !IsIconized() && IsShown())
				m_NormalWindowRect = GetRect();
			e.Skip();
		});

	// Settings button
	{
		wxCommandEvent art_evt(wxEVT_MENU, MainFrameVariables::ID::MENUBAR_EDIT_SETTINGS);
		ProcessEvent(art_evt);
	}

#ifdef OPEN_DATA
	{
		wxCommandEvent evt(wxEVT_MENU, MainFrameVariables::ID::MENUBAR_FILE_OPEN);
		ProcessEvent(evt);
	}
#endif // OPEN_DATA

#ifdef _DEBUG
	{
		wxCommandEvent evt(wxEVT_BUTTON, MainFrameVariables::ID::RIGHT_MT_OUT_FLD_BTN);
		ProcessEvent(evt);
	}
#endif // _DEBUG

#ifdef _DEBUG
	{
		m_FirstStage->stage->SetSelection(1);
		wxCommandEvent evt(wxEVT_CHOICE, MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_CHOICE);
		ProcessEvent(evt);

		m_FirstStage->step->SetValue("0.100");
		m_FirstStage->finish->SetValue("1.000");
	}
#endif // _DEBUG


	{
		//m_StartStopLiveCapturingTglBtn->SetValue(true);
		//wxCommandEvent art_start_live_capturing(wxEVT_TOGGLEBUTTON, MainFrameVariables::ID_RIGHT_CAM_START_STOP_LIVE_CAPTURING_TGL_BTN);
		//ProcessEvent(art_start_live_capturing);
	}
}

void cMain::CreateMainFrame()
{
	InitComponents();
	CreateMenuBarOnFrame();
	CreateStatusBar();

	CreateLeftAndRightSide();
}

void cMain::InitComponents()
{
	m_Settings = std::make_unique<cSettings>
		(
			this,
			m_DefaultWorkStation,
			m_DefaultMotorsIPAddress
		);

	m_FirstStage =
		std::make_unique<MainFrameVariables::MeasurementStage>();

#ifdef USE_2_AXIS_MEASUREMENT
	m_SecondStage =
		std::make_unique<MainFrameVariables::MeasurementStage>();
#endif
}

void cMain::CreateMenuBarOnFrame()
{
	m_MenuBar = std::make_unique<MainFrameVariables::MenuBar>();
	SetMenuBar(m_MenuBar->menu_bar);

	// DIP sizes: these become bitmap bundles, which wx scales per DPI.
	auto bmpSize = wxSize(16, 16);

	auto isDark = wxSystemSettings::GetAppearance().IsDark();
	m_DefaultCellColor = m_DefaultAppearanceColor;
	m_DefaultCellColor = isDark ?
		wxColour(255 - m_DefaultCellColor.GetRed(), 255 - m_DefaultCellColor.GetGreen(), 255 - m_DefaultCellColor.GetBlue()) :
		m_DefaultCellColor;

	auto color = isDark ? wxColour(200, 200, 200) : wxColour(0, 0, 0);

	// File Menu
	{
		// Open
		{
			auto itemID = MainFrameVariables::MENUBAR_FILE_OPEN;
			auto item = new wxMenuItem(m_MenuBar->menu_file, itemID, wxT("Open\tCtrl+O"));

			{
				wxVector<wxBitmap> bitmaps;
				auto bitmap = wxART_FILE_OPEN;
				auto client = wxART_CLIENT_MATERIAL_ROUND;

				for (auto i = 0; i < 3; ++i)
					bitmaps.push_back(
						wxMaterialDesignArtProvider::GetBitmap
						(
							bitmap,
							client,
							wxSize(bmpSize.GetWidth() + i * bmpSize.GetWidth(), bmpSize.GetHeight() + i * bmpSize.GetHeight()),
							color
						)
					);

				item->SetBitmap(wxBitmapBundle::FromBitmaps(bitmaps));
			}

			m_MenuBar->menu_file->Append(item);
		}

		// Exit
		{
			auto itemID = MainFrameVariables::MENUBAR_FILE_QUIT;
			auto item = new wxMenuItem(m_MenuBar->menu_file, itemID, wxT("Quit\tCtrl+Q"));

			{
				wxVector<wxBitmap> bitmaps;
				auto bitmap = wxART_CLOSE;
				auto client = wxART_CLIENT_MATERIAL_ROUND;

				for (auto i = 0; i < 3; ++i)
					bitmaps.push_back(
						wxMaterialDesignArtProvider::GetBitmap
						(
							bitmap,
							client,
							wxSize(bmpSize.GetWidth() + i * bmpSize.GetWidth(), bmpSize.GetHeight() + i * bmpSize.GetHeight()),
							color
						)
					);

				item->SetBitmap(wxBitmapBundle::FromBitmaps(bitmaps));
			}

			m_MenuBar->menu_file->Append(item);
		}
	}

	// Append File Menu to the Menu Bar
	m_MenuBar->menu_bar->Append(m_MenuBar->menu_file, wxT("&File"));

	// Edit Menu
	{
		// Single Shot
		{
			auto itemID = MainFrameVariables::RIGHT_DEVICE_SINGLE_SHOT_BTN;
			auto item = new wxMenuItem(m_MenuBar->menu_edit, itemID, wxT("Single Shot\tCtrl+Shift+S"));

			{
				wxVector<wxBitmap> bitmaps;
				auto bitmap = wxART_PLUS_ONE;
				auto client = wxART_CLIENT_MATERIAL_ROUND;

				for (auto i = 0; i < 3; ++i)
					bitmaps.push_back(
						wxMaterialDesignArtProvider::GetBitmap
						(
							bitmap,
							client,
							wxSize(bmpSize.GetWidth() + i * bmpSize.GetWidth(), bmpSize.GetHeight() + i * bmpSize.GetHeight()),
							color
						)
					);

				item->SetBitmap(wxBitmapBundle::FromBitmaps(bitmaps));
			}

			m_MenuBar->menu_edit->Append(item);
			m_MenuBar->menu_edit->Enable(itemID, false);
		}

		m_MenuBar->menu_edit->AppendCheckItem(MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN, wxT("Start/Stop Live Capturing\tCtrl+Shift+L"));
		m_MenuBar->menu_edit->Enable(MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN, false);

		m_MenuBar->menu_edit->AppendCheckItem(MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN, wxT("Start/Stop Measurement\tCtrl+Shift+M"));
		m_MenuBar->menu_edit->Enable(MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN, false);

		// Settings
		{
			auto itemID = MainFrameVariables::MENUBAR_EDIT_SETTINGS;
			auto item = new wxMenuItem(m_MenuBar->menu_edit, itemID, wxT("Settings\tF6"));

			{
				wxVector<wxBitmap> bitmaps;
				auto bitmap = wxART_SETTINGS;
				auto client = wxART_CLIENT_FLUENTUI_FILLED;

				for (auto i = 0; i < 3; ++i)
					bitmaps.push_back(
						wxMaterialDesignArtProvider::GetBitmap
						(
							bitmap,
							client,
							wxSize(bmpSize.GetWidth() + i * bmpSize.GetWidth(), bmpSize.GetHeight() + i * bmpSize.GetHeight()),
							color
						)
					);

				item->SetBitmap(wxBitmapBundle::FromBitmaps(bitmaps));
			}

			m_MenuBar->menu_edit->Append(item);
		}
	}

	// Append Edit Menu to the Menu Bar
	m_MenuBar->menu_bar->Append(m_MenuBar->menu_edit, wxT("&Edit"));

	m_MenuBar->menu_tools->Append(MainFrameVariables::ID::MENUBAR_TOOLS_CURSOR_OVERLAY, wxT("Cursor Overlay\tCtrl+Shift+V"), wxEmptyString, wxITEM_CHECK);
	m_MenuBar->menu_bar->Append(m_MenuBar->menu_tools, wxT("&Tools"));
	// Window Menu
	{
		// Full Screen
		{
			auto itemID = MainFrameVariables::MENUBAR_WINDOW_FULLSCREEN;
			auto item = new wxMenuItem(m_MenuBar->menu_window, itemID, wxT("Full Screen Mode\tF11"));

			{
				wxVector<wxBitmap> bitmaps;
				auto bitmap = wxART_FULLSCREEN;
				auto client = wxART_CLIENT_MATERIAL_ROUND;

				for (auto i = 0; i < 3; ++i)
					bitmaps.push_back(
						wxMaterialDesignArtProvider::GetBitmap
						(
							bitmap,
							client,
							wxSize(bmpSize.GetWidth() + i * bmpSize.GetWidth(), bmpSize.GetHeight() + i * bmpSize.GetHeight()),
							color
						)
					);

				item->SetBitmap(wxBitmapBundle::FromBitmaps(bitmaps));
			}

			m_MenuBar->menu_window->Append(item);
		}

		m_MenuBar->menu_window->AppendCheckItem(MainFrameVariables::ID::MENUBAR_WINDOW_ENABLE_DARK_MODE, wxT("Dark Mode"));

	}

	// Append Window Menu to the Menu Bar
	m_MenuBar->menu_bar->Append(m_MenuBar->menu_window, wxT("&Window"));

	// Help Menu
	{
		// About
		{
			auto itemID = MainFrameVariables::MENUBAR_HELP_ABOUT;
			auto title = wxT("About ") + m_AppName + wxT("\tF1");
			auto item = new wxMenuItem(m_MenuBar->menu_help, itemID, title);

			{
				wxVector<wxBitmap> bitmaps;
				auto bitmap = wxART_BOOK_QUESTION_MARK;
				auto client = wxART_CLIENT_FLUENTUI_FILLED;

				for (auto i = 0; i < 3; ++i)
					bitmaps.push_back(
						wxMaterialDesignArtProvider::GetBitmap
						(
							bitmap,
							client,
							wxSize(bmpSize.GetWidth() + i * bmpSize.GetWidth(), bmpSize.GetHeight() + i * bmpSize.GetHeight()),
							color
						)
					);

				item->SetBitmap(wxBitmapBundle::FromBitmaps(bitmaps));
			}

			m_MenuBar->menu_help->Append(item);
		}

		// Application Version
		{
			auto id = MainFrameVariables::ID::MENUBAR_HELP_APPS_VERSION;
			wxMenuItem* item = new wxMenuItem
			(
				m_MenuBar->menu_help,
				id,
				"Application Version\tF2"
			);

			// Setting a bitmap to the Close menu item
			{
				wxVector<wxBitmap> bitmaps;
				auto startSize = 16;
				auto bitmap = wxART_INFO;
				auto client = wxART_CLIENT_MATERIAL_ROUND;

				for (auto i{ 0 }; i < 3; ++i)
					bitmaps.push_back
					(
						wxMaterialDesignArtProvider::GetBitmap
						(
							bitmap,
							client,
							wxSize(startSize + i * startSize, startSize + i * startSize),
							color
						)
					);

				item->SetBitmap(wxBitmapBundle::FromBitmaps(bitmaps));
			}
			m_MenuBar->menu_help->Append(item);
		}

		// Open Log Folder
		{
			m_MenuBar->menu_help->AppendSeparator();
			m_MenuBar->menu_help->Append
			(
				MainFrameVariables::ID::MENUBAR_HELP_OPEN_LOG_FOLDER,
				"Open Log Folder",
				"Show the folder with the application log files"
			);
		}
	}

	m_MenuBar->menu_bar->Append(m_MenuBar->menu_help, wxT("&Help"));
}

void cMain::InitDefaultStateWidgets()
{
	m_MenuBar->menu_tools->Check(MainFrameVariables::ID::MENUBAR_TOOLS_CURSOR_OVERLAY, true);
	m_PreviewPanel->SetCursorOverlayActive(true);
	m_IsCursorOverlayActive = true;

	float default_absolute_value{ 0.0f }, default_relative_value{ 1.0f }, default_relative_value_pitch_yaw{ 0.1f };
	/* Default Detector Widgets */
	{
		/* X */
		{
			m_Detector[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Detector[0].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
		}
		/* Y */
		{
			m_Detector[1].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Detector[1].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
		}
	}

	/* Default Optics Widgets */
	{
		/* X */
		{
			m_Optics[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[0].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
		}
		/* Y */
		{
			m_Optics[1].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[1].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
		}
		/* Z */
		{
			m_Optics[2].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[2].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
		}

		/* Pitch */
		{
			m_Optics[3].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[3].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value_pitch_yaw));
		}
		/* Yaw */
		{
			m_Optics[4].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[4].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value_pitch_yaw));
		}
	}

	/* Default Aux Widgets */
	{
		m_Aux[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
		m_Aux[0].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
	}

	/* Disabling Measurement Controls */
	{
		//m_OutDirTextCtrl->Disable();
		//m_OutDirBtn->Disable();

		float default_start{ 0.0f }, default_step{ 1.0f }, default_finish{ 24.0f };

		/* First Stage */
		{
			m_FirstStage->start->ChangeValue(wxString::Format(wxT("%.3f"), default_start));
			m_FirstStage->step->ChangeValue(wxString::Format(wxT("%.3f"), default_step));
			m_FirstStage->finish->ChangeValue(wxString::Format(wxT("%.3f"), default_finish));
			m_FirstStage->EnableAllControls(false);
		}

#ifdef USE_2_AXIS_MEASUREMENT
		/* Second Stage */
		{
			m_SecondStage->start->ChangeValue(wxString::Format(wxT("%.3f"), default_start));
			m_SecondStage->step->ChangeValue(wxString::Format(wxT("%.3f"), default_step));
			m_SecondStage->finish->ChangeValue(wxString::Format(wxT("%.3f"), default_finish));
			m_SecondStage->DisableAllControls();
		}
#endif

		/* Start Capturing */
		m_StartStopMeasurementTglBtn->Disable();
	}
}

void cMain::CreateLeftAndRightSide()
{
	int height_left_and_right_panels{ 600 };

	m_MainSplitter = new wxSplitterWindow(
		this,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxSP_LIVE_UPDATE | wxSP_BORDER
	);

	m_LeftSidePanel = new wxPanel(m_MainSplitter);
	m_RightSidePanel = new wxScrolledWindow(
		m_MainSplitter,
		wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		wxVSCROLL | wxHSCROLL
	);

	m_MainSplitter->SplitVertically(m_LeftSidePanel, m_RightSidePanel);
	m_MainSplitter->SetSashGravity(1.0);

	wxBoxSizer* left_sizer = new wxBoxSizer(wxHORIZONTAL);
	wxBoxSizer* right_sizer = new wxBoxSizer(wxVERTICAL);

	const wxSize sizeOfPreviewWindow = FromDIP(wxSize(600, height_left_and_right_panels));
	const wxSize sizeOfRightSide = FromDIP(wxSize(350, height_left_and_right_panels));

	left_sizer->SetMinSize(sizeOfPreviewWindow);
	right_sizer->SetMinSize(sizeOfRightSide);

	m_LeftSidePanel->SetMinSize(sizeOfPreviewWindow);
	m_RightSidePanel->SetMinSize(sizeOfRightSide);

	CreateLeftSide(m_LeftSidePanel, left_sizer);
	CreateRightSide(m_RightSidePanel, right_sizer);

	m_LeftSidePanel->SetSizer(left_sizer);

	m_RightSidePanel->SetSizer(right_sizer);
	m_RightSidePanel->SetScrollRate(10, 10);
	m_RightSidePanel->FitInside();

	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	main_sizer->Add(m_MainSplitter, 1, wxEXPAND);

	SetSizer(main_sizer);
	Layout();
}

auto cMain::CreateLeftSide(wxWindow* parent, wxSizer* sizer) -> void
{
	auto input_args = std::make_unique<PreviewPanelVariables::InputPreviewPanelArgs>
		(
			m_StatusBar.get(),
			m_MinRangeKEVTxtCtrl.get(),
			m_MaxRangeKEVTxtCtrl.get()
		);

	m_PreviewPanel = std::make_unique<cPreviewPanel>
		(
			parent,
			sizer,
			std::move(input_args)
		);
}

auto cMain::CreateRightSide(wxWindow* parent, wxSizer* sizer) -> void
{
	m_MotorControlsContainer = new wxPanel(
		parent,
		wxID_ANY
	);

	m_DeviceMeasurementContainer = new wxPanel(
		parent,
		wxID_ANY
	);

	auto* motorSizer = new wxBoxSizer(wxVERTICAL);
	auto* deviceMeasurementSizer = new wxBoxSizer(wxVERTICAL);

	CreateSteppersControl(
		m_MotorControlsContainer,
		motorSizer
	);

	CreateDeviceControls(
		m_DeviceMeasurementContainer,
		deviceMeasurementSizer
	);

	CreateMeasurement(
		m_DeviceMeasurementContainer,
		deviceMeasurementSizer
	);

	m_MotorControlsContainer->SetSizer(motorSizer);
	m_DeviceMeasurementContainer->SetSizer(deviceMeasurementSizer);

	// Both sections retain their complete best height.
	// m_RightSidePanel handles scrolling when they do not fit on screen.
	sizer->Add(
		m_MotorControlsContainer,
		0,
		wxEXPAND
	);

	sizer->Add(
		m_DeviceMeasurementContainer,
		0,
		wxEXPAND
	);
}

auto cMain::CreateSteppersControl(wxWindow* parent, wxSizer* sizer) -> void
{
	const wxSize absTxtCtrlSize = FromDIP(wxSize(54, 20)), relTxtCtrlSize = absTxtCtrlSize;
	const wxSize setBtnSize = FromDIP(wxSize(35, 20));
	const wxSize incDecSize = FromDIP(wxSize(20, 20));

	auto size = FromDIP(wxSize(16, 16));
	auto imageListDetector = new wxImageList(size.GetWidth(), size.GetHeight(), true);
	auto imageListOptics = new wxImageList(size.GetWidth(), size.GetHeight(), true);
	auto imageListAux = new wxImageList(size.GetWidth(), size.GetHeight(), true);

	/* Set bitmap */
	wxBitmap setBitmap{};
	{
		auto bitmap = wxART_FLAG;
		auto client = wxART_CLIENT_FLUENTUI_FILLED;
		auto color = wxColour(255, 128, 128);
		setBitmap = wxMaterialDesignArtProvider::GetBitmap
		(
			bitmap,
			client,
			size,
			color
		);
	}

	/* Center bitmap */
	wxBitmap centerBitmap{};
	{
		auto bitmap = wxART_CENTER_HORIZONTAL;
		auto client = wxART_CLIENT_FLUENTUI_FILLED;
		auto color = wxColour(255, 128, 0);
		centerBitmap = wxMaterialDesignArtProvider::GetBitmap
		(
			bitmap,
			client,
			size,
			color
		);
	}

	/* Home bitmap */
	wxBitmap homeBitmap{};
	{
		auto bitmap = wxART_HOME;
		auto client = wxART_CLIENT_MATERIAL_FILLED;
		auto color = wxColour(0, 255, 128);
		homeBitmap = wxMaterialDesignArtProvider::GetBitmap
		(
			bitmap,
			client,
			size,
			color
		);
	}

	// These are drawn at physical pixels (size is FromDIP'ed). Say so, or wx
	// treats them as DIP-sized and scales them up a second time.
	for (wxBitmap* bitmap : { &setBitmap, &centerBitmap, &homeBitmap })
		bitmap->SetScaleFactor(GetDPIScaleFactor());

	int detectorImgIndex{};

	/* Detector bitmap */
	{
		auto bitmap = wxART_CAMERA;
		auto client = wxART_CLIENT_MATERIAL_FILLED;
		auto color = wxColour(128, 0, 255);

		auto bmp = wxMaterialDesignArtProvider::GetBitmap
		(
			bitmap,
			client,
			size,
			color
		);

		detectorImgIndex = imageListDetector->Add(bmp);
	}

	m_MotorControlsBook = new wxSimplebook
	(
		parent,
		wxID_ANY
	);

	m_NativeMotorControlsPage = new wxPanel(
		m_MotorControlsBook,
		wxID_ANY
	);

	m_MotorWebPage = new wxPanel(
		m_MotorControlsBook,
		wxID_ANY
	);

	auto* nativeMotorSizer = new wxBoxSizer(wxVERTICAL);
	auto* webMotorSizer = new wxBoxSizer(wxVERTICAL);

	m_DetectorControlsNotebook = new wxNotebook(m_NativeMotorControlsPage, wxID_ANY);

	m_DetectorControlsNotebook->AssignImageList(imageListDetector);

	m_DetectorControlsNotebook->AddPage
	(
		CreateDetectorPage
		(
			m_DetectorControlsNotebook,
			absTxtCtrlSize,
			relTxtCtrlSize,
			setBtnSize,
			incDecSize,
			setBitmap,
			centerBitmap,
			homeBitmap
		),
		"Detector",
#ifdef _DEBUG
		true,
#else
		true,
#endif // _DEBUG
		detectorImgIndex
	);

#ifndef _DEBUG
	m_DetectorControlsNotebook->Hide();
#endif // !_DEBUG

	nativeMotorSizer->Add(m_DetectorControlsNotebook, 0, wxEXPAND | wxALL, 5);

	int opticsImgIndex{};

	/* Optics bitmap */
	{
		auto bitmap = wxART_CIRCLE_HINT;
		auto client = wxART_CLIENT_FLUENTUI_FILLED;
		auto color = wxColour(255, 128, 128);
		auto bmp = wxMaterialDesignArtProvider::GetBitmap
		(
			bitmap,
			client,
			size,
			color
		);

		opticsImgIndex = imageListOptics->Add(bmp);
	}

	m_OpticsControlsNotebook = new wxNotebook(m_NativeMotorControlsPage, wxID_ANY);

	m_OpticsControlsNotebook->AssignImageList(imageListOptics);

	m_OpticsControlsNotebook->AddPage
	(
		CreateOpticsPage
		(
			m_OpticsControlsNotebook,
			absTxtCtrlSize,
			relTxtCtrlSize,
			setBtnSize,
			incDecSize,
			setBitmap,
			centerBitmap,
			homeBitmap
		),
		"Optics",
#ifdef _DEBUG
		true,
#else
		true,
#endif // _DEBUG
		opticsImgIndex
	);

#ifndef _DEBUG
	m_OpticsControlsNotebook->Hide();
#endif // !_DEBUG

	nativeMotorSizer->Add(m_OpticsControlsNotebook, 0, wxEXPAND | wxALL, 5);

	int auxImgIndex{};

	{
		auto bitmap = wxART_CYCLONE;
		auto client = wxART_CLIENT_MATERIAL_ROUND;
		auto color = wxColour(163, 73, 164);

		auto bmp = wxMaterialDesignArtProvider::GetBitmap(bitmap, client, size, color);
		auxImgIndex = imageListAux->Add(bmp);
	}

	m_AuxControlsNotebook = new wxNotebook(m_NativeMotorControlsPage, wxID_ANY);

	m_AuxControlsNotebook->AssignImageList(imageListAux);

	m_AuxControlsNotebook->AddPage
	(
		CreateAuxPage
		(
			m_AuxControlsNotebook,
			absTxtCtrlSize,
			relTxtCtrlSize,
			setBtnSize,
			incDecSize,
			setBitmap,
			centerBitmap,
			homeBitmap
		),
		"Aux",
		true,
		auxImgIndex
	);

#ifndef _DEBUG
	m_AuxControlsNotebook->Hide();
#endif

	nativeMotorSizer->Add(m_AuxControlsNotebook, 0, wxEXPAND | wxALL, 5);

	m_NativeMotorControlsPage->SetSizer(nativeMotorSizer);

	m_MotorsWebView = wxWebView::New
	(
		m_MotorWebPage,
		wxID_ANY,
		wxWebViewDefaultURLStr
	);

	if (m_MotorsWebView)
	{
		webMotorSizer->Add(
			m_MotorsWebView,
			1,
			wxEXPAND
		);
	}

	m_MotorWebPage->SetSizer(webMotorSizer);

	m_MotorControlsBook->AddPage(
		m_NativeMotorControlsPage,
		"Native Motor Controls",
		true
	);

	m_MotorControlsBook->AddPage(
		m_MotorWebPage,
		"Motor Web Interface",
		false
	);

	sizer->Add(
		m_MotorControlsBook,
		1,
		wxEXPAND
	);

	/* Stop - lives on the outer sizer, below the simplebook, so it's
	   visible and clickable regardless of whether the Native or Web page
	   is currently shown. Always enabled; interrupts whatever is moving
	   rather than requiring a specific axis to be selected. */
	m_StopMotorsBtn = new wxButton
	(
		parent,
		MainFrameVariables::ID::RIGHT_SC_STOP_ALL_BTN,
		"STOP",
		wxDefaultPosition,
		FromDIP(wxSize(-1, 48))
	);
	m_StopMotorsBtn->SetBackgroundColour(wxColour(200, 30, 30));
	m_StopMotorsBtn->SetForegroundColour(*wxWHITE);
	{
		wxFont stopFont = m_StopMotorsBtn->GetFont();
		stopFont.SetPointSize(stopFont.GetPointSize() + 4);
		stopFont.SetWeight(wxFONTWEIGHT_BOLD);
		m_StopMotorsBtn->SetFont(stopFont);
	}

	sizer->Add(m_StopMotorsBtn, 0, wxEXPAND | wxALL, 5);
}

auto cMain::CreateDetectorPage
(
	wxWindow* parent,
	const wxSize& absoluteTxtCtrlSize,
	const wxSize& relativeTxtCtrlSize,
	const wxSize& setBtnSize,
	const wxSize& incrementDecrementBtnSize,
	const wxBitmap& setBitmap,
	const wxBitmap& centerBitmap,
	const wxBitmap& homeBitmap
) -> wxWindow*
{
	wxPanel* page = new wxPanel(parent);
	wxSizer* sizerPage = new wxBoxSizer(wxVERTICAL);

	{
		/* Detector X */
		wxSizer* const x_detector = new wxStaticBoxSizer(wxHORIZONTAL, page, "&X");
		x_detector->AddStretchSpacer();
		{
			/* Absolute */
			{
				wxSizer* const abs_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Absolute [mm]");
				wxFloatingPointValidator<float>	abs_val(3, NULL, wxNUM_VAL_DEFAULT);
				abs_val.SetMin(-1000.f);
				abs_val.SetMax(1000.f);
				m_Detector[0].absolute_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_X_ABS_TE_CTL,
					wxT("123.456"),
					wxDefaultPosition,
					absoluteTxtCtrlSize,
					wxTE_CENTRE | wxTE_PROCESS_ENTER,
					abs_val
				);

				m_Detector[0].set_btn = new wxBitmapButton
				(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_X_SET_BTN,
					setBitmap
				);

				m_Detector[0].set_btn->SetMinSize(setBtnSize);
				m_Detector[0].set_btn->SetToolTip(wxT("Set the absolute motor position"));

				abs_sizer->Add(m_Detector[0].absolute_text_ctrl, 0, wxALIGN_CENTER);
				abs_sizer->Add(m_Detector[0].set_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				x_detector->Add(abs_sizer, 0, wxALIGN_CENTER);
			}

			/* Relative */
			{
				m_Detector[0].decrement_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_X_DEC_BTN,
					wxT("-"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Detector[0].decrement_btn->SetToolTip(wxT("Decrement distance"));
				wxFloatingPointValidator<float>	rel_val(3, NULL, wxNUM_VAL_DEFAULT);
				rel_val.SetMin(-1000.f);
				rel_val.SetMax(1000.f);
				m_Detector[0].relative_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_X_REL_TE_CTL,
					wxT("789.123"),
					wxDefaultPosition,
					relativeTxtCtrlSize,
					wxTE_CENTRE,
					rel_val
				);
				m_Detector[0].increment_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_X_INC_BTN,
					wxT("+"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Detector[0].increment_btn->SetToolTip(wxT("Increment distance"));

				wxSizer* const rel_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Relative [mm]");
				rel_sizer->Add(m_Detector[0].decrement_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				rel_sizer->Add(m_Detector[0].relative_text_ctrl, 0, wxALIGN_CENTER);
				rel_sizer->Add(m_Detector[0].increment_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				x_detector->Add(rel_sizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 2);
			}

			/* Global positioning controls */
			{
				m_Detector[0].center_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_X_CENTER_BTN,
					centerBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Detector[0].center_btn->SetToolTip(wxT("Go to the center position of motor"));
				m_Detector[0].home_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_X_HOME_BTN,
					homeBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Detector[0].home_btn->SetToolTip(wxT("Go to the home position of motor"));

				wxSizer* const jump_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Jump");
				jump_sizer->Add(m_Detector[0].center_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				jump_sizer->Add(m_Detector[0].home_btn, 0, wxALIGN_CENTER);

				x_detector->Add(jump_sizer, 0, wxALIGN_CENTER);
			}
		}
		x_detector->AddStretchSpacer();
		sizerPage->Add(x_detector, 0, wxEXPAND);

		/* Detector Y */
		wxSizer* const y_detector = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Y");
		y_detector->AddStretchSpacer();
		{
			/* Absolute */
			{
				wxSizer* const abs_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Absolute [mm]");
				wxFloatingPointValidator<float>	abs_val(3, NULL, wxNUM_VAL_DEFAULT);
				abs_val.SetMin(-1000.f);
				abs_val.SetMax(1000.f);
				m_Detector[1].absolute_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_Y_ABS_TE_CTL,
					wxT("123.456"),
					wxDefaultPosition,
					absoluteTxtCtrlSize,
					wxTE_CENTRE | wxTE_PROCESS_ENTER,
					abs_val
				);

				m_Detector[1].set_btn = new wxBitmapButton
				(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_Y_SET_BTN,
					setBitmap
				);

				m_Detector[1].set_btn->SetMinSize(setBtnSize);
				m_Detector[1].set_btn->SetToolTip(wxT("Set the absolute motor position"));

				abs_sizer->Add(m_Detector[1].absolute_text_ctrl, 0, wxALIGN_CENTER);
				abs_sizer->Add(m_Detector[1].set_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				y_detector->Add(abs_sizer, 0, wxALIGN_CENTER);
			}

			/* Relative */
			{
				m_Detector[1].decrement_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_Y_DEC_BTN,
					wxT("-"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Detector[1].decrement_btn->SetToolTip(wxT("Decrement distance"));
				wxFloatingPointValidator<float>	rel_val(3, NULL, wxNUM_VAL_DEFAULT);
				rel_val.SetMin(-1000.f);
				rel_val.SetMax(1000.f);
				m_Detector[1].relative_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_Y_REL_TE_CTL,
					wxT("789.123"),
					wxDefaultPosition,
					relativeTxtCtrlSize,
					wxTE_CENTRE,
					rel_val
				);
				m_Detector[1].increment_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_Y_INC_BTN,
					wxT("+"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Detector[1].increment_btn->SetToolTip(wxT("Increment distance"));

				wxSizer* const rel_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Relative [mm]");
				rel_sizer->Add(m_Detector[1].decrement_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				rel_sizer->Add(m_Detector[1].relative_text_ctrl, 0, wxALIGN_CENTER);
				rel_sizer->Add(m_Detector[1].increment_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				y_detector->Add(rel_sizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 2);
			}

			/* Global positioning controls */
			{
				m_Detector[1].center_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_Y_CENTER_BTN,
					centerBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Detector[1].center_btn->SetToolTip(wxT("Go to the center position of motor"));
				m_Detector[1].home_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_DET_Y_HOME_BTN,
					homeBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Detector[1].home_btn->SetToolTip(wxT("Go to the home position of motor"));
				wxSizer* const jump_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Jump");
				jump_sizer->Add(m_Detector[1].center_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				jump_sizer->Add(m_Detector[1].home_btn, 0, wxALIGN_CENTER);

				y_detector->Add(jump_sizer, 0, wxALIGN_CENTER);
			}
		}
		y_detector->AddStretchSpacer();
		sizerPage->Add(y_detector, 0, wxEXPAND);
	}

	page->SetSizer(sizerPage);
	return page;
}

auto cMain::CreateOpticsPage
(
	wxWindow* parent,
	const wxSize& absoluteTxtCtrlSize,
	const wxSize& relativeTxtCtrlSize,
	const wxSize& setBtnSize,
	const wxSize& incrementDecrementBtnSize,
	const wxBitmap& setBitmap,
	const wxBitmap& centerBitmap,
	const wxBitmap& homeBitmap
) -> wxWindow*
{
	wxPanel* page = new wxPanel(parent);
	wxSizer* sizerPage = new wxBoxSizer(wxVERTICAL);

	{
		/* Optics X */
		wxSizer* const x_optics = new wxStaticBoxSizer(wxHORIZONTAL, page, "&X");
		x_optics->AddStretchSpacer();
		{
			/* Absolute */
			{
				wxSizer* const abs_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Absolute [mm]");
				wxFloatingPointValidator<float>	abs_val(3, NULL, wxNUM_VAL_DEFAULT);
				abs_val.SetMin(-1000.f);
				abs_val.SetMax(1000.f);
				m_Optics[0].absolute_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_X_ABS_TE_CTL,
					wxT("123.456"),
					wxDefaultPosition,
					absoluteTxtCtrlSize,
					wxTE_CENTRE | wxTE_PROCESS_ENTER,
					abs_val
				);

				m_Optics[0].set_btn = new wxBitmapButton
				(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_X_SET_BTN,
					setBitmap
				);

				m_Optics[0].set_btn->SetMinSize(setBtnSize);

				m_Optics[0].set_btn->SetToolTip(wxT("Set the absolute motor position"));

				abs_sizer->Add(m_Optics[0].absolute_text_ctrl, 0, wxALIGN_CENTER);
				abs_sizer->Add(m_Optics[0].set_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				x_optics->Add(abs_sizer, 0, wxALIGN_CENTER);
			}

			/* Relative */
			{
				m_Optics[0].decrement_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_X_DEC_BTN,
					wxT("-"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[0].decrement_btn->SetToolTip(wxT("Decrement distance"));
				wxFloatingPointValidator<float>	rel_val(3, NULL, wxNUM_VAL_DEFAULT);
				rel_val.SetMin(-1000.f);
				rel_val.SetMax(1000.f);
				m_Optics[0].relative_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_X_REL_TE_CTL,
					wxT("789.123"),
					wxDefaultPosition,
					relativeTxtCtrlSize,
					wxTE_CENTRE,
					rel_val
				);
				m_Optics[0].increment_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_X_INC_BTN,
					wxT("+"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[0].increment_btn->SetToolTip(wxT("Increment distance"));

				wxSizer* const rel_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Relative [mm]");
				rel_sizer->Add(m_Optics[0].decrement_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				rel_sizer->Add(m_Optics[0].relative_text_ctrl, 0, wxALIGN_CENTER);
				rel_sizer->Add(m_Optics[0].increment_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				x_optics->Add(rel_sizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 2);
			}

			/* Global positioning controls */
			{
				m_Optics[0].center_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_X_CENTER_BTN,
					centerBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[0].center_btn->SetToolTip(wxT("Go to the center position of motor"));
				m_Optics[0].home_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_X_HOME_BTN,
					homeBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[0].home_btn->SetToolTip(wxT("Go to the home position of motor"));

				wxSizer* const jump_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Jump");
				jump_sizer->Add(m_Optics[0].center_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				jump_sizer->Add(m_Optics[0].home_btn, 0, wxALIGN_CENTER);

				x_optics->Add(jump_sizer, 0, wxALIGN_CENTER);
			}
		}
		x_optics->AddStretchSpacer();
		sizerPage->Add(x_optics, 0, wxEXPAND);

		/* Optics Y */
		wxSizer* const y_optics = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Y");
		y_optics->AddStretchSpacer();
		{
			/* Absolute */
			{
				wxSizer* const abs_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Absolute [mm]");
				wxFloatingPointValidator<float>	abs_val(3, NULL, wxNUM_VAL_DEFAULT);
				abs_val.SetMin(-1000.f);
				abs_val.SetMax(1000.f);
				m_Optics[1].absolute_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Y_ABS_TE_CTL,
					wxT("123.456"),
					wxDefaultPosition,
					absoluteTxtCtrlSize,
					wxTE_CENTRE | wxTE_PROCESS_ENTER,
					abs_val
				);

				m_Optics[1].set_btn = new wxBitmapButton
				(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Y_SET_BTN,
					setBitmap
				);

				m_Optics[1].set_btn->SetMinSize(setBtnSize);

				m_Optics[1].set_btn->SetToolTip(wxT("Set the absolute motor position"));

				abs_sizer->Add(m_Optics[1].absolute_text_ctrl, 0, wxALIGN_CENTER);
				abs_sizer->Add(m_Optics[1].set_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				y_optics->Add(abs_sizer, 0, wxALIGN_CENTER);
			}

			/* Relative */
			{
				m_Optics[1].decrement_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Y_DEC_BTN,
					wxT("-"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[1].decrement_btn->SetToolTip(wxT("Decrement distance"));
				wxFloatingPointValidator<float>	rel_val(3, NULL, wxNUM_VAL_DEFAULT);
				rel_val.SetMin(-1000.f);
				rel_val.SetMax(1000.f);
				m_Optics[1].relative_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Y_REL_TE_CTL,
					wxT("789.123"),
					wxDefaultPosition,
					relativeTxtCtrlSize,
					wxTE_CENTRE,
					rel_val
				);
				m_Optics[1].increment_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Y_INC_BTN,
					wxT("+"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[1].increment_btn->SetToolTip(wxT("Increment distance"));

				wxSizer* const rel_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Relative [mm]");
				rel_sizer->Add(m_Optics[1].decrement_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				rel_sizer->Add(m_Optics[1].relative_text_ctrl, 0, wxALIGN_CENTER);
				rel_sizer->Add(m_Optics[1].increment_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				y_optics->Add(rel_sizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 2);
			}

			/* Global positioning controls */
			{
				m_Optics[1].center_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Y_CENTER_BTN,
					centerBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[1].center_btn->SetToolTip(wxT("Go to the center position of motor"));
				m_Optics[1].home_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Y_HOME_BTN,
					homeBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[1].home_btn->SetToolTip(wxT("Go to the home position of motor"));

				wxSizer* const jump_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Jump");
				jump_sizer->Add(m_Optics[1].center_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				jump_sizer->Add(m_Optics[1].home_btn, 0, wxALIGN_CENTER);

				y_optics->Add(jump_sizer, 0, wxALIGN_CENTER);
			}
		}
		y_optics->AddStretchSpacer();
		sizerPage->Add(y_optics, 0, wxEXPAND);

		/* Optics Z */
		wxSizer* const z_optics = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Z");
		z_optics->AddStretchSpacer();
		{
			/* Absolute */
			{
				wxSizer* const abs_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Absolute [mm]");
				wxFloatingPointValidator<float>	abs_val(3, NULL, wxNUM_VAL_DEFAULT);
				abs_val.SetMin(-1000.f);
				abs_val.SetMax(1000.f);
				m_Optics[2].absolute_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Z_ABS_TE_CTL,
					wxT("123.456"),
					wxDefaultPosition,
					absoluteTxtCtrlSize,
					wxTE_CENTRE | wxTE_PROCESS_ENTER,
					abs_val
				);

				m_Optics[2].set_btn = new wxBitmapButton
				(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Z_SET_BTN,
					setBitmap
				);

				m_Optics[2].set_btn->SetMinSize(setBtnSize);

				m_Optics[2].set_btn->SetToolTip(wxT("Set the absolute motor position"));

				abs_sizer->Add(m_Optics[2].absolute_text_ctrl, 0, wxALIGN_CENTER);
				abs_sizer->Add(m_Optics[2].set_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				z_optics->Add(abs_sizer, 0, wxALIGN_CENTER);
			}

			/* Relative */
			{
				m_Optics[2].decrement_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Z_DEC_BTN,
					wxT("-"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[2].decrement_btn->SetToolTip(wxT("Decrement distance"));
				wxFloatingPointValidator<float>	rel_val(3, NULL, wxNUM_VAL_DEFAULT);
				rel_val.SetMin(-1000.f);
				rel_val.SetMax(1000.f);
				m_Optics[2].relative_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Z_REL_TE_CTL,
					wxT("789.123"),
					wxDefaultPosition,
					relativeTxtCtrlSize,
					wxTE_CENTRE,
					rel_val
				);
				m_Optics[2].increment_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Z_INC_BTN,
					wxT("+"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[2].increment_btn->SetToolTip(wxT("Increment distance"));

				wxSizer* const rel_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Relative [mm]");
				rel_sizer->Add(m_Optics[2].decrement_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				rel_sizer->Add(m_Optics[2].relative_text_ctrl, 0, wxALIGN_CENTER);
				rel_sizer->Add(m_Optics[2].increment_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				z_optics->Add(rel_sizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 2);
			}

			/* Global positioning controls */
			{
				m_Optics[2].center_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Z_CENTER_BTN,
					centerBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[2].center_btn->SetToolTip(wxT("Go to the center position of motor"));
				m_Optics[2].home_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_Z_HOME_BTN,
					homeBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[2].home_btn->SetToolTip(wxT("Go to the home position of motor"));

				wxSizer* const jump_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Jump");
				jump_sizer->Add(m_Optics[2].center_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				jump_sizer->Add(m_Optics[2].home_btn, 0, wxALIGN_CENTER);

				z_optics->Add(jump_sizer, 0, wxALIGN_CENTER);
			}
		}
		z_optics->AddStretchSpacer();
		sizerPage->Add(z_optics, 0, wxEXPAND);

		/* Optics Pitch */
		wxSizer* const pitch_optics = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Pitch");
		pitch_optics->AddStretchSpacer();
		{
			/* Absolute */
			{
				wxSizer* const abs_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Absolute [mm]");
				wxFloatingPointValidator<float>	abs_val(3, NULL, wxNUM_VAL_DEFAULT);
				abs_val.SetMin(-1000.f);
				abs_val.SetMax(1000.f);
				m_Optics[3].absolute_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_ABS_TE_CTL,
					wxT("123.456"),
					wxDefaultPosition,
					absoluteTxtCtrlSize,
					wxTE_CENTRE | wxTE_PROCESS_ENTER,
					abs_val
				);

				m_Optics[3].set_btn = new wxBitmapButton
				(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_SET_BTN,
					setBitmap
				);

				m_Optics[3].set_btn->SetMinSize(setBtnSize);

				m_Optics[3].set_btn->SetToolTip(wxT("Set the absolute motor position"));

				abs_sizer->Add(m_Optics[3].absolute_text_ctrl, 0, wxALIGN_CENTER);
				abs_sizer->Add(m_Optics[3].set_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				pitch_optics->Add(abs_sizer, 0, wxALIGN_CENTER);
			}

			/* Relative */
			{
				m_Optics[3].decrement_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_DEC_BTN,
					wxT("-"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[3].decrement_btn->SetToolTip(wxT("Decrement distance"));
				wxFloatingPointValidator<float>	rel_val(3, NULL, wxNUM_VAL_DEFAULT);
				rel_val.SetMin(-1000.f);
				rel_val.SetMax(1000.f);
				m_Optics[3].relative_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_REL_TE_CTL,
					wxT("789.123"),
					wxDefaultPosition,
					relativeTxtCtrlSize,
					wxTE_CENTRE,
					rel_val
				);
				m_Optics[3].increment_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_INC_BTN,
					wxT("+"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[3].increment_btn->SetToolTip(wxT("Increment distance"));

				wxSizer* const rel_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Relative [mm]");
				rel_sizer->Add(m_Optics[3].decrement_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				rel_sizer->Add(m_Optics[3].relative_text_ctrl, 0, wxALIGN_CENTER);
				rel_sizer->Add(m_Optics[3].increment_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				pitch_optics->Add(rel_sizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 2);
			}

			/* Global positioning controls */
			{
				m_Optics[3].center_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_CENTER_BTN,
					centerBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[3].center_btn->SetToolTip(wxT("Go to the center position of motor"));
				m_Optics[3].home_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_HOME_BTN,
					homeBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[3].home_btn->SetToolTip(wxT("Go to the home position of motor"));

				wxSizer* const jump_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Jump");
				jump_sizer->Add(m_Optics[3].center_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				jump_sizer->Add(m_Optics[3].home_btn, 0, wxALIGN_CENTER);

				pitch_optics->Add(jump_sizer, 0, wxALIGN_CENTER);
			}
		}

		pitch_optics->AddStretchSpacer();
		sizerPage->Add(pitch_optics, 0, wxEXPAND);

		/* Optics Yaw */
		wxSizer* const yaw_optics = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Yaw");
		yaw_optics->AddStretchSpacer();
		{
			/* Absolute */
			{
				wxSizer* const abs_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Absolute [mm]");
				wxFloatingPointValidator<float>	abs_val(3, NULL, wxNUM_VAL_DEFAULT);
				abs_val.SetMin(-1000.f);
				abs_val.SetMax(1000.f);
				m_Optics[4].absolute_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_YAW_ABS_TE_CTL,
					wxT("123.456"),
					wxDefaultPosition,
					absoluteTxtCtrlSize,
					wxTE_CENTRE | wxTE_PROCESS_ENTER,
					abs_val
				);

				m_Optics[4].set_btn = new wxBitmapButton
				(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_YAW_SET_BTN,
					setBitmap
				);

				m_Optics[4].set_btn->SetMinSize(setBtnSize);

				m_Optics[4].set_btn->SetToolTip(wxT("Set the absolute motor position"));

				abs_sizer->Add(m_Optics[4].absolute_text_ctrl, 0, wxALIGN_CENTER);
				abs_sizer->Add(m_Optics[4].set_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				yaw_optics->Add(abs_sizer, 0, wxALIGN_CENTER);
			}

			/* Relative */
			{
				m_Optics[4].decrement_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_YAW_DEC_BTN,
					wxT("-"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[4].decrement_btn->SetToolTip(wxT("Decrement distance"));
				wxFloatingPointValidator<float>	rel_val(3, NULL, wxNUM_VAL_DEFAULT);
				rel_val.SetMin(-1000.f);
				rel_val.SetMax(1000.f);
				m_Optics[4].relative_text_ctrl = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_YAW_REL_TE_CTL,
					wxT("789.123"),
					wxDefaultPosition,
					relativeTxtCtrlSize,
					wxTE_CENTRE,
					rel_val
				);
				m_Optics[4].increment_btn = new wxButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_YAW_INC_BTN,
					wxT("+"),
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[4].increment_btn->SetToolTip(wxT("Increment distance"));

				wxSizer* const rel_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Relative [mm]");
				rel_sizer->Add(m_Optics[4].decrement_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				rel_sizer->Add(m_Optics[4].relative_text_ctrl, 0, wxALIGN_CENTER);
				rel_sizer->Add(m_Optics[4].increment_btn, 0, wxALIGN_CENTER | wxLEFT, 2);
				yaw_optics->Add(rel_sizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 2);
			}

			/* Global positioning controls */
			{
				m_Optics[4].center_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_YAW_CENTER_BTN,
					centerBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[4].center_btn->SetToolTip(wxT("Go to the center position of motor"));
				m_Optics[4].home_btn = new wxBitmapButton(
					page,
					MainFrameVariables::ID::RIGHT_SC_OPT_YAW_HOME_BTN,
					homeBitmap,
					wxDefaultPosition,
					incrementDecrementBtnSize);
				m_Optics[4].home_btn->SetToolTip(wxT("Go to the home position of motor"));

				wxSizer* const jump_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Jump");
				jump_sizer->Add(m_Optics[4].center_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
				jump_sizer->Add(m_Optics[4].home_btn, 0, wxALIGN_CENTER);

				yaw_optics->Add(jump_sizer, 0, wxALIGN_CENTER);
			}
		}
		yaw_optics->AddStretchSpacer();
		sizerPage->Add(yaw_optics, 0, wxEXPAND);
	}

	page->SetSizer(sizerPage);
	return page;
}

auto cMain::CreateAuxPage
(
	wxWindow* parent,
	const wxSize& absoluteTxtCtrlSize,
	const wxSize& relativeTxtCtrlSize,
	const wxSize& setBtnSize,
	const wxSize& incrementDecrementBtnSize,
	const wxBitmap& setBitmap,
	const wxBitmap& centerBitmap,
	const wxBitmap& homeBitmap
) -> wxWindow*
{
	wxPanel* page = new wxPanel(parent);
	wxSizer* sizerPage = new wxBoxSizer(wxVERTICAL);

	wxSizer* const x_aux = new wxStaticBoxSizer(wxHORIZONTAL, page, "&X");
	x_aux->AddStretchSpacer();

	{
		/* Absolute */
		{
			wxSizer* const abs_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Absolute [mm]");

			wxFloatingPointValidator<float> abs_val(3, NULL, wxNUM_VAL_DEFAULT);
			abs_val.SetMin(-1000.f);
			abs_val.SetMax(1000.f);

			m_Aux[0].absolute_text_ctrl = new wxTextCtrl(
				page,
				MainFrameVariables::ID::RIGHT_SC_AUX_X_ABS_TE_CTL,
				wxT("123.456"),
				wxDefaultPosition,
				absoluteTxtCtrlSize,
				wxTE_CENTRE | wxTE_PROCESS_ENTER,
				abs_val
			);

			m_Aux[0].set_btn = new wxBitmapButton
			(
				page,
				MainFrameVariables::ID::RIGHT_SC_AUX_X_SET_BTN,
				setBitmap
			);

			m_Aux[0].set_btn->SetMinSize(setBtnSize);
			m_Aux[0].set_btn->SetToolTip(wxT("Set the absolute motor position"));

			abs_sizer->Add(m_Aux[0].absolute_text_ctrl, 0, wxALIGN_CENTER);
			abs_sizer->Add(m_Aux[0].set_btn, 0, wxALIGN_CENTER | wxLEFT, 2);

			x_aux->Add(abs_sizer, 0, wxALIGN_CENTER);
		}

		/* Relative */
		{
			m_Aux[0].decrement_btn = new wxButton(
				page,
				MainFrameVariables::ID::RIGHT_SC_AUX_X_DEC_BTN,
				wxT("-"),
				wxDefaultPosition,
				incrementDecrementBtnSize
			);

			m_Aux[0].decrement_btn->SetToolTip(wxT("Decrement distance"));

			wxFloatingPointValidator<float> rel_val(3, NULL, wxNUM_VAL_DEFAULT);
			rel_val.SetMin(-1000.f);
			rel_val.SetMax(1000.f);

			m_Aux[0].relative_text_ctrl = new wxTextCtrl(
				page,
				MainFrameVariables::ID::RIGHT_SC_AUX_X_REL_TE_CTL,
				wxT("789.123"),
				wxDefaultPosition,
				relativeTxtCtrlSize,
				wxTE_CENTRE,
				rel_val
			);

			m_Aux[0].increment_btn = new wxButton(
				page,
				MainFrameVariables::ID::RIGHT_SC_AUX_X_INC_BTN,
				wxT("+"),
				wxDefaultPosition,
				incrementDecrementBtnSize
			);

			m_Aux[0].increment_btn->SetToolTip(wxT("Increment distance"));

			wxSizer* const rel_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Relative [mm]");
			rel_sizer->Add(m_Aux[0].decrement_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
			rel_sizer->Add(m_Aux[0].relative_text_ctrl, 0, wxALIGN_CENTER);
			rel_sizer->Add(m_Aux[0].increment_btn, 0, wxALIGN_CENTER | wxLEFT, 2);

			x_aux->Add(rel_sizer, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 2);
		}

		/* Jump */
		{
			m_Aux[0].center_btn = new wxBitmapButton(
				page,
				MainFrameVariables::ID::RIGHT_SC_AUX_X_CENTER_BTN,
				centerBitmap,
				wxDefaultPosition,
				incrementDecrementBtnSize
			);

			m_Aux[0].center_btn->SetToolTip(wxT("Go to the center position of motor"));

			m_Aux[0].home_btn = new wxBitmapButton(
				page,
				MainFrameVariables::ID::RIGHT_SC_AUX_X_HOME_BTN,
				homeBitmap,
				wxDefaultPosition,
				incrementDecrementBtnSize
			);

			m_Aux[0].home_btn->SetToolTip(wxT("Go to the home position of motor"));

			wxSizer* const jump_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Jump");
			jump_sizer->Add(m_Aux[0].center_btn, 0, wxALIGN_CENTER | wxRIGHT, 2);
			jump_sizer->Add(m_Aux[0].home_btn, 0, wxALIGN_CENTER);

			x_aux->Add(jump_sizer, 0, wxALIGN_CENTER);
		}
	}

	x_aux->AddStretchSpacer();

	sizerPage->Add(x_aux, 0, wxEXPAND);
	page->SetSizer(sizerPage);

	return page;
}

auto cMain::CreateDeviceControls(wxWindow* parent, wxSizer* sizer) -> void
{
	auto size = FromDIP(wxSize(16, 16));
	auto imageListDevice = new wxImageList(size.GetWidth(), size.GetHeight(), true);

	int deviceImgIndex{}, propertiesImgIndex{};

	/* Device bitmap */
	{
		auto bitmap = wxART_DEBLUR;
		auto client = wxART_CLIENT_MATERIAL_ROUND;
		auto color = wxColour(0, 162, 232);

		auto bmp = wxMaterialDesignArtProvider::GetBitmap
		(
			bitmap,
			client,
			size,
			color
		);

		deviceImgIndex = imageListDevice->Add(bmp);
	}

	/* Properties bitmap */
	{
		auto bitmap = wxART_LIST_ALT;
		auto client = wxART_CLIENT_MATERIAL_ROUND;
		auto color = wxColour(0, 128, 255);

		auto bmp = wxMaterialDesignArtProvider::GetBitmap
		(
			bitmap,
			client,
			size,
			color
		);

		propertiesImgIndex = imageListDevice->Add(bmp);
	}

	m_DeviceControlsNotebook = new wxNotebook(parent, wxID_ANY);

	m_DeviceControlsNotebook->AssignImageList(imageListDevice);

	m_DeviceControlsNotebook->AddPage
	(
		CreateDevicePage(m_DeviceControlsNotebook),
		"Device",
#ifdef _DEBUG
		true,
#else
		true,
#endif // _DEBUG
		deviceImgIndex
	);

	m_DeviceControlsNotebook->AddPage
	(
		CreatePropertiesPage(m_DeviceControlsNotebook),
		"Properties",
#ifdef _DEBUG
		false,
#else
		false,
#endif // _DEBUG
		propertiesImgIndex
	);

	sizer->Add(m_DeviceControlsNotebook, 1, wxEXPAND | wxALL, 5);
}

auto cMain::CreateDevicePage(wxWindow* parent) -> wxWindow*
{
	wxPanel* page = new wxPanel(parent);
	wxSizer* sizerPage = new wxBoxSizer(wxVERTICAL);

	auto gridSizer = new wxGridSizer(2);
	gridSizer->SetVGap(5);

	auto txtCtrlSize = FromDIP(wxSize(64, 20));

	{
		// Exposure
		{
			gridSizer->Add
			(
				new wxStaticText
				(
					page,
					wxID_ANY,
					wxT("Exposure [s]:")
				),
				0,
				wxALIGN_CENTER
			);

			wxIntegerValidator<int>	exposure_val(NULL, wxNUM_VAL_ZERO_AS_BLANK);
			exposure_val.SetMin(1);
			exposure_val.SetMax(1'000'000);

			m_DeviceExposure = std::make_unique<wxTextCtrl>
				(
					page,
					MainFrameVariables::ID::RIGHT_CAM_EXPOSURE_TXT_CTL,
#ifdef _DEBUG
					wxT("1"),
#else
					wxT("1"),
#endif // _DEBUG
					wxDefaultPosition,
					txtCtrlSize,
					wxTE_CENTRE | wxTE_PROCESS_ENTER,
					exposure_val
				);

			m_DeviceExposure->Disable();
			m_DeviceExposure->SetToolTip("Set desired exposure in [s] and press Enter");

			gridSizer->Add(m_DeviceExposure.get(), 0, wxALIGN_CENTER);
		}

		// Desired Range
		{
			gridSizer->Add
			(
				new wxStaticText
				(
					page,
					wxID_ANY,
					wxT("Desired Range [keV]:")
				),
				0,
				wxALIGN_CENTER
			);

			// Left Border
			{
				wxFloatingPointValidator<float>	minVal(3, NULL);
				minVal.SetMin(0.0);
				minVal.SetMax(1'000'000.0);

				m_MinRangeKEVTxtCtrl = std::make_unique<wxTextCtrl>
					(
						page,
						MainFrameVariables::ID::RIGHT_DEVICE_MIN_RANGE_TXT_CTL,
						wxT("0.000"),
						wxDefaultPosition,
						txtCtrlSize,
						wxTE_CENTRE | wxTE_PROCESS_ENTER,
						minVal
					);

				m_MinRangeKEVTxtCtrl->Disable();
				m_MinRangeKEVTxtCtrl->SetToolTip("Left border for the desired range in keV");
			}

			// Right Border
			{
				wxFloatingPointValidator<float>	val(3, NULL);
				val.SetMin(0.0);
				val.SetMax(1'000'000.0);

				m_MaxRangeKEVTxtCtrl = std::make_unique<wxTextCtrl>
					(
						page,
						MainFrameVariables::ID::RIGHT_DEVICE_MAX_RANGE_TXT_CTL,
						wxT("0.000"),
						wxDefaultPosition,
						txtCtrlSize,
						wxTE_CENTRE | wxTE_PROCESS_ENTER,
						val
					);

				m_MaxRangeKEVTxtCtrl->Disable();
				m_MaxRangeKEVTxtCtrl->SetToolTip("Right border for the desired range in keV");
			}

			auto sizer = new wxBoxSizer(wxHORIZONTAL);
			sizer->Add(m_MinRangeKEVTxtCtrl.get(), 0, wxEXPAND);
			sizer->AddSpacer(5);
			sizer->Add(m_MaxRangeKEVTxtCtrl.get(), 0, wxEXPAND);

			gridSizer->Add(sizer, 0, wxALIGN_CENTER);
		}
		sizerPage->Add(gridSizer, 0, wxEXPAND | wxALL, 5);



		/* Preview And Start\Stop Live Capturing */
		{
			wxSizer* const sizer = new wxBoxSizer(wxHORIZONTAL);

			auto size = FromDIP(wxSize(32, 32));

			wxBitmap bmp{};
			{
				auto bitmap = wxART_PLUS_ONE;
				auto client = wxART_CLIENT_MATERIAL_ROUND;
				auto color = m_DefaultWidgetsColor;

				bmp = wxMaterialDesignArtProvider::GetBitmap
				(
					bitmap,
					client,
					size,
					color
				);
				bmp.SetScaleFactor(GetDPIScaleFactor());
			}

			m_SingleShotBtn = std::make_unique<wxBitmapButton>(
				page,
				MainFrameVariables::ID::RIGHT_DEVICE_SINGLE_SHOT_BTN,
				bmp
			);

			m_SingleShotBtn->SetToolTip(wxT("Single Shot (Ctrl+Shift+S)\nCapture single data row and save them on disk"));
			m_SingleShotBtn->Disable();
			m_SingleShotBtn->SetMinSize(size);
			sizer->Add(m_SingleShotBtn.get(), 0, wxCENTER);

			sizer->AddStretchSpacer();

			bmp = GetLiveCapturingBitmap(false);

			m_StartStopLiveCapturingTglBtn = std::make_unique<wxBitmapToggleButton>
				(
					page,
					MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN,
					bmp
				);

			m_StartStopLiveCapturingTglBtn->SetToolTip("Start/Stop live sequence of capturing data (Ctrl+Shift+L)");

			m_StartStopLiveCapturingTglBtn->Disable();

			// Force a tighter size (still may have minimum imposed by OS)
			const auto bsz = bmp.GetSize();
			m_StartStopLiveCapturingTglBtn->SetMinSize(bsz + FromDIP(wxSize(6, 6)));

			sizer->Add(m_StartStopLiveCapturingTglBtn.get(), 0, wxEXPAND | wxTOP, 5);

			sizerPage->Add(sizer, 0, wxEXPAND | wxALL, 5);
		}

		sizerPage->AddSpacer(5);

		{
			m_ExposureProgressPanel = new wxPanel(page);
			m_ExposureProgressPanelSizer = new wxBoxSizer(wxVERTICAL);

			// Exposure Gauge
			{

				m_ExposureGauge = std::make_unique<wxGauge>
					(
						m_ExposureProgressPanel,
						wxID_ANY,
						100
					);

				m_ExposureGauge->SetValue(0);

				m_ExposureProgressPanelSizer->Add(m_ExposureGauge.get(), 0, wxEXPAND | wxBOTTOM, 4);
			}

			// Exposure progress static text
			{
				m_ExposureProgressStaticText = std::make_unique<wxStaticText>
					(
						m_ExposureProgressPanel,
						wxID_ANY,
						wxT("Exposure Progress: 0%")
					);

				m_ExposureProgressStaticText->SetLabel("Exposure Progress: 0%");

				m_ExposureProgressPanelSizer->Add(m_ExposureProgressStaticText.get(), 0, wxEXPAND, 0);
			}

			m_ExposureProgressPanel->SetSizer(m_ExposureProgressPanelSizer);

			// Reserve enough vertical space even when hidden
			m_ExposureProgressPanel->SetMinSize(FromDIP(wxSize(-1, 48)));
			m_ExposureProgressPanel->Layout();

			sizerPage->Add(m_ExposureProgressPanel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 5);

			// Start hidden
			m_ExposureProgressPanel->Hide();
		}

	}

	page->SetSizer(sizerPage);
	return page;
}

auto cMain::CreatePropertiesPage(wxWindow* parent) -> wxWindow*
{
	wxPanel* page = new wxPanel(parent);
	wxSizer* sizerPage = new wxBoxSizer(wxVERTICAL);

	m_PropertiesNames = std::make_unique<MainFrameVariables::PropertiesNames>();

	m_CurrentDeviceSettingsPropertyGrid = new wxPropertyGrid
	(
		page,
		MainFrameVariables::ID::RIGHT_DEVICE_ACTUAL_PARAMETERS_PROPERTY_GRID,
		wxDefaultPosition,
		wxDefaultSize,
		wxPG_SPLITTER_AUTO_CENTER
	);

	auto property = m_CurrentDeviceSettingsPropertyGrid->Append
	(
		new wxStringProperty
		(
			m_PropertiesNames->manufacturer,
			m_PropertiesNames->manufacturer,
			"None"
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->Append
	(
		new wxStringProperty
		(
			m_PropertiesNames->id,
			m_PropertiesNames->id,
			"None"
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->Append
	(
		new wxStringProperty
		(
			m_PropertiesNames->firmware_version,
			m_PropertiesNames->firmware_version,
			"None"
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->Append
	(
		new wxStringProperty
		(
			m_PropertiesNames->mca_length,
			m_PropertiesNames->mca_length,
			"None"
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->Append
	(
		new wxStringProperty
		(
			m_PropertiesNames->bin_width,
			m_PropertiesNames->bin_width,
			"None"
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->Append
	(
		new wxStringProperty
		(
			m_PropertiesNames->gain,
			m_PropertiesNames->gain,
			"None"
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->Append
	(
		new wxStringProperty
		(
			m_PropertiesNames->threshold,
			m_PropertiesNames->threshold,
			"None"
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->Append
	(
		new wxStringProperty
		(
			m_PropertiesNames->polarity,
			m_PropertiesNames->polarity,
			"None"
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	auto temperatureGroup = m_CurrentDeviceSettingsPropertyGrid->Append(new wxPropertyCategory("Temperature [degC]", "TemperatureDegC"));

	property = m_CurrentDeviceSettingsPropertyGrid->AppendIn
	(
		temperatureGroup,
		new wxFloatProperty
		(
			m_PropertiesNames->therm_1,
			m_PropertiesNames->therm_1,
			0.0
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->AppendIn
	(
		temperatureGroup,
		new wxFloatProperty
		(
			m_PropertiesNames->therm_2,
			m_PropertiesNames->therm_2,
			0.0
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->AppendIn
	(
		temperatureGroup,
		new wxFloatProperty
		(
			m_PropertiesNames->sdd_temperature,
			m_PropertiesNames->sdd_temperature,
			0.0
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->AppendIn
	(
		temperatureGroup,
		new wxFloatProperty
		(
			m_PropertiesNames->hot_side_temperature,
			m_PropertiesNames->hot_side_temperature,
			0.0
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	property = m_CurrentDeviceSettingsPropertyGrid->AppendIn
	(
		temperatureGroup,
		new wxFloatProperty
		(
			m_PropertiesNames->board_temperature,
			m_PropertiesNames->board_temperature,
			0.0
		)
	);

	property->ChangeFlag(wxPGFlags::ReadOnly, true);

	auto it = m_CurrentDeviceSettingsPropertyGrid->GetIterator();
	int i = 0;

	for (; !it.AtEnd(); it++)
	{
		auto prop = *it;
		if (prop)
		{
			if (!i)
				m_CurrentDeviceSettingsPropertyGrid->SetPropertyBackgroundColour(prop, wxColour(m_DefaultCellColor.GetRed() - 20, m_DefaultCellColor.GetGreen() - 20, m_DefaultCellColor.GetBlue() - 20));
			else
				m_CurrentDeviceSettingsPropertyGrid->SetPropertyBackgroundColour(prop, m_DefaultCellColor);

			++i;
		}
	}

	m_CurrentDeviceSettingsPropertyGrid->Refresh();

	sizerPage->Add(m_CurrentDeviceSettingsPropertyGrid, 1, wxEXPAND);

	page->SetSizer(sizerPage);
	return page;
}

auto cMain::CreateMeasurement(wxWindow* parent, wxSizer* sizer) -> void
{
	auto size = FromDIP(wxSize(16, 16));
	auto imageListMeasurement = new wxImageList(size.GetWidth(), size.GetHeight(), true);

	int measurementImgIndex{};

	/* Detector bitmap */
	{
		auto bitmap = wxART_CONTROL_CAMERA;
		auto client = wxART_CLIENT_MATERIAL_ROUND;
		auto color = wxColour(126, 132, 247);

		auto bmp = wxMaterialDesignArtProvider::GetBitmap
		(
			bitmap,
			client,
			size,
			color
		);

		measurementImgIndex = imageListMeasurement->Add(bmp);
	}

	m_MeasurementControlsNotebook = new wxNotebook(parent, wxID_ANY);

	m_MeasurementControlsNotebook->AssignImageList(imageListMeasurement);

	m_MeasurementControlsNotebook->AddPage
	(
		CreateMeasurementPage(m_MeasurementControlsNotebook),
		"Measurement",
#ifdef _DEBUG
		true,
#else
		true,
#endif // _DEBUG
		measurementImgIndex
	);

	sizer->Add(m_MeasurementControlsNotebook, 0, wxEXPAND | wxALL, 5);
}

auto cMain::CreateMeasurementPage(wxWindow* parent) -> wxWindow*
{
	wxPanel* page = new wxPanel(parent);
	wxSizer* sizerPage = new wxBoxSizer(wxVERTICAL);

	/* Output directory */
	{
		wxSizer* const out_dir_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Output directory");

		m_OutDirTextCtrl = std::make_unique<wxTextCtrl>(
			page,
			MainFrameVariables::ID::RIGHT_MT_OUT_FLD_TE_CTL,
			wxT("Save directory..."),
			wxDefaultPosition,
			wxDefaultSize,
			wxTE_LEFT | wxTE_READONLY
		);

		auto size = FromDIP(wxSize(32, 32));

		wxBitmap bmp{};
		{
			auto bitmap = wxART_FOLDER_OPEN;
			auto client = wxART_CLIENT_MATERIAL_ROUND;
			auto color = m_DefaultWidgetsColor;

			bmp = wxMaterialDesignArtProvider::GetBitmap
			(
				bitmap,
				client,
				size,
				color
			);
			bmp.SetScaleFactor(GetDPIScaleFactor());
		}

		m_OutDirBtn = std::make_unique<wxBitmapButton>
			(
				page,
				MainFrameVariables::ID::RIGHT_MT_OUT_FLD_BTN,
				bmp
			);

		m_OutDirBtn->SetToolTip(wxT("Set the output directory"));
		m_OutDirBtn->SetMinSize(size);

		out_dir_static_box_sizer->Add(m_OutDirTextCtrl.get(), 1, wxCENTER);

		out_dir_static_box_sizer->AddSpacer(4);

		out_dir_static_box_sizer->Add(m_OutDirBtn.get(), 0, wxALIGN_CENTER);

		sizerPage->Add(out_dir_static_box_sizer, 0, wxEXPAND);
	}

	// Wide enough for the longest accepted value at the current font and
	// DPI; the Start/Step/Finish boxes then share the row equally.
	const wxSize start_text_ctrl_size(page->GetTextExtent("-1000.000").GetWidth() + FromDIP(16), wxDefaultCoord);
	const wxSize step_text_ctrl_size = start_text_ctrl_size, finish_text_ctrl_size = start_text_ctrl_size;

	{
		wxSizer* const directions_static_box_sizer = new wxStaticBoxSizer(wxVERTICAL, page, "&Directions");

		/* First axis */
		{
			wxSizer* const first_axis_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&First axis");

			/* Stage */
			{
				wxSizer* const stage_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Stage");
				m_FirstStage->stage = new wxChoice(
					page,
					MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_CHOICE,
					wxDefaultPosition,
					wxDefaultSize,
					m_FirstStage->motors);
				m_FirstStage->stage->SetSelection(0);
				stage_static_box_sizer->Add(m_FirstStage->stage, 0, wxEXPAND);
				first_axis_static_box_sizer->Add(stage_static_box_sizer, 0, wxEXPAND);
			}

			/* Start */
			{
				wxSizer* const start_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Start");

				wxFloatingPointValidator<float>	start_val(3, NULL, wxNUM_VAL_DEFAULT);
				start_val.SetMin(-1000.0);
				start_val.SetMax(1000.0);

				m_FirstStage->start = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_START,
					wxT("123.456"),
					wxDefaultPosition,
					start_text_ctrl_size,
					wxTE_CENTRE,
					start_val);

				start_static_box_sizer->Add(m_FirstStage->start, 1, wxEXPAND);

				first_axis_static_box_sizer->Add(start_static_box_sizer, 1, wxEXPAND | wxLEFT | wxRIGHT, 2);
			}

			/* Step */
			{
				wxSizer* const step_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Step");

				wxFloatingPointValidator<float>	step_val(3, NULL, wxNUM_VAL_DEFAULT);
				step_val.SetMin(-1000.0);
				step_val.SetMax(1000.0);

				m_FirstStage->step = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_STEP,
					wxT("123.456"),
					wxDefaultPosition,
					step_text_ctrl_size,
					wxTE_CENTRE,
					step_val);

				step_static_box_sizer->Add(m_FirstStage->step, 1, wxEXPAND);
				first_axis_static_box_sizer->Add(step_static_box_sizer, 0, wxEXPAND | wxRIGHT, 2);
			}

			/* Finish */
			{
				wxSizer* const finish_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Finish");

				wxFloatingPointValidator<float>	finish_val(3, NULL, wxNUM_VAL_DEFAULT);
				finish_val.SetMin(-1000.0);
				finish_val.SetMax(1000.0);

				m_FirstStage->finish = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_FINISH,
					wxT("123.456"),
					wxDefaultPosition,
					finish_text_ctrl_size,
					wxTE_CENTRE,
					finish_val);

				finish_static_box_sizer->Add(m_FirstStage->finish, 1, wxEXPAND);
				first_axis_static_box_sizer->Add(finish_static_box_sizer, 1, wxEXPAND);
			}

			directions_static_box_sizer->Add(first_axis_static_box_sizer, 0, wxEXPAND);
		}

#ifdef USE_2_AXIS_MEASUREMENT
		/* Second axis */
		{
			wxSizer* const second_axis_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Second axis");

			/* Stage */
			{
				wxSizer* const stage_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Stage");
				m_SecondStage->stage = new wxChoice(
					page,
					MainFrameVariables::ID::RIGHT_MT_SECOND_STAGE_CHOICE,
					wxDefaultPosition,
					wxDefaultSize,
					m_SecondStage->motors);
				m_SecondStage->stage->SetSelection(0);
				stage_static_box_sizer->Add(m_SecondStage->stage, 0, wxEXPAND);
				second_axis_static_box_sizer->Add(stage_static_box_sizer, 0, wxEXPAND);
			}

			/* Start */
			{
				wxSizer* const start_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Start");

				wxFloatingPointValidator<float>	start_val(3, NULL, wxNUM_VAL_DEFAULT);
				start_val.SetMin(-1000.0);
				start_val.SetMax(1000.0);

				m_SecondStage->start = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_MT_SECOND_STAGE_START,
					wxT("123.456"),
					wxDefaultPosition,
					start_text_ctrl_size,
					wxTE_CENTRE,
					start_val);

				start_static_box_sizer->Add(m_SecondStage->start, 1, wxEXPAND);

				second_axis_static_box_sizer->Add(start_static_box_sizer, 1, wxEXPAND | wxLEFT | wxRIGHT, 2);
			}

			/* Step */
			{
				wxSizer* const step_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Step");

				wxFloatingPointValidator<float>	step_val(3, NULL, wxNUM_VAL_DEFAULT);
				step_val.SetMin(-1000.0);
				step_val.SetMax(1000.0);

				m_SecondStage->step = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_MT_SECOND_STAGE_STEP,
					wxT("123.456"),
					wxDefaultPosition,
					step_text_ctrl_size,
					wxTE_CENTRE,
					step_val);

				step_static_box_sizer->Add(m_SecondStage->step, 1, wxEXPAND);
				second_axis_static_box_sizer->Add(step_static_box_sizer, 1, wxEXPAND | wxRIGHT, 2);
			}

			/* Finish */
			{
				wxSizer* const finish_static_box_sizer = new wxStaticBoxSizer(wxHORIZONTAL, page, "&Finish");

				wxFloatingPointValidator<float>	finish_val(3, NULL, wxNUM_VAL_DEFAULT);
				finish_val.SetMin(-1000.0);
				finish_val.SetMax(1000.0);

				m_SecondStage->finish = new wxTextCtrl(
					page,
					MainFrameVariables::ID::RIGHT_MT_SECOND_STAGE_FINISH,
					wxT("123.456"),
					wxDefaultPosition,
					finish_text_ctrl_size,
					wxTE_CENTRE,
					finish_val);

				finish_static_box_sizer->Add(m_SecondStage->finish, 1, wxEXPAND);
				second_axis_static_box_sizer->Add(finish_static_box_sizer, 1, wxEXPAND);
			}

			directions_static_box_sizer->Add(second_axis_static_box_sizer, 0, wxEXPAND);
		}
#endif

		sizerPage->Add(directions_static_box_sizer, 0, wxEXPAND);
	}

	/* Start Capturing */
	{
		wxSizer* const capturing_sizer = new wxStaticBoxSizer(wxVERTICAL, page, "&Capturing");

		auto bmp = GetMeasurementBitmap(false);

		// Force a tighter size (still may have minimum imposed by OS)
		const auto bsz = bmp.GetSize();

		m_StartStopMeasurementTglBtn = std::make_unique<wxBitmapToggleButton>
			(
				page,
				MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN,
				bmp
			);

		m_StartStopMeasurementTglBtn->SetToolTip("Start/Stop Measurement (Ctrl+Shift+M)\nRepeatedly move the stage, capture, and save images to disk.");

		m_StartStopMeasurementTglBtn->Disable();

		m_StartStopMeasurementTglBtn->SetMinSize(bsz + FromDIP(wxSize(6, 6)));

		capturing_sizer->Add(m_StartStopMeasurementTglBtn.get(), 0, wxALIGN_CENTER);

		sizerPage->Add(capturing_sizer, 0, wxALIGN_RIGHT);
	}

	page->SetSizer(sizerPage);
	return page;
}

auto cMain::OnEnableDarkMode(wxCommandEvent& evt) -> void
{
	const bool isDarkModeEnabled =
		m_MenuBar->menu_window->IsChecked(MainFrameVariables::ID::MENUBAR_WINDOW_ENABLE_DARK_MODE);

	ApplyDarkModeState(isDarkModeEnabled);
	SaveInitializationFile();
}

void cMain::OnSetOutDirectoryBtn(wxCommandEvent& evt)
{
	wxString outDirPath{};

#ifdef _DEBUG
	outDirPath = ".\\src\\dbg_fld";
#else
	wxDirDialog save_dialog(NULL, "Choose save directory", "",
		wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (save_dialog.ShowModal() == wxID_CANCEL)
		return;

	outDirPath = save_dialog.GetPath();
#endif // _DEBUG

	m_OutDirTextCtrl->SetValue(outDirPath);

	ApplyCaptureUiState(m_CaptureUiMode);
}

auto cMain::OnOpenMCAFile(wxCommandEvent& evt) -> void
{
	wxString filePath{};

#ifdef _DEBUG
	filePath = ".\\src\\data\\035-019_01.mca";
#else
	wxFileDialog openFileDialog(this, _("Open file"), "", "",
		"MCA files (*.mca)|*.mca|All files (*.*)|*.*",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() == wxID_CANCEL)
		return; // the user changed idea...

	// Proceed loading the file chosen by the user
	filePath = openFileDialog.GetPath();
#endif // _DEBUG

	if (!ParseMCAFile(filePath)) return;

	m_MinRangeKEVTxtCtrl->Enable();
	m_MaxRangeKEVTxtCtrl->Enable();
}

auto cMain::ParseMCAFile(const wxString filePath) -> bool
{
	std::ifstream file(filePath.ToStdString());
	if (!file.is_open()) {
		wxLogError("Cannot open file '%s'.", filePath);
		return false;
	}

	std::string line;
	double binSize = 0.0;
	std::unique_ptr<unsigned long[]> values;
	bool foundBinSize = false;
	bool found8192 = false;

	while (std::getline(file, line))
	{
		if (line.find("Bin Size") != std::string::npos)
		{
			std::replace(line.begin(), line.end(), ',', '.');
			std::istringstream iss(line);
			std::string key, equalSign;
			iss >> key >> key >> equalSign >> binSize;
			foundBinSize = true;
		}

		if (line == "8192") {
			found8192 = true;
			break;
		}
	}

	if (!foundBinSize) {
		wxLogError("BinSize parameter not found.");
		return false;
	}

	if (!found8192) {
		wxLogError("8192 row not found.");
		return false;
	}

	auto numValues = 8192;

	values = std::make_unique<unsigned long[]>(numValues);

	unsigned long value;
	int i{};
	while (file >> value && i < numValues)
	{
		values[i] = value;
		++i;
	}

	//wxLogMessage("Successfully loaded data. BinSize: %f, Values count: %lu", binSize, numValues);

	m_PreviewPanel->SetReferenceBinSize(binSize);

	unsigned long long sum{};
	sum = std::accumulate(&values[0], &values[numValues], sum);

	const bool preserveUserXState = m_PreviewPanel && m_PreviewPanel->HasCustomizedXDomain();

	m_PreviewPanel->SetPerformanceOverlayEnabled(false);
	m_PreviewPanel->SetKETEKReferenceData(values.get(), numValues, sum);

	if (!preserveUserXState)
		UpdateDesiredEnergyRangeControlsToFullData();

	return true;
}

void cMain::OnOpenSettings(wxCommandEvent& evt)
{
	// Applying settings replaces the motor and detector objects. Threads
	// that are still running hold raw pointers to them, so refuse until
	// everything has stopped.
	if (IsBackgroundWorkRunning())
	{
		wxMessageBox
		(
			"Settings can't be changed while a capture, measurement or "
			"motor movement is running.\n\nStop it first and try again.",
			"Settings",
			wxOK | wxICON_INFORMATION,
			this
		);
		return;
	}

	m_PreviewPanel->SetFocus();

	const auto code = m_Settings->ShowModal();

	if (code == wxID_OK)
	{
		m_DefaultMotorsIPAddress =
			m_Settings->GetIPAddress();

		m_DefaultWorkStation =
			m_Settings->GetSelectedWorkStation();

		m_DefaultMotorsIPAddress.Trim(true);
		m_DefaultMotorsIPAddress.Trim(false);

		m_DefaultWorkStation.Trim(true);
		m_DefaultWorkStation.Trim(false);

		if (!SaveInitializationFile())
		{
			wxLogError
			(
				"Could not save the selected workstation "
				"and motor connection settings."
			);
		}

		m_MotorsWebInterfaceLoaded = false;

		UpdateStagePositions();
		EnableUsedAndDisableNonUsedMotors();
		InitializeSelectedDevice();
	}
}

auto cMain::InitializeSelectedCamera() -> void
{
	auto curr_camera = m_Settings->GetSelectedCamera();
	if (curr_camera == "None") return;

	UpdateDeviceParameters();

	m_StartStopLiveCapturingTglBtn->SetValue(true);
	wxCommandEvent art_start_live_capturing(wxEVT_TOGGLEBUTTON, MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN);
	ProcessEvent(art_start_live_capturing);
}

auto cMain::InitializeSelectedDevice() -> void
{
	auto enable = true;

	const auto ketekConfigPath =
		GetKetekConfigurationFilePath();

	// Release the previous handler first: its destructor calls xiaExit(),
	// which would otherwise shut down the session the new handler opens.
	m_KetekHandler.reset();

	m_KetekHandler = std::make_unique<Ketek>
		(
			m_Settings->GetSelectedKETEK().ToStdString(),
			std::filesystem::path(
				ketekConfigPath.ToStdWstring()
			)
		);

	if (!m_KetekHandler->IsDeviceInitialized())
	{
		wxLogError
		(
			"KETEK initialization failed:\n%s",
			wxString::FromUTF8(
				m_KetekHandler->GetLastError()
			)
		);

		ApplyCaptureUiState(
			MainFrameVariables::CaptureUiMode::Idle
		);

		return;
	}

	if (m_KetekHandler->IsDeviceInitialized())
	{
		m_PreviewPanel->SetCurrentDevice(PreviewPanelVariables::KETEK);
		m_PreviewPanel->SetBinSize(m_KetekHandler->GetBinSize());
	}
	else
	{
		enable = false;
	}

	ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);

	UpdateDeviceParameters();
}

auto cMain::UpdateDeviceParameters() -> void
{
	if (!m_KetekHandler || !m_KetekHandler->IsDeviceInitialized())
		return;

	auto deviceManufacturer = SettingsVariables::DeviceManufacturerToString(m_Settings->GetSelectedDeviceManufacturer());
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->manufacturer, deviceManufacturer);

	auto deviceID = m_Settings->GetSelectedDeviceID();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->id, deviceID);

	auto usbVersion = wxLongLong(m_KetekHandler->GetUSBVersion());
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->firmware_version, usbVersion.ToString());

	auto mcaWidth = m_KetekHandler->GetDataSize();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->mca_length, PreviewPanelVariables::CreateStringWithPrecision(mcaWidth));

	auto binWidth = m_KetekHandler->GetBinSize();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->bin_width, PreviewPanelVariables::CreateStringWithPrecision(binWidth));

	auto gain = m_KetekHandler->GetGain();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->gain, PreviewPanelVariables::CreateStringWithPrecision(gain));

	auto threshold = m_KetekHandler->GetThreshold();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->threshold, PreviewPanelVariables::CreateStringWithPrecision(threshold));

	auto polarity = m_KetekHandler->GetPolarity();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->polarity, PreviewPanelVariables::CreateStringWithPrecision(polarity));

	auto therm_1 = m_KetekHandler->GetThermistor1Temperature();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->therm_1, therm_1);

	auto therm_2 = m_KetekHandler->GetThermistor2Temperature();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->therm_2, therm_2);

	auto sddTemperature = m_KetekHandler->GetSDDTemperature();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->sdd_temperature, sddTemperature);

	auto hotSideTemperature = m_KetekHandler->GetHotSideTemperature();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->hot_side_temperature, hotSideTemperature);

	auto boardTemperature = m_KetekHandler->GetBoardTemperature();
	m_CurrentDeviceSettingsPropertyGrid->SetPropertyValue(m_PropertiesNames->board_temperature, boardTemperature);

	if (m_PreviewPanel) m_PreviewPanel->SetBoardTemperature(boardTemperature);
	if (m_PreviewPanel) m_PreviewPanel->SetSDDTemperature(sddTemperature);
}

void cMain::OnFullScreen(wxCommandEvent& evt)
{
	auto isMaximized = IsMaximized();

	Maximize(!isMaximized);
}

void cMain::OnMaximizeButton(wxMaximizeEvent& evt)
{
}

void cMain::OnExit(wxCloseEvent& evt)
{
	const bool busy = IsBackgroundWorkRunning();

	if (evt.CanVeto())
	{
		// Only ask when closing would interrupt something.
		const bool askUser = busy;

		if (askUser)
		{
			const wxString question =
				"A capture, measurement or motor movement is still running.\n\n"
				"Stop all motors and background work, and exit?";

			if (wxMessageBox(question,
				"Exit application",
				wxICON_QUESTION | wxYES_NO, this) != wxYES)
			{
				evt.Veto();
				return;
			}
		}
	}

	// Stop motors and threads, and wait for every thread to finish before
	// the frame (and the detector/motor objects it owns) is destroyed.
	{
		constexpr int shutdownTimeoutMs = 15'000;
		while (true)
		{
			bool stopped{};
			{
				wxBusyCursor busyCursor;
				stopped = ShutdownBackgroundWork(shutdownTimeoutMs);
			}

			if (stopped || !evt.CanVeto())
				break;

			const auto answer = wxMessageBox
			(
				wxString::Format
				(
					"Background work is still running after %d seconds "
					"(%d thread(s)).\n\n"
					"Yes: keep waiting.\n"
					"No: exit anyway (the application may crash on exit).",
					shutdownTimeoutMs / 1000,
					TrackedThread::LiveCount()
				),
				"Exit application",
				wxICON_WARNING | wxYES_NO,
				this
			);

			if (answer != wxYES)
				break;
		}
	}

	if (!SaveInitializationFile())
		wxLogWarning("Could not save the application settings on exit.");

	AppLog::Info("App", "VEXOTControl exiting.");
	Destroy();
}

void cMain::OnExit(wxCommandEvent& evt)
{
	wxCloseEvent artificialExit(wxEVT_CLOSE_WINDOW);
	ProcessEvent(artificialExit);
}

auto cMain::OnApplicationVersion(wxCommandEvent& evt) -> void
{
	wxAboutDialogInfo info;
	info.SetName(m_AppName);
	auto majorVerStr = wxString::Format(wxT("%i"), MAJOR_VERSION);
	auto minorVerStr = wxString::Format(wxT("%i"), MINOR_VERSION);
	info.SetVersion(majorVerStr + "." + minorVerStr + ".{#CommitNumber}");

	info.SetIcon(logo_xpm);

	wxString description = wxString::Format
	(
		"An application for scientists and engineers that integrates with KETEK detector and motorized stages.\n\n"
		"Build date: %s-%s-%s",
		wxT("{#CurrentYear}"),
		wxT("{#CurrentMonth}"),
		wxT("{#CurrentDay}")
	);

	info.SetDescription(description);

	info.SetCopyright("(C) {#CurrentYear} Rigaku Innovative Technologies Europe s.r.o.");
	info.SetWebSite("https://rigaku.com");
	info.AddDeveloper("Andrej Pcelovodov");

	wxAboutBox(info);
}

auto cMain::CreateStatusBar() -> void
{
	m_StatusBar = std::make_unique<wxStatusBar>(this, wxID_ANY);

	// 0: plot readout (written by the preview panel), 1: what is running,
	// 2: detector, 3: motors
	const int widths[] = { -5, -5, -3, -3 };
	m_StatusBar->SetFieldsCount(4, widths);

	SetStatusBar(m_StatusBar.get());
}

auto cMain::LiveCapturingFinishedCapturingAndDrawing(bool is_finished) -> void
{
}

auto cMain::WorkerThreadFinished(bool is_finished) -> void
{
	if (is_finished)
	{
		wxCommandEvent live_capturing_evt(wxEVT_TOGGLEBUTTON, MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN);
		ProcessEvent(live_capturing_evt);
	}
}

void cMain::UnCheckAllTools()
{
}

void cMain::OnFirstStageChoice(wxCommandEvent& evt)
{
	auto first_stage_selection = m_FirstStage->stage->GetCurrentSelection() - 1;
	double start_stage_value{}, step_stage_value{}, finish_stage_value{};
	switch (first_stage_selection)
	{
		/* Detector */
	case 0:
		if (!m_Detector[0].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 1:
		if (!m_Detector[1].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
		/* Optics */
	case 2:
		if (!m_Optics[0].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 3:
		if (!m_Optics[1].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 4:
		if (!m_Optics[2].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 5:
		if (!m_Optics[3].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 6:
		if (!m_Optics[4].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 7:
		if (!m_Aux[0].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	default:
		break;
	}

	/* Set Start To Current position of motor */
	m_FirstStage->start->SetValue
	(
		wxString::Format
		(
			wxT("%.3f"),
			(float)start_stage_value
		)
	);
	/* Set Finish To Current position of motor + Step */
	if (!m_FirstStage->step->GetValue().ToDouble(&step_stage_value)) return;
	finish_stage_value = start_stage_value + step_stage_value;
	m_FirstStage->finish->SetValue
	(
		wxString::Format
		(
			wxT("%.3f"),
			(float)finish_stage_value
		)
	);
}

void cMain::OnSecondStageChoice(wxCommandEvent& evt)
{
#ifdef USE_2_AXIS_MEASUREMENT
	auto second_stage_selection = m_SecondStage->stage->GetCurrentSelection() - 1;
	double start_stage_value{}, step_stage_value{}, finish_stage_value{};

	switch (second_stage_selection)
	{
		/* Detector */
	case 0:
		if (!m_Detector[0].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
		/* Optics */
	case 1:
		if (!m_Optics[0].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 2:
		if (!m_Optics[1].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 3:
		if (!m_Optics[2].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 4:
		if (!m_Optics[3].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	case 5:
		if (!m_Optics[4].absolute_text_ctrl->GetValue().ToDouble(&start_stage_value)) return;
		break;
	default:
		break;
	}

	/* Set Start To Current position of motor */
	m_SecondStage->start->SetValue
	(
		wxString::Format
		(
			wxT("%.3f"),
			(float)start_stage_value
		)
	);

	/* Set Finish To Current position of motor + Step */
	if (!m_SecondStage->step->GetValue().ToDouble(&step_stage_value)) return;
	finish_stage_value = start_stage_value + step_stage_value;
	m_SecondStage->finish->SetValue
	(
		wxString::Format
		(
			wxT("%.3f"),
			(float)finish_stage_value
		)
	);

#endif // USE_2_AXIS_MEASUREMENT
}

void cMain::ChangeCameraManufacturerChoice(wxCommandEvent& evt)
{
	wxCommandEvent simulate_change_exposure_value(wxEVT_TEXT_ENTER, MainFrameVariables::ID::RIGHT_CAM_EXPOSURE_TXT_CTL);
	ProcessEvent(simulate_change_exposure_value);
}

auto cMain::CreateMetadataFile() -> void
{
	wxString exposure_time_str = m_DeviceExposure->GetValue().IsEmpty()
		? wxString("0")
		: m_DeviceExposure->GetValue();
	unsigned long exposure_time = abs(wxAtoi(exposure_time_str)); // UI value is in seconds

	double det_x_pos{}, det_y_pos{}, det_z_pos{};
	double opt_y_pos{};

	{
		/* Detector */
		if (!m_Detector[0].absolute_text_ctrl->GetValue().ToDouble(&det_x_pos)) return;
		/* Optics */
		if (!m_Optics[1].absolute_text_ctrl->GetValue().ToDouble(&opt_y_pos)) return;
	}

	std::string selected_axis{};
	switch (m_FirstStage->stage->GetCurrentSelection() - 1)
	{
		/* Detector */
	case 0:
		selected_axis = std::string("detector_x");
		break;
	case 1:
		selected_axis = std::string("detector_y");
		break;
		/* Optics */
	case 2:
		selected_axis = std::string("optics_x");
		break;
	case 3:
		selected_axis = std::string("optics_y");
		break;
	case 4:
		selected_axis = std::string("optics_z");
		break;
	case 5:
		selected_axis = std::string("optics_pitch");
		break;
	case 6:
		selected_axis = std::string("optics_yaw");
		break;
	default:
		break;
	}

	double start_first_stage_value{}, step_first_stage_value{}, finish_first_stage_value{};
	{
		if (!m_FirstStage->start->GetValue().ToDouble(&start_first_stage_value)) return;
		if (!m_FirstStage->step->GetValue().ToDouble(&step_first_stage_value)) return;
		if (!m_FirstStage->finish->GetValue().ToDouble(&finish_first_stage_value)) return;
	}
	auto now = std::chrono::system_clock::now();
	auto cur_time = std::chrono::system_clock::to_time_t(now);
	std::string time_metadata_filename{};
	std::string cur_date_and_time{};
	{
		std::string cur_date(30, '\0');
		std::strftime(&cur_date[0], cur_date.size(), "%Y%m%d", std::localtime(&cur_time));
		cur_date_and_time = cur_date.substr(0, 8);
		auto str_time = std::string(std::ctime(&cur_time)).substr(11, 8); // Cut date in format: 20230223
		auto cur_hours = str_time.substr(0, 2);
		auto cur_mins = str_time.substr(3, 2);
		auto cur_secs = str_time.substr(6, 2);
		time_metadata_filename = cur_hours + std::string("H_") + cur_mins + std::string("M_") + cur_secs + std::string("S");
		cur_date_and_time += std::string("_") + cur_hours + cur_mins + cur_secs;
	}

	nlohmann::json main_table{};
	main_table =
	{
		{"units", "mm"},
		{"pos_source", 0.000},
		{"pos_optics", 123.456},
		{"pos_detector", 234.567},
		{"first_stage",
			{
				{"title", "optics"},
				{"position", opt_y_pos}
			}
		},
		{"second_stage",
			{
				{"title", "detector_x"},
				{"position", det_x_pos}
			}
		},
		{"third_stage",
			{
				{"title", "detector_y"},
				{"position", det_y_pos}
			}
		},
		{"fourth_stage",
			{
				{"title", "detector_z"},
				{"position", det_z_pos}
			}
		},
		{"measurement",
			{
				{"stage", selected_axis},
				{"start", start_first_stage_value},
				{"step", step_first_stage_value},
				{"finish", finish_first_stage_value},
				{"exposure",
					{
						{"units", "s"},
						{"time", exposure_time},
						{"gain", 1}
					}
				},
				{"date_time", cur_date_and_time}
			}
		},
		{"message", ""}
	};

	auto out_dir_with_filename =
		m_OutDirTextCtrl->GetValue() +
		wxString("\\metadata_") +
		wxString(time_metadata_filename) +
		wxString(".json");
	std::ofstream out_file(out_dir_with_filename.ToStdString());
	if (out_file.is_open())
	{
		out_file << main_table;
		out_file.close();
	}
}

auto cMain::GetLiveCapturingBitmap(const bool isCapturing) -> wxBitmap
{
	const wxSize bitmapSize = FromDIP(wxSize(32, 32));
	const char* liveCapturingBitmap = wxART_VIDEOCAM;
	const char* liveCapturingClient = wxART_CLIENT_MATERIAL_ROUND;
	const wxColour liveCapturingStartBitmapColor = wxColour(34, 177, 76);
	const wxColour liveCapturingStopBitmapColor = wxColour(237, 28, 36);

	wxBitmap bitmap = wxMaterialDesignArtProvider::GetBitmap
	(
		liveCapturingBitmap,
		liveCapturingClient,
		bitmapSize,
		isCapturing ? liveCapturingStopBitmapColor : liveCapturingStartBitmapColor
	);
	bitmap.SetScaleFactor(GetDPIScaleFactor());
	return bitmap;
}

auto cMain::GetMeasurementBitmap(const bool isCapturing) -> wxBitmap
{
	const wxSize bitmapSize = FromDIP(wxSize(32, 32));
	const char* liveCapturingBitmap = wxART_MOVE_UP;
	const char* liveCapturingClient = wxART_CLIENT_MATERIAL_ROUND;
	const wxColour liveCapturingStartBitmapColor = wxColour(34, 177, 76);
	const wxColour liveCapturingStopBitmapColor = wxColour(237, 28, 36);

	wxBitmap bitmap = wxMaterialDesignArtProvider::GetBitmap
	(
		liveCapturingBitmap,
		liveCapturingClient,
		bitmapSize,
		isCapturing ? liveCapturingStopBitmapColor : liveCapturingStartBitmapColor
	);
	bitmap.SetScaleFactor(GetDPIScaleFactor());
	return bitmap;
}

void cMain::ApplyDesiredEnergyRangeFromControls()
{
	if (!m_PreviewPanel || !m_MinRangeKEVTxtCtrl || !m_MaxRangeKEVTxtCtrl)
		return;

	double minKeV{}, maxKeV{};
	if (!m_MinRangeKEVTxtCtrl->GetValue().ToDouble(&minKeV))
		return;
	if (!m_MaxRangeKEVTxtCtrl->GetValue().ToDouble(&maxKeV))
		return;

	if (minKeV > maxKeV)
	{
		std::swap(minKeV, maxKeV);

		minKeV = std::max(minKeV, 0.0);
		maxKeV = std::min(maxKeV, m_PreviewPanel->GetMaxEnergyKeV());

		m_MinRangeKEVTxtCtrl->ChangeValue(wxString::Format(wxT("%.3f"), minKeV));
		m_MaxRangeKEVTxtCtrl->ChangeValue(wxString::Format(wxT("%.3f"), maxKeV));
	}

	m_PreviewPanel->SetHardEnergyRange(minKeV, maxKeV);

	SaveInitializationFile();
}

void cMain::UpdateDesiredEnergyRangeControlsToFullData()
{
	if (!m_PreviewPanel || !m_MinRangeKEVTxtCtrl || !m_MaxRangeKEVTxtCtrl)
		return;

	const double maxKeV = m_PreviewPanel->GetMaxEnergyKeV();

	m_MinRangeKEVTxtCtrl->Enable();
	m_MaxRangeKEVTxtCtrl->Enable();

	if (!m_InitializationLoaded)
	{
		m_MinRangeKEVTxtCtrl->ChangeValue(wxT("0.000"));
		m_MaxRangeKEVTxtCtrl->ChangeValue(wxString::Format(wxT("%.3f"), maxKeV));
		m_PreviewPanel->ResetHardEnergyRangeToFullData();
		return;
	}

	double savedMinKeV{}, savedMaxKeV{};
	if (!m_MinRangeKEVTxtCtrl->GetValue().ToDouble(&savedMinKeV))
		savedMinKeV = 0.0;
	if (!m_MaxRangeKEVTxtCtrl->GetValue().ToDouble(&savedMaxKeV))
		savedMaxKeV = maxKeV;

	if (savedMaxKeV <= 0.0)
	{
		m_MinRangeKEVTxtCtrl->ChangeValue(wxT("0.000"));
		m_MaxRangeKEVTxtCtrl->ChangeValue(wxString::Format(wxT("%.3f"), maxKeV));
		m_PreviewPanel->ResetHardEnergyRangeToFullData();
		return;
	}

	RestoreDesiredEnergyRangeFromControls();
}

void cMain::ApplyDarkModeState(bool enabled)
{
	if (!m_MenuBar)
		return;

	m_MenuBar->menu_window->Check(MainFrameVariables::ID::MENUBAR_WINDOW_ENABLE_DARK_MODE, enabled);

	auto appearanceColor = enabled ? m_DarkModeAppearanceColor : m_DefaultAppearanceColor;

	if (m_LeftSidePanel)    m_LeftSidePanel->SetBackgroundColour(appearanceColor);
	if (m_RightSidePanel)   m_RightSidePanel->SetBackgroundColour(appearanceColor);
	if (m_MainSplitter)     m_MainSplitter->SetBackgroundColour(appearanceColor);

	if (m_PreviewPanel)     m_PreviewPanel->SetBackgroundColor(appearanceColor);

	if (m_VerticalToolBar && m_VerticalToolBar->tool_bar)
		m_VerticalToolBar->tool_bar->SetBackgroundColour(appearanceColor);

	if (m_MotorControlsBook)
		m_MotorControlsBook->SetBackgroundColour(appearanceColor);

	if (m_NativeMotorControlsPage)
		m_NativeMotorControlsPage->SetBackgroundColour(appearanceColor);

	if (m_MotorWebPage)
		m_MotorWebPage->SetBackgroundColour(appearanceColor);

	if (m_DetectorControlsNotebook)    m_DetectorControlsNotebook->SetBackgroundColour(appearanceColor);
	if (m_OpticsControlsNotebook)      m_OpticsControlsNotebook->SetBackgroundColour(appearanceColor);
	if (m_DeviceControlsNotebook)      m_DeviceControlsNotebook->SetBackgroundColour(appearanceColor);
	if (m_MeasurementControlsNotebook) m_MeasurementControlsNotebook->SetBackgroundColour(appearanceColor);
	if (m_AuxControlsNotebook)         m_AuxControlsNotebook->SetBackgroundColour(appearanceColor);

	Refresh();
	Update();
}

void cMain::InitializeAppearanceFromSystemAndConfig()
{
	if (!LoadInitializationFile())
	{
		const bool systemDarkMode = wxSystemSettings::GetAppearance().IsDark();
		ApplyDarkModeState(systemDarkMode);
		SaveInitializationFile();
	}
}

void cMain::RestoreDesiredEnergyRangeFromControls()
{
	if (!m_PreviewPanel || !m_MinRangeKEVTxtCtrl || !m_MaxRangeKEVTxtCtrl)
		return;

	double minKeV{}, maxKeV{};
	if (!m_MinRangeKEVTxtCtrl->GetValue().ToDouble(&minKeV))
		return;
	if (!m_MaxRangeKEVTxtCtrl->GetValue().ToDouble(&maxKeV))
		return;

	if (maxKeV <= 0.0)
		return;

	const double panelMaxKeV = m_PreviewPanel->GetMaxEnergyKeV();
	if (panelMaxKeV <= 0.0)
		return;

	minKeV = std::clamp(minKeV, 0.0, panelMaxKeV);
	maxKeV = std::clamp(maxKeV, 0.0, panelMaxKeV);

	if (maxKeV <= minKeV)
	{
		m_MinRangeKEVTxtCtrl->ChangeValue(wxT("0.000"));
		m_MaxRangeKEVTxtCtrl->ChangeValue(wxString::Format(wxT("%.3f"), panelMaxKeV));
		m_PreviewPanel->ResetHardEnergyRangeToFullData();
		return;
	}

	m_MinRangeKEVTxtCtrl->ChangeValue(wxString::Format(wxT("%.3f"), minKeV));
	m_MaxRangeKEVTxtCtrl->ChangeValue(wxString::Format(wxT("%.3f"), maxKeV));
	m_PreviewPanel->SetHardEnergyRange(minKeV, maxKeV);
}

bool cMain::Cancelled()
{
	wxCriticalSectionLocker lock(m_CSCancelled);
	return m_Cancelled;
}

void cMain::OnCursorOverlayCheck(wxCommandEvent& evt)
{
	m_IsCursorOverlayActive = m_MenuBar->menu_tools->IsChecked(MainFrameVariables::ID::MENUBAR_TOOLS_CURSOR_OVERLAY);
	m_PreviewPanel->SetCursorOverlayActive(m_IsCursorOverlayActive);
}

void cMain::ExposureValueChanged(wxCommandEvent& evt)
{
	SaveInitializationFile();

	if (!m_StartStopLiveCapturingTglBtn->GetValue())
		return;

	StartLiveCapturing();
}

void cMain::OnXPosCrossHairTextCtrl(wxCommandEvent& evt)
{
	//wxString str_x_pos = m_CrossHairPosXTxtCtrl->IsEmpty() ? wxString("1") : m_CrossHairPosXTxtCtrl->GetValue();
	//int x_pos = wxAtoi(str_x_pos);
	//m_PreviewPanel->SetXCrossHairPosFromParentWindow(x_pos);
}

void cMain::OnYPosCrossHairTextCtrl(wxCommandEvent& evt)
{
	//wxString str_y_pos = m_CrossHairPosYTxtCtrl->IsEmpty() ? wxString("1") : m_CrossHairPosYTxtCtrl->GetValue();
	//int y_pos = wxAtoi(str_y_pos);
	//m_PreviewPanel->SetYCrossHairPosFromParentWindow(y_pos);
}

auto cMain::OnSetPosCrossHairTglBtn(wxCommandEvent& evt) -> void
{
}
