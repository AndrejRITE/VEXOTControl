#pragma once
#ifndef CMAIN_H
#define CMAIN_H

#include "wx/wx.h"
#include "wx/tglbtn.h"
#include "wx/valnum.h"
#include "wx/thread.h"
#include "wx/display.h"
#include "wx/appprogress.h"
#include "wx/filename.h"
#include "wx/notebook.h"
#include "wx/splitter.h"
#include "wx/scrolwin.h"
#include "wx/aboutdlg.h"
#include "wx/dir.h"
#include "wx/propgrid/propgrid.h"
#include "wx/stdpaths.h"

#include <string>
#include <memory>
#include <chrono>
#include <utility>
#include <algorithm>
#include <random>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <atomic>
#include <vector>
#include <cmath>
#include <iomanip>

#include <nlohmann/json.hpp>

#include "cPreviewPanel.h"
#include "cSettings.h"
#include "Ketek.h"

#include "src/img/logo.xpm"

#define MAJOR_VERSION 2
#define MINOR_VERSION 1

#ifdef _DEBUG
	#define OPEN_DATA
#endif // _DEBUG

class cMain;
class ProgressBar;
class ProgressPanel;
class WorkerThread;
class ProgressThread;

namespace MainFrameVariables
{
	enum ID
	{
		/* Menu Bar */
		MENUBAR_FILE_OPEN,
		MENUBAR_FILE_QUIT,
		MENUBAR_EDIT_ENABLE_DARK_MODE,
		MENUBAR_EDIT_SETTINGS,
		MENUBAR_TOOLS_CROSSHAIR,
		MENUBAR_TOOLS_VALUE_DISPLAYING,
		MENUBAR_WINDOW_FULLSCREEN,
		MENUBAR_HELP_ABOUT,
		MENUBAR_HELP_APPS_VERSION,
		/* Detector X */
		RIGHT_SC_DET_X_ABS_TE_CTL,
		RIGHT_SC_DET_X_SET_BTN,
		RIGHT_SC_DET_X_REL_TE_CTL,
		RIGHT_SC_DET_X_DEC_BTN,
		RIGHT_SC_DET_X_INC_BTN,
		RIGHT_SC_DET_X_CENTER_BTN,
		RIGHT_SC_DET_X_HOME_BTN,
		/* Optics X */
		RIGHT_SC_OPT_X_ABS_TE_CTL,
		RIGHT_SC_OPT_X_SET_BTN,
		RIGHT_SC_OPT_X_REL_TE_CTL,
		RIGHT_SC_OPT_X_DEC_BTN,
		RIGHT_SC_OPT_X_INC_BTN,
		RIGHT_SC_OPT_X_CENTER_BTN,
		RIGHT_SC_OPT_X_HOME_BTN,	
		/* Optics Y */
		RIGHT_SC_OPT_Y_ABS_TE_CTL,
		RIGHT_SC_OPT_Y_SET_BTN,
		RIGHT_SC_OPT_Y_REL_TE_CTL,
		RIGHT_SC_OPT_Y_DEC_BTN,
		RIGHT_SC_OPT_Y_INC_BTN,
		RIGHT_SC_OPT_Y_CENTER_BTN,
		RIGHT_SC_OPT_Y_HOME_BTN,
		/* Optics Z */
		RIGHT_SC_OPT_Z_ABS_TE_CTL,
		RIGHT_SC_OPT_Z_SET_BTN,
		RIGHT_SC_OPT_Z_REL_TE_CTL,
		RIGHT_SC_OPT_Z_DEC_BTN,
		RIGHT_SC_OPT_Z_INC_BTN,
		RIGHT_SC_OPT_Z_CENTER_BTN,
		RIGHT_SC_OPT_Z_HOME_BTN,
		/* Optics Pitch */
		RIGHT_SC_OPT_PITCH_ABS_TE_CTL,
		RIGHT_SC_OPT_PITCH_SET_BTN,
		RIGHT_SC_OPT_PITCH_REL_TE_CTL,
		RIGHT_SC_OPT_PITCH_DEC_BTN,
		RIGHT_SC_OPT_PITCH_INC_BTN,
		RIGHT_SC_OPT_PITCH_CENTER_BTN,
		RIGHT_SC_OPT_PITCH_HOME_BTN,	
		/* Optics Yaw */
		RIGHT_SC_OPT_YAW_ABS_TE_CTL,
		RIGHT_SC_OPT_YAW_SET_BTN,
		RIGHT_SC_OPT_YAW_REL_TE_CTL,
		RIGHT_SC_OPT_YAW_DEC_BTN,
		RIGHT_SC_OPT_YAW_INC_BTN,
		RIGHT_SC_OPT_YAW_CENTER_BTN,
		RIGHT_SC_OPT_YAW_HOME_BTN,

		/* Device */
		RIGHT_CAM_EXPOSURE_TXT_CTL,
		RIGHT_DEVICE_MIN_RANGE_TXT_CTL,
		RIGHT_DEVICE_MAX_RANGE_TXT_CTL,
		RIGHT_CAM_MANUFACTURER_CHOICE,
		RIGHT_DEVICE_SINGLE_SHOT_BTN,
		RIGHT_DEVICE_START_STOP_LIVE_CAPTURING_TGL_BTN,
		RIGHT_DEVICE_ACTUAL_PARAMETERS_PROPERTY_GRID,
		RIGHT_CAM_CROSS_HAIR_POS_X_TXT_CTRL,
		RIGHT_CAM_CROSS_HAIR_POS_Y_TXT_CTRL,
		RIGHT_CAM_CROSS_HAIR_SET_POS_TGL_BTN,
		/* Measurement */
		RIGHT_MT_OUT_FLD_TE_CTL,
		RIGHT_MT_OUT_FLD_BTN,
		RIGHT_MT_FIRST_STAGE_CHOICE,
		RIGHT_MT_FIRST_STAGE_START,
		RIGHT_MT_FIRST_STAGE_STEP,
		RIGHT_MT_FIRST_STAGE_FINISH,
		RIGHT_MT_SECOND_STAGE_CHOICE,
		RIGHT_MT_SECOND_STAGE_START,
		RIGHT_MT_SECOND_STAGE_STEP,
		RIGHT_MT_SECOND_STAGE_FINISH,
		RIGHT_MT_START_STOP_MEASUREMENT_TGL_BTN,
		/* Live Capturing */
		THREAD_LIVE_CAPTURING,
		/* Worker Thread */
		THREAD_MAIN_CAPTURING,
		/* Progress */
		THREAD_PROGRESS_CAPTURING,

