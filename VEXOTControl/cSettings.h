#pragma once
#ifndef CSETTINGS_H
#define CSETTINGS_H

#include "wx/wx.h"
#include "wx/artprov.h"
#include "MaterialDesign/wxMaterialDesignArtProvider.hpp"

#include <memory>
#include <map>
#include <utility>
#include <set>
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <regex>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"
#include <nlohmann/json.hpp>

#include "Motor.h"


namespace SettingsVariables
{
	enum ID
	{
		/* Work Station */
		WORK_STATION_CHOICE,

		/* Detector X */
		MOT_DET_X_MOTOR_TXT_CTRL,
		MOT_DET_X_STEPS_PER_MM_ST_TEXT,

		/* Optics X */
		MOT_OPT_X_MOTOR_TXT_CTRL,
		MOT_OPT_X_STEPS_PER_MM_ST_TEXT,

		/* Optics Y */
		MOT_OPT_Y_MOTOR_TXT_CTRL,
		MOT_OPT_Y_STEPS_PER_MM_ST_TEXT,

		/* Optics Z */
		MOT_OPT_Z_MOTOR_TXT_CTRL,
		MOT_OPT_Z_STEPS_PER_MM_ST_TEXT,	

		/* Optics Pitch */
		MOT_OPT_PITCH_MOTOR_TXT_CTRL,
		MOT_OPT_PITCH_STEPS_PER_MM_ST_TEXT,

		/* Optics Yaw */
		MOT_OPT_YAW_MOTOR_TXT_CTRL,
		MOT_OPT_YAW_STEPS_PER_MM_ST_TEXT,

		/* Devices */
		DEVICE_TXT_CTRL,
	};

	enum DeviceManufacturers
	{
		KETEK
	};

	enum MotorManufacturers
	{
		STANDA,
		XERYON
	};

	enum MotorsNames 
	{
		DETECTOR_X,
		OPTICS_X,
		OPTICS_Y,
		OPTICS_Z,
		OPTICS_PITCH,
		OPTICS_YAW
	};

	struct MotorSettings
	{
		wxTextCtrl* motor{}; 
		wxStaticText* steps_per_mm{};
		wxString motor_sn{};
	};

	struct MotorSettingsArray
	{
		std::unique_ptr<MotorSettings[]> m_Detector{}, m_Optics{};

		wxArrayString xml_all_motors[2];
		wxArrayString xml_selected_motors[2];

		std::map<unsigned int, float> unique_motors_map;
		//std::set<float> unique_motors_set[2];
		wxArrayString unique_motors[2];

		MotorSettingsArray()
		{
			m_Detector = std::make_unique<MotorSettings[]>(1); // Only Detector X
			m_Optics = std::make_unique<MotorSettings[]>(5); // Optics X, Y, Z, Pitch, Yaw
		}
	};

	struct MeasurementDevice
	{
		wxTextCtrl* device{};
		wxString selected_device_str{};
	};

	struct WorkStationData
	{
		wxArrayString selectedMotorsInDataFile{};

		std::map<wxString, int> motorsStepsPerMM{};
		std::map<wxString, SettingsVariables::MotorManufacturers> motorVendorBySN{};

		wxString selectedDeviceInDataFile{};
		DeviceManufacturers deviceManufacturer{};
		wxString workStationName{};
	};

	struct WorkStations
	{
		wxChoice* work_station_choice{};
		unsigned short work_stations_count{};
		std::unique_ptr<WorkStationData[]> work_station_data{};
		wxArrayString all_work_station_array_str{};
		wxString initialized_work_station{};
		unsigned short initialized_work_station_num{};
	};

	struct ProgressValues
	{
		int current_capture{}, whole_captures_num{};
		bool is_finished{};
	};

	static MotorManufacturers ParseVendor(const std::string& s)
	{
		auto low = wxString(s).Lower();
		if (low == "xeryon") return SettingsVariables::MotorManufacturers::XERYON;
		return SettingsVariables::MotorManufacturers::STANDA; // default
	}
}

class cSettings final : public wxDialog
{
public:
	cSettings(wxWindow* parent_frame);

	/* Getters */

	bool MotorHasSerialNumber(const int motorName)
	{
		return m_PhysicalMotors->MotorHasSerialNumber
		(
			m_WorkStations->work_station_data[m_WorkStations->initialized_work_station_num].selectedMotorsInDataFile[motorName].ToStdString()
		);
	}

	float GetActualMotorPosition(const int motorName)
	{
		return m_PhysicalMotors->GetActualStagePos
		(
			m_WorkStations->work_station_data[m_WorkStations->initialized_work_station_num].selectedMotorsInDataFile[motorName].ToStdString()
		);
	}

	/* Progress Getter */
	bool IsCapturingFinished() const;
	void ProvideProgressInfo(wxString* msg, int* prgrs);

