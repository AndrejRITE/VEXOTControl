#include "cMain.h"

wxBEGIN_EVENT_TABLE(cMain, wxFrame)
	EVT_MENU(MainFrameVariables::ID::MENUBAR_FILE_OPEN, cMain::OnOpenMCAFile)
	EVT_CLOSE(cMain::OnExit)
	EVT_MENU(MainFrameVariables::ID::MENUBAR_FILE_QUIT, cMain::OnExit)
	EVT_MENU(MainFrameVariables::ID::RIGHT_DEVICE_SINGLE_SHOT_BTN, cMain::OnSingleShotCameraImage)
	EVT_MENU(MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN, cMain::OnStartStopLiveCapturingMenu)
	EVT_MENU(MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN, cMain::OnStartStopMeasurementMenu)
	EVT_MENU(MainFrameVariables::ID::MENUBAR_EDIT_ENABLE_DARK_MODE, cMain::OnEnableDarkMode)
	EVT_MENU(MainFrameVariables::ID::MENUBAR_EDIT_SETTINGS, cMain::OnOpenSettings)
	EVT_MENU(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, cMain::OnCrossHairButton)
	EVT_MENU(MainFrameVariables::ID::MENUBAR_TOOLS_VALUE_DISPLAYING, cMain::OnValueDisplayingCheck)
	EVT_MENU(MainFrameVariables::ID::MENUBAR_WINDOW_FULLSCREEN, cMain::OnFullScreen)
	EVT_MENU(MainFrameVariables::ID::MENUBAR_HELP_APPS_VERSION, cMain::OnApplicationVersion)
	EVT_MAXIMIZE(cMain::OnMaximizeButton)
	/* Detector X */
	EVT_TEXT_ENTER(MainFrameVariables::ID::RIGHT_SC_DET_X_ABS_TE_CTL, cMain::OnEnterTextCtrlDetectorXAbsPos)
	EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_SET_BTN, cMain::OnSetDetectorXAbsPos)
	EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_DEC_BTN, cMain::OnDecrementDetectorXAbsPos)
	EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_INC_BTN, cMain::OnIncrementDetectorXAbsPos)
	EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_CENTER_BTN, cMain::OnCenterDetectorX)
	EVT_BUTTON(MainFrameVariables::ID::RIGHT_SC_DET_X_HOME_BTN, cMain::OnHomeDetectorX)
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
wxEND_EVENT_TABLE()

cMain::cMain(const wxString& title_) 
	: wxFrame(NULL, wxID_ANY, title_), m_AppName(title_)
{
	wxArtProvider::Push(new wxMaterialDesignArtProvider);

	CreateMainFrame();
	InitDefaultStateWidgets();

	SetIcon(logo_xpm);
	SetMinSize(wxSize(800, 600));

	InitializeAppearanceFromSystemAndConfig();

	/* Creating, but not showing ProgressBar */
	CreateProgressBar();
	m_ProgressBar->SetIcon(logo_xpm);

	CenterOnScreen();
	Show();
	Maximize();

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
	CreateVerticalToolBar();
	CreateStatusBar();

	CreateLeftAndRightSide();
}

void cMain::InitComponents()
{
	/* Settings Frame */
	m_Settings = std::make_unique<cSettings>(this);
	//m_Settings->SetIcon(logo_xpm);
	/* Measurement */
	m_FirstStage = std::make_unique<MainFrameVariables::MeasurementStage>();

#ifdef USE_2_AXIS_MEASUREMENT
	m_SecondStage = std::make_unique<MainFrameVariables::MeasurementStage>();
#endif
}

void cMain::CreateMenuBarOnFrame()
{
	m_MenuBar = std::make_unique<MainFrameVariables::MenuBar>();
	SetMenuBar(m_MenuBar->menu_bar);

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
			auto item = new wxMenuItem(m_MenuBar->menu_edit, itemID, wxT("Single Shot\tS"));

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

		m_MenuBar->menu_edit->AppendCheckItem(MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN, wxT("Start/Stop Live Capturing\tL"));
		m_MenuBar->menu_edit->Enable(MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN, false);

		m_MenuBar->menu_edit->AppendCheckItem(MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN, wxT("Start/Stop Measurement\tM"));
		m_MenuBar->menu_edit->Enable(MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN, false);

		m_MenuBar->menu_edit->AppendCheckItem(MainFrameVariables::ID::MENUBAR_EDIT_ENABLE_DARK_MODE, wxT("Dark Mode"));

		// Settings
		{
			auto itemID = MainFrameVariables::MENUBAR_EDIT_SETTINGS;
			auto item = new wxMenuItem(m_MenuBar->menu_edit, itemID, wxT("Settings\tF2"));

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

	// Intensity Profile SubMenu
	m_MenuBar->submenu_intensity_profile->AppendCheckItem(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, wxT("Crosshair\tC"));
	// Append Submenu Selection Tools to the Tools Menu
	m_MenuBar->menu_tools->Append(wxID_ANY, wxT("&Intensity Profile"), m_MenuBar->submenu_intensity_profile);
	// Append Value Displaying Check
	m_MenuBar->menu_tools->Append(MainFrameVariables::ID::MENUBAR_TOOLS_VALUE_DISPLAYING, wxT("Value Displaying\tV"), wxEmptyString, wxITEM_CHECK);

	// Append Tools Menu to the Menu Bar
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
				"Application Version"
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
	}

	m_MenuBar->menu_bar->Append(m_MenuBar->menu_help, wxT("&Help"));
}

void cMain::InitDefaultStateWidgets()
{
	m_MenuBar->menu_tools->Check(MainFrameVariables::ID::MENUBAR_TOOLS_VALUE_DISPLAYING, true);
	m_PreviewPanel->SetValueDisplayingActive(true);
	m_IsValueDisplayingChecked = true;

	m_MenuBar->menu_tools->Enable(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, false);
	m_VerticalToolBar->tool_bar->EnableTool(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, false);

	float default_absolute_value{ 0.0f }, default_relative_value{ 1.0f }, default_relative_value_pitch_yaw{ 0.1f };
	/* Default Detector Widgets */
	{
		/* X */
		{
			m_Detector[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Detector[0].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
			//m_X_Detector->DisableAllControls();
		}
	}
	/* Default Optics Widgets */
	{
		/* X */
		{
			m_Optics[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[0].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
			//m_Optics[0].DisableAllControls();
		}
		/* Y */
		{
			m_Optics[1].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[1].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
			//m_Y_Optics->DisableAllControls();
		}
		/* Z */
		{
			m_Optics[2].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[2].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value));
			//m_Optics[0].DisableAllControls();
		}
		/* Pitch */
		{
			m_Optics[3].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[3].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value_pitch_yaw));
			//m_Optics[0].DisableAllControls();
		}
		/* Yaw */
		{
			m_Optics[4].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_absolute_value));
			m_Optics[4].relative_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), default_relative_value_pitch_yaw));
			//m_Optics[0].DisableAllControls();
		}
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
			m_FirstStage->DisableAllControls();
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

	wxSize sizeOfPreviewWindow = { 600, height_left_and_right_panels };
	wxSize sizeOfRightSide = { 350, height_left_and_right_panels };

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
	CreateSteppersControl(parent, sizer);
	CreateDeviceControls(parent, sizer);

	CreateMeasurement(parent, sizer);
}

auto cMain::CreateSteppersControl(wxWindow* right_side_panel, wxSizer* right_side_panel_sizer) -> void
{
	wxSize absTxtCtrlSize = { 54, 20 }, relTxtCtrlSize = { absTxtCtrlSize };
	wxSize setBtnSize = { 35, 20 };
	wxSize incDecSize = { 20, 20 };

	auto size = wxSize(16, 16);
	auto imageListDetector = new wxImageList(size.GetWidth(), size.GetHeight(), true);
	auto imageListOptics = new wxImageList(size.GetWidth(), size.GetHeight(), true);

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

	m_DetectorControlsNotebook = new wxNotebook(right_side_panel, wxID_ANY);

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

	right_side_panel_sizer->Add(m_DetectorControlsNotebook, 0, wxEXPAND | wxALL, 5);

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

	m_OpticsControlsNotebook = new wxNotebook(right_side_panel, wxID_ANY);

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

	right_side_panel_sizer->Add(m_OpticsControlsNotebook, 0, wxEXPAND | wxALL, 5);
}

auto cMain::CreateDetectorPage
(
	wxWindow* parent, 
	const wxSize& absoluteTxtCtrlSize, 
	const wxSize& relativeTxtCtrlSize, 
	const wxSize& setBtnSize, 
	const wxSize& incrementDecrementBtnSize, 
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

				m_Detector[0].set_btn = new wxButton(
					page, 
					MainFrameVariables::ID::RIGHT_SC_DET_X_SET_BTN, 
					wxT("Set"), 
					wxDefaultPosition, 
					setBtnSize);
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

				m_Optics[0].set_btn = new wxButton(
					page, 
					MainFrameVariables::ID::RIGHT_SC_OPT_X_SET_BTN, 
					wxT("Set"), 
					wxDefaultPosition, 
					setBtnSize);
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

				m_Optics[1].set_btn = new wxButton(
					page, 
					MainFrameVariables::ID::RIGHT_SC_OPT_Y_SET_BTN, 
					wxT("Set"), 
					wxDefaultPosition, 
					setBtnSize);
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

				m_Optics[2].set_btn = new wxButton(
					page, 
					MainFrameVariables::ID::RIGHT_SC_OPT_Z_SET_BTN, 
					wxT("Set"), 
					wxDefaultPosition, 
					setBtnSize);
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

				m_Optics[3].set_btn = new wxButton(
					page, 
					MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_SET_BTN, 
					wxT("Set"), 
					wxDefaultPosition, 
					setBtnSize);
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

				m_Optics[4].set_btn = new wxButton(
					page, 
					MainFrameVariables::ID::RIGHT_SC_OPT_YAW_SET_BTN, 
					wxT("Set"), 
					wxDefaultPosition, 
					setBtnSize);
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

auto cMain::CreateDeviceControls(wxWindow* right_side_panel, wxSizer* right_side_panel_sizer) -> void
{
	auto size = wxSize(16, 16);
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
		auto bitmap = wxART_SETTINGS;
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

	m_DeviceControlsNotebook = new wxNotebook(right_side_panel, wxID_ANY);

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

	right_side_panel_sizer->Add(m_DeviceControlsNotebook, 1, wxEXPAND | wxALL, 5);
}

auto cMain::CreateDevicePage(wxWindow* parent) -> wxWindow*
{
	wxPanel* page = new wxPanel(parent);
	wxSizer* sizerPage = new wxBoxSizer(wxVERTICAL);

	auto gridSizer = new wxGridSizer(2);
	gridSizer->SetVGap(5);

	auto txtCtrlSize = wxSize(64, 20);

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

			auto size = wxSize(32, 32);

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
			}
			
			m_SingleShotBtn = std::make_unique<wxBitmapButton>(
				page,
				MainFrameVariables::ID::RIGHT_DEVICE_SINGLE_SHOT_BTN,
				bmp
				);

			m_SingleShotBtn->SetToolTip(wxT("Single Shot (S)\nCapture single data row and save them on disk"));
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

			m_StartStopLiveCapturingTglBtn->SetToolTip("Start/Stop live sequence of capturing data (L)");

			m_StartStopLiveCapturingTglBtn->Disable();

			// Force a tighter size (still may have minimum imposed by OS)
			const auto bsz = bmp.GetSize();
			m_StartStopLiveCapturingTglBtn->SetMinSize(wxSize(bsz.x + 6, bsz.y + 6));

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
			m_ExposureProgressPanel->SetMinSize(wxSize(-1, 48));
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

auto cMain::CreateMeasurement(wxWindow* right_side_panel, wxSizer* right_side_panel_sizer) -> void
{
	auto size = wxSize(16, 16);
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

	m_MeasurementControlsNotebook = new wxNotebook(right_side_panel, wxID_ANY);

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

	right_side_panel_sizer->Add(m_MeasurementControlsNotebook, 0, wxEXPAND | wxALL, 5);

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

		auto size = wxSize(32, 32);

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

	wxSize start_text_ctrl_size = { 54, 20 }, step_text_ctrl_size = {start_text_ctrl_size}, finish_text_ctrl_size{start_text_ctrl_size};

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

				start_static_box_sizer->Add(m_FirstStage->start, 0, wxEXPAND);

				first_axis_static_box_sizer->Add(start_static_box_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 2);
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
				
				step_static_box_sizer->Add(m_FirstStage->step, 0, wxEXPAND);
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

				finish_static_box_sizer->Add(m_FirstStage->finish, 0, wxEXPAND);
				first_axis_static_box_sizer->Add(finish_static_box_sizer, 0, wxEXPAND);
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

				start_static_box_sizer->Add(m_SecondStage->start, 0, wxEXPAND);

				second_axis_static_box_sizer->Add(start_static_box_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 2);
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
				
				step_static_box_sizer->Add(m_SecondStage->step, 0, wxEXPAND);
				second_axis_static_box_sizer->Add(step_static_box_sizer, 0, wxEXPAND | wxRIGHT, 2);
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

				finish_static_box_sizer->Add(m_SecondStage->finish, 0, wxEXPAND);
				second_axis_static_box_sizer->Add(finish_static_box_sizer, 0, wxEXPAND);
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

		m_StartStopMeasurementTglBtn->SetToolTip("Start/Stop Measurement (M)\nRepeatedly move the stage, capture, and save images to disk.");

		m_StartStopMeasurementTglBtn->Disable();

		m_StartStopMeasurementTglBtn->SetMinSize(wxSize(bsz.x + 6, bsz.y + 6));

		capturing_sizer->Add(m_StartStopMeasurementTglBtn.get(), 0, wxALIGN_CENTER);

		sizerPage->Add(capturing_sizer, 0, wxALIGN_RIGHT);
	}

	page->SetSizer(sizerPage);
	return page;
}

