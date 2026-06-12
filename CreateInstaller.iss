[Setup]
AppId=Rigaku.{#RepoName}
AppName={#RepoName}
AppVersion={#Major}.{#Minor}.{#Build}
AppVerName={#RepoName} - {#Major}.{#Minor}.{#Build}
AppPublisher=Rigaku Innovative Technologies Europe
VersionInfoCompany=Rigaku Innovative Technologies Europe
DefaultDirName={localappdata}\Programs\{#RepoName}
DefaultGroupName={#RepoName}
OutputBaseFilename={#OutputBaseFilename}
OutputDir={#OutputDir}
Compression=lzma
SolidCompression=yes
WizardStyle=modern
DisableWelcomePage=no
SetupIconFile={#IconFullPath}
DisableDirPage=no
UninstallDisplayIcon={app}\{#RepoName}.exe
PrivilegesRequired=admin
LicenseFile=License.txt

[Tasks]
Name: "desktopicon"; Description: "Create a desktop icon"; GroupDescription: "Additional options:"; Flags: checkedonce
Name: "install_vcredist"; Description: "Install required Microsoft Visual C++ Redistributables"; GroupDescription: "Additional options:"; Flags: checkedonce

[Dirs]
Name: "{localappdata}\Programs"; Permissions: users-full

[Files]
Source: "{#OutputDir}\{#RepoName}.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#OutputDir}\src\*"; DestDir: "{app}\src"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#OutputDir}\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#OutputDir}\KETEK.ini"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#OutputDir}\keyfile.sqlite"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#OutputDir}\table.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#IconFullPath}"; DestDir: "{app}"; Flags: ignoreversion

Source: "redist\vcredist_2010_x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
Source: "redist\vcredist_2013_x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall
Source: "redist\VC_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

[Icons]
Name: "{group}\{#RepoName}"; Filename: "{app}\{#RepoName}.exe"; IconFilename: "{app}\logo.ico"
Name: "{commondesktop}\{#RepoName}"; Filename: "{app}\{#RepoName}.exe"; IconFilename: "{app}\logo.ico"
Name: "{commonprograms}\{#RepoName}"; Filename: "{app}\{#RepoName}.exe"; IconFilename: "{app}\logo.ico"

[Registry]
Root: HKCU; Subkey: "SOFTWARE\RITE\{#RepoName}"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: createvalueifdoesntexist uninsdeletekey

[Run]
; VC++ 2010 x64 - provides msvcr100.dll
Filename: "{tmp}\vcredist_2010_x64.exe"; \
    Parameters: "/quiet /norestart"; \
    StatusMsg: "Installing Microsoft Visual C++ 2010 Redistributable x64..."; \
    Flags: waituntilterminated; \
    Tasks: install_vcredist; \
    Check: ShouldInstallVCRedist2010x64

; VC++ 2013 x64 - provides msvcr120.dll
Filename: "{tmp}\vcredist_2013_x64.exe"; \
    Parameters: "/quiet /norestart"; \
    StatusMsg: "Installing Microsoft Visual C++ 2013 Redistributable x64..."; \
    Flags: waituntilterminated; \
    Tasks: install_vcredist; \
    Check: ShouldInstallVCRedist2013x64

; VC++ 2015-2022 x64 - provides modern v14 runtime
Filename: "{tmp}\VC_redist.x64.exe"; \
    Parameters: "/install /quiet /norestart"; \
    StatusMsg: "Installing Microsoft Visual C++ 2015-2022 Redistributable x64..."; \
    Flags: waituntilterminated; \
    Tasks: install_vcredist; \
    Check: ShouldInstallVCRedistModernx64

; Launch app after install
Filename: "{app}\{#RepoName}.exe"; Description: "{cm:LaunchProgram,{#RepoName}}"; Flags: nowait postinstall skipifsilent

[Code]
function GetInstallPath: string;
var
  InstallPath: string;
begin
  Result := '';

  if RegQueryStringValue(HKCU, 'SOFTWARE\RITE\{#RepoName}', 'InstallPath', InstallPath) then
  begin
    Result := InstallPath;
  end;
end;


// ---------- VC++ Redistributable Detection ----------

function IsVCRedist2010x64Installed: Boolean;
var
  Installed: Cardinal;
begin
  Result := False;

  if RegQueryDWordValue(
       HKLM,
       'SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x64',
       'Installed',
       Installed) then
  begin
    if Installed = 1 then
    begin
      Log('VC++ 2010 Redistributable x64 detected.');
      Result := True;
      Exit;
    end;
  end;

  Log('VC++ 2010 Redistributable x64 was not detected.');
end;


function IsVCRedist2013x64Installed: Boolean;
var
  Installed: Cardinal;
begin
  Result := False;

  if RegQueryDWordValue(
       HKLM,
       'SOFTWARE\Microsoft\VisualStudio\12.0\VC\Runtimes\x64',
       'Installed',
       Installed) then
  begin
    if Installed = 1 then
    begin
      Log('VC++ 2013 Redistributable x64 detected.');
      Result := True;
      Exit;
    end;
  end;

  Log('VC++ 2013 Redistributable x64 was not detected.');
end;


function IsVCRedistModernx64Installed: Boolean;
var
  Installed: Cardinal;
  Version: string;
begin
  Result := False;

  if RegQueryDWordValue(
       HKLM,
       'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
       'Installed',
       Installed) then
  begin
    if Installed = 1 then
    begin
      if RegQueryStringValue(
           HKLM,
           'SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64',
           'Version',
           Version) then
      begin
        Log(Format('VC++ 2015-2022 Redistributable x64 detected. Version: %s', [Version]));
      end
      else
      begin
        Log('VC++ 2015-2022 Redistributable x64 detected, but version value was not found.');
      end;

      Result := True;
      Exit;
    end;
  end;

  Log('VC++ 2015-2022 Redistributable x64 was not detected.');
end;


function ShouldInstallVCRedist2010x64: Boolean;
begin
  Result := False;

  if not WizardIsTaskSelected('install_vcredist') then
  begin
    Log('VC++ 2010 Redistributable installation skipped because the user did not select the task.');
    Exit;
  end;

  if IsVCRedist2010x64Installed then
  begin
    Log('VC++ 2010 Redistributable installation skipped because it is already installed.');
    Exit;
  end;

  Log('VC++ 2010 Redistributable installation will run.');
  Result := True;
end;


function ShouldInstallVCRedist2013x64: Boolean;
begin
  Result := False;

  if not WizardIsTaskSelected('install_vcredist') then
  begin
    Log('VC++ 2013 Redistributable installation skipped because the user did not select the task.');
    Exit;
  end;

  if IsVCRedist2013x64Installed then
  begin
    Log('VC++ 2013 Redistributable installation skipped because it is already installed.');
    Exit;
  end;

  Log('VC++ 2013 Redistributable installation will run.');
  Result := True;
end;


function ShouldInstallVCRedistModernx64: Boolean;
begin
  Result := False;

  if not WizardIsTaskSelected('install_vcredist') then
  begin
    Log('VC++ 2015-2022 Redistributable installation skipped because the user did not select the task.');
    Exit;
  end;

  if IsVCRedistModernx64Installed then
  begin
    Log('VC++ 2015-2022 Redistributable installation skipped because it is already installed.');
    Exit;
  end;

  Log('VC++ 2015-2022 Redistributable installation will run.');
  Result := True;
end;