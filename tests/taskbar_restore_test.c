#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

/* Every taskbar HWND below is an owned hidden fixture; all Shell operations and
   visibility writes are intercepted. No actual taskbar or work area is modified. */
static HWND windows[4];
static BOOL visible[4], primaryPresent = TRUE;
static LPARAM shellState;
static int writes, shown, positioned, notified, enumerated;
static BOOL badFlags, badAppBar;
static int Index(HWND h) { for(int i=0;i<4;i++)if(windows[i]&&windows[i]==h)return i;return -1; }
static HWND WINAPI TestFind(LPCWSTR cls,LPCWSTR title) {
    (void)title;
    if(!lstrcmpW(cls,L"Shell_TrayWnd"))return primaryPresent?windows[0]:NULL;
    return NULL;
}
static int WINAPI TestClass(HWND h,LPWSTR text,int count) {
    int i=Index(h);if(i<0)return 0;
    const WCHAR *cls=i==0?L"Shell_TrayWnd":i==3?L"OrdinaryWindow":L"Shell_SecondaryTrayWnd";
    lstrcpynW(text,cls,count);return lstrlenW(text);
}
static BOOL WINAPI TestEnum(WNDENUMPROC callback,LPARAM data) {
    enumerated++;
    for(int i=0;i<4;i++)if((i!=0||primaryPresent)&&!callback(windows[i],data))return FALSE;
    return TRUE;
}
static BOOL WINAPI TestShow(HWND h,int command) {
    int i=Index(h);if(i<0)return FALSE;
    if(command==SW_HIDE)visible[i]=FALSE;
    else {visible[i]=TRUE;shown++;if(command!=SW_SHOWNA)badFlags=TRUE;}
    return TRUE;
}
static BOOL WINAPI TestPosition(HWND h,HWND after,int x,int y,int w,int height,UINT flags) {
    (void)x;(void)y;(void)w;(void)height;
    if(Index(h)<0)return FALSE;
    positioned++;
    if(after!=HWND_TOP || (flags&(SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW))!=
        (SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW))badFlags=TRUE;
    return TRUE;
}
static UINT_PTR WINAPI TestAppBar(DWORD message,PAPPBARDATA data) {
    if(data->cbSize!=sizeof *data||Index(data->hWnd)<0)badAppBar=TRUE;
    if(message==ABM_GETSTATE)return shellState;
    if(message==ABM_SETSTATE){shellState=data->lParam;writes++;return TRUE;}
    if(message==ABM_WINDOWPOSCHANGED){notified++;return TRUE;}
    return 0;
}
#define FindWindowW TestFind
#define GetClassNameW TestClass
#define EnumWindows TestEnum
#define ShowWindow TestShow
#define SetWindowPos TestPosition
#define SHAppBarMessage TestAppBar
#include "../MiniDock-source/minidock.c"

static int checks;
#define CHECK(condition,label) do{checks++;if(!(condition)){fprintf(stderr,"FAIL %d: %s\n",checks,label);return 1;}}while(0)
static void Reset(void) {
    ZeroMemory(visible,sizeof visible);writes=shown=positioned=notified=enumerated=0;
    badFlags=badAppBar=FALSE;primaryPresent=TRUE;shellState=ABS_AUTOHIDE;
    g_taskbarStateSaved=TRUE;g_taskbarWindowsTouched=TRUE;
    g_taskbarOriginalState=0;g_taskbarAppliedState=ABS_AUTOHIDE;
}
int main(void) {
    g_inst=GetModuleHandleW(NULL);DefaultCfg();
    WNDCLASSW cls={0};cls.hInstance=g_inst;cls.lpfnWndProc=DefWindowProcW;cls.lpszClassName=L"MiniDockPrivateTaskbarTest";
    CHECK(RegisterClassW(&cls),"register private fixture class");
    for(int i=0;i<4;i++)windows[i]=CreateWindowExW(WS_EX_TOOLWINDOW,cls.lpszClassName,L"",WS_POPUP,0,0,10,10,NULL,NULL,g_inst,NULL);
    CHECK(windows[0]&&windows[1]&&windows[2]&&windows[3],"create hidden taskbar and ordinary-window fixtures");
    Reset();RestoreTaskbar();
    CHECK(shellState==0&&writes==1,"normal exit restores the original non-autohide state, including zero");
    CHECK(visible[0]&&visible[1]&&visible[2]&&shown==3,"normal exit shows the primary and every secondary taskbar");
    CHECK(!visible[3]&&positioned==3&&!badFlags,"unrelated windows are untouched and taskbars retain position, size and focus");
    CHECK(notified==3&&!badAppBar,"Explorer receives valid position-change notices for each restored taskbar");
    CHECK(!g_taskbarStateSaved&&!g_taskbarWindowsTouched,"cleanup releases its ownership bookkeeping");
    RestoreTaskbar();
    CHECK(enumerated==1&&writes==1&&shown==3,"repeated cleanup is harmless and does not repeat state or visibility writes");
    Reset();g_taskbarOriginalState=ABS_AUTOHIDE;g_taskbarAppliedState=0;shellState=0;RestoreTaskbar();
    CHECK(shellState==ABS_AUTOHIDE&&writes==1,"a user who originally used Windows auto-hide gets that preference back");
    Reset();shellState=ABS_AUTOHIDE|ABS_ALWAYSONTOP;RestoreTaskbar();
    CHECK(shellState==(ABS_AUTOHIDE|ABS_ALWAYSONTOP)&&writes==0&&shown==3,"later external taskbar state changes are preserved while hidden windows are released");
    Reset();cfg.statusBar=0;cfg.taskHide=1;RestoreTaskbar();
    CHECK(shellState==0&&shown==3,"state restoration works without a top status bar and in ordinary auto-hide mode");
    Reset();g_taskbarStateSaved=FALSE;RestoreTaskbar();
    CHECK(writes==0&&shown==3,"touched taskbar windows are shown even when the original Shell state was unavailable");
    Reset();primaryPresent=FALSE;RestoreTaskbar();
    CHECK(writes==0&&shown==2&&visible[1]&&visible[2],"secondary taskbars recover even if the primary taskbar has disappeared");
    Reset();g_taskbarStateSaved=g_taskbarWindowsTouched=FALSE;RestoreTaskbar();
    CHECK(enumerated==0&&shown==0&&writes==0,"a process that never controlled the taskbar leaves it alone");
    Reset();g_taskbarStateSaved=g_taskbarWindowsTouched=FALSE;cfg.taskTrans=0;ApplyTaskbarWindow(windows[0],FALSE);
    CHECK(g_taskbarWindowsTouched&&!visible[0],"the production hide path records that it must release the window at exit");
    RestoreTaskbar();CHECK(shown==3,"a hide-only path is sufficient to schedule taskbar restoration");
    Reset();CHECK(ShowTaskbarOnExit(NULL,0)&&shown==0,"a window destroyed during enumeration is safely ignored");
    for(int i=0;i<4;i++)DestroyWindow(windows[i]);
    printf("PASS %d taskbar restore checks\n",checks);return 0;
}