auto cMain::OnEnableDarkMode(wxCommandEvent& evt) -> void
{
	const bool isDarkModeEnabled =
		m_MenuBar->menu_edit->IsChecked(MainFrameVariables::ID::MENUBAR_EDIT_ENABLE_DARK_MODE);

	ApplyDarkModeState(isDarkModeEnabled);
	SaveInitializationFile();
}

void cMain::CreateProgressBar()
{
	RestoreProgressWindowGeometry();

	const wxPoint pos =
		(m_ProgressWindowPosition == wxDefaultPosition)
		? GetDefaultProgressWindowPosition()
		: m_ProgressWindowPosition;

	m_ProgressBar = std::make_unique<ProgressBar>(this, pos, m_ProgressWindowSize);
	m_ProgressBar->SetIcon(logo_xpm);

#ifndef _DEBUG
	m_ProgressBar->Hide();
#endif // !_DEBUG
}

void cMain::OnSingleShotCameraImage(wxCommandEvent& evt)
{
	auto outDir = m_OutDirTextCtrl->GetValue();
	while (!wxDir::Exists(outDir))
	{
		wxCommandEvent artEvt(wxEVT_BUTTON, MainFrameVariables::ID::RIGHT_MT_OUT_FLD_BTN);
		ProcessEvent(artEvt);
		outDir = m_OutDirTextCtrl->GetValue();
	}

	wxString exposure_time_str = m_DeviceExposure->GetValue().IsEmpty()
		? wxString("1")
		: m_DeviceExposure->GetValue();
	const int exposure_time = std::max(1, std::abs(wxAtoi(exposure_time_str)));

	const bool restartLiveAfterSingleShot = m_StartStopLiveCapturingTglBtn->GetValue();

	if (restartLiveAfterSingleShot)
	{
		m_StartStopLiveCapturingTglBtn->SetValue(false);
		wxCommandEvent stopEvt(wxEVT_TOGGLEBUTTON, MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN);
		ProcessEvent(stopEvt);
	}

	auto timePointToWxString = []()
		{
			auto now = std::chrono::system_clock::now().time_since_epoch().count();
			return wxString::Format(wxT("%lld"), now);
		};

	auto currThreadTimeStamp = timePointToWxString();
	m_StartedThreads.push_back(std::make_pair(currThreadTimeStamp, true));

	ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::SingleShotRunning);

	ShowExposureProgressControls();

	SingleShotThread* singleShotThread = new SingleShotThread
	(
		this,
		m_KetekHandler.get(),
		&m_StartedThreads.back().first,
		&m_StartedThreads.back().second,
		outDir,
		exposure_time
	);

	if (singleShotThread->Create(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
	{
		delete singleShotThread;
		m_StartedThreads.back().second = false;
		m_StartedThreads.back().first.clear();
		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);

		return;
	}

	if (singleShotThread->Run() != wxTHREAD_NO_ERROR)
	{
		delete singleShotThread;
		m_StartedThreads.back().second = false;
		m_StartedThreads.back().first.clear();

		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
		return;
	}
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
	m_PreviewPanel->SetFocus();
	auto code = m_Settings->ShowModal();

	if (code == wxID_OK)
	{
		//InitializeSelectedCamera();
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

	m_KetekHandler = std::make_unique<Ketek>(m_Settings->GetSelectedKETEK().ToStdString());
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
#ifndef _DEBUG
	if (evt.CanVeto())
	{
		if (wxMessageBox("Are you sure?",
			"Exit application",
			wxICON_QUESTION | wxYES_NO) != wxYES)
		{
			evt.Veto();
			return;
		}
	}
#endif // !_DEBUG

	{
		wxString exposure_time_str = m_DeviceExposure->GetValue().IsEmpty() 
			? wxString("0") 
			: m_DeviceExposure->GetValue();

		unsigned long exposure_time = abs(wxAtoi(exposure_time_str)); // UI value is in seconds
		wxThread::This()->Sleep(exposure_time);
	}

	SaveInitializationFile();
	SaveProgressWindowGeometry();

	Destroy();  // you may also do:  event.Skip();
	evt.Skip();
	// since the default event handler does call Destroy(), too
}

void cMain::UpdateStagePositions()
{
	m_Detector[0].absolute_text_ctrl->SetValue
	(
		wxString::Format(wxT("%.3f"), 
			m_Settings->GetActualMotorPosition(SettingsVariables::DETECTOR_X))
	);

	m_Optics[0].absolute_text_ctrl->SetValue
	(
		wxString::Format(wxT("%.3f"), 
			m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_X))
	);

	m_Optics[1].absolute_text_ctrl->SetValue
	(
		wxString::Format(wxT("%.3f"), 
			m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_Y))
	);

	m_Optics[2].absolute_text_ctrl->SetValue
	(
		wxString::Format(wxT("%.3f"), 
			m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_Z))
	);

	m_Optics[3].absolute_text_ctrl->SetValue
	(
		wxString::Format(wxT("%.3f"), 
			m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_PITCH))
	);

	m_Optics[4].absolute_text_ctrl->SetValue
	(
		wxString::Format(wxT("%.3f"), 
			m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_YAW))
	);
}

void cMain::OnExit(wxCommandEvent& evt)
{
	wxCloseEvent artificialExit(wxEVT_CLOSE_WINDOW);
	ProcessEvent(artificialExit);
}

void cMain::OnStartStopMeasurementMenu(wxCommandEvent& evt)
{
	if (!m_StartStopMeasurementTglBtn)
		return;

	const bool checked =
		m_MenuBar &&
		m_MenuBar->menu_edit &&
		m_MenuBar->menu_edit->IsChecked(MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN);

	m_StartStopMeasurementTglBtn->SetValue(checked);

	wxCommandEvent toggleEvt
	(
		wxEVT_TOGGLEBUTTON,
		MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN
	);

	ProcessEvent(toggleEvt);
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

void cMain::EnableUsedAndDisableNonUsedMotors()
{
	/* Detector X */
	if (m_Settings->MotorHasSerialNumber(SettingsVariables::DETECTOR_X)) m_Detector[0].EnableAllControls();
	else m_Detector[0].DisableAllControls();

	/* Optics X */
	if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_X)) m_Optics[0].EnableAllControls();
	else m_Optics[0].DisableAllControls();
	/* Optics Y */
	if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_Y)) m_Optics[1].EnableAllControls();
	else m_Optics[1].DisableAllControls();
	/* Optics Z */
	if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_Z)) m_Optics[2].EnableAllControls();
	else m_Optics[2].DisableAllControls();	
	/* Optics Pitch */
	if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_PITCH)) m_Optics[3].EnableAllControls();
	else m_Optics[3].DisableAllControls();	
	/* Optics Yaw */
	if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_YAW)) m_Optics[4].EnableAllControls();
	else m_Optics[4].DisableAllControls();
}

void cMain::CreateVerticalToolBar()
{
	m_VerticalToolBar = std::make_unique<MainFrameVariables::ToolBar>();
	m_VerticalToolBar->tool_bar = new wxToolBar
	(
		this, 
		wxID_ANY, 
		wxDefaultPosition, 
		wxDefaultSize, 
		wxTB_VERTICAL
	);

	// CrossHair
	//wxImage crosshairToolImage = wxImage(cross_hair_xpm);

	wxBitmap bmp{};
	{
		auto bitmap = wxART_CIRCLE_HINT;
		auto client = wxART_CLIENT_FLUENTUI_FILLED;
		auto color = wxColour(255, 128, 128);
		auto size = wxSize(16, 16);
		bmp = wxMaterialDesignArtProvider::GetBitmap
		(
			bitmap,
			client,
			size,
			color
		);
	}

	//wxBitmap crosshairToolBitmap = wxBitmap(crosshairToolImage);
	m_VerticalToolBar->tool_bar->AddCheckTool(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, _("Crosshair"), bmp);
	//m_VerticalToolBar->tool_bar->SetToolShortHelp(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, wxT("Crosshair (C)"));

	//m_VerticalToolBar->tool_bar->SetToolBitmapSize(wxSize(30, 30));
	m_VerticalToolBar->tool_bar->Realize();
}

auto cMain::CreateStatusBar() -> void
{
	m_StatusBar = std::make_unique<wxStatusBar>(this, wxID_ANY);
	SetStatusBar(m_StatusBar.get());
}

auto cMain::LiveCapturingFinishedCapturingAndDrawing(bool is_finished) -> void
{
	//m_LiveCapturingEndedDrawingOnCamPreview = is_finished;
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
	/* Unchecking CrossHair Button */
	m_VerticalToolBar->tool_bar->ToggleTool(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, false);
	m_MenuBar->menu_tools->Check(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, false);
	//m_CamPreview->SetCrossHairButtonActive(false);
	//m_CrossHairPosXTxtCtrl->Disable();
	//m_CrossHairPosYTxtCtrl->Disable();
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

void cMain::OnStartStopCapturingButton(wxCommandEvent& evt)
{
	if (!m_KetekHandler->IsDeviceInitialized())
	{
		wxLogError("There is no connected device for capturing.");
		return;
	}

	if (!std::filesystem::exists(m_OutDirTextCtrl->GetValue().ToStdString()) ||
		!std::filesystem::is_directory(m_OutDirTextCtrl->GetValue().ToStdString()))
	{
		wxLogError("Desired path doesn't exist.\nPlease, change the output directory to existing path and try again.");
		return;
	}

	if (m_StartStopLiveCapturingTglBtn->GetValue())
	{
		m_StartStopLiveCapturingTglBtn->SetValue(false);
		wxCommandEvent liveEvt(wxEVT_TOGGLEBUTTON, MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN);
		ProcessEvent(liveEvt);
	}

	// START measurement
	if (m_StartStopMeasurementTglBtn->GetValue())
	{
		if (!StartCapturing())
		{
			m_StartStopMeasurementTglBtn->SetValue(false);

			if (m_MenuBar && m_MenuBar->menu_edit)
			{
				m_MenuBar->menu_edit->Check
				(
					MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN,
					false
				);
			}

			ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
			return;
		}

		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::MeasurementRunning);
	}
	// STOP measurement
	else
	{
		if (m_StartedThreads.size())
		{
			m_StartedThreads.back().second = false;
			m_StartStopMeasurementTglBtn->Disable();

			while (!m_StartedThreads.back().first.empty())
				wxThread::This()->Sleep(100);

			m_StartStopMeasurementTglBtn->Enable();
		}

		if (m_PreviewPanel)
			m_PreviewPanel->SetPerformanceOverlayEnabled(false);

		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
		EndExposureProgress();
	}
}

