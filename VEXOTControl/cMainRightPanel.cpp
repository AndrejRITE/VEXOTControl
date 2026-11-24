// cMain: the right-hand control panel - motors, detector and measurement
// sections. Control IDs are unchanged, so the event handlers in the other
// cMain files keep working.
#include "cMain.h"

#include "wx/collpane.h"
#include "wx/statline.h"

namespace
{
	struct AxisIds
	{
		int abs, set, rel, dec, inc, center, home;
	};

	struct AxisBitmaps
	{
		wxBitmap set, center, home;
	};

	void SetLabelIfChanged(wxStaticText* label, const wxString& text)
	{
		if (label && label->GetLabel() != text)
			label->SetLabel(text);
	}
}

/* ======================= Shared building blocks ======================= */

auto cMain::MutedTextColour() const -> wxColour
{
	const bool dark =
		m_MenuBar && m_MenuBar->menu_window &&
		m_MenuBar->menu_window->IsChecked(MainFrameVariables::ID::MENUBAR_WINDOW_ENABLE_DARK_MODE);

	// Windows' "grey text" is meant for light backgrounds only.
	return dark ? wxColour(185, 185, 185) : wxColour(95, 95, 95);
}

auto cMain::MakeMutedLabel(wxWindow* parent, const wxString& text) -> wxStaticText*
{
	auto* label = new wxStaticText(parent, wxID_ANY, text);
	label->SetForegroundColour(MutedTextColour());
	m_MutedLabels.push_back(label);
	return label;
}

void cMain::ApplyRightPanelTextColours()
{
	const wxColour muted = MutedTextColour();

	for (wxStaticText* label : m_MutedLabels)
	{
		label->SetForegroundColour(muted);
		label->Refresh();
	}

	UpdateScanPreview(); // it picks muted or warning colour itself
}

auto cMain::CreateSection(wxWindow* parent, const wxString& title, wxStaticText** statusOut, wxSizer** contentOut) -> wxPanel*
{
	auto* section = new wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME | wxTAB_TRAVERSAL);
	auto* sizer = new wxBoxSizer(wxVERTICAL);
	const int pad = FromDIP(8);

	auto* header = new wxBoxSizer(wxHORIZONTAL);
	auto* titleLabel = new wxStaticText(section, wxID_ANY, title);
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
		*statusOut = new wxStaticText(section, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
		(*statusOut)->SetForegroundColour(MutedTextColour());
		m_MutedLabels.push_back(*statusOut);
		header->Add(*statusOut, 0, wxALIGN_CENTER_VERTICAL);
	}

	sizer->Add(header, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);
	sizer->Add(new wxStaticLine(section), 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, pad);

	auto* content = new wxBoxSizer(wxVERTICAL);
	sizer->Add(content, 1, wxEXPAND | wxALL, pad);

	section->SetSizer(sizer);
	*contentOut = content;
	return section;
}

/* ======================= Motors ======================= */

