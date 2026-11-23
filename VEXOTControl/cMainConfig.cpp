// cMain: settings file (load, migrate, save) and window layout.
#include "cMain.h"

wxString cMain::GetLegacyInitializationFilePath() const
{
	const wxFileName exeFileName(wxStandardPaths::Get().GetExecutablePath());
	return exeFileName.GetPathWithSep() + m_AppName + ".ini";
}

wxString cMain::GetInitializationFilePath() const
{
#ifdef _DEBUG
	// Debug builds keep their settings next to the executable, so running
	// from Visual Studio doesn't change the real per-user settings.
	return GetLegacyInitializationFilePath();
#else
	// %APPDATA%\VEXOTControl\VEXOTControl.ini - writable without admin
	// rights, unlike the installation folder under Program Files.
	return wxStandardPaths::Get().GetUserConfigDir() +
		wxFileName::GetPathSeparator() + m_AppName +
		wxFileName::GetPathSeparator() + m_AppName + ".ini";
#endif
}

auto cMain::WriteInitializationJson(const nlohmann::json& configuration) const -> bool
{
	const wxString iniPath = GetInitializationFilePath();

	std::string error{};
	if (!FileUtils::WriteFileAtomically(std::filesystem::path(iniPath.ToStdWstring()), configuration.dump(4), &error))
	{
		AppLog::Error("Config", "Saving settings failed: " + error);
		return false;
	}

	return true;
}

auto cMain::OnOpenLogFolder(wxCommandEvent& WXUNUSED(evt)) -> void
{
	const auto directory = AppLog::Directory();

	if (directory.empty() || !wxDir::Exists(wxString(directory.wstring())))
	{
		wxMessageBox("The log folder doesn't exist yet.", "Open Log Folder", wxOK | wxICON_INFORMATION, this);
		return;
	}

	wxLaunchDefaultApplication(wxString(directory.wstring()));
}

