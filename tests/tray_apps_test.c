#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
/* Make foreground inputs deterministic while exercising real Win32 window states. */
static HWND fixtureForeground;
static HWND TestGetForegroundWindow(void) { return fixtureForeground; }
static BOOL TestSetForegroundWindow(HWND hwnd) { fixtureForeground = hwnd; return TRUE; }
#define GetForegroundWindow TestGetForegroundWindow
#define SetForegroundWindow TestSetForegroundWindow
#include "../MiniDock-source/minidock.c"
#include <stdlib.h>

static int checks;
#define CHECK(condition, label) do { checks++; if (!(condition)) { fprintf(stderr, "FAIL %d: %s\n", checks, label); return 1; } } while (0)
static const WCHAR *testClass = L"MiniDockTrayRegressionWindow";
static HWND TestWindow(const WCHAR *title, int width, int height) {
    return CreateWindowExW(WS_EX_TOOLWINDOW, testClass, title, WS_OVERLAPPEDWINDOW, -30000, -30000, width, height, NULL, NULL, GetModuleHandleW(NULL), NULL);
}
/* Exercise asynchronous restore requests without exposing fixtures to other Dock scanners. */
static void DrainFixtureMessages(void) {
    MSG message;
    for (int pass = 0; pass < 10; pass++) {
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE)) { TranslateMessage(&message); DispatchMessageW(&message); }
        Sleep(1);
    }
}
static int CountType(int type) {
    int count = 0;
    for (int i = 0; i < nItems; i++) if (items[i].type == type) count++;
    return count;
}
static void ClearItems(void) {
    for (int i = 0; i < nItems; i++) FreeItem(&items[i]);
    nItems = 0;
}
int main(int argc, char **argv) {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = testClass;
    CHECK(RegisterClassW(&wc), "register fixture window class");
    if (argc > 1 && !strcmp(argv[1], "--tray-test-child")) {
        TestWindow(L"Tray Feature Child", 600, 500);
        Sleep(30000);
        return 0;
    }
    OleInitialize(NULL);
    GdiplusStartupInput gi = {1, NULL, FALSE, FALSE};
    CHECK(GdiplusStartup(&g_gdipTok, &gi, NULL) == 0, "start image library");
    g_inst = GetModuleHandleW(NULL);
    g_dpi = g_dpiCfg = 96;
    DefaultCfg();
    cfg.minAnim = cfg.statusBar = cfg.guardian = 0;
    cfg.showRunning = 0;
    RebuildDockFonts();
    WCHAR temp[MAX_PATH], filename[64], exe[MAX_PATH];
    GetTempPathW(MAX_PATH, temp);
    wsprintfW(filename, L"minidock-tray-regression-%lu.ini", GetCurrentProcessId());
    PathCombineW(g_ini, temp, filename);
    GetModuleFileNameW(NULL, exe, MAX_PATH);
    LoadTrayApps();
    CHECK(g_ntrayApps == 0, "legacy config starts with an empty selection");
    CHECK(!SyncTrayApps() && nItems == 0, "empty selection adds no section");

    HWND mainWindow = TestWindow(L"Tray Feature Main", 600, 500);
    HWND helper = TestWindow(L"Tray Feature Login Helper", 1200, 1000);
    CHECK(mainWindow && helper, "create main and larger helper windows");
    TrayApp *a = &g_trayApps[0];
    lstrcpyW(a->exe, exe);
    lstrcpyW(a->cls, testClass);
    lstrcpyW(a->title, L"Tray Feature Main");
    lstrcpyW(a->name, L"Fixture App");
    a->pid = GetCurrentProcessId();
    a->hwnd = mainWindow;
    a->process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, a->pid);
    g_ntrayApps = 1;
    CHECK(SyncTrayApps(), "selected running app creates a runtime icon");
    CHECK(nItems == 2 && items[0].type == T_SEP && items[0].appmode == TRAY_SEP && items[1].type == T_TRAY, "dedicated section exists with ordinary running-app display disabled");
    CHECK(items[1].temp && items[1].running && items[1].hwnd == mainWindow, "runtime icon binds to the selected window");
    CHECK(!IsWindowVisible(mainWindow) && !SyncTrayApps() && CountType(T_TRAY) == 1, "hidden app stays in the section");
    RECT testWorkArea;
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &testWorkArea, 0);
    SetWindowPos(mainWindow, NULL, testWorkArea.left + 20, testWorkArea.top + 20, 600, 500, SWP_NOACTIVATE | SWP_NOZORDER);
    RECT before, after;
    GetWindowRect(mainWindow, &before);
    LaunchItem(1, FALSE);
    DrainFixtureMessages();
    GetWindowRect(mainWindow, &after);
    CHECK(IsWindowVisible(mainWindow) && !IsWindowVisible(helper), "normal icon entry restores main window and keeps helper hidden");
    CHECK(EqualRect(&before, &after), "restoring preserves the main window rectangle");
    SetForegroundWindow(mainWindow);
    CHECK(GetForegroundWindow() == mainWindow, "fixture main window is foreground before repeated click");
    LaunchItem(1, FALSE);
    DrainFixtureMessages();
    CHECK(IsIconic(mainWindow), "second icon click minimizes the foreground main window");
    CHECK(CountType(T_TRAY) == 1, "minimized app retains its runtime icon");
    LaunchItem(1, FALSE);
    DrainFixtureMessages();
    CHECK(IsWindowVisible(mainWindow) && !IsIconic(mainWindow), "third icon click restores the minimized main window");
    cfg.clickToggle = 0;
    SetForegroundWindow(mainWindow);
    LaunchItem(1, FALSE);
    DrainFixtureMessages();
    CHECK(!IsIconic(mainWindow), "disabling click-toggle keeps foreground apps visible");
    cfg.clickToggle = 1;
    ShowWindow(helper, SW_SHOW);
    SetForegroundWindow(helper);
    DrainFixtureMessages();
    SetForegroundWindow(helper);
    CHECK(GetForegroundWindow() == helper, "fixture helper is foreground for background-app test");
    LaunchItem(1, FALSE);
    DrainFixtureMessages();
    CHECK(!IsIconic(mainWindow) && GetForegroundWindow() == mainWindow, "background main window is brought forward instead of minimized");
    ShowWindow(helper, SW_HIDE);
    GetWindowRect(mainWindow, &after);
    CHECK(EqualRect(&before, &after), "repeated minimize/restore preserves original window rectangle");
    SetWindowTextW(mainWindow, L"Tray Feature Updated Main");
    RefreshTrayApps();
    CHECK(!lstrcmpW(a->title, L"Tray Feature Updated Main"), "visible title changes update the saved identity");
    ShowWindow(mainWindow, SW_HIDE);
    SaveNow();
    CHECK(GetPrivateProfileIntW(L"TrayApps", L"Count", 0, g_ini) == 1, "selection persists in config");
    CHECK(GetPrivateProfileIntW(L"Items", L"Count", 99, g_ini) == 0, "runtime icons are excluded from pinned items");
    LoadTrayApps();
    RefreshTrayApps();
    CHECK(g_ntrayApps == 1 && g_trayApps[0].hwnd == mainWindow, "reload binds an already hidden main window");
    CHECK(g_trayApps[0].hwnd != helper, "reload rejects the larger login/helper window");
    DestroyWindow(mainWindow);
    RefreshTrayApps();
    CHECK(TrayProcessAlive(&g_trayApps[0]) && !g_trayApps[0].hwnd, "destroyed window retains process tracking");
    SyncTrayApps();
    CHECK(CountType(T_TRAY) == 1 && !items[nItems - 1].hwnd, "runtime icon survives HWND destruction");
    mainWindow = TestWindow(L"Tray Feature Updated Main", 600, 500);
    RefreshTrayApps();
    CHECK(g_trayApps[0].hwnd == mainWindow, "recreated main window is rebound");

    Item pinned;
    ZeroMemory(&pinned, sizeof pinned);
    pinned.type = T_FILE;
    lstrcpyW(pinned.name, L"Pinned fixture");
    InsertItemStruct(0, &pinned, FALSE);
    ScanCtx ctx;
    ZeroMemory(&ctx, sizeof ctx);
    cfg.showRunning = 1;
    ctx.nex = 1;
    GetSystemDirectoryW(g_scanEx[0], MAX_PATH);
    PathAppendW(g_scanEx[0], L"cmd.exe");
    SyncTemp(&ctx);
    CHECK(items[nItems - 1].type == T_TRAY && items[0].temp == FALSE, "ordinary running apps are inserted before tray apps");
    cfg.multiMode = 1;
    ctx.nwin = 1;
    ctx.win[0].h = helper;
    lstrcpyW(ctx.win[0].exe, exe);
    lstrcpyW(ctx.win[0].title, L"Document fixture");
    SyncWin(&ctx);
    CHECK(CountType(T_WIN) == 1 && items[nItems - 1].type == T_TRAY, "document windows are inserted before tray apps");
    BuildRows();
    BOOL addRow = FALSE, removeRow = FALSE;
    for (int i = 0; i < g_nrow; i++) {
        if (g_row[i].act == A_TRAYADD) addRow = TRUE;
        if (g_row[i].act == A_TRAYREMOVE && wcsstr(g_row[i].label, L"Fixture App")) removeRow = TRUE;
    }
    CHECK(addRow && removeRow && g_nrow < 620, "settings expose add/remove actions within row capacity");
    RemoveTrayApp(0);
    CHECK(!CountType(T_TRAY) && TraySectionStart() == nItems, "removing the last selection cleans up before the empty-list scan fast path");
    SyncTrayApps();
    CHECK(!g_ntrayApps && !CountType(T_TRAY), "removing a selection removes its runtime icon");
    CHECK(CountType(T_FILE) == 2 && CountType(T_WIN) == 1, "removal preserves pinned and ordinary runtime entries");
    BOOL trayDivider = FALSE;
    for (int i = 0; i < nItems; i++) if (items[i].type == T_SEP && items[i].appmode == TRAY_SEP) trayDivider = TRUE;
    CHECK(!trayDivider, "empty tray section drops its divider");
    ClearItems();
    DestroyWindow(mainWindow);
    DestroyWindow(helper);

    STARTUPINFOW si = {sizeof si};
    PROCESS_INFORMATION pi;
    WCHAR command[MAX_PATH + 40];
    _snwprintf(command, MAX_PATH + 39, L"\"%s\" --tray-test-child", exe);
    CHECK(CreateProcessW(exe, command, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi), "start isolated child-process fixture");
    ZeroMemory(&g_trayApps[0], sizeof(TrayApp));
    a = &g_trayApps[0];
    lstrcpyW(a->exe, exe);
    lstrcpyW(a->cls, testClass);
    lstrcpyW(a->title, L"Tray Feature Child");
    lstrcpyW(a->name, L"Child Fixture App");
    g_ntrayApps = 1;
    for (int i = 0; i < 100 && !a->hwnd; i++) { RefreshTrayApps(); Sleep(10); }
    CHECK(a->hwnd && a->pid == pi.dwProcessId, "bind to the correct child process");
    SyncTrayApps();
    CHECK(CountType(T_TRAY) == 1, "child process has one runtime icon");
    CHECK(TerminateProcess(pi.hProcess, 0), "stop isolated child-process fixture");
    CHECK(WaitForSingleObject(pi.hProcess, 5000) == WAIT_OBJECT_0, "wait for fixture exit");
    SyncTrayApps();
    CHECK(!CountType(T_TRAY) && nItems == 0, "actual process exit removes the icon and divider");
    CHECK(g_ntrayApps == 1, "app selection survives process exit");
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    ClearTrayApps();
    WritePrivateProfileStringW(L"TrayApps", L"Count", L"-1", g_ini);
    LoadTrayApps();
    CHECK(g_ntrayApps == 0, "negative config count is clamped");
    DeleteFileW(g_ini);
    printf("PASS %d checks\n", checks);
    return 0;
}