namespace
{
	// One row of a motor group: name | position | go to [flag] | - step + | center home
	void AddAxisRow
	(
		wxWindow* parent,
		wxFlexGridSizer* grid,
		const wxString& name,
		MainFrameVariables::StepperControl& axis,
		const AxisIds& ids,
		const AxisBitmaps& bitmaps,
		const wxSize& fieldSize,
		const wxSize& stepFieldSize,
		const wxSize& buttonSize,
		int nameWidth
	)
	{
		axis.name_label = new wxStaticText(parent, wxID_ANY, name);
		{
			wxFont font = axis.name_label->GetFont();
			font.MakeBold();
			axis.name_label->SetFont(font);
		}
		// Same width in every group, so the columns of all groups line up
		axis.name_label->SetMinSize(wxSize(nameWidth, -1));

		axis.position_label = new wxStaticText
		(
			parent, wxID_ANY, "-", wxDefaultPosition, fieldSize,
			wxALIGN_RIGHT | wxST_NO_AUTORESIZE
		);
		axis.position_label->SetFont(wxFont(wxFontInfo(axis.position_label->GetFont().GetFractionalPointSize()).Family(wxFONTFAMILY_TELETYPE)));
		axis.position_label->SetToolTip("Current position reported by the motor controller");

		wxFloatingPointValidator<float> absValidator(3, nullptr, wxNUM_VAL_DEFAULT);
		absValidator.SetMin(-1000.f);
		absValidator.SetMax(1000.f);

		axis.absolute_text_ctrl = new wxTextCtrl
		(
			parent, ids.abs, "0.000", wxDefaultPosition, fieldSize,
			wxTE_RIGHT | wxTE_PROCESS_ENTER, absValidator
		);
		axis.absolute_text_ctrl->SetToolTip("Target position. Press Enter or the flag button to move there.");

		axis.set_btn = new wxBitmapButton(parent, ids.set, bitmaps.set, wxDefaultPosition, buttonSize);
		axis.set_btn->SetToolTip("Move to the target position");

		axis.decrement_btn = new wxButton(parent, ids.dec, "-", wxDefaultPosition, buttonSize);
		axis.decrement_btn->SetToolTip("Move back by the step");

		wxFloatingPointValidator<float> relValidator(3, nullptr, wxNUM_VAL_DEFAULT);
		relValidator.SetMin(-1000.f);
		relValidator.SetMax(1000.f);

		axis.relative_text_ctrl = new wxTextCtrl
		(
			parent, ids.rel, "1.000", wxDefaultPosition, stepFieldSize,
			wxTE_RIGHT, relValidator
		);
		axis.relative_text_ctrl->SetToolTip("Step for the - and + buttons");

		axis.increment_btn = new wxButton(parent, ids.inc, "+", wxDefaultPosition, buttonSize);
		axis.increment_btn->SetToolTip("Move forward by the step");

		axis.center_btn = new wxBitmapButton(parent, ids.center, bitmaps.center, wxDefaultPosition, buttonSize);
		axis.center_btn->SetToolTip("Move to the center of the travel range");

		axis.home_btn = new wxBitmapButton(parent, ids.home, bitmaps.home, wxDefaultPosition, buttonSize);
		axis.home_btn->SetToolTip("Home the motor (move to its reference position and zero it)");

		const int gap = parent->FromDIP(2);
		grid->Add(axis.name_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, gap * 2);
		grid->Add(axis.position_label, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
		grid->Add(axis.absolute_text_ctrl, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);
		grid->Add(axis.set_btn, 0, wxALIGN_CENTER_VERTICAL);
		grid->Add(axis.decrement_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, gap * 2);
		grid->Add(axis.relative_text_ctrl, 1, wxALIGN_CENTER_VERTICAL | wxEXPAND);
		grid->Add(axis.increment_btn, 0, wxALIGN_CENTER_VERTICAL);
		grid->Add(axis.center_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, gap * 2);
		grid->Add(axis.home_btn, 0, wxALIGN_CENTER_VERTICAL);
	}

	auto CreateAxisGrid(wxWindow* parent, const std::function<wxStaticText* (wxWindow*, const wxString&)>& makeLabel) -> wxFlexGridSizer*
	{
		const int gap = parent->FromDIP(3);
		auto* grid = new wxFlexGridSizer(9, gap, gap);
		grid->AddGrowableCol(2, 2);
		grid->AddGrowableCol(5, 1);

		// Column headings, once per group; numeric columns are right-aligned
		// like their values.
		const std::pair<const char*, bool> headings[] =
		{
			{ "Axis", false }, { "Position", true }, { "Go to (mm)", true }, { "", false },
			{ "", false }, { "Step", true }, { "", false }, { "", false }, { "", false }
		};
		for (const auto& [heading, right] : headings)
			grid->Add(makeLabel(parent, heading), 0, wxALIGN_BOTTOM | (right ? wxALIGN_RIGHT : wxALIGN_LEFT));

		return grid;
	}
}

auto cMain::CreateMotorGroup(wxWindow* parent, wxSizer* sizer, const wxString& title) -> wxCollapsiblePane*
{
	auto* group = new wxCollapsiblePane
	(
		parent, wxID_ANY, title, wxDefaultPosition, wxDefaultSize,
		wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
	);
	group->Expand();

	group->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent&)
		{
			CallAfter([this]() { UpdateMotorControlsLayout(); });
		});

	sizer->Add(group, 0, wxEXPAND | wxBOTTOM, FromDIP(4));
	return group;
}

