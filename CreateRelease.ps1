#Requires -Version 5.1

[CmdletBinding()]
param(
    [string]$Configuration = "Release",
    [string]$Platform = "x64",

    [switch]$SkipSigning,
    [switch]$SkipGitHub,
    [switch]$SkipOneDrive
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# =============================================================================
# Configuration
# =============================================================================

$Config = @{
    GitHubUser      = "AndrejRITE"
    RepositoryName  = "VEXOTControl"
    RepositoryRoot  = "D:\Projects\RIGAKU\VEXOTControl"

    Configuration   = $Configuration
    Platform        = $Platform
    OperatingSystem = "win"

    MSBuildPath     = "C:\Program Files\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe"
    SevenZipPath    = "C:\Program Files\7-Zip\7z.exe"
    InnoSetupPath   = "C:\Program Files (x86)\Inno Setup 6\ISCC.exe"

    OneDriveFolder  = "C:\Users\Andrej Pcelovodov\OneDrive - Rigaku Americas Holding\EXPORT\VEXOTControl"
}

$Paths = @{
    Solution       = Join-Path $Config.RepositoryRoot "$($Config.RepositoryName).sln"
    ProjectSource  = Join-Path $Config.RepositoryRoot $Config.RepositoryName
    Release        = Join-Path $Config.RepositoryRoot "bin\$($Config.Platform)\$($Config.Configuration)"
    Temp           = Join-Path $Config.RepositoryRoot ".temp"
    Redist         = Join-Path $Config.RepositoryRoot "redist"
    Log            = Join-Path $Config.RepositoryRoot "log.txt"

    Header         = Join-Path $Config.RepositoryRoot "$($Config.RepositoryName)\cMain.h"
    MainCpp        = Join-Path $Config.RepositoryRoot "$($Config.RepositoryName)\cMain.cpp"
	
	License = Join-Path $Config.RepositoryRoot "License.txt"

    InnoTemplate   = Join-Path $Config.RepositoryRoot "CreateInstaller.iss"
    InnoTemp       = Join-Path $Config.RepositoryRoot ".temp\CreateInstaller.iss"
    CodeSign       = Join-Path $Config.RepositoryRoot "CodeSign.ps1"
}

# Script-wide release state. Populated by Initialize-ReleaseState.
$script:Version = $null
$script:BuildVersion = $null
$script:TagName = $null
$script:ArchiveName = $null
$script:ArchivePath = $null
$script:InstallerNameWithoutExtension = $null
$script:InstallerName = $null
$script:InstallerPath = $null
$script:CommitMessage = $null

# =============================================================================
# Common helpers
# =============================================================================

function Write-ReleaseLog {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Message
    )

    $timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
    $line = "[$timestamp] $Message"

    Write-Host $line
    Add-Content -LiteralPath $Paths.Log -Value $line
}

function Assert-FileExists {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [string]$Description = "Required file"
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        throw "$Description was not found: $Path"
    }
}

function Assert-DirectoryExists {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Path,

        [string]$Description = "Required directory"
    )

    if (-not (Test-Path -LiteralPath $Path -PathType Container)) {
        throw "$Description was not found: $Path"
    }
}

function Invoke-NativeCommand {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Executable,

        [string[]]$Arguments = @(),

        [Parameter(Mandatory = $true)]
        [string]$Description
    )

    Assert-FileExists -Path $Executable -Description "Executable"
    Write-ReleaseLog $Description

    & $Executable @Arguments
    $exitCode = $LASTEXITCODE

    if ($exitCode -ne 0) {
        throw "$Description failed with exit code $exitCode."
    }
}

function Get-ExecutablePath {
    param(
        [Parameter(Mandatory = $true)]
        [string]$CommandName,

        [Parameter(Mandatory = $true)]
        [string]$Description
    )

    $command = Get-Command $CommandName -ErrorAction SilentlyContinue
    if (-not $command) {
        throw "$Description was not found in PATH ($CommandName)."
    }

    return $command.Source
}

# =============================================================================
# Preflight
# =============================================================================