	/* Progress Setter */
	void ResetCapturing();

	/* Setters */
	float GoToAbsPos(const int motorName, const float absolute_position) 
	{
		return m_PhysicalMotors->GoMotorToAbsPos
		(
			m_WorkStations->work_station_data[m_WorkStations->initialized_work_station_num].selectedMotorsInDataFile[motorName].ToStdString(),
			absolute_position
		);
	};

	float GoOffsetMotor(const int motorName, const float delta) 
	{
		return m_PhysicalMotors->GoMotorOffset
		(
			m_WorkStations->work_station_data[m_WorkStations->initialized_work_station_num].selectedMotorsInDataFile[motorName].ToStdString(),
			delta
		);
	};

	float CenterMotor(const int motorName) 
	{
		return m_PhysicalMotors->GoMotorCenter
		(
			m_WorkStations->work_station_data[m_WorkStations->initialized_work_station_num].selectedMotorsInDataFile[motorName].ToStdString()
		);

	};
	float HomeMotor(const int motorName) 
	{
		return m_PhysicalMotors->GoMotorHome
		(
			m_WorkStations->work_station_data[m_WorkStations->initialized_work_station_num].selectedMotorsInDataFile[motorName].ToStdString()
		);
	};



	/* Detector X */
	//float GoToAbsDetectorX(float absolute_position);
	//float GoOffsetDetectorX(float delta);
	//float CenterDetectorX();
	//float HomeDetectorX();
	/* Detector Y */
	//float GoToAbsDetectorY(float absolute_position);
	//float GoOffsetDetectorY(float delta);
	//float CenterDetectorY();
	//float HomeDetectorY();
	/* Detector Z */
	//float GoToAbsDetectorZ(float absolute_position);
	//float GoOffsetDetectorZ(float delta);
	//float CenterDetectorZ();
	//float HomeDetectorZ();
	/* Optics Y */
	//float GoToAbsOpticsY(float absolute_position);
	//float GoOffsetOpticsY(float delta);
	//float CenterOpticsY();
	//float HomeOpticsY();
	/* Progress */
	void SetCurrentProgress(const int& curr_capturing_num, const int& whole_capturing_num);

	/* KETEK */
	auto GetSelectedKETEK() const -> wxString 
	{ 
		if (m_KETEK) return m_KETEK->selected_device_str; 
		return "";
	};

	/* Camera */
	auto GetSelectedCamera() const -> wxString;

private:
	void CreateMainFrame();
	void CreateSettings();
	void CreateMotorsSelection(wxBoxSizer* panel_sizer);
	void InitDefaultStateWidgets();
	void InitComponents();

	void BindControls();
	void UpdateRangesTextCtrls();

	auto OnWorkStationChoice(wxCommandEvent& evt) -> void;
	auto UpdateMotorsAndCameraTXTCtrls(const short selected_work_station = -1) -> void;
	void OnRefreshBtn(wxCommandEvent& evt);
	void OnOkBtn(wxCommandEvent& evt);
	bool CheckIfThereIsCollisionWithMotors();
	bool CheckIfUserSelectedAllRangesForAllSelectedMotors();
	bool CheckIfUserSelectedAllMotorsForAllSelectedRanges();
	void OnCancelBtn(wxCommandEvent& evt);

	unsigned int FindSerialNumber(const uint8_t selection_number, const SettingsVariables::MotorSettings* motor_settings) const;

	/* Working with XML data and operating with m_Motors variables */
	auto CompareXMLWithConnectedDevices();
	auto ReadInitializationFile() -> void;
	auto LoadWorkStationFiles() -> void;
	auto ReadWorkStationFile(const std::string& fileName, int fileNum) -> void;
	void UpdateUniqueArray();
	void SelectMotorsAndRangesFromXMLFile();

	auto RewriteInitializationFile() -> void;

private:
	const wxString initialization_file_path = "src\\init.ini";
	const wxString work_stations_path = "src\\";
	wxString workStation{}, standaIP{ "10.0.0.134" };
	//const wxString xml_file_path = "src\\old_xml\\mtrs.xml";

	std::unique_ptr<SettingsVariables::WorkStations> m_WorkStations{};
	std::unique_ptr<wxButton> m_OkBtn{}, m_CancelBtn{}, m_RefreshBtn{};
	std::unique_ptr<SettingsVariables::MotorSettingsArray> m_Motors{};
	std::unique_ptr<MotorArray> m_PhysicalMotors{};
	std::unique_ptr<SettingsVariables::MeasurementDevice> m_xPIN{}, m_KETEK{};

	const int m_MotorsCount{ 6 };
	std::unique_ptr<SettingsVariables::ProgressValues> m_Progress = std::make_unique<SettingsVariables::ProgressValues>();
};

#endif // !CSETTINGS_H

