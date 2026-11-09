#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <stdio.h>
/* Only fixture process identity is substituted; window matching and handle lifetime
   use production code and real hidden HWNDs. No installed music app is controlled. */
static WCHAR fixtureExe[MAX_PATH]=L"D:\\FixtureMusic\\player.exe";
static BOOL WINAPI FixtureImageName(HANDLE process,DWORD flags,LPWSTR out,PDWORD count) {
    if(GetProcessId(process)==GetCurrentProcessId()) {
        DWORD n=lstrlenW(fixtureExe);
        if(*count<=n){SetLastError(ERROR_INSUFFICIENT_BUFFER);return FALSE;}
        lstrcpyW(out,fixtureExe);*count=n;return TRUE;
    }
    return QueryFullProcessImageNameW(process,flags,out,count);
}
#define QueryFullProcessImageNameW FixtureImageName
#include "../MiniDock-source/minidock.c"
static int checks;
#define CHECK(condition,label) do{checks++;if(!(condition)){fprintf(stderr,"FAIL %d: %s\n",checks,label);return 1;}}while(0)
static TrayApp *Selection(const WCHAR *exe,const WCHAR *title) {
    ClearTrayApps();g_ntrayApps=1;
    TrayApp *a=&g_trayApps[0];lstrcpyW(a->exe,exe);lstrcpyW(a->cls,L"MiniDockDynamicTrayMain");lstrcpyW(a->title,title);lstrcpyW(a->name,L"Player fixture");return a;
}
static HWND Window(const WCHAR *cls,const WCHAR *title,DWORD ex,HWND owner) {
    return CreateWindowExW(ex,cls,title,WS_OVERLAPPEDWINDOW,-30000,-30000,800,600,owner,NULL,GetModuleHandleW(NULL),NULL);
}
int main(void) {
    WNDCLASSW wc={0};wc.lpfnWndProc=DefWindowProcW;wc.hInstance=GetModuleHandleW(NULL);wc.lpszClassName=L"MiniDockDynamicTrayMain";
    CHECK(RegisterClassW(&wc),"register private main-window class");
    wc.lpszClassName=L"MiniDockMusicAuxiliary";
    CHECK(RegisterClassW(&wc),"register private auxiliary-window class");
    HWND main=Window(L"MiniDockDynamicTrayMain",L"Song B - Artist",WS_EX_APPWINDOW,NULL);
    HWND aux=Window(wc.lpszClassName,L"Desktop lyrics",WS_EX_APPWINDOW,NULL);
    HWND tool=Window(L"MiniDockDynamicTrayMain",L"Mini player",WS_EX_TOOLWINDOW,NULL);
    HWND owned=Window(L"MiniDockDynamicTrayMain",L"Player login",0,aux);
    CHECK(main&&aux&&tool&&owned,"create hidden main, lyrics, mini-player and owned helper fixtures");
    TrayApp *a=Selection(fixtureExe,L"Song A - Artist");RefreshTrayApps();
    CHECK(a->hwnd==main&&TrayProcessAlive(a)&&TrayWindowValid(a),"a changed song title rebinds the saved music main window");
    CHECK(!IsWindowVisible(main)&&!lstrcmpW(a->title,L"Song B - Artist"),"a hidden main window is recognized and its title is refreshed");
    SetWindowTextW(main,L"Song C - Artist");RefreshTrayApps();
    CHECK(a->hwnd==main&&TrayWindowValid(a),"song changes retain the existing live process and HWND binding");
    a=Selection(fixtureExe,L"Song A - Artist");RefreshTrayApps();
    CHECK(a->hwnd==main&&!lstrcmpW(a->title,L"Song C - Artist"),"rebinding tolerates another title change while the main window stays hidden");
    WCHAR temp[MAX_PATH],file[MAX_PATH];GetTempPathW(MAX_PATH,temp);GetTempFileNameW(temp,L"mdm",0,file);lstrcpyW(g_ini,file);
    g_dpi=g_dpiCfg=96;DefaultCfg();SaveNow();
    CHECK(a->processStart!=0&&GetPrivateProfileIntW(L"TrayApps",L"Pid0",0,g_ini)==GetCurrentProcessId(),"saved identity includes process creation time and PID");
    SetWindowTextW(main,L"Song D - Artist");LoadTrayApps();RefreshTrayApps();a=&g_trayApps[0];
    CHECK(a->hwnd==main&&!lstrcmpW(a->title,L"Song D - Artist"),"saved selection reloads after the title changes while Dock is stopped");
    HWND second=Window(L"MiniDockDynamicTrayMain",L"Another player main",WS_EX_APPWINDOW,NULL);
    CHECK(second!=NULL,"create a second plausible main window");
    LoadTrayApps();RefreshTrayApps();a=&g_trayApps[0];
    CHECK(a->hwnd==main,"saved HWND/PID/start-time identity survives a changed title and a same-class competing main window");
    ULONGLONG birth=a->processStart;WCHAR number[32];
    _snwprintf(number,31,L"%llu",(unsigned long long)(birth+1));WritePrivateProfileStringW(L"TrayApps",L"Start0",number,g_ini);
    LoadTrayApps();RefreshTrayApps();
    CHECK(!g_trayApps[0].hwnd,"a stale process creation time cannot authenticate a saved HWND");
    _snwprintf(number,31,L"%llu",(unsigned long long)birth);WritePrivateProfileStringW(L"TrayApps",L"Start0",number,g_ini);
    wsprintfW(number,L"%lu",GetCurrentProcessId()+1);WritePrivateProfileStringW(L"TrayApps",L"Pid0",number,g_ini);
    LoadTrayApps();RefreshTrayApps();
    CHECK(!g_trayApps[0].hwnd,"a stale PID cannot authenticate a saved HWND");
    wsprintfW(number,L"%lu",GetCurrentProcessId());WritePrivateProfileStringW(L"TrayApps",L"Pid0",number,g_ini);
    _snwprintf(number,31,L"%llu",(unsigned long long)(UINT_PTR)aux);WritePrivateProfileStringW(L"TrayApps",L"Window0",number,g_ini);
    LoadTrayApps();RefreshTrayApps();
    CHECK(!g_trayApps[0].hwnd,"saved identity also verifies the selected window class");
    SetWindowLongW(second,GWL_STYLE,WS_POPUP|WS_CAPTION|WS_SYSMENU);
    LoadTrayApps();RefreshTrayApps();a=&g_trayApps[0];
    CHECK(a->hwnd==main,"saved main-window capabilities distinguish a login-style window after identity is lost");
    SetWindowTextW(second,L"Song C - Artist");LoadTrayApps();RefreshTrayApps();
    CHECK(g_trayApps[0].hwnd==main,"a helper copying the old caption cannot override the saved main-window capabilities");
    SetWindowLongW(second,GWL_STYLE,WS_OVERLAPPEDWINDOW);
    a=Selection(fixtureExe,L"Song A - Artist");RefreshTrayApps();
    CHECK(!a->hwnd&&!a->process,"ambiguous main-window candidates are not guessed by size or enumeration order");
    a=Selection(fixtureExe,L"Song D - Artist");RefreshTrayApps();
    CHECK(a->hwnd==main,"exact saved-title match remains preferred when several main windows exist");
    SetWindowTextW(second,L"Song D - Artist");a=Selection(fixtureExe,L"Song D - Artist");RefreshTrayApps();
    CHECK(!a->hwnd,"two identical title hints are ambiguous rather than resolved by enumeration order");
    DestroyWindow(second);
    lstrcpyW(fixtureExe,L"D:\\FixtureMusic\\another-player.exe");a=Selection(fixtureExe,L"Song A - Artist");RefreshTrayApps();
    CHECK(a->hwnd==main,"the same recovery works for another player without executable-name or class-name special cases");
    a=Selection(L"E:\\DifferentInstall\\another-player.exe",L"Song A - Artist");RefreshTrayApps();
    CHECK(!a->hwnd,"another executable path is never accepted as the selected music app");
    SetWindowTextW(main,L"");a=Selection(fixtureExe,L"Song A - Artist");RefreshTrayApps();
    CHECK(!a->hwnd,"an untitled main placeholder is not used as a fallback");
    SetWindowTextW(main,L"Temporary caption");a=Selection(fixtureExe,L"Temporary caption");RefreshTrayApps();SaveNow();
    SetWindowTextW(main,L"");LoadTrayApps();RefreshTrayApps();a=&g_trayApps[0];
    CHECK(a->hwnd==main,"an authenticated saved HWND remains selected when its caption becomes empty");
    SaveNow();LoadTrayApps();RefreshTrayApps();
    CHECK(g_ntrayApps==1&&g_trayApps[0].hwnd==main,"an empty display title never deletes a selection on reload");
    SetWindowTextW(main,L"Before recreation");a=Selection(fixtureExe,L"Before recreation");RefreshTrayApps();
    ULONGLONG retainedStart=a->processStart;DestroyWindow(main);
    main=Window(L"MiniDockDynamicTrayMain",L"After recreation",WS_EX_APPWINDOW,NULL);RefreshTrayApps();
    CHECK(main&&a->hwnd==main&&a->processStart==retainedStart,"a recreated main HWND with a new caption rebinds inside the retained live process");
    DestroyWindow(main);RefreshTrayApps();
    CHECK(!a->hwnd&&TrayProcessAlive(a),"losing the main HWND retains live process tracking without selecting a helper");
    a=Selection(fixtureExe,L"Song A - Artist");RefreshTrayApps();
    CHECK(!a->hwnd&&!a->process,"remaining lyrics, tool and owned helper windows cannot impersonate the missing main window");
    ClearTrayApps();DestroyWindow(tool);DestroyWindow(owned);DestroyWindow(aux);DeleteFileW(file);
    printf("PASS %d dynamic tray checks\n",checks);return 0;
}
