// cMain: motor control - moves on a background thread, stop, positions.
#include "cMain.h"

void cMain::UpdateStagePositions()
{
	m_Detector[0].absolute_text_ctrl->SetValue
	(
		wxString::Format(wxT("%.3f"),
			m_Settings->GetActualMotorPosition(SettingsVariables::DETECTOR_X))
	);

	m_Detector[1].absolute_text_ctrl->SetValue
	(
		wxString::Format(wxT("%.3f"),
			m_Settings->GetActualMotorPosition(SettingsVariables::DETECTOR_Y))
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

	m_Aux[0].absolute_text_ctrl->SetValue
	(
		wxString::Format(wxT("%.3f"),
			m_Settings->GetActualMotorPosition(SettingsVariables::AUX_X))
	);

	RefreshAxisPositionLabels();
}

void cMain::EnableUsedAndDisableNonUsedMotors()
{
	auto enableDetectorNotebook = false;

	/* Detector X */
	{
		auto enable = false;

		if (m_Settings->MotorHasSerialNumber(SettingsVariables::DETECTOR_X))
			enableDetectorNotebook = enable = true;

		m_Detector[0].EnableAllControls(enable);
	}

	/* Detector Y */
	{
		auto enable = false;
		if (m_Settings->MotorHasSerialNumber(SettingsVariables::DETECTOR_Y))
			enableDetectorNotebook = enable = true;

		m_Detector[1].EnableAllControls(enable);
	}

	m_DetectorControlsNotebook->Enable(enableDetectorNotebook);

#ifndef _DEBUG
	m_DetectorControlsNotebook->Show(enableDetectorNotebook);
#endif // _DEBUG

	auto enableOpticsNotebook = false;

	/* Optics X */
	{
		auto enable = false;

		if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_X))
			enableOpticsNotebook = enable = true;

		m_Optics[0].EnableAllControls(enable);
	}

	/* Optics Y */
	{
		auto enable = false;

		if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_Y))
			enableOpticsNotebook = enable = true;

		m_Optics[1].EnableAllControls(enable);
	}

	/* Optics Z */
	{
		auto enable = false;

		if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_Z))
			enableOpticsNotebook = enable = true;

		m_Optics[2].EnableAllControls(enable);
	}

	/* Optics Pitch */
	{
		auto enable = false;

		if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_PITCH))
			enableOpticsNotebook = enable = true;

		m_Optics[3].EnableAllControls(enable);
	}

	/* Optics Yaw */
	{
		auto enable = false;

		if (m_Settings->MotorHasSerialNumber(SettingsVariables::OPTICS_YAW))
			enableOpticsNotebook = enable = true;

		m_Optics[4].EnableAllControls(enable);
	}

	m_OpticsControlsNotebook->Enable(enableOpticsNotebook);

#ifndef _DEBUG
	m_OpticsControlsNotebook->Show(enableOpticsNotebook);
#endif // _DEBUG

	auto enableAuxNotebook = false;

	/* Aux X */
	{
		auto enable = false;

		if (m_Settings->MotorHasSerialNumber(SettingsVariables::AUX_X))
			enableAuxNotebook = enable = true;

		m_Aux[0].EnableAllControls(enable);
	}

	m_AuxControlsNotebook->Enable(enableAuxNotebook);

#ifndef _DEBUG
	m_AuxControlsNotebook->Show(enableAuxNotebook);
#endif // _DEBUG

	m_HasDetectedMotorAxes =
		enableDetectorNotebook ||
		enableOpticsNotebook ||
		enableAuxNotebook;

	m_StopMotorsBtn->Show(m_HasDetectedMotorAxes);

#ifndef _DEBUG
	UpdateMotorControlsMode();
#endif // _DEBUG

	RefreshAxisPositionLabels();

	CallAfter([this]()
		{
			UpdateMotorControlsLayout();
		});
}

