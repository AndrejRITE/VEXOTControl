// Measurement thread: moves the stage, captures and saves each step, and
// writes the summary graphs and files.
#include "cMain.h"
/* ___ End Live Capturing Thread ___ */

/* ___ Start Worker Thread ___ */
wxThread::ExitCode WorkerThread::Entry()
{
	MainFrameVariables::ThreadFinishedGuard finishedGuard{ m_Control };

	const auto postFailure = [this](const wxString& message)
		{
			AppLog::Error("Measurement", message.utf8_string());
			m_Control->continueRunning = false;

			if (m_Settings)
				m_Settings->StopAllMotors();

			wxThreadEvent failEvt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_MAIN_CAPTURING);
			failEvt.SetInt(static_cast<int>(MainFrameVariables::MeasurementThreadResult::Finished));
			failEvt.SetString(message);
			failEvt.SetExtraLong(m_Control->generation);
			wxQueueEvent(m_MainFrame, failEvt.Clone());
		};

	try
	{
		RunMeasurement();
	}
	catch (const std::exception& e)
	{
		postFailure(wxString::Format("The measurement stopped because of an error: %s", e.what()));
	}
	catch (...)
	{
		postFailure("The measurement stopped because of an unknown error.");
	}

	return (wxThread::ExitCode)0;
}

