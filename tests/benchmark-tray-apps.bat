@echo off
setlocal
cd /d "%~dp0"
where gcc >nul 2>nul || (echo [ERROR] MinGW-w64 gcc not found in PATH. & exit /b 1)
set "BENCH_EXE=%TEMP%\MiniDock-tray-bench-%RANDOM%-%RANDOM%.exe"
gcc -Os -s -static-libgcc -ffunction-sections -Wl,--gc-sections -o "%BENCH_EXE%" tray_apps_bench.c -lgdiplus -lole32 -loleaut32 -lshell32 -lshlwapi -luser32 -lgdi32 -lcomctl32 -lcomdlg32 -luuid -lurlmon -lversion -ldwmapi -ladvapi32 -lpsapi -luxtheme -liphlpapi -limm32 -lwtsapi32 -lpowrprof -lmsimg32 -lwinmm
if errorlevel 1 exit /b 1
for /l %%R in (1,1,3) do (
    echo Round %%R
    for %%C in (0 1 16 missing) do (
        "%BENCH_EXE%" %%C 96
        if errorlevel 1 goto failure
    )
    for %%C in (1 16) do (
        "%BENCH_EXE%" %%C 256
        if errorlevel 1 goto failure
    )
)
del /q "%BENCH_EXE%" >nul 2>nul
exit /b 0
:failure
del /q "%BENCH_EXE%" >nul 2>nul
exit /b 1
