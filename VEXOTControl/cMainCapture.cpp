// cMain: capture and measurement control - starting/stopping single shot,
// live capturing and measurements, thread result handlers, status bar.
#include "cMain.h"

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

	auto control = BeginNewCapture();
	if (!control)
		return;

	ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::SingleShotRunning);

	ShowExposureProgressControls();

	SingleShotThread* singleShotThread = new SingleShotThread
	(
		this,
		m_KetekHandler.get(),
		control,
		outDir,
		exposure_time
	);

	AppLog::Info("Capture", "Single shot started, exposure " + std::to_string(exposure_time) + " s.");

	if (singleShotThread->Run() != wxTHREAD_NO_ERROR)
	{
		delete singleShotThread;
		control->continueRunning = false;
		control->finished = true;

		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
		EndExposureProgress();
		wxLogError("Could not start the single shot thread.");
		return;
	}
}

auto cMain::IsBackgroundWorkRunning() const -> bool
{
	return TrackedThread::LiveCount() > 0 || m_MotorMoveInProgress.load();
}

auto cMain::StopActiveCapture(const int timeoutMs) -> bool
{
	if (!m_ActiveCapture || m_ActiveCapture->finished)
		return true;

	m_ActiveCapture->continueRunning = false;

	wxBusyCursor busyCursor;
	const auto deadline =
		std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);

	// Capture threads never wait for the UI thread, so blocking here cannot
	// deadlock; the detector polls the stop flag every 10 ms, and a
	// measurement finishes its current stage move first.
	while (!m_ActiveCapture->finished && std::chrono::steady_clock::now() < deadline)
		wxMilliSleep(20);

	if (!m_ActiveCapture->finished)
	{
		wxLogWarning("The capture thread did not stop within %d seconds.", timeoutMs / 1000);
		return false;
	}

	return true;
}

auto cMain::BeginNewCapture() -> MainFrameVariables::ThreadControlPtr
{
	if (!StopActiveCapture())
	{
		wxLogError("The previous capture is still running, so a new one can't be started yet.");
		return nullptr;
	}

	m_ActiveCapture = std::make_shared<MainFrameVariables::ThreadControl>();
	m_ActiveCapture->generation = ++m_CaptureGeneration;
	return m_ActiveCapture;
}

auto cMain::IsEventFromActiveCapture(const wxThreadEvent& evt) const -> bool
{
	return m_ActiveCapture && evt.GetExtraLong() == m_ActiveCapture->generation;
}