		/* Exposure UI */
		THREAD_EXPOSURE_PROGRESS,
		THREAD_EXPOSURE_FINISHED,
	};

	enum class CaptureUiMode
	{
		Idle,
		SingleShotRunning,
		LiveRunning,
		MeasurementRunning
	};

	struct MenuBar
	{
		wxMenuBar* menu_bar{};
		wxMenu* menu_file{};
		wxMenu* menu_edit{};
		wxMenu* menu_tools{};
		wxMenu* submenu_intensity_profile{};
		wxMenu* menu_window{};
		wxMenu* menu_help{};
		MenuBar() :
			menu_bar(new wxMenuBar()),
			menu_file(new wxMenu()),
			menu_edit(new wxMenu()),
			menu_tools(new wxMenu()),
			submenu_intensity_profile(new wxMenu()),
			menu_window(new wxMenu()),
			menu_help(new wxMenu()) {};
		~MenuBar()
		{
			menu_file->~wxMenu();
			menu_edit->~wxMenu();
			submenu_intensity_profile->~wxMenu();
			menu_tools->~wxMenu();
			menu_window->~wxMenu();
			menu_help->~wxMenu();
			menu_bar->~wxMenuBar();
		}
	};
	
	struct PropertiesNames
	{
		wxString manufacturer{ "Manufacturer" };
		wxString id{ "ID" };

		wxString firmware_version{ "Firmware Version" };

		wxString mca_length{ "MCA Length" };
		wxString bin_width{ "Bin Width" };
		wxString gain{ "Gain" };
		wxString threshold{ "Threshold" };
		wxString polarity{ "Polarity" };
		wxString board_temperature{ "Board Temperature" };
	};

	struct ToolBar
	{
		wxToolBar* tool_bar{};
		ToolBar() {};
	};

	struct StepperControl
	{
		wxTextCtrl* absolute_text_ctrl{}, *relative_text_ctrl{};
		wxBitmapButton* set_btn{};
		wxButton* increment_btn{}, * decrement_btn{};
		wxBitmapButton* center_btn{}, * home_btn{};

		auto EnableAllControls(const bool enable = true) -> void
		{
			absolute_text_ctrl->Enable(enable);
			relative_text_ctrl->Enable(enable);
			set_btn->Enable(enable);
			increment_btn->Enable(enable);
			decrement_btn->Enable(enable);
			center_btn->Enable(enable);
			home_btn->Enable(enable);
		}
	};
	struct MeasurementStage
	{
		wxChoice* stage{};
		wxTextCtrl* start{}, * step{}, * finish{};
		wxArrayString motors;

		MeasurementStage()
		{
			motors.Add("None");
			motors.Add("Detector X");
			//motors.Add("Detector Y");
			//motors.Add("Detector Z");
			motors.Add("Optics X");
			motors.Add("Optics Y");
			motors.Add("Optics Z");
			motors.Add("Optics Pitch");
			motors.Add("Optics Yaw");
		};

		auto EnableAllControls(const bool enable = true) -> void
		{
			stage->Enable(enable);
			start->Enable(enable);
			step->Enable(enable);
			finish->Enable(enable);
		}
	};

	/* Struct to transfer axis data to WorkerThread */
	struct AxisMeasurement
	{
		int axis_number{ -1 };
		float start{}, step{}, finish{};
		int step_number{};
	};

	struct MotorControlElements 
	{
		wxTextCtrl* absolute_text_ctrl;
		wxTextCtrl* relative_text_ctrl;
		SettingsVariables::MotorsNames motor_id;
	};

