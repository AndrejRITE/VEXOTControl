// Background threads for live capturing, measurement progress and motor moves.
#include "cMain.h"

/* ___ Start Live Capturing Thread ___ */
LiveCapturing::LiveCapturing
(
	cMain* mainFrame,
	Ketek* ketekHandler,
	MainFrameVariables::ThreadControlPtr control,
	const int exposureSeconds
)
	: m_MainFrame(mainFrame),
	m_KetekHandler(ketekHandler),
	m_Control(std::move(control)),
	m_ExposureSeconds(exposureSeconds) {
}

wxThread::ExitCode LiveCapturing::Entry()
{
	MainFrameVariables::ThreadFinishedGuard finishedGuard{ m_Control };

	const auto postStopped = [this](const wxString& message)
		{
			AppLog::Error("Capture", message.utf8_string());

			// -1 tells the UI that live capturing has ended on its own.
			wxThreadEvent stopEvt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_LIVE_CAPTURING);
			stopEvt.SetInt(-1);
			stopEvt.SetString(message);
			stopEvt.SetExtraLong(m_Control->generation);
			wxQueueEvent(m_MainFrame, stopEvt.Clone());
		};

	try
	{
		const auto dataSize = m_KetekHandler->GetDataSize();

		auto imageNumber{ 0 };
		while (m_MainFrame && m_Control->continueRunning)
		{
			// A fresh buffer per frame: the UI thread keeps its own reference
			// to the previous one, so the two never share memory.
			auto mcaData = std::make_shared<std::vector<unsigned long>>(dataSize, 0UL);

			auto exposureProgressRunning = std::make_shared<std::atomic_bool>(true);

			ExposureProgressThread* exposureThread = new ExposureProgressThread
			(
				m_MainFrame,
				exposureProgressRunning,
				m_ExposureSeconds,
				"Live Exposure"
			);

			if (exposureThread->Run() != wxTHREAD_NO_ERROR)
				delete exposureThread;

			if (!m_KetekHandler->CaptureData(m_ExposureSeconds, mcaData->data(), &m_Control->continueRunning))
			{
				exposureProgressRunning->store(false);
				MainFrameVariables::PostExposureFinishedEvent(m_MainFrame);

				// Not cancelled by the user: the detector failed. Tell the UI
				// so it can reset the Live button and show the reason.
				if (m_Control->continueRunning)
				{
					postStopped
					(
						wxString::Format
						(
							"Live capturing stopped because the detector capture failed:\n%s",
							wxString::FromUTF8(m_KetekHandler->GetLastError())
						)
					);
				}

				return (wxThread::ExitCode)0;
			}

			exposureProgressRunning->store(false);

			MainFrameVariables::PostExposureProgressEvent
			(
				m_MainFrame,
				100,
				"Live Exposure: 100%"
			);

			wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_LIVE_CAPTURING);
			evt.SetInt(imageNumber);
			evt.SetPayload<MainFrameVariables::SpectrumPtr>(mcaData);
			wxQueueEvent(m_MainFrame, evt.Clone());

			++imageNumber;
		}
	}
	catch (const std::exception& e)
	{
		postStopped(wxString::Format("Live capturing stopped because of an error: %s", e.what()));
	}
	catch (...)
	{
		postStopped("Live capturing stopped because of an unknown error.");
	}

	MainFrameVariables::PostExposureFinishedEvent(m_MainFrame);
	return (wxThread::ExitCode)0;
}

LiveCapturing::~LiveCapturing()
{
	m_MainFrame = nullptr;
	m_KetekHandler = nullptr;
}
/* ___ End Worker Thread ___ */

/* ___ Start Progress Thread ___ */
wxThread::ExitCode ProgressThread::Entry()
{
	m_Progress = 0;
	m_ProgressMsg = "";
	wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_PROGRESS_CAPTURING);
	while (m_Control->continueRunning)
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

/* ___ Start Motor Move Thread ___ */
wxThread::ExitCode MotorMoveThread::Entry()
{
	float newPosition = std::numeric_limits<float>::quiet_NaN();

	try
	{
		if (m_Operation)
			newPosition = m_Operation();
	}
	catch (...)
	{
		// Always report back: the UI re-enables the motor controls only
		// when it receives the finished event.
	}

	if (m_Frame)
	{
		MainFrameVariables::MotorMovePayload payload{};
		payload.newPosition = newPosition;
		payload.targetCtrl = m_TargetCtrl;
		payload.useChangeValue = m_UseChangeValue;
		payload.refreshAllPositions = m_RefreshAllPositions;

		wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_MOTOR_MOVE_FINISHED);
		evt.SetPayload(payload);
		wxQueueEvent(m_Frame, evt.Clone());
	}

	return (wxThread::ExitCode)0;
}