wxString cMain::GetMotorsWebURL() const
{
	wxString address =
		m_Settings
		? m_Settings->GetIPAddress()
		: m_DefaultMotorsIPAddress;

	address.Trim(true);
	address.Trim(false);

	if (address.IsEmpty())
		return {};

	if (!address.StartsWith("http://") &&
		!address.StartsWith("https://"))
	{
		address.Prepend("http://");
	}

	return address;
}

void cMain::UpdateMotorControlsMode()
{
	if (!m_MotorControlsBook)
		return;

	if (m_HasDetectedMotorAxes)
	{
		m_MotorControlsBook->ChangeSelection(0);
		return;
	}

	m_MotorControlsBook->ChangeSelection(1);

	if (!m_MotorsWebView || m_MotorsWebInterfaceLoaded)
		return;

	const wxString url = GetMotorsWebURL();

	if (url.IsEmpty())
	{
		wxLogWarning(
			"Motor web interface cannot be opened because "
			"motors_ip_address is empty."
		);

		return;
	}

	m_MotorsWebView->LoadURL(url);
	m_MotorsWebInterfaceLoaded = true;
}

void cMain::UpdateMotorControlsLayout()
{
	if (!m_RightSidePanel ||
		!m_MotorControlsContainer ||
		!m_DeviceMeasurementContainer ||
		!m_MotorControlsBook)
	{
		return;
	}

	if (m_DetectorControlsNotebook)
		m_DetectorControlsNotebook->InvalidateBestSize();

	if (m_OpticsControlsNotebook)
		m_OpticsControlsNotebook->InvalidateBestSize();

	if (m_AuxControlsNotebook)
		m_AuxControlsNotebook->InvalidateBestSize();

	int motorBookHeight = FromDIP(300); // the web interface page

	// Native controls shown (always in Debug): use their full height.
	if (m_NativeMotorControlsPage && m_MotorControlsBook->GetSelection() == 0)
	{
		m_NativeMotorControlsPage->InvalidateBestSize();
		m_NativeMotorControlsPage->Layout();

		if (wxSizer* nativeSizer =
			m_NativeMotorControlsPage->GetSizer())
		{
			motorBookHeight =
				nativeSizer->CalcMin().GetHeight();
		}
	}

	// wxSimplebook otherwise may report the height of its current
	// viewport instead of the complete native-controls content.
	m_MotorControlsBook->SetMinSize(
		wxSize(-1, motorBookHeight)
	);

	m_MotorControlsBook->InvalidateBestSize();
	m_MotorControlsBook->Layout();

	m_MotorControlsContainer->InvalidateBestSize();
	m_MotorControlsContainer->Layout();

	m_DeviceMeasurementContainer->InvalidateBestSize();
	m_DeviceMeasurementContainer->Layout();

	m_RightSidePanel->InvalidateBestSize();
	m_RightSidePanel->Layout();
	m_RightSidePanel->FitInside();
	m_RightSidePanel->Refresh();

	Layout();
	Refresh();
}

void cMain::UpdateAllAxisGlobalPositions()
{
	/* Detectors */
	m_Detector[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::DETECTOR_X)));
	m_Detector[1].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::DETECTOR_Y)));

	/* Optics */
	m_Optics[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_X)));
	m_Optics[1].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_Y)));
	m_Optics[2].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_Z)));
	m_Optics[3].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_PITCH)));
	m_Optics[4].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::OPTICS_YAW)));

	/* Aux */
	m_Aux[0].absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), m_Settings->GetActualMotorPosition(SettingsVariables::AUX_X)));

	RefreshAxisPositionLabels();
}
/* ___ End Motor Move Thread ___ */