auto cMain::ShutdownBackgroundWork(const int timeoutMs) -> bool
{
	if (m_ActiveCapture)
		m_ActiveCapture->continueRunning = false;

	if (m_Settings)
		m_Settings->StopAllMotors();

	const auto deadline =
		std::chrono::steady_clock::now() + std::chrono::milliseconds(timeoutMs);

	while (TrackedThread::LiveCount() > 0 && std::chrono::steady_clock::now() < deadline)
		wxMilliSleep(20);

	return TrackedThread::LiveCount() == 0;
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

			// StartCapturing may have shown progress UI before failing.
			m_AppProgressIndicator.reset();
			m_ActivityText.clear();
			if (m_PreviewPanel)
				m_PreviewPanel->SetPerformanceOverlayEnabled(false);
			EndExposureProgress();

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
		m_StartStopMeasurementTglBtn->Disable();
		// Interrupt the current stage move as well; otherwise the worker
		// finishes it (up to the move timeout) before it sees the stop flag.
		if (m_ActiveCapture && !m_ActiveCapture->finished)
			m_Settings->StopAllMotors();
		StopActiveCapture();
		m_StartStopMeasurementTglBtn->Enable();

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
			if (!m_FirstStage->start->GetValue().ToDouble(&start_first_stage_value))
				return false;

			if (!m_FirstStage->step->GetValue().ToDouble(&step_first_stage_value))
				return false;

			if (!m_FirstStage->finish->GetValue().ToDouble(&finish_first_stage_value))
				return false;

			const auto startUm =
				std::llround(start_first_stage_value * 1000.0);

			const auto stepUm =
				std::llround(step_first_stage_value * 1000.0);

			const auto finishUm =
				std::llround(finish_first_stage_value * 1000.0);

			if (stepUm == 0)
			{
				wxMessageBox
				(
					"Measurement step must be at least 0.001 in the stage units.",
					"Stage error",
					wxOK | wxICON_ERROR
				);

				return false;
			}

			first_axis->start =
				static_cast<float>(startUm / 1000.0);

			first_axis->step =
				static_cast<float>(stepUm / 1000.0);

			first_axis->finish =
				static_cast<float>(finishUm / 1000.0);

			if (
				(finish_first_stage_value - start_first_stage_value < 0.0 &&
					step_first_stage_value > 0.0)
				||
				(finish_first_stage_value - start_first_stage_value > 0.0 &&
					step_first_stage_value < 0.0)
				)
			{
				raise_exception_msg("first");
				return false;
			}

			first_axis->step_number =
				static_cast<int>((finishUm - startUm) / stepUm + 1);
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

		m_Settings->ResetCapturing();
		m_ActivityText = "Measuring: moving to the start position";
		UpdateStatusFields();

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

	auto control = BeginNewCapture();
	if (!control)
		return false;

	/* Worker and Progress Threads */
	{
		auto out_dir = m_OutDirTextCtrl->GetValue();

		const int axisNumberForLog = first_axis->axis_number;
		const float startForLog = first_axis->start;
		const float stepForLog = first_axis->step;
		const int stepCountForLog = first_axis->step_number;

		WorkerThread* worker_thread = new WorkerThread
		(
			this,
			m_Settings.get(),
			m_KetekHandler.get(),
			control,
			out_dir,
			exposureSeconds,
			first_axis.release(),
			second_axis.release(),
			m_GraphFontSize
		);

		AppLog::Info
		(
			"Measurement",
			"Measurement started: axis " + std::to_string(axisNumberForLog) +
			", start " + std::to_string(startForLog) +
			", step " + std::to_string(stepForLog) +
			", " + std::to_string(stepCountForLog) + " steps, exposure " +
			std::to_string(exposureSeconds) + " s, output " + out_dir.utf8_string()
		);

		if (worker_thread->Run() != wxTHREAD_NO_ERROR)
		{
			delete worker_thread;
			control->continueRunning = false;
			control->finished = true;
			wxLogError("Could not start the measurement thread.");
			return false;
		}

		// The progress thread only reports progress; if it can't start, the
		// measurement still runs.
		ProgressThread* progress_thread = new ProgressThread(this, m_Settings.get(), control);
		if (progress_thread->Run() != wxTHREAD_NO_ERROR)
			delete progress_thread;
	}
	return true;
}

void cMain::StartLiveCapturing()
{
	wxBusyCursor cursor;

	auto control = BeginNewCapture();
	if (!control)
	{
		m_StartStopLiveCapturingTglBtn->SetValue(false);
		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
		return;
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

	LiveCapturing* live_capturing = new LiveCapturing
	(
		this,
		m_KetekHandler.get(),
		control,
		exposureSeconds
	);

	AppLog::Info("Capture", "Live capturing started, exposure " + std::to_string(exposureSeconds) + " s.");

	if (live_capturing->Run() != wxTHREAD_NO_ERROR)
	{
		delete live_capturing;
		control->continueRunning = false;
		control->finished = true;

		m_StartStopLiveCapturingTglBtn->SetValue(false);
		if (m_PreviewPanel)
			m_PreviewPanel->SetPerformanceOverlayEnabled(false);
		EndExposureProgress();
		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
		wxLogError("Could not start the live capturing thread.");
		return;
	}
}

auto cMain::LiveCapturingThread(wxThreadEvent& evt) -> void
{
	auto curr_code = evt.GetInt();

	// -1 == Camera is disconnected
	if (curr_code == -1)
	{
		if (!IsEventFromActiveCapture(evt))
			return; // late event from an earlier capture
		m_StartStopLiveCapturingTglBtn->SetValue(false);
		wxCommandEvent live_capturing_evt(wxEVT_TOGGLEBUTTON, MainFrameVariables::ID::RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN);
		ProcessEvent(live_capturing_evt);

		if (m_PreviewPanel)
			m_PreviewPanel->SetPerformanceOverlayEnabled(false);

		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);

		EndExposureProgress();

		if (!evt.GetString().IsEmpty())
			wxMessageBox(evt.GetString(), "Live capturing", wxOK | wxICON_ERROR, this);

		return;
	}

	const auto spectrum = evt.GetPayload<MainFrameVariables::SpectrumPtr>();
	if (!spectrum || spectrum->empty() || !m_PreviewPanel)
		return;

	unsigned long long sum{};
	sum = std::accumulate(spectrum->begin(), spectrum->end(), sum);

	const bool preserveUserXState = m_PreviewPanel->HasCustomizedXDomain();

	m_PreviewPanel->NotifyNewFrame(static_cast<unsigned long long>(curr_code) + 1ULL);
	m_PreviewPanel->SetKETEKData(spectrum->data(), static_cast<unsigned long>(spectrum->size()), sum);

	UpdateDeviceParameters();

	if (!preserveUserXState)
		UpdateDesiredEnergyRangeControlsToFullData();
}

