#
# Copyright 2025      Rigaku Americas
#                     9009 New Trails Drive
#                     The Woodlands, TX, USA  77381
#
# The contents are unpublished proprietary source
# code of Rigaku Americas.
#
# All rights reserved.
#
# Powershell script for signing files using Azure Trusted Signing.
#
#
#
param(
    [string]$file = "",
    [string]$file1 = "",
    [string]$file2 = "",
    [string]$file3 = "",
    [string]$file4 = "",
    [string]$file5 = "",
    [boolean]$login = $true,

    # Zero means retry indefinitely. Press Ctrl+C to cancel the script.
    [ValidateRange(0, [int]::MaxValue)]
    [int]$MaxAttempts = 10,

    [ValidateRange(1, 3600)]
    [int]$RetryDelaySeconds = 30
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Write-SigningLog {
    param([Parameter(Mandatory = $true)][string]$Message)

    Write-Host "[$(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')] $Message"
}

if ([string]::IsNullOrWhiteSpace($file)) {
    Write-Error "Usage: CodeSign.ps1 -file <path to file to sign>"
    exit 1
}

$filesToSign = @($file, $file1, $file2, $file3, $file4, $file5) |
    Where-Object { -not [string]::IsNullOrWhiteSpace($_) } |
    ForEach-Object { (Resolve-Path -LiteralPath $_ -ErrorAction Stop).Path }

$metadataPath = Join-Path $PSScriptRoot "CodeSignMetadata.json"
$loginDataPath = Join-Path $PSScriptRoot "CodeSignLoginData.json"
$dlibPath = Join-Path $env:LOCALAPPDATA "Microsoft\MicrosoftTrustedSigningClientTools\Azure.CodeSigning.Dlib.dll"
$kitPath = "C:\Program Files (x86)\Windows Kits\10\bin"

foreach ($requiredFile in @($metadataPath, $dlibPath)) {
    if (-not (Test-Path -LiteralPath $requiredFile -PathType Leaf)) {
        Write-Error "Required signing file was not found: $requiredFile"
        exit 1
    }
}

$signtool = Get-ChildItem -Path $kitPath -Recurse -File -Filter "signtool.exe" -ErrorAction Stop |
    Where-Object { $_.FullName -like "*\x64\*" } |
    Sort-Object -Property FullName -Descending |
    Select-Object -First 1

if (-not $signtool) {
    Write-Error "No x64 signtool.exe was found below '$kitPath'."
    exit 1
}

$tenantId = "fffbbdfd-299d-49d6-ba56-a87bb9a3c67e"
$loginData = $null
$credential = $null
$connected = $false
$signingSucceeded = $false

try {
    if ($login) {
        if (-not (Test-Path -LiteralPath $loginDataPath -PathType Leaf)) {
            throw "Code-signing login data was not found: $loginDataPath"
        }

        $loginData = Get-Content -LiteralPath $loginDataPath -Raw | ConvertFrom-Json
        if ([string]::IsNullOrWhiteSpace($loginData.appId) -or
            [string]::IsNullOrWhiteSpace($loginData.secretText)) {
            throw "CodeSignLoginData.json must contain non-empty appId and secretText values."
        }

        $secureSecret = ConvertTo-SecureString $loginData.secretText -AsPlainText -Force
        $credential = [System.Management.Automation.PSCredential]::new($loginData.appId, $secureSecret)
    }

    Write-SigningLog "Using signing tool: $($signtool.FullName)"
    $signtoolPath = $signtool.FullName

    $attempt = 0
    while ($true) {
        $attempt++
        $attemptDescription = if ($MaxAttempts -eq 0) {
            "attempt $attempt"
        }
        else {
            "attempt $attempt of $MaxAttempts"
        }

        Write-SigningLog "Starting signing $attemptDescription."

        try {
            if ($login -and -not $connected) {
                Write-SigningLog "Connecting to Azure for code signing."
                Connect-AzAccount -Tenant $tenantId -Credential $credential -ServicePrincipal -ErrorAction Stop | Out-Null
                $connected = $true
            }

            $signArguments = @(
                "sign", "/v", "/debug", "/fd", "SHA256",
                "/tr", "http://timestamp.acs.microsoft.com",
                "/td", "SHA256",
                "/dlib", $dlibPath,
                "/dmdf", $metadataPath
            ) + $filesToSign

            & $signtoolPath @signArguments
            $signExitCode = $LASTEXITCODE

            if ($signExitCode -eq 0) {
                $allSignaturesValid = $true

                foreach ($signedFile in $filesToSign) {
                    & $signtoolPath "verify" "/pa" "/v" $signedFile
                    $verifyExitCode = $LASTEXITCODE

                    if ($verifyExitCode -ne 0) {
                        Write-SigningLog "Signature verification failed for '$signedFile' with exit code $verifyExitCode."
                        $allSignaturesValid = $false
                        break
                    }
                }

                if ($allSignaturesValid) {
                    $signingSucceeded = $true
                    Write-SigningLog "Signing and signature verification completed successfully."
                    break
                }
            }
            else {
                Write-SigningLog "signtool failed with exit code $signExitCode."
            }
        }
        catch {
            Write-SigningLog "Signing attempt failed: $($_.Exception.Message)"
        }
        finally {
            # Obtain a fresh Azure session on every retry. This also recovers
            # from expired or otherwise unusable authentication contexts.
            if (-not $signingSucceeded -and $connected) {
                Disconnect-AzAccount -TenantId $tenantId -ApplicationId $loginData.appId -ErrorAction SilentlyContinue | Out-Null
                $connected = $false
            }
        }

        if ($MaxAttempts -gt 0 -and $attempt -ge $MaxAttempts) {
            throw "Code signing failed after $attempt attempt(s)."
        }

        Write-SigningLog "The next signing attempt will start in $RetryDelaySeconds second(s). Press Ctrl+C to cancel."
        Start-Sleep -Seconds $RetryDelaySeconds
    }
}
catch {
    Write-Error "Code signing failed: $($_.Exception.Message)"
}
finally {
    if ($connected) {
        Disconnect-AzAccount -TenantId $tenantId -ApplicationId $loginData.appId -ErrorAction SilentlyContinue | Out-Null
    }
}

if ($signingSucceeded) {
    exit 0
}

exit 1