auto cMain::LoadAndMigrateConfigurationEarly() -> bool
{
	const wxString iniPath = GetInitializationFilePath();

	const wxFileName executable
	(
		wxStandardPaths::Get().GetExecutablePath()
	);

	const wxString legacyInitPath =
		executable.GetPathWithSep() + wxT("src\\init.json");

	nlohmann::json configuration = nlohmann::json::object();
	bool configurationChanged = false;
	bool legacyConfigurationLoaded = false;

	const auto readJsonFile =
		[]
		(
			const wxString& path,
			nlohmann::json& destination
			) -> bool
		{
			std::ifstream input(path.ToStdWstring());

			if (!input.is_open())
				return false;

			try
			{
				input >> destination;
			}
			catch (const std::exception& e)
			{
				wxLogError
				(
					"Cannot parse configuration file \"%s\": %s",
					path,
					wxString::FromUTF8(e.what())
				);

				return false;
			}

			if (!destination.is_object())
			{
				wxLogError
				(
					"Configuration file \"%s\" must contain "
					"a JSON object.",
					path
				);

				return false;
			}

			return true;
		};

	/*
	 * Load the existing VEXOTControl.ini if it exists.
	 */
	const bool iniExists = wxFileExists(iniPath);
	const wxString legacyIniPath = GetLegacyInitializationFilePath();

	if (iniExists)
	{
		if (!readJsonFile(iniPath, configuration))
			return false;
	}
	else if (legacyIniPath != iniPath && wxFileExists(legacyIniPath) &&
		readJsonFile(legacyIniPath, configuration))
	{
		// First start after the settings moved to the user profile: carry
		// the old settings over. The old file is left where it is.
		configurationChanged = true;
		AppLog::Info
		(
			"Config",
			"Migrated settings from " + legacyIniPath.utf8_string() + " to " + iniPath.utf8_string()
		);
	}
	else
	{
		const bool systemDarkMode =
			wxSystemSettings::GetAppearance().IsDark();

		configuration =
		{
			{ "work_station", m_DefaultWorkStation.ToStdString() },
			{ "standa_ip", m_DefaultMotorsIPAddress.ToStdString() },
			{ "last_exposure_seconds", 1 },
			{
				"desired_range_keV",
				{
					{ "min", 0.0 },
					{ "max", 0.0 }
				}
			},
			{ "dark_mode", systemDarkMode },
			{ "graph_font_size", 18 }
		};

		configurationChanged = true;
	}

	/*
	 * Read the old src\init.json only as a migration source.
	 * The application will never use it as an active configuration again.
	 */
	nlohmann::json legacyConfiguration;

	if (wxFileExists(legacyInitPath))
	{
		legacyConfigurationLoaded =
			readJsonFile
			(
				legacyInitPath,
				legacyConfiguration
			);
	}

	const auto containsString =
		[]
		(
			const nlohmann::json& json,
			const char* key
			) -> bool
		{
			return
				json.contains(key) &&
				json[key].is_string();
		};

	/*
	 * Migrate work_station.
	 */
	if (!containsString(configuration, "work_station"))
	{
		if (
			legacyConfigurationLoaded &&
			containsString(legacyConfiguration, "work_station")
			)
		{
			configuration["work_station"] =
				legacyConfiguration["work_station"];
		}
		else
		{
			configuration["work_station"] =
				m_DefaultWorkStation.ToStdString();
		}

		configurationChanged = true;
	}

	/*
	 * Migrate standa_ip.
	 *
	 * Priority:
	 *   1. Existing standa_ip in VEXOTControl.ini
	 *   2. standa_ip from legacy src\init.json
	 *   3. Old motors_ip_address from VEXOTControl.ini
	 *   4. Compiled default
	 */
	if (!containsString(configuration, "standa_ip"))
	{
		if (
			legacyConfigurationLoaded &&
			containsString(legacyConfiguration, "standa_ip")
			)
		{
			configuration["standa_ip"] =
				legacyConfiguration["standa_ip"];
		}
		else if (containsString(configuration, "motors_ip_address"))
		{
			configuration["standa_ip"] =
				configuration["motors_ip_address"];
		}
		else
		{
			configuration["standa_ip"] =
				m_DefaultMotorsIPAddress.ToStdString();
		}

		configurationChanged = true;
	}

	/*
	 * Remove the old duplicate key after its value has been migrated.
	 */
	if (configuration.erase("motors_ip_address") > 0)
		configurationChanged = true;

	m_DefaultWorkStation =
		wxString::FromUTF8
		(
			configuration["work_station"]
			.get<std::string>()
			.c_str()
		);

	m_DefaultMotorsIPAddress =
		wxString::FromUTF8
		(
			configuration["standa_ip"]
			.get<std::string>()
			.c_str()
		);

	/*
	 * Normalize whitespace. A whitespace-only Standa address means
	 * directly connected USB/COM motors.
	 */
	m_DefaultWorkStation.Trim(true);
	m_DefaultWorkStation.Trim(false);

	m_DefaultMotorsIPAddress.Trim(true);
	m_DefaultMotorsIPAddress.Trim(false);

	configuration["work_station"] =
		m_DefaultWorkStation.ToStdString();

	configuration["standa_ip"] =
		m_DefaultMotorsIPAddress.ToStdString();

	if (configurationChanged)
	{
		if (!WriteInitializationJson(configuration))
		{
			wxLogError
			(
				"Cannot write application configuration: %s",
				iniPath
			);

			return false;
		}
	}

	/*
	 * Keep the old file as a backup, but remove its .json extension so
	 * cSettings will not mistake it for a workstation definition.
	 */
	if (legacyConfigurationLoaded)
	{
		const std::filesystem::path oldPath
		(
			legacyInitPath.ToStdWstring()
		);

		const std::filesystem::path backupPath =
			oldPath.parent_path() / L"init.json.migrated";

		std::error_code error;

		if (!std::filesystem::exists(backupPath, error))
		{
			error.clear();
			std::filesystem::rename
			(
				oldPath,
				backupPath,
				error
			);

			if (error)
			{
				wxLogWarning
				(
					"Legacy init.json was migrated, but could "
					"not be renamed: %s",
					wxString::FromUTF8(error.message())
				);
			}
		}
	}

	return true;
}

auto cMain::LoadInitializationFile() -> bool
{
	const wxString iniPath = GetInitializationFilePath();

	std::ifstream in(iniPath.ToStdWstring());
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

	const std::string workStation =
		j.value
		(
			"work_station",
			m_DefaultWorkStation.ToStdString()
		);

	const std::string standaIP =
		j.value
		(
			"standa_ip",
			m_DefaultMotorsIPAddress.ToStdString()
		);

	m_DefaultWorkStation =
		wxString::FromUTF8(workStation.c_str());

	m_DefaultMotorsIPAddress =
		wxString::FromUTF8(standaIP.c_str());

	m_DefaultWorkStation.Trim(true);
	m_DefaultWorkStation.Trim(false);

	m_DefaultMotorsIPAddress.Trim(true);
	m_DefaultMotorsIPAddress.Trim(false);

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

	nlohmann::json j = nlohmann::json::object();

	/*
	 * Preserve existing and future settings that this function does not
	 * explicitly modify.
	 */
	{
		std::ifstream input(iniPath.ToStdWstring());

		if (input.is_open())
		{
			try
			{
				input >> j;
			}
			catch (const std::exception& e)
			{
				wxLogError
				(
					"Cannot parse existing configuration \"%s\": %s",
					iniPath,
					wxString::FromUTF8(e.what())
				);

				return false;
			}
		}
	}

	if (!j.is_object())
		j = nlohmann::json::object();

	int exposureSeconds = 1;

	if (m_DeviceExposure)
	{
		const wxString exposureString =
			m_DeviceExposure->GetValue().IsEmpty()
			? wxString("1")
			: m_DeviceExposure->GetValue();

		exposureSeconds =
			std::max
			(
				1,
				std::abs(wxAtoi(exposureString))
			);
	}

	double minKeV = 0.0;
	double maxKeV = 0.0;

	if (m_MinRangeKEVTxtCtrl)
		m_MinRangeKEVTxtCtrl->GetValue().ToDouble(&minKeV);

	if (m_MaxRangeKEVTxtCtrl)
		m_MaxRangeKEVTxtCtrl->GetValue().ToDouble(&maxKeV);

	const bool darkMode =
		m_MenuBar &&
		m_MenuBar->menu_window->IsChecked
		(
			MainFrameVariables::ID::
			MENUBAR_WINDOW_ENABLE_DARK_MODE
		);

	j["work_station"] =
		m_DefaultWorkStation.ToStdString();

	j["standa_ip"] =
		m_DefaultMotorsIPAddress.ToStdString();

	j["last_exposure_seconds"] =
		exposureSeconds;

	j["desired_range_keV"] =
	{
		{ "min", std::max(0.0, minKeV) },
		{ "max", std::max(0.0, maxKeV) }
	};

	j["dark_mode"] = darkMode;
	j["graph_font_size"] = m_GraphFontSize;
	j["window"] = WindowLayoutJson();
	j.erase("progress_window"); // the floating progress window is gone

	// Remove the old duplicate key permanently.
	j.erase("motors_ip_address");

	return WriteInitializationJson(j);
}