	static auto WriteMCAFile
	(
		const wxString filePath, 
		const unsigned long* const mcaData, 
		Ketek* handler, 
		const unsigned long long sumValues, 
		const int exposureSec = 1
	) -> unsigned long
	{
		auto replace_dot_to_comma = []
		(
			std::string str,
			const std::string oldStr,
			const std::string newStr
			)
			{
				std::string::size_type pos = 0u;
				while ((pos = str.find(oldStr, pos)) != std::string::npos) {
					str.replace(pos, oldStr.length(), newStr);
					pos += newStr.length();
				}
				return str;
			};

		auto writeHeader = [&](std::ofstream& outStream, const unsigned long maxValue, const int maxValuePos) 
			{
				outStream << "File Version = " << 1 << '\n';
				outStream << "MCA data = " << "<Generated by ProSpect - v1.1.36>" << '\n';
				outStream << "Title = " << "SingleShot" << '\n';
				outStream << "Setup = " << "" << '\n';
				outStream << "Sample = " << "" << '\n';
				outStream << "User Name = " << "VM" << '\n';
				// Writing current date
				{
					// Get the current time
					std::time_t now = std::time(nullptr);

					// Convert it to a tm structure
					std::tm* local_time = std::localtime(&now);

					// Print the current date in YYYY-MM-DD format
					auto currYear = (1900 + local_time->tm_year);
					auto currMonth = (1 + local_time->tm_mon);
					auto currDay = local_time->tm_mday;
					outStream << "Current Date = " << currDay << "." << currMonth << "." << currYear << '\n';
				}
				auto binSize = replace_dot_to_comma(std::to_string(handler->GetBinSize()), std::string("."), std::string(","));
				outStream << "Bin Size = " << binSize << '\n';
				outStream << "Board Type = " << "KETEK DPP2" << '\n';
				outStream << "Serial = " << handler->GetSerialNumber() << '\n';
				outStream << "Channel = " << 0 << '\n';
				outStream << "Reset Interval = " << "5,000" << " us" << '\n';
				outStream << "Polarity = " << "(+) Positive" << '\n';
				outStream << "Peaking Time Range = " << "0,1 - 24" << '\n';
				outStream << "Peaking Time = " << "24,00" << '\n';
				outStream << "Genset = " << "4" << '\n';
				outStream << "MCA Bin Width (1: Finest) = " << (int)handler->GetBinWidth() << '\n';
				auto baseGain = replace_dot_to_comma(std::to_string(handler->GetGain()), std::string("."), std::string(","));
				outStream << "Base Gain (1-100) = " << baseGain << '\n';
				outStream << "Number MCA Bins = " << handler->GetDataSize() << '\n';
				outStream << "Dynamic Range (keV) = " << "20,000" << '\n';
				outStream << "Fine Gain Trim (0.5-2.0) = " << "1,32095336914062" << '\n';
				outStream << "Baseline (0-4095) = " << 0 << '\n';
				outStream << "Energy (0-4095) = " << 0 << '\n';
				outStream << "Trigger (0-4095) = " << 30 << '\n';
				outStream << "Baseline Average Length = " << 512 << '\n';
				outStream << "Board Temperature = " << replace_dot_to_comma(std::to_string(handler->GetBoardTemperature()), std::string("."), std::string(",")) << '\n';
				outStream << "Events = " << sumValues << '\n';
				outStream << "MaxValue = " << maxValue << '\n';
				outStream << "MaxValuePosition = " << maxValuePos + 1 << '\n';
				outStream << "ICR: = " << "2,4866 kcps" << '\n';
				outStream << "OCR: = " << "2,2000 kcps" << '\n';
				outStream << "Live Time: = " << exposureSec << ",0 s" << '\n';
				outStream << "Real Time: = " << exposureSec << ",0 s" << '\n';
				
				outStream << '\n';
				outStream << handler->GetDataSize() << '\n';

			};

		auto dataSize = handler->GetDataSize();
		auto maxElement = std::max_element(mcaData, mcaData + dataSize);
		auto maxElementPos = static_cast<int>(std::distance(mcaData, maxElement));

		std::ofstream outFile(filePath.ToStdString());

		if (!outFile)
		{
			return *maxElement;
		}
		
		writeHeader(outFile, *maxElement, maxElementPos);

		auto ds = static_cast<int>(handler->GetDataSize());
		for (auto i{ 0 }; i < ds; ++i)
			outFile << mcaData[i] << '\n';

		outFile.close();

		return *maxElement;
	}

	static inline void PostExposureProgressEvent
	(
		cMain* frame,
		const int percent,
		const wxString& text
	)
	{
		if (!frame)
			return;

		wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_EXPOSURE_PROGRESS);
		evt.SetInt(std::clamp(percent, 0, 100));
		evt.SetString(text);
		wxQueueEvent(reinterpret_cast<wxFrame*>(frame), evt.Clone());
	}

	static inline void PostExposureFinishedEvent(cMain* frame)
	{
		if (!frame)
			return;

		wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_EXPOSURE_FINISHED);
		wxQueueEvent(reinterpret_cast<wxFrame*>(frame), evt.Clone());
	}
}

#define USE_MULTITHREAD

/* ___ Start cMain ___ */
class cMain final : public wxFrame
{
public:
	cMain(const wxString& title_);
	//auto StopLiveCapturing() -> bool;
	auto LiveCapturingFinishedCapturingAndDrawing(bool is_finished) -> void;
	auto WorkerThreadFinished(bool is_finished) -> void;
	void UpdateStagePositions();
private:
	void CreateMainFrame();
	void InitComponents();
	void InitDefaultStateWidgets();
	void CreateMenuBarOnFrame();
	void CreateVerticalToolBar();
	auto CreateStatusBar() -> void;
	void CreateLeftAndRightSide();
	auto CreateLeftSide(wxWindow* parent, wxSizer* sizer) -> void;
	auto CreateRightSide(wxWindow* parent, wxSizer* sizer) -> void;
	auto CreateSteppersControl(wxWindow* right_side_panel, wxSizer* right_side_panel_sizer) -> void;

	auto CreateDetectorPage
	(
		wxWindow* parent,
		const wxSize& absoluteTxtCtrlSize,
		const wxSize& relativeTxtCtrlSize,
		const wxSize& setBtnSize,
		const wxSize& incrementDecrementBtnSize,
		const wxBitmap& setBitmap,
		const wxBitmap& centerBitmap,
		const wxBitmap& homeBitmap
	) -> wxWindow*;

	auto CreateOpticsPage
	(
		wxWindow* parent,
		const wxSize& absoluteTxtCtrlSize,
		const wxSize& relativeTxtCtrlSize,
		const wxSize& setBtnSize,
		const wxSize& incrementDecrementBtnSize,
		const wxBitmap& setBitmap,
		const wxBitmap& centerBitmap,
		const wxBitmap& homeBitmap
	) -> wxWindow*;

	auto CreateDeviceControls(wxWindow* right_side_panel, wxSizer* right_side_panel_sizer) -> void;

	auto CreateDevicePage(wxWindow* parent) -> wxWindow*;
	auto CreatePropertiesPage(wxWindow* parent) -> wxWindow*;

	auto CreateMeasurement(wxWindow* right_side_panel, wxSizer* right_side_panel_sizer) -> void;

	auto CreateMeasurementPage(wxWindow* parent) -> wxWindow*;

	auto OnEnableDarkMode(wxCommandEvent& evt) -> void;

	void UnCheckAllTools();
	/* ProgressBar */
	void CreateProgressBar();

	/* Live Capturing */
	void StartLiveCapturing();