auto cMain::CreateSteppersControl(wxWindow* parent, wxSizer* sizer) -> void
{
	wxSizer* content{};
	m_MotorSection = CreateSection(parent, "Motors", &m_MotorSummaryText, &content);

	/* Icons for the row buttons: drawn at physical pixels and tagged with
	   the screen scale so wx doesn't enlarge them again. */
	const wxSize iconSize = FromDIP(wxSize(16, 16));
	const auto icon = [&](const char* id, const char* client, const wxColour& colour)
		{
			wxBitmap bitmap = wxMaterialDesignArtProvider::GetBitmap(id, client, iconSize, colour);
			bitmap.SetScaleFactor(GetDPIScaleFactor());
			return bitmap;
		};

	const AxisBitmaps bitmaps
	{
		icon(wxART_FLAG, wxART_CLIENT_FLUENTUI_FILLED, wxColour(230, 100, 100)),
		icon(wxART_CENTER_HORIZONTAL, wxART_CLIENT_FLUENTUI_FILLED, wxColour(230, 128, 0)),
		icon(wxART_HOME, wxART_CLIENT_MATERIAL_FILLED, wxColour(0, 170, 90))
	};

	const wxSize fieldSize(GetTextExtent("-1000.000").GetWidth() + FromDIP(14), wxDefaultCoord);
	const wxSize stepFieldSize(GetTextExtent("100.000").GetWidth() + FromDIP(14), wxDefaultCoord);
	const wxSize buttonSize = FromDIP(wxSize(28, 26));

	int nameWidth = 0;
	{
		wxFont bold = GetFont();
		bold.MakeBold();
		nameWidth = GetTextExtent("Pitch").GetWidth();
		wxClientDC dc(this);
		dc.SetFont(bold);
		nameWidth = dc.GetTextExtent("Pitch").GetWidth() + FromDIP(8);
	}

	/* Stop: always visible, above both the native controls and the web page */
	m_StopMotorsBtn = new wxButton
	(
		m_MotorSection,
		MainFrameVariables::ID::RIGHT_SC_STOP_ALL_BTN,
		"Stop all motors",
		wxDefaultPosition,
		FromDIP(wxSize(-1, 36))
	);
	m_StopMotorsBtn->SetBackgroundColour(wxColour(200, 30, 30));
	m_StopMotorsBtn->SetForegroundColour(*wxWHITE);
	{
		wxFont stopFont = m_StopMotorsBtn->GetFont();
		stopFont.SetPointSize(stopFont.GetPointSize() + 2);
		stopFont.MakeBold();
		m_StopMotorsBtn->SetFont(stopFont);
	}
	m_StopMotorsBtn->SetToolTip("Stop every motor immediately");
	content->Add(m_StopMotorsBtn, 0, wxEXPAND | wxBOTTOM, FromDIP(6));

	/* Native controls or, when no axes are configured, the controller's web page */
	m_MotorControlsBook = new wxSimplebook(m_MotorSection, wxID_ANY);
	m_NativeMotorControlsPage = new wxPanel(m_MotorControlsBook, wxID_ANY);
	m_MotorWebPage = new wxPanel(m_MotorControlsBook, wxID_ANY);

	auto* nativeSizer = new wxBoxSizer(wxVERTICAL);

	const auto buildGroup = [&](const wxString& title,
		std::initializer_list<std::tuple<wxString, MainFrameVariables::StepperControl*, AxisIds>> axes) -> wxWindow*
		{
			wxCollapsiblePane* group = CreateMotorGroup(m_NativeMotorControlsPage, nativeSizer, title);
			wxWindow* pane = group->GetPane();

			wxFlexGridSizer* grid = CreateAxisGrid(pane, [this](wxWindow* p, const wxString& t) { return MakeMutedLabel(p, t); });
			for (const auto& [name, axis, ids] : axes)
				AddAxisRow(pane, grid, name, *axis, ids, bitmaps, fieldSize, stepFieldSize, buttonSize, nameWidth);

			// Remember the widest row before the group may be hidden (Release
			// hides groups until their motors are found), so the right panel
			// can be made wide enough for it.
			m_MotorRowsMinWidth = std::max(m_MotorRowsMinWidth, grid->CalcMin().GetWidth());

			auto* paneSizer = new wxBoxSizer(wxVERTICAL);
			paneSizer->Add(grid, 1, wxEXPAND | wxLEFT | wxTOP, FromDIP(4));
			pane->SetSizer(paneSizer);

#ifndef _DEBUG
			group->Hide(); // shown once its axes are found (EnableUsedAndDisableNonUsedMotors)
#endif
			return group;
		};

	using ID = MainFrameVariables::ID;

	m_DetectorControlsNotebook = buildGroup("Detector",
		{
			{ "X", &m_Detector[0], { ID::RIGHT_SC_DET_X_ABS_TE_CTL, ID::RIGHT_SC_DET_X_SET_BTN, ID::RIGHT_SC_DET_X_REL_TE_CTL, ID::RIGHT_SC_DET_X_DEC_BTN, ID::RIGHT_SC_DET_X_INC_BTN, ID::RIGHT_SC_DET_X_CENTER_BTN, ID::RIGHT_SC_DET_X_HOME_BTN } },
			{ "Y", &m_Detector[1], { ID::RIGHT_SC_DET_Y_ABS_TE_CTL, ID::RIGHT_SC_DET_Y_SET_BTN, ID::RIGHT_SC_DET_Y_REL_TE_CTL, ID::RIGHT_SC_DET_Y_DEC_BTN, ID::RIGHT_SC_DET_Y_INC_BTN, ID::RIGHT_SC_DET_Y_CENTER_BTN, ID::RIGHT_SC_DET_Y_HOME_BTN } },
		});

	m_OpticsControlsNotebook = buildGroup("Optics",
		{
			{ "X", &m_Optics[0], { ID::RIGHT_SC_OPT_X_ABS_TE_CTL, ID::RIGHT_SC_OPT_X_SET_BTN, ID::RIGHT_SC_OPT_X_REL_TE_CTL, ID::RIGHT_SC_OPT_X_DEC_BTN, ID::RIGHT_SC_OPT_X_INC_BTN, ID::RIGHT_SC_OPT_X_CENTER_BTN, ID::RIGHT_SC_OPT_X_HOME_BTN } },
			{ "Y", &m_Optics[1], { ID::RIGHT_SC_OPT_Y_ABS_TE_CTL, ID::RIGHT_SC_OPT_Y_SET_BTN, ID::RIGHT_SC_OPT_Y_REL_TE_CTL, ID::RIGHT_SC_OPT_Y_DEC_BTN, ID::RIGHT_SC_OPT_Y_INC_BTN, ID::RIGHT_SC_OPT_Y_CENTER_BTN, ID::RIGHT_SC_OPT_Y_HOME_BTN } },
			{ "Z", &m_Optics[2], { ID::RIGHT_SC_OPT_Z_ABS_TE_CTL, ID::RIGHT_SC_OPT_Z_SET_BTN, ID::RIGHT_SC_OPT_Z_REL_TE_CTL, ID::RIGHT_SC_OPT_Z_DEC_BTN, ID::RIGHT_SC_OPT_Z_INC_BTN, ID::RIGHT_SC_OPT_Z_CENTER_BTN, ID::RIGHT_SC_OPT_Z_HOME_BTN } },
			{ "Pitch", &m_Optics[3], { ID::RIGHT_SC_OPT_PITCH_ABS_TE_CTL, ID::RIGHT_SC_OPT_PITCH_SET_BTN, ID::RIGHT_SC_OPT_PITCH_REL_TE_CTL, ID::RIGHT_SC_OPT_PITCH_DEC_BTN, ID::RIGHT_SC_OPT_PITCH_INC_BTN, ID::RIGHT_SC_OPT_PITCH_CENTER_BTN, ID::RIGHT_SC_OPT_PITCH_HOME_BTN } },
			{ "Yaw", &m_Optics[4], { ID::RIGHT_SC_OPT_YAW_ABS_TE_CTL, ID::RIGHT_SC_OPT_YAW_SET_BTN, ID::RIGHT_SC_OPT_YAW_REL_TE_CTL, ID::RIGHT_SC_OPT_YAW_DEC_BTN, ID::RIGHT_SC_OPT_YAW_INC_BTN, ID::RIGHT_SC_OPT_YAW_CENTER_BTN, ID::RIGHT_SC_OPT_YAW_HOME_BTN } },
		});

	m_AuxControlsNotebook = buildGroup("Aux",
		{
			{ "X", &m_Aux[0], { ID::RIGHT_SC_AUX_X_ABS_TE_CTL, ID::RIGHT_SC_AUX_X_SET_BTN, ID::RIGHT_SC_AUX_X_REL_TE_CTL, ID::RIGHT_SC_AUX_X_DEC_BTN, ID::RIGHT_SC_AUX_X_INC_BTN, ID::RIGHT_SC_AUX_X_CENTER_BTN, ID::RIGHT_SC_AUX_X_HOME_BTN } },
		});

	m_NativeMotorControlsPage->SetSizer(nativeSizer);

	auto* webSizer = new wxBoxSizer(wxVERTICAL);
	m_MotorsWebView = wxWebView::New(m_MotorWebPage, wxID_ANY, wxWebViewDefaultURLStr);
	if (m_MotorsWebView)
		webSizer->Add(m_MotorsWebView, 1, wxEXPAND);
	m_MotorWebPage->SetSizer(webSizer);

	m_MotorControlsBook->AddPage(m_NativeMotorControlsPage, "Native Motor Controls", true);
	m_MotorControlsBook->AddPage(m_MotorWebPage, "Motor Web Interface", false);

	content->Add(m_MotorControlsBook, 1, wxEXPAND);

	sizer->Add(m_MotorSection, 1, wxEXPAND | wxALL, FromDIP(6));
}