auto WorkerThread::RunMeasurement() -> void
{
	m_Settings->SetCurrentProgress(0, m_FirstAxis->step_number);

	// wxDateTime is thread-safe; std::ctime uses a shared static buffer.
	const auto startTime = wxDateTime::Now();
	m_RunDate = startTime.Format("%Y-%m-%d").ToStdString();
	const auto cur_hours = startTime.Format("%H").ToStdString();
	const auto cur_mins = startTime.Format("%M").ToStdString();
	const auto cur_secs = startTime.Format("%S").ToStdString();

	auto createFileName = [&](const wxString& suffix = "") -> wxString
		{
			wxString fileName = m_DataPath + wxString("\\") + suffix + wxString("_") + wxString(m_RunDate) + wxString("_") + cur_hours + wxString("H_") + cur_mins + wxString("M_") + cur_secs + wxString("S");
			return fileName;
		};

	auto graphFileName = createFileName("measurement");
	auto temperatureGraphFileName = createFileName("temperature");

	m_MeasurementGraphFilePath = graphFileName + wxString(".bmp");
	m_MeasurementTemperatureGraphFilePath = temperatureGraphFileName + wxString(".bmp");
	m_MeasurementGraphTxtFilePath = graphFileName + wxString(".txt");
	m_MeasurementTimingFilePath = createFileName("measurement_timing") + wxString(".csv");

	m_TimingLogStart = std::chrono::steady_clock::now();
	OpenTimingLog();
	LogTimingEvent(0, "measurement_started", 0.0,
		std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::quiet_NaN(), "ok");

	const auto dataSize = m_KetekHandler->GetDataSize();

	m_AllMaxElementsDuringCapturing = std::make_unique<unsigned long[]>(m_FirstAxis->step_number);
	m_AllSumsDuringCapturing = std::make_unique<unsigned long long[]>(m_FirstAxis->step_number);

	m_AllBoardTemperaturesDuringCapturing = std::make_unique<double[]>(m_FirstAxis->step_number);
	m_AllSDDTemperaturesDuringCapturing = std::make_unique<double[]>(m_FirstAxis->step_number);
	m_AllHotSideTemperaturesDuringCapturing = std::make_unique<double[]>(m_FirstAxis->step_number);
	m_AllThermistor1TemperaturesDuringCapturing = std::make_unique<double[]>(m_FirstAxis->step_number);
	m_AllThermistor2TemperaturesDuringCapturing = std::make_unique<double[]>(m_FirstAxis->step_number);

	float first_axis_rounded_go_to{};
	float first_axis_position{}, second_axis_position{};
	auto positionsArray = std::make_unique<float[]>(m_FirstAxis->step_number);
	unsigned int capturedCount{};

	const auto saveAvailableResults = [&]()
		{
			if (capturedCount == 0)
				return;

			const wxString timestamp =
				wxDateTime::Now().Format(wxT("%d-%m-%Y, %H:%M:%S"));

			SaveMeasurementResults
			(
				positionsArray.get(),
				capturedCount,
				timestamp
			);
		};

	const auto finishMeasurement =
		[&]
		(
			const MainFrameVariables::MeasurementThreadResult result,
			const wxString& message = wxString(),
			const MainFrameVariables::MeasurementSummary* summary = nullptr
			)
		{
			m_Control->continueRunning = false;

			if (message.IsEmpty())
				AppLog::Info("Measurement", "Measurement finished after " + std::to_string(capturedCount) + " step(s).");
			else
				AppLog::Error("Measurement", message.utf8_string());

			wxThreadEvent doneEvt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_MAIN_CAPTURING);
			doneEvt.SetInt(static_cast<int>(result));
			doneEvt.SetString(message);
			doneEvt.SetExtraLong(m_Control->generation);
			if (summary)
				doneEvt.SetPayload(*summary);

			wxQueueEvent(m_MainFrame, doneEvt.Clone());
		};

	for (auto i{ 0 }; i < m_FirstAxis->step_number; ++i)
	{
		if (!m_Control->continueRunning)
		{
			LogTimingEvent(static_cast<int>(capturedCount), "measurement_finished", 0.0,
				std::numeric_limits<float>::quiet_NaN(),
				std::numeric_limits<float>::quiet_NaN(), "cancelled");
			saveAvailableResults();
			finishMeasurement(MainFrameVariables::MeasurementThreadResult::Finished);
			return;
		}
		m_Settings->SetCurrentProgress(i, m_FirstAxis->step_number);

		const auto start_um = std::llround(static_cast<double>(m_FirstAxis->start) * 1000.0);
		const auto step_um = std::llround(static_cast<double>(m_FirstAxis->step) * 1000.0);

		if (step_um == 0)
		{
			LogTimingEvent(i + 1, "measurement_finished", 0.0,
				std::numeric_limits<float>::quiet_NaN(),
				std::numeric_limits<float>::quiet_NaN(), "invalid_step");
			saveAvailableResults();
			finishMeasurement
			(
				MainFrameVariables::MeasurementThreadResult::MotorPositionError,
				"Measurement step is too small after conversion to microns."
			);
			return;
		}

		const auto target_um = start_um + static_cast<long long>(i) * step_um;
		first_axis_rounded_go_to = static_cast<float>(target_um / 1000.0);

		// The UI accepts three decimal places. A small positioning difference is
		// therefore permitted. A failed read-back is retried before the scan is
		// stopped because some stages do not reach the target on the first command.
		const float positionTolerance = std::max
		(
			0.005f,
			std::min
			(
				std::abs(m_FirstAxis->step) * 0.25f,
				0.05f
			)
		);

		constexpr int maxPositioningAttempts = 10;
		bool positionReached = false;
		int attemptsMade = 0;

		for (int attempt = 1; attempt <= maxPositioningAttempts; ++attempt)
		{
			if (!m_Control->continueRunning)
				break;

			const std::string attemptPrefix =
				"attempt_" + std::to_string(attempt);

			LogTimingEvent
			(
				i + 1,
				"stage_move_started",
				0.0,
				first_axis_rounded_go_to,
				std::numeric_limits<float>::quiet_NaN(),
				attemptPrefix + "_started"
			);

			const auto moveStarted = std::chrono::steady_clock::now();
			first_axis_position = MoveFirstStage(first_axis_rounded_go_to);
			const auto moveFinished = std::chrono::steady_clock::now();

			const double moveDurationMs =
				std::chrono::duration<double, std::milli>
				(
					moveFinished - moveStarted
				).count();

			attemptsMade = attempt;
			positionReached =
				std::isfinite(first_axis_position)
				&&
				std::abs
				(
					first_axis_position -
					first_axis_rounded_go_to
				) <= positionTolerance;

			const std::string attemptResult = positionReached
				? attemptPrefix + "_ok"
				: (attempt < maxPositioningAttempts
					? attemptPrefix + "_retry"
					: attemptPrefix + "_failed");

			LogTimingEvent
			(
				i + 1,
				"stage_move_finished",
				moveDurationMs,
				first_axis_rounded_go_to,
				first_axis_position,
				attemptResult
			);

			if (positionReached)
				break;

			if (attempt < maxPositioningAttempts && m_Control->continueRunning)
				wxThread::Sleep(100);
		}

		positionsArray[i] = first_axis_position;

		if (!m_Control->continueRunning)
		{
			LogTimingEvent(i + 1, "measurement_finished", 0.0,
				first_axis_rounded_go_to, first_axis_position, "cancelled");
			saveAvailableResults();
			finishMeasurement(MainFrameVariables::MeasurementThreadResult::Finished);
			return;
		}

		if (!positionReached)
		{
			m_Settings->StopAllMotors();

			const wxString actualPosition =
				std::isfinite(first_axis_position)
				? wxString::Format("%.3f", first_axis_position)
				: wxString("not available");

			const auto axisNameUtf8 =
				AxisNameToString(m_FirstAxis->axis_number);

			const wxString axisName =
				wxString::FromUTF8(axisNameUtf8.c_str());

			LogTimingEvent(i + 1, "measurement_finished", 0.0,
				first_axis_rounded_go_to, first_axis_position, "stage_error");
			saveAvailableResults();

			finishMeasurement
			(
				MainFrameVariables::
				MeasurementThreadResult::MotorPositionError,

				wxString::Format
				(
					"Measurement was stopped because motor %s "
					"did not reach the requested position after %d attempts.\n\n"
					"Requested position: %.3f\n"
					"Actual position: %s\n"
					"Allowed difference: %.3f\n\n"
					"Check the stage, controller connection, "
					"and motor state before restarting the measurement.",

					axisName,
					attemptsMade,
					first_axis_rounded_go_to,
					actualPosition,
					positionTolerance
				)
			);

			return;
		}

		wxString filePath{};

		// A fresh buffer per step: the UI thread keeps its own reference to
		// the previous spectrum, so nothing has to wait for the UI.
		auto mcaData = std::make_shared<std::vector<unsigned long>>(dataSize, 0UL);

		if (!CaptureAndSaveData
		(
			mcaData->data(),
			i + 1,
			first_axis_position,
			second_axis_position,
			cur_hours, cur_mins, cur_secs,
			&filePath
		))
		{
			LogTimingEvent(i + 1, "measurement_finished", 0.0,
				first_axis_rounded_go_to, first_axis_position,
				m_Control->continueRunning ? "capture_error" : "cancelled");
			saveAvailableResults();
			finishMeasurement
			(
				MainFrameVariables::MeasurementThreadResult::Finished,
				(m_Control->continueRunning || !m_LastSaveError.IsEmpty())
				? wxString::Format
				(
					"The measurement stopped at step %d because the detector capture "
					"or saving the data failed:\n%s",
					i + 1,
					m_LastSaveError.IsEmpty()
					? wxString::FromUTF8(m_KetekHandler->GetLastError())
					: m_LastSaveError
				)
				: wxString()
			);

			return;
		}

		capturedCount = static_cast<unsigned int>(i + 1);

		wxThreadEvent frameEvt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_MAIN_CAPTURING);
		frameEvt.SetInt(i);
		frameEvt.SetPayload<MainFrameVariables::SpectrumPtr>(mcaData);
		frameEvt.SetString(filePath);

		wxQueueEvent(m_MainFrame, frameEvt.Clone());
		LogTimingEvent(i + 1, "ui_update_queued", 0.0,
			first_axis_rounded_go_to, first_axis_position, "ok");

		/* Update Current Progress */
		m_Settings->SetCurrentProgress(i, m_FirstAxis->step_number);
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

	LogTimingEvent(static_cast<int>(capturedCount), "measurement_finished", 0.0,
		std::numeric_limits<float>::quiet_NaN(),
		std::numeric_limits<float>::quiet_NaN(), "completed");
	saveAvailableResults();

	// The UI thread offers to move to the best position; message boxes and
	// widget updates must not happen on this thread.
	MainFrameVariables::MeasurementSummary summary{};
	summary.maxSum = m_MaxSumDuringCapturing;
	summary.bestPosition = m_BestFirstAxisPosition;
	summary.bestMeasurementNumber = m_BestMeasurementNumber;
	summary.axisNumber = m_FirstAxis->axis_number;

	finishMeasurement
	(
		MainFrameVariables::MeasurementThreadResult::CompletedWithSummary,
		wxString(),
		&summary
	);
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

	if (exposureThread->Run() != wxTHREAD_NO_ERROR)
		delete exposureThread;

	LogTimingEvent(image_number, "capture_started", 0.0,
		first_stage_position, first_stage_position, "started");
	const auto captureStarted = std::chrono::steady_clock::now();
	const bool captureOk = m_KetekHandler->CaptureData(m_ExposureTimeSeconds, mca, &m_Control->continueRunning);
	const auto captureFinished = std::chrono::steady_clock::now();
	const double captureDurationMs = std::chrono::duration<double, std::milli>
		(captureFinished - captureStarted).count();
	LogTimingEvent(image_number, "capture_finished", captureDurationMs,
		first_stage_position, first_stage_position,
		captureOk ? "ok" : (m_Control->continueRunning ? "failed" : "cancelled"));

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

	if (!wxDir::Exists(m_DataPath))
	{
		m_LastSaveError = wxString::Format("The output folder no longer exists:\n%s", m_DataPath);
		return false;
	}

	/* Save Data */
	{
		LogTimingEvent(image_number, "data_save_started", 0.0,
			first_stage_position, first_stage_position, "started");
		const auto saveStarted = std::chrono::steady_clock::now();
		std::string first_axis_position_str = std::format("{:.3f}", first_stage_position);
		std::replace(first_axis_position_str.begin(), first_axis_position_str.end(), '.', '_');

		std::string second_axis_position_str = std::format("{:.3f}", second_stage_position);
		std::replace(second_axis_position_str.begin(), second_axis_position_str.end(), '.', '_');

		std::string file_name = std::string(m_DataPath.mb_str()) + std::string("\\") +
			std::string("ktk_");
		file_name += image_number < 10 ? std::string("0") : std::string("");
		file_name += std::to_string(image_number) + std::string("_") +
			m_RunDate + std::string("_") +
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

		unsigned long maxElement{};
		wxString writeError{};
		if (!MainFrameVariables::WriteMCAFile
		(
			file_name,
			mca,
			m_KetekHandler,
			sum,
			&maxElement,
			&writeError
		))
		{
			m_LastSaveError = wxString::Format("Could not save %s:\n%s", wxString(file_name), writeError);
			LogTimingEvent(image_number, "data_save_finished", 0.0,
				first_stage_position, first_stage_position, "failed");
			return false;
		}

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

		m_AllBoardTemperaturesDuringCapturing[image_number - 1] =
			static_cast<double>(m_KetekHandler->GetBoardTemperature());

		m_AllSDDTemperaturesDuringCapturing[image_number - 1] =
			static_cast<double>(m_KetekHandler->GetSDDTemperature());

		m_AllHotSideTemperaturesDuringCapturing[image_number - 1] =
			static_cast<double>(m_KetekHandler->GetHotSideTemperature());

		m_AllThermistor1TemperaturesDuringCapturing[image_number - 1] =
			static_cast<double>(m_KetekHandler->GetThermistor1Temperature());

		m_AllThermistor2TemperaturesDuringCapturing[image_number - 1] =
			static_cast<double>(m_KetekHandler->GetThermistor2Temperature());

		const auto saveFinished = std::chrono::steady_clock::now();
		const double saveDurationMs = std::chrono::duration<double, std::milli>
			(saveFinished - saveStarted).count();
		LogTimingEvent(image_number, "data_save_finished", saveDurationMs,
			first_stage_position, first_stage_position, "ok");

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

	if (dataSize == 0 || !countData || !sumData || !positionsData)
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
	const int bestMarkerRadius = 8;

	const int polylineThickness = 6;
	const int markerRadius = std::ceil(static_cast<double>(polylineThickness) * 0.8);

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
				return graphRect.GetLeft() + graphRect.GetWidth() / 2;

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
			dc.SetPen(wxPen(color, polylineThickness));
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
			dc.SetFont(tickFont);
			dc.SetTextForeground(axisColor);

			const int legendPaddingX = 10;
			const int legendPaddingY = 8;
			const int swatchWidth = 18;
			const int swatchGap = 8;
			const int itemGapX = 22;
			const int rowHeight = 24;

			int x = graphRect.GetLeft() + 12;
			int y = graphRect.GetTop() + 10;
			const int maxX = graphRect.GetRight() - 12;

			for (auto i = 0; i < 2; ++i)
			{
				auto label = (i == 0) ? leftYAxisLabel : rightYAxisLabel;

				const wxSize labelSize = dc.GetTextExtent(label);
				const int itemWidth = legendPaddingX * 2 + swatchWidth + swatchGap + labelSize.GetWidth();

				if (x + itemWidth > maxX)
				{
					x = graphRect.GetLeft() + 12;
					y += rowHeight;
				}

				dc.SetPen(wxPen(borderColor, 1));
				dc.SetBrush(wxBrush(wxColour(255, 255, 255, 220)));
				dc.DrawRoundedRectangle(x, y - 2, itemWidth, labelSize.GetHeight() + 4, 4);

				dc.SetPen(wxPen((i == 0) ? countColor : sumColor, 3));
				dc.DrawLine(x + legendPaddingX, y + 8, x + legendPaddingX + swatchWidth, y + 8);

				dc.SetTextForeground(axisColor);
				dc.DrawText(label, x + legendPaddingX + swatchWidth + swatchGap, y - 1);
				x += itemWidth + itemGapX;
			}
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

wxBitmap WorkerThread::CreateTemperatureGraph
(
	const double* const boardTemperatureData,
	const double* const sddTemperatureData,
	const double* const hotSideTemperatureData,
	const double* const thermistor1TemperatureData,
	const double* const thermistor2TemperatureData,
	const float* const positionsData,
	unsigned int dataSize,
	int width,
	int height,
	const wxString& xAxisLabel,
	const wxString& yAxisLabel,
	const wxString& timestamp
)
{
	wxBitmap bitmap(width, height);
	wxMemoryDC dc(bitmap);

	const wxColour backgroundColor(250, 250, 252);
	dc.SetBackground(wxBrush(backgroundColor));
	dc.Clear();

	if (dataSize == 0 || !positionsData)
	{
		dc.SelectObject(wxNullBitmap);
		return bitmap;
	}

	const wxColour plotBackgroundColor(255, 255, 255);
	const wxColour borderColor(210, 214, 220);
	const wxColour gridColor(220, 224, 230);
	const wxColour axisColor(35, 35, 35, 80);
	const wxColour tickTextColor(70, 70, 70);

	const wxColour boardColor(230, 57, 70);
	const wxColour sddColor(30, 136, 229);
	const wxColour hotSideColor(67, 160, 71);
	const wxColour therm1Color(255, 143, 0);
	const wxColour therm2Color(123, 31, 162);

	const wxColour exposureColor(0, 102, 204);
	const wxColour timestampColor(156, 39, 176);

	const int marginLeft = 130;
	const int marginRight = 80;
	const int marginTop = 60;
	const int marginBottom = 190;

	const int xAxisTitleOffset = 120;
	const int yAxisTitleOffset = 120;
	const int xTickOffset = 10;
	const int posTickOffset = 52;
	const int footerY = height - 52;
	const int tickLength = 6;

	const int polylineThickness = 6;
	const int markerRadius = std::ceil(static_cast<double>(polylineThickness) * 0.8);

	const wxRect graphRect
	(
		marginLeft,
		marginTop,
		std::max(100, width - marginLeft - marginRight),
		std::max(100, height - marginTop - marginBottom)
	);

	wxFont titleFont(std::max(14, m_GraphFontSize + 8), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
	wxFont axisTitleFont(std::max(15, m_GraphFontSize + 10), wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
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

	auto clampToGraphY = [](int y, const wxRect& rect) -> int
		{
			return std::clamp(y, rect.GetTop(), rect.GetBottom());
		};

	auto trimNumber = [](double value, int decimals = 2) -> wxString
		{
			std::ostringstream oss;
			oss.setf(std::ios::fixed);
			oss << std::setprecision(decimals) << value;

			std::string s = oss.str();

			if (decimals > 0)
				while (!s.empty() && s.back() == '0')
					s.pop_back();

			if (!s.empty() && s.back() == '.')
				s.pop_back();

			if (s.empty() || s == "-0")
				s = "0";

			return wxString(s);
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
	const wxColour fadedTempAxisTitleColor = blendColor(boardColor, backgroundColor, 0.60);
	const wxColour fadedPositionTickColor = blendColor(tickTextColor, backgroundColor, 0.50);

	dc.SetPen(wxPen(borderColor, 1));
	dc.SetBrush(wxBrush(plotBackgroundColor));
	dc.DrawRectangle(graphRect);

	{
		const wxString graphTitle = "Temperatures";
		dc.SetFont(titleFont);
		dc.SetTextForeground(axisColor);
		drawCenteredText(graphTitle, width / 2, 14);
	}

	{
		wxString axisName = wxString(AxisNameToString(m_FirstAxis->axis_number));

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

	struct TemperatureSeries
	{
		const double* data{};
		wxString label;
		wxColour color;
		bool enabled{ false };
	};

	std::vector<TemperatureSeries> series =
	{
		{ boardTemperatureData, "Board",        boardColor, boardTemperatureData != nullptr },
		{ sddTemperatureData,   "SDD",          sddColor,   sddTemperatureData != nullptr },
		{ hotSideTemperatureData, "Hot Side",   hotSideColor, hotSideTemperatureData != nullptr },
		{ thermistor1TemperatureData, "Thermistor 1", therm1Color, thermistor1TemperatureData != nullptr },
		{ thermistor2TemperatureData, "Thermistor 2", therm2Color, thermistor2TemperatureData != nullptr }
	};

	std::vector<TemperatureSeries> activeSeries;
	for (const auto& s : series)
	{
		if (s.enabled && s.data)
			activeSeries.push_back(s);
	}

	if (activeSeries.empty())
	{
		dc.SelectObject(wxNullBitmap);
		return bitmap;
	}

	double minTempValue = activeSeries.front().data[0];
	double maxTempValue = activeSeries.front().data[0];

	for (const auto& s : activeSeries)
	{
		for (size_t i = 0; i < dataSize; ++i)
		{
			minTempValue = std::min(minTempValue, s.data[i]);
			maxTempValue = std::max(maxTempValue, s.data[i]);
		}
	}

	if (minTempValue == maxTempValue)
	{
		minTempValue -= 5.0;
		maxTempValue += 5.0;
	}

	auto chooseNiceTemperatureStep = [](double range) -> double
		{
			const std::vector<double> candidates =
			{
				5.0, 10.0, 15.0, 20.0, 25.0, 30.0, 40.0, 50.0, 75.0, 100.0
			};

			const double rawStep = std::max(5.0, range / 6.0);

			for (double step : candidates)
			{
				if (step >= rawStep)
					return step;
			}

			return 100.0;
		};

	const double rawRange = maxTempValue - minTempValue;
	const double yStep = chooseNiceTemperatureStep(rawRange);

	const double yAxisMin = std::floor(minTempValue / yStep) * yStep;
	const double yAxisMax = std::ceil(maxTempValue / yStep) * yStep;
	const double yAxisRange = std::max(yStep, yAxisMax - yAxisMin);

	auto mapX = [&](size_t index) -> int
		{
			if (dataSize <= 1)
				return graphRect.GetLeft() + graphRect.GetWidth() / 2;

			const double t = static_cast<double>(index) / static_cast<double>(dataSize - 1);
			return graphRect.GetLeft() + static_cast<int>(std::lround(t * graphRect.GetWidth()));
		};

	auto mapY = [&](double value) -> int
		{
			const double t = (value - yAxisMin) / yAxisRange;
			const int y = graphRect.GetBottom() - static_cast<int>(std::lround(t * graphRect.GetHeight()));
			return clampToGraphY(y, graphRect);
		};

	auto drawHorizontalRulersAndGrid = [&]()
		{
			dc.SetFont(tickFont);

			for (double tickValue = yAxisMin; tickValue <= yAxisMax + 0.001; tickValue += yStep)
			{
				const int y = mapY(tickValue);
				const bool isBorder = (std::abs(tickValue - yAxisMin) < 0.001) || (std::abs(tickValue - yAxisMax) < 0.001);

				if (!isBorder)
				{
					dc.SetPen(wxPen(gridColor, 1, wxPENSTYLE_DOT));
					dc.DrawLine(graphRect.GetLeft(), y, graphRect.GetRight(), y);
				}

				const wxString leftText = trimNumber(tickValue, 0);
				const wxSize leftSize = dc.GetTextExtent(leftText);

				dc.SetPen(wxPen(boardColor, 1));
				dc.SetTextForeground(boardColor);
				dc.DrawLine(graphRect.GetLeft() - tickLength, y, graphRect.GetLeft(), y);
				dc.DrawText(leftText, graphRect.GetLeft() - tickLength - 6 - leftSize.GetWidth(), y - leftSize.GetHeight() / 2);
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
		};

	auto drawAxisTitles = [&]()
		{
			dc.SetFont(axisTitleFont);

			dc.SetTextForeground(fadedAxisTitleColor);
			drawCenteredText(xAxisLabel, graphRect.GetLeft() + graphRect.GetWidth() / 2, graphRect.GetBottom() + xAxisTitleOffset);

			dc.SetTextForeground(fadedTempAxisTitleColor);
			const wxSize leftAxisSize = dc.GetTextExtent(yAxisLabel);
			dc.DrawRotatedText
			(
				yAxisLabel,
				marginLeft - yAxisTitleOffset,
				graphRect.GetTop() + graphRect.GetHeight() / 2 + leftAxisSize.GetWidth() / 2,
				90
			);
		};

	auto buildPolylinePoints = [&](const double* data) -> std::vector<wxPoint>
		{
			std::vector<wxPoint> points;
			points.reserve(dataSize);

			for (size_t i = 0; i < dataSize; ++i)
				points.emplace_back(mapX(i), mapY(data[i]));

			return points;
		};

	auto drawPolyline = [&](const std::vector<wxPoint>& points, const wxColour& color)
		{
			if (points.size() < 2)
				return;

			dc.SetPen(wxPen(color, polylineThickness));
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

	auto drawLegend = [&]()
		{
			dc.SetFont(tickFont);
			dc.SetTextForeground(axisColor);

			const int legendPaddingX = 10;
			const int legendPaddingY = 8;
			const int swatchWidth = 18;
			const int swatchGap = 8;
			const int itemGapX = 22;
			const int rowHeight = 24;

			int x = graphRect.GetLeft() + 12;
			int y = graphRect.GetTop() + 10;
			const int maxX = graphRect.GetRight() - 12;

			for (const auto& s : activeSeries)
			{
				const wxSize labelSize = dc.GetTextExtent(s.label);
				const int itemWidth = legendPaddingX * 2 + swatchWidth + swatchGap + labelSize.GetWidth();

				if (x + itemWidth > maxX)
				{
					x = graphRect.GetLeft() + 12;
					y += rowHeight;
				}

				dc.SetPen(wxPen(borderColor, 1));
				dc.SetBrush(wxBrush(wxColour(255, 255, 255, 220)));
				dc.DrawRoundedRectangle(x, y - 2, itemWidth, labelSize.GetHeight() + 4, 4);

				dc.SetPen(wxPen(s.color, 3));
				dc.DrawLine(x + legendPaddingX, y + 8, x + legendPaddingX + swatchWidth, y + 8);

				dc.SetTextForeground(axisColor);
				dc.DrawText(s.label, x + legendPaddingX + swatchWidth + swatchGap, y - 1);

				x += itemWidth + itemGapX;
			}
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

	for (const auto& s : activeSeries)
	{
		auto points = buildPolylinePoints(s.data);
		drawPolyline(points, s.color);
		drawMarkers(points, s.color);
	}

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
			AppLog::Error("Data", "Failed to save bitmap " + filePath.utf8_string());
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
	const float* const positionsData,
	const unsigned int dataSize,
	const wxString timestamp
) -> void
{
	if (!countData || !sumData || !positionsData)
		return;

	std::ofstream outFile;

	outFile.open
	(
		m_MeasurementGraphTxtFilePath.ToStdWstring()
	);

	if (!outFile)
	{
		wxLogError("Could not create the measurement summary '%s'.", m_MeasurementGraphTxtFilePath);
		AppLog::Error("Data", "Could not create " + m_MeasurementGraphTxtFilePath.utf8_string());
		return;
	}

	outFile << timestamp << std::endl;

	outFile
		<< "Measurement Number" << '\t'
		<< "Actual Stage Position" << '\t'
		<< "Max Value" << '\t'
		<< "Sum Values"
		<< std::endl;

	for (auto i{ 0 }; i < static_cast<int>(dataSize); ++i)
	{
		outFile
			<< i + 1 << '\t'
			<< positionsData[i] << '\t'
			<< countData[i] << '\t'
			<< sumData[i]
			<< std::endl;
	}

	outFile.close();
	if (!outFile)
	{
		wxLogError("Writing the measurement summary '%s' failed.", m_MeasurementGraphTxtFilePath);
		AppLog::Error("Data", "Writing " + m_MeasurementGraphTxtFilePath.utf8_string() + " failed.");
	}
}

auto WorkerThread::SaveMeasurementResults
(
	const float* positionsData,
	const unsigned int capturedCount,
	const wxString& timestamp
) -> void
{
	if (!positionsData || capturedCount == 0)
		return;

	auto measurementBitmap = CreateGraph
	(
		m_AllMaxElementsDuringCapturing.get(),
		m_AllSumsDuringCapturing.get(),
		positionsData,
		capturedCount,
		1920, 1160,
		"Measurement Number",
		"Max Value",
		"Sum Values",
		timestamp
	);

	SaveGraph(measurementBitmap, m_MeasurementGraphFilePath);

	auto temperatureBitmap = CreateTemperatureGraph
	(
		m_AllBoardTemperaturesDuringCapturing.get(),
		m_AllSDDTemperaturesDuringCapturing.get(),
		m_AllHotSideTemperaturesDuringCapturing.get(),
		nullptr,
		nullptr,
		positionsData,
		capturedCount,
		1920, 960,
		"Measurement Number",
		"Temperature [degC]",
		timestamp
	);

	SaveGraph(temperatureBitmap, m_MeasurementTemperatureGraphFilePath);

	SaveGraphTxt
	(
		m_AllMaxElementsDuringCapturing.get(),
		m_AllSumsDuringCapturing.get(),
		positionsData,
		capturedCount,
		timestamp
	);

	if (!wxLaunchDefaultApplication(m_MeasurementTemperatureGraphFilePath))
		wxLogError
		(
			"Could not open file '%s' with the default application.",
			m_MeasurementTemperatureGraphFilePath
		);

	if (!wxLaunchDefaultApplication(m_MeasurementGraphFilePath))
		wxLogError
		(
			"Could not open file '%s' with the default application.",
			m_MeasurementGraphFilePath
		);
}

auto WorkerThread::OpenTimingLog() -> bool
{
	if (m_TimingLog.is_open())
		m_TimingLog.close();

	m_TimingLog.open(m_MeasurementTimingFilePath.ToStdWstring(), std::ios::out | std::ios::trunc);
	if (!m_TimingLog)
	{
		wxLogError("Could not create measurement timing log '%s'.", m_MeasurementTimingFilePath);
		return false;
	}

	m_TimingLog
		<< "timestamp_local,elapsed_ms,measurement_number,event,duration_ms,"
		"target_position,actual_position,status\n";
	m_TimingLog.flush();
	return true;
}

auto WorkerThread::LogTimingEvent
(
	const int measurementNumber,
	const std::string& event,
	const double durationMilliseconds,
	const float targetPosition,
	const float actualPosition,
	const std::string& status
) -> void
{
	if (!m_TimingLog.is_open())
		return;

	const auto systemNow = std::chrono::system_clock::now();
	const auto systemNowMs = std::chrono::duration_cast<std::chrono::milliseconds>
		(systemNow.time_since_epoch());
	const auto milliseconds = systemNowMs.count() % 1000;
	const std::time_t nowTime = std::chrono::system_clock::to_time_t(systemNow);

	std::tm localTime{};
#ifdef _WIN32
	localtime_s(&localTime, &nowTime);
#else
	localtime_r(&nowTime, &localTime);
#endif

	const double elapsedMilliseconds = std::chrono::duration<double, std::milli>
		(std::chrono::steady_clock::now() - m_TimingLogStart).count();

	m_TimingLog << std::put_time(&localTime, "%Y-%m-%dT%H:%M:%S")
		<< '.' << std::setw(3) << std::setfill('0') << milliseconds
		<< std::setfill(' ')
		<< ',' << std::fixed << std::setprecision(3) << elapsedMilliseconds
		<< ',' << measurementNumber
		<< ',' << event
		<< ',' << std::fixed << std::setprecision(3) << durationMilliseconds
		<< ',';

	if (std::isfinite(targetPosition))
		m_TimingLog << std::fixed << std::setprecision(3) << targetPosition;

	m_TimingLog << ',';

	if (std::isfinite(actualPosition))
		m_TimingLog << std::fixed << std::setprecision(3) << actualPosition;

	m_TimingLog << ',' << status << '\n';
	m_TimingLog.flush();
}

auto WorkerThread::MoveFirstStage(const float position) -> float
{
	// axis_number uses the same numbering as SettingsVariables::MotorsNames.
	const int axis = m_FirstAxis->axis_number;

	if (axis < SettingsVariables::DETECTOR_X || axis > SettingsVariables::AUX_X)
		return std::numeric_limits<float>::quiet_NaN();

	m_Settings->GoToAbsPos(axis, position);

	// Ask the controller where the stage actually is. Neither the value
	// returned by the move nor the cached position is proof that the
	// requested coordinate was reached.
	return m_Settings->ReadMotorPositionFromController(axis);
}

auto WorkerThread::AxisNameToString(const int axis) -> std::string
{
	switch (axis) {
	case SettingsVariables::DETECTOR_X:   return "DETECTOR X";
	case SettingsVariables::DETECTOR_Y:   return "DETECTOR Y";
	case SettingsVariables::OPTICS_X:   return "OPTICS X";
	case SettingsVariables::OPTICS_Y:   return "OPTICS Y";
	case SettingsVariables::OPTICS_Z:   return "OPTICS Z";
	case SettingsVariables::OPTICS_PITCH:   return "OPTICS PITCH";
	case SettingsVariables::OPTICS_YAW:   return "OPTICS YAW";
	case SettingsVariables::AUX_X: return "AUX X";
	default:           return "Unknown";
	}
}