auto cMain::WorkerThreadEvent(wxThreadEvent& evt) -> void
{
	auto curr_code = evt.GetInt();
	auto filePath = evt.GetString();

	const auto finishedCode =
		static_cast<int>
		(
			MainFrameVariables::MeasurementThreadResult::Finished
			);

	const auto motorErrorCode =
		static_cast<int>
		(
			MainFrameVariables::MeasurementThreadResult::MotorPositionError
			);

	const auto completedCode =
		static_cast<int>
		(
			MainFrameVariables::MeasurementThreadResult::CompletedWithSummary
			);

	if (curr_code == finishedCode || curr_code == motorErrorCode || curr_code == completedCode)
	{
		if (!IsEventFromActiveCapture(evt))
			return; // late event from an earlier capture

		if (
			m_CaptureUiMode ==
			MainFrameVariables::CaptureUiMode::MeasurementRunning
			)
		{
			m_StartStopMeasurementTglBtn->SetValue(false);

			wxCommandEvent measurement_capturing_evt
			(
				wxEVT_TOGGLEBUTTON,
				MainFrameVariables::ID::
				RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN
			);

			ProcessEvent(measurement_capturing_evt);
		}

		if (m_PreviewPanel)
			m_PreviewPanel->SetPerformanceOverlayEnabled(false);

		ApplyCaptureUiState
		(
			MainFrameVariables::CaptureUiMode::Idle
		);

		EndExposureProgress();

		if (curr_code == motorErrorCode)
		{
			UpdateStagePositions();

			wxMessageBox
			(
				filePath,
				"Motor movement error",
				wxOK | wxICON_ERROR,
				this
			);
		}
		else if (curr_code == finishedCode && !filePath.IsEmpty())
		{
			// For terminal codes the string carries an error message.
			wxMessageBox
			(
				filePath,
				"Capture error",
				wxOK | wxICON_ERROR,
				this
			);
		}
		else if (curr_code == completedCode)
		{
			UpdateStagePositions();
			OfferMoveToBestPosition
			(
				evt.GetPayload<MainFrameVariables::MeasurementSummary>()
			);
		}

		return;
	}

	const auto spectrum = evt.GetPayload<MainFrameVariables::SpectrumPtr>();
	if (!spectrum || spectrum->empty() || !m_PreviewPanel)
		return;

	unsigned long long sum{};
	sum = std::accumulate(spectrum->begin(), spectrum->end(), sum);

	const bool preserveUserXState = m_PreviewPanel->HasCustomizedXDomain();

	m_PreviewPanel->NotifyNewFrame(static_cast<unsigned long long>(curr_code) + 1ULL);
	m_PreviewPanel->SetKETEKData(spectrum->data(), static_cast<unsigned long>(spectrum->size()), sum);

	UpdateDeviceParameters();

	if (!preserveUserXState)
		UpdateDesiredEnergyRangeControlsToFullData();

	// Save the PNG right next to the MCA (no path if the MCA wasn't saved)
	if (!filePath.IsEmpty())
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

	UpdateStagePositions();
}