auto cMain::FindAxisByTextCtrl(const wxTextCtrl* ctrl) -> MainFrameVariables::StepperControl*
{
	if (!ctrl)
		return nullptr;

	for (int i = 0; i < 2; ++i) if (m_Detector[i].absolute_text_ctrl == ctrl) return &m_Detector[i];
	for (int i = 0; i < 5; ++i) if (m_Optics[i].absolute_text_ctrl == ctrl) return &m_Optics[i];
	if (m_Aux[0].absolute_text_ctrl == ctrl) return &m_Aux[0];
	return nullptr;
}

void cMain::RefreshAxisPositionLabels()
{
	if (!m_Settings)
		return;

	const std::pair<MainFrameVariables::StepperControl*, int> axes[] =
	{
		{ &m_Detector[0], SettingsVariables::DETECTOR_X },
		{ &m_Detector[1], SettingsVariables::DETECTOR_Y },
		{ &m_Optics[0], SettingsVariables::OPTICS_X },
		{ &m_Optics[1], SettingsVariables::OPTICS_Y },
		{ &m_Optics[2], SettingsVariables::OPTICS_Z },
		{ &m_Optics[3], SettingsVariables::OPTICS_PITCH },
		{ &m_Optics[4], SettingsVariables::OPTICS_YAW },
		{ &m_Aux[0], SettingsVariables::AUX_X },
	};

	const wxColour normal = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
	const wxColour moving(0, 120, 215);

	for (const auto& [axis, motor] : axes)
	{
		if (!axis->position_label)
			continue;

		wxString text = m_Settings->MotorHasSerialNumber(motor)
			? wxString::Format("%.3f", m_Settings->GetActualMotorPosition(motor))
			: wxString("-");

		const bool isMoving = (axis == m_MovingAxis);
		if (isMoving)
			text = "moving";

		SetLabelIfChanged(axis->position_label, text);

		const wxColour colour = isMoving ? moving : normal;
		if (axis->position_label->GetForegroundColour() != colour)
		{
			axis->position_label->SetForegroundColour(colour);
			axis->position_label->Refresh();
		}
	}
}