	void ChangeCameraManufacturerChoice(wxCommandEvent& evt);
	void OnSingleShotCameraImage(wxCommandEvent& evt);
	void OnSetOutDirectoryBtn(wxCommandEvent& evt);

	auto OnOpenMCAFile(wxCommandEvent& evt) -> void;
	auto ParseMCAFile(const wxString filePath) -> bool;
	void OnOpenSettings(wxCommandEvent& evt);
	auto InitializeSelectedCamera() -> void;
	auto InitializeSelectedDevice() -> void;

	auto UpdateDeviceParameters() -> void;

	void EnableUsedAndDisableNonUsedMotors();

	void OnCrossHairButton(wxCommandEvent& evt);
	void OnValueDisplayingCheck(wxCommandEvent& evt);

	void OnFullScreen(wxCommandEvent& evt);
	void OnMaximizeButton(wxMaximizeEvent& evt);

	void OnExit(wxCloseEvent& evt);
	void OnExit(wxCommandEvent& evt);

	auto OnApplicationVersion(wxCommandEvent& evt) -> void;

	/* Stepper Control Functions */
	void OnEnterTextCtrlAbsPos(int btn_id)
	{
		wxCommandEvent enter_evt(wxEVT_BUTTON, btn_id);
		ProcessEvent(enter_evt);
	}