void cMain::LaunchMotorMove(wxTextCtrl* targetCtrl, bool useChangeValue, std::function<float()> operation, bool refreshAllPositions)
{
	if (m_MotorMoveInProgress.exchange(true))
		return; // a move is already running; ignore this click

	wxBeginBusyCursor();
	m_MovingAxis = FindAxisByTextCtrl(targetCtrl);
	RefreshAxisPositionLabels();
	UpdateStatusFields();
	m_DetectorControlsNotebook->Enable(false);
	m_OpticsControlsNotebook->Enable(false);
	m_AuxControlsNotebook->Enable(false);

	auto* thread = new MotorMoveThread(this, std::move(operation), targetCtrl, useChangeValue, refreshAllPositions);

	if (thread->Run() != wxTHREAD_NO_ERROR)
	{
		delete thread;

		m_DetectorControlsNotebook->Enable(true);
		m_OpticsControlsNotebook->Enable(true);
		m_AuxControlsNotebook->Enable(true);
		wxEndBusyCursor();
		m_MotorMoveInProgress = false;
		m_MovingAxis = nullptr;
		RefreshAxisPositionLabels();
	}
}

void cMain::OnStopAllMotors(wxCommandEvent& evt)
{
	// command_stop() is fast and non-blocking (confirmed against libximc's
	// own docs), so this can be issued directly from the UI thread - no
	// Xeryon-style background dispatch needed here, this project is
	// Standa/IP only. It reuses the same device handle the in-progress
	// move thread is using (see DeviceHandle/Motor::SetActiveDevice),
	// which is why the move unblocks almost immediately after this runs.
	m_Settings->StopAllMotors();
}

void cMain::OnMotorMoveFinished(wxThreadEvent& evt)
{
	const auto payload = evt.GetPayload<MainFrameVariables::MotorMovePayload>();

	if (!std::isfinite(payload.newPosition))
	{
		// Show the last known positions rather than "nan", re-enable the
		// controls first, then tell the user.
		UpdateStagePositions();

		m_DetectorControlsNotebook->Enable(true);
		m_OpticsControlsNotebook->Enable(true);
		m_AuxControlsNotebook->Enable(true);
		wxEndBusyCursor();
		m_MotorMoveInProgress = false;
		m_MovingAxis = nullptr;
		RefreshAxisPositionLabels();

		wxMessageBox
		(
			"The motor movement could not be completed.\n\n"
			"Possible causes: the target is outside the stage range, the motor "
			"controller is not connected or not reachable, or the movement "
			"timed out.\n\nThe displayed positions are the last known ones.",
			"Motor movement error",
			wxOK | wxICON_ERROR,
			this
		);
		return;
	}

	if (payload.targetCtrl)
	{
		const auto text = wxString::Format(wxT("%.3f"), payload.newPosition);

		if (payload.useChangeValue)
			payload.targetCtrl->ChangeValue(text);
		else
			payload.targetCtrl->SetValue(text);
	}

	if (payload.refreshAllPositions)
		UpdateStagePositions();

	m_DetectorControlsNotebook->Enable(true);
	m_OpticsControlsNotebook->Enable(true);
	m_AuxControlsNotebook->Enable(true);

	wxEndBusyCursor();

	m_MotorMoveInProgress = false;
	m_MovingAxis = nullptr;
	RefreshAxisPositionLabels();
	UpdateStatusFields();
}

auto cMain::OfferMoveToBestPosition(const MainFrameVariables::MeasurementSummary& summary) -> void
{
#ifndef _DEBUG
	if (!summary.maxSum)
		return;
#endif // !_DEBUG

	if (summary.axisNumber < SettingsVariables::DETECTOR_X ||
		summary.axisNumber > SettingsVariables::AUX_X)
		return;

	wxString message =
		"The maximum sum value was: " + wxString::Format(wxT("%llu"), summary.maxSum) + '\n'
		+ "at position: " + wxString::Format(wxT("%.3f"), summary.bestPosition) + '\n'
		+ "measurement number: " + wxString::Format(wxT("%i"), summary.bestMeasurementNumber + 1);
	message += "\nDo you want to move stage to the best position?";

	if (wxMessageBox(message, "Move stage?", wxICON_QUESTION | wxYES_NO, this) != wxYES)
		return;

	auto* settings = m_Settings.get();
	const int motorId = summary.axisNumber;
	const float target = summary.bestPosition;

	LaunchMotorMove
	(
		nullptr,
		false,
		[settings, motorId, target]() { return settings->GoToAbsPos(motorId, target); },
		true
	);
}
