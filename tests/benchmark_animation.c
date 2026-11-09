#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <dwmapi.h>
#include <stdio.h>
#include <wchar.h>
static HWND fixture;
static int sourceMoves, previewFrames;
static double intervals[1024], lastFrame;
static LARGE_INTEGER frequency;
static BOOL forceDirect;
static double Now(void){LARGE_INTEGER t;QueryPerformanceCounter(&t);return (double)t.QuadPart*1000/frequency.QuadPart;}
static void Frame(void){double n=Now();if(lastFrame&&previewFrames<1024)intervals[previewFrames-1]=n-lastFrame;lastFrame=n;previewFrames++;}
static BOOL WINAPI CountPosition(HWND h,HWND after,int x,int y,int w,int height,UINT flags){if(h==fixture&&!(flags&SWP_NOSIZE)){sourceMoves++;if(forceDirect)Frame();}return SetWindowPos(h,after,x,y,w,height,flags);}
static HRESULT WINAPI CountUpdate(HTHUMBNAIL t,const DWM_THUMBNAIL_PROPERTIES*p){if(p->fVisible)Frame();return DwmUpdateThumbnailProperties(t,p);}
static HRESULT WINAPI ChooseRegister(HWND dst,HWND src,PHTHUMBNAIL t){return forceDirect?E_FAIL:DwmRegisterThumbnail(dst,src,t);}
#define SetWindowPos CountPosition
#define DwmUpdateThumbnailProperties CountUpdate
#define DwmRegisterThumbnail ChooseRegister
#include "../MiniDock-source/minidock.c"
static int sizes;
static LRESULT CALLBACK FixtureProc(HWND h,UINT m,WPARAM w,LPARAM l){
 if(m==WM_SIZE){sizes++;Sleep(8);}
 if(m==WM_APP+1)return sizes;
 if(m==WM_PAINT){PAINTSTRUCT ps;HDC dc=BeginPaint(h,&ps);RECT r;GetClientRect(h,&r);COLORREF colors[]={RGB(230,45,55),RGB(40,180,75),RGB(35,90,220)};for(int i=0;i<3;i++){RECT b={i*r.right/3,0,(i+1)*r.right/3,r.bottom};HBRUSH brush=CreateSolidBrush(colors[i]);FillRect(dc,&b,brush);DeleteObject(brush);}SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(255,255,255));TextOutW(dc,24,35,L"MiniDock animation test - own window",36);EndPaint(h,&ps);return 0;}
 if(m==WM_TIMER){DestroyWindow(h);return 0;}
 if(m==WM_DESTROY){PostQuitMessage(0);return 0;}return DefWindowProcW(h,m,w,l);
}
static BOOL CALLBACK FindFixture(HWND h,LPARAM p){DWORD pid;GetWindowThreadProcessId(h,&pid);if(pid==(DWORD)p){WCHAR cls[64];GetClassNameW(h,cls,64);if(!wcscmp(cls,L"MiniDockAnimationIntegrationFixture")){fixture=h;return FALSE;}}return TRUE;}
static void Pump(int ms){DWORD start=GetTickCount();MSG m;do{while(PeekMessageW(&m,NULL,0,0,PM_REMOVE))DispatchMessageW(&m);Sleep(5);}while(GetTickCount()-start<(DWORD)ms);}
static ULONGLONG Cpu(HANDLE p){FILETIME a,b,k,u;GetProcessTimes(p,&a,&b,&k,&u);return ((ULONGLONG)k.dwHighDateTime<<32)+k.dwLowDateTime+((ULONGLONG)u.dwHighDateTime<<32)+u.dwLowDateTime;}
static int Compare(const void*a,const void*b){double x=*(const double*)a,y=*(const double*)b;return(x>y)-(x<y);}
int main(int argc,char**argv){
 SetProcessDPIAware();g_inst=GetModuleHandleW(NULL);WNDCLASSW wc={0};wc.hInstance=g_inst;wc.lpfnWndProc=FixtureProc;wc.lpszClassName=L"MiniDockAnimationIntegrationFixture";RegisterClassW(&wc);
 if(argc>1){HWND h=CreateWindowExW(WS_EX_NOACTIVATE,wc.lpszClassName,L"MiniDock animation integration fixture",WS_OVERLAPPEDWINDOW,120,140,800,520,NULL,NULL,g_inst,NULL);SetTimer(h,1,30000,NULL);ShowWindow(h,SW_SHOWNOACTIVATE);MSG m;while(GetMessageW(&m,NULL,0,0)>0)DispatchMessageW(&m);return 0;}
 SetProcessDPIAware();QueryPerformanceFrequency(&frequency);DefaultCfg();cfg.minStyle=MIN_STYLE_SMOOTH;cfg.minAnim=1;cfg.bounce=0;
 wc.lpfnWndProc=DefWindowProcW;wc.lpszClassName=CLS_ANIM;RegisterClassW(&wc);g_hwnd=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,CLS_ANIM,L"",WS_POPUP,0,0,1,1,NULL,NULL,g_inst,NULL);
 WCHAR exe[MAX_PATH],command[MAX_PATH+30];GetModuleFileNameW(NULL,exe,MAX_PATH);swprintf(command,MAX_PATH+30,L"\"%s\" --child",exe);STARTUPINFOW si={sizeof si};PROCESS_INFORMATION child={0};if(!CreateProcessW(exe,command,NULL,NULL,FALSE,CREATE_NO_WINDOW,NULL,NULL,&si,&child))return 2;CloseHandle(child.hThread);
 for(int i=0;i<200&&!fixture;i++){EnumWindows(FindFixture,child.dwProcessId);Sleep(10);}if(!fixture){TerminateProcess(child.hProcess,3);CloseHandle(child.hProcess);return 3;}
 BOOL disable=TRUE;DwmSetWindowAttribute(fixture,3,&disable,sizeof disable);Pump(500);
 RECT icon={1100,800,1164,864};int failures=0;
 for(int round=0;round<2;round++)for(int path=0;path<2;path++)for(int restore=0;restore<2;restore++){
  forceDirect=(round==0?path==0:path==1);RECT from,target;WINDOWPLACEMENT original={sizeof original};GetWindowPlacement(fixture,&original);WindowShownRect(fixture,&from);
  sourceMoves=previewFrames=0;lastFrame=0;int before=(int)SendMessageW(fixture,WM_APP+1,0,0);ULONGLONG appCpu=Cpu(child.hProcess),dockCpu=Cpu(GetCurrentProcess());double start=Now();
  if(!FlyBegin(fixture,restore?icon:from,icon,restore,-1)){failures++;continue;}
  BOOL compositor=!g_animDirect;
  if(restore){if(!PrepareRestoreAtIcon(fixture,icon,FALSE,&target)){failures++;AnimFinish();continue;}g_animTo=target;}
  else if(compositor)CommitMinimizeKeepPlacement(fixture,&original);
  FlyRun(400);
  if(restore&&compositor){RestoreUnderPreview(fixture,FALSE);RECT visible;DwmGetWindowAttribute(fixture,9,&visible,sizeof visible);BOOL aligned=EqualRect(&visible,&g_animTo);printf("handoff_bounds_match=%d\n",aligned);failures+=!aligned;}
  if(!restore&&!compositor)CommitMinimizeKeepPlacement(fixture,&original);
  AnimFinish();double elapsed=Now()-start;Pump(150);BOOL correct=IsIconic(fixture)==!restore;failures+=!correct;
  int count=previewFrames-1;if(count>1023)count=1023;if(count>0)qsort(intervals,count,sizeof(double),Compare);
  printf("{\"round\":%d,\"path\":\"%s\",\"restore\":%d,\"elapsed_ms\":%.3f,\"frames\":%d,\"interval_p95_ms\":%.3f,\"source_resize_requests\":%d,\"wm_size\":%d,\"dock_cpu_ms\":%.3f,\"fixture_cpu_ms\":%.3f,\"state_ok\":%d}\n",round+1,compositor?"compositor":"direct",restore,elapsed,previewFrames,count>0?intervals[(count-1)*95/100]:0,sourceMoves,(int)SendMessageW(fixture,WM_APP+1,0,0)-before,(Cpu(GetCurrentProcess())-dockCpu)/10000.0,(Cpu(child.hProcess)-appCpu)/10000.0,correct);fflush(stdout);
 }
 RestoreTransitions();PostMessageW(fixture,WM_CLOSE,0,0);if(WaitForSingleObject(child.hProcess,3000)!=WAIT_OBJECT_0)TerminateProcess(child.hProcess,1);CloseHandle(child.hProcess);DestroyWindow(g_hwnd);return failures?1:0;
}
