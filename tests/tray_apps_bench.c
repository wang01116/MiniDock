/* Feature overhead benchmark. Child fixtures have hidden windows and independent EXE paths. */
#include "../MiniDock-source/minidock.c"
#include <stdlib.h>

static const WCHAR *fixtureClass = L"MiniDockTrayBenchmarkFixture";

static DWORD ThreadCount(void) {
    DWORD count = 0, pid = GetCurrentProcessId();
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return 0;
    THREADENTRY32 entry = { sizeof entry };
    if (Thread32First(snapshot, &entry)) do {
        if (entry.th32OwnerProcessID == pid) count++;
    } while (Thread32Next(snapshot, &entry));
    CloseHandle(snapshot);
    return count;
}

static ULONGLONG CpuTime(void) {
    FILETIME created, exited, kernel, user;
    GetProcessTimes(GetCurrentProcess(), &created, &exited, &kernel, &user);
    return (((ULONGLONG)kernel.dwHighDateTime << 32) | kernel.dwLowDateTime) +
           (((ULONGLONG)user.dwHighDateTime << 32) | user.dwLowDateTime);
}

static void CleanupFixtures(const WCHAR *directory, HANDLE *processes, int count) {
    for (int i = 0; i < count; i++) {
        if (processes[i]) {
            TerminateProcess(processes[i], 0);
            WaitForSingleObject(processes[i], 5000);
            CloseHandle(processes[i]);
        }
        WCHAR name[32], path[MAX_PATH];
        wsprintfW(name, L"fixture-%d.exe", i);
        PathCombineW(path, directory, name);
        DeleteFileW(path);
    }
    if (directory[0]) RemoveDirectoryW(directory);
}

