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
static SHORT fixtureShiftState;
static SHORT TestGetKeyState(int key) { return key == VK_SHIFT ? fixtureShiftState : GetKeyState(key); }
static int launchRequests;
static BOOL TestQueueUserWorkItem(LPTHREAD_START_ROUTINE callback, PVOID context, ULONG flags) {
    (void)callback; (void)flags;
    launchRequests++;
    HeapFree(GetProcessHeap(), 0, context);
    return TRUE;
}
#define GetForegroundWindow TestGetForegroundWindow
#define SetForegroundWindow TestSetForegroundWindow
#define GetKeyState TestGetKeyState
#define QueueUserWorkItem TestQueueUserWorkItem
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
/* A separately running Dock may animate normal-style fixtures. Observe the final state,
   rather than the brief restore that its system-minimize animation uses. */
static BOOL WaitFixtureIconic(HWND window, BOOL iconic) {
    DWORD started = GetTickCount(), stable = 0;
    while (GetTickCount() - started < 2500) {
        DrainFixtureMessages();
        if (!!IsIconic(window) == !!iconic) {
            if (!stable) stable = GetTickCount();
            if (GetTickCount() - stable >= 150) return TRUE;
        } else stable = 0;
    }
    return FALSE;
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
/* Dispatch both mouse messages through the production Dock procedure. */
static void ClickItem(int index, WPARAM modifiers) {
    M.g = 0; M.Z = 48; barTop = 0; LH = 100; gHideCur = 0;
    for (int i = 0; i < nItems; i++) {
        slotL[i] = 10 + i * 60; slotR[i] = slotL[i] + 48; items[i].cur = 48;
    }
    LPARAM point = MAKELPARAM((int)slotL[index] + 24, 50);
    DockProc(g_hwnd, WM_LBUTTONDOWN, modifiers | MK_LBUTTON, point);
    DockProc(g_hwnd, WM_LBUTTONUP, modifiers, point);
    DrainFixtureMessages();
}
static void ScanGroupFixtures(HWND first, HWND second) {
    ScanCtx scan = {0};
    if (first && IsWindow(first)) ScanProc(first, (LPARAM)&scan);
    if (second && IsWindow(second)) ScanProc(second, (LPARAM)&scan);
    for (int i = 0; i < nItems; i++) if (!items[i].temp) items[i].running = scan.seen[i];
    SyncTemp(&scan);
}
static int RunningFixtureIndex(void) {
    for (int i = 0; i < nItems; i++) if (items[i].temp && items[i].type == T_FILE) return i;
    return -1;
}
static int TestGroupRunning(const WCHAR *exe) {
    cfg.groupRunning = 0;
    SaveSettingsOnly();
    WritePrivateProfileStringW(L"Settings", L"ShowGroupRunning", NULL, g_ini);
    cfg.groupRunning = 1;
    LoadConfig();
    CHECK(cfg.groupRunning == 0, "legacy config keeps group running icons disabled");
    cfg.groupRunning = 1;
    SaveSettingsOnly();
    cfg.groupRunning = 0;
    LoadConfig();
    CHECK(cfg.groupRunning == 1, "group running option survives config save and reload");
    cfg.minAnim = cfg.guardian = cfg.statusBar = 0;
    cfg.showRunning = cfg.clickToggle = 1;
    cfg.multiMode = 0;
    fixtureShiftState = 0;
    int launches = launchRequests;
    HWND host = TestWindow(L"Grouped Dock click fixture", 300, 100);
    g_hwnd = host;
    HWND first = CreateWindowExW(WS_EX_NOACTIVATE, testClass, L"Grouped main fixture", WS_OVERLAPPEDWINDOW,
                                 -30000, -30000, 600, 500, NULL, NULL, g_inst, NULL);
    HWND second = CreateWindowExW(WS_EX_NOACTIVATE, testClass, L"Grouped second fixture", WS_OVERLAPPEDWINDOW,
                                  -30000, -30000, 600, 500, NULL, NULL, g_inst, NULL);
    ShowWindow(first, SW_SHOWNOACTIVATE); ShowWindow(second, SW_SHOWNOACTIVATE);
    DrainFixtureMessages();
    CHECK(IsAppWindow(first) && IsAppWindow(second), "group fixtures use real normal application windows");
    Item group = {0}; group.type = T_GROUP; group.nk = 1;
    lstrcpyW(group.name, L"Fixture group");
    group.kids = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Item));
    CHECK(group.kids != NULL, "allocate a fixed group child");
    FillItem(&group.kids[0], T_FILE, 0, L"Grouped app", exe, NULL);
    group.kids[0].tdir[0] = 0;
    InsertItemStruct(0, &group, FALSE);
    cfg.groupRunning = 0;
    ScanGroupFixtures(first, second);
    CHECK(items[0].running && RunningFixtureIndex() < 0, "default mode marks the group running without duplicating its child");
    cfg.groupRunning = 1;
    fixtureForeground = NULL;
    ScanGroupFixtures(first, second);
    int runtime = RunningFixtureIndex();
    CHECK(runtime > 0 && CountType(T_FILE) == 1 && items[0].running, "enabled mode shows one running icon for a grouped executable");
    CHECK(items[0].nk == 1 && !items[0].temp && items[0].kids == group.kids, "running icon preserves the original group and shortcut");
    CHECK(items[runtime].hwnd == first, "scan binds the runtime icon to the existing main window");
    CHECK(ItemOfWin(first) == runtime, "window animations resolve to the runtime icon instead of the group");
    ScanGroupFixtures(first, second);
    CHECK(CountType(T_FILE) == 1, "repeated scans do not duplicate a grouped app's runtime icon");
    ShowWindow(first, SW_MINIMIZE);
    CHECK(WaitFixtureIconic(first, TRUE), "grouped main fixture is minimized before clicking");
    fixtureForeground = NULL;
    ScanGroupFixtures(second, first);
    CHECK(items[runtime].hwnd == first, "a scan retains the minimized bound window even when a sibling window is enumerated first");
    items[runtime].running = FALSE;
    ClickItem(runtime, 0);
    CHECK(WaitFixtureIconic(first, FALSE) && fixtureForeground == first && !IsIconic(second), "runtime click restores its bound minimized window despite a stale running flag");
    CHECK(launchRequests == launches, "restoring a grouped runtime window never executes its shortcut");
    WCHAR actualTarget[MAX_PATH];
    lstrcpyW(actualTarget, items[runtime].target);
    PathRemoveFileSpecW(items[runtime].target);
    PathAppendW(items[runtime].target, L"NotTheGroupedApplication.exe");
    fixtureForeground = NULL;
    ClickItem(runtime, 0);
    CHECK(!fixtureForeground && launchRequests == launches, "a runtime icon never switches to a different executable in the same folder");
    lstrcpyW(items[runtime].target, actualTarget);
    fixtureForeground = first;
    ClickItem(runtime, 0);
    CHECK(WaitFixtureIconic(first, TRUE), "repeated grouped runtime click minimizes the foreground main window");
    ClickItem(runtime, 0);
    CHECK(WaitFixtureIconic(first, FALSE), "grouped runtime click restores the same window again");
    fixtureForeground = second;
    ScanGroupFixtures(first, second);
    CHECK(items[runtime].hwnd == second, "a foreground window of the same application becomes the active binding");
    fixtureForeground = first;
    ClickItem(runtime, 0);
    CHECK(WaitFixtureIconic(first, TRUE), "foreground matching takes priority over an older binding");
    ShowWindow(second, SW_MINIMIZE);
    CHECK(WaitFixtureIconic(second, TRUE), "second grouped window is minimized for the group-shortcut test");
    fixtureForeground = NULL;
    LaunchPtr(&items[0].kids[0]);
    CHECK(launchRequests == launches && fixtureForeground && !IsIconic(fixtureForeground), "opening the group child activates an existing window rather than relaunching");
    cfg.groupRunning = 0;
    ScanGroupFixtures(first, second);
    CHECK(RunningFixtureIndex() < 0 && CountType(T_GROUP) == 1 && items[0].nk == 1, "disabling the option removes runtime icons and preserves the group child");
    cfg.groupRunning = 1; cfg.showRunning = 0;
    ScanGroupFixtures(first, second);
    CHECK(RunningFixtureIndex() < 0, "the existing running-app display switch remains the master switch");
    cfg.showRunning = 1;
    Item pinned;
    FillItem(&pinned, T_FILE, 0, L"Directly pinned fixture", exe, NULL);
    pinned.tdir[0] = 0;
    InsertItemStruct(1, &pinned, FALSE);
    ScanGroupFixtures(first, second);
    CHECK(RunningFixtureIndex() < 0, "an app also pinned directly to the Dock is not duplicated");
    items[1].running = FALSE; fixtureForeground = NULL;
    LaunchItem(1, FALSE);
    CHECK(launchRequests == launches && fixtureForeground, "a fixed shortcut also activates an existing window when its scan flag is stale");
    RemoveItem(1);
    ScanGroupFixtures(first, second);
    runtime = RunningFixtureIndex();
    cfg.clickToggle = 0; fixtureForeground = second;
    ClickItem(runtime, 0);
    CHECK(!IsIconic(second) && launchRequests == launches, "grouped runtime icons honor the disabled click-toggle setting");
    cfg.clickToggle = 1;
    DestroyWindow(first);
    items[runtime].hwnd = first; fixtureForeground = NULL;
    ClickItem(runtime, 0);
    CHECK(items[runtime].hwnd == second && launchRequests == launches, "a destroyed binding resolves to another existing window without launching");
    TrayApp *tray = &g_trayApps[0];
    lstrcpyW(tray->exe, exe); lstrcpyW(tray->cls, testClass); lstrcpyW(tray->title, L"Grouped second fixture");
    lstrcpyW(tray->name, L"Grouped tray fixture"); tray->pid = GetCurrentProcessId(); tray->hwnd = second;
    tray->process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, tray->pid);
    g_ntrayApps = 1;
    SyncTrayApps(); ScanGroupFixtures(NULL, second);
    CHECK(RunningFixtureIndex() < 0 && CountType(T_TRAY) == 1 && CountType(T_GROUP) == 1, "tray selection keeps one dedicated icon instead of duplicating the grouped runtime entry");
    ClearTrayApps(); SyncTrayApps(); ScanGroupFixtures(NULL, second);
    runtime = RunningFixtureIndex();
    CHECK(runtime > 0 && !CountType(T_TRAY), "removing tray selection restores the ordinary grouped runtime icon");
    DestroyWindow(second); fixtureForeground = NULL;
    ClickItem(runtime, 0);
    CHECK(launchRequests == launches, "a stale runtime icon never launches the executable when no window remains");
    ScanGroupFixtures(NULL, NULL);
    CHECK(RunningFixtureIndex() < 0 && nItems == 1 && items[0].nk == 1, "closing all application windows removes the runtime icon and divider only");
    LaunchPtr(&items[0].kids[0]);
    CHECK(launchRequests == launches + 1, "the fixed group shortcut can launch an app when it has no windows");
    BuildRows();
    BOOL found = FALSE;
    for (int i = 0; i < g_nrow; i++) if (g_row[i].type == R_TOGGLE && g_row[i].v == &cfg.groupRunning && g_row[i].page == 0) found = TRUE;
    CHECK(found && g_nrow < 620, "general settings expose the persisted group running option");
    ClearItems(); DestroyWindow(host); g_hwnd = NULL;
    DeleteFileW(g_ini);
    return 0;
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
    /* Keep normal window styles to exercise the real app-window filter; start offscreen. */
    HWND ordinary = CreateWindowExW(WS_EX_NOACTIVATE, testClass, L"Ordinary running app fixture", WS_OVERLAPPEDWINDOW,
                                    -30000, -30000, 600, 500, NULL, NULL, g_inst, NULL);
    ShowWindow(ordinary, SW_SHOWNOACTIVATE);
    DrainFixtureMessages();
    CHECK(IsAppWindow(ordinary), "ordinary fixture is accepted by the normal app-window filter");
    Item ordinaryItem;
    FillItem(&ordinaryItem, T_FILE, 0, L"Ordinary fixture", exe, NULL);
    ordinaryItem.tdir[0] = 0;
    ordinaryItem.running = TRUE;
    InsertItemStruct(0, &ordinaryItem, TRUE);
    SetForegroundWindow(ordinary);
    LaunchItem(0, FALSE);
    DrainFixtureMessages();
    CHECK(WaitFixtureIconic(ordinary, TRUE), "ordinary runtime app minimizes through LaunchItem");
    LaunchItem(0, FALSE);
    DrainFixtureMessages();
    CHECK(WaitFixtureIconic(ordinary, FALSE), "ordinary runtime app restores through LaunchItem");
    HWND clickHost = TestWindow(L"Dock click fixture", 300, 100);
    g_hwnd = clickHost;
    Item fixedItem;
    FillItem(&fixedItem, T_FILE, 0, L"Pinned fixture", exe, NULL);
    fixedItem.tdir[0] = 0; fixedItem.running = TRUE;
    InsertItemStruct(nItems, &fixedItem, FALSE);
    HWND document = TestWindow(L"Document click fixture", 600, 500);
    Item windowItem = {0}; windowItem.type = T_WIN; windowItem.hwnd = document;
    lstrcpyW(windowItem.name, L"Document fixture");
    InsertItemStruct(nItems, &windowItem, TRUE);
    HWND trayWindow = TestWindow(L"Tray click fixture", 600, 500);
    a = &g_trayApps[0];
    lstrcpyW(a->exe, exe); lstrcpyW(a->cls, testClass);
    lstrcpyW(a->title, L"Tray click fixture"); lstrcpyW(a->name, L"Tray fixture");
    a->pid = GetCurrentProcessId(); a->hwnd = trayWindow;
    a->process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, a->pid);
    g_ntrayApps = 1;
    SyncTrayApps();
    CHECK(CountType(T_FILE) == 2 && CountType(T_WIN) == 1 && CountType(T_TRAY) == 1, "mixed fixture contains pinned, ordinary runtime, document and tray icons");
    ClickItem(nItems - 1, 0);
    CHECK(IsWindowVisible(trayWindow), "tray mouse click reveals the hidden bound window");
    fixtureShiftState = (SHORT)0x8000; /* stale thread state; neither mouse message carries Shift */
    HWND targets[] = {ordinary, ordinary, document, trayWindow};
    int indices[] = {0, 1, 2, nItems - 1};
    for (int i = 0; i < 4; i++) {
        ShowWindow(targets[i], SW_SHOWNOACTIVATE);
        fixtureForeground = NULL;
        ClickItem(indices[i], 0);
        CHECK(fixtureForeground == targets[i] && !IsIconic(targets[i]), "mixed mouse click brings a background window forward");
        ClickItem(indices[i], 0);
        CHECK(WaitFixtureIconic(targets[i], TRUE), "mixed repeated mouse click minimizes the foreground window");
        ClickItem(indices[i], 0);
        CHECK(WaitFixtureIconic(targets[i], FALSE), "mixed mouse click restores a minimized window");
    }
    CHECK(launchRequests == 0, "ordinary clicks never relaunch despite stale queued Shift state");
    fixtureShiftState = 0; /* opposite mismatch: mouse message carries an actual Shift click */
    ClickItem(0, MK_SHIFT);
    CHECK(launchRequests == 1 && !IsIconic(ordinary), "explicit Shift click still requests a new launch");
    cfg.clickToggle = 0;
    fixtureForeground = ordinary;
    ClickItem(0, 0);
    CHECK(!IsIconic(ordinary) && launchRequests == 1, "ordinary mouse click honors disabled toggle without relaunching");
    fixtureForeground = trayWindow;
    ClickItem(nItems - 1, 0);
    CHECK(!IsIconic(trayWindow), "tray mouse click honors disabled toggle");
    ClearTrayApps();
    DestroyWindow(trayWindow); DestroyWindow(document); DestroyWindow(clickHost); g_hwnd = NULL;
    DestroyWindow(ordinary);
    ClearItems();
    if (TestGroupRunning(exe)) return 1;
    printf("PASS %d checks\n", checks);
    return 0;
}
