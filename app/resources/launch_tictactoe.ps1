# Tic-Tac-Toe PowerShell Launcher
# This script provides enhanced error handling and diagnostics for Windows

param(
    [switch]$Console,
    [switch]$Verbose,
    [switch]$SkipChecks
)

# Set up error handling
$ErrorActionPreference = "Stop"

function Write-Header {
    Write-Host "===============================================" -ForegroundColor Cyan
    Write-Host "     Tic-Tac-Toe Game Launcher (PowerShell)" -ForegroundColor Cyan
    Write-Host "===============================================" -ForegroundColor Cyan
    Write-Host ""
}

function Write-Success {
    param([string]$Message)
    Write-Host "  ✓ $Message" -ForegroundColor Green
}

function Write-Warning {
    param([string]$Message)
    Write-Host "  ⚠ $Message" -ForegroundColor Yellow
}

function Write-Error {
    param([string]$Message)
    Write-Host "  ✗ $Message" -ForegroundColor Red
}

function Test-RequiredFiles {
    Write-Host "Checking for required files..." -ForegroundColor White

    $missingFiles = @()
    $requiredFiles = @(
        "tictactoe_app.exe",
        "Qt6Core.dll",
        "Qt6Gui.dll",
        "Qt6Widgets.dll",
        "sqlite3.dll"
    )

    foreach ($file in $requiredFiles) {
        if (Test-Path $file) {
            Write-Success "Found $file"
        } else {
            Write-Error "Missing $file"
            $missingFiles += $file
        }
    }

    # Check for OpenSSL DLLs (multiple possible names)
    $opensslFound = $false
    $opensslFiles = @("libcrypto-3-x64.dll", "libcrypto-1_1-x64.dll")

    foreach ($file in $opensslFiles) {
        if (Test-Path $file) {
            Write-Success "Found $file"
            $opensslFound = $true
            break
        }
    }

    if (-not $opensslFound) {
        Write-Error "Missing OpenSSL crypto library"
        $missingFiles += "libcrypto-3-x64.dll (or similar)"
    }

    # Check for platforms directory
    if (Test-Path "platforms") {
        Write-Success "Found platforms directory"
    } else {
        Write-Warning "platforms directory not found - may cause Qt issues"
    }

    return $missingFiles
}

function Test-SystemRequirements {
    if ($Verbose) {
        Write-Host "`nChecking system requirements..." -ForegroundColor White

        # Check Windows version
        $osVersion = [System.Environment]::OSVersion.Version
        if ($osVersion.Major -ge 10) {
            Write-Success "Windows version: $($osVersion.Major).$($osVersion.Minor) (compatible)"
        } else {
            Write-Warning "Windows version: $($osVersion.Major).$($osVersion.Minor) (may not be fully supported)"
        }

        # Check available memory
        $memory = Get-CimInstance -ClassName Win32_ComputerSystem | Select-Object -ExpandProperty TotalPhysicalMemory
        $memoryGB = [math]::Round($memory / 1GB, 1)
        if ($memoryGB -ge 2) {
            Write-Success "Available RAM: $memoryGB GB"
        } else {
            Write-Warning "Available RAM: $memoryGB GB (minimum 2 GB recommended)"
        }

        # Check architecture
        $arch = (Get-CimInstance -ClassName Win32_Processor | Select-Object -First 1).Architecture
        if ($arch -eq 9) {  # x64
            Write-Success "Architecture: x64 (compatible)"
        } else {
            Write-Warning "Architecture: Not x64 (may cause compatibility issues)"
        }
    }
}

function Start-TicTacToe {
    param([bool]$UseConsole)

    if ($UseConsole) {
        if (Test-Path "tictactoe_console.exe") {
            Write-Host "`nLaunching console version..." -ForegroundColor Green
            & ".\tictactoe_console.exe"
        } else {
            Write-Error "Console version not found!"
            return $false
        }
    } else {
        Write-Host "`nLaunching GUI version..." -ForegroundColor Green

        # Set Qt plugin path
        $env:QT_PLUGIN_PATH = $PWD.Path

        try {
            # Start the process and get handle
            $process = Start-Process -FilePath ".\tictactoe_app.exe" -PassThru -WindowStyle Normal

            # Wait a moment and check if it's still running
            Start-Sleep -Seconds 2

            if ($process.HasExited) {
                Write-Error "Application exited immediately with code: $($process.ExitCode)"
                return $false
            } else {
                Write-Success "Application started successfully! (PID: $($process.Id))"
                return $true
            }
        } catch {
            Write-Error "Failed to start application: $($_.Exception.Message)"
            return $false
        }
    }

    return $true
}