int main(int argc, char **argv) {
    if (argc >= 3 && !strcmp(argv[1], "--fixture")) {
        WNDCLASSW wc = { 0 };
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpfnWndProc = DefWindowProcW;
        wc.lpszClassName = fixtureClass;
        if (!RegisterClassW(&wc)) return 1;
        WCHAR title[64];
        wsprintfW(title, L"MiniDock tray fixture %d", atoi(argv[2]));
        if (!CreateWindowExW(0, fixtureClass, title, WS_OVERLAPPEDWINDOW,
                             -30000, -30000, 600, 500, NULL, NULL, wc.hInstance, NULL)) return 1;
        Sleep(60000);
        return 0;
    }
    if (argc != 3) {
        fprintf(stderr, "Usage: tray_apps_bench.exe <0|1|16|missing> <96|256>\n");
        return 1;
    }
    BOOL missing = !strcmp(argv[1], "missing");
    int count = missing ? MAXTRAYAPPS : atoi(argv[1]);
    int cache = atoi(argv[2]);
    if (count < 0 || count > MAXTRAYAPPS || (cache != 96 && cache != 256)) return 1;
    OleInitialize(NULL);
    GdiplusStartupInput startup = { 1, NULL, FALSE, FALSE };
    if (GdiplusStartup(&g_gdipTok, &startup, NULL)) return 1;
    g_inst = GetModuleHandleW(NULL);
    g_dpi = g_dpiCfg = 96;
    g_cacheSize = cache;
    DefaultCfg();
    cfg.guardian = cfg.minAnim = cfg.statusBar = 0;
    RebuildDockFonts();
    ClearTrayApps();
    /* Model an existing Dock with 20 items. Their pre-existing caches are not counted. */
    nItems = 20;
    for (int i = 0; i < nItems; i++) items[i].type = T_FILE;
    WCHAR exe[MAX_PATH], directory[MAX_PATH] = L"", temp[MAX_PATH], folder[64];
    GetModuleFileNameW(NULL, exe, MAX_PATH);
    GpBitmap *warmIcon = ShellIconBitmap(exe, NULL, cache);
    if (warmIcon) GdipDisposeImage(warmIcon);


    HANDLE fixtureProcesses[MAXTRAYAPPS] = { 0 };
    GetTempPathW(MAX_PATH, temp);
    wsprintfW(folder, L"MiniDock-tray-bench-%lu", GetCurrentProcessId());
    PathCombineW(directory, temp, folder);
    if (!CreateDirectoryW(directory, NULL)) return 1;
    g_ntrayApps = count;
    BOOL ready = TRUE;
    for (int i = 0; i < count; i++) {
        TrayApp *app = &g_trayApps[i];
        WCHAR name[32];
        wsprintfW(name, L"fixture-%d.exe", i);
        PathCombineW(app->exe, directory, name);
        lstrcpyW(app->cls, missing ? L"Chrome_WidgetWin_1" : fixtureClass);
        wsprintfW(app->title, L"MiniDock tray fixture %d", i);
        wsprintfW(app->name, L"Fixture %d", i);
        if (missing) continue;
        if (!CopyFileW(exe, app->exe, TRUE)) { ready = FALSE; break; }
        WCHAR command[MAX_PATH + 64];
        wsprintfW(command, L"\"%s\" --fixture %d", app->exe, i);
        STARTUPINFOW si = { sizeof si };
        PROCESS_INFORMATION pi;
        if (!CreateProcessW(app->exe, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            ready = FALSE; break;
        }
        fixtureProcesses[i] = pi.hProcess;
        CloseHandle(pi.hThread);
    }
    if (!missing && ready) {
        for (int attempt = 0; attempt < 200; attempt++) {
            RefreshTrayApps();
            ready = TRUE;
            for (int i = 0; i < count; i++) if (!g_trayApps[i].hwnd) ready = FALSE;
            if (ready) break;
            Sleep(10);
        }
    }
    if (!ready) {
        ClearTrayApps();
        CleanupFixtures(directory, fixtureProcesses, count);
        fprintf(stderr, "Fixture setup failed\n");
        return 1;
    }
    /* Warm the shared Shell/GDI+ paths before measuring retained feature resources. */
    for (int i = 0; !missing && i < count; i++) {
        GpBitmap *icon = ShellIconBitmap(g_trayApps[i].exe, NULL, cache);
        if (icon) GdipDisposeImage(icon);
        if (g_trayApps[i].process) CloseHandle(g_trayApps[i].process);
        g_trayApps[i].process = NULL;
        g_trayApps[i].hwnd = NULL;
        g_trayApps[i].pid = 0;
    }
    Sleep(500);
    DWORD baseThreads = ThreadCount(), baseHandles;
    GetProcessHandleCount(GetCurrentProcess(), &baseHandles);
    PROCESS_MEMORY_COUNTERS_EX before = { sizeof before };
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&before, sizeof before);
    if (g_ntrayApps) for (int i = 0; i < 20; i++) SyncTrayApps();
    DWORD handles, threads = ThreadCount();
    GetProcessHandleCount(GetCurrentProcess(), &handles);
    PROCESS_MEMORY_COUNTERS_EX after = { sizeof after };
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&after, sizeof after);
    int iterations = missing ? 1000 : 10000;
    LARGE_INTEGER frequency, started, finished;
    QueryPerformanceFrequency(&frequency);
    ULONGLONG cpuBefore = CpuTime();
    QueryPerformanceCounter(&started);
    for (int i = 0; i < iterations; i++) if (g_ntrayApps) SyncTrayApps();
    QueryPerformanceCounter(&finished);
    ULONGLONG cpuAfter = CpuTime();
    printf("{\"selected\":%d,\"missing\":%d,\"cache\":%d,\"iterations\":%d,"
           "\"scan_us\":%.3f,\"cpu_us_per_scan\":%.3f,\"private_delta\":%lld,"
           "\"working_set_delta\":%lld,\"handle_delta\":%ld,\"thread_delta\":%ld,"
           "\"metadata_bytes\":%llu,\"icon_raw_bytes\":%llu}\n",
           count, missing, cache, iterations,
           (finished.QuadPart - started.QuadPart) * 1e6 / frequency.QuadPart / iterations,
           (cpuAfter - cpuBefore) / 10.0 / iterations,
           (long long)after.PrivateUsage - (long long)before.PrivateUsage,
           (long long)after.WorkingSetSize - (long long)before.WorkingSetSize,
           (long)handles - (long)baseHandles, (long)threads - (long)baseThreads,
           (unsigned long long)sizeof g_trayApps,
           (unsigned long long)(missing ? 0 : count) * cache * cache * 4);
    ClearTrayApps();
    for (int i = 0; i < nItems; i++) FreeItem(&items[i]);
    CleanupFixtures(directory, fixtureProcesses, count);
    return 0;
}
