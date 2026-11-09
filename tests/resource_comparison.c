#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <shellapi.h>
static UINT_PTR WINAPI BenchTimer(HWND window, UINT_PTR timer, UINT interval, TIMERPROC procedure) { (void)window; (void)interval; (void)procedure; return timer ? timer : 1; }
static BOOL WINAPI BenchKillTimer(HWND window, UINT_PTR timer) { (void)window; (void)timer; return TRUE; }
#define SetTimer BenchTimer
#define KillTimer BenchKillTimer
#include MINIDOCK_BENCH_SOURCE
#include <stdlib.h>
static ULONGLONG CpuTicks(void){FILETIME a,b,k,u;GetProcessTimes(GetCurrentProcess(),&a,&b,&k,&u);return ((ULONGLONG)k.dwHighDateTime<<32)+k.dwLowDateTime+((ULONGLONG)u.dwHighDateTime<<32)+u.dwLowDateTime;}
static void Memory(PROCESS_MEMORY_COUNTERS_EX *m){ZeroMemory(m,sizeof *m);m->cb=sizeof *m;GetProcessMemoryInfo(GetCurrentProcess(),(PROCESS_MEMORY_COUNTERS*)m,sizeof *m);}
static void StepStatus(void){
#ifdef BENCH_STATUS_CURRENT
 (void)SbForegroundMode();
#else
 (void)GameActive();
#endif
 if(cfg.statusBar)SbDraw(g_sb);
}
int main(int argc,char**argv){
 if(argc<2)return 2;
 SetProcessDPIAware();OleInitialize(NULL);GdiplusStartupInput gi={1,NULL,FALSE,FALSE};if(GdiplusStartup(&g_gdipTok,&gi,NULL))return 3;
 g_inst=GetModuleHandleW(NULL);g_dpi=g_dpiCfg=96;g_noDl=TRUE;
 MultiByteToWideChar(CP_UTF8,0,argv[1],-1,g_dir,MAX_PATH);PathCombineW(g_ini,g_dir,L"MiniDock.ini");PathCombineW(g_iconDir,g_dir,L"icons");
 LoadConfig();cfg.minAnim=cfg.reserve=cfg.taskHide=cfg.pinMenu=cfg.guardian=cfg.grpFloat=cfg.updChk=0;cfg.logCollect=0;
#ifdef BENCH_GROUP_RUNNING
 if(argc>2)cfg.groupRunning=0; /* keep the existing upstream tray feature enabled */
#endif
 g_cacheSize=CacheFor(Pxs(cfg.zoom?(cfg.zoomSize>cfg.iconSize?cfg.zoomSize:cfg.iconSize):cfg.iconSize));
 GpImageAttributes**attributes[]={&g_attr,&g_attrFade,&g_attrFrost,&g_attrRefl,&g_attrRefl2};
 for(int i=0;i<5;i++){GdipCreateImageAttributes(attributes[i]);GdipSetImageAttributesWrapMode(*attributes[i],3,0,FALSE);}
 RebuildDockFonts();GdipCreateStringFormat(0,0,&g_fmtR);GdipSetStringFormatAlign(g_fmtR,2);GdipSetStringFormatLineAlign(g_fmtR,1);
 GdipCreateStringFormat(0,0,&g_fmtW);GdipCreateStringFormat(0,0,&g_fmtC);GdipSetStringFormatAlign(g_fmtC,1);GdipSetStringFormatLineAlign(g_fmtC,1);
 GdipCreateStringFormat(0x1000,0,&g_fmtL);GdipSetStringFormatLineAlign(g_fmtL,1);
 HDC screen=GetDC(NULL);g_memdc=CreateCompatibleDC(screen);g_fdc=CreateCompatibleDC(screen);ReleaseDC(NULL,screen);
 GetMon();LoadItems();UpdateGeometry();WNDCLASSW wc={0};wc.lpfnWndProc=DefWindowProcW;wc.hInstance=g_inst;wc.lpszClassName=L"MiniDockPrivateResourceBench";RegisterClassW(&wc);
 g_hwnd=CreateWindowExW(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,wc.lpszClassName,L"",WS_POPUP,winX,winY,winW,winH,NULL,NULL,g_inst,NULL);
 if(cfg.statusBar){int x,y,w,h;SbGeom(&x,&y,&w,&h);CanvasNew(&g_sbCv,w,h);SetupG(g_sbCv.g);g_sb=CreateWindowExW(WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,wc.lpszClassName,L"",WS_POPUP,x,y,w,h,NULL,NULL,g_inst,NULL);}
 int pinned=nItems, groups=0;for(int i=0;i<nItems;i++)if(items[i].type==T_GROUP)groups++;
 ScanRunning();for(int i=0;i<35;i++){RenderFrame();Sleep(10);}StepStatus();UpdateFrost();Sleep(1000);
 PROCESS_MEMORY_COUNTERS_EX before,after;Memory(&before);DWORD handlesBefore=0,handlesAfter=0;GetProcessHandleCount(GetCurrentProcess(),&handlesBefore);
 LARGE_INTEGER frequency,begin,now;QueryPerformanceFrequency(&frequency);QueryPerformanceCounter(&begin);ULONGLONG cpu=CpuTicks();
 double lastScan=0,lastStatus=0,lastFrost=0;int scans=0,draws=0;double elapsed;
 do{QueryPerformanceCounter(&now);elapsed=(double)(now.QuadPart-begin.QuadPart)/frequency.QuadPart;
 if(elapsed-lastScan>=3.5){ScanRunning();RenderFrame();lastScan=elapsed;scans++;}
 if(elapsed-lastStatus>=0.3){StepStatus();lastStatus=elapsed;draws++;}
 if(elapsed-lastFrost>=5){UpdateFrost();lastFrost=elapsed;}
 Sleep(10);
 }while(elapsed<10);
 double cpuMs=(CpuTicks()-cpu)/10000.0;Memory(&after);GetProcessHandleCount(GetCurrentProcess(),&handlesAfter);
 int runtime=0;for(int i=0;i<nItems;i++)if(items[i].temp&&items[i].type!=T_SEP)runtime++;
 printf("{\"elapsed_ms\":%.3f,\"cpu_ms\":%.3f,\"working_set\":%llu,\"private_bytes\":%llu,\"working_set_start\":%llu,\"private_start\":%llu,\"handles\":%lu,\"handle_delta\":%ld,\"pinned\":%d,\"groups\":%d,\"runtime\":%d,\"cache\":%d,\"dpi\":%d,\"scans\":%d,\"status_draws\":%d}\n",
 elapsed*1000,cpuMs,(unsigned long long)after.WorkingSetSize,(unsigned long long)after.PrivateUsage,(unsigned long long)before.WorkingSetSize,(unsigned long long)before.PrivateUsage,handlesAfter,(long)(handlesAfter-handlesBefore),pinned,groups,runtime,g_cacheSize,g_dpi,scans,draws);
 return 0;
}