/* ======================= Detector ======================= */

auto cMain::CreateDeviceControls(wxWindow* parent, wxSizer* sizer) -> void
{
	wxSizer* content{};
	m_DetectorSection = CreateSection(parent, "Detector", &m_DetectorSummaryText, &content);
	m_DeviceControlsNotebook = m_DetectorSection;

	wxPanel* page = m_DetectorSection;
	const int gap = FromDIP(6);
	const wxSize fieldSize(GetTextExtent("1000000.000").GetWidth() + FromDIP(12), wxDefaultCoord);

	/* Exposure and displayed energy range */
	{
		auto* grid = new wxFlexGridSizer(3, FromDIP(2), gap);
		for (int i = 0; i < 3; ++i)
			grid->AddGrowableCol(i, 1);

		grid->Add(MakeMutedLabel(page, "Exposure (s)"));
		grid->Add(MakeMutedLabel(page, "Range from (keV)"));
		grid->Add(MakeMutedLabel(page, "to (keV)"));

		wxIntegerValidator<int> exposureValidator(nullptr, wxNUM_VAL_ZERO_AS_BLANK);
		exposureValidator.SetMin(1);
		exposureValidator.SetMax(1'000'000);

		m_DeviceExposure = std::make_unique<wxTextCtrl>
			(
				page, MainFrameVariables::ID::RIGHT_CAM_EXPOSURE_TXT_CTL, "1",
				wxDefaultPosition, fieldSize, wxTE_RIGHT | wxTE_PROCESS_ENTER, exposureValidator
			);
		m_DeviceExposure->Disable();
		m_DeviceExposure->SetToolTip("Exposure time per spectrum in seconds. Press Enter to apply.");

		wxFloatingPointValidator<float> minValidator(3, nullptr);
		minValidator.SetMin(0.0);
		minValidator.SetMax(1'000'000.0);

		m_MinRangeKEVTxtCtrl = std::make_unique<wxTextCtrl>
			(
				page, MainFrameVariables::ID::RIGHT_DEVICE_MIN_RANGE_TXT_CTL, "0.000",
				wxDefaultPosition, fieldSize, wxTE_RIGHT | wxTE_PROCESS_ENTER, minValidator
			);
		m_MinRangeKEVTxtCtrl->Disable();
		m_MinRangeKEVTxtCtrl->SetToolTip("Lowest energy shown in the plot. Press Enter to apply.");

		wxFloatingPointValidator<float> maxValidator(3, nullptr);
		maxValidator.SetMin(0.0);
		maxValidator.SetMax(1'000'000.0);

		m_MaxRangeKEVTxtCtrl = std::make_unique<wxTextCtrl>
			(
				page, MainFrameVariables::ID::RIGHT_DEVICE_MAX_RANGE_TXT_CTL, "0.000",
				wxDefaultPosition, fieldSize, wxTE_RIGHT | wxTE_PROCESS_ENTER, maxValidator
			);
		m_MaxRangeKEVTxtCtrl->Disable();
		m_MaxRangeKEVTxtCtrl->SetToolTip("Highest energy shown in the plot. Press Enter to apply.");

		grid->Add(m_DeviceExposure.get(), 1, wxEXPAND);
		grid->Add(m_MinRangeKEVTxtCtrl.get(), 1, wxEXPAND);
		grid->Add(m_MaxRangeKEVTxtCtrl.get(), 1, wxEXPAND);

		content->Add(grid, 0, wxEXPAND | wxBOTTOM, gap);
	}

	/* Single shot and live capturing */
	{
		auto* buttons = new wxBoxSizer(wxHORIZONTAL);
		const wxSize buttonSize = FromDIP(wxSize(-1, 34));

		wxBitmap shotBitmap = wxMaterialDesignArtProvider::GetBitmap
		(
			wxART_PLUS_ONE, wxART_CLIENT_MATERIAL_ROUND, FromDIP(wxSize(20, 20)), m_DefaultWidgetsColor
		);
		shotBitmap.SetScaleFactor(GetDPIScaleFactor());

		m_SingleShotBtn = std::make_unique<wxButton>
			(
				page, MainFrameVariables::ID::RIGHT_DEVICE_SINGLE_SHOT_BTN, "Single shot",
				wxDefaultPosition, buttonSize
			);
		m_SingleShotBtn->SetBitmap(shotBitmap);
		m_SingleShotBtn->SetToolTip("Capture one spectrum and save it (Ctrl+Shift+S)");
		m_SingleShotBtn->Disable();

		m_StartStopLiveCapturingTglBtn = std::make_unique<wxToggleButton>
			(
				page, MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN, "Start live",
				wxDefaultPosition, buttonSize
			);
		m_StartStopLiveCapturingTglBtn->SetBitmap(GetLiveCapturingBitmap(false));
		m_StartStopLiveCapturingTglBtn->SetToolTip("Capture spectra continuously without saving them (Ctrl+Shift+L)");
		m_StartStopLiveCapturingTglBtn->Disable();

		buttons->Add(m_SingleShotBtn.get(), 1, wxEXPAND | wxRIGHT, gap / 2);
		buttons->Add(m_StartStopLiveCapturingTglBtn.get(), 1, wxEXPAND | wxLEFT, gap / 2);
		content->Add(buttons, 0, wxEXPAND | wxBOTTOM, gap);
	}

	/* Exposure progress (shown while capturing) */
	{
		m_ExposureProgressPanel = new wxPanel(page);
		m_ExposureProgressPanelSizer = new wxBoxSizer(wxVERTICAL);

		m_ExposureGauge = std::make_unique<wxGauge>(m_ExposureProgressPanel, wxID_ANY, 100, wxDefaultPosition, FromDIP(wxSize(-1, 10)));
		m_ExposureGauge->SetValue(0);
		m_ExposureProgressPanelSizer->Add(m_ExposureGauge.get(), 0, wxEXPAND | wxBOTTOM, FromDIP(2));

		m_ExposureProgressStaticText = std::make_unique<wxStaticText>(m_ExposureProgressPanel, wxID_ANY, "Exposure Progress: 0%");
		m_ExposureProgressStaticText->SetForegroundColour(MutedTextColour());
		m_MutedLabels.push_back(m_ExposureProgressStaticText.get());
		m_ExposureProgressPanelSizer->Add(m_ExposureProgressStaticText.get(), 0, wxEXPAND);

		m_ExposureProgressPanel->SetSizer(m_ExposureProgressPanelSizer);
		content->Add(m_ExposureProgressPanel, 0, wxEXPAND | wxBOTTOM, gap);
		m_ExposureProgressPanel->Hide();
	}

	/* Detector parameters and temperatures, collapsed by default */
	{
		auto* details = new wxCollapsiblePane
		(
			page, wxID_ANY, "Detector settings", wxDefaultPosition, wxDefaultSize,
			wxCP_DEFAULT_STYLE | wxCP_NO_TLW_RESIZE
		);

		wxWindow* pane = details->GetPane();
		auto* paneSizer = new wxBoxSizer(wxVERTICAL);
		wxWindow* properties = CreatePropertiesPage(pane);
		properties->SetMinSize(FromDIP(wxSize(-1, 230)));
		paneSizer->Add(properties, 1, wxEXPAND | wxTOP, FromDIP(4));
		pane->SetSizer(paneSizer);

		details->Bind(wxEVT_COLLAPSIBLEPANE_CHANGED, [this](wxCollapsiblePaneEvent&)
			{
				CallAfter([this]() { UpdateMotorControlsLayout(); });
			});

		content->Add(details, 0, wxEXPAND);
	}

	sizer->Add(m_DetectorSection, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(6));
}