auto cMain::StartCapturing() -> bool
{
	constexpr auto raise_exception_msg = [](wxString axis) 
	{
		wxString title = "Finish position error";
		wxMessageBox(
			wxT
			(
				"Finish position of " + axis + " axis is not correct!"
				"\nPlease, check if STEP and FINISH values are correct!"
			),
			title,
			wxICON_ERROR);
	};

	auto timePointToWxString = []()
		{
			auto now = std::chrono::system_clock::now().time_since_epoch().count();
			wxString formattedTime = wxString::Format(wxT("%lld"), now);
			return formattedTime;
		};

	wxString exposure_time_str = m_DeviceExposure->GetValue().IsEmpty() 
		? wxString("1") 
		: m_DeviceExposure->GetValue();
	auto exposureSeconds = abs(wxAtoi(exposure_time_str)); // UI value is in seconds

	auto first_axis = std::make_unique<MainFrameVariables::AxisMeasurement>();
	auto second_axis = std::make_unique<MainFrameVariables::AxisMeasurement>();
	double start_first_stage_value{}, step_first_stage_value{}, finish_first_stage_value{};
	double start_second_stage_value{}, step_second_stage_value{}, finish_second_stage_value{};
	int first_stage_step_count{}, second_stage_step_count{};
	/* Checking if user selected None as a stage */
	{
		/* Checking first stage */
		{
			if (m_FirstStage->stage->GetCurrentSelection() == 0) return false;
			else
			{
				first_axis->axis_number = m_FirstStage->stage->GetCurrentSelection() - 1;
			}
		}
		/* Checking Start, Step and Finish values */
		{
			if (!m_FirstStage->start->GetValue().ToDouble(&start_first_stage_value)) return false;
			first_axis->start = (int)(start_first_stage_value * 1000.0) / 1000.f;
			if (!m_FirstStage->step->GetValue().ToDouble(&step_first_stage_value)) return false;
			first_axis->step = (int)(step_first_stage_value * 1000.0) / 1000.f;
			if (!m_FirstStage->finish->GetValue().ToDouble(&finish_first_stage_value)) return false;
			first_axis->finish = (int)(finish_first_stage_value * 1000.0) / 1000.f;
			if (
				(finish_first_stage_value - start_first_stage_value < 0.0 && step_first_stage_value > 0.0)
				|| (finish_first_stage_value - start_first_stage_value > 0.0 && step_first_stage_value < 0.0)
				) 
				raise_exception_msg("first");
			first_axis->step_number = ((int)(finish_first_stage_value * 1000.0) - 
				(int)(start_first_stage_value * 1000.0)) / 
				(int)(step_first_stage_value * 1000.0) + 1;
		}

#ifdef USE_2_AXIS_MEASUREMENT
		/* Checking second stage */
		if (m_SecondStage->stage->GetCurrentSelection() - 1 == first_axis->axis_number) return false;
		/* 
		if (m_SecondStage->stage->GetCurrentSelection() == 0) return;
		else selected_second_stage = m_SecondStage->stage->GetCurrentSelection() - 1;		
		*/
#endif
	}
	{
		m_StartCalculationTime = std::chrono::steady_clock::now();

		if (m_ProgressBar)
		{
			m_ProgressBar->Show();
			m_ProgressBar->Raise();
		}

		m_Settings->ResetCapturing();
		m_ProgressBar->Show();

		m_AppProgressIndicator = std::make_unique<wxAppProgressIndicator>(this, 100);

		//this->Disable();
		//m_StartMeasurement->Disable();
	}

	ShowExposureProgressControls("Measurement Exposure: 0%");

	if (m_PreviewPanel)
	{
		m_PreviewPanel->ResetFrameStats();
		m_PreviewPanel->SetPerformanceOverlayEnabled(true, static_cast<double>(exposureSeconds));
	}

	auto currThreadTimeStamp = timePointToWxString();
	m_StartedThreads.push_back(std::make_pair(currThreadTimeStamp, true));
	/* Worker and Progress Threads */
	{
		auto out_dir = m_OutDirTextCtrl->GetValue();

		WorkerThread* worker_thread = new WorkerThread
		(
			this,
			m_Settings.get(),
			m_KetekHandler.get(),
			&m_StartedThreads.back().first,
			&m_StartedThreads.back().second,
			out_dir,
			exposureSeconds,
			first_axis.release(), 
			second_axis.release(),
			m_GraphFontSize
		);

		ProgressThread* progress_thread = new ProgressThread(this, m_Settings.get(), &m_StartedThreads.back().second);

		if (worker_thread->Create(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
		{
			delete worker_thread;
			worker_thread = nullptr;
			return false;
		}
		if (progress_thread->Create(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
		{
			delete progress_thread;
			progress_thread = nullptr;
			return false;
		}
		if (progress_thread->Run() != wxTHREAD_NO_ERROR)
		{
			delete progress_thread;
			progress_thread = nullptr;
			return false;
		}
		if (worker_thread->Run() != wxTHREAD_NO_ERROR)
		{
			delete progress_thread;
			progress_thread = nullptr;
			delete worker_thread;
			worker_thread = nullptr;
			return false;
		}
	}
	return true;
}

void cMain::StartLiveCapturing()
{
	auto timePointToWxString = []()
		{
			auto now = std::chrono::system_clock::now().time_since_epoch().count();
			wxString formattedTime = wxString::Format(wxT("%lld"), now);
			return formattedTime;
		};

	wxBusyCursor cursor;
	if (m_StartedThreads.size() && m_StartedThreads.back().second)
	{
		m_StartedThreads.back().second = false;
		while (!m_StartedThreads.back().first.empty())
			wxThread::This()->Sleep(100);
	}

	wxString exposure_time_str = m_DeviceExposure->GetValue().IsEmpty() 
		? wxString("1") 
		: m_DeviceExposure->GetValue();
	auto exposureSeconds = abs(wxAtoi(exposure_time_str)); // UI value is in seconds

	ShowExposureProgressControls("Live Exposure: 0%");

	if (m_PreviewPanel)
	{
		m_PreviewPanel->ResetFrameStats();
		m_PreviewPanel->SetPerformanceOverlayEnabled(true, static_cast<double>(exposureSeconds));
	}

	auto currThreadTimeStamp = timePointToWxString();
	m_StartedThreads.push_back(std::make_pair(currThreadTimeStamp, true));

	LiveCapturing* live_capturing = new LiveCapturing
	(
		this, 
		m_KetekHandler.get(), 
		&m_StartedThreads.back().first, 
		&m_StartedThreads.back().second,
		exposureSeconds
	);

	if (live_capturing->Create(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
	{
		delete live_capturing;
		live_capturing = nullptr;
		return;
	}
	if (live_capturing->Run() != wxTHREAD_NO_ERROR)
	{
		delete live_capturing;
		live_capturing = nullptr;
		return;
	}
}

void cMain::ChangeCameraManufacturerChoice(wxCommandEvent& evt)
{
	wxCommandEvent simulate_change_exposure_value(wxEVT_TEXT_ENTER, MainFrameVariables::ID::RIGHT_CAM_EXPOSURE_TXT_CTL);
	ProcessEvent(simulate_change_exposure_value);
}

void cMain::OnCrossHairButton(wxCommandEvent& evt)
{
	UnCheckAllTools();
	if (!m_IsCrossHairChecked)
	{
		m_MenuBar->menu_tools->Check(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, true);
		m_VerticalToolBar->tool_bar->ToggleTool(MainFrameVariables::ID::MENUBAR_TOOLS_CROSSHAIR, true);
		m_IsCrossHairChecked = true;
		//m_PreviewPanel->SetCrossHairButtonActive(true);
		//m_CrossHairPosXTxtCtrl->Enable();
		//m_CrossHairPosYTxtCtrl->Enable();
		{
			//auto img_size = m_PreviewPanel->GetImageSize();
			//m_CamPreview->SetXCrossHairPosFromParentWindow(img_size.GetWidth() / 2);
			//m_CamPreview->SetYCrossHairPosFromParentWindow(img_size.GetHeight() / 2);
			//m_CrossHairPosXTxtCtrl->SetValue(wxString::Format(wxT("%i"), img_size.GetWidth() / 2));
			//m_CrossHairPosYTxtCtrl->SetValue(wxString::Format(wxT("%i"), img_size.GetHeight() / 2));
		}
	}
	else
	{
		m_IsCrossHairChecked = false;
	}
}

auto cMain::LiveCapturingThread(wxThreadEvent& evt) -> void
{
	auto curr_code = evt.GetInt();

	// -1 == Camera is disconnected
	if (curr_code == -1)
	{
		m_StartStopLiveCapturingTglBtn->SetValue(false);
		wxCommandEvent live_capturing_evt(wxEVT_TOGGLEBUTTON, MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN);
		ProcessEvent(live_capturing_evt);

		if (m_PreviewPanel)
			m_PreviewPanel->SetPerformanceOverlayEnabled(false);

		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);

		EndExposureProgress();

		return;
	}

	auto img_ptr = evt.GetPayload<unsigned long*>();
	auto dataSize = m_KetekHandler->GetDataSize();

	unsigned long long sum{};
	sum = std::accumulate(&img_ptr[0], &img_ptr[m_KetekHandler->GetDataSize()], sum);

	const bool preserveUserXState = m_PreviewPanel && m_PreviewPanel->HasCustomizedXDomain();

	m_PreviewPanel->NotifyNewFrame(static_cast<unsigned long long>(curr_code) + 1ULL);
	m_PreviewPanel->SetKETEKData(img_ptr, dataSize, sum);

	if (!preserveUserXState)
		UpdateDesiredEnergyRangeControlsToFullData();
}

auto cMain::WorkerThreadEvent(wxThreadEvent& evt) -> void
{
	auto curr_code = evt.GetInt();
	auto filePath = evt.GetString();

	// -1 == Camera is disconnected
	if (curr_code == -1)
	{
		if (m_CaptureUiMode == MainFrameVariables::CaptureUiMode::MeasurementRunning)
		{
			m_StartStopMeasurementTglBtn->SetValue(false);
			wxCommandEvent measurement_capturing_evt(wxEVT_TOGGLEBUTTON, MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN);
			ProcessEvent(measurement_capturing_evt);
		}

		if (m_PreviewPanel)
			m_PreviewPanel->SetPerformanceOverlayEnabled(false);

		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
		EndExposureProgress();
		return;
	}

	auto img_ptr = evt.GetPayload<unsigned long*>();
	auto dataSize = m_KetekHandler->GetDataSize();

	unsigned long long sum{};
	sum = std::accumulate(&img_ptr[0], &img_ptr[m_KetekHandler->GetDataSize()], sum);

	const bool preserveUserXState = m_PreviewPanel && m_PreviewPanel->HasCustomizedXDomain();

	m_PreviewPanel->NotifyNewFrame(static_cast<unsigned long long>(curr_code) + 1ULL);
	m_PreviewPanel->SetKETEKData(img_ptr, dataSize, sum);

	if (!preserveUserXState)
		UpdateDesiredEnergyRangeControlsToFullData();

	// Save the PNG right next to the MCA
	{
		wxFileName pngPath(filePath);
		pngPath.SetExt("png");

		if (!m_PreviewPanel->SavePNG(pngPath.GetFullPath())) wxLogWarning("Failed to save preview PNG: %s", pngPath.GetFullPath());
	}

	if (m_CaptureUiMode == MainFrameVariables::CaptureUiMode::SingleShotRunning)
	{
		if (m_PreviewPanel)
			m_PreviewPanel->SetPerformanceOverlayEnabled(false);

		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
		EndExposureProgress();
	}
}

void cMain::UpdateProgress(wxThreadEvent& evt)
{
	int progress = evt.GetInt();
	wxString msg = evt.GetString();
	unsigned long long elapsed_seconds{};

	if (progress != -1)
	{
		auto current_time = std::chrono::steady_clock::now();
		elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(current_time - m_StartCalculationTime).count();
		m_ProgressBar->UpdateProgressWithMessage(msg, progress);
		m_AppProgressIndicator->SetValue(progress);
		m_ProgressBar->UpdateElapsedTime(elapsed_seconds);
		m_ProgressBar->UpdateEstimatedTime(progress, elapsed_seconds);
	}
	else
	{
		m_ProgressBar->Hide();
		m_ProgressBar->UpdateProgressWithMessage("", 0);
		m_ProgressBar->UpdateElapsedTime(0);
		m_ProgressBar->UpdateEstimatedTime(0, 0);
		m_AppProgressIndicator->~wxAppProgressIndicator();
		UpdateAllAxisGlobalPositions();
		this->Enable();
	}
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
	/* Optics */
	case 1:
		selected_axis = std::string("optics_x");
		break;
	case 2:
		selected_axis = std::string("optics_y");
		break;
	case 3:
		selected_axis = std::string("optics_z");
		break;
	case 4:
		selected_axis = std::string("optics_pitch");
		break;
	case 5:
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
	const wxSize bitmapSize = wxSize(32, 32);
	const char* liveCapturingBitmap = wxART_VIDEOCAM;
	const char* liveCapturingClient = wxART_CLIENT_MATERIAL_ROUND;
	const wxColour liveCapturingStartBitmapColor = wxColour(34, 177, 76);
	const wxColour liveCapturingStopBitmapColor = wxColour(237, 28, 36);

	return wxMaterialDesignArtProvider::GetBitmap
	(
		liveCapturingBitmap,
		liveCapturingClient,
		bitmapSize,
		isCapturing ? liveCapturingStopBitmapColor : liveCapturingStartBitmapColor
	);
}

auto cMain::GetMeasurementBitmap(const bool isCapturing) -> wxBitmap
{
	const wxSize bitmapSize = wxSize(32, 32);
	const char* liveCapturingBitmap = wxART_MOVE_UP;
	const char* liveCapturingClient = wxART_CLIENT_MATERIAL_ROUND;
	const wxColour liveCapturingStartBitmapColor = wxColour(34, 177, 76);
	const wxColour liveCapturingStopBitmapColor = wxColour(237, 28, 36);

	return wxMaterialDesignArtProvider::GetBitmap
	(
		liveCapturingBitmap,
		liveCapturingClient,
		bitmapSize,
		isCapturing ? liveCapturingStopBitmapColor : liveCapturingStartBitmapColor
	);
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

wxString cMain::GetInitializationFilePath() const
{
	const wxFileName exeFileName(wxStandardPaths::Get().GetExecutablePath());
	return exeFileName.GetPathWithSep() + m_AppName + ".ini";
}

auto cMain::CreateDefaultInitializationFileIfMissing() -> bool
{
	const wxString iniPath = GetInitializationFilePath();

	if (wxFileExists(iniPath))
		return true;

	bool systemDarkMode = false;
	if (wxSystemSettings::HasFeature(wxSYS_CAN_DRAW_FRAME_DECORATIONS))
		systemDarkMode = wxSystemSettings::GetAppearance().IsDark();
	else
		systemDarkMode = wxSystemSettings::GetAppearance().IsDark();

	nlohmann::json j =
	{
		{"last_exposure_seconds", 1},
		{"desired_range_keV",
			{
				{"min", 0.0},
				{"max", 0.0}
			}
		},
		{"dark_mode", systemDarkMode},
		{ "graph_font_size", 18 }
	};

	std::ofstream out(iniPath.ToStdString(), std::ios::out | std::ios::trunc);
	if (!out.is_open())
		return false;

	out << j.dump(4);
	return true;
}

auto cMain::LoadInitializationFile() -> bool
{
	if (!CreateDefaultInitializationFileIfMissing())
		return false;

	const wxString iniPath = GetInitializationFilePath();

	std::ifstream in(iniPath.ToStdString());
	if (!in.is_open())
		return false;

	nlohmann::json j;
	try
	{
		in >> j;
	}
	catch (const std::exception&)
	{
		return false;
	}

	const bool systemDarkMode = wxSystemSettings::GetAppearance().IsDark();

	const int exposureSeconds = j.value("last_exposure_seconds", 1);
	const bool darkMode = j.value("dark_mode", systemDarkMode);

	double minKeV = 0.0;
	double maxKeV = 0.0;

	if (j.contains("desired_range_keV") && j["desired_range_keV"].is_object())
	{
		minKeV = j["desired_range_keV"].value("min", 0.0);
		maxKeV = j["desired_range_keV"].value("max", 0.0);
	}

	const int graphFontSize = j.value("graph_font_size", 18);
	m_GraphFontSize = std::max(6, graphFontSize);

	if (m_DeviceExposure)
		m_DeviceExposure->ChangeValue(wxString::Format(wxT("%d"), std::max(1, exposureSeconds)));

	if (m_MinRangeKEVTxtCtrl)
		m_MinRangeKEVTxtCtrl->ChangeValue(wxString::Format(wxT("%.3f"), std::max(0.0, minKeV)));

	if (m_MaxRangeKEVTxtCtrl)
		m_MaxRangeKEVTxtCtrl->ChangeValue(wxString::Format(wxT("%.3f"), std::max(0.0, maxKeV)));

	ApplyDarkModeState(darkMode);

	m_InitializationLoaded = true;
	return true;
}

auto cMain::SaveInitializationFile() const -> bool
{
	const wxString iniPath = GetInitializationFilePath();

	int exposureSeconds = 1;
	if (m_DeviceExposure)
	{
		const wxString exposureStr = m_DeviceExposure->GetValue().IsEmpty()
			? wxString("1")
			: m_DeviceExposure->GetValue();

		exposureSeconds = std::max(1, std::abs(wxAtoi(exposureStr)));
	}

	double minKeV = 0.0;
	double maxKeV = 0.0;

	if (m_MinRangeKEVTxtCtrl)
		m_MinRangeKEVTxtCtrl->GetValue().ToDouble(&minKeV);

	if (m_MaxRangeKEVTxtCtrl)
		m_MaxRangeKEVTxtCtrl->GetValue().ToDouble(&maxKeV);

	const bool darkMode =
		m_MenuBar &&
		m_MenuBar->menu_edit->IsChecked(MainFrameVariables::ID::MENUBAR_EDIT_ENABLE_DARK_MODE);

	nlohmann::json j =
	{
		{"last_exposure_seconds", exposureSeconds},
		{"desired_range_keV",
			{
				{"min", std::max(0.0, minKeV)},
				{"max", std::max(0.0, maxKeV)}
			}
		},
		{"dark_mode", darkMode},
		{"graph_font_size", m_GraphFontSize}
	};

	std::ofstream out(iniPath.ToStdString(), std::ios::out | std::ios::trunc);
	if (!out.is_open())
		return false;

	out << j.dump(4);
	return true;
}

void cMain::ApplyDarkModeState(bool enabled)
{
	if (!m_MenuBar)
		return;

	m_MenuBar->menu_edit->Check(MainFrameVariables::ID::MENUBAR_EDIT_ENABLE_DARK_MODE, enabled);

	auto appearanceColor = enabled ? m_DarkModeAppearanceColor : m_DefaultAppearanceColor;

	if (m_LeftSidePanel)    m_LeftSidePanel->SetBackgroundColour(appearanceColor);
	if (m_RightSidePanel)   m_RightSidePanel->SetBackgroundColour(appearanceColor);
	if (m_MainSplitter)     m_MainSplitter->SetBackgroundColour(appearanceColor);

	if (m_PreviewPanel)     m_PreviewPanel->SetBackgroundColor(appearanceColor);

	if (m_VerticalToolBar && m_VerticalToolBar->tool_bar)
		m_VerticalToolBar->tool_bar->SetBackgroundColour(appearanceColor);

	if (m_DetectorControlsNotebook)    m_DetectorControlsNotebook->SetBackgroundColour(appearanceColor);
	if (m_OpticsControlsNotebook)      m_OpticsControlsNotebook->SetBackgroundColour(appearanceColor);
	if (m_DeviceControlsNotebook)      m_DeviceControlsNotebook->SetBackgroundColour(appearanceColor);
	if (m_MeasurementControlsNotebook) m_MeasurementControlsNotebook->SetBackgroundColour(appearanceColor);

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

void cMain::UpdateExposureProgress(wxThreadEvent& evt)
{
	const int progress = std::clamp(evt.GetInt(), 0, 100);
	const wxString label = evt.GetString();

	if (m_ExposureGauge)
		m_ExposureGauge->SetValue(progress);

	if (m_ExposureProgressStaticText)
		m_ExposureProgressStaticText->SetLabel(label);

	if (m_ExposureProgressPanel)
	{
		m_ExposureProgressPanel->Layout();
		m_ExposureProgressPanel->Refresh();
	}

	if (m_ExposureProgressPanel && m_ExposureProgressPanel->GetParent())
	{
		m_ExposureProgressPanel->GetParent()->Layout();
		m_ExposureProgressPanel->GetParent()->Refresh();
	}

	if (m_DeviceControlsNotebook)
	{
		m_DeviceControlsNotebook->Layout();
		m_DeviceControlsNotebook->Refresh();
	}

	if (m_RightSidePanel)
	{
		m_RightSidePanel->Layout();
		m_RightSidePanel->FitInside();
		m_RightSidePanel->Refresh();
	}

	Layout();
}

bool cMain::Cancelled()
{
	wxCriticalSectionLocker lock(m_CSCancelled);
	return m_Cancelled;
}

void cMain::OnValueDisplayingCheck(wxCommandEvent& evt)
{
	m_IsValueDisplayingChecked = m_MenuBar->menu_tools->IsChecked(MainFrameVariables::ID::MENUBAR_TOOLS_VALUE_DISPLAYING);
	m_PreviewPanel->SetValueDisplayingActive(m_IsValueDisplayingChecked);
}

void cMain::UpdateAllAxisGlobalPositions()
{
	/* Detectors */
	m_Detector[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::DETECTOR_X)));
	/* Optics */
	m_Optics[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_X)));
	m_Optics[1].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_Y)));
	m_Optics[2].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_Z)));
	m_Optics[3].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_PITCH)));
	m_Optics[4].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_YAW)));
}