	void OnSetAbsPos(const MainFrameVariables::MotorControlElements& motor)
	{
		wxBusyCursor cursor;
		double absolute_position{};
		if (!motor.absolute_text_ctrl->GetValue().ToDouble(&absolute_position)) return;
		auto position = m_Settings->GoToAbsPos(motor.motor_id, (float)absolute_position);

		motor.absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), position));
	}

	void OnOffsetAbsPos(const MainFrameVariables::MotorControlElements& motor, float multiplier)
	{
		wxBusyCursor cursor;
		double delta_position{};
		if (!motor.relative_text_ctrl->GetValue().ToDouble(&delta_position)) return;
		auto new_pos = m_Settings->GoOffsetMotor(motor.motor_id, multiplier * (float)delta_position);

		motor.absolute_text_ctrl->ChangeValue(wxString::Format(wxT("%.3f"), new_pos));
	}

	void OnCenterMotor(const MainFrameVariables::MotorControlElements& motor)
	{
		wxBusyCursor cursor;
		motor.absolute_text_ctrl->SetValue(
			wxString::Format(wxT("%.3f"), m_Settings->CenterMotor(motor.motor_id))
		);
	}

	void OnHomeMotor(const MainFrameVariables::MotorControlElements& motor, bool use_change_value = false)
	{
		wxBusyCursor cursor;
		auto value = wxString::Format(wxT("%.3f"), m_Settings->HomeMotor(motor.motor_id));

		if (use_change_value)
			motor.absolute_text_ctrl->ChangeValue(value);
		else
			motor.absolute_text_ctrl->SetValue(value);
	}

	// Wrapper handlers for each motor group
	/* _____________________Detector X_____________________ */
	void OnSetDetectorXAbsPos(wxCommandEvent& evt) { OnSetAbsPos({ m_Detector[0].absolute_text_ctrl, nullptr, SettingsVariables::DETECTOR_X }); }

	void OnDecrementDetectorXAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Detector[0].absolute_text_ctrl, m_Detector[0].relative_text_ctrl, SettingsVariables::DETECTOR_X }, -1); }

	void OnIncrementDetectorXAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Detector[0].absolute_text_ctrl, m_Detector[0].relative_text_ctrl, SettingsVariables::DETECTOR_X }, 1); }

	void OnCenterDetectorX(wxCommandEvent& evt) { OnCenterMotor({ m_Detector[0].absolute_text_ctrl, nullptr, SettingsVariables::DETECTOR_X }); }

	void OnHomeDetectorX(wxCommandEvent& evt) { OnHomeMotor({ m_Detector[0].absolute_text_ctrl, nullptr, SettingsVariables::DETECTOR_X }, true); }

	void OnEnterTextCtrlDetectorXAbsPos(wxCommandEvent& evt) { OnEnterTextCtrlAbsPos(MainFrameVariables::ID::RIGHT_SC_DET_X_SET_BTN); }
	
	/* _____________________Optics X_____________________ */
	void OnSetOpticsXAbsPos(wxCommandEvent& evt) { OnSetAbsPos({ m_Optics[0].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_X }); }

	void OnDecrementOpticsXAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[0].absolute_text_ctrl, m_Optics[0].relative_text_ctrl, SettingsVariables::OPTICS_X }, -1); }

	void OnIncrementOpticsXAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[0].absolute_text_ctrl, m_Optics[0].relative_text_ctrl, SettingsVariables::OPTICS_X }, 1); }

	void OnCenterOpticsX(wxCommandEvent& evt) { OnCenterMotor({ m_Optics[0].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_X }); }

	void OnHomeOpticsX(wxCommandEvent& evt) { OnHomeMotor({ m_Optics[0].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_X }); }

	void OnEnterTextCtrlOpticsXAbsPos(wxCommandEvent& evt) { OnEnterTextCtrlAbsPos(MainFrameVariables::ID::RIGHT_SC_OPT_X_SET_BTN); }

	/* _____________________Optics Y_____________________ */
	void OnSetOpticsYAbsPos(wxCommandEvent& evt) { OnSetAbsPos({ m_Optics[1].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_Y }); }

	void OnDecrementOpticsYAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[1].absolute_text_ctrl, m_Optics[1].relative_text_ctrl, SettingsVariables::OPTICS_Y }, -1); }

	void OnIncrementOpticsYAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[1].absolute_text_ctrl, m_Optics[1].relative_text_ctrl, SettingsVariables::OPTICS_Y }, 1); }

	void OnCenterOpticsY(wxCommandEvent& evt) { OnCenterMotor({ m_Optics[1].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_Y }); }

	void OnHomeOpticsY(wxCommandEvent& evt) { OnHomeMotor({ m_Optics[1].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_Y }); }

	void OnEnterTextCtrlOpticsYAbsPos(wxCommandEvent& evt) { OnEnterTextCtrlAbsPos(MainFrameVariables::ID::RIGHT_SC_OPT_Y_SET_BTN); }

	/* _____________________Optics Z_____________________ */
	void OnSetOpticsZAbsPos(wxCommandEvent& evt) { OnSetAbsPos({ m_Optics[2].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_Z }); }

	void OnDecrementOpticsZAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[2].absolute_text_ctrl, m_Optics[2].relative_text_ctrl, SettingsVariables::OPTICS_Z }, -1); }

	void OnIncrementOpticsZAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[2].absolute_text_ctrl, m_Optics[2].relative_text_ctrl, SettingsVariables::OPTICS_Z }, 1); }

	void OnCenterOpticsZ(wxCommandEvent& evt) { OnCenterMotor({ m_Optics[2].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_Z }); }

	void OnHomeOpticsZ(wxCommandEvent& evt) { OnHomeMotor({ m_Optics[2].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_Z }); }

	void OnEnterTextCtrlOpticsZAbsPos(wxCommandEvent& evt) { OnEnterTextCtrlAbsPos(MainFrameVariables::ID::RIGHT_SC_OPT_Z_SET_BTN); }

	/* _____________________Optics Pitch_____________________ */
	void OnSetOpticsPitchAbsPos(wxCommandEvent& evt) { OnSetAbsPos({ m_Optics[3].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_PITCH }); }

	void OnDecrementOpticsPitchAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[3].absolute_text_ctrl, m_Optics[3].relative_text_ctrl, SettingsVariables::OPTICS_PITCH }, -1); }

	void OnIncrementOpticsPitchAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[3].absolute_text_ctrl, m_Optics[3].relative_text_ctrl, SettingsVariables::OPTICS_PITCH }, 1); }

	void OnCenterOpticsPitch(wxCommandEvent& evt) { OnCenterMotor({ m_Optics[3].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_PITCH }); }

	void OnHomeOpticsPitch(wxCommandEvent& evt) { OnHomeMotor({ m_Optics[3].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_PITCH }); }

	void OnEnterTextCtrlOpticsPitchAbsPos(wxCommandEvent& evt) { OnEnterTextCtrlAbsPos(MainFrameVariables::ID::RIGHT_SC_OPT_PITCH_SET_BTN); }

	/* _____________________Optics Yaw_____________________ */
	void OnSetOpticsYawAbsPos(wxCommandEvent& evt) { OnSetAbsPos({ m_Optics[4].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_YAW }); }

	void OnDecrementOpticsYawAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[4].absolute_text_ctrl, m_Optics[4].relative_text_ctrl, SettingsVariables::OPTICS_YAW }, -1); }

	void OnIncrementOpticsYawAbsPos(wxCommandEvent& evt) { OnOffsetAbsPos({ m_Optics[4].absolute_text_ctrl, m_Optics[4].relative_text_ctrl, SettingsVariables::OPTICS_YAW }, 1); }

	void OnCenterOpticsYaw(wxCommandEvent& evt) { OnCenterMotor({ m_Optics[4].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_YAW }); }

	void OnHomeOpticsYaw(wxCommandEvent& evt) { OnHomeMotor({ m_Optics[4].absolute_text_ctrl, nullptr, SettingsVariables::OPTICS_YAW }); }

	void OnEnterTextCtrlOpticsYawAbsPos(wxCommandEvent& evt) { OnEnterTextCtrlAbsPos(MainFrameVariables::ID::RIGHT_SC_OPT_YAW_SET_BTN); }

	/* First Stage */
	void OnFirstStageChoice(wxCommandEvent& evt);
	/* Second Stage */
	void OnSecondStageChoice(wxCommandEvent& evt);
	/* Changed Exposure value */
	void ExposureValueChanged(wxCommandEvent& evt);
	/* Start Capturing */
	void OnStartStopCapturingButton(wxCommandEvent& evt);
	auto StartCapturing() -> bool;

	void OnStartStopLiveCapturingMenu(wxCommandEvent& evt);
	void OnStartStopLiveCapturingTglBtn(wxCommandEvent& evt);

	void OnStartStopMeasurementMenu(wxCommandEvent& evt);

	/* Thread Live Capturing */
	auto LiveCapturingThread(wxThreadEvent& evt) -> void;
	auto WorkerThreadEvent(wxThreadEvent& evt) -> void;
	/* Progress */
	void UpdateProgress(wxThreadEvent& evt);
	bool Cancelled();

	void UpdateAllAxisGlobalPositions();

	/* CrossHair */
	void OnXPosCrossHairTextCtrl(wxCommandEvent& evt);
	void OnYPosCrossHairTextCtrl(wxCommandEvent& evt);
	auto OnSetPosCrossHairTglBtn(wxCommandEvent& evt) -> void;

	auto CreateMetadataFile() -> void;

	auto GetLiveCapturingBitmap(const bool isCapturing) -> wxBitmap;
	auto GetMeasurementBitmap(const bool isCapturing) -> wxBitmap;

	void OnMinRangeKEVChanged(wxCommandEvent& evt) { ApplyDesiredEnergyRangeFromControls(); }
	void OnMaxRangeKEVChanged(wxCommandEvent& evt) { ApplyDesiredEnergyRangeFromControls(); }
	void ApplyDesiredEnergyRangeFromControls();
	void UpdateDesiredEnergyRangeControlsToFullData();

	wxString GetInitializationFilePath() const;
	wxString LoadMotorsIPAddressEarly() const;
	auto CreateDefaultInitializationFileIfMissing() -> bool;
	auto LoadInitializationFile() -> bool;
	auto SaveInitializationFile() const -> bool;

	void ApplyDarkModeState(bool enabled);
	void InitializeAppearanceFromSystemAndConfig();
	void RestoreDesiredEnergyRangeFromControls();

	void UpdateExposureProgress(wxThreadEvent& evt);
	void FinishExposureProgress(wxThreadEvent& evt) { EndExposureProgress(); }

	void ShowExposureProgressControls(const wxString& label = "Exposure Progress: 0%");
	void HideExposureProgressControls();
	void ResetExposureProgressControls();
	void BeginExposureProgress(int exposureSeconds, const wxString& prefix);
	void EndExposureProgress();

	void ApplyCaptureUiState(MainFrameVariables::CaptureUiMode mode);

	wxPoint GetDefaultProgressWindowPosition() const;
	void SaveProgressWindowGeometry() const;
	void RestoreProgressWindowGeometry();

