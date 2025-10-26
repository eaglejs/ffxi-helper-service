@echo off
cd /d "%~dp0build"
echo.
echo ========================================
echo   FFXI Helper Service with Chat Monitor
echo ========================================
echo.
echo Starting service...
echo.
FFXIHelperService_d.exe
pause
