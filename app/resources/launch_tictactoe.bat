@echo off
setlocal enabledelayedexpansion

:: Tic-Tac-Toe Windows Launcher Script
:: This script helps diagnose and fix common startup issues

echo ===============================================
echo     Tic-Tac-Toe Game Launcher
echo ===============================================
echo.

:: Check if we're in the correct directory
if not exist "tictactoe_app.exe" (
    echo ERROR: tictactoe_app.exe not found in current directory!
    echo Please make sure you're running this script from the game folder.
    echo.
    pause
    exit /b 1
)

:: Check for required DLLs
set "missing_dlls="
set "required_dlls=Qt6Core.dll Qt6Gui.dll Qt6Widgets.dll sqlite3.dll"

echo Checking for required files...
for %%d in (%required_dlls%) do (
    if not exist "%%d" (
        set "missing_dlls=!missing_dlls! %%d"
    ) else (
        echo   ✓ Found %%d
    )
)

:: Check for OpenSSL DLLs (multiple possible names)
set "openssl_found=0"
if exist "libcrypto-3-x64.dll" (
    echo   ✓ Found libcrypto-3-x64.dll
    set "openssl_found=1"
)
if exist "libcrypto-1_1-x64.dll" (
    echo   ✓ Found libcrypto-1_1-x64.dll
    set "openssl_found=1"
)
if exist "libssl-3-x64.dll" (
    echo   ✓ Found libssl-3-x64.dll
)
if exist "libssl-1_1-x64.dll" (
    echo   ✓ Found libssl-1_1-x64.dll
)

if !openssl_found! == 0 (
    set "missing_dlls=!missing_dlls! libcrypto-3-x64.dll"
)

:: Check for platforms directory
if not exist "platforms" (
    echo   ⚠ Warning: platforms directory not found
    echo     This may cause Qt platform plugin issues
) else (
    echo   ✓ Found platforms directory
)

:: Report missing DLLs
if not "!missing_dlls!" == "" (
    echo.
    echo ERROR: Missing required DLL files:
    for %%d in (!missing_dlls!) do (
        echo   ✗ %%d
    )
    echo.
    echo Please ensure all files were extracted from the ZIP archive.
    echo You may need to install Visual C++ Redistributable 2019 or later.
    echo.
    echo Would you like to try launching anyway? (y/n)
    set /p "choice="
    if /i not "!choice!" == "y" (
        pause
        exit /b 1
    )
)

echo.
echo All checks passed! Launching Tic-Tac-Toe...
echo.

:: Set Qt plugin path to current directory
set "QT_PLUGIN_PATH=%CD%"

:: Launch the game and capture any error
start "" "tictactoe_app.exe"

:: Check if the process started successfully
timeout /t 2 >nul
tasklist /fi "imagename eq tictactoe_app.exe" 2>nul | find /i "tictactoe_app.exe" >nul
if errorlevel 1 (
    echo.
    echo WARNING: The application may not have started correctly.
    echo.
    echo Troubleshooting suggestions:
    echo 1. Try running as Administrator (right-click this file and "Run as administrator")
    echo 2. Disable antivirus temporarily
    echo 3. Install Visual C++ Redistributable from Microsoft
    echo 4. Check Windows Event Viewer for detailed error messages
    echo.
    echo Would you like to try the console version instead? (y/n)
    set /p "console_choice="
    if /i "!console_choice!" == "y" (
        if exist "tictactoe_console.exe" (
            echo.
            echo Launching console version...
            "tictactoe_console.exe"
        ) else (
            echo Console version not found!
        )
    )
) else (
    echo Application started successfully!
)

echo.
echo Launcher script completed.
pause
