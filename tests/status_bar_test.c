#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <stdio.h>
#include <wchar.h>

/* Real private windows; deterministic foreground and monitor inputs. AppBar calls
   are simulated so these tests never change the user's desktop work area. */
static HWND foregroundFixture;
static RECT monitorFixture = {-30000, -30000, -28080, -28920};
static RECT frameFixture;
static WCHAR exeFixture[MAX_PATH], titleFixture[512], classFixture[64];
static BOOL maximizedFixture, iconicFixture, otherMonitorFixture;
static int cloakedFixture, shellStateFixture = 5;
static LONG extendedFixture, styleFixture;
static POINT cursorFixture;
static int appBarAdds, appBarRemoves, appBarPositions, moves;
static int taskbarEdge = -1, queryTopInset;
static BOOL taskbarOtherMonitor, failQuery, failPosition;
static RECT reservedFixture;
static LPARAM taskbarStateFixture;
static int taskbarStateWrites;
static HWND TestForeground(void) { return foregroundFixture; }
static BOOL TestZoomed(HWND h) { return h == foregroundFixture ? maximizedFixture : IsZoomed(h); }
static BOOL TestIconic(HWND h) { return h == foregroundFixture ? iconicFixture : IsIconic(h); }
static LONG TestWindowLong(HWND h, int index) {
    if (h == foregroundFixture && index == GWL_EXSTYLE) return extendedFixture;
    if (h == foregroundFixture && index == GWL_STYLE) return styleFixture;
    return GetWindowLongW(h, index);
}
static int TestClass(HWND h, LPWSTR text, int count) {
    if (h != foregroundFixture) return GetClassNameW(h, text, count);
    lstrcpynW(text, classFixture, count); return lstrlenW(text);
}
static int TestTitle(HWND h, LPWSTR text, int count) {
    if (h != foregroundFixture) return GetWindowTextW(h, text, count);
    lstrcpynW(text, titleFixture, count); return lstrlenW(text);
}
static HRESULT WINAPI TestDwm(HWND h, DWORD attribute, PVOID value, DWORD size) {
    if (h == foregroundFixture && attribute == 9 && size == sizeof(RECT)) { *(RECT *)value = frameFixture; return S_OK; }
    if (h == foregroundFixture && attribute == 14 && size == sizeof(int)) { *(int *)value = cloakedFixture; return S_OK; }
    return DwmGetWindowAttribute(h, attribute, value, size);
}
static HMONITOR TestMonitor(HWND h, DWORD flags) { (void)h; (void)flags; return (HMONITOR)(UINT_PTR)(otherMonitorFixture ? 2 : 1); }
static BOOL TestMonitorInfo(HMONITOR monitor, LPMONITORINFO info) {
    info->rcMonitor = monitorFixture; info->rcWork = monitorFixture;
    if ((UINT_PTR)monitor == 2) { OffsetRect(&info->rcMonitor, 1920, 0); OffsetRect(&info->rcWork, 1920, 0); }
    return TRUE;
}
static BOOL WINAPI TestImagePath(HANDLE process, DWORD flags, LPWSTR path, PDWORD length) {
    (void)process; (void)flags;
    if (*length <= (DWORD)lstrlenW(exeFixture)) return FALSE;
    lstrcpyW(path, exeFixture); *length = lstrlenW(path); return TRUE;
}
static HRESULT WINAPI TestShellState(int *state) { *state = shellStateFixture; return S_OK; }
static FARPROC WINAPI TestProcAddress(HMODULE module, LPCSTR name) {
    if ((UINT_PTR)name > 0xffff && !strcmp(name, "SHQueryUserNotificationState")) return (FARPROC)TestShellState;
    return GetProcAddress(module, name);
}
static UINT_PTR WINAPI TestAppBar(DWORD message, PAPPBARDATA data) {
    if (message == ABM_GETSTATE) return taskbarStateFixture;
    if (message == ABM_SETSTATE) { taskbarStateFixture = data->lParam; taskbarStateWrites++; return 1; }
    if (message == ABM_GETTASKBARPOS) {
        if (taskbarEdge < 0) return 0;
        data->uEdge = taskbarEdge; data->rc = monitorFixture;
        data->rc.bottom = data->rc.top + 32;
        if (taskbarOtherMonitor) OffsetRect(&data->rc, 1920, 0);
        return 1;
    }
    if (message == ABM_QUERYPOS) { data->rc.top += queryTopInset; return !failQuery; }
    if (message == ABM_NEW) appBarAdds++;
    if (message == ABM_REMOVE) appBarRemoves++;
    if (message == ABM_SETPOS) { appBarPositions++; if (failPosition) return 0; reservedFixture = data->rc; }
    return 1;
}
static BOOL TestCursor(LPPOINT point) { *point = cursorFixture; return TRUE; }
static BOOL TestPosition(HWND h, HWND after, int x, int y, int width, int height, UINT flags) {
    moves++; return SetWindowPos(h, after, x, y, width, height, flags);
}
#define GetForegroundWindow TestForeground
#define IsZoomed TestZoomed
#define IsIconic TestIconic
#define GetWindowLongW TestWindowLong
#define GetClassNameW TestClass
#define GetWindowTextW TestTitle
#define DwmGetWindowAttribute TestDwm
#define MonitorFromWindow TestMonitor
#define GetMonitorInfoW TestMonitorInfo
#define QueryFullProcessImageNameW TestImagePath
#define GetProcAddress TestProcAddress
#define SHAppBarMessage TestAppBar
#define GetCursorPos TestCursor
#define SetWindowPos TestPosition
#include "../MiniDock-source/minidock.c"

