@echo off
setlocal
cd /d "%~dp0"
where gcc >nul 2>nul || (echo [ERROR] MinGW-w64 gcc not found in PATH. & exit /b 1)
set "TEST_EXE=%TEMP%\MiniDock-favicon-test-%RANDOM%-%RANDOM%.exe"
gcc -O1 -ffunction-sections -Wl,--gc-sections -o "%TEST_EXE%" favicon_download_test.c -lgdiplus -lole32 -loleaut32 -lshell32 -lshlwapi -luser32 -lgdi32 -lcomctl32 -lcomdlg32 -luuid -lurlmon -lversion -ldwmapi -ladvapi32 -lpsapi -luxtheme -liphlpapi -limm32 -lwtsapi32 -lpowrprof -lmsimg32 -lwinmm -lwinhttp
if errorlevel 1 exit /b 1
"%TEST_EXE%"
set "TEST_RESULT=%ERRORLEVEL%"
del /q "%TEST_EXE%" >nul 2>nul
exit /b %TEST_RESULT%