void cMain::UpdateProgress(wxThreadEvent& evt)
{
	const int progress = evt.GetInt();
	const wxString msg = evt.GetString();

	const auto formatDuration = [](long long seconds)
		{
			seconds = std::max(0LL, seconds);
			return seconds >= 3600
				? wxString::Format("%lld:%02lld:%02lld", seconds / 3600, (seconds / 60) % 60, seconds % 60)
				: wxString::Format("%02lld:%02lld", seconds / 60, seconds % 60);
		};

	if (progress != -1)
	{
		const long long elapsed = std::chrono::duration_cast<std::chrono::seconds>
			(std::chrono::steady_clock::now() - m_StartCalculationTime).count();

		const wxString dot(L" \u00B7 ");
		wxString text = "Measuring: " + msg + dot + formatDuration(elapsed) + " elapsed";

		if (progress > 0 && progress < 100)
			text += dot + "~" + formatDuration(elapsed * (100 - progress) / progress) + " left";

		m_ActivityText = text;
		UpdateStatusFields();

		if (m_AppProgressIndicator)
			m_AppProgressIndicator->SetValue(progress);
	}
	else
	{
		// reset(), not an explicit destructor call: the unique_ptr would
		// otherwise destroy the same object a second time later.
		m_AppProgressIndicator.reset();
		m_ActivityText.clear();
		UpdateStatusFields();
		UpdateAllAxisGlobalPositions();
		this->Enable();
	}
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
		m_StartStopLiveCapturingTglBtn->Disable();
		StopActiveCapture();
		m_StartStopLiveCapturingTglBtn->Enable();
		AppLog::Info("Capture", "Live capturing stopped.");

		if (m_PreviewPanel)
			m_PreviewPanel->SetPerformanceOverlayEnabled(false);

		EndExposureProgress();
		ApplyCaptureUiState(MainFrameVariables::CaptureUiMode::Idle);
	}
}


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
		m_HasDetectedMotorAxes &&
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

	if (measurementRunning)
	{
		for (int i = 0; i < 2; ++i)
			m_Detector[i].EnableAllControls(false);

		for (int i = 0; i < 5; ++i)
			m_Optics[i].EnableAllControls(false);

		m_Aux[0].EnableAllControls(false);

		if (m_FirstStage)
			m_FirstStage->EnableAllControls(
				outputDirValid &&
				m_HasDetectedMotorAxes
			);

#ifdef USE_2_AXIS_MEASUREMENT
		if (m_SecondStage)
			m_SecondStage->EnableAllControls(
				outputDirValid &&
				m_HasDetectedMotorAxes
			);
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

	if (!measurementRunning)
		m_ActivityText.clear();

	UpdateStatusFields();

	if (m_RightSidePanel)
	{
		m_RightSidePanel->Layout();
		m_RightSidePanel->FitInside();
	}

	Layout();
	Refresh();
}

auto cMain::ActivityTextForMode() const -> wxString
{
	if (!m_ActivityText.IsEmpty())
		return m_ActivityText;

	switch (m_CaptureUiMode)
	{
	case MainFrameVariables::CaptureUiMode::SingleShotRunning:  return "Single shot running";
	case MainFrameVariables::CaptureUiMode::LiveRunning:        return "Live capturing";
	case MainFrameVariables::CaptureUiMode::MeasurementRunning: return "Measuring";
	default: break;
	}

	return m_MotorMoveInProgress ? "Moving motor..." : "Idle";
}

void cMain::UpdateStatusFields()
{
	if (!m_StatusBar || m_StatusBar->GetFieldsCount() < 4)
		return;

	m_StatusBar->SetStatusText(ActivityTextForMode(), 1);

	const bool detectorReady = m_KetekHandler && m_KetekHandler->IsDeviceInitialized();
	m_StatusBar->SetStatusText
	(
		detectorReady
		? "Detector: " + wxString::FromUTF8(m_KetekHandler->GetSerialNumber())
		: wxString("Detector: not connected"),
		2
	);

	m_StatusBar->SetStatusText(m_Settings ? m_Settings->GetMotorStatusSummary() : wxString("Motors: -"), 3);
}
