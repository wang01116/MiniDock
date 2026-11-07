@echo off
setlocal
cd /d "%~dp0"
where gcc >nul 2>nul || (echo [ERROR] MinGW-w64 gcc not found in PATH. & exit /b 1)
where windres >nul 2>nul || (echo [ERROR] MinGW-w64 windres not found in PATH. & exit /b 1)
windres -O coff res/app.rc -o app_res.o -Ires
if errorlevel 1 (echo [ERROR] Resource compilation failed. & exit /b 1)
gcc -Os -fno-ident -mwindows -s -static-libgcc -Wl,--gc-sections,--file-alignment=512 -o MiniDock.exe minidock.c app_res.o -lgdiplus -lole32 -loleaut32 -lshell32 -lshlwapi -luser32 -lgdi32 -lcomctl32 -lcomdlg32 -luuid -lurlmon -lversion -ldwmapi -ladvapi32 -lpsapi -luxtheme -liphlpapi -limm32 -lwtsapi32 -lpowrprof -lmsimg32 -lwinmm
if errorlevel 1 (echo [ERROR] MiniDock compilation failed. & exit /b 1)
del /q app_res.o >nul 2>nul
echo [OK] MiniDock.exe built successfully.
endlocal