void cMain::ExposureValueChanged(wxCommandEvent& evt)
{
	SaveInitializationFile();

	if (!m_StartStopLiveCapturingTglBtn->GetValue())
		return;

	StartLiveCapturing();
}

void cMain::OnStartStopLiveCapturingMenu(wxCommandEvent& evt)
{
	if (m_MenuBar->menu_edit->IsChecked(MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN))
	{
		m_StartStopLiveCapturingTglBtn->SetValue(true);
	}
	else
	{
		m_StartStopLiveCapturingTglBtn->SetValue(false);
	}
	wxCommandEvent art_start_live_pressed(wxEVT_TOGGLEBUTTON, MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN);
	ProcessEvent(art_start_live_pressed);
}

void cMain::OnStartStopLiveCapturingTglBtn(wxCommandEvent& evt)
{
	if (m_StartStopLiveCapturingTglBtn->GetValue())
	{
		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::LiveRunning);
		StartLiveCapturing();
	}
	else
	{	
		if (m_StartedThreads.size())
		{
			m_StartedThreads.back().second = false;

			m_StartStopLiveCapturingTglBtn->Disable();

			while (!m_StartedThreads.back().first.empty())
			{
				wxThread::This()->Sleep(100);
			}

			m_StartStopLiveCapturingTglBtn->Enable();
		}

		if (m_PreviewPanel)
			m_PreviewPanel->SetPerformanceOverlayEnabled(false);

		EndExposureProgress();
		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
	}
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

/* ___ Start Live Capturing Thread ___ */
LiveCapturing::LiveCapturing
(
	cMain* mainFrame,
	Ketek* ketekHandler,
	wxString* threadKey,
	bool* continueCapturing,
	const int exposureSeconds
) 
	: m_MainFrame(mainFrame), 
	m_KetekHandler(ketekHandler),
	m_ThreadID(threadKey),
	m_ContinueCapturing(continueCapturing),
	m_ExposureSeconds(exposureSeconds) {}

wxThread::ExitCode LiveCapturing::Entry()
{
	wxLogDebug(*m_ThreadID);
	
	wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_LIVE_CAPTURING);

	auto mcaData = std::make_unique<unsigned long[]>(m_KetekHandler->GetDataSize());

	auto imageNumber{ 0 };
	while (m_MainFrame && *m_ContinueCapturing)
	{
		auto exposureProgressRunning = std::make_shared<std::atomic_bool>(true);

		ExposureProgressThread* exposureThread = new ExposureProgressThread
		(
			m_MainFrame,
			exposureProgressRunning,
			m_ExposureSeconds,
			"Live Exposure"
		);

		if (exposureThread->Create(wxTHREAD_DETACHED) == wxTHREAD_NO_ERROR)
			exposureThread->Run();
		else
			delete exposureThread;

		if (!m_KetekHandler->CaptureData(m_ExposureSeconds, mcaData.get(), m_ContinueCapturing))
		{
			exposureProgressRunning->store(false);
			MainFrameVariables::PostExposureFinishedEvent(m_MainFrame);

			*m_ThreadID = "";
			return (wxThread::ExitCode)0;
		}

		exposureProgressRunning->store(false);

		MainFrameVariables::PostExposureProgressEvent
		(
			m_MainFrame,
			100,
			"Live Exposure: 100%"
		);

		evt.SetInt(imageNumber);
		evt.SetPayload(mcaData.get());
		wxQueueEvent(m_MainFrame, evt.Clone());

		{
			auto signalValue = ULONG_MAX - mcaData[0];
			while (mcaData[0] != signalValue)
				wxThread::Sleep(10);
		}

		++imageNumber;
	}

	MainFrameVariables::PostExposureFinishedEvent(m_MainFrame);
	*m_ThreadID = "";
	return (wxThread::ExitCode)0;
}

auto LiveCapturing::CaptureImage
(
	unsigned short* short_data_ptr, 
	wxImage* image_ptr
) -> bool
{
//	unsigned long* data_ptr{};
//	if (!data_ptr) return false;
//
//	wxLogDebug("4");
//	//if (!m_XimeaControl->GetThreadState(m_ThreadID)) return false;
//#ifdef USE_MULTITHREAD
//	UpdatePixelsMultithread(data_ptr, image_ptr);
//#else
//	unsigned short current_value{};
//	unsigned char red{}, green{}, blue{};
//	for (auto y{ 0 }; y < m_ImageSize.GetHeight(); ++y)
//	{
//		for (auto x{ 0 }; x < m_ImageSize.GetWidth(); ++x)
//		{
//			current_value = data_ptr[y * m_ImageSize.GetWidth() + x];
//			//short_data_ptr[y * m_ImageSize.GetWidth() + x] = current_value;
//			/* Matlab implementation of JetColormap */
//			/* Because XIMEA camera can produce 12-bit per pixel maximum, we use RGB12bit converter */
//			m_CamPreviewWindow->CalculateMatlabJetColormapPixelRGB12bit(current_value, red, green, blue);
//			image_ptr->SetRGB(x, y, red, green, blue);
//		}
//	}
//#endif
//	if (!short_data_ptr) return false;
//	//if (!m_XimeaControl->GetThreadState(m_ThreadID)) return false;
//	memcpy
//	(
//		short_data_ptr, 
//		data_ptr, 
//		sizeof(unsigned short) * m_ImageSize.GetWidth() * m_ImageSize.GetHeight()
//	);
//	if (!short_data_ptr) return false;
//
	return true;
}

auto LiveCapturing::UpdatePixelsMultithread
(
	unsigned short* short_data_ptr, 
	wxImage* image_ptr
) -> void
{
	//auto numThreads = std::thread::hardware_concurrency();
	//auto tileSize = m_ImageSize.GetHeight() % numThreads > 0 ? m_ImageSize.GetHeight() / numThreads + 1 : m_ImageSize.GetHeight() / numThreads;

	//std::vector<std::thread> threads;
	//threads.reserve(numThreads);

	//for (auto i{ 0 }; i < numThreads; ++i)
	//{
	//	auto start_x = 0;
	//	auto start_y = i * tileSize;
	//	auto finish_x = m_ImageSize.GetWidth();
	//	auto finish_y = (i + 1) * tileSize > m_ImageSize.GetHeight() ? m_ImageSize.GetHeight() : (i + 1) * tileSize;

	//	threads.emplace_back
	//	(
	//		std::thread
	//		(
	//		&LiveCapturing::AdjustImageParts, 
	//		this, 
	//		&short_data_ptr[start_y * m_ImageSize.GetWidth() + start_x], 
	//		image_ptr, 
	//		start_x, 
	//		start_y, 
	//		finish_x, 
	//		finish_y
	//		)
	//	);
	//}

	//for (auto& thread : threads)
	//{
	//	thread.join();
	//}
}