static int checks;
#define CHECK(condition, label) do { checks++; if (!(condition)) { fprintf(stderr, "FAIL %d: %s\n", checks, label); return 1; } } while (0)
static void ForegroundCase(const WCHAR *exe, const WCHAR *title, BOOL maximized, BOOL fullscreen) {
    lstrcpynW(exeFixture, exe, MAX_PATH); lstrcpynW(titleFixture, title, 512);
    lstrcpyW(classFixture, L"StatusBarForegroundFixture");
    maximizedFixture = maximized; iconicFixture = otherMonitorFixture = FALSE;
    extendedFixture = cloakedFixture = 0; shellStateFixture = 5;
    styleFixture = fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    frameFixture = monitorFixture;
    if (!fullscreen) InflateRect(&frameFixture, -80, -80);
    cursorFixture.x = monitorFixture.left + 400; cursorFixture.y = monitorFixture.top + 500;
}
int main(void) {
    OleInitialize(NULL);
    GdiplusStartupInput startup = {1, NULL, FALSE, FALSE};
    CHECK(GdiplusStartup(&g_gdipTok, &startup, NULL) == 0, "start image library");
    g_inst = GetModuleHandleW(NULL); g_dpi = g_dpiCfg = 96;
    DefaultCfg();
    CHECK(cfg.sbFullscreenHide == 1, "fullscreen content hiding defaults on without enabling mouse-edge hiding");
    WCHAR temporary[MAX_PATH]; GetTempPathW(MAX_PATH, temporary);
    CHECK(GetTempFileNameW(temporary, L"MSB", 0, g_ini), "create a private configuration fixture");
    WritePrivateProfileStringW(L"Settings", L"SbAutoHide", L"1", g_ini); LoadConfig();
    CHECK(cfg.sbFullscreenHide == 1, "legacy smart-hide-on setting does not enable edge hiding");
    WritePrivateProfileStringW(L"Settings", L"SbAutoHide", L"0", g_ini); LoadConfig();
    CHECK(cfg.sbFullscreenHide == 1, "legacy smart-hide-off setting does not disable fullscreen hiding");
    cfg.sbFullscreenHide = 0; SaveSettingsOnly(); cfg.sbFullscreenHide = 1; LoadConfig();
    CHECK(cfg.sbFullscreenHide == 0, "explicit fullscreen-hide opt-out is saved and reloaded independently");
    cfg.sbFullscreenHide = 1; SaveSettingsOnly();
    CHECK(GetPrivateProfileIntW(L"Settings", L"SbFullscreenHide", 0, g_ini) == 1, "fullscreen hiding is saved under its own setting key");
    DeleteFileW(g_ini); g_ini[0] = 0;
    DefaultCfg(); cfg.statusBar = cfg.sbMaxAvoid = 1; cfg.sbFrost = 0;
    cfg.guardian = cfg.clock = cfg.ctrlCenter = cfg.trayW = cfg.resMon = cfg.sbBattery = cfg.notifyW = 0;
    cfg.gameGuard = 0; g_ngames = 0;
    g_mi.cbSize = sizeof g_mi; g_mi.rcMonitor = g_mi.rcWork = monitorFixture;
    RebuildDockFonts();
    WNDCLASSW wc = {0}; wc.lpfnWndProc = DefWindowProcW; wc.hInstance = g_inst; wc.lpszClassName = L"StatusBarForegroundFixture";
    CHECK(RegisterClassW(&wc), "register foreground fixture class");
    foregroundFixture = CreateWindowExW(WS_EX_NOACTIVATE, wc.lpszClassName, L"Status bar test foreground", WS_OVERLAPPEDWINDOW,
        -30000, -30000, 600, 500, NULL, NULL, g_inst, NULL);
    ShowWindow(foregroundFixture, SW_SHOWNOACTIVATE);
    CHECK(foregroundFixture && IsWindowVisible(foregroundFixture), "create a real visible foreground fixture without activating it");
    ForegroundCase(L"C:\\fixtures\\WINWORD.EXE", L"Document - Word", TRUE, FALSE);
    CHECK(SbForegroundMode() == 1, "ordinary maximized application is not immersive content");
    SbTick();
    CHECK(g_sb && IsWindowVisible(g_sb) && g_sbAbReg, "ordinary maximized app keeps a visible bar and top reservation");
    HWND originalBar = g_sb;
    ForegroundCase(L"C:\\fixtures\\PotPlayerMini64.exe", L"Movie", TRUE, FALSE);
    CHECK(SbForegroundMode() == 1, "maximized independent video player is not fullscreen");
    SbTick();
    CHECK(IsWindowVisible(g_sb) && g_sbAbReg && !g_sbContentSuppressed, "a maximized player keeps the bar visible");
    ForegroundCase(L"C:\\fixtures\\PotPlayerMini64.exe", L"Movie", FALSE, TRUE);
    CHECK(SbForegroundMode() == 2, "true fullscreen player is immersive without Dock game mode");
    SbTick();
    CHECK(g_sb == originalBar && !IsWindowVisible(g_sb) && g_sbContentSuppressed, "video hides the existing bar without destroying its canvas");
    CHECK(!g_sbAbReg && appBarRemoves == 1, "video suppression releases the top work-area reservation");
    RECT hiddenPosition, afterDraw;
    GetWindowRect(g_sb, &hiddenPosition);
    SbProc(g_sb, WM_TIMER, 1, 0);
    GetWindowRect(g_sb, &afterDraw);
    CHECK(!IsWindowVisible(g_sb) && EqualRect(&hiddenPosition, &afterDraw), "a pending paint timer cannot redisplay or reposition the suppressed bar");
    int additions = appBarAdds;
    SbAppBar(TRUE);
    CHECK(!g_sbAbReg && !IsWindowVisible(g_sb) && appBarAdds == additions, "reservation repair cannot show a content-suppressed status bar");
    for (int i = 0; i < 5; i++) SbTick();
    CHECK(!IsWindowVisible(g_sb) && appBarRemoves == 1, "repeated fullscreen ticks do not re-register or redisplay the bar");
    SbProc(g_sb, WM_APP_AB, ABN_FULLSCREENAPP, 1);
    CHECK(!IsWindowVisible(g_sb) && !g_sbAbReg, "shell fullscreen-start notification preserves suppression");
    ForegroundCase(L"C:\\fixtures\\msedge.exe", L"Sample video - YouTube - Microsoft Edge", TRUE, FALSE);
    CHECK(SbForegroundMode() == 1, "a maximized video website is not fullscreen");
    SbTick();
    RECT actual; GetWindowRect(g_sb, &actual);
    CHECK(IsWindowVisible(g_sb) && !g_sbContentSuppressed && actual.top == monitorFixture.top && g_sbAbReg,
        "leaving fullscreen for a maximized video page restores visibility, position and reservation");
    ForegroundCase(L"C:\\fixtures\\chrome.exe", L"bilibili - video", FALSE, FALSE);
    SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(SbForegroundMode() == 0 && IsWindowVisible(g_sb) && actual.top == monitorFixture.top && g_sbAbReg,
        "opening a windowed browser on a video site does not hide the bar with the cursor away from the top");
    ForegroundCase(L"C:\\fixtures\\msedge.exe", L"Project - GitHub - Microsoft Edge", TRUE, FALSE);
    CHECK(SbForegroundMode() == 1, "ordinary maximized browser page remains an ordinary application");
    ForegroundCase(L"C:\\fixtures\\msedge.exe", L"Project - GitHub - Microsoft Edge", TRUE, TRUE);
    styleFixture = WS_OVERLAPPEDWINDOW;
    CHECK(SbForegroundMode() == 1, "an ordinary framed maximized browser is not fullscreen even when hidden taskbars let it fill the monitor");
    SbTick();
    CHECK(IsWindowVisible(g_sb) && !g_sbContentSuppressed && g_sbAbReg, "switching between two maximized windows restores visibility and reservation");
    GetWindowRect(g_sb, &actual);
    CHECK(actual.top == monitorFixture.top, "ordinary maximized browser remains on screen with fullscreen hiding enabled");
    ForegroundCase(L"C:\\fixtures\\msedge.exe", L"Custom video site", FALSE, TRUE);
    CHECK(SbForegroundMode() == 2, "fullscreen browser content is recognized without a known site title");
    SbTick();
    CHECK(!IsWindowVisible(g_sb) && !g_sbAbReg, "fullscreen browser hides pixels and releases reservation");
    ForegroundCase(L"C:\\fixtures\\chrome.exe", L"Video", TRUE, TRUE);
    CHECK(SbForegroundMode() == 2, "a borderless fullscreen browser is recognized even if its previous maximize state persists");
    cfg.sbFullscreenHide = 0; SbTick();
    CHECK(IsWindowVisible(g_sb) && !g_sbContentSuppressed && g_sbAbReg, "turning fullscreen hiding off restores the bar immediately");
    cfg.sbFullscreenHide = 1; SbTick();
    CHECK(!IsWindowVisible(g_sb) && g_sbContentSuppressed && !g_sbAbReg, "turning fullscreen hiding back on applies to the current fullscreen content");
    ForegroundCase(L"C:\\fixtures\\League of Legends.exe", L"League of Legends", TRUE, FALSE);
    CHECK(SbForegroundMode() == 1, "a maximized windowed game is not fullscreen");
    SbTick();
    CHECK(IsWindowVisible(g_sb) && !g_sbContentSuppressed, "a maximized windowed game restores the status bar");
    ForegroundCase(L"C:\\fixtures\\League of Legends.exe", L"League of Legends", FALSE, TRUE);
    CHECK(SbForegroundMode() == 2, "a true fullscreen game is recognized");
    ForegroundCase(L"C:\\fixtures\\custom-game.exe", L"Custom game", FALSE, TRUE);
    lstrcpyW(g_games[0], L"custom-game.exe"); g_ngames = 1;
    CHECK(SbForegroundMode() == 2, "custom game list is honored independently of Dock game guard");
    g_ngames = 0; lstrcpyW(classFixture, L"UnityWndClass");
    CHECK(SbForegroundMode() == 2, "known game-engine window class is recognized");
    ForegroundCase(L"C:\\fixtures\\unknown.exe", L"Exclusive game", FALSE, TRUE); shellStateFixture = QUNS_D3D_;
    CHECK(SbForegroundMode() == 2, "exclusive Direct3D fullscreen content is recognized");
    ForegroundCase(L"C:\\fixtures\\WINWORD.EXE", L"Document", TRUE, FALSE); shellStateFixture = QUNS_D3D_;
    CHECK(SbForegroundMode() == 1, "a global Direct3D state does not misclassify an ordinary maximized window");
    frameFixture = monitorFixture;
    CHECK(SbForegroundMode() == 1, "a framed ordinary maximized window covering the monitor is not misclassified by a global Direct3D state");
    ForegroundCase(L"C:\\fixtures\\vlc.exe", L"Movie", FALSE, FALSE);
    CHECK(SbForegroundMode() == 0, "ordinary small video window does not trigger content suppression");
    ForegroundCase(L"C:\\fixtures\\vlc.exe", L"Movie", FALSE, TRUE); otherMonitorFixture = TRUE;
    CHECK(SbForegroundMode() == 0, "media on a different monitor does not hide this monitor's bar");
    otherMonitorFixture = FALSE; iconicFixture = TRUE;
    CHECK(SbForegroundMode() == 0, "minimized media does not suppress the bar");
    iconicFixture = FALSE; cloakedFixture = 1;
    CHECK(SbForegroundMode() == 0, "cloaked media does not suppress the bar");
    cloakedFixture = 0; extendedFixture = WS_EX_TOOLWINDOW;
    CHECK(SbForegroundMode() == 0, "tool overlays are excluded");
    extendedFixture = 0; lstrcpyW(classFixture, L"Shell_TrayWnd");
    CHECK(SbForegroundMode() == 0, "shell surfaces are excluded");
    ForegroundCase(L"C:\\fixtures\\ApplicationFrameHost.exe", L"Calculator", TRUE, FALSE);
    CHECK(SbForegroundMode() == 1, "a generic UWP host is not assumed to be a video player");
    ForegroundCase(L"C:\\fixtures\\notepad.exe", L"Notes", FALSE, FALSE);
    SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(IsWindowVisible(g_sb) && actual.top == monitorFixture.top && g_sbAbReg, "ordinary windows retain the visible bar even with the cursor away from the top");
    RECT visiblePosition = actual;
    SbDraw(g_sb); GetWindowRect(g_sb, &actual);
    CHECK(EqualRect(&actual, &visiblePosition), "redrawing the bar preserves its actual monitor position instead of moving to zero-zero");
    SbProc(g_sb, WM_TIMER, 1, 0); GetWindowRect(g_sb, &actual);
    CHECK(EqualRect(&actual, &visiblePosition), "the normal refresh timer preserves the monitor position");
    SetWindowPos(g_sb, NULL, actual.left, monitorFixture.top - 200, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(actual.top == monitorFixture.top, "a stale offscreen position is corrected after an ordinary window is opened");
    cursorFixture.y = monitorFixture.top; SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(actual.top == monitorFixture.top && g_sbAbReg, "the bar remains visible at the top edge");
    cursorFixture.x = monitorFixture.right + 10; SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(actual.top == monitorFixture.top && g_sbAbReg, "moving the cursor to another monitor does not hide this bar");
    gMenuOpen = TRUE; SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(actual.top == monitorFixture.top, "an open status menu retains its anchor"); gMenuOpen = FALSE;
    SbTick();
    int registrations = appBarAdds, placements = appBarPositions, oldMoves = moves;
    for (int i = 0; i < 8; i++) SbTick();
    CHECK(appBarAdds == registrations && appBarPositions == placements && moves == oldMoves, "stable ordinary content does not cause repeated work-area or position updates");
    cfg.sbHeight += 4; SbTick();
    CHECK(g_sb && IsWindowVisible(g_sb) && g_sbAbReg && g_sbCv.h2 == Sc(cfg.sbHeight), "status bar resize recreates and registers the correct bar");
    cfg.statusBar = 0; SbTick();
    CHECK(!g_sb && !g_sbAbReg && !g_sbContentSuppressed, "disabling the status bar clears visibility, reservation and suppression state");
    cfg.statusBar = 1; ForegroundCase(L"C:\\fixtures\\vlc.exe", L"Movie", FALSE, TRUE); SbTick();
    CHECK(!g_sb && g_sbContentSuppressed && !g_sbAbReg, "starting while fullscreen avoids creating or reserving a visible bar");
    ForegroundCase(L"C:\\fixtures\\WINWORD.EXE", L"Document", TRUE, FALSE); SbTick();
    CHECK(g_sb && IsWindowVisible(g_sb) && g_sbAbReg, "leaving fullscreen creates and displays the bar correctly");
    BuildRows();
    BOOL fullscreenHideOption = FALSE;
    for (int i = 0; i < g_nrow; i++) if (g_row[i].page == 30 && g_row[i].type == R_TOGGLE && g_row[i].v == &cfg.sbFullscreenHide) fullscreenHideOption = TRUE;
    CHECK(fullscreenHideOption && g_nrow < 620, "settings expose fullscreen content hiding without exceeding row capacity");
    cfg.taskHide = 2; taskbarEdge = ABE_TOP;
    CHECK(TaskbarStateForMode(0) == ABS_AUTOHIDE, "a permanently hidden top taskbar releases its normal reserved height");
    CHECK(TaskbarStateForMode(ABS_ALWAYSONTOP) == (ABS_ALWAYSONTOP | ABS_AUTOHIDE), "top taskbar release preserves unrelated state flags");
    taskbarEdge = ABE_BOTTOM;
    CHECK(TaskbarStateForMode(0) == 0, "the bottom taskbar policy is unchanged");
    cfg.taskHide = 1;
    CHECK(TaskbarStateForMode(0) == ABS_AUTOHIDE, "ordinary taskbar auto-hide keeps its existing behavior");
    cfg.taskHide = 0;
    CHECK(TaskbarStateForMode(ABS_AUTOHIDE | ABS_ALWAYSONTOP) == ABS_ALWAYSONTOP, "visible taskbar mode clears a forced auto-hide state");
    cfg.taskHide = 2; taskbarEdge = ABE_TOP; taskbarOtherMonitor = TRUE;
    CHECK(!SbHiddenTopTaskbar(), "a top taskbar on another monitor is not treated as this bar's reservation");
    taskbarOtherMonitor = FALSE; queryTopInset = 2; g_sbAbDirty = TRUE;
    SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(g_sbAbReg && reservedFixture.top == monitorFixture.top + 2 && reservedFixture.bottom == actual.bottom,
        "the taskbar reveal strip plus status bar reservation ends exactly at the drawn bar bottom");
    CHECK(actual.top == monitorFixture.top, "the hidden taskbar reveal strip does not move the status bar below the monitor edge");
    int stablePositions = appBarPositions, stableAdds = appBarAdds;
    for (int i = 0; i < 8; i++) { SbProc(g_sb, WM_APP_AB, ABN_POSCHANGED, 0); SbTick(); }
    CHECK(appBarPositions == stablePositions && appBarAdds == stableAdds && !g_sbAbDirty,
        "SETPOS notifications settle without repeated reservation writes or a Shell feedback loop");
    queryTopInset = 0; SbProc(g_sb, WM_APP_AB, ABN_POSCHANGED, 0); SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(reservedFixture.top == monitorFixture.top && reservedFixture.bottom == actual.bottom && appBarPositions == stablePositions + 1,
        "a Shell reservation change is reconciled without adding the previous inset twice");
    cfg.sbTopGap = 7; cfg.sbHeight += 6; SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(actual.top == monitorFixture.top + 7 && reservedFixture.bottom == actual.bottom,
        "custom status bar height and top gap are reserved exactly once");
    failQuery = TRUE; g_sbAbDirty = TRUE; SbTick();
    CHECK(!g_sbAbReg && IsWindowVisible(g_sb), "a failed reservation query is unregistered for retry without hiding ordinary content");
    failQuery = FALSE; SbTick();
    CHECK(g_sbAbReg && reservedFixture.bottom == actual.bottom, "a transient Shell query failure is retried");
    failPosition = TRUE; queryTopInset = 1; g_sbAbDirty = TRUE; SbTick();
    CHECK(!g_sbAbReg, "a failed reservation write does not leave a falsely accepted rectangle cached");
    failPosition = FALSE; SbTick();
    CHECK(g_sbAbReg && reservedFixture.bottom == actual.bottom, "a failed reservation write recovers on the existing tick");
    queryTopInset = 200; g_sbAbDirty = TRUE; stablePositions = appBarPositions; SbTick();
    CHECK(!g_sbAbReg && appBarPositions == stablePositions, "a still-stale full taskbar reservation does not add a second status-bar-height strip");
    queryTopInset = 2; SbTick();
    CHECK(g_sbAbReg && reservedFixture.bottom == actual.bottom, "the status bar registers after Explorer releases the hidden taskbar height");
    ForegroundCase(L"C:\\fixtures\\vlc.exe", L"Movie", FALSE, TRUE); SbTick();
    CHECK(!g_sbAbReg && !IsWindowVisible(g_sb), "fullscreen still releases the status reservation with a hidden top taskbar");
    ForegroundCase(L"C:\\fixtures\\notepad.exe", L"Notes", TRUE, FALSE); SbTick(); GetWindowRect(g_sb, &actual);
    CHECK(g_sbAbReg && IsWindowVisible(g_sb) && reservedFixture.bottom == actual.bottom, "leaving fullscreen restores exactly the status bar's own height");
    g_taskbarStateSaved = TRUE; g_taskbarOriginalState = 0; g_taskbarAppliedState = ABS_AUTOHIDE; taskbarStateFixture = ABS_AUTOHIDE;
    RestoreTaskbarState();
    CHECK(taskbarStateFixture == 0 && taskbarStateWrites == 1, "shutdown restores a taskbar state that still matches our temporary change");
    taskbarStateFixture = ABS_AUTOHIDE | ABS_ALWAYSONTOP; RestoreTaskbarState();
    CHECK(taskbarStateFixture == (ABS_AUTOHIDE | ABS_ALWAYSONTOP) && taskbarStateWrites == 1, "shutdown preserves a later taskbar state change made outside the Dock");
    g_taskbarStateSaved = FALSE;
    int removalsBeforeClose = appBarRemoves; SbHide();
    CHECK(!g_sb && !g_sbAbReg && appBarRemoves == removalsBeforeClose + 1, "closing the status bar explicitly unregisters its reservation before destroying the window");
    cfg.statusBar = 0; SbTick(); DestroyWindow(foregroundFixture);
    printf("PASS %d status-bar checks\n", checks);
    return 0;
}