private:
	wxString m_AppName{};

	std::unique_ptr<MainFrameVariables::PropertiesNames> m_PropertiesNames{};
	wxPropertyGrid* m_CurrentDeviceSettingsPropertyGrid{};

	wxSplitterWindow* m_MainSplitter{};

	wxPanel* m_LeftSidePanel{};

	wxScrolledWindow* m_RightSidePanel{};

	wxNotebook* m_DetectorControlsNotebook{}, * m_OpticsControlsNotebook{};

	wxNotebook* m_DeviceControlsNotebook{};

	wxNotebook* m_MeasurementControlsNotebook{};

	std::unique_ptr<wxStatusBar> m_StatusBar{};

	/* Settings Menu */
	std::unique_ptr<cSettings> m_Settings{};
	/* Menu Bar */
	std::unique_ptr<MainFrameVariables::MenuBar> m_MenuBar{};
	/* Tool Bar */
	std::unique_ptr<MainFrameVariables::ToolBar> m_VerticalToolBar{};
	/* Preview Panel */
	std::unique_ptr<cPreviewPanel> m_PreviewPanel{};
	/* Steppers Control */
	std::unique_ptr<MainFrameVariables::StepperControl[]> m_Detector = std::make_unique<MainFrameVariables::StepperControl[]>(1);
	std::unique_ptr<MainFrameVariables::StepperControl[]> m_Optics = std::make_unique<MainFrameVariables::StepperControl[]>(5);

	/* Device */
	//std::unique_ptr<XimeaControl> m_XimeaControl{};
	wxArrayString m_DeviceArrayString{ "KETEK", "Raspberry", "xPIN" };
	std::unique_ptr<wxTextCtrl> m_DeviceExposure{};
	std::unique_ptr<wxTextCtrl> m_MinRangeKEVTxtCtrl{}, m_MaxRangeKEVTxtCtrl{};
	std::unique_ptr<wxBitmapButton> m_SingleShotBtn{};
	std::unique_ptr<wxBitmapToggleButton> m_StartStopLiveCapturingTglBtn{};

#ifdef SET_CROSSHAIR
	/* CrossHair */
	std::unique_ptr<wxTextCtrl> m_CrossHairPosXTxtCtrl{}, m_CrossHairPosYTxtCtrl{};
	std::unique_ptr<wxToggleButton> m_SetCrossHairPosTglBtn{};
#endif // SET_CROSSHAIR

	/* Measurement */
	std::unique_ptr<wxTextCtrl> m_OutDirTextCtrl{};
	std::unique_ptr<wxBitmapButton> m_OutDirBtn{};
	std::unique_ptr<MainFrameVariables::MeasurementStage> m_FirstStage{};

#ifdef USE_2_AXIS_MEASUREMENT
	std::unique_ptr<MainFrameVariables::MeasurementStage> m_SecondStage{};
#endif // USE_2_AXIS_MEASUREMENT

	std::unique_ptr<wxBitmapToggleButton> m_StartStopMeasurementTglBtn{};

	/* Progress */
	bool m_Cancelled{}, m_DataCalculatedInThisApp{};
	wxCriticalSection m_CSCancelled{};
	std::unique_ptr<ProgressBar> m_ProgressBar{};
	std::chrono::steady_clock::time_point m_StartCalculationTime;
	std::unique_ptr<wxAppProgressIndicator> m_AppProgressIndicator{};

	int m_Progress{};
	wxString m_ProgressMsg{};

	/* CrossHair */
	bool m_IsCrossHairChecked{};

	/* Value Displaying */
	bool m_IsValueDisplayingChecked{};

	/* Live Capturing */
	//bool m_StopLiveCapturing{};
	//bool m_LiveCapturingEndedDrawingOnCamPreview{ true };

	/* Appearance Colors */
	wxColour m_DefaultAppearanceColor = wxColour(230, 230, 230);
	wxColour m_DarkModeAppearanceColor = wxColour(70, 70, 70);

	wxColour m_DefaultWidgetsColor = wxColour(50, 130, 246);

	wxColour m_DefaultCellColor{};

	// kETEK
	std::unique_ptr<Ketek> m_KetekHandler{};

	/* Capturing */
	std::vector<std::pair<wxString, bool>> m_StartedThreads{};

	std::unique_ptr<wxGauge> m_ExposureGauge{};
	std::unique_ptr<wxStaticText> m_ExposureProgressStaticText{};

	bool m_InitializationLoaded{ false };

	std::unique_ptr<wxTimer> m_ExposureUiTimer{};
	std::chrono::steady_clock::time_point m_ExposureStartTime{};
	int m_ExposureDurationSeconds{ 0 };
	wxString m_ExposureProgressPrefix{ "Exposure Progress" };
	bool m_ExposureInProgress{ false };

	wxPanel* m_ExposureProgressPanel{};
	wxBoxSizer* m_ExposureProgressPanelSizer{};

	int m_GraphFontSize{ 18 };

	MainFrameVariables::CaptureUiMode m_CaptureUiMode{ MainFrameVariables::CaptureUiMode::Idle };

	wxPoint m_ProgressWindowPosition{ wxDefaultPosition };
	wxSize  m_ProgressWindowSize{ 340, 124 };

	wxString m_DefaultMotorsIPAddress{ "10.0.0.133" };

	wxDECLARE_EVENT_TABLE();
};
/* ___ End cMain ___ */