function Test-ReleaseEnvironment {
    Write-ReleaseLog "Running release preflight checks."

    Assert-DirectoryExists -Path $Config.RepositoryRoot -Description "Repository root"
    Assert-FileExists -Path $Paths.Solution -Description "Visual Studio solution"
    Assert-FileExists -Path $Paths.Header -Description "Version header"
    Assert-FileExists -Path $Paths.MainCpp -Description "Main source file"
    Assert-FileExists -Path $Paths.InnoTemplate -Description "Inno Setup template"
	
	Assert-FileExists -Path $Paths.License -Description "License file"

    Assert-FileExists -Path $Config.MSBuildPath -Description "MSBuild"
    Assert-FileExists -Path $Config.SevenZipPath -Description "7-Zip"
    Assert-FileExists -Path $Config.InnoSetupPath -Description "Inno Setup compiler"

    $null = Get-ExecutablePath -CommandName "git.exe" -Description "Git"

    if (-not $SkipSigning) {
        Assert-FileExists -Path $Paths.CodeSign -Description "Code-signing script"
    }

    if (-not $SkipGitHub) {
        $ghPath = Get-ExecutablePath -CommandName "gh.exe" -Description "GitHub CLI"
        Invoke-NativeCommand -Executable $ghPath -Arguments @("auth", "status") -Description "Checking GitHub CLI authentication"
    }

    if (-not $SkipOneDrive) {
        Assert-DirectoryExists -Path $Config.OneDriveFolder -Description "OneDrive release folder"
    }

    if ([string]::IsNullOrWhiteSpace($env:XIMC_LATEST)) {
        throw "Environment variable XIMC_LATEST is not defined."
    }

    if ([string]::IsNullOrWhiteSpace($env:KETEK_LATEST)) {
        throw "Environment variable KETEK_LATEST is not defined."
    }

    $ximcFolder = Join-Path $env:XIMC_LATEST "win64"
    $ketekFolder = Join-Path $env:KETEK_LATEST "lib"

    Assert-DirectoryExists -Path $ximcFolder -Description "XIMC runtime directory"
    Assert-DirectoryExists -Path $ketekFolder -Description "KETEK runtime directory"

    $requiredFiles = @(
        @{ Path = Join-Path $ximcFolder "bindy.dll";       Description = "XIMC bindy.dll" },
        @{ Path = Join-Path $ximcFolder "libximc.dll";     Description = "XIMC libximc.dll" },
        @{ Path = Join-Path $ximcFolder "xiwrapper.dll";   Description = "XIMC xiwrapper.dll" },

        @{ Path = Join-Path $ketekFolder "handel.dll";     Description = "KETEK handel.dll" },
        @{ Path = Join-Path $ketekFolder "xia_usb2.dll";   Description = "KETEK xia_usb2.dll" },
        @{ Path = Join-Path $ketekFolder "xw.dll";         Description = "KETEK xw.dll" },

        @{ Path = Join-Path $Paths.ProjectSource "keyfile.sqlite"; Description = "keyfile.sqlite" },
        @{ Path = Join-Path $Paths.ProjectSource "KetekConfig.json"; Description = "KetekConfig.json" },
        @{ Path = Join-Path $Paths.ProjectSource "KETEK.ini"; Description = "KETEK.ini" },
        @{ Path = Join-Path $Paths.ProjectSource "table.txt"; Description = "table.txt" },

        @{ Path = Join-Path $Paths.Redist "vcredist_2010_x64.exe"; Description = "Visual C++ 2010 redistributable" },
        @{ Path = Join-Path $Paths.Redist "vcredist_2013_x64.exe"; Description = "Visual C++ 2013 redistributable" },
        @{ Path = Join-Path $Paths.Redist "VC_redist.x64.exe"; Description = "Current Visual C++ redistributable" }
    )

    foreach ($required in $requiredFiles) {
        Assert-FileExists -Path $required.Path -Description $required.Description
    }

    $sourceJsonFolder = Join-Path $Paths.ProjectSource "src"
    Assert-DirectoryExists -Path $sourceJsonFolder -Description "Project src folder"

    Write-ReleaseLog "Preflight checks completed successfully."
}

# =============================================================================
# Version and release state
# =============================================================================

