@echo off
REM Windows batch wrapper for font generation in WSL
REM This script automatically runs the font generator in WSL2

echo ==========================================
echo ESP32 IPA Font Generator (Windows)
echo ==========================================
echo.

REM Check if WSL is available
wsl --list --quiet >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: WSL is not available!
    echo.
    echo Please install WSL2:
    echo 1. Open PowerShell as Administrator
    echo 2. Run: wsl --install
    echo 3. Restart your computer
    echo 4. Run this script again
    echo.
    pause
    exit /b 1
)

echo WSL detected. Starting font generation...
echo.

REM Get current directory in WSL format
for /f %%i in ('wsl wslpath "%CD%"') do set WSL_DIR=%%i

echo Working directory: %WSL_DIR%
echo.

REM Run the setup script in WSL
wsl bash -c "cd '%WSL_DIR%' && chmod +x setup_and_generate_fonts.sh && ./setup_and_generate_fonts.sh"

if %errorlevel% equ 0 (
    echo.
    echo ==========================================
    echo SUCCESS! Fonts generated successfully!
    echo ==========================================
    echo.
    echo The IPA fonts are now available in:
    echo   include\fonts\noto_sans_ipa_12.h
    echo   include\fonts\noto_sans_ipa_16.h
    echo.
    echo You can now build your ESP-IDF project with full IPA support!
    echo.
) else (
    echo.
    echo ==========================================
    echo ERROR: Font generation failed!
    echo ==========================================
    echo.
    echo Please check the error messages above.
    echo.
)

pause