auto LiveCapturing::AdjustImageParts
(
	const unsigned short* data_ptr, 
	wxImage* image_ptr,
	const unsigned int start_x, 
	const unsigned int start_y, 
	const unsigned int finish_x, 
	const unsigned int finish_y
) -> void
{
	//if (!data_ptr) return;
	//if (!image_ptr->IsOk()) return;
	//unsigned short current_value{};
	//unsigned char red{}, green{}, blue{};
	//unsigned long long position_in_data_pointer{};

	//for (auto y{ start_y }; y < finish_y; ++y)
	//{
	//	for (auto x{ start_x }; x < finish_x; ++x)
	//	{
	//		current_value = data_ptr[position_in_data_pointer];
	//		/* Matlab implementation of JetColormap */
	//		/* Because XIMEA camera can produce 12-bit per pixel maximum, we use RGB12bit converter */
	//		m_CamPreviewWindow->CalculateMatlabJetColormapPixelRGB12bit(current_value, red, green, blue);
	//		image_ptr->SetRGB(x, y, red, green, blue);
	//		++position_in_data_pointer;
	//	}
	//}
}

LiveCapturing::~LiveCapturing()
{
	m_MainFrame = nullptr;
	m_KetekHandler = nullptr;
}
/* ___ End Live Capturing Thread ___ */

/* ___ Start Worker Thread ___ */
wxThread::ExitCode WorkerThread::Entry()
{
	constexpr auto raise_exception_msg = [](wxString camera_name) 
	{
		wxString title = "Connection error";
		wxMessageBox(
			wxT
			(
				"The " + camera_name + " camera can't capture an image."
				"\nPlease, check if the " + camera_name + " camera is connected properly and restart the program."
			),
			title,
			wxICON_ERROR);
	};
	//auto exit_thread = [&](XimeaControl* cam_control)
	//{
	//	m_Settings->SetCurrentProgress(m_FirstAxis->step_number, m_FirstAxis->step_number);
	//	cam_control = nullptr;
	//	m_MainFrame->WorkerThreadFinished(true);
	//};

	//m_MainFrame->WorkerThreadFinished(false);
	m_Settings->SetCurrentProgress(0, m_FirstAxis->step_number);

	auto now = std::chrono::system_clock::now();
	auto cur_time = std::chrono::system_clock::to_time_t(now);
	auto str_time = std::string(std::ctime(&cur_time)).substr(11, 8);
	auto cur_hours = str_time.substr(0, 2);
	auto cur_mins = str_time.substr(3, 2);
	auto cur_secs = str_time.substr(6, 2);

	auto graphFileName = m_DataPath + wxString("\\") + wxString("ktk_measurement_result_") + cur_hours + wxString("H_") + cur_mins + wxString("M_") + cur_secs + wxString("S");

	m_MeasurementGraphFilePath = graphFileName + wxString(".bmp");
	m_MeasurementGraphTxtFilePath = graphFileName + wxString(".txt");

	wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_MAIN_CAPTURING);

	auto mcaData = std::make_unique<unsigned long[]>(m_KetekHandler->GetDataSize());

	m_AllMaxElementsDuringCapturing = std::make_unique<unsigned long[]>(m_FirstAxis->step_number);
	m_AllSumsDuringCapturing = std::make_unique<unsigned long long[]>(m_FirstAxis->step_number);

	float first_axis_rounded_go_to{};
	float first_axis_position{}, second_axis_position{};
	auto positionsArray = std::make_unique<float[]>(m_FirstAxis->step_number);
	for (auto i{ 0 }; i < m_FirstAxis->step_number; ++i)
	{
		if (!*m_ContinueCapturing)
		{
			*m_ThreadID = "";
			evt.SetInt(-1);
			wxQueueEvent(m_MainFrame, evt.Clone());
			return 0;
		}
		m_Settings->SetCurrentProgress(i, m_FirstAxis->step_number);
		/* Here we need to round values, for the correct positioning of motors */
		auto correctedStart = static_cast<int>(m_FirstAxis->start * 1000.f + .5f);
		auto correctedStep = static_cast<int>(m_FirstAxis->step * 1000.f + .5f);
		auto correctedPos = static_cast<float>(correctedStart + i * correctedStep);
		first_axis_rounded_go_to = correctedPos / 1000.f;

		first_axis_position = MoveFirstStage(first_axis_rounded_go_to);
		positionsArray[i] = first_axis_rounded_go_to;

		wxString filePath{};

		if (!CaptureAndSaveData
		(
			mcaData.get(),
			i + 1,
			first_axis_position,
			second_axis_position,
			cur_hours, cur_mins, cur_secs,
			&filePath
		))
		{
			*m_ThreadID = "";
			evt.SetInt(-1);
			wxQueueEvent(m_MainFrame, evt.Clone());
			return 0;
		}

		evt.SetInt(i);
		evt.SetPayload(mcaData.get());
		evt.SetString(filePath);

		wxQueueEvent(m_MainFrame, evt.Clone());

		/* Update Current Progress */
		m_Settings->SetCurrentProgress(i, m_FirstAxis->step_number);

		auto signalValue = ULONG_MAX - mcaData[0];
		while (mcaData[0] != signalValue && *m_ContinueCapturing)
			wxThread::Sleep(10);
	}

#ifdef ENABLE_SECOND_AXIS
	switch (m_SecondAxis->axis_number)
	{
		/* Detector */
		case 0:
			first_axis_position = m_Settings->GoToAbsPos(SettingsVariables::DETECTOR_X, first_axis_rounded_go_to);
			break;
		/* Optics */
		case 1:
			first_axis_position = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_X, first_axis_rounded_go_to);
			break;
		case 2:
			first_axis_position = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_Y, first_axis_rounded_go_to);
			break;
		case 3:
			first_axis_position = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_Z, first_axis_rounded_go_to);
			break;
		case 4:
			first_axis_position = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_PITCH, first_axis_rounded_go_to);
			break;
		case 5:
			first_axis_position = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_YAW, first_axis_rounded_go_to);
			break;
		default:
			break;
	}
#endif // FALSE

	*m_ThreadID = "";
	evt.SetInt(-1);
	wxQueueEvent(m_MainFrame, evt.Clone());

	// Go to the best captured position
#ifndef _DEBUG
	if (m_MaxSumDuringCapturing)
	{
#endif // !_DEBUG
		auto message = wxString(
			"The maximum sum value was: " + wxString::Format(wxT("%ld"), m_MaxSumDuringCapturing) + '\n'
			+ "at position: "  + wxString::Format(wxT("%.3f"), m_BestFirstAxisPosition) + '\n'
			+ "measurement number: " + wxString::Format(wxT("%i"), (int)m_BestMeasurementNumber + 1)
		);
		message += "\nDo you want to move stage to the best position?";

		wxDateTime nowDateTime = wxDateTime::Now();

		// Format the date and time
		wxString timestamp = nowDateTime.Format(wxT("%d-%m-%Y, %H:%M:%S"));

		auto bmp = CreateGraph
		(
			m_AllMaxElementsDuringCapturing.get(), 
			m_AllSumsDuringCapturing.get(),
			positionsArray.get(),
			m_FirstAxis->step_number, 
			1920, 1160, 
			"Measurement Number", 
			"Max Value", 
			"Sum Values",
			timestamp
		);
		
		SaveGraph(bmp, m_MeasurementGraphFilePath);

		SaveGraphTxt
		(
			m_AllMaxElementsDuringCapturing.get(), 
			m_AllSumsDuringCapturing.get(), 
			m_FirstAxis->step_number, 
			timestamp
		);

		// Open the image with the default application
		if (!wxLaunchDefaultApplication(m_MeasurementGraphFilePath))
			wxLogError("Could not open file '%s' with the default application.", m_MeasurementGraphFilePath);
		
		if (wxMessageBox
		(
			message,
			"Move stage?",
			wxICON_QUESTION | wxYES_NO
		) == wxYES)
		{
			MoveFirstStage(m_BestFirstAxisPosition);
			m_MainFrame->UpdateStagePositions();
		}
#ifndef _DEBUG
	}
#endif // !_DEBUG

	return (wxThread::ExitCode)0;
}

auto WorkerThread::CaptureAndSaveData
(
	unsigned long* const mca,
	const int& image_number,
	const float& first_stage_position,
	const float& second_stage_position,
	const std::string& hours,
	const std::string& minutes,
	const std::string& seconds,
	wxString* filePath
) -> bool
{
	if (!mca) return false;

	auto exposureProgressRunning = std::make_shared<std::atomic_bool>(true);

	ExposureProgressThread* exposureThread = new ExposureProgressThread
	(
		m_MainFrame,
		exposureProgressRunning,
		static_cast<int>(m_ExposureTimeSeconds),
		"Measurement Exposure"
	);

	if (exposureThread->Create(wxTHREAD_DETACHED) == wxTHREAD_NO_ERROR)
		exposureThread->Run();
	else
		delete exposureThread;

	const bool captureOk = m_KetekHandler->CaptureData(m_ExposureTimeSeconds, mca, m_ContinueCapturing);

	exposureProgressRunning->store(false);

	if (!captureOk)
	{
		MainFrameVariables::PostExposureFinishedEvent(m_MainFrame);
		return false;
	}

	MainFrameVariables::PostExposureProgressEvent
	(
		m_MainFrame,
		100,
		"Measurement Exposure: 100%"
	);

	if (!std::filesystem::exists(m_DataPath.ToStdString()) && !std::filesystem::is_directory(m_DataPath.ToStdString()))
	{
		wxLogError("Desired path doesn't exist.");
		return false;
	}

	/* Save Data */
	{
		std::string first_axis_position_str = std::format("{:.3f}", first_stage_position);
		std::replace(first_axis_position_str.begin(), first_axis_position_str.end(), '.', '_');

		std::string second_axis_position_str = std::format("{:.3f}", second_stage_position);
		std::replace(second_axis_position_str.begin(), second_axis_position_str.end(), '.', '_');
		
		std::string file_name = std::string(m_DataPath.mb_str()) + std::string("\\") +
			std::string("ktk_");
		file_name += image_number < 10 ? std::string("0") : std::string("");
		file_name += std::to_string(image_number) + std::string("_") + 
			hours + std::string("H_") + 
			minutes + std::string("M_") + 
			seconds + std::string("S_") + 
			std::to_string(m_ExposureTimeSeconds) + std::string("s") 
			+ std::string("_1A_") + first_axis_position_str 
			+ std::string("_2A_") + second_axis_position_str 
			+ std::string(".mca");

		*filePath = wxString(file_name);

		unsigned long long sum{};
		sum = std::accumulate(&mca[0], &mca[m_KetekHandler->GetDataSize()], sum);

		m_AllSumsDuringCapturing[image_number - 1] = sum;
#ifdef _DEBUG
		{
			// Seed with a real random value, if available
			std::random_device rd;

			// Choose a random number generator
			std::mt19937 gen(rd());

			// Define the range [22500, 23500]
			std::uniform_int_distribution<> dis(22500, 23500);

			m_AllSumsDuringCapturing[image_number - 1] = dis(gen);;
		}
#endif // DEBUG

		if (sum > m_MaxSumDuringCapturing)
		{
			m_MaxSumDuringCapturing = sum;
			m_BestFirstAxisPosition = first_stage_position;
			m_BestMeasurementNumber = image_number - 1;
		}

		auto maxElement = MainFrameVariables::WriteMCAFile
		(
			file_name, 
			mca, 
			m_KetekHandler, 
			sum,
			m_ExposureTimeSeconds
		);

		m_AllMaxElementsDuringCapturing[image_number - 1] = maxElement;
#ifdef _DEBUG
		{
			// Seed with a real random value, if available
			std::random_device rd;

			// Choose a random number generator
			std::mt19937 gen(rd());

			// Define the range [22500, 23500]
			std::uniform_int_distribution<> dis(150, 250);

			m_AllMaxElementsDuringCapturing[image_number - 1] = dis(gen);;
		}
#endif // DEBUG

		//if (maxElement > m_MaxElementDuringCapturing)
		//{
		//	m_MaxElementDuringCapturing = maxElement;
		//	m_BestFirstAxisPosition = first_stage_position;
		//	m_BestMeasurementNumber = image_number - 1;
		//}
	}

	return true;
}