function Get-ProjectVersion {
    $content = Get-Content -LiteralPath $Paths.Header -Raw

    $majorMatch = [regex]::Match($content, '#define\s+MAJOR_VERSION\s+(\d+)')
    $minorMatch = [regex]::Match($content, '#define\s+MINOR_VERSION\s+(\d+)')

    if (-not $majorMatch.Success) {
        throw "MAJOR_VERSION was not found in '$($Paths.Header)'."
    }

    if (-not $minorMatch.Success) {
        throw "MINOR_VERSION was not found in '$($Paths.Header)'."
    }

    $gitPath = Get-ExecutablePath -CommandName "git.exe" -Description "Git"

    $commitCount = & $gitPath -C $Config.RepositoryRoot rev-list --count HEAD
    if ($LASTEXITCODE -ne 0) {
        throw "Unable to determine Git commit count."
    }

    $commitCountText = ($commitCount | Out-String).Trim()
    $commitCountValue = 0
    if (-not [int]::TryParse($commitCountText, [ref]$commitCountValue)) {
        throw "Git returned an invalid commit count: '$commitCountText'."
    }

    return @{
        Major = [int]$majorMatch.Groups[1].Value
        Minor = [int]$minorMatch.Groups[1].Value
        Build = $commitCountValue
    }
}

function Initialize-ReleaseState {
    $script:Version = Get-ProjectVersion
    $script:BuildVersion = "$($script:Version.Major).$($script:Version.Minor).$($script:Version.Build)"
    $script:TagName = "v$($script:BuildVersion)"

    $script:ArchiveName = "$($Config.RepositoryName)_v$($script:BuildVersion).7z"
    $script:ArchivePath = Join-Path $Paths.Release $script:ArchiveName

    $script:InstallerNameWithoutExtension = "$($Config.RepositoryName)Installer_v$($script:BuildVersion)_$($Config.OperatingSystem)_$($Config.Platform)"
    $script:InstallerName = "$($script:InstallerNameWithoutExtension).exe"
    $script:InstallerPath = Join-Path $Paths.Release $script:InstallerName

    $gitPath = Get-ExecutablePath -CommandName "git.exe" -Description "Git"
    $message = & $gitPath -C $Config.RepositoryRoot log -1 --pretty=%B
    if ($LASTEXITCODE -ne 0) {
        throw "Unable to read the latest Git commit message."
    }

    $script:CommitMessage = ($message | Out-String).Trim()

    Write-ReleaseLog "Release version: $($script:BuildVersion)"
}

# =============================================================================
# Build
# =============================================================================