/* ___ Start Live Capturing Theread ___ */
class LiveCapturing final: public wxThread
{
public:
	LiveCapturing
	(
		cMain* mainFrame,
		Ketek* ketekHandler,
		wxString* threadKey,
		bool* continueCapturing,
		const int exposureSeconds
	);
	~LiveCapturing();

	virtual void* Entry();

private:
	auto CaptureImage
	(
		unsigned short* short_data_ptr, 
		wxImage* image_ptr
	) -> bool;
	auto UpdatePixelsMultithread
	(
		unsigned short* short_data_ptr, 
		wxImage* image_ptr
	) -> void;
	auto AdjustImageParts
	(
		const unsigned short* data_ptr,
		wxImage* image_ptr,
		const unsigned int start_x,
		const unsigned int start_y,
		const unsigned int finish_x,
		const unsigned int finish_y
	) -> void;

private:
	cMain* m_MainFrame{};
	Ketek* m_KetekHandler{};
	wxString* m_ThreadID{};
	bool* m_ContinueCapturing{};
	int m_ExposureSeconds{};
};
/* ___ End Live Capturing Thread ___ */

/* ___ Start Worker Theread ___ */
class WorkerThread final: public wxThread
{
public:
	WorkerThread
	(
		cMain* mainFrame,
		cSettings* settings,
		Ketek* ketekHandler,
		wxString* threadKey,
		bool* continueCapturing,
		const wxString& path,
		const unsigned long exposureSeconds,
		MainFrameVariables::AxisMeasurement* first_axis,
		MainFrameVariables::AxisMeasurement* second_axis,
		int graphFontSize
	)
		: m_MainFrame(mainFrame), 
		m_Settings(settings), 
		m_KetekHandler(ketekHandler), 
		m_ThreadID(threadKey), 
		m_ContinueCapturing(continueCapturing), 
		m_DataPath(path), 
		m_ExposureTimeSeconds(exposureSeconds), 
		m_FirstAxis(first_axis), 
		m_SecondAxis(second_axis),
		m_GraphFontSize(std::max(10, graphFontSize)) {};

	~WorkerThread() 
	{
		delete m_FirstAxis;
		m_FirstAxis = nullptr;
		delete m_SecondAxis;
		m_SecondAxis = nullptr;
	};

	virtual void* Entry();

private:
	auto CaptureAndSaveData
	(
		unsigned long* const mca,
		const int& image_number,
		const float& first_stage_position,
		const float& second_stage_position,
		const std::string& hours,
		const std::string& minutes,
		const std::string& seconds,
		wxString* filePath
	) -> bool;

	wxBitmap CreateGraph
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
	);

	auto SaveGraph(const wxBitmap& bitmap, const wxString filePath) -> void;

	auto SaveGraphTxt
	(
		const unsigned long* const countData,
		const unsigned long long* const sumData,
		const unsigned int dataSize,
		const wxString timestamp
	) -> void;

	auto MoveFirstStage(const float position) -> float;

	auto SaveImageOnDisk(const int& image_number) -> bool;

	auto AxisNameToString(const int axis) -> std::string;

private:
	cMain* m_MainFrame{};
	cSettings* m_Settings{};
	Ketek* m_KetekHandler{};
	wxString* m_ThreadID{};
	bool* m_ContinueCapturing{};
	wxString m_DataPath{};
	unsigned long m_ExposureTimeSeconds{};
	MainFrameVariables::AxisMeasurement* m_FirstAxis{}, * m_SecondAxis{};
	unsigned long m_MaxElementDuringCapturing{}, m_MaxSumDuringCapturing{};
	int m_BestMeasurementNumber{};
	std::unique_ptr<unsigned long[]> m_AllMaxElementsDuringCapturing{};
	std::unique_ptr<unsigned long long[]> m_AllSumsDuringCapturing{};
	float m_BestFirstAxisPosition{}, m_BestSecondAxisPosition{};
	wxString m_MeasurementGraphFilePath{};
	wxString m_MeasurementGraphTxtFilePath{};

	int m_GraphFontSize{ 18 };
};
/* ___ End Worker Thread ___ */

/* ___ Start Progress Thread ___ */
class ProgressThread final : public wxThread
{
public:
	ProgressThread
	(
		cMain* mainFrame,
		cSettings* settings,
		bool* continueWaiting
	)
		: m_Frame(mainFrame), 
		m_Settings(settings), 
		m_ContinueWaiting(continueWaiting) {};

	virtual void* Entry();

	~ProgressThread() 
	{
		m_Frame = nullptr;
		m_Settings = nullptr;
	};

private:
	cSettings* m_Settings{};
	cMain* m_Frame{};
	int m_Progress{};
	bool* m_ContinueWaiting{};
	wxString m_ProgressMsg{};
};
/* ___ End  Progress Thread ___ */

/* ___ Start ProgressBar ___ */
class ProgressBar final : public wxFrame
{
public:
	ProgressBar(wxWindow* parent, const wxPoint& pos, const wxSize& size);
	void UpdateProgressWithMessage(const wxString& msg, const int& progress);
	void UpdateElapsedTime(const uint64_t& elapsed_seconds);
	void UpdateEstimatedTime(const int& prgs, const uint64_t& estimated_seconds);
	~ProgressBar();

private:
	void CreateProgressBar();

private:
	ProgressPanel* m_ProgressPanel{};
	wxSize m_MainSize{};

	DECLARE_EVENT_TABLE()
};
/* ___ End ProgressBar ___ */

/* ___ Start ProgressPanel ___ */
class ProgressPanel final : public wxPanel
{
public:
	ProgressPanel(wxFrame* parent, const wxSize& size);
	void SetSize(const wxSize& new_size);
	void SetProgress(const int& progress);
	void SetElapsedTime(const int& elapsed_time);
	void SetEstimatedTime(const int& progress, const int& estimated_time);
	void SetComment(const wxString& progress_comment);

private:
	void PaintEvent(wxPaintEvent& evt);
	void Render(wxBufferedPaintDC& dc);
	void OnSize(wxSizeEvent& evt);