/* ======================= Measurement ======================= */

auto cMain::CreateMeasurement(wxWindow* parent, wxSizer* sizer) -> void
{
	wxSizer* content{};
	m_MeasurementSection = CreateSection(parent, "Measurement", nullptr, &content);
	m_MeasurementControlsNotebook = m_MeasurementSection;

	wxPanel* page = m_MeasurementSection;
	const int gap = FromDIP(6);

	/* Output folder */
	{
		content->Add(MakeMutedLabel(page, "Output folder"), 0, wxBOTTOM, FromDIP(2));

		auto* row = new wxBoxSizer(wxHORIZONTAL);

		m_OutDirTextCtrl = std::make_unique<wxTextCtrl>
			(
				page, MainFrameVariables::ID::RIGHT_MT_OUT_FLD_TE_CTL, "Save directory...",
				wxDefaultPosition, wxDefaultSize, wxTE_LEFT | wxTE_READONLY
			);
		m_OutDirTextCtrl->SetToolTip("Folder where spectra, graphs and logs of a measurement are saved");

		wxBitmap folderBitmap = wxMaterialDesignArtProvider::GetBitmap
		(
			wxART_FOLDER_OPEN, wxART_CLIENT_MATERIAL_ROUND, FromDIP(wxSize(18, 18)), m_DefaultWidgetsColor
		);
		folderBitmap.SetScaleFactor(GetDPIScaleFactor());

		m_OutDirBtn = std::make_unique<wxBitmapButton>(page, MainFrameVariables::ID::RIGHT_MT_OUT_FLD_BTN, folderBitmap);
		m_OutDirBtn->SetToolTip("Choose the output folder");

		row->Add(m_OutDirTextCtrl.get(), 1, wxALIGN_CENTER_VERTICAL);
		row->Add(m_OutDirBtn.get(), 0, wxALIGN_CENTER_VERTICAL | wxLEFT, FromDIP(4));
		content->Add(row, 0, wxEXPAND | wxBOTTOM, gap);
	}

	const wxSize fieldSize(GetTextExtent("-1000.000").GetWidth() + FromDIP(16), wxDefaultCoord);

	const auto addScanAxis = [&](MainFrameVariables::MeasurementStage& stage, int choiceId, int startId, int stepId, int finishId, const wxString& title)
		{
			content->Add(MakeMutedLabel(page, title), 0, wxBOTTOM, FromDIP(2));

			stage.stage = new wxChoice(page, choiceId, wxDefaultPosition, wxDefaultSize, stage.motors);
			stage.stage->SetSelection(0);
			stage.stage->SetToolTip("Motor moved between the measurement points");
			content->Add(stage.stage, 0, wxEXPAND | wxBOTTOM, gap);

			auto* grid = new wxFlexGridSizer(3, FromDIP(2), gap);
			for (int i = 0; i < 3; ++i)
				grid->AddGrowableCol(i, 1);

			grid->Add(MakeMutedLabel(page, "Start"));
			grid->Add(MakeMutedLabel(page, "Step"));
			grid->Add(MakeMutedLabel(page, "Finish"));

			const auto makeField = [&](int id, const wxString& tooltip)
				{
					wxFloatingPointValidator<float> validator(3, nullptr, wxNUM_VAL_DEFAULT);
					validator.SetMin(-1000.0);
					validator.SetMax(1000.0);

					auto* field = new wxTextCtrl(page, id, "0.000", wxDefaultPosition, fieldSize, wxTE_RIGHT, validator);
					field->SetToolTip(tooltip);
					grid->Add(field, 1, wxEXPAND);
					return field;
				};

			stage.start = makeField(startId, "Position of the first measurement point");
			stage.step = makeField(stepId, "Distance between measurement points (negative to scan downwards)");
			stage.finish = makeField(finishId, "Position of the last measurement point");

			content->Add(grid, 0, wxEXPAND | wxBOTTOM, FromDIP(4));
		};

	addScanAxis
	(
		*m_FirstStage,
		MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_CHOICE,
		MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_START,
		MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_STEP,
		MainFrameVariables::ID::RIGHT_MT_FIRST_STAGE_FINISH,
		"Scan axis"
	);

#ifdef USE_2_AXIS_MEASUREMENT
	addScanAxis
	(
		*m_SecondStage,
		MainFrameVariables::ID::RIGHT_MT_SECOND_STAGE_CHOICE,
		MainFrameVariables::ID::RIGHT_MT_SECOND_STAGE_START,
		MainFrameVariables::ID::RIGHT_MT_SECOND_STAGE_STEP,
		MainFrameVariables::ID::RIGHT_MT_SECOND_STAGE_FINISH,
		"Second scan axis"
	);
#endif

	/* Preview of the scan, updated while typing */
	m_ScanPreviewText = new wxStaticText(page, wxID_ANY, wxEmptyString);
	m_ScanPreviewText->SetForegroundColour(MutedTextColour());
	content->Add(m_ScanPreviewText, 0, wxEXPAND | wxBOTTOM, gap);

	const auto updatePreview = [this](wxEvent& evt)
		{
			evt.Skip(); // the regular handlers still run
			CallAfter([this]() { UpdateScanPreview(); });
		};

	for (wxTextCtrl* field : { m_FirstStage->start, m_FirstStage->step, m_FirstStage->finish })
		field->Bind(wxEVT_TEXT, updatePreview);
	m_FirstStage->stage->Bind(wxEVT_CHOICE, updatePreview);
	m_DeviceExposure->Bind(wxEVT_TEXT, updatePreview);

	/* Start / stop */
	m_StartStopMeasurementTglBtn = std::make_unique<wxToggleButton>
		(
			page, MainFrameVariables::ID::RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN, "Start measurement",
			wxDefaultPosition, FromDIP(wxSize(-1, 36))
		);
	m_StartStopMeasurementTglBtn->SetBitmap(GetMeasurementBitmap(false));
	m_StartStopMeasurementTglBtn->SetToolTip("Move the scan axis through all points, capturing and saving a spectrum at each (Ctrl+Shift+M)");
	m_StartStopMeasurementTglBtn->Disable();
	{
		wxFont font = m_StartStopMeasurementTglBtn->GetFont();
		font.MakeBold();
		m_StartStopMeasurementTglBtn->SetFont(font);
	}
	content->Add(m_StartStopMeasurementTglBtn.get(), 0, wxEXPAND);

	sizer->Add(m_MeasurementSection, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, FromDIP(6));

	CallAfter([this]() { UpdateScanPreview(); });
}

