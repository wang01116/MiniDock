#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <dwmapi.h>
#include <mmsystem.h>
#include <stdio.h>

/* Hidden private HWNDs with a deterministic source state. These substitutes test
   the animation protocol without moving, minimizing or styling any user window. */
static HWND sourceWindow, foregroundWindow;
static RECT sourceRect = {100, 120, 900, 720}, normalRect = {100, 120, 900, 720};
static LONG sourceExtended = WS_EX_TOOLWINDOW;
static BOOL sourceMinimized, sourceMaximized;
static BOOL failRegister, failQuery, failProperties;
static int registrations, releases, updates, sourceResizes, sourceStyles, alphaChanges;
static int framesMinimized, framesRestored, resolutionStarts, resolutionEnds;
static int activations, paintRequests, coveredFlushes;
static BOOL coveredActivation, coveredPaint, failPaint;
static BOOL transitionDisabled, failTransition, restoredWithTransitionsDisabled;
static int transitionSets;
static SIZE thumbnailSize = {800, 600};
static RECT lastPreviewRect;
static BOOL previewShown, thumbnailVisible, emptyPreviewShown, visibleThumbnailReleased;
static HWND TestForeground(void) { return foregroundWindow; }
static BOOL TestForegroundSet(HWND h) {
    foregroundWindow = h;
    if (h == sourceWindow) { activations++; coveredActivation = registrations > releases && !sourceMinimized; }
    return TRUE;
}
static BOOL TestIconic(HWND h) { return h == sourceWindow ? sourceMinimized : IsIconic(h); }
static BOOL TestZoomed(HWND h) { return h == sourceWindow ? sourceMaximized && !sourceMinimized : IsZoomed(h); }
static BOOL TestRect(HWND h, LPRECT r) { if (h == sourceWindow) { *r = sourceRect; return TRUE; } return GetWindowRect(h,r); }
static LONG TestLong(HWND h, int index) { if (h == sourceWindow && index == GWL_EXSTYLE) return sourceExtended; return GetWindowLongW(h,index); }
static LONG TestLongSet(HWND h, int index, LONG value) {
    if (h == sourceWindow && index == GWL_EXSTYLE) { LONG old=sourceExtended; sourceExtended=value; sourceStyles++; return old; }
    return SetWindowLongW(h,index,value);
}
static BOOL TestAlpha(HWND h, COLORREF color, BYTE alpha, DWORD flags) { if (h == sourceWindow) { alphaChanges++; return TRUE; } return SetLayeredWindowAttributes(h,color,alpha,flags); }
static BOOL TestPlacement(HWND h, WINDOWPLACEMENT *p) {
    if(h!=sourceWindow)return GetWindowPlacement(h,p);
    p->length=sizeof *p; p->flags=sourceMaximized?WPF_RESTORETOMAXIMIZED:0;
    p->showCmd=sourceMinimized?SW_SHOWMINIMIZED:(sourceMaximized?SW_SHOWMAXIMIZED:SW_SHOWNORMAL);p->rcNormalPosition=normalRect;return TRUE;
}
static BOOL TestShow(HWND h, int command) {
    if(h!=sourceWindow){if(command==SW_HIDE)previewShown=FALSE;return ShowWindow(h,SW_HIDE);}
    if(sourceMinimized && command!=SW_MINIMIZE && command!=SW_SHOWMINIMIZED)
        restoredWithTransitionsDisabled=transitionDisabled;
    sourceMinimized=command==SW_MINIMIZE||command==SW_SHOWMINIMIZED;
    if(!sourceMinimized)sourceRect=normalRect;
    if(command==SW_SHOWMAXIMIZED)sourceMaximized=TRUE;
    return TRUE;
}
static BOOL TestPlacementSet(HWND h, const WINDOWPLACEMENT *p) {
    if(h!=sourceWindow)return SetWindowPlacement(h,p);
    normalRect=p->rcNormalPosition;return TestShow(h,p->showCmd);
}
static BOOL TestPosition(HWND h, HWND after, int x,int y,int width,int height,UINT flags) {
    if(h!=sourceWindow){
        if(flags&SWP_SHOWWINDOW){previewShown=TRUE;if(!thumbnailVisible)emptyPreviewShown=TRUE;}
        if(!(flags&SWP_NOSIZE))SetRect(&lastPreviewRect,x,y,x+width,y+height);
        return SetWindowPos(h,after,x,y,width,height,flags&~SWP_SHOWWINDOW);
    }
    if(!(flags&SWP_NOMOVE)){OffsetRect(&sourceRect,x-sourceRect.left,y-sourceRect.top);}
    if(!(flags&SWP_NOSIZE)){sourceRect.right=sourceRect.left+width;sourceRect.bottom=sourceRect.top+height;sourceResizes++;}
    if(after==HWND_TOPMOST)sourceExtended|=WS_EX_TOPMOST;
    if(after==HWND_NOTOPMOST)sourceExtended&=~WS_EX_TOPMOST;
    return TRUE;
}
static HRESULT WINAPI TestRegister(HWND destination, HWND source, PHTHUMBNAIL thumbnail) {
    (void)destination;(void)source;registrations++;
    *thumbnail=failRegister?NULL:(HTHUMBNAIL)(UINT_PTR)1;return failRegister?E_ACCESSDENIED:S_OK;
}
static HRESULT WINAPI TestQuery(HTHUMBNAIL thumbnail, PSIZE size) { (void)thumbnail;*size=thumbnailSize;return failQuery?E_FAIL:S_OK; }
static HRESULT WINAPI TestUpdate(HTHUMBNAIL thumbnail,const DWM_THUMBNAIL_PROPERTIES *properties) {
    (void)thumbnail;updates++;
    thumbnailVisible=properties->fVisible;
    if(properties->fVisible){if(sourceMinimized)framesMinimized++;else framesRestored++;}
    return failProperties?E_FAIL:S_OK;
}
static HRESULT WINAPI TestUnregister(HTHUMBNAIL thumbnail) { (void)thumbnail;if(previewShown)visibleThumbnailReleased=TRUE;thumbnailVisible=FALSE;releases++;return S_OK; }
static HRESULT WINAPI TestDwmGet(HWND h,DWORD attribute,PVOID value,DWORD size) {
    if(h==sourceWindow&&attribute==9&&size==sizeof(RECT)){*(RECT*)value=sourceRect;return S_OK;}
    return DwmGetWindowAttribute(h,attribute,value,size);
}
static HRESULT WINAPI TestDwmSet(HWND h,DWORD attribute,LPCVOID value,DWORD size) {
    if(h==sourceWindow){
        if(attribute==3 && size==sizeof(BOOL)) {
            transitionSets++;
            if(failTransition)return E_ACCESSDENIED;
            transitionDisabled=*(const BOOL*)value;
        }
        return S_OK;
    }
    return DwmSetWindowAttribute(h,attribute,value,size);
}
static HRESULT WINAPI TestFlush(void) { if(!sourceMinimized&&registrations>releases)coveredFlushes++;Sleep(1);return S_OK; }
static LRESULT WINAPI TestPaint(HWND h,UINT message,WPARAM w,LPARAM l,UINT flags,UINT timeout,PDWORD_PTR result) {
    if(h!=sourceWindow)return SendMessageTimeoutW(h,message,w,l,flags,timeout,result);
    if(message==WM_PAINT){
        paintRequests++;
        coveredPaint=registrations>releases&&!sourceMinimized&&foregroundWindow==sourceWindow&&timeout==40&&
            (flags&SMTO_ABORTIFHUNG)&&(flags&SMTO_ERRORONEXIT);
        if(result)*result=0;return !failPaint;
    }
    return 0;
}
static MMRESULT WINAPI TestResolutionStart(UINT period) { (void)period;resolutionStarts++;return TIMERR_NOERROR; }
static MMRESULT WINAPI TestResolutionEnd(UINT period) { (void)period;resolutionEnds++;return TIMERR_NOERROR; }
/* Protocol tests must never capture or cover the actual desktop. */
static HDC WINAPI TestGetDC(HWND window) { return window ? GetDC(window) : NULL; }
static BOOL WINAPI TestEnumWindows(WNDENUMPROC callback, LPARAM arg) { return callback(sourceWindow, arg); }
static BOOL WINAPI TestVisible(HWND window) { return window == sourceWindow || IsWindowVisible(window); }
#define EnumWindows TestEnumWindows
#define IsWindowVisible TestVisible
#define GetDC TestGetDC
#define GetForegroundWindow TestForeground
#define SetForegroundWindow TestForegroundSet
#define IsIconic TestIconic
#define IsZoomed TestZoomed
#define GetWindowRect TestRect
#define GetWindowLongW TestLong
#define SetWindowLongW TestLongSet
#define SetLayeredWindowAttributes TestAlpha
#define GetWindowPlacement TestPlacement
#define SetWindowPlacement TestPlacementSet
#define ShowWindow TestShow
#define ShowWindowAsync TestShow
#define SetWindowPos TestPosition
#define DwmRegisterThumbnail TestRegister
#define DwmQueryThumbnailSourceSize TestQuery
#define DwmUpdateThumbnailProperties TestUpdate
#define DwmUnregisterThumbnail TestUnregister
#define DwmGetWindowAttribute TestDwmGet
#define DwmSetWindowAttribute TestDwmSet
#define DwmFlush TestFlush
#define SendMessageTimeoutW TestPaint
#define timeBeginPeriod TestResolutionStart
#define timeEndPeriod TestResolutionEnd
#include "../MiniDock-source/minidock.c"