wxBitmap WorkerThread::CreateGraph
(
	const unsigned long* const countData,
	const unsigned long long* const sumData,
	const float* const positionsData,
	unsigned int dataSize,
	int width, 
	int height, 
	const wxString& xAxisLabel, 
	const wxString& leftYAxisLabel,
	const wxString& rightYAxisLabel,
	const wxString& timestamp
)
{
	wxBitmap bitmap(width, height);
	wxMemoryDC dc(bitmap);

	const wxColour backgroundColor(250, 250, 252);
	dc.SetBackground(wxBrush(backgroundColor));
	dc.Clear();

	if (dataSize <= 1 || !countData || !sumData || !positionsData)
	{
		dc.SelectObject(wxNullBitmap);
		return bitmap;
	}

	const wxColour plotBackgroundColor(255, 255, 255);
	const wxColour borderColor(210, 214, 220);
	const wxColour gridColor(220, 224, 230);
	const wxColour axisColor(35, 35, 35);
	const wxColour tickTextColor(70, 70, 70);
	const wxColour countColor(34, 177, 76);
	const wxColour sumColor(255, 128, 64);
	const wxColour bestPointColor(220, 40, 40);
	const wxColour exposureColor(0, 102, 204);
	const wxColour timestampColor(156, 39, 176);

	const int marginLeft = 130;
	const int marginRight = 130;
	const int marginTop = 60;
	const int marginBottom = 190;

	const int xAxisTitleOffset = 120;
	const int yAxisTitleOffset = 136;
	const int xTickOffset = 10;
	const int posTickOffset = 52;
	const int footerY = height - 52;
	const int tickLength = 6;
	const int markerRadius = 3;
	const int bestMarkerRadius = 8;

	const wxRect graphRect
	(
		marginLeft,
		marginTop,
		std::max(100, width - marginLeft - marginRight),
		std::max(100, height - marginTop - marginBottom)
	);

	wxFont titleFont(std::max(14, m_GraphFontSize + 4), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxFont axisTitleFont(std::max(22, m_GraphFontSize + 20), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxFont tickFont(std::max(8, m_GraphFontSize), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont footerFont(std::max(10, m_GraphFontSize - 1), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxFont watermarkFont(std::clamp(width / 4, 42, 180), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

	auto blendColor = [](const wxColour& fg, const wxColour& bg, double t) -> wxColour
		{
			t = std::clamp(t, 0.0, 1.0);

			const auto blend = [t](unsigned char a, unsigned char b) -> unsigned char
				{
					return static_cast<unsigned char>(std::lround(a * t + b * (1.0 - t)));
				};

			return wxColour
			(
				blend(fg.Red(), bg.Red()),
				blend(fg.Green(), bg.Green()),
				blend(fg.Blue(), bg.Blue())
			);
		};

	auto roundDownToNice = [](double value) -> double
		{
			if (value <= 0.0)
				return 0.0;

			const double magnitude = std::pow(10.0, std::floor(std::log10(value)));
			const double normalized = value / magnitude;

			double nice = 1.0;
			if (normalized >= 10.0)      nice = 10.0;
			else if (normalized >= 5.0)  nice = 5.0;
			else if (normalized >= 2.0)  nice = 2.0;

			return std::floor(value / (nice * magnitude)) * (nice * magnitude);
		};

	auto roundUpToNice = [](double value) -> double
		{
			if (value <= 0.0)
				return 1.0;

			const double magnitude = std::pow(10.0, std::floor(std::log10(value)));
			const double normalized = value / magnitude;

			double nice = 1.0;
			if (normalized > 5.0)       nice = 10.0;
			else if (normalized > 2.0)  nice = 5.0;
			else if (normalized > 1.0)  nice = 2.0;

			return std::ceil(value / (nice * magnitude)) * (nice * magnitude);
		};

	auto clampToGraphY = [](int y, const wxRect& rect) -> int
		{
			return std::clamp(y, rect.GetTop(), rect.GetBottom());
		};

	auto trimNumber = [](double value, int decimals = 3) -> wxString
		{
			std::ostringstream oss;
			oss.setf(std::ios::fixed);
			oss << std::setprecision(decimals) << value;

			std::string s = oss.str();

			while (!s.empty() && s.back() == '0')
				s.pop_back();

			if (!s.empty() && s.back() == '.')
				s.pop_back();

			if (s.empty() || s == "-0")
				s = "0";

			return wxString(s);
		};

	auto compactNumber = [](double value) -> wxString
		{
			const double absValue = std::abs(value);

			auto fmtTrimmed = [](double v, int decimals) -> std::string
				{
					std::ostringstream oss;
					oss.setf(std::ios::fixed);
					oss << std::setprecision(decimals) << v;

					std::string s = oss.str();

					while (!s.empty() && s.back() == '0')
						s.pop_back();

					if (!s.empty() && s.back() == '.')
						s.pop_back();

					if (s.empty() || s == "-0")
						s = "0";

					return s;
				};

			if (absValue >= 1'000'000'000.0) return wxString(fmtTrimmed(value / 1'000'000'000.0, 1) + "G");
			if (absValue >= 1'000'000.0)     return wxString(fmtTrimmed(value / 1'000'000.0, 1) + "M");
			if (absValue >= 1'000.0)         return wxString(fmtTrimmed(value / 1'000.0, 1) + "k");

			return wxString::Format("%.0f", value);
		};

	auto drawCenteredText = [&](const wxString& text, int centerX, int y)
		{
			const wxSize ts = dc.GetTextExtent(text);
			dc.DrawText(text, centerX - ts.GetWidth() / 2, y);
		};

	auto drawRightAlignedText = [&](const wxString& text, int rightX, int y)
		{
			const wxSize ts = dc.GetTextExtent(text);
			dc.DrawText(text, rightX - ts.GetWidth(), y);
		};

	const wxColour fadedWatermarkColor = blendColor(wxColour(70, 110, 180), backgroundColor, 0.22);
	const wxColour fadedAxisTitleColor = blendColor(axisColor, backgroundColor, 0.55);
	const wxColour fadedCountAxisTitleColor = blendColor(countColor, backgroundColor, 0.60);
	const wxColour fadedSumAxisTitleColor = blendColor(sumColor, backgroundColor, 0.60);

	const wxColour fadedPositionTickColor = blendColor(tickTextColor, backgroundColor, 0.50);

	dc.SetPen(wxPen(borderColor, 1));
	dc.SetBrush(wxBrush(plotBackgroundColor));
	dc.DrawRectangle(graphRect);

	{
		const wxString graphTitle = "Measurement Summary";
		dc.SetFont(titleFont);
		dc.SetTextForeground(axisColor);
		drawCenteredText(graphTitle, width / 2, 14);
	}

	{
		wxString axisName = wxString(AxisNameToString(m_FirstAxis->axis_number));
		//axisName.Replace("_", " ");
		//axisName = "Directions " + axisName.AfterLast(' ');

		dc.SetFont(watermarkFont);
		dc.SetTextForeground(fadedWatermarkColor);

		const wxSize wmSize = dc.GetTextExtent(axisName);
		dc.DrawText
		(
			axisName,
			graphRect.GetLeft() + graphRect.GetWidth() / 2 - wmSize.GetWidth() / 2,
			graphRect.GetTop() + graphRect.GetHeight() / 2 - wmSize.GetHeight() / 2
		);
	}

	const auto minCountIt = std::min_element(countData, countData + dataSize);
	const auto maxCountIt = std::max_element(countData, countData + dataSize);
	const auto minSumIt = std::min_element(sumData, sumData + dataSize);
	const auto maxSumIt = std::max_element(sumData, sumData + dataSize);

	double minCountValue = static_cast<double>(*minCountIt);
	double maxCountValue = static_cast<double>(*maxCountIt);
	double minSumValue = static_cast<double>(*minSumIt);
	double maxSumValue = static_cast<double>(*maxSumIt);

	if (minCountValue == maxCountValue)
	{
		minCountValue = std::max(0.0, minCountValue - 1.0);
		maxCountValue += 1.0;
	}
	else
	{
		minCountValue = roundDownToNice(minCountValue);
		maxCountValue = roundUpToNice(maxCountValue);
	}

	if (minSumValue == maxSumValue)
	{
		minSumValue = std::max(0.0, minSumValue - 1.0);
		maxSumValue += 1.0;
	}
	else
	{
		minSumValue = roundDownToNice(minSumValue);
		maxSumValue = roundUpToNice(maxSumValue);
	}

	auto mapX = [&](size_t index) -> int
		{
			if (dataSize <= 1)
				return graphRect.GetLeft();

			const double t = static_cast<double>(index) / static_cast<double>(dataSize - 1);
			return graphRect.GetLeft() + static_cast<int>(std::lround(t * graphRect.GetWidth()));
		};

	auto mapCountY = [&](double value) -> int
		{
			const double t = (value - minCountValue) / (maxCountValue - minCountValue);
			const int y = graphRect.GetBottom() - static_cast<int>(std::lround(t * graphRect.GetHeight()));
			return clampToGraphY(y, graphRect);
		};

	auto mapSumY = [&](double value) -> int
		{
			const double t = (value - minSumValue) / (maxSumValue - minSumValue);
			const int y = graphRect.GetBottom() - static_cast<int>(std::lround(t * graphRect.GetHeight()));
			return clampToGraphY(y, graphRect);
		};

	auto drawHorizontalRulersAndGrid = [&]()
		{
			const int yDivisions = 5;
			dc.SetFont(tickFont);

			for (int i = 0; i <= yDivisions; ++i)
			{
				const double t = static_cast<double>(i) / static_cast<double>(yDivisions);
				const int y = graphRect.GetBottom() - static_cast<int>(std::lround(t * graphRect.GetHeight()));

				if (i > 0 && i < yDivisions)
				{
					dc.SetPen(wxPen(gridColor, 1, wxPENSTYLE_DOT));
					dc.DrawLine(graphRect.GetLeft(), y, graphRect.GetRight(), y);
				}

				const double countValue = minCountValue + t * (maxCountValue - minCountValue);
				const wxString leftText = compactNumber(countValue);
				const wxSize leftSize = dc.GetTextExtent(leftText);

				dc.SetPen(wxPen(countColor, 1));
				dc.SetTextForeground(countColor);
				dc.DrawLine(graphRect.GetLeft() - tickLength, y, graphRect.GetLeft(), y);
				dc.DrawText(leftText, graphRect.GetLeft() - tickLength - 6 - leftSize.GetWidth(), y - leftSize.GetHeight() / 2);

				const double sumValue = minSumValue + t * (maxSumValue - minSumValue);
				const wxString rightText = compactNumber(sumValue);

				dc.SetPen(wxPen(sumColor, 1));
				dc.SetTextForeground(sumColor);
				dc.DrawLine(graphRect.GetRight(), y, graphRect.GetRight() + tickLength, y);
				dc.DrawText(rightText, graphRect.GetRight() + tickLength + 6, y - dc.GetTextExtent(rightText).GetHeight() / 2);
			}
		};

	auto drawVerticalGridAndLabels = [&]()
		{
			dc.SetFont(tickFont);

			const int labelStep =
				(dataSize <= 12) ? 1 :
				(dataSize <= 30) ? 2 :
				(dataSize <= 80) ? 5 : 10;

			for (size_t i = 0; i < dataSize; ++i)
			{
				const int x = mapX(i);
				const bool drawLabel = (i == 0 || i == dataSize - 1 || (i % labelStep) == 0);

				if (i > 0 && i < dataSize - 1)
				{
					dc.SetPen(wxPen(gridColor, 1, wxPENSTYLE_DOT));
					dc.DrawLine(x, graphRect.GetTop(), x, graphRect.GetBottom());
				}

				dc.SetPen(wxPen(axisColor, 1));
				dc.DrawLine(x, graphRect.GetBottom(), x, graphRect.GetBottom() + tickLength);

				if (drawLabel)
				{
					const wxString measText = wxString::Format("%u", static_cast<unsigned>(i + 1));
					const wxSize measSize = dc.GetTextExtent(measText);

					dc.SetTextForeground(axisColor);
					dc.DrawText(measText, x - measSize.GetWidth() / 2, graphRect.GetBottom() + xTickOffset);

					const wxString posText = trimNumber(positionsData[i], 3);
					const wxSize posSize = dc.GetTextExtent(posText);

					dc.SetTextForeground(fadedPositionTickColor);
					dc.DrawRotatedText(posText, x + posSize.GetHeight() / 2, graphRect.GetBottom() + posTickOffset, 270);
				}
			}
		};

	auto drawAxes = [&]()
		{
			dc.SetPen(wxPen(axisColor, 2));
			dc.DrawLine(graphRect.GetLeft(), graphRect.GetBottom(), graphRect.GetRight(), graphRect.GetBottom());
			dc.DrawLine(graphRect.GetLeft(), graphRect.GetBottom(), graphRect.GetLeft(), graphRect.GetTop());
			dc.DrawLine(graphRect.GetRight(), graphRect.GetBottom(), graphRect.GetRight(), graphRect.GetTop());
		};

	auto drawAxisTitles = [&]()
		{
			dc.SetFont(axisTitleFont);

			dc.SetTextForeground(fadedAxisTitleColor);
			drawCenteredText(xAxisLabel, graphRect.GetLeft() + graphRect.GetWidth() / 2, graphRect.GetBottom() + xAxisTitleOffset);

			dc.SetTextForeground(fadedCountAxisTitleColor);
			const wxSize leftAxisSize = dc.GetTextExtent(leftYAxisLabel);
			dc.DrawRotatedText
			(
				leftYAxisLabel,
				marginLeft - yAxisTitleOffset,
				graphRect.GetTop() + graphRect.GetHeight() / 2 + leftAxisSize.GetWidth() / 2,
				90
			);

			dc.SetTextForeground(fadedSumAxisTitleColor);
			const wxSize rightAxisSize = dc.GetTextExtent(rightYAxisLabel);
			dc.DrawRotatedText
			(
				rightYAxisLabel,
				width - marginRight + yAxisTitleOffset,
				graphRect.GetTop() + graphRect.GetHeight() / 2 - rightAxisSize.GetWidth() / 2,
				270
			);
		};

	std::vector<wxPoint> countPoints;
	std::vector<wxPoint> sumPoints;
	countPoints.reserve(dataSize);
	sumPoints.reserve(dataSize);

	auto buildPolylinePoints = [&]()
		{
			for (size_t i = 0; i < dataSize; ++i)
			{
				countPoints.emplace_back(mapX(i), mapCountY(static_cast<double>(countData[i])));
				sumPoints.emplace_back(mapX(i), mapSumY(static_cast<double>(sumData[i])));
			}
		};

	auto drawPolyline = [&](const std::vector<wxPoint>& points, const wxColour& color)
		{
			dc.SetPen(wxPen(color, 3));
			for (size_t i = 1; i < points.size(); ++i)
				dc.DrawLine(points[i - 1], points[i]);
		};

	auto drawMarkers = [&](const std::vector<wxPoint>& points, const wxColour& color)
		{
			dc.SetPen(*wxTRANSPARENT_PEN);
			dc.SetBrush(wxBrush(color));
			for (const auto& p : points)
				dc.DrawCircle(p, markerRadius);
		};

	auto drawBestPoint = [&]()
		{
			if (m_BestMeasurementNumber < 0 || static_cast<size_t>(m_BestMeasurementNumber) >= sumPoints.size())
				return;

			const wxPoint best = sumPoints[static_cast<size_t>(m_BestMeasurementNumber)];

			dc.SetPen(wxPen(bestPointColor, 2));
			dc.SetBrush(*wxTRANSPARENT_BRUSH);
			dc.DrawCircle(best, bestMarkerRadius);

			dc.SetFont(tickFont);
			dc.SetTextForeground(bestPointColor);

			const wxString bestLabel = wxString::Format
			(
				"Best #%d (%s)",
				m_BestMeasurementNumber + 1,
				trimNumber(positionsData[m_BestMeasurementNumber], 3)
			);

			const wxSize bestSize = dc.GetTextExtent(bestLabel);
			int labelX = best.x + 10;
			int labelY = best.y - bestSize.GetHeight() - 10;

			if (labelX + bestSize.GetWidth() > graphRect.GetRight())
				labelX = best.x - bestSize.GetWidth() - 10;
			if (labelY < graphRect.GetTop())
				labelY = best.y + 10;

			dc.DrawText(bestLabel, labelX, labelY);
		};

	auto drawLegend = [&]()
		{
			const int legendX = graphRect.GetLeft() + 12;
			const int legendY = graphRect.GetTop() + 10;
			const int sw = 20;

			dc.SetFont(tickFont);

			dc.SetPen(wxPen(countColor, 3));
			dc.DrawLine(legendX, legendY + 8, legendX + sw, legendY + 8);
			dc.SetTextForeground(axisColor);
			dc.DrawText(leftYAxisLabel, legendX + sw + 8, legendY);

			dc.SetPen(wxPen(sumColor, 3));
			dc.DrawLine(legendX + 170, legendY + 8, legendX + 170 + sw, legendY + 8);
			dc.DrawText(rightYAxisLabel, legendX + 170 + sw + 8, legendY);
		};

	auto drawFooter = [&]()
		{
			dc.SetFont(footerFont);

			const wxString exposureStr = wxString::Format("Exposure: %lu s", m_ExposureTimeSeconds);

			dc.SetTextForeground(exposureColor);
			dc.DrawText(exposureStr, marginLeft, footerY);

			dc.SetTextForeground(timestampColor);
			drawRightAlignedText(timestamp, width - marginRight, footerY);
		};

	drawHorizontalRulersAndGrid();
	drawVerticalGridAndLabels();
	drawAxes();
	drawAxisTitles();
	buildPolylinePoints();
	drawPolyline(sumPoints, sumColor);
	drawPolyline(countPoints, countColor);
	drawMarkers(sumPoints, sumColor);
	drawMarkers(countPoints, countColor);
	drawBestPoint();
	drawLegend();
	drawFooter();

	dc.SelectObject(wxNullBitmap);
	return bitmap;
}

auto WorkerThread::SaveGraph(const wxBitmap& bitmap, const wxString filePath) -> void
{
	if (bitmap.IsOk()) 
	{
		// Save the bitmap as a BMP file
		//wxString filePath = wxT("output.bmp");
		if (!bitmap.SaveFile(filePath, wxBITMAP_TYPE_BMP)) 
		{
			wxLogError("Failed to save bitmap to %s", filePath);
			//wxLogMessage("Bitmap saved successfully to %s", filePath);
		}
	}
	else 
	{
		wxLogError("Bitmap is not valid.");
	}
}

auto WorkerThread::SaveGraphTxt
(
	const unsigned long* const countData, 
	const unsigned long long* const sumData, 
	const unsigned int dataSize,
	const wxString timestamp
) -> void
{
	if (!countData || !sumData) return;

	std::ofstream outFile;

	outFile.open(m_MeasurementGraphTxtFilePath.ToStdString());

	// Check if the file was opened successfully
	if (!outFile) 
		return;

	outFile << timestamp << std::endl;
	outFile << "Measurement Number" << '\t' << "Max Value" << '\t' << "Sum Values" << std::endl;
	for (auto i{ 0 }; i < static_cast<int>(dataSize); ++i)
	{
		outFile << i + 1 << '\t' << countData[i] << '\t' << sumData[i] << std::endl;
	}

	outFile.close();
}

auto WorkerThread::MoveFirstStage(const float position) -> float
{
	float firstAxisPos{};
	switch (m_FirstAxis->axis_number)
	{
		/* Detector */
		case 0:
			firstAxisPos = m_Settings->GoToAbsPos(SettingsVariables::DETECTOR_X, position);
			break;
		/* Optics */
		case 1:
			firstAxisPos = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_X, position);
			break;
		case 2:
			firstAxisPos = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_Y, position);
			break;
		case 3:
			firstAxisPos = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_Z, position);
			break;
		case 4:
			firstAxisPos = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_PITCH, position);
			break;
		case 5:
			firstAxisPos = m_Settings->GoToAbsPos(SettingsVariables::OPTICS_YAW, position);
			break;
		default:
			break;
	}

	return firstAxisPos;
}