auto cMain::WindowLayoutJson() const -> nlohmann::json
{
	int rightPanelWidth = 0;
	if (m_MainSplitter && m_MainSplitter->IsSplit())
		rightPanelWidth = ToDIP(m_MainSplitter->GetClientSize().GetWidth() - m_MainSplitter->GetSashPosition());

	nlohmann::json window =
	{
		// A minimized window reports "not maximized"; use its state before.
		{ "maximized", IsIconized() ? m_WasMaximized : IsMaximized() },
		{ "right_panel_width_dip", rightPanelWidth }
	};

	// Only a real normal-size rectangle is saved; while the app has only
	// ever been maximized there is none, and the next start centres it.
	if (!m_NormalWindowRect.IsEmpty())
	{
		window["x"] = m_NormalWindowRect.x;
		window["y"] = m_NormalWindowRect.y;
		window["w"] = m_NormalWindowRect.width;
		window["h"] = m_NormalWindowRect.height;
	}

	return window;
}

void cMain::RestoreWindowLayout()
{
	wxRect rect{};
	bool maximize = true;
	int rightPanelWidth = 0;
	bool haveLayout = false;
	{
		std::ifstream in(GetInitializationFilePath().ToStdWstring());
		if (in.is_open())
		{
			// A missing or hand-edited (wrongly typed) entry just means the
			// default layout.
			try
			{
				nlohmann::json j;
				in >> j;
				if (j.is_object() && j.contains("window") && j["window"].is_object())
				{
					const auto& window = j["window"];
					rect = wxRect(window.value("x", 0), window.value("y", 0), window.value("w", 0), window.value("h", 0));
					maximize = window.value("maximized", true);
					rightPanelWidth = window.value("right_panel_width_dip", 0);
					haveLayout = true;
				}
			}
			catch (...)
			{
				rect = wxRect();
				maximize = true;
				rightPanelWidth = 0;
				haveLayout = false;
			}
		}
	}

	bool restored = false;

	if (haveLayout)
	{

		// Only use the saved rectangle if it is still on a connected display.
		const wxSize minSize = GetMinSize();
		if (rect.width >= minSize.GetWidth() && rect.height >= minSize.GetHeight() &&
			wxDisplay::GetFromPoint(wxPoint(rect.x + rect.width / 2, rect.y + rect.height / 2)) != wxNOT_FOUND)
		{
			SetSize(rect);
			m_NormalWindowRect = rect;
			restored = true;
		}
	}

	if (!restored)
		CenterOnScreen();

	Show();

	if (maximize)
		Maximize();

	if (rightPanelWidth > 0 && m_MainSplitter)
	{
		// After the frame has its final size
		CallAfter([this, rightPanelWidth]()
			{
				if (!m_MainSplitter || !m_MainSplitter->IsSplit())
					return;

				const int total = m_MainSplitter->GetClientSize().GetWidth();
				const int minLeft = m_LeftSidePanel ? m_LeftSidePanel->GetMinSize().GetWidth() : 0;
				const int minRight = m_RightSidePanel ? m_RightSidePanel->GetMinSize().GetWidth() : 0;
				const int right = std::clamp(FromDIP(rightPanelWidth), std::max(0, minRight), std::max(minRight, total - minLeft));
				m_MainSplitter->SetSashPosition(total - right);
			});
	}
}

wxString cMain::GetKetekConfigurationFilePath() const
{
	const wxFileName executable(
		wxStandardPaths::Get().GetExecutablePath()
	);

	return executable.GetPathWithSep() +
		wxT("KetekConfig.json");
}
