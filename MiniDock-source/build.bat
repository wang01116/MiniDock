@echo off
setlocal
cd /d "%~dp0"
where gcc >nul 2>nul || (echo [ERROR] MinGW-w64 gcc not found in PATH. & exit /b 1)
where g++ >nul 2>nul || (echo [ERROR] MinGW-w64 g++ not found in PATH. & exit /b 1)
where windres >nul 2>nul || (echo [ERROR] MinGW-w64 windres not found in PATH. & exit /b 1)
windres -O coff res/app.rc -o app_res.o -Ires
if errorlevel 1 (echo [ERROR] Resource compilation failed. & exit /b 1)
gcc -c -Os -fno-ident -mwindows minidock.c -o minidock.o
if errorlevel 1 (echo [ERROR] MiniDock compilation failed. & exit /b 1)
gcc -c -Os -mwindows wgc_capture.c -o wgc_capture.o
if errorlevel 1 (echo [ERROR] WGC capture compilation failed. & exit /b 1)
g++ -c -Os -mwindows hdr_snap.cpp -o hdr_snap.o
if errorlevel 1 (echo [ERROR] HDR snapshot compilation failed. & exit /b 1)
g++ -o MiniDock.exe minidock.o wgc_capture.o hdr_snap.o app_res.o -mwindows -s -static-libgcc -static-libstdc++ -Wl,--gc-sections,--file-alignment=512 -lgdiplus -lole32 -loleaut32 -lshell32 -lshlwapi -luser32 -lgdi32 -lcomctl32 -lcomdlg32 -luuid -lurlmon -lversion -ldwmapi -ladvapi32 -lpsapi -luxtheme -liphlpapi -limm32 -lwtsapi32 -lpowrprof -lmsimg32 -lwinmm -lwindowsapp -lruntimeobject -ld3d11 -ldxgi -ldcomp
if errorlevel 1 (echo [ERROR] Link failed. & exit /b 1)
del /q app_res.o minidock.o wgc_capture.o hdr_snap.o >nul 2>nul
echo [OK] MiniDock.exe built successfully.
endlocal