auto WorkerThread::AxisNameToString(const int axis) -> std::string
{
	switch (axis) {
	case SettingsVariables::DETECTOR_X:   return "DETECTOR X";
	case SettingsVariables::OPTICS_X:   return "OPTICS X";
	case SettingsVariables::OPTICS_Y:   return "OPTICS Y";
	case SettingsVariables::OPTICS_Z:   return "OPTICS Z";
	case SettingsVariables::OPTICS_PITCH:   return "OPTICS PITCH";
	case SettingsVariables::OPTICS_YAW:   return "OPTICS YAW";
	default:           return "Unknown";
	}
}
/* ___ End Worker Thread ___ */

/* ___ Start Progress Thread ___ */
wxThread::ExitCode ProgressThread::Entry()
{
	m_Progress = 0;
	m_ProgressMsg = "";
	wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_PROGRESS_CAPTURING);
	while (*m_ContinueWaiting)
	{
		//wxThreadEvent calc_event(wxEVT_THREAD, MainFrameVariables::ID_THREAD_PROGRESS_CAPTURING);
		m_Settings->ProvideProgressInfo(&m_ProgressMsg, &m_Progress);

		evt.SetInt(m_Progress);
		evt.SetString(m_ProgressMsg);

		wxQueueEvent(m_Frame, evt.Clone());

		wxThread::This()->Sleep(100);
	}
	evt.SetInt(-1);
	wxQueueEvent(m_Frame, evt.Clone());

	return (wxThread::ExitCode)0;
}
/* ___ End Progress Thread ___ */

/* ___ Start ProgressBar ___ */
BEGIN_EVENT_TABLE(ProgressBar, wxFrame)
END_EVENT_TABLE()

ProgressBar::ProgressBar(wxWindow* parent, const wxPoint& pos, const wxSize& size)
	: 
	wxFrame(
		parent,
		wxID_ANY,
		wxT("Progress"),
		pos,
		size,
		wxDEFAULT_FRAME_STYLE |
		wxFRAME_FLOAT_ON_PARENT |
		wxSTAY_ON_TOP
	),
	m_MainSize(size)
{
	CreateProgressBar();
}

void ProgressBar::UpdateProgressWithMessage(const wxString& msg, const int& progress)
{
	m_ProgressPanel->SetProgress(progress);
	m_ProgressPanel->SetComment(msg);
}

void ProgressBar::UpdateElapsedTime(const uint64_t& elapsed_seconds)
{
	m_ProgressPanel->SetElapsedTime(elapsed_seconds);
}

void ProgressBar::UpdateEstimatedTime(const int& prgs, const uint64_t& elapsed_seconds)
{
	m_ProgressPanel->SetEstimatedTime(prgs, elapsed_seconds);
}

void ProgressBar::CreateProgressBar()
{
	wxSizer* const main_sizer = new wxBoxSizer(wxVERTICAL);
	m_ProgressPanel = new ProgressPanel(this, m_MainSize);
	main_sizer->Add(m_ProgressPanel, 1, wxEXPAND | wxALL, 2);

	const bool isDark = wxSystemSettings::GetAppearance().IsDark();
	SetBackgroundColour(isDark ? wxColour(28, 28, 30) : wxColour(245, 247, 250));

	SetSizer(main_sizer);
	SetClientSize(m_MainSize);
	Layout();
}

ProgressBar::~ProgressBar()
{
	//m_ProgressPanel->Destroy();
}
/* ___ End ProgressBar ___ */

/* ___ Start ProgressPanel ___ */
BEGIN_EVENT_TABLE(ProgressPanel, wxPanel)
EVT_PAINT(ProgressPanel::PaintEvent)
EVT_SIZE(ProgressPanel::OnSize)
END_EVENT_TABLE()

ProgressPanel::ProgressPanel(
	wxFrame* parent, const wxSize& size)
	: wxPanel(parent)
{
	SetDoubleBuffered(true);

	SetBackgroundColour(parent->GetBackgroundColour());

	SetMinSize(size);
}

void ProgressPanel::SetSize(const wxSize& new_size)
{
	m_Width = new_size.x;
	m_Height = new_size.y;
	Refresh();
}

void ProgressPanel::SetProgress(const int& progress)
{
	m_Progress = progress <= 100 ? progress : 100;
}

void ProgressPanel::SetElapsedTime(const int& elapsed_time)
{
	m_ElapsedTime = elapsed_time;

	m_ElapsedHours = elapsed_time / 3600;
	m_ElapsedMinutes = elapsed_time / 60 - m_ElapsedHours * 60;
	m_ElapsedSeconds = elapsed_time - m_ElapsedHours * 3600 - m_ElapsedMinutes * 60;
}

void ProgressPanel::SetEstimatedTime(const int& progress, const int& elapsed_time)
{
	if (progress > 0 && progress != m_PreviousEstimateProgress)
	{
		m_EstimatedTime = (double)elapsed_time / (double)progress * 100.0 - elapsed_time;
		m_PreviousEstimateProgress = progress;
	}
	else if (progress == 0)
	{
		/* Reset Estimated time variables */
		m_EstimatedTime = 0;
		m_PreviousEstimateProgress = 0;
	}

	Refresh();
}

void ProgressPanel::SetComment(const wxString& progress_comment)
{
	m_ProgressComment = progress_comment;
}

void ProgressPanel::PaintEvent(wxPaintEvent& evt)
{
	wxBufferedPaintDC dc(this);
	Render(dc);
}