static int checks;
#define CHECK(condition,label) do{checks++;if(!(condition)){fprintf(stderr,"FAIL %d: %s\n",checks,label);return 1;}}while(0)
static void ResetFlight(BOOL minimized) {
    registrations=releases=updates=sourceResizes=sourceStyles=alphaChanges=0;
    framesMinimized=framesRestored=resolutionStarts=resolutionEnds=0;
    activations=paintRequests=coveredFlushes=0;coveredActivation=coveredPaint=failPaint=FALSE;
    thumbnailSize.cx=800;thumbnailSize.cy=600;SetRectEmpty(&lastPreviewRect);
    previewShown=thumbnailVisible=emptyPreviewShown=visibleThumbnailReleased=FALSE;
    failRegister=failQuery=failProperties=FALSE;sourceMinimized=minimized;sourceMaximized=FALSE;
    SetRect(&normalRect,100,120,900,720);sourceRect=normalRect;sourceExtended=WS_EX_TOOLWINDOW;
    foregroundWindow=minimized?NULL:sourceWindow;cfg.minStyle=MIN_STYLE_SMOOTH;cfg.minAnim=1;cfg.animSpeed=200;cfg.bounce=0;
}
int main(void) {
    g_inst=GetModuleHandleW(NULL);g_dpi=g_dpiCfg=96;DefaultCfg();
    CHECK(cfg.minStyle==MIN_STYLE_COMPRESS,"original compression is the default animation style");
    WNDCLASSW wc={0};wc.hInstance=g_inst;wc.lpfnWndProc=DefWindowProcW;wc.lpszClassName=CLS_ANIM;
    CHECK(RegisterClassW(&wc),"register private animation window class");
    sourceWindow=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,CLS_ANIM,L"Private animation source",WS_POPUP,100,120,800,600,NULL,NULL,g_inst,NULL);
    g_hwnd=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,CLS_ANIM,L"Private Dock",WS_POPUP,800,800,400,100,NULL,NULL,g_inst,NULL);
    CHECK(sourceWindow&&g_hwnd,"create hidden private source and Dock windows");
    nItems=1;items[0].type=T_WIN;items[0].hwnd=sourceWindow;items[0].cur=48;
    slotL[0]=20;slotR[0]=68;barBottom=80;M.pad=8;winX=800;winY=800;
    failTransition=TRUE;DisableTransitions(sourceWindow,TRUE);
    CHECK(g_nfdis==0,"a rejected DWM transition update is not cached as successful");
    failTransition=FALSE;DisableTransitions(sourceWindow,TRUE);
    CHECK(g_nfdis==1&&transitionDisabled,"a rejected transition update can be retried");
    RestoreTransitions();
    ResetFlight(FALSE);ToggleWindowIdx(sourceWindow,0);
    CHECK(sourceMinimized,"compositor flight commits the native minimized state");
    CHECK(registrations==1&&releases==1&&updates>3,"compositor preview is used and released after minimize");
    CHECK(sourceResizes==0&&sourceStyles==0&&alphaChanges==0,"compression does not resize or restyle the application each frame");
    CHECK(framesMinimized>2,"the application remains minimized while the preview flies into the Dock");
    CHECK(EqualRect(&sourceRect,&normalRect),"native minimize preserves the application restore rectangle");
    CHECK(!g_anim&&!g_animTh&&!g_animBusy&&!g_animWin,"minimize releases all animation state");
    CHECK(resolutionStarts==1&&resolutionEnds==1,"animation-only timer resolution is balanced after minimize");
    ResetFlight(TRUE);
    transitionDisabled=FALSE;restoredWithTransitionsDisabled=FALSE;int setsBeforeRestore=transitionSets;
    ToggleWindowIdx(sourceWindow,0);
    CHECK(transitionSets>setsBeforeRestore&&restoredWithTransitionsDisabled&&g_nfdis==1,
          "restore refreshes a tracked window's changed DWM attribute before showing it without duplicating cleanup entries");
    CHECK(!sourceMinimized&&foregroundWindow==sourceWindow,"compositor restore reuses and activates the existing window");
    CHECK(registrations==1&&releases==1&&framesMinimized>2,"restore animates the minimized preview before committing the real window");
    CHECK(sourceResizes==0&&sourceStyles==0&&alphaChanges==0,"restore does not repeatedly resize or change window styles");
    CHECK(EqualRect(&sourceRect,&normalRect),"restored window preserves its placement");
    CHECK(resolutionStarts==1&&resolutionEnds==1&&!g_anim&&!g_animTh,"restore balances scheduling and releases the preview");
    CHECK(activations==1&&coveredActivation,"restore activates exactly once while the covering preview is still alive");
    CHECK(paintRequests==1&&coveredPaint,"the application's pending paint is requested under cover with a bounded timeout");
    CHECK(framesRestored==0&&coveredFlushes>=2,"the landed preview is not retargeted after restoring and survives the activation/paint handoff");
    CHECK(!emptyPreviewShown,"preview content is installed before its popup is first shown");
    CHECK(!visibleThumbnailReleased&&!previewShown,"the preview is hidden before unregistering its thumbnail so no empty popup can be composed");
    ResetFlight(TRUE);thumbnailSize.cx=782;thumbnailSize.cy=591;ToggleWindowIdx(sourceWindow,0);
    RECT visible=normalRect;visible.left+=9;visible.right-=9;visible.bottom-=9;
    CHECK(EqualRect(&lastPreviewRect,&visible),"normal restore lands at the visible frame without invisible resize borders");
    CHECK(sourceResizes==0&&framesRestored==0&&coveredActivation,"border correction does not resize the application or snap the preview after restoration");
    ResetFlight(TRUE);thumbnailSize.cx=256;thumbnailSize.cy=128;ToggleWindowIdx(sourceWindow,0);
    CHECK(EqualRect(&lastPreviewRect,&normalRect),"an unrelated iconic thumbnail size does not produce a large guessed frame trim");
    ResetFlight(TRUE);failPaint=TRUE;ToggleWindowIdx(sourceWindow,0);
    CHECK(!sourceMinimized&&!g_anim&&!g_animTh&&!g_animBusy&&coveredPaint,"a timed-out application paint still releases the preview and leaves the restored window accessible");
    ResetFlight(TRUE);sourceMaximized=TRUE;ToggleWindowIdx(sourceWindow,0);
    CHECK(!sourceMinimized&&sourceMaximized,"maximized restore retains the native maximize state");
    CHECK(sourceResizes==0&&releases==1&&!g_anim,"maximized restore also avoids per-frame application resizing");
    MONITORINFO monitor={sizeof monitor};GetMonitorInfoW(MonitorFromWindow(sourceWindow,MONITOR_DEFAULTTONEAREST),&monitor);
    CHECK(EqualRect(&lastPreviewRect,&monitor.rcWork),"maximized preview lands at the work area without inflating and clamping invisible borders");
    ResetFlight(TRUE);sourceMaximized=TRUE;thumbnailSize.cx=monitor.rcMonitor.right-monitor.rcMonitor.left;
    thumbnailSize.cy=monitor.rcMonitor.bottom-monitor.rcMonitor.top;ToggleWindowIdx(sourceWindow,0);
    CHECK(EqualRect(&lastPreviewRect,&monitor.rcMonitor),"borderless fullscreen surfaces that retain a maximize flag land at the full monitor");
    ResetFlight(FALSE);failRegister=TRUE;ToggleWindowIdx(sourceWindow,0);
    CHECK(sourceMinimized&&registrations==1&&releases==0,"unavailable compositor falls back to a valid minimize");
    CHECK(sourceResizes>2&&!(sourceExtended&WS_EX_TOPMOST),"direct compatibility flight remains available and restores topmost state");
    CHECK(!g_anim&&!g_animTh&&resolutionStarts==resolutionEnds,"direct fallback leaves no animation resources or timer-resolution reference");
    ResetFlight(TRUE);failQuery=TRUE;ToggleWindowIdx(sourceWindow,0);
    CHECK(!sourceMinimized&&sourceResizes>2,"invalid preview dimensions fall back to direct restore");
    CHECK(releases==1&&!g_animTh&&!g_anim,"failed preview initialization is unregistered before fallback");
    ResetFlight(FALSE);failProperties=TRUE;ToggleWindowIdx(sourceWindow,0);
    CHECK(sourceMinimized&&sourceResizes>2&&releases==1,"preview property failure releases its handle and uses direct flight");
    ResetFlight(FALSE);cfg.minStyle=MIN_STYLE_COMPRESS;ToggleWindowIdx(sourceWindow,0);
    CHECK(sourceMinimized&&registrations==0&&sourceResizes>2,"classic compression minimizes with the real window and no compositor preview");
    CHECK(resolutionStarts==0&&resolutionEnds==0,"classic minimize uses the original pacing");
    ResetFlight(TRUE);cfg.minStyle=MIN_STYLE_COMPRESS;ToggleWindowIdx(sourceWindow,0);
    CHECK(!sourceMinimized&&registrations==0&&sourceResizes>2&&paintRequests==0,"classic restore uses the real window without snapshot handoff");
    CHECK(resolutionStarts==0&&resolutionEnds==0,"classic restore uses the original pacing");
    ResetFlight(FALSE);cfg.minStyle=MIN_STYLE_ICON_FLY;ToggleWindowIdx(sourceWindow,0);
    CHECK(sourceMinimized&&registrations==0&&sourceResizes>2,"the existing icon-fly style retains its compatibility behavior");
    ResetFlight(FALSE);sourceExtended|=WS_EX_LAYERED;LONG originalStyle=sourceExtended;ToggleWindowIdx(sourceWindow,0);
    CHECK(sourceMinimized&&sourceExtended==originalStyle&&sourceStyles==0&&alphaChanges==0,"compositor flight preserves an application's existing layered style and opacity");
    ResetFlight(FALSE);cfg.minAnim=0;ToggleWindowIdx(sourceWindow,0);
    CHECK(sourceMinimized&&registrations==0&&resolutionStarts==0,"disabling Dock animation bypasses preview allocation and frame scheduling");
    ResetFlight(FALSE);foregroundWindow=NULL;ToggleWindowIdx(sourceWindow,0);
    CHECK(!sourceMinimized&&foregroundWindow==sourceWindow&&registrations==0,"a background application is brought forward without an unnecessary flight");
    items[0].type=T_FILE;
    CHECK(GetModuleFileNameW(NULL,items[0].path,MAX_PATH)>0,"bind the Dock icon to the test executable");
    lstrcpyW(items[0].target,items[0].path);
    for (int style=MIN_STYLE_COMPRESS;style<=MIN_STYLE_SMOOTH;style++) {
        ResetFlight(TRUE);cfg.minStyle=style;cfg.clickToggle=0;
        sourceExtended=0;items[0].temp=(style==MIN_STYLE_SMOOTH);
        LaunchItem(0,FALSE);
        CHECK(!sourceMinimized&&(style==MIN_STYLE_SMOOTH?registrations==1:sourceResizes>2),
              "a Dock icon animates restore independently of repeat-click minimization");
        LaunchItem(0,FALSE);
        CHECK(!sourceMinimized,"disabling repeat-click minimization keeps the foreground window open");
        cfg.clickToggle=1;
        LaunchItem(0,FALSE);
        CHECK(sourceMinimized&&(style==MIN_STYLE_SMOOTH?registrations==2:sourceResizes>2),
              "enabling repeat-click minimization uses the selected animation from the Dock");
    }
    RestoreTransitions();DestroyWindow(sourceWindow);DestroyWindow(g_hwnd);g_hwnd=NULL;nItems=0;
    printf("PASS %d animation checks\n",checks);return 0;
}