function Show-TroubleshootingHelp {
    Write-Host "`n" -NoNewline
    Write-Host "Troubleshooting Suggestions:" -ForegroundColor Yellow
    Write-Host "1. Run PowerShell as Administrator" -ForegroundColor White
    Write-Host "2. Install Visual C++ Redistributable 2019+ from Microsoft" -ForegroundColor White
    Write-Host "3. Temporarily disable antivirus software" -ForegroundColor White
    Write-Host "4. Ensure Windows is up to date" -ForegroundColor White
    Write-Host "5. Check Windows Event Viewer for detailed errors" -ForegroundColor White
    Write-Host "6. Verify all files were extracted from the ZIP" -ForegroundColor White
    Write-Host ""
    Write-Host "For more help, see README.md" -ForegroundColor Cyan
}

function Show-Usage {
    Write-Host "Usage: .\launch_tictactoe.ps1 [options]" -ForegroundColor Cyan
    Write-Host ""
    Write-Host "Options:" -ForegroundColor White
    Write-Host "  -Console     Launch console version instead of GUI" -ForegroundColor Gray
    Write-Host "  -Verbose     Show detailed system information" -ForegroundColor Gray
    Write-Host "  -SkipChecks  Skip file verification (faster startup)" -ForegroundColor Gray
    Write-Host ""
    Write-Host "Examples:" -ForegroundColor White
    Write-Host "  .\launch_tictactoe.ps1              # Launch GUI version" -ForegroundColor Gray
    Write-Host "  .\launch_tictactoe.ps1 -Console     # Launch console version" -ForegroundColor Gray
    Write-Host "  .\launch_tictactoe.ps1 -Verbose     # Show system info" -ForegroundColor Gray
}

# Main execution
try {
    Write-Header

    # Check if we're in the right directory
    if (-not (Test-Path "tictactoe_app.exe") -and -not $Console) {
        Write-Error "tictactoe_app.exe not found in current directory!"
        Write-Host "Please run this script from the game installation folder." -ForegroundColor Yellow
        exit 1
    }

    # Show usage if requested
    if ($args -contains "-help" -or $args -contains "--help" -or $args -contains "/?") {
        Show-Usage
        exit 0
    }

    # Run system checks
    Test-SystemRequirements

    # Check required files unless skipped
    if (-not $SkipChecks) {
        $missingFiles = Test-RequiredFiles

        if ($missingFiles.Count -gt 0) {
            Write-Host "`nMissing required files:" -ForegroundColor Red
            foreach ($file in $missingFiles) {
                Write-Error $file
            }

            $continue = Read-Host "`nTry to launch anyway? (y/N)"
            if ($continue -ne "y" -and $continue -ne "Y") {
                Show-TroubleshootingHelp
                exit 1
            }
        }
    }

    # Launch the appropriate version
    $success = Start-TicTacToe -UseConsole $Console

    if (-not $success) {
        Write-Host "`nLaunch failed!" -ForegroundColor Red

        if (-not $Console) {
            $fallback = Read-Host "Try console version instead? (y/N)"
            if ($fallback -eq "y" -or $fallback -eq "Y") {
                Start-TicTacToe -UseConsole $true
            }
        }

        Show-TroubleshootingHelp
        exit 1
    }

} catch {
    Write-Host "`nUnexpected error: $($_.Exception.Message)" -ForegroundColor Red
    Show-TroubleshootingHelp
    exit 1
}

Write-Host "`nLauncher completed successfully!" -ForegroundColor Green
Write-Host "Press any key to exit..."
$null = $Host.UI.RawUI.ReadKey("NoEcho,IncludeKeyDown")