function Invoke-VersionedBuild {
    $backupPath = "$($Paths.MainCpp).release-backup"

    Assert-FileExists -Path $Paths.MainCpp -Description "Main source file"

    Copy-Item -LiteralPath $Paths.MainCpp -Destination $backupPath -Force

    try {
        $now = Get-Date
        $content = [System.IO.File]::ReadAllText($Paths.MainCpp)

        $content = $content.Replace("{#CommitNumber}", "$($script:Version.Build)")
        $content = $content.Replace("{#CurrentYear}", $now.ToString("yyyy"))
        $content = $content.Replace("{#CurrentMonth}", $now.ToString("MM"))
        $content = $content.Replace("{#CurrentDay}", $now.ToString("dd"))

        $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
        [System.IO.File]::WriteAllText($Paths.MainCpp, $content, $utf8NoBom)

        $msbuildArguments = @(
            $Paths.Solution,
            "/p:Configuration=$($Config.Configuration)",
            "/p:Platform=$($Config.Platform)",
            "/t:Build",
            "/m"
        )

        Invoke-NativeCommand `
            -Executable $Config.MSBuildPath `
            -Arguments $msbuildArguments `
            -Description "Building $($Config.RepositoryName) $($script:BuildVersion)"
    }
    finally {
        if (Test-Path -LiteralPath $backupPath -PathType Leaf) {
            Move-Item -LiteralPath $backupPath -Destination $Paths.MainCpp -Force
        }
    }

    $applicationExe = Join-Path $Paths.Release "$($Config.RepositoryName).exe"
    Assert-FileExists -Path $applicationExe -Description "Built application executable"
}

# =============================================================================
# Release staging
# =============================================================================

function Clear-PreviousReleaseArtifacts {
    Write-ReleaseLog "Removing previous release artifacts owned by this script."

    if (-not (Test-Path -LiteralPath $Paths.Release -PathType Container)) {
        New-Item -Path $Paths.Release -ItemType Directory -Force | Out-Null
        return
    }

    Get-ChildItem -LiteralPath $Paths.Release -Filter "$($Config.RepositoryName)Installer_*.exe" -File -ErrorAction SilentlyContinue |
        Remove-Item -Force

    Get-ChildItem -LiteralPath $Paths.Release -Filter "$($Config.RepositoryName)_v*.7z" -File -ErrorAction SilentlyContinue |
        Remove-Item -Force
}

function Copy-RuntimeFiles {
    Write-ReleaseLog "Copying runtime and configuration files."

    $ximcFolder = Join-Path $env:XIMC_LATEST "win64"
    $ketekFolder = Join-Path $env:KETEK_LATEST "lib"

    $runtimeFiles = @(
        @{ Source = Join-Path $ximcFolder "bindy.dll"; Destination = "bindy.dll" },
        @{ Source = Join-Path $ximcFolder "libximc.dll"; Destination = "libximc.dll" },
        @{ Source = Join-Path $ximcFolder "xiwrapper.dll"; Destination = "xiwrapper.dll" },

        @{ Source = Join-Path $ketekFolder "handel.dll"; Destination = "handel.dll" },
        @{ Source = Join-Path $ketekFolder "xia_usb2.dll"; Destination = "xia_usb2.dll" },
        @{ Source = Join-Path $ketekFolder "xw.dll"; Destination = "xw.dll" },

        @{ Source = Join-Path $Paths.ProjectSource "keyfile.sqlite"; Destination = "keyfile.sqlite" },
        @{ Source = Join-Path $Paths.ProjectSource "KetekConfig.json"; Destination = "KetekConfig.json" },
        @{ Source = Join-Path $Paths.ProjectSource "KETEK.ini"; Destination = "KETEK.ini" },
        @{ Source = Join-Path $Paths.ProjectSource "table.txt"; Destination = "table.txt" }
    )

    foreach ($item in $runtimeFiles) {
        Assert-FileExists -Path $item.Source
        $destination = Join-Path $Paths.Release $item.Destination
        Copy-Item -LiteralPath $item.Source -Destination $destination -Force
    }

    $sourceJsonFolder = Join-Path $Paths.ProjectSource "src"
    $destinationJsonFolder = Join-Path $Paths.Release "src"

    if (Test-Path -LiteralPath $destinationJsonFolder -PathType Container) {
        Remove-Item -LiteralPath $destinationJsonFolder -Recurse -Force
    }

    New-Item -Path $destinationJsonFolder -ItemType Directory -Force | Out-Null

    $jsonFiles = Get-ChildItem -LiteralPath $sourceJsonFolder -Filter "*.json" -File |
        Where-Object { $_.Name -notlike "debug_*.json" }

    foreach ($file in $jsonFiles) {
        Copy-Item -LiteralPath $file.FullName -Destination $destinationJsonFolder -Force
    }
}

# =============================================================================
# Archive
# =============================================================================

function New-ReleaseArchive {
    Write-ReleaseLog "Creating archive '$($script:ArchiveName)'."

    $applicationExe = Join-Path $Paths.Release "$($Config.RepositoryName).exe"
    $srcFolder = Join-Path $Paths.Release "src"

    $filesToArchive = @(
        $srcFolder,
        $applicationExe,
        (Join-Path $Paths.Release "KETEK.ini"),
        (Join-Path $Paths.Release "KetekConfig.json"),
        (Join-Path $Paths.Release "keyfile.sqlite"),
        (Join-Path $Paths.Release "table.txt"),
        (Join-Path $Paths.Redist "vcredist_2010_x64.exe"),
        (Join-Path $Paths.Redist "vcredist_2013_x64.exe"),
        (Join-Path $Paths.Redist "VC_redist.x64.exe")
    )

    $dllFiles = Get-ChildItem -LiteralPath $Paths.Release -Filter "*.dll" -File |
        ForEach-Object { $_.FullName }

    $filesToArchive += $dllFiles

    foreach ($path in $filesToArchive) {
        if (-not (Test-Path -LiteralPath $path)) {
            throw "Archive input does not exist: $path"
        }
    }

    if (Test-Path -LiteralPath $script:ArchivePath -PathType Leaf) {
        Remove-Item -LiteralPath $script:ArchivePath -Force
    }

    $sevenZipArguments = @("a", "-t7z", $script:ArchivePath) + $filesToArchive

    Invoke-NativeCommand `
        -Executable $Config.SevenZipPath `
        -Arguments $sevenZipArguments `
        -Description "Creating 7-Zip archive"

    Assert-FileExists -Path $script:ArchivePath -Description "Release archive"

    return $script:ArchivePath
}

# =============================================================================
# Installer
# =============================================================================

function New-ReleaseInstaller {
    Write-ReleaseLog "Preparing Inno Setup script."

    New-Item -Path $Paths.Temp -ItemType Directory -Force | Out-Null
    Copy-Item -LiteralPath $Paths.InnoTemplate -Destination $Paths.InnoTemp -Force

    $iconFullPath = Join-Path $Paths.ProjectSource "src\img\logo.ico"
    Assert-FileExists -Path $iconFullPath -Description "Installer icon"

    $content = [System.IO.File]::ReadAllText($Paths.InnoTemp)
    $content = $content.Replace("{#Major}", "$($script:Version.Major)")
    $content = $content.Replace("{#Minor}", "$($script:Version.Minor)")
    $content = $content.Replace("{#Build}", "$($script:Version.Build)")
    $content = $content.Replace("{#RepoName}", $Config.RepositoryName)
    $content = $content.Replace("{#OutputBaseFilename}", $script:InstallerNameWithoutExtension)
    $content = $content.Replace("{#OutputDir}", $Paths.Release)
    $content = $content.Replace("{#IconFullPath}", $iconFullPath)
	$content = $content.Replace("{#SourceDir}", $Config.RepositoryRoot)
	
	$unresolvedPlaceholders =
    [regex]::Matches($content, '\{#[^}]+\}')

	if ($unresolvedPlaceholders.Count -gt 0) {

		$names =
			$unresolvedPlaceholders |
			ForEach-Object { $_.Value } |
			Sort-Object -Unique

		throw (
			"Unresolved Inno Setup placeholders: " +
			($names -join ", ")
		)
	}

    $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($Paths.InnoTemp, $content, $utf8NoBom)

    Invoke-NativeCommand `
        -Executable $Config.InnoSetupPath `
        -Arguments @($Paths.InnoTemp) `
        -Description "Creating installer with Inno Setup"

    Assert-FileExists -Path $script:InstallerPath -Description "Generated installer"

    return $script:InstallerPath
}

# =============================================================================
# Signing
# =============================================================================

function Invoke-CodeSigning {
    param(
        [Parameter(Mandatory = $true)]
        [string]$File
    )

    if ($SkipSigning) {
        Write-ReleaseLog "Skipping code signing."
        return
    }

    Assert-FileExists -Path $File -Description "Installer to sign"
    Assert-FileExists -Path $Paths.CodeSign -Description "CodeSign.ps1"

    $pwshCommand = Get-Command "pwsh.exe" -ErrorAction SilentlyContinue
    if ($pwshCommand) {
        $pwshPath = $pwshCommand.Source
    }
    else {
        $pwshPath = Join-Path $env:ProgramFiles "PowerShell\7\pwsh.exe"
    }

    Assert-FileExists -Path $pwshPath -Description "PowerShell 7"

    $versionText = & $pwshPath -NoLogo -NoProfile -NonInteractive -Command '$PSVersionTable.PSVersion.ToString()'
    if ($LASTEXITCODE -ne 0) {
        throw "Unable to determine the PowerShell version from '$pwshPath'."
    }

    $pwshVersion = [version](($versionText | Out-String).Trim())
    if ($pwshVersion.Major -lt 7) {
        throw "Code signing requires PowerShell 7 or later; found $pwshVersion."
    }

    Push-Location $Config.RepositoryRoot
    try {
        $arguments = @(
            "-NoLogo",
            "-NoProfile",
            "-NonInteractive",
            "-ExecutionPolicy", "Bypass",
            "-File", $Paths.CodeSign,
            "-file", $File
        )

        Invoke-NativeCommand `
            -Executable $pwshPath `
            -Arguments $arguments `
            -Description "Signing installer with PowerShell $pwshVersion"
    }
    finally {
        Pop-Location
    }

    Write-ReleaseLog "Installer signed successfully."
}