	bool IsDarkAppearance() const;
	wxColour Blend(const wxColour& a, const wxColour& b, int alpha255) const;

private:
	int m_Width{}, m_Height{};
	int m_Progress{}, m_PreviousEstimateProgress{};
	int m_ElapsedTime{}, m_EstimatedTime{}; // Time in seconds
	int m_ElapsedHours{}, m_ElapsedMinutes{}, m_ElapsedSeconds{};
	wxString m_ProgressComment{ "Moving to start position" };

	DECLARE_EVENT_TABLE()
};
/* ___ End ProgressPanel ___ */

class ExposureProgressThread final : public wxThread
{
public:
	ExposureProgressThread
	(
		cMain* mainFrame,
		std::shared_ptr<std::atomic_bool> continueRunning,
		int exposureSeconds,
		wxString prefix
	)
		: m_MainFrame(mainFrame),
		m_ContinueRunning(std::move(continueRunning)),
		m_ExposureSeconds(std::max(1, exposureSeconds)),
		m_Prefix(std::move(prefix))
	{
	}

	virtual ExitCode Entry() override
	{
		using namespace std::chrono;

		const auto start = steady_clock::now();
		const auto totalMs = m_ExposureSeconds * 1000;

		while (m_ContinueRunning && m_ContinueRunning->load())
		{
			const auto now = steady_clock::now();
			const auto elapsedMs =
				static_cast<int>(duration_cast<milliseconds>(now - start).count());

			int percent = (elapsedMs * 100) / totalMs;
			percent = std::clamp(percent, 0, 99);

			MainFrameVariables::PostExposureProgressEvent
			(
				m_MainFrame,
				percent,
				wxString::Format("%s: %d%%", m_Prefix, percent)
			);

			if (elapsedMs >= totalMs)
				break;

			wxThread::Sleep(50);
		}

		return (wxThread::ExitCode)0;
	}

private:
	cMain* m_MainFrame{};

	std::shared_ptr<std::atomic_bool> m_ContinueRunning{};

	int m_ExposureSeconds{};
	wxString m_Prefix{};
};

class SingleShotThread final : public wxThread
{
public:
	SingleShotThread
	(
		cMain* mainFrame,
		Ketek* ketekHandler,
		wxString* threadKey,
		bool* continueCapturing,
		wxString outDir,
		int exposureSeconds
	)
		: m_MainFrame(mainFrame),
		m_KetekHandler(ketekHandler),
		m_ThreadID(threadKey),
		m_ContinueCapturing(continueCapturing),
		m_OutDir(std::move(outDir)),
		m_ExposureSeconds(exposureSeconds)
	{
	}

	virtual ExitCode Entry() override
	{
		auto mcaData = std::make_unique<unsigned long[]>(m_KetekHandler->GetDataSize());

		auto exposureProgressRunning = std::make_shared<std::atomic_bool>(true);

		ExposureProgressThread* exposureThread = new ExposureProgressThread
		(
			m_MainFrame,
			exposureProgressRunning,
			m_ExposureSeconds,
			"Single Shot Exposure"
		);

		if (exposureThread->Create(wxTHREAD_DETACHED) == wxTHREAD_NO_ERROR)
			exposureThread->Run();
		else
			delete exposureThread;

		if (!m_KetekHandler->CaptureData(m_ExposureSeconds, mcaData.get(), m_ContinueCapturing))
		{
			exposureProgressRunning->store(false);
			MainFrameVariables::PostExposureFinishedEvent(m_MainFrame);

			wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_MAIN_CAPTURING);
			evt.SetInt(-1);
			wxQueueEvent(m_MainFrame, evt.Clone());

			*m_ThreadID = "";
			return (wxThread::ExitCode)0;
		}

		exposureProgressRunning->store(false);

		MainFrameVariables::PostExposureProgressEvent
		(
			m_MainFrame,
			100,
			"Single Shot Exposure: 100%"
		);

		MainFrameVariables::PostExposureFinishedEvent(m_MainFrame);

		unsigned long long sum{};
		sum = std::accumulate(&mcaData[0], &mcaData[m_KetekHandler->GetDataSize()], sum);

		auto now = std::chrono::system_clock::now();
		auto cur_time = std::chrono::system_clock::to_time_t(now);
		auto str_time = std::string(std::ctime(&cur_time)).substr(11, 8);
		auto cur_hours = str_time.substr(0, 2);
		auto cur_mins = str_time.substr(3, 2);
		auto cur_secs = str_time.substr(6, 2);

		const std::string file_name = std::string(m_OutDir.mb_str()) + std::string("\\") +
			std::string("ss_") +
			cur_hours + std::string("H_") +
			cur_mins + std::string("M_") +
			cur_secs + std::string("S_") +
			std::to_string(m_ExposureSeconds) + std::string("s.mca");

		MainFrameVariables::WriteMCAFile
		(
			wxString(file_name),
			mcaData.get(),
			m_KetekHandler,
			sum,
			m_ExposureSeconds
		);

		wxThreadEvent evt(wxEVT_THREAD, MainFrameVariables::ID::THREAD_MAIN_CAPTURING);
		evt.SetInt(0);
		evt.SetString(wxString(file_name));
		evt.SetPayload(mcaData.get());
		wxQueueEvent(m_MainFrame, evt.Clone());

		{
			auto signalValue = ULONG_MAX - mcaData[0];
			while (mcaData[0] != signalValue)
				wxThread::Sleep(10);
		}


		*m_ThreadID = "";
		return (wxThread::ExitCode)0;
	}

private:
	cMain* m_MainFrame{};
	Ketek* m_KetekHandler{};
	wxString* m_ThreadID{};
	bool* m_ContinueCapturing{};
	wxString m_OutDir{};
	int m_ExposureSeconds{};
};

#endif // !CMAIN_H