void cMain::UpdateScanPreview()
{
	if (!m_ScanPreviewText || !m_FirstStage || !m_FirstStage->stage)
		return;

	const wxColour muted = MutedTextColour();
	const bool dark = muted.Red() > 128;
	const wxColour danger = dark ? wxColour(255, 120, 110) : wxColour(190, 30, 30);

	const auto show = [&](const wxString& text, bool problem)
		{
			SetLabelIfChanged(m_ScanPreviewText, text);
			const wxColour colour = problem ? danger : muted;
			if (m_ScanPreviewText->GetForegroundColour() != colour)
			{
				m_ScanPreviewText->SetForegroundColour(colour);
				m_ScanPreviewText->Refresh();
			}
		};

	if (m_FirstStage->stage->GetSelection() <= 0)
	{
		show("Choose the motor to scan.", false);
		return;
	}

	double start{}, step{}, finish{};
	if (!m_FirstStage->start->GetValue().ToDouble(&start) ||
		!m_FirstStage->step->GetValue().ToDouble(&step) ||
		!m_FirstStage->finish->GetValue().ToDouble(&finish))
	{
		show("Enter numbers for Start, Step and Finish.", true);
		return;
	}

	// Same rounding to micrometres as StartCapturing
	const long long startUm = std::llround(start * 1000.0);
	const long long stepUm = std::llround(step * 1000.0);
	const long long finishUm = std::llround(finish * 1000.0);

	if (stepUm == 0)
	{
		show("Step can't be 0 (the smallest step is 0.001).", true);
		return;
	}

	if ((finishUm - startUm < 0 && stepUm > 0) || (finishUm - startUm > 0 && stepUm < 0))
	{
		show("Finish can't be reached from Start with this Step - check the sign of Step.", true);
		return;
	}

	const long long points = (finishUm - startUm) / stepUm + 1;

	long exposure = 0;
	m_DeviceExposure->GetValue().ToLong(&exposure);
	exposure = std::max(1L, exposure);

	const long long seconds = points * exposure;
	const wxString duration = seconds >= 3600
		? wxString::Format("%lld h %02lld min", seconds / 3600, (seconds / 60) % 60)
		: seconds >= 60
		? wxString::Format("%lld min %02lld s", seconds / 60, seconds % 60)
		: wxString::Format("%lld s", seconds);

	show
	(
		wxString::Format
		(
			"%lld point%s, %s of exposure plus stage moves.",
			points, points == 1 ? "" : "s", duration
		),
		false
	);
}