# =============================================================================
# Checksums and release notes
# =============================================================================

function Get-ReleaseChecksums {
    $installerHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $script:InstallerPath).Hash
    $archiveHash = (Get-FileHash -Algorithm SHA256 -LiteralPath $script:ArchivePath).Hash

    Write-ReleaseLog "Installer SHA256: $installerHash"
    Write-ReleaseLog "Archive SHA256: $archiveHash"

    return @{
        Installer = $installerHash
        Archive   = $archiveHash
    }
}

function New-ReleaseNotes {
    param(
        [Parameter(Mandatory = $true)]
        [hashtable]$Checksums
    )

    return @"
## Release Notes for $($Config.RepositoryName)_v$($script:BuildVersion)

### Commit Message
$($script:CommitMessage)

### Download Links
- [Download $($script:InstallerName)](https://github.com/$($Config.GitHubUser)/$($Config.RepositoryName)/releases/download/$($script:TagName)/$($script:InstallerName))
- [Download $($script:ArchiveName)](https://github.com/$($Config.GitHubUser)/$($Config.RepositoryName)/releases/download/$($script:TagName)/$($script:ArchiveName))

### SHA256
Installer:
```
$($Checksums.Installer)
```

Archive:
```
$($Checksums.Archive)
```
"@
}

# =============================================================================
# Publishing
# =============================================================================

function Publish-GitHubRelease {
    param(
        [Parameter(Mandatory = $true)]
        [string]$ReleaseNotes
    )

    if ($SkipGitHub) {
        Write-ReleaseLog "Skipping GitHub release publishing."
        return
    }

    $ghPath = Get-ExecutablePath -CommandName "gh.exe" -Description "GitHub CLI"

    $arguments = @(
        "release",
        "create",
        $script:TagName,
        $script:InstallerPath,
        $script:ArchivePath,
        "--title",
        "Release $($script:TagName)",
        "--notes",
        $ReleaseNotes
    )

    Invoke-NativeCommand `
        -Executable $ghPath `
        -Arguments $arguments `
        -Description "Publishing GitHub release $($script:TagName)"
}

function Publish-OneDriveArtifacts {
    if ($SkipOneDrive) {
        Write-ReleaseLog "Skipping OneDrive publishing."
        return
    }

    Assert-DirectoryExists -Path $Config.OneDriveFolder -Description "OneDrive release folder"

    $installerDestination = Join-Path $Config.OneDriveFolder $script:InstallerName
    $archiveDestination = Join-Path $Config.OneDriveFolder $script:ArchiveName

    Write-ReleaseLog "Copying release artifacts to OneDrive."

    Copy-Item -LiteralPath $script:InstallerPath -Destination $installerDestination -Force
    Copy-Item -LiteralPath $script:ArchivePath -Destination $archiveDestination -Force
}

# =============================================================================
# Cleanup
# =============================================================================

function Remove-TemporaryReleaseFiles {
    if (Test-Path -LiteralPath $Paths.InnoTemp -PathType Leaf) {
        Remove-Item -LiteralPath $Paths.InnoTemp -Force -ErrorAction SilentlyContinue
    }

    $backupPath = "$($Paths.MainCpp).release-backup"
    if (Test-Path -LiteralPath $backupPath -PathType Leaf) {
        # If a backup remains after an abnormal interruption, do not silently
        # overwrite cMain.cpp here. Log it so it can be inspected manually.
        Write-ReleaseLog "WARNING: source backup still exists: $backupPath"
    }
}

# =============================================================================
# Release pipeline
# =============================================================================

$originalLocation = Get-Location

try {
    New-Item -Path $Config.RepositoryRoot -ItemType Directory -Force | Out-Null
    New-Item -Path $Paths.Temp -ItemType Directory -Force | Out-Null

    Write-ReleaseLog "================================================================"
    Write-ReleaseLog "Starting VEXOTControl release pipeline."

    Set-Location $Config.RepositoryRoot

    Test-ReleaseEnvironment
    Initialize-ReleaseState
    Clear-PreviousReleaseArtifacts

    Invoke-VersionedBuild
    Copy-RuntimeFiles

    $null = New-ReleaseArchive
    $null = New-ReleaseInstaller

    Invoke-CodeSigning -File $script:InstallerPath

    $checksums = Get-ReleaseChecksums
    $releaseNotes = New-ReleaseNotes -Checksums $checksums

    Publish-GitHubRelease -ReleaseNotes $releaseNotes
    Publish-OneDriveArtifacts

    Write-ReleaseLog "Release $($script:BuildVersion) completed successfully."
}
catch {
    try {
        Write-ReleaseLog "RELEASE FAILED: $($_.Exception.Message)"
    }
    catch {
        Write-Host "RELEASE FAILED: $($_.Exception.Message)"
    }

    throw
}
finally {
    try {
        Remove-TemporaryReleaseFiles
    }
    catch {
        Write-Host "Cleanup warning: $($_.Exception.Message)"
    }

    Set-Location $originalLocation
}