void ProgressPanel::Render(wxBufferedPaintDC& dc)
{
	dc.Clear();

	std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
	if (!gc)
		return;

	const wxSize sz = GetClientSize();
	const double w = static_cast<double>(sz.GetWidth());
	const double h = static_cast<double>(sz.GetHeight());

	const double pad = 10.0;
	const double radius = 8.0;

	const bool isDark = IsDarkAppearance();

	const wxColour baseBg = isDark ? wxColour(28, 28, 30) : wxColour(245, 247, 250);
	const wxColour panelBg = isDark ? wxColour(36, 36, 38) : wxColour(252, 252, 253);
	const wxColour border = isDark ? wxColour(70, 70, 74) : wxColour(210, 214, 220);
	const wxColour titleColor = isDark ? wxColour(235, 235, 235) : wxColour(45, 45, 48);
	const wxColour textColor = isDark ? wxColour(210, 210, 214) : wxColour(70, 70, 74);
	const wxColour mutedText = isDark ? wxColour(160, 160, 168) : wxColour(110, 114, 120);
	const wxColour track = isDark ? wxColour(58, 58, 62) : wxColour(232, 236, 242);
	const wxColour fillStart = wxColour(50, 130, 246);
	const wxColour fillEnd = wxColour(34, 177, 76);

	gc->SetPen(wxPen(border, 1));
	gc->SetBrush(wxBrush(panelBg));
	gc->DrawRoundedRectangle(0.5, 0.5, w - 1.0, h - 1.0, radius);

	wxFont titleFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxFont bodyFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	wxFont smallFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

	gc->SetFont(titleFont, titleColor);
	gc->DrawText("Measurement Progress", pad, pad - 1.0);

	wxString pct = wxString::Format("%d%%", std::clamp(m_Progress, 0, 100));
	wxDouble tw{}, th{};
	gc->GetTextExtent(pct, &tw, &th);
	gc->DrawText(pct, w - pad - tw, pad - 1.0);

	const double barY = pad + 18.0;
	const double barH = 10.0;
	const double barW = w - 2.0 * pad;

	gc->SetPen(*wxTRANSPARENT_PEN);
	gc->SetBrush(wxBrush(track));
	gc->DrawRoundedRectangle(pad, barY, barW, barH, barH / 2.0);

	const double clampedProgress = std::clamp(static_cast<double>(m_Progress), 0.0, 100.0);
	const double fillW = std::max(0.0, barW * clampedProgress / 100.0);

	if (fillW > 0.0)
	{
		wxGraphicsGradientStops stops(fillStart, fillEnd);
		stops.Add(fillStart, 0.0f);
		stops.Add(fillEnd, 1.0f);

		wxGraphicsBrush fillBrush =
			gc->CreateLinearGradientBrush(pad, barY, pad + barW, barY, stops);

		gc->SetBrush(fillBrush);
		gc->DrawRoundedRectangle(pad, barY, fillW, barH, barH / 2.0);
	}

	gc->SetFont(bodyFont, textColor);

	wxString comment = m_ProgressComment;
	if (comment.Length() > 52)
		comment = comment.Left(49) + "...";

	gc->DrawText(comment, pad, barY + barH + 6.0);

	const auto fmtTime = [](int totalSeconds) -> wxString
		{
			totalSeconds = std::max(0, totalSeconds);
			const int hh = totalSeconds / 3600;
			const int mm = (totalSeconds % 3600) / 60;
			const int ss = totalSeconds % 60;
			return wxString::Format("%02d:%02d:%02d", hh, mm, ss);
		};

	gc->SetFont(smallFont, mutedText);
	gc->DrawText("Elapsed: " + fmtTime(m_ElapsedTime), pad, h - 18.0);

	const wxString remaining =
		(m_Progress > 0 && m_Progress < 100)
		? "Remaining: " + fmtTime(m_EstimatedTime)
		: wxString("Remaining: --:--:--");

	gc->GetTextExtent(remaining, &tw, &th);
	gc->DrawText(remaining, w - pad - tw, h - 18.0);
}

void ProgressPanel::OnSize(wxSizeEvent& evt)
{
	int newWidth{ evt.GetSize().x }, newHeight{ evt.GetSize().y };
	if (newWidth != m_Width || newHeight != m_Height)
	{
		m_Width = newWidth;
		m_Height = newHeight;
		Refresh();
	}
}
bool ProgressPanel::IsDarkAppearance() const
{
	if (const wxWindow* parent = GetParent())
	{
		const wxColour bg = parent->GetBackgroundColour();
		if (bg.IsOk())
		{
			const int luminance =
				(static_cast<int>(bg.Red()) * 299 +
					static_cast<int>(bg.Green()) * 587 +
					static_cast<int>(bg.Blue()) * 114) / 1000;

			return luminance < 128;
		}
	}

	return wxSystemSettings::GetAppearance().IsDark();
}
wxColour ProgressPanel::Blend(const wxColour& a, const wxColour& b, int alpha255) const
{
	alpha255 = std::clamp(alpha255, 0, 255);

	const auto mix = [alpha255](unsigned char x, unsigned char y) -> unsigned char
		{
			return static_cast<unsigned char>(
				((255 - alpha255) * static_cast<int>(x) + alpha255 * static_cast<int>(y)) / 255
				);
		};

	return wxColour
	(
		mix(a.Red(), b.Red()),
		mix(a.Green(), b.Green()),
		mix(a.Blue(), b.Blue())
	);
}
/* ___ End ProgressPanel ___ */

void cMain::ShowExposureProgressControls(const wxString& label)
{
	if (m_ExposureGauge)
	{
		m_ExposureGauge->SetRange(100);
		m_ExposureGauge->SetValue(0);
		m_ExposureGauge->Show();
	}

	if (m_ExposureProgressStaticText)
	{
		m_ExposureProgressStaticText->SetLabel(label);
		m_ExposureProgressStaticText->Show();
	}

	if (m_ExposureProgressPanel)
		m_ExposureProgressPanel->Show();

	if (m_ExposureProgressPanel)
	{
		m_ExposureProgressPanel->Layout();
		m_ExposureProgressPanel->Refresh();
	}

	if (m_ExposureProgressPanel && m_ExposureProgressPanel->GetParent())
	{
		m_ExposureProgressPanel->GetParent()->Layout();
		m_ExposureProgressPanel->GetParent()->Refresh();
	}

	if (m_DeviceControlsNotebook)
	{
		m_DeviceControlsNotebook->Layout();
		m_DeviceControlsNotebook->Refresh();
	}

	if (m_RightSidePanel)
	{
		m_RightSidePanel->Layout();
		m_RightSidePanel->FitInside();
		m_RightSidePanel->Refresh();
	}

	Layout();
	Refresh();
	Update();
}

void cMain::HideExposureProgressControls()
{
	if (m_ExposureGauge)
		m_ExposureGauge->SetValue(0);

	if (m_ExposureProgressStaticText)
		m_ExposureProgressStaticText->SetLabel("Exposure Progress: 0%");

	if (m_ExposureProgressPanel)
		m_ExposureProgressPanel->Hide();

	if (m_ExposureProgressPanel && m_ExposureProgressPanel->GetParent())
	{
		m_ExposureProgressPanel->GetParent()->Layout();
		m_ExposureProgressPanel->GetParent()->Refresh();
	}

	if (m_DeviceControlsNotebook)
	{
		m_DeviceControlsNotebook->Layout();
		m_DeviceControlsNotebook->Refresh();
	}

	if (m_RightSidePanel)
	{
		m_RightSidePanel->Layout();
		m_RightSidePanel->FitInside();
		m_RightSidePanel->Refresh();
	}

	Layout();
	Refresh();
	Update();
}

void cMain::ResetExposureProgressControls()
{
	if (m_ExposureGauge)
		m_ExposureGauge->SetValue(0);

	if (m_ExposureProgressStaticText)
		m_ExposureProgressStaticText->SetLabel("Exposure Progress: 0%");
}

void cMain::BeginExposureProgress(int exposureSeconds, const wxString& prefix)
{
	m_ExposureDurationSeconds = std::max(1, exposureSeconds);
	m_ExposureProgressPrefix = prefix;
	m_ExposureStartTime = std::chrono::steady_clock::now();
	m_ExposureInProgress = true;

	ShowExposureProgressControls(prefix + ": 0%");
}

void cMain::EndExposureProgress()
{
	m_ExposureInProgress = false;
	m_ExposureDurationSeconds = 0;
	m_ExposureProgressPrefix = "Exposure Progress";
	HideExposureProgressControls();
}

void cMain::ApplyCaptureUiState(MainFrameVariables::CaptureUiMode mode)
{
	m_CaptureUiMode = mode;

	const bool idle = (mode == MainFrameVariables::CaptureUiMode::Idle);
	const bool liveRunning = (mode == MainFrameVariables::CaptureUiMode::LiveRunning);
	const bool measurementRunning = (mode == MainFrameVariables::CaptureUiMode::MeasurementRunning);
	const bool singleShotRunning = (mode == MainFrameVariables::CaptureUiMode::SingleShotRunning);

	const bool anyCaptureRunning = liveRunning || measurementRunning || singleShotRunning;

	const bool deviceReady =
		m_KetekHandler &&
		m_KetekHandler->IsDeviceInitialized();

	const bool outputDirValid =
		m_OutDirTextCtrl &&
		wxDir::Exists(m_OutDirTextCtrl->GetValue());

	const bool canStartMeasurement =
		deviceReady &&
		outputDirValid &&
		!anyCaptureRunning;

	const bool canUseDeviceControls =
		deviceReady &&
		!anyCaptureRunning;

	// Buttons / text controls on the device page
	if (m_SingleShotBtn)
		m_SingleShotBtn->Enable(canUseDeviceControls);

	if (m_DeviceExposure)
		m_DeviceExposure->Enable(canUseDeviceControls);

	if (m_MinRangeKEVTxtCtrl)
		m_MinRangeKEVTxtCtrl->Enable(canUseDeviceControls);

	if (m_MaxRangeKEVTxtCtrl)
		m_MaxRangeKEVTxtCtrl->Enable(canUseDeviceControls);

	// Output directory selection should not change during any capture
	if (m_OutDirBtn)
		m_OutDirBtn->Enable(!anyCaptureRunning);

	// Measurement page controls
	if (m_StartStopMeasurementTglBtn)
	{
		m_StartStopMeasurementTglBtn->Enable(canStartMeasurement || measurementRunning);
		m_StartStopMeasurementTglBtn->SetValue(measurementRunning);
		m_StartStopMeasurementTglBtn->SetBitmap(GetMeasurementBitmap(measurementRunning));
	}

	// Live capturing button
	if (m_StartStopLiveCapturingTglBtn)
	{
		m_StartStopLiveCapturingTglBtn->Enable((deviceReady && !measurementRunning && !singleShotRunning) || liveRunning);
		m_StartStopLiveCapturingTglBtn->SetValue(liveRunning);
		m_StartStopLiveCapturingTglBtn->SetBitmap(GetLiveCapturingBitmap(liveRunning));
	}

	// Stage controls should generally be frozen during any capture
	if (anyCaptureRunning)
	{
		m_Detector[0].DisableAllControls();
		for (int i = 0; i < 5; ++i)
			m_Optics[i].DisableAllControls();

		if (m_FirstStage)
			m_FirstStage->DisableAllControls();

#ifdef USE_2_AXIS_MEASUREMENT
		if (m_SecondStage)
			m_SecondStage->DisableAllControls();
#endif
	}
	else
	{
		EnableUsedAndDisableNonUsedMotors();

		if (m_FirstStage && outputDirValid)
			m_FirstStage->EnableAllControls();

#ifdef USE_2_AXIS_MEASUREMENT
		if (m_SecondStage && outputDirValid)
			m_SecondStage->EnableAllControls();
#endif
	}

	// Menu items
	if (m_MenuBar && m_MenuBar->menu_edit)
	{
		m_MenuBar->menu_edit->Enable(MainFrameVariables::ID::RIGHT_DEVICE_SINGLE_SHOT_BTN, canUseDeviceControls);

		m_MenuBar->menu_edit->Enable
		(
			MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN,
			(deviceReady && !measurementRunning && !singleShotRunning) || liveRunning
		);

		m_MenuBar->menu_edit->Check
		(
			MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN,
			liveRunning
		);

		m_MenuBar->menu_edit->Enable
		(
			MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN,
			canStartMeasurement || measurementRunning
		);

		m_MenuBar->menu_edit->Check
		(
			MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN,
			measurementRunning
		);
	}

	if (m_RightSidePanel)
	{
		m_RightSidePanel->Layout();
		m_RightSidePanel->FitInside();
	}

	Layout();
	Refresh();
}

wxPoint cMain::GetDefaultProgressWindowPosition() const
{
	wxDisplay display(wxDisplay::GetFromWindow(const_cast<cMain*>(this)));

	if (!display.IsOk())
		return wxPoint(80, 80);

	const wxRect screen = display.GetGeometry();
	return wxPoint
	(
		screen.GetRight() - m_ProgressWindowSize.GetWidth() - 24,
		screen.GetBottom() - m_ProgressWindowSize.GetHeight() - 48
	);
}

void cMain::SaveProgressWindowGeometry() const
{
	if (!m_ProgressBar)
		return;

	const wxString iniPath = GetInitializationFilePath();

	nlohmann::json j;
	{
		std::ifstream in(iniPath.ToStdString());
		if (in.is_open())
		{
			try { in >> j; }
			catch (...) { j = nlohmann::json::object(); }
		}
	}

	const wxPoint pos = m_ProgressBar->GetPosition();
	const wxSize size = m_ProgressBar->GetSize();

	j["progress_window"] =
	{
		{"x", pos.x},
		{"y", pos.y},
		{"w", size.GetWidth()},
		{"h", size.GetHeight()}
	};

	std::ofstream out(iniPath.ToStdString(), std::ios::out | std::ios::trunc);
	if (out.is_open())
		out << j.dump(4);
}

void cMain::RestoreProgressWindowGeometry()
{
	const wxString iniPath = GetInitializationFilePath();

	std::ifstream in(iniPath.ToStdString());
	if (!in.is_open())
		return;

	nlohmann::json j;
	try
	{
		in >> j;
	}
	catch (...)
	{
		return;
	}

	if (j.contains("progress_window") && j["progress_window"].is_object())
	{
		const auto& pw = j["progress_window"];

		const int x = pw.value("x", wxDefaultCoord);
		const int y = pw.value("y", wxDefaultCoord);
		const int w = pw.value("w", 340);
		const int h = pw.value("h", 124);

		if (x != wxDefaultCoord && y != wxDefaultCoord)
			m_ProgressWindowPosition = wxPoint(x, y);

		m_ProgressWindowSize = wxSize(std::min(260, w), std::min(96, h));
	}
}
