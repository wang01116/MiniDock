/*  MiniDock 1.1 - tiny macOS-style dock for Windows (x64).  Made by Wangcy
 *  Pure Win32 + GDI+ flat API; portable; config stored next to the exe.
 */
#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <windowsx.h>
#include <imm.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <commctrl.h>
#include <commoncontrols.h>
#include <commdlg.h>
#include <urlmon.h>
#include <oleidl.h>
#include <dwmapi.h>
#include <math.h>
#include <wchar.h>
#include <psapi.h>
#include <iphlpapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <ntddvdeo.h>
#include <iptypes.h>
#include <stdio.h>
#include <richedit.h>
#include <tlhelp32.h>
#include <wtsapi32.h>
#include <propsys.h>
#include <mmsystem.h>   /* timeBeginPeriod / timeEndPeriod for high-refresh animation pacing */

#define APPVER L"1.7.4"

/* ---------------- minimal GDI+ flat API ---------------- */
typedef int GpStatus;
typedef float REAL;
typedef DWORD ARGB;
typedef void GpGraphics, GpImage, GpBitmap, GpBrush, GpPath, GpPen, GpFont, GpFontFamily, GpStringFormat, GpImageAttributes;
typedef struct { UINT32 GdiplusVersion; void *DebugEventCallback; BOOL SuppressBackgroundThread; BOOL SuppressExternalCodecs; } GdiplusStartupInput;
typedef struct { REAL X, Y, Width, Height; } RectF;
typedef struct { INT X, Y, Width, Height; } GpRect;
typedef struct { UINT Width, Height; INT Stride; INT PixelFormat; void *Scan0; UINT_PTR Reserved; } GpBitmapData;
#define GPAPI WINAPI
#define PXF_PARGB 0xE200B
#define PXF_ARGB 0x26200A
GpStatus GPAPI GdiplusStartup(ULONG_PTR *, const GdiplusStartupInput *, void *);
void GPAPI GdiplusShutdown(ULONG_PTR);
GpStatus GPAPI GdipCreateBitmapFromScan0(INT, INT, INT, INT, BYTE *, GpBitmap **);
GpStatus GPAPI GdipCreateBitmapFromHICON(HICON, GpBitmap **);
GpStatus GPAPI GdipCreateBitmapFromHBITMAP(HBITMAP, HPALETTE, GpBitmap **);
GpStatus GPAPI GdipCreateBitmapFromFile(const WCHAR *, GpBitmap **);
GpStatus GPAPI GdipBitmapLockBits(GpBitmap *, const GpRect *, UINT, INT, GpBitmapData *);
GpStatus GPAPI GdipBitmapUnlockBits(GpBitmap *, GpBitmapData *);
GpStatus GPAPI GdipGetImageWidth(GpImage *, UINT *);
GpStatus GPAPI GdipGetImageHeight(GpImage *, UINT *);
GpStatus GPAPI GdipDisposeImage(GpImage *);
GpStatus GPAPI GdipGetImageGraphicsContext(GpImage *, GpGraphics **);
GpStatus GPAPI GdipCreateFromHDC(HDC, GpGraphics **);
GpStatus GPAPI GdipDeleteGraphics(GpGraphics *);
GpStatus GPAPI GdipGraphicsClear(GpGraphics *, ARGB);
GpStatus GPAPI GdipSetSmoothingMode(GpGraphics *, INT);
GpStatus GPAPI GdipSetInterpolationMode(GpGraphics *, INT);
GpStatus GPAPI GdipSetPixelOffsetMode(GpGraphics *, INT);
GpStatus GPAPI GdipSetTextRenderingHint(GpGraphics *, INT);
GpStatus GPAPI GdipResetWorldTransform(GpGraphics *);
GpStatus GPAPI GdipTranslateWorldTransform(GpGraphics *, REAL, REAL, INT);
GpStatus GPAPI GdipRotateWorldTransform(GpGraphics *, REAL, INT);
GpStatus GPAPI GdipSetClipPath(GpGraphics *, GpPath *, INT);
GpStatus GPAPI GdipResetClip(GpGraphics *);
GpStatus GPAPI GdipCreateSolidFill(ARGB, GpBrush **);
GpStatus GPAPI GdipDeleteBrush(GpBrush *);
GpStatus GPAPI GdipCreateLineBrushFromRect(const RectF *, ARGB, ARGB, INT, INT, GpBrush **);
GpStatus GPAPI GdipCreatePen1(ARGB, REAL, INT, GpPen **);
GpStatus GPAPI GdipDeletePen(GpPen *);
GpStatus GPAPI GdipSetPenStartCap(GpPen *, INT);
GpStatus GPAPI GdipSetPenEndCap(GpPen *, INT);
GpStatus GPAPI GdipSetPenLineJoin(GpPen *, INT);
GpStatus GPAPI GdipCreatePath(INT, GpPath **);
GpStatus GPAPI GdipDeletePath(GpPath *);
GpStatus GPAPI GdipAddPathArc(GpPath *, REAL, REAL, REAL, REAL, REAL, REAL);
GpStatus GPAPI GdipClosePathFigure(GpPath *);
GpStatus GPAPI GdipFillPath(GpGraphics *, GpBrush *, GpPath *);
GpStatus GPAPI GdipDrawPath(GpGraphics *, GpPen *, GpPath *);
GpStatus GPAPI GdipFillRectangle(GpGraphics *, GpBrush *, REAL, REAL, REAL, REAL);
GpStatus GPAPI GdipFillEllipse(GpGraphics *, GpBrush *, REAL, REAL, REAL, REAL);
GpStatus GPAPI GdipDrawLine(GpGraphics *, GpPen *, REAL, REAL, REAL, REAL);
GpStatus GPAPI GdipDrawEllipse(GpGraphics *, GpPen *, REAL, REAL, REAL, REAL);
GpStatus GPAPI GdipDrawArc(GpGraphics *, GpPen *, REAL, REAL, REAL, REAL, REAL, REAL);
GpStatus GPAPI GdipDrawRectangle(GpGraphics *, GpPen *, REAL, REAL, REAL, REAL);
BOOLEAN WINAPI SetSuspendState(BOOLEAN, BOOLEAN, BOOLEAN);
GpStatus GPAPI GdipSetClipRect(GpGraphics *, REAL, REAL, REAL, REAL, INT);
GpStatus GPAPI GdipResetClip(GpGraphics *);
GpStatus GPAPI GdipDrawImageRectRect(GpGraphics *, GpImage *, REAL, REAL, REAL, REAL, REAL, REAL, REAL, REAL, INT, const GpImageAttributes *, void *, void *);
GpStatus GPAPI GdipCreateImageAttributes(GpImageAttributes **);
GpStatus GPAPI GdipSetImageAttributesWrapMode(GpImageAttributes *, INT, ARGB, BOOL);
GpStatus GPAPI GdipSetImageAttributesColorMatrix(GpImageAttributes *, INT, BOOL, const float *, const float *, INT);
GpStatus GPAPI GdipCreateFontFamilyFromName(const WCHAR *, void *, GpFontFamily **);
GpStatus GPAPI GdipGetGenericFontFamilySansSerif(GpFontFamily **);
GpStatus GPAPI GdipCreateFont(const GpFontFamily *, REAL, INT, INT, GpFont **);
GpStatus GPAPI GdipDeleteFont(GpFont *);
GpStatus GPAPI GdipCreateStringFormat(INT, LANGID, GpStringFormat **);
GpStatus GPAPI GdipSetStringFormatAlign(GpStringFormat *, INT);
GpStatus GPAPI GdipSetStringFormatLineAlign(GpStringFormat *, INT);
GpStatus GPAPI GdipSetStringFormatTrimming(GpStringFormat *, INT);
GpStatus GPAPI GdipDrawString(GpGraphics *, const WCHAR *, INT, const GpFont *, const RectF *, const GpStringFormat *, const GpBrush *);
GpStatus GPAPI GdipDrawImagePointsRect(GpGraphics *, GpImage *, const float *, INT, REAL, REAL, REAL, REAL, INT, const GpImageAttributes *, void *, void *);
typedef struct { REAL X, Y; } PointF;
GpStatus GPAPI GdipCreateLineBrush(const PointF *, const PointF *, ARGB, ARGB, INT, GpBrush **);
GpStatus GPAPI GdipSetLinePresetBlend(GpBrush *, const ARGB *, const REAL *, INT);
GpStatus GPAPI GdipFillPie(GpGraphics *, GpBrush *, REAL, REAL, REAL, REAL, REAL, REAL);
GpStatus GPAPI GdipMeasureString(GpGraphics *, const WCHAR *, INT, const GpFont *, const RectF *, const GpStringFormat *, RectF *, INT *, INT *);

static const GUID MY_IID_IImageList = {0x46EB5926,0x582E,0x4017,{0x9F,0xDF,0xE8,0x99,0x8D,0xAA,0x09,0x50}};
#define ARGBC(a, r, g, b) ((ARGB)(((DWORD)(a) << 24) | ((DWORD)(r) << 16) | ((DWORD)(g) << 8) | (DWORD)(b)))
#define PI_F 3.14159265f

/* ---------------- types & globals ---------------- */
#define MAXI 128
#define MAXK 16
#define WM_APP_ICON (WM_APP + 1)
#define WM_APP_TRAY (WM_APP + 2)
#define WM_APP_AB   (WM_APP + 3)
#define WM_APP_FAIL (WM_APP + 4)
#define T_ANIM 1
#define T_SCAN 2
#define T_HIDE 3
#define T_SAVE 4
#define T_TRIM 5
#define T_HOVER 6
#define T_MIN 7
#define T_CLOCK 8
#define CLS_DOCK L"MiniDockWnd"
#define CLS_SET  L"MiniDockSettings"
#define CLS_IN   L"MiniDockInput"
#define CLS_MENU L"MiniDockMenu"
#define CLS_GRP  L"MiniDockGroup"
#define CLS_PV   L"MiniDockPreview"
#define CLS_FAN  L"MiniDockFan"
#define CLS_NOTE L"MiniDockNote"
#define CLS_ICOD L"MiniDockIconDlg"
#define CLS_ANIM L"MiniDockAnim"
#define CLS_BACK L"MiniDockBackdrop"
#define T_NOTEICON 9

enum { T_FILE = 0, T_FOLDER = 1, T_URL = 2, T_SEP = 3, T_GROUP = 4, T_TRASH = 5, T_NOTE = 6, T_WIN = 7, T_TRAY = 8 };
enum { E_BOTTOM = 0, E_TOP = 1, E_LEFT = 2, E_RIGHT = 3 };

typedef struct Item {
    int type, appmode, fav, temp, nk, fit;
    WCHAR args[260];
    WCHAR name[128];
    WCHAR path[1024];
    WCHAR icon_src[512];
    WCHAR icon_dark[512];   /* optional dark-mode variant of the icon */
    WCHAR target[MAX_PATH];
    WCHAR tdir[MAX_PATH];   /* install folder of the target: matches launchers / suites whose windows belong to a sibling exe */
    WCHAR aumid[160];       /* UWP app id (shell:AppsFolder\...): matches windows hosted by ApplicationFrameHost */
    GpBitmap *icon;
    GpBitmap *iconDark;
    float cur, slide, prevL;
    BOOL running, notify;
    HWND hwnd;
    DWORD snapTick;
    DWORD bounce;
    struct Item *kids;
} Item;

enum { MIN_STYLE_COMPRESS, MIN_STYLE_ICON_FLY, MIN_STYLE_SMOOTH };

typedef struct {
    int iconSize, zoomSize, spacing, margin, pad, hPad, radius, opacity, blur;
    int zoom, theme, dots, labels, bounce, webApp, border, bgMode;
    int mode, reserve, edge, monitor, preview, showRunning, lock, hotkey;
    int dotStyle, plateMode, plateTheme, plateOpacity, plateRadius, bgColor1, bgColor2, bgColor3, accentTone;
    int reflect, clock, folderFan, minAnim, notes, uiTheme, menuTheme;
    int borderColor, borderWidth, sepColor, dotColor, gradAngle, gradA1, gradA2, gradA3;
    int clockShow, clockColLight, clockColDark, reflSize, reflGap, reflOpacity;
    int fanArc, fanGap, fanCut, groupFrost, groupPos, guardian, clickToggle;
    int sepGap, sepWidth, clockSec, clockWeek, dateFmt, clockOrder, clockBoldT, clockBoldD, browser, grpOpacity, grpBg, minStyle;
    int mnAuto, mnNotes, mnAdd, launchpad;
    int wSide, wOrder, notifyW, fanBgL, fanBgD, fanFgL, fanFgD, pinMenu, multiMode, updChk, updLast, lpCols, lpRows, fanDrag, hideTaskbar, trayHide, ctrlCenter, ccEnergy, ccHotspot, resMon, toolW, wideMask, modeW, uiFrost, lpFrost, trayW, wMerge, statusBar, sbFrost, sbOpacity, sbColor, sbRadius, sbHeight, sbFollowSys, sbTopGap, sbMaxAvoid, sbFloat, sbSideGap, sbFullscreenHide, sbBattery, sepScale, menuFrost;
    int sbBorder;      /* draw the hairline frame around the top status bar */
    int grpFloat;      /* show the app groups in their own always-on-top strip (independent of the dock) */
    int hkMods, hkVk;  /* customisable summon hotkey */
    int gameGuard, gameAuto;   /* game mode: never reveal the dock over a game */
    int lpTabs;        /* launchpad: allow switching the tag tabs */
    int lpOpacity;     /* launchpad background opacity, independent of window fade */
    int reflMode, reflGrad;    /* 0 = below the bar, 1 = inside the bar; 0 fade, 1 linear, 2 solid */
    int iconDarkMode, iconDarkFrom, iconDarkTo;   /* 0 off, 1 follow system, 2 by time */
    int mnTaskMgr;     /* show "Task Manager" in the right-click menus */
    int powerW;        /* power widget: shut down / restart / lock */
    int taskHide;      /* 0 off, 1 auto-hide (hover shows), 2 hidden for good */
    int taskTop;       /* taskbar parked at the top edge */
    int taskTrans, taskAlpha;   /* taskbar transparency */
    int ecoMode;       /* master switch for the expensive effects (reflections stay independent) */
    int batEco, batLow;   /* low battery: switch eco mode on automatically */
    int sbPopFrost, sbAutoOpaque, taskPeek, sbPopBlur, pvList;
    int frostHold, animSpeed, fanReverse; /* configurable backdrop refresh and fly animation timing */
    int logCollect; /* opt-in diagnostic log collection; no window titles or document paths */
    int cmMerge, cmSep, cmIcon, cmRename, cmCopy, cmDock; /* per-command visibility in Dock context menus */
    int groupRunning; /* also show grouped applications in the existing running-app section */
} Cfg;

static Cfg cfg;
#define MAXTRAYAPPS 16
#define TRAY_SEP 10
typedef struct {
    WCHAR exe[MAX_PATH], cls[128], title[256], name[128];
    HWND hwnd;
    DWORD pid;
    HANDLE process;
    ULONGLONG processStart;
    DWORD roleStyle, roleExStyle;
    BOOL roleKnown;
} TrayApp;
static TrayApp g_trayApps[MAXTRAYAPPS];
static int g_ntrayApps;
static float PopBlurOp(void);
static Item items[MAXI];
static int nItems;
static float Asp(const Item *it) { return (it->type == T_GROUP && (it->appmode & 2)) ? 2.f : 1.f; }   /* wide groups take two icon slots */
static HINSTANCE g_inst;
static HWND g_hwnd, g_hset;
static ULONG_PTR g_gdipTok;
static int g_dpi = 96;   /* effective DPI of the monitor the dock currently sits on */
static int g_dpiCfg = 96;   /* the DPI the pixel values stored in Cfg were authored at */
static float DpiK(void) { return g_dpiCfg > 0 ? (float)g_dpi / (float)g_dpiCfg : 1.f; }
static int Pxs(int v) { float f = (float)v * DpiK(); return f < 0 ? 0 : (int)(f + 0.5f); }
static void RebuildDockFonts(void);
static void ReloadAllIcons(void);
static int CacheFor(int z);
static WCHAR g_ini[MAX_PATH], g_dir[MAX_PATH], g_iconDir[MAX_PATH], g_exe[MAX_PATH];
static UINT g_taskbarMsg;
static int g_cacheSize = 96;
static BOOL g_noDl, g_dark = TRUE;
static int g_reflIn;
static BOOL DarkIconActive(void) {
    if (cfg.iconDarkMode == 0) return FALSE;
    if (cfg.iconDarkMode == 1) return g_dark;
    SYSTEMTIME st;
    GetLocalTime(&st);
    int m = st.wHour * 60 + st.wMinute, a = cfg.iconDarkFrom, b = cfg.iconDarkTo;
    if (a == b) return FALSE;
    return a < b ? (m >= a && m < b) : (m >= a || m < b);
}
static GpBitmap *ItemIco(Item *it) { return (it->iconDark && DarkIconActive()) ? it->iconDark : it->icon; }
static BOOL g_win11;     /* Windows 11 renders layered windows much faster: keep a higher frame rate there */
static DWORD g_osBuild;
static HFONT g_uiFont, g_titleFont;
static BOOL g_uiDark, g_menuDark = TRUE;
static WCHAR g_noteDir[MAX_PATH];
static int g_menuEdge = -1;
static GpImageAttributes *g_attrFrost, *g_attrRefl, *g_attrRefl2;
static int g_clockW;

/* canvas / dock surface */
static HDC g_memdc;
static HBITMAP g_dib, g_oldbmp;
static BYTE *g_bits;
static GpBitmap *g_surf;
static GpGraphics *g_g;
static int winX, winY, winW, winH;      /* actual window rect */
static int LW, LH;                      /* logical (horizontal-layout) size */
static MONITORINFO g_mi;
static GpImageAttributes *g_attr, *g_attrFade;
static GpFont *g_font, *g_fontBold, *g_fTR, *g_fDR, *g_fDB, *g_fS, *g_fontTitle;
static GpFontFamily *g_fam;
static GpStringFormat *g_fmtC, *g_fmtL, *g_fmtR;
/* frosted background capture */
static HDC g_fdc;
static HBITMAP g_fbmp, g_fold;
static BYTE *g_fbits;
static GpBitmap *g_frost;
static int g_fw, g_fh;
static DWORD g_fhash;

/* layout state (logical coordinates) */
static float slotL[MAXI + 1], slotR[MAXI + 1], actC[MAXI + 1], baseC[MAXI + 1];
static int prevN = -1;
static float barL, barW, barTop, barBottom;
static float gHideCur;
static int gHideTgt;
static BOOL gForceHide;
static float gFsOff;      /* 0 = dock parked at its edge, 1 = slid out because a fullscreen app took over */
static int gFsTgt;
static int g_fsKind;      /* last FullscreenKind() result, shared with the auto-hide timer */
static BOOL gFsPeek;      /* the user pulled the dock out over a fullscreen app: keep it while the mouse is near */
static DWORD gFsNear;
static DWORD gFsEdgeTick; /* when the pointer started resting on the edge of a fullscreen video */
#define SUMMON_MS 4000    /* after Ctrl+Alt+D the dock stays out this long, even over a fullscreen app */
static DWORD gSummonTick;   /* the global hotkey just summoned the dock: no auto-hide for a moment */
static DWORD gLastNear;
static BOOL gMenuOpen, gAnimOn, gMoved, gFsHidden, gLpOpen;
static DWORD g_launchTick;
static HWND g_lpWnd;
static HWND g_lpEdit;   /* hidden edit behind the launchpad search field: real keyboard + IME input */
static float HideAmt(void) { return gHideCur > gFsOff ? gHideCur : gFsOff; }
static float mouseU = -1, mouseV = -1;
static int hot = -1, mergeT = -1;
static BOOL hover;
static int pressIdx = -1;
static POINT pressPt;
static BOOL dragging, removing;
static DWORD g_remTick;         /* when "drag out to remove" started: a very short pass must not delete an icon */
static BOOL sepDrag;         /* dragging a separator resizes the dock */
static int sepScaleY0, sepScaleIcon0, sepScaleZoom0;
static int dragIdx = -1;
static POINT dragPtA;         /* actual client coords */
static int gapIdx = -1;
static float gapCur;
static int g_menuItem = -1;
static HWND g_pv;
static int pvItem = -1;

typedef struct {
    float S, Z, g, padX, sepW, pad, barH, overhang, margin;
    int lw, lh;
} Metrics;
static Metrics M;
static struct { BOOL valid; int rad, nw; RECT bar, w[2]; } g_barScr;
static RECT g_barRest;   /* the dock bar at its resting place, without the auto-hide slide */

/* ---------------- prototypes ---------------- */
static void Kick(void);
static BOOL RenderFrame(void);
static void DrawRes(GpGraphics *g, float x, float y, float w, float h);
static void DrawPowerWidget(GpGraphics *g, float x, float y, float w, float h);
static void HkText(WCHAR *out, int n);
static void DrawTool(GpGraphics *g, float x, float y, float w, float h);
static void ShowTool(void);
static void DrawMode(GpGraphics *g, float x, float y, float w, float h);
static void ShowModeMenu(void);
static void DrawTray(GpGraphics *g, float x, float y, float w, float h);
static void TrayOpen(void);
static void ShowCtrlCenter(void);
static void CaptureHotkey(HWND owner);
static void EnsureHotkey(void);
static void ShowPowerMenu(void);
static void ShowResMon(void);
static void ScheduleSave(void);
static void UpdateGeometry(void);
static void ApplySettings(void);
static void ShowSettings(int page);
static void LaunchItem(int i, BOOL force);
static void SaveNow(void);
static void LoadItems(void);
static void LoadConfig(void);
static void ReloadAllIcons(void);
static void LoadItemIcon(Item *it);
static void ClosePreview(void);
static void ShowGroup(int idx);
static void ShowFan(int idx);
static void ShowNote(void);
static void ToggleWindowItem(int i);
static void ToggleWindowIdx(HWND w, int idx);
static void ActivateDockWindow(HWND w, int idx);
static BOOL ActivateRunningItem(int i);
static void ShowLaunchpad(void);
static void ShowCalendar(void);
static void ShowNotifyList(void);
static void SetMinHook(BOOL on);
static BOOL ToggleApp(const struct Item *it);
static void LaunchAsync(const Item *it);
static void UpdateBackdrop(void);
static BOOL ShortcutDialog(HWND parent, Item *out);
static HWND MakeBackdropWindow(void);
static void PlaceBackdrop(HWND bk, HWND above, const RECT *rs, const int *rads, int n);
#define WM_APP_TRASH (WM_APP + 5)
#define T_DTLEAVE 10
#define T_UPD 11
#define T_FS 12
#define T_FROST 14
#define T_GEOM 15
#define WM_APP_UPD (WM_APP + 9)
#define WM_APP_SUMMON (WM_APP + 10)   /* another instance asked for the dock: show it, never toggle it away */
#define WM_APP_MINANIM (WM_APP + 11)  /* run the minimise animation after the WinEvent callback returned */
#define GH_REPO L"https://github.com/wang01116/MiniDock"
static void RefreshAccent(void);
static BOOL CursorNearDock(POINT p);
static int FullscreenKind(void);
static int ItemOfKey(DWORD key);
static BOOL WebAppWindow(HWND h, const WCHAR *name);
static HWND g_search;   /* settings search box */
static HWND g_sb;       /* top status bar window */
static BOOL BuildFromPidl(Item *it, LPCITEMIDLIST pidl);
static LRESULT CALLBACK DockProc(HWND, UINT, WPARAM, LPARAM);
static void SetPinMenu(BOOL on);
static void StartUpdateCheck(BOOL autoMode);
static void PinPath(const WCHAR *path);
static void LoadTrayApps(void);
static void ClearTrayApps(void);
static BOOL SyncTrayApps(void);
static WCHAR g_updText[220], g_updUrl[320];
static BOOL g_updNew, g_updAuto;

/* ---------------- helpers ---------------- */
static int Sc(int v) { return MulDiv(v, g_dpi, 96); }
static float Clampf(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }


/* Opt-in diagnostics. The log deliberately excludes window captions, document names,
   user file paths, typed text, and clipboard contents. It records lifecycle and numeric
   state only. Writes are append-only and happen only while the user enables collection. */
static WCHAR g_logPath[MAX_PATH], g_logDir[MAX_PATH];
static void LogInit(void) {
    if (!g_dir[0]) return;
    if (!g_logPath[0] || lstrcmpiW(g_logDir, g_dir) != 0) {
        PathCombineW(g_logPath, g_dir, L"MiniDock.log");
        lstrcpynW(g_logDir, g_dir, MAX_PATH);
    }
}
static void LogEvent(const WCHAR *event, DWORD value) {
    if (!cfg.logCollect) return;
    LogInit();
    if (!g_logPath[0]) return;
    SYSTEMTIME st; GetLocalTime(&st);
    WCHAR line[256];
    wsprintfW(line, L"%04u-%02u-%02u %02u:%02u:%02u.%03u | %s | %lu\r\n",
        st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
        event ? event : L"event", (unsigned long)value);
    int need = WideCharToMultiByte(CP_UTF8, 0, line, -1, NULL, 0, NULL, NULL);
    if (need <= 1 || need > 1024) return;
    char buf[1024]; WideCharToMultiByte(CP_UTF8, 0, line, -1, buf, need, NULL, NULL);
    HANDLE f = CreateFileW(g_logPath, GENERIC_WRITE | FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                           OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f == INVALID_HANDLE_VALUE) return;
    LARGE_INTEGER z; BOOL hasData = GetFileSizeEx(f, &z) && z.QuadPart > 0;
    if (hasData && z.QuadPart > 2 * 1024 * 1024) { /* bounded log: keep disk use predictable */
        LARGE_INTEGER zero; zero.QuadPart = 0; SetFilePointerEx(f, zero, NULL, FILE_BEGIN); SetEndOfFile(f); hasData = FALSE;
    }
    if (!hasData) {
        static const BYTE bom[3] = { 0xEF, 0xBB, 0xBF }; DWORD bw; WriteFile(f, bom, 3, &bw, NULL);
    }
    DWORD wr; WriteFile(f, buf, (DWORD)(need - 1), &wr, NULL); CloseHandle(f);
}
static void LogExport(HWND owner) {
    LogInit();
    if (!g_logPath[0] || GetFileAttributesW(g_logPath) == INVALID_FILE_ATTRIBUTES) {
        MessageBoxW(owner, L"目前没有日志文件。请先在此页开启日志收集，复现问题后再导出。", L"MiniDock 日志", MB_OK | MB_ICONINFORMATION);
        return;
    }
    WCHAR dst[MAX_PATH] = L"MiniDock-diagnostic.log";
    OPENFILENAMEW of; ZeroMemory(&of, sizeof of);
    of.lStructSize = sizeof of; of.hwndOwner = owner; of.lpstrFilter = L"日志文件 (*.log)\0*.log\0所有文件\0*.*\0";
    of.lpstrFile = dst; of.nMaxFile = MAX_PATH; of.lpstrDefExt = L"log";
    of.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    of.lpstrTitle = L"导出 MiniDock 诊断日志";
    if (GetSaveFileNameW(&of)) {
        if (CopyFileW(g_logPath, dst, FALSE)) MessageBoxW(owner, L"日志已导出。上传前请自行检查内容；日志不包含窗口标题、文档名称或输入内容。", L"MiniDock 日志", MB_OK | MB_ICONINFORMATION);
        else MessageBoxW(owner, L"导出失败，请检查目标文件夹权限。", L"MiniDock 日志", MB_OK | MB_ICONWARNING);
    }
}
static void LogClear(HWND owner) {
    LogInit();
    if (!g_logPath[0]) return;
    if (MessageBoxW(owner, L"确定清空已有诊断日志吗？", L"MiniDock 日志", MB_YESNO | MB_ICONQUESTION) == IDYES) {
        HANDLE f = CreateFileW(g_logPath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f != INVALID_HANDLE_VALUE) { CloseHandle(f); LogEvent(L"log-cleared", 0); }
    }
}

static const struct { const WCHAR *k; int *p; } g_tab[] = {
    { L"IconSize", &cfg.iconSize }, { L"ZoomSize", &cfg.zoomSize }, { L"Spacing", &cfg.spacing }, { L"Margin", &cfg.margin }, { L"HPad", &cfg.hPad },
    { L"Padding", &cfg.pad }, { L"Radius", &cfg.radius }, { L"Opacity", &cfg.opacity }, { L"Blur", &cfg.blur },
    { L"Zoom", &cfg.zoom }, { L"Theme", &cfg.theme }, { L"Dots", &cfg.dots }, { L"Labels", &cfg.labels },
    { L"Bounce", &cfg.bounce }, { L"WebAppMode", &cfg.webApp }, { L"Border", &cfg.border }, { L"BgMode", &cfg.bgMode },
    { L"Mode", &cfg.mode }, { L"Reserve", &cfg.reserve }, { L"Edge", &cfg.edge }, { L"Monitor", &cfg.monitor },
    { L"Preview", &cfg.preview }, { L"ShowRunning", &cfg.showRunning }, { L"ShowGroupRunning", &cfg.groupRunning }, { L"Lock", &cfg.lock }, { L"Hotkey", &cfg.hotkey },
    { L"DotStyle", &cfg.dotStyle }, { L"PlateMode", &cfg.plateMode }, { L"PlateTheme", &cfg.plateTheme }, { L"PlateOpacity", &cfg.plateOpacity },
    { L"PlateRadius", &cfg.plateRadius }, { L"BgColor1", &cfg.bgColor1 }, { L"BgColor2", &cfg.bgColor2 }, { L"BgColor3", &cfg.bgColor3 },
    { L"AccentTone", &cfg.accentTone }, { L"Reflect", &cfg.reflect }, { L"Clock", &cfg.clock }, { L"FolderFan", &cfg.folderFan },
    { L"MinAnim", &cfg.minAnim }, { L"Notes", &cfg.notes }, { L"UiTheme", &cfg.uiTheme }, { L"MenuTheme", &cfg.menuTheme },
    { L"BorderColor", &cfg.borderColor }, { L"BorderWidth", &cfg.borderWidth }, { L"SepColor", &cfg.sepColor }, { L"DotColor", &cfg.dotColor },
    { L"GradAngle", &cfg.gradAngle }, { L"GradA1", &cfg.gradA1 }, { L"GradA2", &cfg.gradA2 }, { L"GradA3", &cfg.gradA3 },
    { L"ClockShow", &cfg.clockShow }, { L"ClockColLight", &cfg.clockColLight }, { L"ClockColDark", &cfg.clockColDark },
    { L"ReflSize", &cfg.reflSize }, { L"ReflGap", &cfg.reflGap }, { L"ReflOpacity", &cfg.reflOpacity },
    { L"FanArc", &cfg.fanArc }, { L"FanGap", &cfg.fanGap }, { L"FanCut", &cfg.fanCut }, { L"GroupFrost", &cfg.groupFrost },
    { L"GroupPos", &cfg.groupPos }, { L"Guardian", &cfg.guardian }, { L"ClickToggle", &cfg.clickToggle },
    { L"SepGap", &cfg.sepGap }, { L"SepWidth", &cfg.sepWidth }, { L"ClockSec", &cfg.clockSec }, { L"ClockWeek", &cfg.clockWeek }, { L"DateFmt", &cfg.dateFmt },
    { L"ClockOrder", &cfg.clockOrder }, { L"ClockBoldT", &cfg.clockBoldT }, { L"ClockBoldD", &cfg.clockBoldD }, { L"Browser", &cfg.browser },
    { L"GrpOpacity", &cfg.grpOpacity }, { L"GrpBg", &cfg.grpBg }, { L"MinStyle", &cfg.minStyle }, { L"MnAuto", &cfg.mnAuto }, { L"MnNotes", &cfg.mnNotes },
    { L"MnAdd", &cfg.mnAdd }, { L"Launchpad", &cfg.launchpad },
    { L"WSide", &cfg.wSide }, { L"WOrder", &cfg.wOrder }, { L"NotifyW", &cfg.notifyW }, { L"FanBgL", &cfg.fanBgL }, { L"FanBgD", &cfg.fanBgD },
    { L"FanFgL", &cfg.fanFgL }, { L"FanFgD", &cfg.fanFgD }, { L"PinMenu", &cfg.pinMenu }, { L"MultiMode", &cfg.multiMode }, { L"UpdChk", &cfg.updChk }, { L"UpdLast", &cfg.updLast }, { L"LpCols", &cfg.lpCols }, { L"LpRows", &cfg.lpRows }, { L"FanDrag", &cfg.fanDrag }, { L"TrayHide", &cfg.trayHide }, { L"CtrlCenter", &cfg.ctrlCenter }, { L"CcEnergy", &cfg.ccEnergy }, { L"CcHotspot", &cfg.ccHotspot }, { L"ResMon", &cfg.resMon }, { L"ToolW", &cfg.toolW }, { L"WideMask", &cfg.wideMask }, { L"ModeW", &cfg.modeW }, { L"UiFrost", &cfg.uiFrost }, { L"LpFrost", &cfg.lpFrost }, { L"TrayW", &cfg.trayW }, { L"WMerge", &cfg.wMerge }, { L"StatusBar", &cfg.statusBar }, { L"SbFrost", &cfg.sbFrost }, { L"SbOpacity", &cfg.sbOpacity }, { L"SbColor", &cfg.sbColor }, { L"SbRadius", &cfg.sbRadius }, { L"SbHeight", &cfg.sbHeight }, { L"SbFollowSys", &cfg.sbFollowSys }, { L"SbTopGap", &cfg.sbTopGap }, { L"SbMaxAvoid", &cfg.sbMaxAvoid }, { L"SbFloat", &cfg.sbFloat }, { L"SbSideGap", &cfg.sbSideGap }, { L"SbFullscreenHide", &cfg.sbFullscreenHide }, { L"SbBattery", &cfg.sbBattery }, { L"SepScale", &cfg.sepScale }, { L"MenuFrost", &cfg.menuFrost },
    { L"SbBorder", &cfg.sbBorder }, { L"GrpFloat", &cfg.grpFloat },
    { L"HkMods", &cfg.hkMods }, { L"HkVk", &cfg.hkVk }, { L"GameGuard", &cfg.gameGuard }, { L"GameAuto", &cfg.gameAuto },
    { L"CmMerge", &cfg.cmMerge }, { L"CmSep", &cfg.cmSep }, { L"CmIcon", &cfg.cmIcon }, { L"CmRename", &cfg.cmRename }, { L"CmCopy", &cfg.cmCopy }, { L"CmDock", &cfg.cmDock },
    { L"LpTabs", &cfg.lpTabs }, { L"LpOpacity", &cfg.lpOpacity }, { L"ReflMode", &cfg.reflMode }, { L"ReflGrad", &cfg.reflGrad },
    { L"IconDarkMode", &cfg.iconDarkMode }, { L"IconDarkFrom", &cfg.iconDarkFrom }, { L"IconDarkTo", &cfg.iconDarkTo },
    { L"MnTaskMgr", &cfg.mnTaskMgr }, { L"PowerW", &cfg.powerW }, { L"TaskHide", &cfg.taskHide }, { L"TaskTop", &cfg.taskTop }, { L"TaskTrans", &cfg.taskTrans }, { L"TaskAlpha", &cfg.taskAlpha }, { L"EcoMode", &cfg.ecoMode }, { L"BatEco", &cfg.batEco }, { L"BatLow", &cfg.batLow }, { L"SbPopFrost", &cfg.sbPopFrost }, { L"SbPopBlur", &cfg.sbPopBlur }, { L"PvList", &cfg.pvList }, { L"SbAutoOpaque", &cfg.sbAutoOpaque }, { L"TaskPeek", &cfg.taskPeek },
    { L"FrostHold", &cfg.frostHold }, { L"AnimSpeed", &cfg.animSpeed }, { L"FanReverse", &cfg.fanReverse }, { L"LogCollect", &cfg.logCollect } };
#define NTAB ((int)(sizeof g_tab / sizeof g_tab[0]))

static void DefaultCfg(void) {
    cfg.iconSize = Sc(48); cfg.zoomSize = Sc(84); cfg.spacing = Sc(6); cfg.margin = Sc(8); cfg.pad = Sc(8); cfg.hPad = Sc(12);
    cfg.radius = Sc(18); cfg.opacity = 60; cfg.blur = 6;
    cfg.zoom = 1; cfg.theme = 0; cfg.dots = 1; cfg.labels = 1; cfg.bounce = 1; cfg.webApp = 1; cfg.border = 1; cfg.bgMode = 2;
    cfg.mode = 0; cfg.reserve = 0; cfg.edge = 0; cfg.monitor = 0; cfg.preview = 1; cfg.showRunning = 1; cfg.lock = 0; cfg.hotkey = 1;
    cfg.groupRunning = 0;
    cfg.dotStyle = 0; cfg.plateMode = 0; cfg.plateTheme = 2; cfg.plateOpacity = 100; cfg.plateRadius = 22; cfg.bgColor1 = -1; cfg.bgColor2 = -1; cfg.bgColor3 = -1; cfg.accentTone = 0;
    cfg.reflect = 0; cfg.clock = 0; cfg.folderFan = 1; cfg.minAnim = 1; cfg.notes = 0; cfg.uiTheme = 2; cfg.menuTheme = 2;
    cfg.borderColor = -1; cfg.borderWidth = 1; cfg.sepColor = -1; cfg.dotColor = -1; cfg.gradAngle = 90; cfg.gradA1 = 100; cfg.gradA2 = 100; cfg.gradA3 = 100;
    cfg.clockShow = 0; cfg.clockColLight = -1; cfg.clockColDark = -1; cfg.reflSize = 38; cfg.reflGap = 3; cfg.reflOpacity = 30;
    cfg.fanArc = 40; cfg.fanGap = 9; cfg.fanCut = 0; cfg.groupFrost = 1; cfg.groupPos = 0; cfg.guardian = 1; cfg.clickToggle = 1;
    cfg.sepGap = Sc(4); cfg.sepWidth = 2; cfg.clockSec = 0; cfg.clockWeek = 1; cfg.dateFmt = 0; cfg.clockOrder = 0; cfg.clockBoldT = 1; cfg.clockBoldD = 0;
    cfg.browser = 0; cfg.grpOpacity = 94; cfg.grpBg = -1; cfg.minStyle = MIN_STYLE_COMPRESS; cfg.mnAuto = 1; cfg.mnNotes = 1; cfg.mnAdd = 1;
    cfg.launchpad = 0; cfg.wSide = 0; cfg.wOrder = 0; cfg.notifyW = 0; cfg.fanBgL = -1; cfg.fanBgD = -1; cfg.fanFgL = -1; cfg.fanFgD = -1;
    cfg.pinMenu = 0; cfg.multiMode = 0; cfg.updChk = 1; cfg.updLast = 0; cfg.lpCols = 7; cfg.lpRows = 4; cfg.fanDrag = 1;
    cfg.hideTaskbar = 0;   /* feature removed in 1.5.5-r5: the dock never claims a taskbar button anyway */
    cfg.trayHide = 0;
    cfg.ctrlCenter = 1;
    cfg.ccEnergy = 1;
    cfg.ccHotspot = 1;
    cfg.resMon = 1;
    cfg.toolW = 1;
    cfg.wideMask = 0;
    cfg.modeW = 1;
    cfg.uiFrost = 1;
    cfg.lpFrost = 1;
    cfg.trayW = 1;
    cfg.wMerge = 1;
    cfg.statusBar = 0; cfg.sbBattery = 1; cfg.sbFrost = 1; cfg.sbOpacity = 88; cfg.sbColor = -1; cfg.sbRadius = 10; cfg.sbHeight = 32; cfg.sbFollowSys = 1; cfg.sbTopGap = 0;
    cfg.sbMaxAvoid = 0; cfg.sbFloat = 0; cfg.sbSideGap = 12; cfg.sbFullscreenHide = 1;
    cfg.sbBorder = 1; cfg.grpFloat = 0;
    cfg.hkMods = MOD_CONTROL | MOD_ALT; cfg.hkVk = 'D';
    cfg.gameGuard = 0; cfg.gameAuto = 1; cfg.lpTabs = 1; cfg.lpOpacity = 88;
    cfg.cmMerge = cfg.cmSep = cfg.cmIcon = cfg.cmRename = cfg.cmCopy = cfg.cmDock = 1;
    cfg.reflMode = 0; cfg.reflGrad = 0;
    cfg.iconDarkMode = 0; cfg.iconDarkFrom = 19 * 60; cfg.iconDarkTo = 7 * 60;
    cfg.mnTaskMgr = 1;
    cfg.powerW = 0;
    cfg.taskHide = 0; cfg.taskTrans = 0; cfg.taskAlpha = 85; cfg.taskTop = 0;
    cfg.ecoMode = 0;
    cfg.batEco = 1; cfg.batLow = 20;
    cfg.sbPopFrost = 1; cfg.sbPopBlur = 100; cfg.pvList = 1; cfg.sbAutoOpaque = 1; cfg.taskPeek = 1;
    cfg.frostHold = 5; cfg.animSpeed = 450; cfg.fanReverse = 0; cfg.logCollect = 0;
}

static void EnsureIni(void) {
    if (GetFileAttributesW(g_ini) != INVALID_FILE_ATTRIBUTES) return;
    HANDLE h = CreateFileW(g_ini, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) { WORD bom = 0xFEFF; DWORD w; WriteFile(h, &bom, 2, &w, NULL); CloseHandle(h); }
}

static void InitPaths(void) {
    GetModuleFileNameW(NULL, g_exe, MAX_PATH);
    WCHAR exeDir[MAX_PATH], ap[MAX_PATH], local[MAX_PATH];
    lstrcpyW(exeDir, g_exe);
    PathRemoveFileSpecW(exeDir);
    SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, ap);
    PathCombineW(g_dir, ap, L"MiniDock");
    CreateDirectoryW(g_dir, NULL);
    {   /* optional custom data folder, remembered in %APPDATA%\\MiniDock\\location.txt */
        WCHAR loc[MAX_PATH], lf[MAX_PATH];
        PathCombineW(lf, g_dir, L"location.txt");
        loc[0] = 0;
        HANDLE h = CreateFileW(lf, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            DWORD n = 0;
            ReadFile(h, loc, (MAX_PATH - 1) * 2, &n, NULL);
            loc[n / 2] = 0;
            CloseHandle(h);
            StrTrimW(loc, L" \r\n\t\xFEFF");
            if (loc[0] && GetFileAttributesW(loc) != INVALID_FILE_ATTRIBUTES) lstrcpyW(g_dir, loc);
        }
    }
    PathCombineW(g_ini, g_dir, L"MiniDock.ini");
    PathCombineW(local, exeDir, L"MiniDock.ini");
    /* settings live in %APPDATA%\\MiniDock so they survive moving / re-downloading the exe;
       an existing ini next to the exe is migrated once, and a file named MiniDock.portable forces portable mode */
    WCHAR marker[MAX_PATH];
    PathCombineW(marker, exeDir, L"MiniDock.portable");
    if (GetFileAttributesW(marker) != INVALID_FILE_ATTRIBUTES) {
        lstrcpyW(g_dir, exeDir);
        lstrcpyW(g_ini, local);
    } else if (GetFileAttributesW(g_ini) == INVALID_FILE_ATTRIBUTES && GetFileAttributesW(local) != INVALID_FILE_ATTRIBUTES) {
        CopyFileW(local, g_ini, TRUE);
        WCHAR li[MAX_PATH], di[MAX_PATH];
        PathCombineW(li, exeDir, L"icons");
        PathCombineW(di, g_dir, L"icons");
        CreateDirectoryW(di, NULL);
        WIN32_FIND_DATAW fd;
        WCHAR pat[MAX_PATH];
        PathCombineW(pat, li, L"*");
        HANDLE ff = FindFirstFileW(pat, &fd);
        if (ff != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                WCHAR a[MAX_PATH], b[MAX_PATH];
                PathCombineW(a, li, fd.cFileName);
                PathCombineW(b, di, fd.cFileName);
                CopyFileW(a, b, TRUE);
            } while (FindNextFileW(ff, &fd));
            FindClose(ff);
        }
    }
    EnsureIni();
    PathCombineW(g_iconDir, g_dir, L"icons");
    CreateDirectoryW(g_iconDir, NULL);
}

static BOOL IsUrlStr(const WCHAR *s) {
    return StrCmpNIW(s, L"http://", 7) == 0 || StrCmpNIW(s, L"https://", 8) == 0 || StrCmpNIW(s, L"www.", 4) == 0;
}
static void HostOf(const WCHAR *url, WCHAR *out, int cch) {
    const WCHAR *p = wcsstr(url, L"://");
    p = p ? p + 3 : url;
    int n = 0;
    while (*p && *p != L'/' && *p != L':' && *p != L'?' && *p != L'#' && n < cch - 1) out[n++] = *p++;
    out[n] = 0;
}
static DWORD Fnv(const WCHAR *s) {
    DWORD h = 2166136261u;
    for (; *s; s++) { h ^= (WORD)*s; h *= 16777619u; }
    return h;
}
static void IconFileFor(const WCHAR *url, WCHAR *out) {
    WCHAR host[256], nm[40];
    HostOf(url, host, 256);
    CharLowerW(host);
    wsprintfW(nm, L"%08x.png", Fnv(host));
    PathCombineW(out, g_iconDir, nm);
}

/* ---------------- geometry mapping (edge aware) ---------------- */
static void MapR(float u, float v, float w, float h, float *x, float *y, float *ww, float *hh) {
    switch (cfg.edge) {
    case E_TOP:   *x = u; *y = LH - v - h; *ww = w; *hh = h; break;
    case E_LEFT:  *x = LH - v - h; *y = u; *ww = h; *hh = w; break;
    case E_RIGHT: *x = v; *y = u; *ww = h; *hh = w; break;
    default:      *x = u; *y = v; *ww = w; *hh = h; break;
    }
}
static void ToLogical(float px, float py, float *u, float *v) {
    switch (cfg.edge) {
    case E_TOP:   *u = px; *v = LH - py; break;
    case E_LEFT:  *u = py; *v = LH - px; break;
    case E_RIGHT: *u = py; *v = px; break;
    default:      *u = px; *v = py; break;
    }
}
static BOOL Horiz(void) { return cfg.edge == E_BOTTOM || cfg.edge == E_TOP; }

/* ---------------- per-monitor DPI ----------------
   Every pixel value in Cfg is authored at g_dpiCfg (written to the ini as CfgDpi). When the dock is moved
   to a monitor with another scaling factor, g_dpi follows that monitor and Pxs() converts the stored
   values, so the dock keeps the same apparent size and lands exactly on the edge of that monitor. */
typedef HRESULT (WINAPI *pGetDpiForMon)(HMONITOR, int, UINT *, UINT *);
static int MonDpi(HMONITOR m) {
    static pGetDpiForMon fn;
    static BOOL tried;
    if (!tried) {
        tried = TRUE;
        HMODULE s = LoadLibraryW(L"shcore.dll");
        if (s) fn = (pGetDpiForMon)GetProcAddress(s, "GetDpiForMonitor");
    }
    UINT x = 0, y = 0;
    if (fn && m && SUCCEEDED(fn(m, 0 /* MDT_EFFECTIVE_DPI */, &x, &y)) && x >= 72 && x <= 480) return (int)x;
    return 96;
}
static void RebuildDockFonts(void) {
    if (g_font) { GdipDeleteFont(g_font); g_font = NULL; }
    if (g_fontBold) { GdipDeleteFont(g_fontBold); g_fontBold = NULL; }
    if (g_fTR) { GdipDeleteFont(g_fTR); g_fTR = NULL; }
    if (g_fDR) { GdipDeleteFont(g_fDR); g_fDR = NULL; }
    if (g_fDB) { GdipDeleteFont(g_fDB); g_fDB = NULL; }
    if (g_fS) { GdipDeleteFont(g_fS); g_fS = NULL; }
    if (g_fontTitle) { GdipDeleteFont(g_fontTitle); g_fontTitle = NULL; }
    if (!g_fam && GdipCreateFontFamilyFromName(L"Microsoft YaHei UI", NULL, &g_fam) != 0) GdipGetGenericFontFamilySansSerif(&g_fam);
    if (g_fam) {
        GdipCreateFont(g_fam, (REAL)Sc(13), 0, 2, &g_font);
        GdipCreateFont(g_fam, (REAL)Sc(15), 1, 2, &g_fontBold);
        GdipCreateFont(g_fam, (REAL)Sc(15), 0, 2, &g_fTR);
        GdipCreateFont(g_fam, (REAL)Sc(13), 1, 2, &g_fDB);
        GdipCreateFont(g_fam, (REAL)Sc(13), 0, 2, &g_fDR);
        GdipCreateFont(g_fam, (REAL)Sc(10), 0, 2, &g_fS);
        GdipCreateFont(g_fam, (REAL)Sc(24), 1, 2, &g_fontTitle);
    }
    {   /* Win32 fonts for the dialogs and the settings window */
        NONCLIENTMETRICSW ncm = { sizeof ncm };
        if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof ncm, &ncm, 0)) {
            if (g_uiFont) DeleteObject(g_uiFont);
            if (g_titleFont) DeleteObject(g_titleFont);
            ncm.lfMessageFont.lfHeight = -Sc(14);
            lstrcpyW(ncm.lfMessageFont.lfFaceName, L"Microsoft YaHei UI");
            g_uiFont = CreateFontIndirectW(&ncm.lfMessageFont);
            ncm.lfMessageFont.lfHeight = -Sc(22);
            ncm.lfMessageFont.lfWeight = FW_SEMIBOLD;
            g_titleFont = CreateFontIndirectW(&ncm.lfMessageFont);
        }
    }
}
/* follow the DPI of the monitor the dock is on; returns TRUE when it changed */
static BOOL SyncDockDpi(void) {
    HMONITOR hm = MonitorFromRect(&g_mi.rcMonitor, MONITOR_DEFAULTTONEAREST);
    int d = MonDpi(hm);
    if (d == g_dpi) return FALSE;
    g_dpi = d;
    RebuildDockFonts();
    {   /* icons are cached at a fixed pixel size: a new monitor scaling means new cache bitmaps */
        int need = CacheFor(Pxs(cfg.zoom ? (cfg.zoomSize > cfg.iconSize ? cfg.zoomSize : cfg.iconSize) : cfg.iconSize));
        if (need != g_cacheSize) { g_cacheSize = need; ReloadAllIcons(); }
    }
    if (g_search) SendMessageW(g_search, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
    if (g_hset) { InvalidateRect(g_hset, NULL, FALSE); }
    return TRUE;
}

/* ---------------- theme ---------------- */
static BOOL SystemDark(void) {
    DWORD v = 1, sz = 4;
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", L"AppsUseLightTheme", RRF_RT_REG_DWORD, NULL, &v, &sz) != ERROR_SUCCESS) v = 1;
    return v == 0;
}
static void UpdateTheme(void) {
    g_dark = cfg.theme == 0 ? TRUE : cfg.theme == 1 ? FALSE : SystemDark();
    g_uiDark = cfg.uiTheme == 0 ? FALSE : cfg.uiTheme == 1 ? TRUE : SystemDark();
    g_menuDark = cfg.menuTheme == 0 ? FALSE : cfg.menuTheme == 1 ? TRUE : SystemDark();
}

/* ---------------- config ---------------- */
static void LoadConfig(void) {
    DefaultCfg();
    LoadTrayApps();
    if ((int)GetPrivateProfileIntW(L"Settings", L"BgMode", -1, g_ini) < 0) cfg.bgMode = GetPrivateProfileIntW(L"Settings", L"Frost", 1, g_ini) ? 2 : 0;
    for (int i = 0; i < NTAB; i++) *g_tab[i].p = (int)GetPrivateProfileIntW(L"Settings", g_tab[i].k, *g_tab[i].p, g_ini);
    if (cfg.minStyle < MIN_STYLE_COMPRESS || cfg.minStyle > MIN_STYLE_SMOOTH) cfg.minStyle = MIN_STYLE_COMPRESS;
    if (cfg.bgMode < 0 || cfg.bgMode > 2) cfg.bgMode = 2;
    if (cfg.opacity < 0) cfg.opacity = 0;
    GetPrivateProfileStringW(L"Notes", L"Dir", L"", g_noteDir, MAX_PATH, g_ini);
    if (!g_noteDir[0]) lstrcpyW(g_noteDir, g_dir);
    g_dpiCfg = (int)GetPrivateProfileIntW(L"Settings", L"CfgDpi", g_dpi, g_ini);
    if (g_dpiCfg < 72 || g_dpiCfg > 480) g_dpiCfg = g_dpi;
    if (cfg.iconSize < 16) cfg.iconSize = 16;
    if (cfg.iconSize > 256) cfg.iconSize = 256;
    if (cfg.edge < 0 || cfg.edge > 3) cfg.edge = 0;
    if (cfg.mode < 0 || cfg.mode > 2) cfg.mode = 0;
    UpdateTheme();
}

static void SaveSettingsOnly(void) {
    for (int i = 0; i < NTAB; i++) {
        WCHAR b[24];
        wsprintfW(b, L"%d", *g_tab[i].p);
        WritePrivateProfileStringW(L"Settings", g_tab[i].k, b, g_ini);
    }
    {   /* remember the DPI these pixel values belong to, so another monitor scaling can convert them */
        WCHAR b[24];
        wsprintfW(b, L"%d", g_dpiCfg);
        WritePrivateProfileStringW(L"Settings", L"CfgDpi", b, g_ini);
    }
}

static void ScheduleSave(void) { if (g_hwnd) SetTimer(g_hwnd, T_SAVE, 700, NULL); }

#define RUNKEY L"Software\\Microsoft\\Windows\\CurrentVersion\\Run"
static BOOL GetAutostart(void) {
    HKEY k; BOOL r = FALSE;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, RUNKEY, 0, KEY_READ, &k) == ERROR_SUCCESS) {
        r = RegQueryValueExW(k, L"MiniDock", NULL, NULL, NULL, NULL) == ERROR_SUCCESS;
        RegCloseKey(k);
    }
    return r;
}
static void SetAutostart(BOOL on) {
    HKEY k;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, RUNKEY, 0, NULL, 0, KEY_WRITE, NULL, &k, NULL) != ERROR_SUCCESS) return;
    if (on) {
        WCHAR v[MAX_PATH + 4];
        wsprintfW(v, L"\"%s\"", g_exe);
        RegSetValueExW(k, L"MiniDock", 0, REG_SZ, (BYTE *)v, (lstrlenW(v) + 1) * 2);
    } else RegDeleteValueW(k, L"MiniDock");
    RegCloseKey(k);
}

/* ---------------- canvas for popup windows (menu, group) ---------------- */
typedef struct { HWND h; HDC dc; HBITMAP bm, old; BYTE *bits; GpBitmap *surf; GpGraphics *g; int w, h2; } Canvas;

static void SetupG(GpGraphics *g) {
    GdipSetSmoothingMode(g, 4);
    GdipSetInterpolationMode(g, 7);
    GdipSetPixelOffsetMode(g, 4);
    GdipSetTextRenderingHint(g, 4);
}
static BOOL CanvasNew(Canvas *c, int w, int h) {
    ZeroMemory(c, sizeof *c);
    c->w = w; c->h2 = h;
    HDC s = GetDC(NULL);
    c->dc = CreateCompatibleDC(s);
    ReleaseDC(NULL, s);
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof bi);
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h; bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
    c->bm = CreateDIBSection(c->dc, &bi, DIB_RGB_COLORS, (void **)&c->bits, NULL, 0);
    if (!c->bm) return FALSE;
    c->old = (HBITMAP)SelectObject(c->dc, c->bm);
    GdipCreateBitmapFromScan0(w, h, w * 4, PXF_PARGB, c->bits, &c->surf);
    GdipGetImageGraphicsContext(c->surf, &c->g);
    SetupG(c->g);
    return TRUE;
}
static void CanvasFree(Canvas *c) {
    if (c->g) GdipDeleteGraphics(c->g);
    if (c->surf) GdipDisposeImage(c->surf);
    if (c->bm) { SelectObject(c->dc, c->old); DeleteObject(c->bm); }
    if (c->dc) DeleteDC(c->dc);
    ZeroMemory(c, sizeof *c);
}
static void CanvasPresentA(Canvas *c, HWND h, int x, int y, int alpha) {
    POINT pt = { x, y }, src = { 0, 0 };
    SIZE sz = { c->w, c->h2 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)alpha, AC_SRC_ALPHA };
    UpdateLayeredWindow(h, NULL, &pt, &sz, c->dc, &src, 0, &bf, ULW_ALPHA);
}
static void CanvasPresent(Canvas *c, HWND h, int x, int y) { CanvasPresentA(c, h, x, y, 255); }


/* ---------------- popup guard: closes popups when the user clicks anywhere outside ---------------- */
typedef BOOL (*InsideFn)(POINT, void *);
typedef struct { InsideFn fn; void *ctx; volatile int *done; HWND wnd; } Guard;
static Guard g_gs[6];
static int g_gn;
static HHOOK g_mh;
static DWORD g_popupClosedTick;
static LRESULT CALLBACK MouseLL(int code, WPARAM w, LPARAM l) {
    if (code >= 0 && g_gn > 0 && (w == WM_LBUTTONDOWN || w == WM_RBUTTONDOWN || w == WM_MBUTTONDOWN)) {
        Guard *g = &g_gs[g_gn - 1];
        MSLLHOOKSTRUCT *ms = (MSLLHOOKSTRUCT *)l;
        if (g->fn && !g->fn(ms->pt, g->ctx)) {
            *g->done = 1;
            g_popupClosedTick = GetTickCount();
            if (g->wnd) PostMessageW(g->wnd, WM_NULL, 0, 0);
        }
    }
    return CallNextHookEx(g_mh, code, w, l);
}
static void GuardStart(InsideFn f, void *ctx, volatile int *done, HWND wnd) {
    if (g_gn < 6) { g_gs[g_gn].fn = f; g_gs[g_gn].ctx = ctx; g_gs[g_gn].done = done; g_gs[g_gn].wnd = wnd; g_gn++; }
    if (!g_mh) g_mh = SetWindowsHookExW(WH_MOUSE_LL, MouseLL, g_inst, 0);
}
static void GuardStop(void) {
    if (g_gn > 0) g_gn--;
    if (g_gn == 0 && g_mh) { UnhookWindowsHookEx(g_mh); g_mh = NULL; }
}

/* ---------------- drawing primitives ---------------- */
static void DrawRoundRectPath(GpPath *p, float x, float y, float w, float h, float r) {
    float d = r * 2;
    if (d > w) d = w;
    if (d > h) d = h;
    if (d < 0.5f) d = 0.5f;
    GdipAddPathArc(p, x, y, d, d, 180, 90);
    GdipAddPathArc(p, x + w - d, y, d, d, 270, 90);
    GdipAddPathArc(p, x + w - d, y + h - d, d, d, 0, 90);
    GdipAddPathArc(p, x, y + h - d, d, d, 90, 90);
    GdipClosePathFigure(p);
}
static void FillRoundRect(GpGraphics *g, GpBrush *b, float x, float y, float w, float h, float r) {
    GpPath *p;
    GdipCreatePath(0, &p);
    DrawRoundRectPath(p, x, y, w, h, r);
    GdipFillPath(g, b, p);
    GdipDeletePath(p);
}
static void FillRR(GpGraphics *g, ARGB c, float x, float y, float w, float h, float r) {
    GpBrush *b;
    GdipCreateSolidFill(c, &b);
    FillRoundRect(g, b, x, y, w, h, r);
    GdipDeleteBrush(b);
}
static void StrokeRR(GpGraphics *g, ARGB c, float wd, float x, float y, float w, float h, float r) {
    GpPen *pen;
    GpPath *p;
    GdipCreatePen1(c, wd, 2, &pen);
    GdipCreatePath(0, &p);
    DrawRoundRectPath(p, x + wd / 2, y + wd / 2, w - wd, h - wd, r);
    GdipDrawPath(g, pen, p);
    GdipDeletePath(p);
    GdipDeletePen(pen);
}
static void DrawImg(GpGraphics *g, GpBitmap *src, float x, float y, float w, float h, BOOL fade) {
    UINT sw = 0, sh = 0;
    GdipGetImageWidth(src, &sw);
    GdipGetImageHeight(src, &sh);
    GdipDrawImageRectRect(g, src, x, y, w, h, 0, 0, (REAL)sw, (REAL)sh, 2, fade ? g_attrFade : g_attr, NULL, NULL);
}
static void DrawTextBox(GpGraphics *g, const WCHAR *t, GpFont *f, float x, float y, float w, float h, ARGB col, BOOL left) {
    GpBrush *br;
    GdipCreateSolidFill(col, &br);
    RectF rc = { x, y, w, h };
    GdipDrawString(g, t, -1, f, &rc, left ? g_fmtL : g_fmtC, br);
    GdipDeleteBrush(br);
}
static void DrawTextBoxR(GpGraphics *g, const WCHAR *t, GpFont *f, float x, float y, float w, float h, ARGB col) {
    GpBrush *br;
    GdipCreateSolidFill(col, &br);
    RectF rc = { x, y, w, h };
    GdipDrawString(g, t, -1, f, &rc, g_fmtR, br);
    GdipDeleteBrush(br);
}
static void DrawCentered(GpGraphics *g, const WCHAR *t, GpFont *f, float x, float y, float w, float h, ARGB col) {
    DrawTextBox(g, t, f, x, y, w, h, col, FALSE);
}
static float TextWidth(GpGraphics *g, const WCHAR *t, GpFont *f) {
    RectF lay = { 0, 0, 600, 40 }, bb = { 0, 0, 0, 0 };
    INT cp = 0, ln = 0;
    GdipMeasureString(g, t, -1, f, &lay, g_fmtL, &bb, &cp, &ln);
    return bb.Width;
}

/* ======================= icons ======================= */
static GpBitmap *NewCache(int s, GpGraphics **gout) {
    GpBitmap *b = NULL;
    GdipCreateBitmapFromScan0(s, s, 0, PXF_PARGB, NULL, &b);
    if (b) { GdipGetImageGraphicsContext(b, gout); SetupG(*gout); }
    return b;
}

static void AlphaBBox(GpBitmap *b, UINT w, UINT h, int *x0, int *y0, int *x1, int *y1, int thr) {
    *x0 = 0; *y0 = 0; *x1 = (int)w; *y1 = (int)h;
    GpRect r = { 0, 0, (INT)w, (INT)h };
    GpBitmapData d;
    if (GdipBitmapLockBits(b, &r, 1, PXF_ARGB, &d) != 0) return;
    int minx = (int)w, miny = (int)h, maxx = -1, maxy = -1;
    for (int y = 0; y < (int)h; y++) {
        BYTE *row = (BYTE *)d.Scan0 + (INT_PTR)y * d.Stride;
        for (int x = 0; x < (int)w; x++)
            if (row[x * 4 + 3] > thr) {
                if (x < minx) minx = x;
                if (x > maxx) maxx = x;
                if (y < miny) miny = y;
                if (y > maxy) maxy = y;
            }
    }
    GdipBitmapUnlockBits(b, &d);
    if (maxx >= 0) { *x0 = minx; *y0 = miny; *x1 = maxx + 1; *y1 = maxy + 1; }
}

/* find the bounding box of a uniformly coloured (e.g. black / white) border so it can be trimmed */
static void SolidBBox(GpBitmap *b, UINT w, UINT h, int *x0, int *y0, int *x1, int *y1) {
    GpRect r = { 0, 0, (INT)w, (INT)h };
    GpBitmapData d;
    if (GdipBitmapLockBits(b, &r, 1, PXF_ARGB, &d) != 0) return;
    BYTE *p0 = (BYTE *)d.Scan0;
    BYTE ref[4] = { p0[0], p0[1], p0[2], p0[3] };
    if (ref[3] < 200) { GdipBitmapUnlockBits(b, &d); return; }
    #define ISBG(px) ((px)[3] > 200 && abs((int)(px)[0] - ref[0]) + abs((int)(px)[1] - ref[1]) + abs((int)(px)[2] - ref[2]) < 70)
    int top = 0, bot = (int)h - 1, lef = 0, rig = (int)w - 1;
    for (; top < bot; top++) { BYTE *row = p0 + (INT_PTR)top * d.Stride; int bad = 0; for (int x = 0; x < (int)w; x++) if (!ISBG(row + x * 4)) bad++; if (bad * 100 > (int)w * 2) break; }
    for (; bot > top; bot--) { BYTE *row = p0 + (INT_PTR)bot * d.Stride; int bad = 0; for (int x = 0; x < (int)w; x++) if (!ISBG(row + x * 4)) bad++; if (bad * 100 > (int)w * 2) break; }
    for (; lef < rig; lef++) { int bad = 0; for (int y = top; y <= bot; y++) if (!ISBG(p0 + (INT_PTR)y * d.Stride + lef * 4)) bad++; if (bad * 100 > (bot - top + 1) * 2) break; }
    for (; rig > lef; rig--) { int bad = 0; for (int y = top; y <= bot; y++) if (!ISBG(p0 + (INT_PTR)y * d.Stride + rig * 4)) bad++; if (bad * 100 > (bot - top + 1) * 2) break; }
    GdipBitmapUnlockBits(b, &d);
    if (bot - top > 8 && rig - lef > 8) { *x0 = lef; *y0 = top; *x1 = rig + 1; *y1 = bot + 1; }
}

/* fit: 0 = auto (trim transparent / solid borders), 1 = keep whole image, 2 = centre-crop to fill */
static GpBitmap *FitBitmap(GpBitmap *src, int cs, int fit, float fill) {
    UINT w = 0, h = 0;
    GdipGetImageWidth(src, &w);
    GdipGetImageHeight(src, &h);
    if (!w || !h) return NULL;
    int x0 = 0, y0 = 0, x1 = (int)w, y1 = (int)h;
    BOOL full = FALSE;
    if (fit == 0) {
        AlphaBBox(src, w, h, &x0, &y0, &x1, &y1, 40);
        if (x0 == 0 && y0 == 0 && x1 == (int)w && y1 == (int)h) { SolidBBox(src, w, h, &x0, &y0, &x1, &y1); full = TRUE; }
    } else if (fit == 2) {
        int sq = (int)(w < h ? w : h);
        x0 = ((int)w - sq) / 2; y0 = ((int)h - sq) / 2; x1 = x0 + sq; y1 = y0 + sq;
        fill = 1.0f; full = TRUE;
    }
    float sw = (float)(x1 - x0), sh = (float)(y1 - y0);
    float mx = sw > sh ? sw : sh;
    float sc = cs * fill / mx, dw = sw * sc, dh = sh * sc;
    GpGraphics *g = NULL;
    GpBitmap *dst = NewCache(cs, &g);
    if (!dst) return NULL;
    if (full) { /* opaque square image -> round the corners like a real app icon */
        GpPath *cp;
        GdipCreatePath(0, &cp);
        DrawRoundRectPath(cp, (cs - dw) / 2, (cs - dh) / 2, dw, dh, (dw < dh ? dw : dh) * 0.22f);
        GdipSetClipPath(g, cp, 0);
        GdipDeletePath(cp);
    }
    GdipDrawImageRectRect(g, src, (cs - dw) / 2, (cs - dh) / 2, dw, dh, (REAL)x0, (REAL)y0, sw, sh, 2, g_attr, NULL, NULL);
    GdipDeleteGraphics(g);
    return dst;
}

/* HICON -> straight-alpha GDI+ bitmap (GdipCreateBitmapFromHICON leaves a black plate behind many icons) */
static GpBitmap *HiconToGp(HICON hi) {
    ICONINFO ii;
    GpBitmap *out = NULL;
    if (!GetIconInfo(hi, &ii)) return NULL;
    if (ii.hbmColor) {
        BITMAP bm;
        GetObjectW(ii.hbmColor, sizeof bm, &bm);
        int w = bm.bmWidth, h = bm.bmHeight;
        BITMAPINFO bi;
        ZeroMemory(&bi, sizeof bi);
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h; bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
        BYTE *px = (BYTE *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)w * h * 4);
        if (px) {
            HDC dc = GetDC(NULL);
            GetDIBits(dc, ii.hbmColor, 0, h, px, &bi, DIB_RGB_COLORS);
            BOOL anyA = FALSE;
            for (int i = 0; i < w * h; i++) if (px[i * 4 + 3]) { anyA = TRUE; break; }
            if (!anyA && ii.hbmMask) {
                BYTE *mk = (BYTE *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)w * h * 4);
                if (mk) {
                    GetDIBits(dc, ii.hbmMask, 0, h, mk, &bi, DIB_RGB_COLORS);
                    for (int i = 0; i < w * h; i++) px[i * 4 + 3] = (mk[i * 4] == 0) ? 255 : 0;
                    HeapFree(GetProcessHeap(), 0, mk);
                }
            }
            ReleaseDC(NULL, dc);
            if (GdipCreateBitmapFromScan0(w, h, 0, PXF_ARGB, NULL, &out) == 0 && out) {
                GpRect r = { 0, 0, w, h };
                GpBitmapData d;
                if (GdipBitmapLockBits(out, &r, 2, PXF_ARGB, &d) == 0) {
                    for (int y = 0; y < h; y++) memcpy((BYTE *)d.Scan0 + (INT_PTR)y * d.Stride, px + (SIZE_T)y * w * 4, (SIZE_T)w * 4);
                    GdipBitmapUnlockBits(out, &d);
                } else { GdipDisposeImage(out); out = NULL; }
            }
            HeapFree(GetProcessHeap(), 0, px);
        }
        DeleteObject(ii.hbmColor);
    }
    if (ii.hbmMask) DeleteObject(ii.hbmMask);
    if (!out) GdipCreateBitmapFromHICON(hi, &out);
    return out;
}

static void HsvRgb(float h, float s, float v, int *r, int *g, int *b) {
    float c = v * s, hp = h / 60.f, x = c * (1 - fabsf(fmodf(hp, 2) - 1)), r1 = 0, g1 = 0, b1 = 0;
    if (hp < 1) { r1 = c; g1 = x; } else if (hp < 2) { r1 = x; g1 = c; } else if (hp < 3) { g1 = c; b1 = x; }
    else if (hp < 4) { g1 = x; b1 = c; } else if (hp < 5) { r1 = x; b1 = c; } else { r1 = c; b1 = x; }
    float m = v - c;
    *r = (int)((r1 + m) * 255); *g = (int)((g1 + m) * 255); *b = (int)((b1 + m) * 255);
}

static GpBitmap *LetterTile(const WCHAR *name, int cs) {
    GpGraphics *g = NULL;
    GpBitmap *b = NewCache(cs, &g);
    if (!b) return NULL;
    float hue = (float)(Fnv(name) % 360);
    int r1, g1, b1, r2, g2, b2;
    HsvRgb(hue, 0.55f, 0.95f, &r1, &g1, &b1);
    HsvRgb(hue, 0.75f, 0.70f, &r2, &g2, &b2);
    float m = cs * 0.06f;
    RectF rc = { m, m, cs - 2 * m, cs - 2 * m };
    GpBrush *br;
    GdipCreateLineBrushFromRect(&rc, ARGBC(255, r1, g1, b1), ARGBC(255, r2, g2, b2), 1, 0, &br);
    FillRoundRect(g, br, m, m, cs - 2 * m, cs - 2 * m, cs * 0.22f);
    GdipDeleteBrush(br);
    WCHAR ch[3] = { 0, 0, 0 };
    const WCHAR *p = name;
    if (StrCmpNIW(p, L"www.", 4) == 0) p += 4;
    ch[0] = *p ? *p : L'?';
    CharUpperBuffW(ch, 1);
    GpFontFamily *fam = NULL;
    GpFont *f = NULL;
    if (GdipCreateFontFamilyFromName(L"Microsoft YaHei UI", NULL, &fam) != 0) GdipGetGenericFontFamilySansSerif(&fam);
    GdipCreateFont(fam, cs * 0.5f, 1, 2, &f);
    DrawCentered(g, ch, f, 0, 0, (float)cs, (float)cs, ARGBC(255, 255, 255, 255));
    GdipDeleteFont(f);
    GdipDeleteGraphics(g);
    return b;
}

/* favicon / website icon file -> cache bitmap (small icons go on a white tile) */
static GpBitmap *FaviconBitmap(const WCHAR *file, int cs) {
    GpBitmap *src = NULL;
    if (GdipCreateBitmapFromFile(file, &src) != 0 || !src) return NULL;
    UINT w = 0, h = 0;
    GdipGetImageWidth(src, &w);
    GdipGetImageHeight(src, &h);
    if (w < 8 || h < 8) { GdipDisposeImage(src); return NULL; }
    GpBitmap *out = NULL;
    if (w < 80) {
        GpGraphics *g = NULL;
        out = NewCache(cs, &g);
        if (out) {
            float m = cs * 0.06f;
            RectF rc = { m, m, cs - 2 * m, cs - 2 * m };
            GpBrush *br;
            GdipCreateLineBrushFromRect(&rc, ARGBC(255, 255, 255, 255), ARGBC(255, 232, 234, 240), 1, 0, &br);
            FillRoundRect(g, br, m, m, cs - 2 * m, cs - 2 * m, cs * 0.22f);
            GdipDeleteBrush(br);
            float s = cs * 0.55f, o = (cs - s) / 2;
            DrawImg(g, src, o, o, s, s, FALSE);
            GdipDeleteGraphics(g);
        }
    } else out = FitBitmap(src, cs, 0, 0.94f);
    GdipDisposeImage(src);
    return out;
}

static HICON ShellHicon(const WCHAR *path, LPCITEMIDLIST pidl) {
    SHFILEINFOW sfi;
    ZeroMemory(&sfi, sizeof sfi);
    DWORD fl = SHGFI_SYSICONINDEX | (pidl ? SHGFI_PIDL : 0);
    if (!SHGetFileInfoW(pidl ? (LPCWSTR)pidl : path, 0, &sfi, sizeof sfi, fl)) return NULL;
    HICON hi = NULL;
    static const int lists[2] = { SHIL_JUMBO, SHIL_EXTRALARGE };
    for (int k = 0; k < 2 && !hi; k++) {
        IImageList *il = NULL;
        if (SUCCEEDED(SHGetImageList(lists[k], &MY_IID_IImageList, (void **)&il)) && il) {
            IImageList_GetIcon(il, sfi.iIcon, ILD_TRANSPARENT, &hi);
            IImageList_Release(il);
        }
    }
    return hi;
}

static GpBitmap *HiconToCache(HICON hi, int cs) {
    GpBitmap *src = HiconToGp(hi);
    DestroyIcon(hi);
    if (!src) return NULL;
    GpBitmap *b = FitBitmap(src, cs, 0, 0.94f);
    GdipDisposeImage(src);
    return b;
}

static GpBitmap *ShellIconBitmap(const WCHAR *path, LPCITEMIDLIST pidl, int cs) {
    HICON hi = ShellHicon(path, pidl);
    if (!hi && !pidl) {
        SHFILEINFOW sfi;
        ZeroMemory(&sfi, sizeof sfi);
        if (SHGetFileInfoW(path, 0, &sfi, sizeof sfi, SHGFI_ICON | SHGFI_LARGEICON)) hi = sfi.hIcon;
    }
    return hi ? HiconToCache(hi, cs) : NULL;
}

/* ---- shortcuts ---- */
static BOOL ReadLnk(const WCHAR *lnk, WCHAR *target, WCHAR *args, WCHAR *iconLoc, int *iconIdx) {
    target[0] = 0; args[0] = 0; iconLoc[0] = 0; *iconIdx = 0;
    IShellLinkW *psl = NULL;
    BOOL ok = FALSE;
    if (SUCCEEDED(CoCreateInstance(&CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, &IID_IShellLinkW, (void **)&psl))) {
        IPersistFile *ppf = NULL;
        if (SUCCEEDED(IShellLinkW_QueryInterface(psl, &IID_IPersistFile, (void **)&ppf))) {
            if (SUCCEEDED(IPersistFile_Load(ppf, lnk, STGM_READ))) {
                IShellLinkW_GetPath(psl, target, MAX_PATH, NULL, 0);
                IShellLinkW_GetArguments(psl, args, 1024);
                IShellLinkW_GetIconLocation(psl, iconLoc, MAX_PATH, iconIdx);
                ok = TRUE;
            }
            IPersistFile_Release(ppf);
        }
        IShellLinkW_Release(psl);
    }
    return ok;
}

static GpBitmap *LnkIconBitmap(const WCHAR *lnk, int cs) {
    WCHAR t[MAX_PATH], a[1024], il[MAX_PATH], ex[MAX_PATH];
    int idx;
    if (!ReadLnk(lnk, t, a, il, &idx) || !il[0]) return NULL;
    ExpandEnvironmentStringsW(il, ex, MAX_PATH);
    HICON hi = NULL;
    static const int sizes[3] = { 256, 128, 64 };
    for (int k = 0; k < 3 && !hi; k++) {
        UINT id = 0;
        if (PrivateExtractIconsW(ex, idx, sizes[k], sizes[k], &hi, &id, 1, 0) != 1) hi = NULL;
    }
    return hi ? HiconToCache(hi, cs) : NULL;
}

/* ---- downloads (favicons / custom icon links) ---- */
typedef struct { int mode; WCHAR host[256]; WCHAR url[1024]; WCHAR file[MAX_PATH]; } DlJob;

static UINT BmpWidth(const WCHAR *f) {
    GpBitmap *b = NULL;
    UINT w = 0;
    if (GdipCreateBitmapFromFile(f, &b) == 0 && b) { GdipGetImageWidth(b, &w); GdipDisposeImage(b); }
    return w;
}

static BOOL DlOne(const WCHAR *url, const WCHAR *file) {
    DeleteFileW(file);
    if (URLDownloadToFileW(NULL, url, file, 0, NULL) != S_OK) { DeleteFileW(file); return FALSE; }
    WIN32_FILE_ATTRIBUTE_DATA fa;
    if (!GetFileAttributesExW(file, GetFileExInfoStandard, &fa) || fa.nFileSizeLow < 60) { DeleteFileW(file); return FALSE; }
    return TRUE;
}

static DWORD WINAPI DlThread(LPVOID p) {
    DlJob *j = (DlJob *)p;
    CoInitializeEx(NULL, COINIT_MULTITHREADED);
    WCHAR tmp[MAX_PATH + 8];
    if (j->mode == 1) {
        lstrcpyW(tmp, j->file);
        lstrcatW(tmp, L".tmp");
        if (DlOne(j->url, tmp)) MoveFileExW(tmp, j->file, MOVEFILE_REPLACE_EXISTING);
    } else {
        static const WCHAR *fmts[4] = { L"https://www.google.com/s2/favicons?domain=%s&sz=128", L"https://%s/apple-touch-icon.png",
                                        L"https://%s/favicon.ico", L"https://icons.duckduckgo.com/ip3/%s.ico" };
        WCHAR cand[4][MAX_PATH + 8] = { 0 }, u[700];
        UINT best = 0;
        int bi = -1;
        for (int k = 0; k < (int)(sizeof fmts / sizeof fmts[0]); k++) {
            wsprintfW(cand[k], L"%s.%d", j->file, k);
            wsprintfW(u, fmts[k], j->host);
            if (DlOne(u, cand[k])) {
                UINT w = BmpWidth(cand[k]);
                if (w > best) { best = w; bi = k; }
                if (w >= 120) break;
            }
        }
        if (bi >= 0) MoveFileExW(cand[bi], j->file, MOVEFILE_REPLACE_EXISTING);
        for (int k = 0; k < (int)(sizeof cand / sizeof cand[0]); k++) if (cand[k][0]) DeleteFileW(cand[k]);
    }
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_ICON, 0, 0);
    CoUninitialize();
    HeapFree(GetProcessHeap(), 0, j);
    return 0;
}

static void StartDownload(int mode, const WCHAR *url, const WCHAR *file) {
    DlJob *j = (DlJob *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(DlJob));
    if (!j) return;
    j->mode = mode;
    if (mode == 0) HostOf(url, j->host, 256); else lstrcpynW(j->url, url, 1024);
    lstrcpynW(j->file, file, MAX_PATH);
    HANDLE h = CreateThread(NULL, 0, DlThread, j, 0, NULL);
    if (h) CloseHandle(h); else HeapFree(GetProcessHeap(), 0, j);
}

/* ---- icon plate colour (shared by dock icons and the group tile) ---- */
static ARGB PlateArgb(void) {
    BOOL d = cfg.plateTheme == 0 ? FALSE : cfg.plateTheme == 1 ? TRUE : g_dark;
    int a = cfg.plateOpacity * 255 / 100;
    if (cfg.plateMode == 1) return d ? ARGBC(a, 48, 50, 58) : ARGBC(a, 244, 245, 249);
    return d ? ARGBC(a, 18, 19, 24) : ARGBC(a, 255, 255, 255);
}
/* one icon with the dock's icon plate behind it: application groups and the launchpad use this too, so
   the plate setting applies everywhere instead of only to the icons on the bar itself */
static void DrawIconPlated(GpGraphics *g, GpBitmap *ic, float x, float y, float sz, float radPct) {
    if (!ic) return;
    if (cfg.plateMode) {
        float pr = sz * radPct / 100.f;
        if (pr > sz / 2) pr = sz / 2;
        FillRR(g, PlateArgb(), x, y, sz, sz, pr);
        if (cfg.plateMode == 2) StrokeRR(g, ARGBC(60, 255, 255, 255), 1.f, x, y, sz, sz, pr);
        float in = sz * 0.13f;
        DrawImg(g, ic, x + in, y + in, sz - 2 * in, sz - 2 * in, FALSE);
    } else DrawImg(g, ic, x, y, sz, sz, FALSE);
}

/* ---- group thumbnail (first four apps in a 2x2 grid), follows the dock theme / plate style ---- */
static void BuildGroupIcon(Item *g) {
    if (g->icon) { GdipDisposeImage(g->icon); g->icon = NULL; }
    int cs = g_cacheSize;
    if (!(g->appmode & 2) && g->icon_src[0] && GetFileAttributesW(g->icon_src) != INVALID_FILE_ATTRIBUTES) {   /* user-chosen group icon */
        GpBitmap *sb = NULL;
        if (GdipCreateBitmapFromFile(g->icon_src, &sb) == 0 && sb) {
            g->icon = FitBitmap(sb, cs, g->fit, 0.94f);
            GdipDisposeImage(sb);
            if (g->icon) return;
        }
    }
    if (g->appmode & 2) {   /* wide: two icon slots, up to 8 thumbnails in a 4 x 2 grid */
        GpBitmap *wb = NULL;
        GpGraphics *wg = NULL;
        GdipCreateBitmapFromScan0(cs * 2, cs, 0, PXF_PARGB, NULL, &wb);
        if (!wb) return;
        GdipGetImageGraphicsContext(wb, &wg);
        SetupG(wg);
        float m = cs * 0.04f, ww = cs * 2 - 2 * m, hh = cs - 2 * m, rr = cs * (cfg.plateMode ? cfg.plateRadius / 100.f : 0.22f);
        if (rr > hh / 2) rr = hh / 2;
        if (cfg.plateMode) { /* the plate is drawn by the dock / launchpad / group strip, not baked in */ }
        else {
            RectF rc = { m, m, ww, hh };
            GpBrush *br;
            if (g_dark) GdipCreateLineBrushFromRect(&rc, ARGBC(78, 255, 255, 255), ARGBC(44, 255, 255, 255), 1, 0, &br);
            else GdipCreateLineBrushFromRect(&rc, ARGBC(205, 255, 255, 255), ARGBC(185, 226, 229, 238), 1, 0, &br);
            FillRoundRect(wg, br, m, m, ww, hh, rr);
            GdipDeleteBrush(br);
            StrokeRR(wg, g_dark ? ARGBC(90, 255, 255, 255) : ARGBC(55, 0, 0, 0), cs * 0.015f + 0.5f, m, m, ww, hh, rr);
        }
        float cell = cs * 0.37f, gap = cs * 0.07f, ox = (cs * 2 - 4 * cell - 3 * gap) / 2, oy = (cs - 2 * cell - gap) / 2;
        for (int k = 0; k < 8; k++) {
            float x = ox + (k % 4) * (cell + gap), y = oy + (k / 4) * (cell + gap);
            if (k < g->nk && g->kids[k].icon) DrawImg(wg, g->kids[k].icon, x, y, cell, cell, FALSE);
            else StrokeRR(wg, g_dark ? ARGBC(46, 255, 255, 255) : ARGBC(40, 0, 0, 0), 1.2f, x + cell * 0.1f, y + cell * 0.1f, cell * 0.8f, cell * 0.8f, cell * 0.2f);
        }
        GdipDeleteGraphics(wg);
        g->icon = wb;
        return;
    }
    GpGraphics *gr = NULL;
    GpBitmap *b = NewCache(cs, &gr);
    if (!b) return;
    float m = cs * 0.04f, w = cs - 2 * m;
    float rr = cs * (cfg.plateMode ? cfg.plateRadius / 100.f : 0.22f);
    if (rr > w / 2) rr = w / 2;
    if (cfg.plateMode) { /* the plate is drawn by the dock / launchpad / group strip, not baked in */ }
    else {
        RectF rc = { m, m, w, w };
        GpBrush *br;
        if (g_dark) GdipCreateLineBrushFromRect(&rc, ARGBC(78, 255, 255, 255), ARGBC(44, 255, 255, 255), 1, 0, &br);
        else GdipCreateLineBrushFromRect(&rc, ARGBC(205, 255, 255, 255), ARGBC(185, 226, 229, 238), 1, 0, &br);
        FillRoundRect(gr, br, m, m, w, w, rr);
        GdipDeleteBrush(br);
        StrokeRR(gr, g_dark ? ARGBC(90, 255, 255, 255) : ARGBC(55, 0, 0, 0), cs * 0.015f + 0.5f, m, m, w, w, rr);
    }
    float cell = cs * 0.34f, gap = cs * 0.07f, o = (cs - 2 * cell - gap) / 2;
    for (int k = 0; k < g->nk && k < 4; k++)
        if (g->kids[k].icon) DrawImg(gr, g->kids[k].icon, o + (k % 2) * (cell + gap), o + (k / 2) * (cell + gap), cell, cell, FALSE);
    GdipDeleteGraphics(gr);
    g->icon = b;
}

static void RebuildGroups(void) {
    for (int i = 0; i < nItems; i++) if (items[i].type == T_GROUP) BuildGroupIcon(&items[i]);
}

/* Windows-logo icon for the "Start" shortcut */
static GpBitmap *StartIcon(int cs) {
    GpGraphics *g = NULL;
    GpBitmap *b = NewCache(cs, &g);
    if (!b) return NULL;
    float m = cs * 0.06f, w = cs - 2 * m;
    RectF rc = { m, m, w, w };
    GpBrush *br;
    GdipCreateLineBrushFromRect(&rc, ARGBC(255, 44, 150, 255), ARGBC(255, 10, 84, 215), 1, 0, &br);
    FillRoundRect(g, br, m, m, w, w, cs * 0.22f);
    GdipDeleteBrush(br);
    float q = w * 0.27f, gp = w * 0.04f, o = m + (w - 2 * q - gp) / 2;
    for (int i = 0; i < 4; i++) FillRR(g, ARGBC(255, 255, 255, 255), o + (i % 2) * (q + gp), o + (i / 2) * (q + gp), q, q, q * 0.08f);
    GdipDeleteGraphics(g);
    return b;
}

static GpBitmap *LaunchIcon(int cs) {
    GpGraphics *g = NULL;
    GpBitmap *b = NewCache(cs, &g);
    if (!b) return NULL;
    float m = cs * 0.06f, w = cs - 2 * m;
    RectF rc = { m, m, w, w };
    GpBrush *br;
    GdipCreateLineBrushFromRect(&rc, ARGBC(255, 96, 104, 126), ARGBC(255, 44, 48, 64), 1, 0, &br);
    FillRoundRect(g, br, m, m, w, w, cs * 0.22f);
    GdipDeleteBrush(br);
    static const ARGB cols[9] = { 0xFFFF5F57, 0xFFFEBC2E, 0xFF28C840, 0xFF0A84FF, 0xFFBF5AF2, 0xFFFF9F0A, 0xFF64D2FF, 0xFFFF375F, 0xFF30D158 };
    float q = w * 0.2f, gp = w * 0.075f, o = m + (w - 3 * q - 2 * gp) / 2;
    for (int i = 0; i < 9; i++) FillRR(g, cols[i], o + (i % 3) * (q + gp), o + (i / 3) * (q + gp), q, q, q * 0.28f);
    GdipDeleteGraphics(g);
    return b;
}

static GpBitmap *NoteIcon(int cs) {
    GpGraphics *g = NULL;
    GpBitmap *b = NewCache(cs, &g);
    if (!b) return NULL;
    float m = cs * 0.06f, w = cs - 2 * m;
    RectF rc = { m, m, w, w };
    GpBrush *br;
    GdipCreateLineBrushFromRect(&rc, ARGBC(255, 255, 224, 110), ARGBC(255, 255, 190, 40), 1, 0, &br);
    FillRoundRect(g, br, m, m, w, w, cs * 0.22f);
    GdipDeleteBrush(br);
    FillRR(g, ARGBC(60, 120, 70, 0), m, m + w * 0.22f, w, 2, 0);
    GpPen *pn;
    GdipCreatePen1(ARGBC(150, 130, 80, 0), cs * 0.045f, 2, &pn);
    for (int i = 0; i < 4; i++) GdipDrawLine(g, pn, m + w * 0.2f, m + w * (0.36f + i * 0.14f), m + w * (i == 3 ? 0.55f : 0.8f), m + w * (0.36f + i * 0.14f));
    GdipDeletePen(pn);
    GdipDeleteGraphics(g);
    return b;
}

static void BuildWinIcon(Item *it) {
    if (it->icon) { GdipDisposeImage(it->icon); it->icon = NULL; }
    int cs = g_cacheSize;
    GpGraphics *g = NULL;
    GpBitmap *b = NewCache(cs, &g);
    if (!b) return;
    float m = cs * 0.04f, w = cs - 2 * m, rr = cs * 0.2f;
    FillRR(g, g_dark ? ARGBC(255, 52, 54, 62) : ARGBC(255, 255, 255, 255), m, m, w, w, rr);
    HWND hw = it->hwnd;
    RECT r;
    if (hw && IsWindow(hw) && !IsIconic(hw) && GetWindowRect(hw, &r)) {
        int ww = r.right - r.left, wh = r.bottom - r.top;
        if (ww > 60 && wh > 60 && ww < 4000 && wh < 3000) {
            HDC sdc = GetDC(NULL), mdc = CreateCompatibleDC(sdc);
            HBITMAP hb = CreateCompatibleBitmap(sdc, ww, wh);
            HGDIOBJ old = SelectObject(mdc, hb);
            BOOL ok = PrintWindow(hw, mdc, 2);   /* PW_RENDERFULLCONTENT */
            SelectObject(mdc, old);
            GpBitmap *sb = NULL;
            if (ok) GdipCreateBitmapFromHBITMAP(hb, NULL, &sb);
            DeleteObject(hb); DeleteDC(mdc); ReleaseDC(NULL, sdc);
            if (sb) {
                float pad = cs * 0.1f, iw = cs - 2 * pad, ih = cs * 0.68f - pad;
                float sc = iw / ww < ih / wh ? iw / ww : ih / wh, dw = ww * sc, dh = wh * sc;
                DrawImg(g, sb, (cs - dw) / 2, pad + (ih - dh) / 2, dw, dh, FALSE);
                StrokeRR(g, ARGBC(70, 0, 0, 0), 1.f, (cs - dw) / 2, pad + (ih - dh) / 2, dw, dh, cs * 0.02f);
                GdipDisposeImage(sb);
            }
        }
    }
    GpBitmap *ai = ShellIconBitmap(it->target[0] ? it->target : it->path, NULL, cs / 2);
    if (ai) { DrawImg(g, ai, cs * 0.5f, cs * 0.5f, cs * 0.44f, cs * 0.44f, FALSE); GdipDisposeImage(ai); }
    StrokeRR(g, g_dark ? ARGBC(90, 255, 255, 255) : ARGBC(55, 0, 0, 0), cs * 0.015f + 0.5f, m, m, w, w, rr);
    GdipDeleteGraphics(g);
    it->icon = b;
    it->snapTick = GetTickCount();
}

static void LoadItemIcon(Item *it) {
    if (it->icon) { GdipDisposeImage(it->icon); it->icon = NULL; }
    if (it->iconDark) { GdipDisposeImage(it->iconDark); it->iconDark = NULL; }
    it->fav = 0;
    if (it->type == T_SEP) return;
    if (it->type == T_GROUP) { BuildGroupIcon(it); return; }
    if (it->type == T_WIN) { BuildWinIcon(it); return; }
    int cs = g_cacheSize;
    if (it->type == T_NOTE) { it->icon = NoteIcon(cs); return; }
    if (it->type == T_FILE && !lstrcmpW(it->path, L"mdock:start")) { it->icon = StartIcon(cs); return; }
    if (it->type == T_FILE && !lstrcmpW(it->path, L"mdock:launchpad")) { it->icon = LaunchIcon(cs); return; }
    if (it->icon_src[0] && GetFileAttributesW(it->icon_src) != INVALID_FILE_ATTRIBUTES) {
        GpBitmap *s = NULL;
        if (GdipCreateBitmapFromFile(it->icon_src, &s) == 0 && s) {
            it->icon = FitBitmap(s, cs, it->fit, 0.94f);
            GdipDisposeImage(s);
            if (it->icon) it->fav = 1;
        }
    }
    if (!it->icon) {
        if (it->type == T_URL) {
            WCHAR f[MAX_PATH], host[256];
            IconFileFor(it->path, f);
            if (GetFileAttributesW(f) != INVALID_FILE_ATTRIBUTES) {
                it->icon = FaviconBitmap(f, cs);
                if (it->icon) { if (!it->icon_src[0]) it->fav = 1; } else DeleteFileW(f);
            } else if (!g_noDl) StartDownload(0, it->path, f);
            if (!it->icon) { HostOf(it->path, host, 256); it->icon = LetterTile(host, cs); }
        } else if (it->type == T_TRASH) {
            LPITEMIDLIST pidl = NULL;
            if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_BITBUCKET, &pidl)) && pidl) {
                it->icon = ShellIconBitmap(NULL, pidl, cs);
                CoTaskMemFree(pidl);
            }
        } else {
            if (it->type == T_FILE && (StrCmpNIW(it->path, L"shell:", 6) == 0 || StrCmpNIW(it->path, L"::{", 3) == 0)) {
                LPITEMIDLIST pidl = NULL;
                if (SUCCEEDED(SHParseDisplayName(it->path, NULL, &pidl, 0, NULL)) && pidl) { it->icon = ShellIconBitmap(NULL, pidl, cs); CoTaskMemFree(pidl); }
            } else {
                if (it->type == T_FILE && PathMatchSpecW(it->path, L"*.lnk")) it->icon = LnkIconBitmap(it->path, cs);
                if (!it->icon) it->icon = ShellIconBitmap(it->path, NULL, cs);
            }
        }
    }
    if (!it->icon) it->icon = LetterTile(it->name, cs);
    if (it->icon_dark[0] && GetFileAttributesW(it->icon_dark) != INVALID_FILE_ATTRIBUTES) {
        GpBitmap *sd = NULL;
        if (GdipCreateBitmapFromFile(it->icon_dark, &sd) == 0 && sd) {
            it->iconDark = FitBitmap(sd, cs, it->fit, 0.94f);
            GdipDisposeImage(sd);
        }
    }
}

static void ReloadAllIcons(void) {
    for (int i = 0; i < nItems; i++) {
        LoadItemIcon(&items[i]);
        for (int k = 0; k < items[i].nk; k++) LoadItemIcon(&items[i].kids[k]);
        if (items[i].type == T_GROUP) BuildGroupIcon(&items[i]);
    }
}

/* ======================= items ======================= */
static void FileDescr(const WCHAR *path, WCHAR *out, int cch) {
    out[0] = 0;
    DWORD h;
    DWORD sz = GetFileVersionInfoSizeW(path, &h);
    if (!sz) return;
    void *buf = LocalAlloc(LMEM_FIXED, sz);
    if (!buf) return;
    if (GetFileVersionInfoW(path, 0, sz, buf)) {
        struct { WORD l, c; } *tr = NULL;
        UINT len = 0;
        if (VerQueryValueW(buf, L"\\VarFileInfo\\Translation", (LPVOID *)&tr, &len) && len >= 4) {
            WCHAR q[96], *v = NULL;
            wsprintfW(q, L"\\StringFileInfo\\%04x%04x\\FileDescription", tr[0].l, tr[0].c);
            if (VerQueryValueW(buf, q, (LPVOID *)&v, &len) && len > 1) lstrcpynW(out, v, cch);
            if (!out[0]) {
                wsprintfW(q, L"\\StringFileInfo\\%04x%04x\\ProductName", tr[0].l, tr[0].c);
                if (VerQueryValueW(buf, q, (LPVOID *)&v, &len) && len > 1) lstrcpynW(out, v, cch);
            }
        }
    }
    LocalFree(buf);
}

static void CleanName(WCHAR *n) {
    for (WCHAR *p = n; *p; p++) if (*p == L'|' || *p == L'\r' || *p == L'\n') *p = L'/';
}

static BOOL GenericDir(const WCHAR *d) {   /* too common to identify one application */
    WCHAR win[MAX_PATH];
    GetWindowsDirectoryW(win, MAX_PATH);
    if (StrCmpNIW(d, win, lstrlenW(win)) == 0) return TRUE;
    int n = 0;
    for (const WCHAR *q = d; *q; q++) if (*q == L'\\') n++;
    return n < 2;
}

static void ResolveTarget(Item *it) {
    it->target[0] = 0; it->tdir[0] = 0; it->aumid[0] = 0;
    if (it->type != T_FILE) return;
    WCHAR any[MAX_PATH] = L"";
    if (StrCmpNIW(it->path, L"shell:AppsFolder\\", 17) == 0) { lstrcpynW(it->aumid, it->path + 17, 160); return; }
    if (PathMatchSpecW(it->path, L"*.lnk")) {
        WCHAR t[MAX_PATH], a[1024], il[MAX_PATH];
        int ix;
        if (ReadLnk(it->path, t, a, il, &ix) && t[0]) {
            lstrcpynW(any, t, MAX_PATH);
            if (PathMatchSpecW(t, L"*.exe")) lstrcpynW(it->target, t, MAX_PATH);
        }
    } else if (PathMatchSpecW(it->path, L"*.exe")) { lstrcpynW(it->target, it->path, MAX_PATH); lstrcpynW(any, it->path, MAX_PATH); }
    if (any[0]) {
        PathRemoveFileSpecW(any);
        if (any[0] && !GenericDir(any)) lstrcpynW(it->tdir, any, MAX_PATH);
    }
}

static void FillItem(Item *it, int type, int appmode, const WCHAR *name, const WCHAR *path, const WCHAR *icon) {
    ZeroMemory(it, sizeof(Item));
    it->type = type;
    it->appmode = appmode;
    if (name) lstrcpynW(it->name, name, 128);
    if (path) lstrcpynW(it->path, path, 1024);
    if (icon) lstrcpynW(it->icon_src, icon, 512);
    CleanName(it->name);
    ResolveTarget(it);
    if (type != T_GROUP) LoadItemIcon(it);
}

static void FreeItem(Item *it) {
    if (it->icon) GdipDisposeImage(it->icon);
    if (it->kids) {
        for (int k = 0; k < it->nk; k++) if (it->kids[k].icon) GdipDisposeImage(it->kids[k].icon);
        HeapFree(GetProcessHeap(), 0, it->kids);
    }
    it->icon = NULL; it->kids = NULL; it->nk = 0;
}

static int PinnedCount(void) {
    for (int i = 0; i < nItems; i++) if (items[i].temp) return i;
    return nItems;
}

/* Insert an already built item (ownership of icon/kids moves into the array) */
static int InsertItemStruct(int idx, Item *src, BOOL temp) {
    if (nItems >= MAXI) { FreeItem(src); return -1; }
    int pc = PinnedCount();
    if (!temp && (idx < 0 || idx > pc)) idx = pc;
    if (temp && (idx < 0 || idx > nItems)) idx = nItems;
    for (int i = 0; i < nItems; i++) items[i].prevL = (i < idx) ? slotL[i] : slotL[i];
    memmove(&items[idx + 1], &items[idx], sizeof(Item) * (nItems - idx));
    nItems++;
    items[idx] = *src;
    items[idx].temp = temp;
    items[idx].cur = 0;
    items[idx].slide = 0;
    items[idx].prevL = slotL[idx < MAXI ? idx : 0];
    gMoved = TRUE;
    return idx;
}

static void RemoveItem(int i) {
    if (i < 0 || i >= nItems) return;
    FreeItem(&items[i]);
    memmove(&items[i], &items[i + 1], sizeof(Item) * (nItems - i - 1));
    nItems--;
    prevN = -1;
    ScheduleSave();
    Kick();
}

static void MoveItem(int from, int to) {
    if (from == to || from < 0 || to < 0 || from >= nItems || to >= nItems) return;
    for (int i = 0; i < nItems; i++) items[i].prevL = slotL[i];
    Item tmp = items[from];
    if (from < to) memmove(&items[from], &items[from + 1], sizeof(Item) * (to - from));
    else memmove(&items[to + 1], &items[to], sizeof(Item) * (from - to));
    items[to] = tmp;
    gMoved = TRUE;
}

static BOOL KidAdd(Item *g, Item *kid) {
    if (!g->kids) {
        g->kids = (Item *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Item) * MAXK);
        if (!g->kids) return FALSE;
    }
    if (g->nk >= MAXK) return FALSE;
    kid->temp = 0; kid->cur = 0; kid->kids = NULL; kid->nk = 0;
    g->kids[g->nk++] = *kid;
    BuildGroupIcon(g);
    return TRUE;
}

static void MergeInto(int from, int to) {
    if (from == to || from < 0 || to < 0 || from >= nItems || to >= nItems) return;
    Item *a = &items[from], *t = &items[to];
    if (a->type == T_SEP || a->type == T_GROUP || t->type == T_SEP) return;
    Item dragged = *a;
    if (t->type == T_GROUP) {
        if (!KidAdd(t, &dragged)) return;
    } else {
        Item g;
        ZeroMemory(&g, sizeof g);
        g.type = T_GROUP;
        lstrcpyW(g.name, L"应用组");
        g.temp = t->temp;
        g.cur = t->cur;
        g.kids = (Item *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Item) * MAXK);
        if (!g.kids) return;
        g.kids[0] = *t; g.kids[0].temp = 0; g.kids[0].cur = 0;
        g.kids[1] = dragged; g.kids[1].temp = 0; g.kids[1].cur = 0;
        g.nk = 2;
        BuildGroupIcon(&g);
        items[to] = g;
    }
    memmove(&items[from], &items[from + 1], sizeof(Item) * (nItems - from - 1));
    nItems--;
    prevN = -1;
    ScheduleSave();
}

static void NewEmptyGroup(BOOL temp) {
    Item g;
    ZeroMemory(&g, sizeof g);
    g.type = T_GROUP;
    g.appmode = temp ? 3 : 0;   /* group appmode: bit0 = temporary group (emptied after a restart), bit1 = wide icon (8 thumbnails) */
    lstrcpyW(g.name, temp ? L"临时周转" : L"应用组");
    g.kids = (Item *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Item) * MAXK);
    BuildGroupIcon(&g);
    InsertItemStruct(-1, &g, FALSE);
    ScheduleSave();
}

static void ExtractKid(int gi, int k) {
    if (gi < 0 || gi >= nItems || items[gi].type != T_GROUP || k < 0 || k >= items[gi].nk) return;
    Item kid = items[gi].kids[k];
    memmove(&items[gi].kids[k], &items[gi].kids[k + 1], sizeof(Item) * (items[gi].nk - k - 1));
    items[gi].nk--;
    BuildGroupIcon(&items[gi]);
    InsertItemStruct(gi + 1, &kid, items[gi].temp);
    ScheduleSave();
}

static void DissolveGroup(int gi) {
    if (gi < 0 || gi >= nItems || items[gi].type != T_GROUP) return;
    while (items[gi].nk > 0) ExtractKid(gi, items[gi].nk - 1);
    RemoveItem(gi);
}

/* Edge / Chrome installed web apps keep their icon under ...\Web Applications\_crx_<id>\*.ico */
static BOOL BuildItem(Item *it, const WCHAR *s0);
static BOOL FindPwaIcon(const WCHAR *args, WCHAR *out) {
    const WCHAR *p = wcsstr(args, L"--app-id=");
    if (!p) return FALSE;
    WCHAR id[40];
    int n = 0;
    p += 9;
    while (*p && *p != L' ' && *p != L'"' && n < 38) id[n++] = *p++;
    id[n] = 0;
    WCHAR local[MAX_PATH];
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, local))) return FALSE;
    static const WCHAR *bases[2] = { L"Microsoft\\Edge\\User Data", L"Google\\Chrome\\User Data" };
    static const WCHAR *profs[4] = { L"Default", L"Profile 1", L"Profile 2", L"Profile 3" };
    for (int b = 0; b < 2; b++)
        for (int q = 0; q < 4; q++) {
            WCHAR pat[MAX_PATH * 2], dir[MAX_PATH * 2];
            wsprintfW(dir, L"%s\\%s\\%s\\Web Applications\\_crx_%s", local, bases[b], profs[q], id);
            wsprintfW(pat, L"%s\\*.ico", dir);
            WIN32_FIND_DATAW fd;
            HANDLE h = FindFirstFileW(pat, &fd);
            if (h != INVALID_HANDLE_VALUE) { wsprintfW(out, L"%s\\%s", dir, fd.cFileName); FindClose(h); return TRUE; }
        }
    return FALSE;
}

static BOOL BuildFromPidl(Item *it, LPCITEMIDLIST pidl) {
    PWSTR parse = NULL, disp = NULL;
    if (FAILED(SHGetNameFromIDList(pidl, SIGDN_DESKTOPABSOLUTEPARSING, &parse)) || !parse) return FALSE;
    SHGetNameFromIDList(pidl, SIGDN_NORMALDISPLAY, &disp);
    BOOL ok = FALSE;
    if (GetFileAttributesW(parse) != INVALID_FILE_ATTRIBUTES) ok = BuildItem(it, parse);
    else if (StrCmpNIW(parse, L"shell:", 6) == 0 || StrCmpNIW(parse, L"::{", 3) == 0) {
        FillItem(it, T_FILE, 0, disp ? disp : L"应用", parse, NULL);
        ok = TRUE;
    }
    if (parse) CoTaskMemFree(parse);
    if (disp) CoTaskMemFree(disp);
    return ok;
}

/* Build an item from a dropped path / url / shortcut. Returns FALSE when unusable. */
static BOOL BuildItem(Item *it, const WCHAR *s0) {
    WCHAR s[1024], name[128];
    lstrcpynW(s, s0, 1024);
    StrTrimW(s, L" \t\r\n\"");
    if (!s[0]) return FALSE;
    if (IsUrlStr(s)) {
        WCHAR url[1024];
        if (StrCmpNIW(s, L"www.", 4) == 0) wsprintfW(url, L"https://%s", s); else lstrcpyW(url, s);
        HostOf(url, name, 128);
        WCHAR *nm = name;
        if (StrCmpNIW(nm, L"www.", 4) == 0) nm += 4;
        FillItem(it, T_URL, cfg.webApp, nm, url, NULL);
        return TRUE;
    }
    if (StrCmpNIW(s, L"shell:", 6) == 0) { /* e.g. shell:AppsFolder\... from the Start menu */
        LPITEMIDLIST pidl = NULL;
        if (SUCCEEDED(SHParseDisplayName(s, NULL, &pidl, 0, NULL)) && pidl) {
            PWSTR disp = NULL;
            SHGetNameFromIDList(pidl, SIGDN_NORMALDISPLAY, &disp);
            FillItem(it, T_FILE, 0, disp ? disp : L"应用", s, NULL);
            if (disp) CoTaskMemFree(disp);
            CoTaskMemFree(pidl);
            return TRUE;
        }
        return FALSE;
    }
    DWORD at = GetFileAttributesW(s);
    if (at == INVALID_FILE_ATTRIBUTES) return FALSE;
    if (at & FILE_ATTRIBUTE_DIRECTORY) {
        WCHAR *b = PathFindFileNameW(s);
        lstrcpynW(name, (b && *b) ? b : s, 128);
        FillItem(it, T_FOLDER, 0, name, s, NULL);
        return TRUE;
    }
    BOOL isApp = PathMatchSpecW(s, L"*.exe;*.lnk;*.url;*.bat;*.cmd");
    lstrcpynW(name, PathFindFileNameW(s), 128);
    if (isApp) PathRemoveExtensionW(name);   /* documents keep their full file name, apps lose the extension */
    if (PathMatchSpecW(s, L"*.url")) {
        WCHAR url[1024] = L"";
        GetPrivateProfileStringW(L"InternetShortcut", L"URL", L"", url, 1024, s);
        if (url[0]) { FillItem(it, T_URL, cfg.webApp, name, url, NULL); return TRUE; }
    }
    WCHAR pwaIcon[MAX_PATH] = L"";
    if (PathMatchSpecW(s, L"*.lnk")) {
        WCHAR t[MAX_PATH], a[1024], il[MAX_PATH];
        int ix;
        if (ReadLnk(s, t, a, il, &ix)) {
            WCHAR *p = wcsstr(a, L"--app=");
            if (p && PathMatchSpecW(t, L"*.exe")) { /* browser shortcut with an explicit app URL */
                p += 6;
                WCHAR url[1024];
                int n = 0;
                BOOL q = (*p == L'"');
                if (q) p++;
                while (*p && n < 1023 && ((q && *p != L'"') || (!q && *p != L' '))) url[n++] = *p++;
                url[n] = 0;
                if (IsUrlStr(url)) { FillItem(it, T_URL, 1, name, url, NULL); return TRUE; }
            }
            if (!il[0]) FindPwaIcon(a, pwaIcon);   /* installed PWA: pick up its own icon */
        }
    }
    if (PathMatchSpecW(s, L"*.exe")) {
        WCHAR d[128];
        FileDescr(s, d, 128);
        if (d[0]) lstrcpynW(name, d, 128);
    }
    FillItem(it, T_FILE, 0, name, s, pwaIcon[0] ? pwaIcon : NULL);
    return TRUE;
}

static int AddFromString(const WCHAR *s, int idx) {
    Item it;
    if (!BuildItem(&it, s)) return -1;
    int r = InsertItemStruct(idx, &it, FALSE);
    if (r >= 0) ScheduleSave();
    return r;
}

static void AddNote(void) {
    for (int i = 0; i < nItems; i++) if (items[i].type == T_NOTE) return;
    Item it;
    FillItem(&it, T_NOTE, 0, L"速记", L"", NULL);
    InsertItemStruct(-1, &it, FALSE);
    ScheduleSave();
}
static void RemoveNote(void) {
    for (int i = nItems - 1; i >= 0; i--) if (items[i].type == T_NOTE) RemoveItem(i);
}

/* ---- quick-add system icons (left: Start, Settings, Explorer, This PC, Browser; right: Recycle Bin) ---- */
enum { SYS_START, SYS_SETTINGS, SYS_EXPLORER, SYS_PC, SYS_BROWSER, SYS_LAUNCH, SYS_TRASH, SYS_N };
static const WCHAR *g_sysName[SYS_N] = { L"开始", L"设置", L"文件资源管理器", L"此电脑", L"默认浏览器", L"起动台", L"回收站" };

static BOOL SysPath(int k, WCHAR *out) {
    out[0] = 0;
    switch (k) {
    case SYS_START: lstrcpyW(out, L"mdock:start"); return TRUE;
    case SYS_LAUNCH: lstrcpyW(out, L"mdock:launchpad"); return TRUE;
    case SYS_SETTINGS: lstrcpyW(out, L"shell:AppsFolder\\windows.immersivecontrolpanel_cw5n1h2txyewy!microsoft.windows.immersivecontrolpanel"); return TRUE;
    case SYS_EXPLORER: { WCHAR t[MAX_PATH]; ExpandEnvironmentStringsW(L"%windir%\\explorer.exe", t, MAX_PATH); lstrcpyW(out, t); return TRUE; }
    case SYS_PC: lstrcpyW(out, L"shell:MyComputerFolder"); return TRUE;
    case SYS_BROWSER: { DWORD n = MAX_PATH; return SUCCEEDED(AssocQueryStringW(ASSOCF_NOTRUNCATE, ASSOCSTR_EXECUTABLE, L"http", L"open", out, &n)) && out[0]; }
    }
    return FALSE;
}

static int SysFind(int k) {
    if (k == SYS_TRASH) { for (int i = 0; i < nItems; i++) if (items[i].type == T_TRASH) return i; return -1; }
    WCHAR p[MAX_PATH * 2];
    if (!SysPath(k, p)) return -1;
    for (int i = 0; i < nItems; i++) if (items[i].type == T_FILE && !items[i].temp && !lstrcmpiW(items[i].path, p)) return i;
    return -1;
}

static void SysAdd(int k) {
    if (SysFind(k) >= 0) return;
    Item it;
    if (k == SYS_TRASH) { FillItem(&it, T_TRASH, 0, L"回收站", L"shell:RecycleBinFolder", NULL); InsertItemStruct(PinnedCount(), &it, FALSE); }
    else {
        WCHAR p[MAX_PATH * 2], nm[128];
        if (!SysPath(k, p)) return;
        lstrcpynW(nm, g_sysName[k], 128);
        if (k == SYS_BROWSER) { FileDescr(p, nm, 128); if (!nm[0]) lstrcpyW(nm, g_sysName[k]); }
        FillItem(&it, T_FILE, 0, nm, p, NULL);
        int idx = 0;
        for (int q = 0; q < k; q++) if (SysFind(q) >= 0) idx++;
        InsertItemStruct(idx, &it, FALSE);
    }
    ScheduleSave();
}

static void SysRemove(int k) {
    int i = SysFind(k);
    if (i >= 0) RemoveItem(i);
}

/* ======================= save / load ======================= */
static int SplitFields(WCHAR *v, WCHAR **f, int max) {
    int n = 0;
    f[n++] = v;
    for (WCHAR *p = v; *p && n < max; p++)
        if (*p == L'|') { *p = 0; f[n++] = p + 1; }
    return n;
}

typedef struct { WCHAR *b; size_t n, cap; } WBuf;
static void WCat(WBuf *w, const WCHAR *t) {
    size_t l = lstrlenW(t);
    if (w->n + l + 4 > w->cap) {
        while (w->n + l + 4 > w->cap) w->cap *= 2;
        w->b = (WCHAR *)HeapReAlloc(GetProcessHeap(), 0, w->b, w->cap * sizeof(WCHAR));
    }
    memcpy(w->b + w->n, t, l * sizeof(WCHAR));
    w->n += l;
    w->b[w->n] = 0;
}
static void WItem(WBuf *w, const WCHAR *key, const Item *it) {
    WCHAR t[96];
    wsprintfW(t, L"%s=%d|%d|", key, it->type, it->appmode);
    WCat(w, t); WCat(w, it->name); WCat(w, L"|"); WCat(w, it->path); WCat(w, L"|"); WCat(w, it->icon_src);
    wsprintfW(t, L"|%d|", it->fit);
    WCat(w, t); WCat(w, it->args); WCat(w, L"|"); WCat(w, it->icon_dark); WCat(w, L"\r\n");
}

/* the whole ini is serialised in memory and swapped in atomically, so a crash or a cleaner tool killing the
   process can never leave a half-written configuration behind */
static void SaveNow(void) {
    WBuf w = { (WCHAR *)HeapAlloc(GetProcessHeap(), 0, 32768 * sizeof(WCHAR)), 0, 32768 }, gr = { (WCHAR *)HeapAlloc(GetProcessHeap(), 0, 8192 * sizeof(WCHAR)), 0, 8192 };
    if (!w.b || !gr.b) return;
    w.b[0] = 0; gr.b[0] = 0;
    WCat(&w, L"[Settings]\r\n");
    for (int i = 0; i < NTAB; i++) {
        WCHAR t[96];
        wsprintfW(t, L"%s=%d\r\n", g_tab[i].k, *g_tab[i].p);
        WCat(&w, t);
    }
    WCat(&w, L"[Notes]\r\nDir="); WCat(&w, g_noteDir); WCat(&w, L"\r\n");
    WCat(&w, L"[Items]\r\n");
    WCat(&gr, L"[Groups]\r\n");
    int si = 0;
    for (int i = 0; i < nItems; i++) {
        Item *it = &items[i];
        if (it->temp) continue;
        WCHAR k[32], t[48];
        wsprintfW(k, L"I%d", si);
        WItem(&w, k, it);
        if (it->type == T_GROUP) {
            wsprintfW(t, L"N%d=%d\r\n", si, it->nk);
            WCat(&gr, t);
            for (int j = 0; j < it->nk; j++) { wsprintfW(k, L"G%d_%d", si, j); WItem(&gr, k, &it->kids[j]); }
        }
        si++;
    }
    { WCHAR t[48]; wsprintfW(t, L"Count=%d\r\n", si); WCat(&w, t); }
    WCat(&w, gr.b);
    WCat(&w, L"[TrayApps]\r\n");
    { WCHAR t[32]; wsprintfW(t, L"Count=%d\r\n", g_ntrayApps); WCat(&w, t); }
    for (int i = 0; i < g_ntrayApps; i++) {
        WCHAR k[32];
        wsprintfW(k, L"Path%d=", i); WCat(&w, k); WCat(&w, g_trayApps[i].exe); WCat(&w, L"\r\n");
        wsprintfW(k, L"Class%d=", i); WCat(&w, k); WCat(&w, g_trayApps[i].cls); WCat(&w, L"\r\n");
        wsprintfW(k, L"Title%d=", i); WCat(&w, k); WCat(&w, g_trayApps[i].title); WCat(&w, L"\r\n");
        wsprintfW(k, L"Name%d=", i); WCat(&w, k); WCat(&w, g_trayApps[i].name); WCat(&w, L"\r\n");
        WCHAR identity[192];
        _snwprintf(identity, 191, L"Window%d=%llu\r\nPid%d=%lu\r\nStart%d=%llu\r\n", i,
                   (unsigned long long)(UINT_PTR)g_trayApps[i].hwnd, i, g_trayApps[i].pid,
                   i, (unsigned long long)g_trayApps[i].processStart);
        identity[191] = 0; WCat(&w, identity);
        if (g_trayApps[i].roleKnown) {
            _snwprintf(identity, 191, L"Style%d=%lu\r\nExStyle%d=%lu\r\n", i, g_trayApps[i].roleStyle, i, g_trayApps[i].roleExStyle);
            identity[191] = 0; WCat(&w, identity);
        }
    }
    WCHAR tmp[MAX_PATH + 8];
    lstrcpyW(tmp, g_ini);
    lstrcatW(tmp, L".tmp");
    HANDLE h = CreateFileW(tmp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        WORD bom = 0xFEFF;
        DWORD wr;
        BOOL ok = WriteFile(h, &bom, 2, &wr, NULL) && WriteFile(h, w.b, (DWORD)(w.n * sizeof(WCHAR)), &wr, NULL);
        CloseHandle(h);
        if (ok) MoveFileExW(tmp, g_ini, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
        else DeleteFileW(tmp);
    }
    HeapFree(GetProcessHeap(), 0, w.b);
    HeapFree(GetProcessHeap(), 0, gr.b);
}

static BOOL ParseItemLine(const WCHAR *line, Item *it) {
    WCHAR v[2600], *f[8] = { 0 };
    lstrcpynW(v, line, 2600);
    int n = SplitFields(v, f, 8);
    if (n < 4) return FALSE;
    int type = _wtoi(f[0]);
    if (type < 0 || type > 6) return FALSE;
    int fit = n >= 6 ? _wtoi(f[5]) : 0;
    WCHAR *icon = (n >= 5 && f[4][0]) ? f[4] : NULL;
    if (fit != 0 && icon) { /* load once with the saved fit mode */
        ZeroMemory(it, sizeof(Item));
        it->fit = fit;
        it->type = type; it->appmode = _wtoi(f[1]);
        lstrcpynW(it->name, f[2], 128); lstrcpynW(it->path, f[3], 1024); lstrcpynW(it->icon_src, icon, 512);
        if (n >= 7) lstrcpynW(it->args, f[6], 260);
        if (n >= 8) lstrcpynW(it->icon_dark, f[7], 512);
        ResolveTarget(it);
        LoadItemIcon(it);
        return TRUE;
    }
    FillItem(it, type, _wtoi(f[1]), f[2], f[3], icon);
    if (n >= 7) lstrcpynW(it->args, f[6], 260);
    if (n >= 8 && f[7][0]) { lstrcpynW(it->icon_dark, f[7], 512); LoadItemIcon(it); }
    return TRUE;
}

static BOOL NewSession(void) {   /* a volatile registry key disappears on restart / shutdown / logoff */
    HKEY hk;
    DWORD disp = 0;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\MiniDockSession", 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hk, &disp) != ERROR_SUCCESS) return FALSE;
    RegCloseKey(hk);
    return disp == REG_CREATED_NEW_KEY;
}

static void LoadItems(void) {
    BOOL newSess = NewSession();
    int cnt = (int)GetPrivateProfileIntW(L"Items", L"Count", 0, g_ini);
    for (int i = 0; i < cnt && i < MAXI; i++) {
        WCHAR k[24], v[2200];
        wsprintfW(k, L"I%d", i);
        GetPrivateProfileStringW(L"Items", k, L"", v, 2200, g_ini);
        Item it;
        if (!ParseItemLine(v, &it)) continue;
        if (it.type == T_GROUP) {
            wsprintfW(k, L"N%d", i);
            int nk = (int)GetPrivateProfileIntW(L"Groups", k, 0, g_ini);
            if ((it.appmode & 1) && newSess) nk = 0;
            for (int j = 0; j < nk && j < MAXK; j++) {
                Item kid;
                wsprintfW(k, L"G%d_%d", i, j);
                GetPrivateProfileStringW(L"Groups", k, L"", v, 2200, g_ini);
                if (ParseItemLine(v, &kid)) KidAdd(&it, &kid);
            }
            if (!it.kids) it.kids = (Item *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Item) * MAXK);
            BuildGroupIcon(&it);
        }
        InsertItemStruct(-1, &it, FALSE);
    }
    gMoved = FALSE;
}

static void ReloadConfigAll(void) {
    if (g_hwnd) WTSUnRegisterSessionNotification(g_hwnd);
    for (int i = 0; i < nItems; i++) FreeItem(&items[i]);
    nItems = 0;
    prevN = -1;
    LoadConfig();
    g_cacheSize = 96;
    g_cacheSize = CacheFor(Pxs(cfg.zoom ? (cfg.zoomSize > cfg.iconSize ? cfg.zoomSize : cfg.iconSize) : cfg.iconSize));
    LoadItems();
    ApplySettings();
}

/* ---- export / import (.mdock = ini + icon cache in one file) ---- */
static BOOL ReadWhole(const WCHAR *f, BYTE **buf, DWORD *n) {
    HANDLE h = CreateFileW(f, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return FALSE;
    *n = GetFileSize(h, NULL);
    *buf = (BYTE *)HeapAlloc(GetProcessHeap(), 0, *n + 1);
    DWORD r = 0;
    BOOL ok = *buf && ReadFile(h, *buf, *n, &r, NULL) && r == *n;
    CloseHandle(h);
    if (!ok && *buf) HeapFree(GetProcessHeap(), 0, *buf);
    return ok;
}

static void WriteEntry(HANDLE h, const WCHAR *name, const WCHAR *file) {
    BYTE *b = NULL;
    DWORD n = 0, w;
    if (!ReadWhole(file, &b, &n)) return;
    DWORD nl = lstrlenW(name);
    WriteFile(h, &nl, 4, &w, NULL);
    WriteFile(h, name, nl * 2, &w, NULL);
    WriteFile(h, &n, 4, &w, NULL);
    WriteFile(h, b, n, &w, NULL);
    HeapFree(GetProcessHeap(), 0, b);
}

static void CopySection(const WCHAR *sec, const WCHAR *from, const WCHAR *to) {
    WCHAR *buf = (WCHAR *)HeapAlloc(GetProcessHeap(), 0, 65536 * sizeof(WCHAR));
    if (!buf) return;
    DWORD n = GetPrivateProfileSectionW(sec, buf, 65535, from);
    if (n > 0) WritePrivateProfileSectionW(sec, buf, to);
    HeapFree(GetProcessHeap(), 0, buf);
}

/* kind: 0 = everything, 1 = settings only, 2 = apps / groups / icons only */
static void ExportConfig(HWND parent, int kind) {
    SaveNow();
    WCHAR f[MAX_PATH];
    lstrcpyW(f, kind == 1 ? L"MiniDock设置参数.mdock" : kind == 2 ? L"MiniDock应用与图标.mdock" : L"MiniDock完整配置.mdock");
    OPENFILENAMEW o;
    ZeroMemory(&o, sizeof o);
    o.lStructSize = sizeof o; o.hwndOwner = parent;
    o.lpstrFilter = L"MiniDock 配置 (*.mdock)\0*.mdock\0";
    o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.lpstrDefExt = L"mdock";
    o.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if (!GetSaveFileNameW(&o)) return;
    WCHAR tmp[MAX_PATH];
    lstrcpyW(tmp, g_ini);
    lstrcatW(tmp, L".exp");
    DeleteFileW(tmp);
    { HANDLE h = CreateFileW(tmp, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
      if (h != INVALID_HANDLE_VALUE) { WORD bom = 0xFEFF; DWORD w; WriteFile(h, &bom, 2, &w, NULL); CloseHandle(h); } }
    if (kind != 2) CopySection(L"Settings", g_ini, tmp);
    if (kind != 1) { CopySection(L"Items", g_ini, tmp); CopySection(L"Groups", g_ini, tmp); }
    HANDLE h = CreateFileW(f, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) { DeleteFileW(tmp); return; }
    DWORD w, cnt = 0;
    WIN32_FIND_DATAW fd;
    WCHAR pat[MAX_PATH], full[MAX_PATH], nm[MAX_PATH];
    PathCombineW(pat, g_iconDir, L"*");
    if (kind != 1) {
        HANDLE ff = FindFirstFileW(pat, &fd);
        if (ff != INVALID_HANDLE_VALUE) { do { if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) cnt++; } while (FindNextFileW(ff, &fd)); FindClose(ff); }
    }
    cnt++;
    WriteFile(h, "MDK1", 4, &w, NULL);
    WriteFile(h, &cnt, 4, &w, NULL);
    WriteEntry(h, L"MiniDock.ini", tmp);
    if (kind != 1) {
        HANDLE ff = FindFirstFileW(pat, &fd);
        if (ff != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                PathCombineW(full, g_iconDir, fd.cFileName);
                wsprintfW(nm, L"icons\\%s", fd.cFileName);
                WriteEntry(h, nm, full);
            } while (FindNextFileW(ff, &fd));
            FindClose(ff);
        }
    }
    CloseHandle(h);
    DeleteFileW(tmp);
    MessageBoxW(parent, L"已导出。", L"MiniDock", MB_OK | MB_ICONINFORMATION);
}

static void ImportConfig(HWND parent) {
    WCHAR f[MAX_PATH] = L"";
    OPENFILENAMEW o;
    ZeroMemory(&o, sizeof o);
    o.lStructSize = sizeof o; o.hwndOwner = parent;
    o.lpstrFilter = L"MiniDock 配置 (*.mdock)\0*.mdock\0";
    o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.Flags = OFN_FILEMUSTEXIST;
    if (!GetOpenFileNameW(&o)) return;
    BYTE *b = NULL;
    DWORD n = 0;
    if (!ReadWhole(f, &b, &n) || n < 8 || memcmp(b, "MDK1", 4) != 0) {
        MessageBoxW(parent, L"这不是有效的 MiniDock 配置文件。", L"MiniDock", MB_OK | MB_ICONWARNING);
        if (b) HeapFree(GetProcessHeap(), 0, b);
        return;
    }
    WCHAR imp[MAX_PATH];
    lstrcpyW(imp, g_ini);
    lstrcatW(imp, L".imp");
    DWORD cnt = *(DWORD *)(b + 4), pos = 8;
    for (DWORD i = 0; i < cnt && pos + 4 <= n; i++) {
        DWORD nl = *(DWORD *)(b + pos);
        pos += 4;
        if (nl > 260 || pos + nl * 2 + 4 > n) break;
        WCHAR nm[300];
        memcpy(nm, b + pos, nl * 2);
        nm[nl] = 0;
        pos += nl * 2;
        DWORD sz = *(DWORD *)(b + pos);
        pos += 4;
        if (pos + sz > n) break;
        WCHAR dst[MAX_PATH];
        if (!lstrcmpiW(nm, L"MiniDock.ini")) lstrcpyW(dst, imp);
        else PathCombineW(dst, g_iconDir, PathFindFileNameW(nm));
        HANDLE h = CreateFileW(dst, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(h, b + pos, sz, &w, NULL); CloseHandle(h); }
        pos += sz;
    }
    HeapFree(GetProcessHeap(), 0, b);
    /* merge only the parts the file actually contains */
    CopySection(L"Settings", imp, g_ini);
    if ((int)GetPrivateProfileIntW(L"TrayApps", L"Count", -1, imp) >= 0) {
        WritePrivateProfileStringW(L"TrayApps", NULL, NULL, g_ini);
        CopySection(L"TrayApps", imp, g_ini);
    }
    if ((int)GetPrivateProfileIntW(L"Items", L"Count", -1, imp) >= 0) {
        WritePrivateProfileStringW(L"Items", NULL, NULL, g_ini);
        WritePrivateProfileStringW(L"Groups", NULL, NULL, g_ini);
        CopySection(L"Items", imp, g_ini);
        CopySection(L"Groups", imp, g_ini);
    }
    DeleteFileW(imp);
    ReloadConfigAll();
    MessageBoxW(parent, L"已导入，图标已恢复。", L"MiniDock", MB_OK | MB_ICONINFORMATION);
}

/* ======================= geometry ======================= */
static HMONITOR g_mons[8];
static int g_nmon;
static BOOL CALLBACK MonEnum(HMONITOR m, HDC dc, LPRECT r, LPARAM l) { if (g_nmon < 8) g_mons[g_nmon++] = m; return TRUE; }

static void GetMon(void) {
    g_nmon = 0;
    EnumDisplayMonitors(NULL, NULL, MonEnum, 0);
    for (int i = 0; i < g_nmon; i++) {
        MONITORINFO mi = { sizeof mi };
        GetMonitorInfoW(g_mons[i], &mi);
        if (mi.dwFlags & MONITORINFOF_PRIMARY) { HMONITOR t = g_mons[0]; g_mons[0] = g_mons[i]; g_mons[i] = t; break; }
    }
    g_mi.cbSize = sizeof g_mi;
    if (g_nmon == 0) {
        GetMonitorInfoW(MonitorFromPoint((POINT){ 0, 0 }, MONITOR_DEFAULTTOPRIMARY), &g_mi);
        return;
    }
    GetMonitorInfoW((cfg.monitor < g_nmon) ? g_mons[cfg.monitor] : g_mons[0], &g_mi);
}

static float g_ce, g_reflH, g_accH = 210.f;
enum { W_CLOCK, W_NOTIFY, W_CC, W_RES, W_TOOL, W_MODE, W_TRAY, W_POWER, W_N, W_HUB = W_N };
static int g_wk[W_N], g_wn, g_wWid[W_N];
static float g_wU[W_N];
static RECT g_wScr[W_N];


static void CalcMetrics(void) {
    float k = DpiK();
    float S0 = (float)Pxs(cfg.iconSize);
    float Z0 = (float)Pxs(cfg.zoom ? (cfg.zoomSize > cfg.iconSize ? cfg.zoomSize : cfg.iconSize) : cfg.iconSize);
    float g0 = (float)Pxs(cfg.spacing), pad0 = (float)Pxs(cfg.pad), sep0 = (float)Pxs(cfg.sepGap), padX0 = pad0 + 2 * k;
    int nIcon = 0, nSep = 0;
    for (int i = 0; i < nItems; i++) { if (items[i].type == T_SEP) nSep++; else nIcon += (int)Asp(&items[i]); }
    float wb = 2 * padX0 + nIcon * S0 + nSep * sep0 + (nItems > 1 ? g0 * (nItems - 1) : 0);
    if (nItems == 0) wb = 2 * padX0 + S0 * 4;
    RECT wa = (cfg.taskHide != 0) ? g_mi.rcMonitor : g_mi.rcWork;
    float along = (float)(Horiz() ? wa.right - wa.left : wa.bottom - wa.top);
    float limit = along - 40, fit = 1;
    if (wb > limit) fit = Clampf((limit - 2 * padX0) / (wb - 2 * padX0), 0.25f, 1.f);
    M.S = S0 * fit; M.Z = Z0 * fit; M.g = g0 * fit; M.sepW = sep0 * fit;
    M.pad = pad0; M.padX = (float)Pxs(cfg.hPad);   /* horizontal gap from the bar edges to the first / last icon */
    g_reflH = (cfg.reflect && cfg.edge == E_BOTTOM) ? M.S * cfg.reflSize / 100.f + Pxs(cfg.reflGap) + 2 : 0;
    g_reflIn = (cfg.reflMode == 1 && g_reflH > 0);   /* reflection inside the bar: the bar grows instead of the margin */
    M.margin = (float)Pxs(cfg.margin) + (g_reflIn ? 0 : g_reflH);
    M.barH = M.S + 2 * M.pad + 4 + (g_reflIn ? g_reflH : 0);
    g_clockW = 0;
    if (cfg.clock) {
        BOOL hasT = cfg.clockShow != 2, hasD = cfg.clockShow != 1;
        if (Horiz()) {
            int wt = hasT ? Sc(cfg.clockSec ? 92 : 74) : 0, wd = 0;
            if (hasD) wd = cfg.dateFmt == 0 ? Sc(cfg.clockWeek ? 166 : 124) : Sc(cfg.clockWeek ? 146 : 110);
            g_clockW = wt > wd ? wt : wd;
        } else {
            int nl = (hasT ? 1 : 0) + (hasD ? (cfg.clockWeek ? 2 : 1) : 0);
            g_clockW = Sc(nl * 21 + 14);
        }
    }
    g_wn = 0;
    {
        int ord[8] = { cfg.wOrder ? W_NOTIFY : W_CLOCK, cfg.wOrder ? W_CLOCK : W_NOTIFY, W_CC, W_RES, W_TOOL, W_MODE, W_TRAY, W_POWER };
        for (int k = 0; k < 8; k++) {
            if (ord[k] == W_CLOCK && cfg.clock) { g_wWid[W_CLOCK] = g_clockW; g_wk[g_wn++] = W_CLOCK; }
            else if (ord[k] == W_NOTIFY && cfg.notifyW) { g_wWid[W_NOTIFY] = (int)(M.barH * 0.92f); g_wk[g_wn++] = W_NOTIFY; }
            else if (ord[k] == W_CC && cfg.ctrlCenter) { g_wWid[W_CC] = (int)(M.barH * ((cfg.wideMask & (1 << W_CC)) ? 1.6f : 0.72f)); g_wk[g_wn++] = W_CC; }
            else if (ord[k] == W_RES && cfg.resMon) { g_wWid[W_RES] = (int)(M.barH * ((cfg.wideMask & (1 << W_RES)) ? 2.2f : 1.16f)); g_wk[g_wn++] = W_RES; }
            else if (ord[k] == W_TOOL && cfg.toolW) { g_wWid[W_TOOL] = (int)(M.barH * ((cfg.wideMask & (1 << W_TOOL)) ? 1.6f : 0.72f)); g_wk[g_wn++] = W_TOOL; }
            else if (ord[k] == W_MODE && cfg.modeW) { g_wWid[W_MODE] = (int)(M.barH * 0.72f); g_wk[g_wn++] = W_MODE; }
            else if (ord[k] == W_TRAY && cfg.trayW) { g_wWid[W_TRAY] = (int)(M.barH * 0.72f); g_wk[g_wn++] = W_TRAY; }
            else if (ord[k] == W_POWER && cfg.powerW) { g_wWid[W_POWER] = (int)(M.barH * 0.72f); g_wk[g_wn++] = W_POWER; }
        }
    }
    if (cfg.statusBar) g_wn = 0;   /* the top status bar replaces the dock widgets */
    g_ce = 0;
    if (!Horiz() && g_wn > 0) {
        for (int k = 0; k < g_wn; k++) g_wWid[g_wk[k]] = (int)M.barH;
        g_ce = M.barH + M.g + 6; /* side docks expose one compact 1x1 widget launcher */
    } else {
        for (int k = 0; k < g_wn; k++) g_ce += (float)g_wWid[g_wk[k]] + M.g + 6;
    }
    float over = (M.Z - M.S - M.pad > 0 ? M.Z - M.S - M.pad : 0) + M.S * 0.55f + 34 + 8;
    if (!Horiz()) over += 170;
    M.overhang = over;
    float w = wb * fit + (M.Z - M.S) * 6 + M.S + 260 + g_ce;
    if (nItems == 0) w = wb + 260;
    if (w > along) w = along;
    M.lw = (int)w;
    M.lh = (int)(M.barH + over + M.margin) + 2;
}

static void FreeSurface(void) {
    if (g_g) { GdipDeleteGraphics(g_g); g_g = NULL; }
    if (g_surf) { GdipDisposeImage(g_surf); g_surf = NULL; }
    if (g_dib) { SelectObject(g_memdc, g_oldbmp); DeleteObject(g_dib); g_dib = NULL; }
    g_bits = NULL;
}

static void MakeSurface(int w, int h) {
    FreeSurface();
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof bi);
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h; bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
    g_dib = CreateDIBSection(g_memdc, &bi, DIB_RGB_COLORS, (void **)&g_bits, NULL, 0);
    if (!g_dib) return;
    g_oldbmp = (HBITMAP)SelectObject(g_memdc, g_dib);
    GdipCreateBitmapFromScan0(w, h, w * 4, PXF_PARGB, g_bits, &g_surf);
    GdipGetImageGraphicsContext(g_surf, &g_g);
    SetupG(g_g);
}

/* ---- app bar: reserve desktop space so maximized windows avoid the dock ---- */
static BOOL g_abReg;
static RECT g_abRect;
static DWORD g_abKey;

static void SetupAppBar(BOOL on, int thick) {
    APPBARDATA ab;
    ZeroMemory(&ab, sizeof ab);
    ab.cbSize = sizeof ab;
    ab.hWnd = g_hwnd;
    if (!on) {
        if (g_abReg) { SHAppBarMessage(ABM_REMOVE, &ab); g_abReg = FALSE; g_abKey = 0; }
        return;
    }
    RECT m = g_mi.rcMonitor;
    DWORD key = (DWORD)(cfg.edge * 100003 + thick * 31 + m.left * 7 + m.top * 13 + m.right * 17 + m.bottom * 19) | 1;
    if (g_abReg && key == g_abKey) return;
    if (!g_abReg) {
        ab.uCallbackMessage = WM_APP_AB;
        if (!SHAppBarMessage(ABM_NEW, &ab)) { g_abKey = 0; return; }   /* not registered yet: retry next geometry tick */
        g_abReg = TRUE;
    }
    RECT rc = m;
    switch (cfg.edge) {
    case E_TOP: ab.uEdge = ABE_TOP; rc.bottom = rc.top + thick; break;
    case E_LEFT: ab.uEdge = ABE_LEFT; rc.right = rc.left + thick; break;
    case E_RIGHT: ab.uEdge = ABE_RIGHT; rc.left = rc.right - thick; break;
    default: ab.uEdge = ABE_BOTTOM; rc.top = rc.bottom - thick; break;
    }
    ab.rc = rc;
    SHAppBarMessage(ABM_QUERYPOS, &ab);
    /* Anchor on the queried edge (so the strip sits above a bottom taskbar) but clamp it back inside the
       monitor: without a taskbar ABM_QUERYPOS returns the screen edge itself, which used to put the rect
       off-screen and make ABM_SETPOS fail - that is why "maximised windows avoid the dock" stopped working. */
    switch (cfg.edge) {
    case E_TOP:
        if (ab.rc.top < m.top) ab.rc.top = m.top;
        ab.rc.bottom = ab.rc.top + thick; break;
    case E_LEFT:
        if (ab.rc.left < m.left) ab.rc.left = m.left;
        ab.rc.right = ab.rc.left + thick; break;
    case E_RIGHT:
        if (ab.rc.right > m.right) ab.rc.right = m.right;
        ab.rc.left = ab.rc.right - thick; break;
    default:
        if (ab.rc.top > m.bottom - thick) ab.rc.top = m.bottom - thick;
        ab.rc.bottom = ab.rc.top + thick; break;
    }
    LRESULT sres = SHAppBarMessage(ABM_SETPOS, &ab);
    g_abRect = ab.rc;
    g_abKey = sres ? key : 0;   /* a failed SETPOS must be retried, not cached as done */
}

static void GrpBarSync(void);
static RECT g_fbBase;
static BOOL g_fbSaved, g_fbOn;
static void ApplyDockReserveFallback(BOOL on) {
    if (!on) {
        if (g_fbOn && g_fbSaved) { SystemParametersInfoW(SPI_SETWORKAREA, 0, &g_fbBase, 0); g_fbOn = FALSE; }
        return;
    }
    if (!g_fbSaved) { SystemParametersInfoW(SPI_GETWORKAREA, 0, &g_fbBase, 0); g_fbSaved = TRUE; }
    RECT wa = g_fbBase;
    int th = (int)(M.barH + M.margin + 4);
    switch (cfg.edge) {
    case E_TOP: wa.top += th; break;
    case E_LEFT: wa.left += th; break;
    case E_RIGHT: wa.right -= th; break;
    default: wa.bottom -= th; break;
    }
    SystemParametersInfoW(SPI_SETWORKAREA, 0, &wa, 0);
    g_fbOn = TRUE;
}
static RECT g_waBase, g_waCur;
static BOOL g_waBaseSaved, g_waSet;
/* Reserve the dock strip by shrinking the system work area. Driven only by the user's settings, so it
   works the same whether the taskbar is visible, auto-hidden or permanently hidden, and it does not
   depend on the Shell accepting our AppBar (it can answer ABM_NEW/SETPOS with 0). */
static void ApplyWorkArea(void) {
    BOOL want = (cfg.taskHide == 2) && cfg.reserve && cfg.mode == 0 && !gFsTgt && !g_fsKind;

    if (!want) {
        if (g_waSet && g_waBaseSaved) { SystemParametersInfoW(SPI_SETWORKAREA, 0, &g_waBase, 0); g_waSet = FALSE; }
        return;
    }
    if (!g_waBaseSaved) { SystemParametersInfoW(SPI_GETWORKAREA, 0, &g_waBase, 0); g_waBaseSaved = TRUE; }
    RECT wa = (cfg.taskHide == 0) ? g_waBase : g_mi.rcMonitor;
    int th = (int)(M.barH + M.margin + 4);
    if (th > 0) {
        switch (cfg.edge) {
        case E_TOP: wa.top += th; break;
        case E_LEFT: wa.left += th; break;
        case E_RIGHT: wa.right -= th; break;
        default: wa.bottom -= th; break;
        }
    }
    /* read the live work area: Explorer resets it after appbar / taskbar changes, so a cached
       "already applied" flag would silently stop re-applying the reservation */
    RECT cur;
    if (!SystemParametersInfoW(SPI_GETWORKAREA, 0, &cur, 0)) return;
    if (memcmp(&cur, &wa, sizeof wa) != 0) {
        if (SystemParametersInfoW(SPI_SETWORKAREA, 0, &wa, 0)) { g_waCur = wa; g_waSet = TRUE; }
    }
}static void RestoreWorkArea(void) {
    if (g_waSet && g_waBaseSaved) { SystemParametersInfoW(SPI_SETWORKAREA, 0, &g_waBase, 0); g_waSet = FALSE; }
}
/* Does the shell actually reserve the strip the dock asked for? AppBar registrations can be lost (Explorer
   restart, a stale ABM_NEW, a fullscreen episode). When that happens the maximised window overlaps the
   dock even though cfg.reserve is on, so the scan timer re-registers from scratch. */
static BOOL AppBarSpaceReserved(void) {
    if (!cfg.reserve || cfg.mode != 0) return TRUE;
    MONITORINFO mi = { sizeof mi };
    if (!GetMonitorInfoW(MonitorFromRect(&g_mi.rcMonitor, MONITOR_DEFAULTTONEAREST), &mi)) return TRUE;
    int thick = (int)(M.barH + M.margin + 4);
    if (thick < 8) return TRUE;
    switch (cfg.edge) {
    case E_TOP: return mi.rcWork.top >= mi.rcMonitor.top + thick - 2;
    case E_LEFT: return mi.rcWork.left >= mi.rcMonitor.left + thick - 2;
    case E_RIGHT: return mi.rcWork.right <= mi.rcMonitor.right - thick + 2;
    default: return mi.rcWork.bottom <= mi.rcMonitor.bottom - thick + 2;
    }
}

static void UpdateGeometry(void) {
    GetMon();
    SyncDockDpi();
    CalcMetrics();
    ApplyWorkArea();
    LW = M.lw; LH = M.lh;
    int ww = Horiz() ? LW : LH, hh = Horiz() ? LH : LW;
    if (ww != winW || hh != winH || !g_dib) { winW = ww; winH = hh; MakeSurface(winW, winH); }
    RECT wa = (cfg.taskHide != 0) ? g_mi.rcMonitor : g_mi.rcWork;
    /* never reserve desktop space while a fullscreen app owns the screen: doing that would resize the
       game / video window when the dock is summoned with the hotkey */
    /* With an auto-hidden/hidden taskbar, reserve the physical monitor edge instead of
       letting Explorer's taskbar appbar leave a taskbar-height gap above the Dock. */
    BOOL res = cfg.reserve && cfg.mode == 0 && g_hwnd && !gFsTgt && !g_fsKind && cfg.taskHide != 2;
    if (g_hwnd) SetupAppBar(res, (int)(M.barH + M.margin + 4));
    /* If the app bar registration was refused there is no reserved rectangle yet. Positioning the dock
       against the zeroed g_abRect would push it off-screen (winY = -height); use the work area instead
       and let the retry in the scan timer register it again. */
    if (res && (!g_abReg || !g_abKey)) {   /* Shell refused the AppBar: reserve through the work area instead */
        ApplyDockReserveFallback(TRUE);
        res = FALSE;
    } else {
        ApplyDockReserveFallback(FALSE);
    }

    switch (cfg.edge) {
    case E_TOP:
        winX = wa.left + ((wa.right - wa.left) - winW) / 2; winY = res ? g_abRect.top : wa.top; break;
    case E_LEFT:
        winX = res ? g_abRect.left : wa.left; winY = wa.top + ((wa.bottom - wa.top) - winH) / 2; break;
    case E_RIGHT:
        winX = (res ? g_abRect.right : wa.right) - winW; winY = wa.top + ((wa.bottom - wa.top) - winH) / 2; break;
    default:
        winX = wa.left + ((wa.right - wa.left) - winW) / 2; winY = (res ? g_abRect.bottom : wa.bottom) - winH; break;
    }
    ApplyWorkArea();   /* AppBar SETPOS makes the shell recompute the work area: re-apply the gap fix */
    if (cfg.grpFloat) GrpBarSync();   /* the floating strip lives outside the dock window: move it along */
}

/* ======================= frosted background ======================= */
static void BlurPass(BYTE *src, BYTE *dst, int w, int h, int r, BOOL horiz) {
    int len = horiz ? w : h, cnt = horiz ? h : w;
    int stepIn = horiz ? 4 : w * 4, stepLine = horiz ? w * 4 : 4;
    int win = 2 * r + 1;
    for (int line = 0; line < cnt; line++)
        for (int c = 0; c < 3; c++) {
            BYTE *s = src + (INT_PTR)line * stepLine + c, *d = dst + (INT_PTR)line * stepLine + c;
            int sum = 0;
            for (int k = -r; k <= r; k++) { int ix = k < 0 ? 0 : (k >= len ? len - 1 : k); sum += s[ix * stepIn]; }
            for (int i = 0; i < len; i++) {
                d[i * stepIn] = (BYTE)(sum / win);
                int a = i + r + 1, b = i - r;
                if (a >= len) a = len - 1;
                if (b < 0) b = 0;
                sum += s[a * stepIn] - s[b * stepIn];
            }
        }
}

static BOOL UpdateFrost(void) {
    if (cfg.bgMode != 2 || !g_dib || gFsHidden || !g_fdc) return FALSE;
    int sc = 4, w = winW / sc, h = winH / sc;
    if (w < 8) w = 8;
    if (h < 8) h = 8;
    if (w != g_fw || h != g_fh || !g_fbmp) {
        if (g_frost) { GdipDisposeImage(g_frost); g_frost = NULL; }
        if (g_fbmp) { SelectObject(g_fdc, g_fold); DeleteObject(g_fbmp); g_fbmp = NULL; }
        BITMAPINFO bi;
        ZeroMemory(&bi, sizeof bi);
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h; bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
        g_fbmp = CreateDIBSection(g_fdc, &bi, DIB_RGB_COLORS, (void **)&g_fbits, NULL, 0);
        if (!g_fbmp) return FALSE;
        g_fold = (HBITMAP)SelectObject(g_fdc, g_fbmp);
        GdipCreateBitmapFromScan0(w, h, w * 4, PXF_PARGB, g_fbits, &g_frost);
        g_fw = w; g_fh = h;
    }
    HDC scr = GetDC(NULL);
    /* the dock must not end up inside its own frosted glass: keep it out of the capture, but only for
       the moment we grab (the user's own screenshots / recordings are unaffected) */
    SetWindowDisplayAffinity(g_hwnd, 0x00000011 /* WDA_EXCLUDEFROMCAPTURE */);
    SetStretchBltMode(g_fdc, HALFTONE);
    SetBrushOrgEx(g_fdc, 0, 0, NULL);
    StretchBlt(g_fdc, 0, 0, w, h, scr, winX, winY, winW, winH, SRCCOPY);
    SetWindowDisplayAffinity(g_hwnd, 0 /* WDA_NONE */);
    ReleaseDC(NULL, scr);
    GdiFlush();
    BYTE *tmp = (BYTE *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)w * h * 4);
    if (tmp) {
        int r = cfg.blur < 1 ? 1 : cfg.blur;
        for (int p = 0; p < 2; p++) {
            BlurPass(g_fbits, tmp, w, h, r, TRUE);
            BlurPass(tmp, g_fbits, w, h, r, FALSE);
        }
        HeapFree(GetProcessHeap(), 0, tmp);
    }
    DWORD hs = 2166136261u;
    {   /* true frosted glass: blurred backdrop + a light neutral tint, no glow */
        int tr = g_dark ? 26 : 248, tg = g_dark ? 27 : 249, tb = g_dark ? 32 : 252;
        int tt = (g_dark ? 105 : 140) * (cfg.opacity > 100 ? 100 : cfg.opacity) / 100;   /* opacity = how milky the glass is, the blur itself always stays */
        for (int i = 0; i < w * h; i++) {
            BYTE *q = g_fbits + i * 4;
            q[0] = (BYTE)((q[0] * (255 - tt) + tb * tt) / 255);
            q[1] = (BYTE)((q[1] * (255 - tt) + tg * tt) / 255);
            q[2] = (BYTE)((q[2] * (255 - tt) + tr * tt) / 255);
            q[3] = 255;
            hs = (hs ^ (q[0] >> 2)) * 16777619u;
        }
    }
    BOOL ch = hs != g_fhash;
    g_fhash = hs;
    return ch;
}

/* ======================= running apps ======================= */
static WCHAR g_scanEx[48][MAX_PATH];
typedef struct { HWND h; WCHAR exe[MAX_PATH]; WCHAR title[96]; } WinRec;
typedef struct { BOOL seen[MAXI]; int nex, nwin; WinRec win[24]; HWND appwin[48]; } ScanCtx;
static int g_trashFull = -1;

static BOOL ExeOfWindow(HWND h, WCHAR *out) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid) return FALSE;
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return FALSE;
    DWORD n = MAX_PATH;
    BOOL ok = QueryFullProcessImageNameW(p, 0, out, &n);
    CloseHandle(p);
    return ok;
}

static BOOL IsAppWindow(HWND h) {
    if (!IsWindowVisible(h)) return FALSE;
    if (GetWindow(h, GW_OWNER)) return FALSE;
    if (GetWindowLongW(h, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return FALSE;
    if (GetWindowTextLengthW(h) == 0) return FALSE;
    int cloaked = 0;
    DwmGetWindowAttribute(h, 14, &cloaked, sizeof cloaked);
    if (cloaked) return FALSE;
    WCHAR cls[64];
    GetClassNameW(h, cls, 64);
    if (!lstrcmpW(cls, L"Progman") || !lstrcmpW(cls, L"WorkerW") || !lstrcmpW(cls, L"Shell_TrayWnd")) return FALSE;
    return TRUE;
}

/* A user-selected main window stays addressable after its app hides it in the tray.
   The process handle guards against PID reuse. Class and title distinguish hidden
   login/helper windows when the dock starts again or the main HWND is recreated. */
static void ClearTrayApps(void) {
    for (int i = 0; i < g_ntrayApps; i++) if (g_trayApps[i].process) CloseHandle(g_trayApps[i].process);
    ZeroMemory(g_trayApps, sizeof g_trayApps);
    g_ntrayApps = 0;
}

static int TrayAppIndex(const WCHAR *exe) {
    for (int i = 0; i < g_ntrayApps; i++) if (!lstrcmpiW(g_trayApps[i].exe, exe)) return i;
    return -1;
}

static void LoadTrayApps(void) {
    ClearTrayApps();
    int count = (int)GetPrivateProfileIntW(L"TrayApps", L"Count", 0, g_ini);
    if (count < 0) count = 0;
    if (count > MAXTRAYAPPS) count = MAXTRAYAPPS;
    for (int i = 0; i < count; i++) {
        TrayApp a;
        WCHAR key[32], value[32];
        ZeroMemory(&a, sizeof a);
        wsprintfW(key, L"Path%d", i); GetPrivateProfileStringW(L"TrayApps", key, L"", a.exe, MAX_PATH, g_ini);
        wsprintfW(key, L"Class%d", i); GetPrivateProfileStringW(L"TrayApps", key, L"", a.cls, 128, g_ini);
        wsprintfW(key, L"Title%d", i); GetPrivateProfileStringW(L"TrayApps", key, L"", a.title, 256, g_ini);
        wsprintfW(key, L"Name%d", i); GetPrivateProfileStringW(L"TrayApps", key, L"", a.name, 128, g_ini);
        wsprintfW(key, L"Window%d", i); GetPrivateProfileStringW(L"TrayApps", key, L"0", value, 32, g_ini);
        a.hwnd = (HWND)(UINT_PTR)wcstoull(value, NULL, 10);
        wsprintfW(key, L"Pid%d", i); a.pid = (DWORD)GetPrivateProfileIntW(L"TrayApps", key, 0, g_ini);
        wsprintfW(key, L"Start%d", i); GetPrivateProfileStringW(L"TrayApps", key, L"0", value, 32, g_ini);
        a.processStart = wcstoull(value, NULL, 10);
        wsprintfW(key, L"Style%d", i); GetPrivateProfileStringW(L"TrayApps", key, L"", value, 32, g_ini);
        if (value[0]) { a.roleKnown = TRUE; a.roleStyle = (DWORD)wcstoull(value, NULL, 10); }
        wsprintfW(key, L"ExStyle%d", i); a.roleExStyle = (DWORD)GetPrivateProfileIntW(L"TrayApps", key, 0, g_ini);
        if (!a.exe[0] || !a.cls[0] || TrayAppIndex(a.exe) >= 0) continue;
        if (!a.name[0]) lstrcpynW(a.name, PathFindFileNameW(a.exe), 128);
        g_trayApps[g_ntrayApps++] = a;
    }
}

static void TrayWindowTitle(HWND h, WCHAR *title) {
    GetWindowTextW(h, title, 256);
    for (WCHAR *p = title; *p; p++) if (*p == L'\r' || *p == L'\n') *p = L' ';
}

static BOOL TrayProcessAlive(const TrayApp *a) {
    return a->process && WaitForSingleObject(a->process, 0) == WAIT_TIMEOUT;
}

static ULONGLONG TrayProcessStart(HANDLE process) {
    FILETIME birth, exitTime, kernel, user;
    if (!GetProcessTimes(process, &birth, &exitTime, &kernel, &user)) return 0;
    return ((ULONGLONG)birth.dwHighDateTime << 32) | birth.dwLowDateTime;
}

#define TRAY_ROLE_STYLE (WS_CAPTION | WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)
#define TRAY_ROLE_EXSTYLE (WS_EX_APPWINDOW | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)

static BOOL RestoreTrayIdentity(TrayApp *app) {
    if (!app->hwnd || !app->pid || !app->processStart || !IsWindow(app->hwnd)) return FALSE;
    DWORD pid = 0; GetWindowThreadProcessId(app->hwnd, &pid);
    WCHAR cls[128], exe[MAX_PATH]; GetClassNameW(app->hwnd, cls, 128);
    if (pid != app->pid || lstrcmpW(cls, app->cls) ||
        !ExeOfWindow(app->hwnd, exe) || lstrcmpiW(exe, app->exe)) return FALSE;
    HANDLE process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return FALSE;
    if (WaitForSingleObject(process, 0) != WAIT_TIMEOUT || TrayProcessStart(process) != app->processStart) {
        CloseHandle(process); return FALSE;
    }
    app->process = process;
    TrayWindowTitle(app->hwnd, app->title);
    return TRUE;
}

static BOOL TrayWindowValid(const TrayApp *a) {
    if (!a->hwnd || !a->process || !IsWindow(a->hwnd)) return FALSE;
    DWORD pid = 0;
    GetWindowThreadProcessId(a->hwnd, &pid);
    WCHAR cls[128];
    GetClassNameW(a->hwnd, cls, 128);
    /* The retained live process handle already identifies the executable. */
    return pid == a->pid && !lstrcmpW(cls, a->cls);
}

static BOOL BindTrayWindow(TrayApp *app, HWND window) {
    DWORD pid = 0;
    if (!IsWindow(window) || !GetWindowThreadProcessId(window, &pid)) return FALSE;
    if (app->process && pid != app->pid) return FALSE;
    if (!app->process) app->process = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!app->process) return FALSE;
    app->pid = pid; app->hwnd = window;
    app->processStart = TrayProcessStart(app->process);
    app->roleStyle = (DWORD)GetWindowLongW(window, GWL_STYLE) & TRAY_ROLE_STYLE;
    app->roleExStyle = (DWORD)GetWindowLongW(window, GWL_EXSTYLE) & TRAY_ROLE_EXSTYLE;
    app->roleKnown = TRUE;
    TrayWindowTitle(window, app->title);
    ScheduleSave();
    return TRUE;
}

static void FindTrayWindow(TrayApp *app) {
    /* Titles are mutable display text (tracks, documents, unread counts). Prefer the
       saved title only as a hint after stable HWND/process identity is unavailable. */
    HWND fallback = NULL;
    HWND exactWindow = NULL;
    BOOL ambiguous = FALSE, exactAmbiguous = FALSE;
    HWND window = NULL;
    while ((window = FindWindowExW(NULL, window, app->cls, NULL)) != NULL) {
        WCHAR title[256], exe[MAX_PATH];
        TrayWindowTitle(window, title);
        BOOL exact = app->title[0] && !lstrcmpW(title, app->title);
        if (!ExeOfWindow(window, exe) || lstrcmpiW(exe, app->exe)) continue;
        DWORD pid = 0;
        GetWindowThreadProcessId(window, &pid);
        if (app->process && pid != app->pid) continue;
        if (GetWindow(window, GW_OWNER) || (GetWindowLongW(window, GWL_STYLE) & WS_CHILD)) continue;
        if (app->roleKnown &&
            (((DWORD)GetWindowLongW(window, GWL_STYLE) & TRAY_ROLE_STYLE) != app->roleStyle ||
             ((DWORD)GetWindowLongW(window, GWL_EXSTYLE) & TRAY_ROLE_EXSTYLE) != app->roleExStyle)) continue;
        if (exact) {
            if (exactWindow) exactAmbiguous = TRUE;
            else exactWindow = window;
            continue;
        }
        /* Hidden/minimized main windows are valid. Owned, tool, child, and untitled
           placeholders are not; never guess if two plausible hosts remain. */
        if (!title[0] || GetWindow(window, GW_OWNER) ||
            (GetWindowLongW(window, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) ||
            (GetWindowLongW(window, GWL_STYLE) & WS_CHILD)) continue;
        if (fallback) ambiguous = TRUE;
        else fallback = window;
    }
    if (exactWindow) { if (!exactAmbiguous) BindTrayWindow(app, exactWindow); }
    else if (fallback && !ambiguous) BindTrayWindow(app, fallback);
}

static void RefreshTrayApps(void) {
    for (int i = 0; i < g_ntrayApps; i++) {
        TrayApp *app = &g_trayApps[i];
        if (!app->process) RestoreTrayIdentity(app);
        if (!TrayProcessAlive(app)) {
            if (app->process) CloseHandle(app->process);
            app->process = NULL; app->pid = 0; app->hwnd = NULL; app->processStart = 0;
        }
        if (!TrayWindowValid(app)) { app->hwnd = NULL; FindTrayWindow(app); }
        else if (IsWindowVisible(app->hwnd)) TrayWindowTitle(app->hwnd, app->title);
    }
}

static void RemoveTrayApp(int index) {
    if (index < 0 || index >= g_ntrayApps) return;
    if (g_trayApps[index].process) CloseHandle(g_trayApps[index].process);
    memmove(&g_trayApps[index], &g_trayApps[index + 1], sizeof(TrayApp) * (g_ntrayApps - index - 1));
    ZeroMemory(&g_trayApps[--g_ntrayApps], sizeof(TrayApp));
    SyncTrayApps();
    ScheduleSave();
}

static void ActivateTrayApp(const Item *it) {
    int index = TrayAppIndex(it->path);
    if (index < 0) return;
    RefreshTrayApps();
    TrayApp *a = &g_trayApps[index];
    if (!TrayProcessAlive(a)) { SetTimer(g_hwnd, T_SCAN, 50, NULL); return; }
    if (!TrayWindowValid(a)) {
        MessageBoxW(g_hwnd, L"应用仍在运行，但暂时无法唯一识别主窗口。请先从系统托盘打开主窗口；如果应用有多个同类窗口，可在设置中重新选择。", L"托盘应用常驻", MB_OK | MB_ICONINFORMATION);
        return;
    }
    if (IsWindowVisible(a->hwnd)) {
        ActivateDockWindow(a->hwnd, (int)(it - items));
        return;
    }
    ShowWindowAsync(a->hwnd, IsIconic(a->hwnd) ? SW_RESTORE : SW_SHOW);
    SetForegroundWindow(a->hwnd);
}

/* ---- which pinned item does a window belong to? ----
   exe path > UWP app id > same file name (not for generic names) > same install folder > launched by the item's exe (parent chain) */
static struct { DWORD pid, ppid; WCHAR name[36]; } g_ps[1400];
static int g_nps;
static DWORD g_psTick;
static void ProcSnap(void) {
    if (g_nps && GetTickCount() - g_psTick < 1500) return;
    g_nps = 0; g_psTick = GetTickCount();
    HANDLE sn = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (sn == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof pe;
    if (Process32FirstW(sn, &pe)) do {
        if (g_nps >= 1400) break;
        g_ps[g_nps].pid = pe.th32ProcessID; g_ps[g_nps].ppid = pe.th32ParentProcessID;
        lstrcpynW(g_ps[g_nps].name, pe.szExeFile, 36);
        g_nps++;
    } while (Process32NextW(sn, &pe));
    CloseHandle(sn);
}

static BOOL GenericName(const WCHAR *n) {
    static const WCHAR *g[] = { L"explorer.exe", L"cmd.exe", L"powershell.exe", L"conhost.exe", L"javaw.exe", L"java.exe", L"python.exe", L"pythonw.exe",
                                L"wscript.exe", L"cscript.exe", L"rundll32.exe", L"mmc.exe", L"msiexec.exe", L"svchost.exe", L"dllhost.exe", L"wt.exe", L"WindowsTerminal.exe" };
    for (int i = 0; i < (int)(sizeof g / sizeof g[0]); i++) if (!lstrcmpiW(n, g[i])) return TRUE;
    return FALSE;
}

typedef struct { WCHAR exe[MAX_PATH]; WCHAR aumid[200]; DWORD pid; BOOL host; } WinId;
static const PROPERTYKEY k_aumid = { { 0x9F4C2855, 0x9F79, 0x4B39, { 0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3 } }, 5 };
static void WinAumid(HWND h, WCHAR *out, int n) {
    out[0] = 0;
    IPropertyStore *ps = NULL;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(h, &IID_IPropertyStore, (void **)&ps)) && ps) {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        if (SUCCEEDED(IPropertyStore_GetValue(ps, &k_aumid, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal) lstrcpynW(out, pv.pwszVal, n);
        PropVariantClear(&pv);
        IPropertyStore_Release(ps);
    }
}
static void WinIdOf(HWND h, WinId *w) {
    ZeroMemory(w, sizeof *w);
    GetWindowThreadProcessId(h, &w->pid);
    if (!ExeOfWindow(h, w->exe)) return;
    if (!lstrcmpiW(PathFindFileNameW(w->exe), L"ApplicationFrameHost.exe")) {   /* UWP window: Settings, Calculator ... */
        w->host = TRUE;
        WinAumid(h, w->aumid, 200);
        HWND cw = FindWindowExW(h, NULL, L"Windows.UI.Core.CoreWindow", NULL);
        DWORD p2 = 0;
        if (cw) GetWindowThreadProcessId(cw, &p2);
        if (p2) {
            HANDLE ph = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, p2);
            if (ph) { DWORD n = MAX_PATH; WCHAR t[MAX_PATH]; if (QueryFullProcessImageNameW(ph, 0, t, &n)) { lstrcpyW(w->exe, t); w->pid = p2; } CloseHandle(ph); }
        }
    }
}

/* "same install folder" is a good hint for a launcher (Kingdee K3 and other old suites), but Office and
   other suites put Word / Excel / PowerPoint in *one* folder: there an exe in that folder belongs to its
   own icon only, never to its neighbours. That was the "Word is open, so PowerPoint also looks open" bug. */
static BOOL OfficeExeName(const WCHAR *n) {
    return !lstrcmpiW(n, L"WINWORD.EXE") || !lstrcmpiW(n, L"EXCEL.EXE") || !lstrcmpiW(n, L"POWERPNT.EXE") ||
           !lstrcmpiW(n, L"OUTLOOK.EXE") || !lstrcmpiW(n, L"ONENOTE.EXE") || !lstrcmpiW(n, L"MSACCESS.EXE") ||
           !lstrcmpiW(n, L"MSPUB.EXE") || !lstrcmpiW(n, L"WINPROJ.EXE") || !lstrcmpiW(n, L"VISIO.EXE");
}
static BOOL TdirAmbiguous(const Item *it) {
    if (!it->tdir[0]) return FALSE;
    if (it->target[0] && OfficeExeName(PathFindFileNameW(it->target))) return TRUE;
    for (int i = 0; i < nItems; i++) {
        if ((const Item *)&items[i] == it || items[i].temp || items[i].type == T_WIN || !items[i].tdir[0]) continue;
        if (lstrcmpiW(items[i].tdir, it->tdir)) continue;
        if (it->target[0] && items[i].target[0] && !lstrcmpiW(it->target, items[i].target)) continue;   /* the very same app */
        return TRUE;   /* two dock icons share this folder: do not decide by folder */
    }
    return FALSE;
}
/* The exact rules: the same executable, the same UWP app id, or the same file name (never for generic
   hosts such as explorer.exe / ApplicationFrameHost.exe). */
static BOOL ItemMatchExact(const Item *it, const WinId *w) {
    if (it->aumid[0]) {
        if (w->aumid[0] && !lstrcmpiW(it->aumid, w->aumid)) return TRUE;
        if (w->host && StrStrIW(it->aumid, L"immersivecontrolpanel") && StrStrIW(w->exe, L"SystemSettings")) return TRUE;
    }
    if (!w->exe[0]) return FALSE;
    if (it->target[0]) {
        /* Match the canonical executable path, not just its basename. Adobe and other suites
           can install several major versions side by side with identical EXE names. */
        if (!lstrcmpiW(it->target, w->exe)) return TRUE;
    }
    return FALSE;
}
/* Is this window the *exact* app of some dock icon (a group child included)? Cached because the scan asks
   this once per icon and per window. */
static BOOL AnyExactMatch(const WinId *w) {
    static DWORD cachedPid, cachedTick;
    static WCHAR cachedExe[MAX_PATH];
    static BOOL cachedVal;
    DWORD now = GetTickCount();
    if (cachedTick && w->pid == cachedPid && now - cachedTick < 400 && !lstrcmpiW(cachedExe, w->exe)) return cachedVal;
    BOOL f = FALSE;
    for (int i = 0; i < nItems && !f; i++) {
        if (items[i].temp || items[i].type == T_WIN) continue;
        if (ItemMatchExact(&items[i], w)) f = TRUE;
        else for (int k = 0; k < items[i].nk && !f; k++) if (ItemMatchExact(&items[i].kids[k], w)) f = TRUE;
    }
    cachedPid = w->pid; cachedTick = now; lstrcpynW(cachedExe, w->exe, MAX_PATH); cachedVal = f;
    return f;
}
static BOOL TargetBaseAmbiguous(const Item *it) {
    if (!it->target[0]) return FALSE;
    const WCHAR *base = PathFindFileNameW(it->target);
    for (int i = 0; i < nItems; i++) {
        const Item *other = &items[i];
        if (other == it || other->temp || other->type == T_WIN || !other->target[0]) continue;
        if (!lstrcmpiW(base, PathFindFileNameW(other->target)) && lstrcmpiW(it->target, other->target)) return TRUE;
        for (int k = 0; k < other->nk; k++) {
            const Item *kid = &other->kids[k];
            if (kid == it || !kid->target[0]) continue;
            if (!lstrcmpiW(base, PathFindFileNameW(kid->target)) && lstrcmpiW(it->target, kid->target)) return TRUE;
        }
    }
    for (int i = 0; i < it->nk; i++) {
        const Item *kid = &it->kids[i];
        if (!kid->target[0]) continue;
        const WCHAR *kb = PathFindFileNameW(kid->target);
        for (int j = 0; j < nItems; j++) {
            const Item *other = &items[j];
            if (other == it || other->temp || other->type == T_WIN) continue;
            if (other->target[0] && !lstrcmpiW(kb, PathFindFileNameW(other->target)) && lstrcmpiW(kid->target, other->target)) return TRUE;
        }
    }
    return FALSE;
}
static BOOL ItemMatch(const Item *it, const WinId *w) {
    if (ItemMatchExact(it, w)) return TRUE;
    /* Nothing below may fire for a window that another icon owns exactly: Office, Adobe and every other
       suite puts its programs in one folder, and two icons sharing a folder must never both look running
       (that was "Word is open, so PowerPoint looks open too"). */
    if (AnyExactMatch(w)) return FALSE;
    if (it->tdir[0] && !TdirAmbiguous(it)) {
        int n = lstrlenW(it->tdir);
        if (StrCmpNIW(w->exe, it->tdir, n) == 0 && w->exe[n] == L'\\') return TRUE;
    }
    if (it->target[0] && w->pid && !TargetBaseAmbiguous(it) && !GenericName(PathFindFileNameW(it->target))) {   /* parent-chain matching is unsafe when versions share an EXE name */
        ProcSnap();
        const WCHAR *tn = PathFindFileNameW(it->target);
        DWORD cur = w->pid;
        for (int d = 0; d < 5; d++) {
            int k = -1;
            for (int i = 0; i < g_nps; i++) if (g_ps[i].pid == cur) { k = i; break; }
            if (k < 0) break;
            DWORD pp = g_ps[k].ppid;
            int pk = -1;
            for (int i = 0; i < g_nps; i++) if (g_ps[i].pid == pp) { pk = i; break; }
            if (pk < 0) break;
            if (!lstrcmpiW(g_ps[pk].name, tn)) return TRUE;
            cur = pp;
        }
    }
    return FALSE;
}
static BOOL ItemMatchAny(const Item *it, const WinId *w) {
    if (ItemMatch(it, w)) return TRUE;
    for (int k = 0; k < it->nk; k++) if (ItemMatch(&it->kids[k], w)) return TRUE;
    return FALSE;
}
static BOOL HasWinKey(const Item *it) {
    return it->type == T_FILE && (it->target[0] || it->aumid[0] || it->tdir[0]);
}

/* ---- "open as app window" web icons: the browser window they open is bound to the icon, not to the browser ---- */
static struct { HWND h; DWORD key; } g_bind[16];
static struct { DWORD key, t0; HWND pre[64]; int npre; } g_pend;
static DWORD ItemKey(const Item *it) { return Fnv(it->path); }
static int BoundKey(HWND h, DWORD *key) {
    for (int i = 0; i < 16; i++) if (g_bind[i].h == h) { if (key) *key = g_bind[i].key; return i; }
    return -1;
}
static void BindAdd(HWND h, DWORD key) {
    if (BoundKey(h, NULL) >= 0) return;
    for (int i = 0; i < 16; i++) if (!g_bind[i].h || !IsWindow(g_bind[i].h)) { g_bind[i].h = h; g_bind[i].key = key; return; }
}
static BOOL IsBrowserExe(const WCHAR *exe) {
    const WCHAR *fn = PathFindFileNameW(exe);
    return !lstrcmpiW(fn, L"msedge.exe") || !lstrcmpiW(fn, L"chrome.exe") || !lstrcmpiW(fn, L"brave.exe") || !lstrcmpiW(fn, L"vivaldi.exe") || !lstrcmpiW(fn, L"opera.exe");
}
/* A normal browser *window* carries the browser name in its title ("YouTube - Google Chrome"); a window
   opened with --app (a real web app window) only shows the page title. This is what keeps the running
   dot of a web app off the browser and off web apps the user never opened. */
static BOOL BrowserTitleSuffix(const WCHAR *t) {
    static const WCHAR *suf[] = { L" - Google Chrome", L" - Microsoft Edge", L" - Mozilla Firefox", L" - Firefox",
                                  L" - Brave", L" - Vivaldi", L" - Opera", L" - Opera GX", L" - Chromium",
                                  L" - Microsoft Edge Beta", L" - Microsoft Edge Dev", L" - 360SE", L" - 360ChromeX" };
    int n = lstrlenW(t);
    for (int i = 0; i < (int)(sizeof suf / sizeof suf[0]); i++) {
        int m = lstrlenW(suf[i]);
        if (n > m && !lstrcmpiW(t + n - m, suf[i])) return TRUE;
    }
    return FALSE;
}
static BOOL WebAppWindow(HWND h, const WCHAR *name) {
    if (!name || lstrlenW(name) < 3) return FALSE;
    WCHAR title[200] = L"";
    GetWindowTextW(h, title, 200);
    if (!title[0]) return FALSE;
    if (BrowserTitleSuffix(title)) return FALSE;   /* a tab of a normal browser window */
    return StrStrIW(title, name) != NULL;
}
static BOOL CALLBACK PendEnum(HWND h, LPARAM lp) {
    int *mode = (int *)lp;   /* 0: remember existing windows, 1: look for a new one */
    WCHAR cls[40];
    GetClassNameW(h, cls, 40);
    if (lstrcmpW(cls, L"Chrome_WidgetWin_1") || !IsWindowVisible(h)) return TRUE;
    WCHAR exe[MAX_PATH];
    if (!ExeOfWindow(h, exe) || !IsBrowserExe(exe)) return TRUE;
    if (*mode == 0) { if (g_pend.npre < 64) g_pend.pre[g_pend.npre++] = h; return TRUE; }
    for (int i = 0; i < g_pend.npre; i++) if (g_pend.pre[i] == h) return TRUE;
    if (BoundKey(h, NULL) >= 0 || !IsAppWindow(h)) return TRUE;
    {   /* only a real web-app window may be bound: never the browser's own window */
        int it = ItemOfKey(g_pend.key);
        if (it < 0 || !items[it].appmode) return TRUE;
        if (!WebAppWindow(h, items[it].name)) return TRUE;
    }
    BindAdd(h, g_pend.key);
    g_pend.key = 0;
    return FALSE;
}
static void BindStart(const Item *it) {
    int mode = 0;
    ZeroMemory(&g_pend, sizeof g_pend);
    g_pend.key = ItemKey(it); g_pend.t0 = GetTickCount();
    EnumWindows(PendEnum, (LPARAM)&mode);
}
static void BindTick(void) {
    for (int i = 0; i < 16; i++) if (g_bind[i].h && !IsWindow(g_bind[i].h)) g_bind[i].h = NULL;
    if (g_pend.key && GetTickCount() - g_pend.t0 < 15000) { int mode = 1; EnumWindows(PendEnum, (LPARAM)&mode); }
    else g_pend.key = 0;
}
static int ItemOfKey(DWORD key) {
    for (int i = 0; i < nItems; i++) if (items[i].type == T_URL && !items[i].temp && ItemKey(&items[i]) == key) return i;
    return -1;
}

static HWND g_fdis[128];
static int g_nfdis;
static void SetTransitionsDisabled(HWND h, BOOL on, BOOL refresh) {
    int i;
    for (i = 0; i < g_nfdis; i++) if (g_fdis[i] == h) break;
    if (on) {
        if (i < g_nfdis && !refresh) return;
        BOOL t = TRUE;
        /* A failed call must remain retryable. Applications may also change their own
           DWM attributes after a scan, so restoration can explicitly refresh this value. */
        if (FAILED(DwmSetWindowAttribute(h, 3, &t, sizeof t))) return;
        if (i < g_nfdis) return;
        if (g_nfdis >= 128) {   /* recycle the oldest entry: foreground windows keep arriving */
            if (IsWindow(g_fdis[0])) { BOOL t0 = FALSE; DwmSetWindowAttribute(g_fdis[0], 3, &t0, sizeof t0); }
            memmove(&g_fdis[0], &g_fdis[1], sizeof(HWND) * 127);
            g_nfdis = 127;
        }
        g_fdis[g_nfdis++] = h;
    } else if (i < g_nfdis) {
        BOOL t = FALSE;
        if (IsWindow(h)) DwmSetWindowAttribute(h, 3, &t, sizeof t);
        g_fdis[i] = g_fdis[--g_nfdis];
    }
}
static void DisableTransitions(HWND h, BOOL on) { SetTransitionsDisabled(h, on, FALSE); }
static void RestoreTransitions(void) { while (g_nfdis > 0) DisableTransitions(g_fdis[0], FALSE); }

/* Office documents and Explorer folders can be shown one icon per window ("separate" display) */
static BOOL IsDocApp(const WCHAR *exe, HWND h) {
    const WCHAR *fn = PathFindFileNameW(exe);
    if (!lstrcmpiW(fn, L"WINWORD.EXE") || !lstrcmpiW(fn, L"EXCEL.EXE") || !lstrcmpiW(fn, L"POWERPNT.EXE")) return TRUE;
    if (!lstrcmpiW(fn, L"explorer.exe")) { WCHAR cls[40]; GetClassNameW(h, cls, 40); return !lstrcmpW(cls, L"CabinetWClass"); }
    return FALSE;
}

static BOOL CALLBACK ScanProc(HWND h, LPARAM lp) {
    ScanCtx *c = (ScanCtx *)lp;
    if (!IsAppWindow(h) || h == g_hwnd || h == g_pv) return TRUE;
    DWORD bk = 0;
    if (BoundKey(h, &bk) >= 0) {   /* web icon opened as an app window: the indicator belongs to that icon */
        int bi = ItemOfKey(bk);
        if (bi >= 0) c->seen[bi] = TRUE;
        if (cfg.minAnim) DisableTransitions(h, TRUE);
        return TRUE;
    }
    WinId wid;
    WinIdOf(h, &wid);
    if (!wid.exe[0]) return TRUE;
    WCHAR *exe = wid.exe;
    BOOL sepWin = cfg.multiMode == 1 && TrayAppIndex(exe) < 0 && IsDocApp(exe, h);
    if (sepWin && c->nwin < 24) {
        WinRec *wr = &c->win[c->nwin++];
        wr->h = h;
        lstrcpynW(wr->exe, exe, MAX_PATH);
        GetWindowTextW(h, wr->title, 96);
        WCHAR *dash = wcsrchr(wr->title, L'-');
        if (dash && dash > wr->title + 1 && dash[1] == L' ' && dash[-1] == L' ') dash[-1] = 0;   /* "Report.docx - Word" -> "Report.docx" */
    }
    BOOL known = FALSE;
    if (cfg.minAnim) DisableTransitions(h, TRUE);
    if (IsBrowserExe(exe)) {   /* an app-window web icon started before the dock: match it by its page title */
        WCHAR title[160];
        GetWindowTextW(h, title, 160);
        for (int i = 0; i < nItems; i++)
            if (items[i].type == T_URL && !items[i].temp && items[i].appmode && WebAppWindow(h, items[i].name)) {
                BindAdd(h, ItemKey(&items[i]));
                c->seen[i] = TRUE;
                return TRUE;
            }
    }
    for (int i = 0; i < nItems; i++) {
        if (items[i].temp || items[i].type == T_WIN) continue;
        if (ItemMatchAny(&items[i], &wid)) {
            c->seen[i] = TRUE;
            if (items[i].type != T_GROUP || !cfg.groupRunning) known = TRUE;
        }
    }
    if (known || sepWin || wid.host || TrayAppIndex(exe) >= 0) return TRUE;
    for (int k = 0; k < c->nex; k++) if (!lstrcmpiW(g_scanEx[k], exe)) {
        HWND foreground = GetForegroundWindow();
        if (foreground == h) c->appwin[k] = h;
        else if (c->appwin[k] != foreground) {
            /* Keep the last bound window when it is minimized behind another window of the same app. */
            for (int i = 0; i < nItems; i++) if (items[i].temp && items[i].type == T_FILE &&
                items[i].hwnd == h && !lstrcmpiW(items[i].target, exe)) { c->appwin[k] = h; break; }
        }
        return TRUE;
    }
    if (c->nex < 48) {
        lstrcpynW(g_scanEx[c->nex], exe, MAX_PATH);
        c->appwin[c->nex++] = h;
    }
    return TRUE;
}

static int TempEnd(void) {   /* running-app icons go before the document-window section */
    for (int i = 0; i < nItems; i++) if (items[i].temp && items[i].type == T_SEP && items[i].appmode >= 9) return i;
    return nItems;
}

static BOOL SyncTemp(ScanCtx *c) {
    BOOL ch = FALSE;
    for (int i = nItems - 1; i >= 0; i--) {
        if (!items[i].temp || items[i].type == T_SEP || items[i].type == T_WIN || items[i].type == T_TRAY) continue;
        BOOL f = FALSE;
        for (int k = 0; k < c->nex; k++) if (!lstrcmpiW(g_scanEx[k], items[i].target)) { f = TRUE; items[i].hwnd = c->appwin[k]; break; }
        if (!f || !cfg.showRunning) { RemoveItem(i); ch = TRUE; }
    }
    if (cfg.showRunning) {
        for (int k = 0; k < c->nex; k++) {
            BOOL have = FALSE;
            for (int i = 0; i < nItems; i++) if (items[i].temp && items[i].type != T_SEP && !lstrcmpiW(items[i].target, g_scanEx[k])) have = TRUE;
            if (have) continue;
            BOOL hasSep = FALSE;
            for (int i = 0; i < nItems; i++) if (items[i].temp && items[i].type == T_SEP && items[i].appmode < 9) hasSep = TRUE;
            if (nItems + 2 > MAXI) break;
            if (!hasSep) { Item s; FillItem(&s, T_SEP, 0, L"", L"", NULL); InsertItemStruct(TempEnd(), &s, TRUE); }
            WCHAR nm[128];
            FileDescr(g_scanEx[k], nm, 128);
            if (!nm[0]) { lstrcpynW(nm, PathFindFileNameW(g_scanEx[k]), 128); PathRemoveExtensionW(nm); }
            Item it;
            FillItem(&it, T_FILE, 0, nm, g_scanEx[k], NULL);
            it.running = TRUE;
            it.hwnd = c->appwin[k];
            InsertItemStruct(TempEnd(), &it, TRUE);
            ch = TRUE;
        }
    }
    int nt = 0, ns = 0;
    for (int i = 0; i < nItems; i++) if (items[i].temp && items[i].type != T_WIN && items[i].type != T_TRAY && !(items[i].type == T_SEP && items[i].appmode >= 9)) { if (items[i].type == T_SEP) ns++; else nt++; }
    if (nt == 0 && ns > 0) {
        for (int i = nItems - 1; i >= 0; i--) if (items[i].temp && items[i].type == T_SEP && items[i].appmode < 9) RemoveItem(i);
        ch = TRUE;
    }
    return ch;
}

static int TraySectionStart(void) {
    for (int i = 0; i < nItems; i++)
        if (items[i].type == T_TRAY || (items[i].temp && items[i].type == T_SEP && items[i].appmode == TRAY_SEP)) return i;
    return nItems;
}

static BOOL SyncTrayApps(void) {
    RefreshTrayApps();
    BOOL changed = FALSE;
    for (int i = nItems - 1; i >= 0; i--) {
        if (items[i].type != T_TRAY) continue;
        int at = TrayAppIndex(items[i].path);
        if (at < 0 || !TrayProcessAlive(&g_trayApps[at])) { RemoveItem(i); changed = TRUE; }
        else items[i].hwnd = g_trayApps[at].hwnd;
    }
    for (int k = 0; k < g_ntrayApps; k++) {
        TrayApp *a = &g_trayApps[k];
        if (!TrayProcessAlive(a)) continue;
        BOOL have = FALSE;
        for (int i = 0; i < nItems; i++) if (items[i].type == T_TRAY && !lstrcmpiW(items[i].path, a->exe)) have = TRUE;
        if (have) continue;
        BOOL divider = TraySectionStart() < nItems;
        if (nItems + (divider ? 1 : 2) > MAXI) break;
        if (!divider) { Item sp; FillItem(&sp, T_SEP, TRAY_SEP, L"", L"", NULL); InsertItemStruct(nItems, &sp, TRUE); }
        Item it;
        FillItem(&it, T_TRAY, 0, a->name, a->exe, NULL);
        lstrcpynW(it.target, a->exe, MAX_PATH);
        it.hwnd = a->hwnd;
        it.running = TRUE;
        InsertItemStruct(nItems, &it, TRUE);
        changed = TRUE;
    }
    BOOL any = FALSE;
    for (int i = 0; i < nItems; i++) if (items[i].type == T_TRAY) any = TRUE;
    if (!any) for (int i = nItems - 1; i >= 0; i--)
        if (items[i].temp && items[i].type == T_SEP && items[i].appmode == TRAY_SEP) { RemoveItem(i); changed = TRUE; }
    return changed;
}

/* separate display: one dock icon per open Word / Excel / PowerPoint document and per Explorer window */
static BOOL SyncWin(ScanCtx *c) {
    BOOL ch = FALSE;
    for (int i = nItems - 1; i >= 0; i--) {
        if (items[i].type != T_WIN) continue;
        int f = -1;
        if (cfg.multiMode == 1) for (int k = 0; k < c->nwin; k++) if (c->win[k].h == items[i].hwnd) f = k;
        if (f < 0) { RemoveItem(i); ch = TRUE; continue; }
        BOOL fg = GetForegroundWindow() == items[i].hwnd && !IsIconic(items[i].hwnd);
        if (lstrcmpW(items[i].name, c->win[f].title) != 0) { lstrcpynW(items[i].name, c->win[f].title, 128); BuildWinIcon(&items[i]); ch = TRUE; }
        else if (fg && GetTickCount() - items[i].snapTick > 4000) { BuildWinIcon(&items[i]); ch = TRUE; }   /* keep the thumbnail of the window being edited fresh */
    }
    {   /* drop the divider when no window icon is left */
        BOOL any = FALSE;
        for (int i = 0; i < nItems; i++) if (items[i].type == T_WIN) any = TRUE;
        if (!any) for (int i = nItems - 1; i >= 0; i--) if (items[i].temp && items[i].type == T_SEP && items[i].appmode == 9) { RemoveItem(i); ch = TRUE; }
    }
    if (cfg.multiMode != 1) return ch;
    for (int k = 0; k < c->nwin; k++) {
        BOOL have = FALSE;
        for (int i = 0; i < nItems; i++) if (items[i].type == T_WIN && items[i].hwnd == c->win[k].h) have = TRUE;
        if (have || nItems + 2 > MAXI) continue;
        BOOL hasSec = FALSE;
        for (int i = 0; i < nItems; i++) if (items[i].temp && items[i].type == T_SEP && items[i].appmode == 9) hasSec = TRUE;
        if (!hasSec) { Item sp; FillItem(&sp, T_SEP, 9, L"", L"", NULL); InsertItemStruct(TraySectionStart(), &sp, TRUE); ch = TRUE; }   /* its own section, behind its own divider */
        int pos = TraySectionStart();
        for (int i = nItems - 1; i >= 0; i--) if (items[i].type == T_WIN && !lstrcmpiW(items[i].target, c->win[k].exe)) { pos = i + 1; break; }   /* same app: keep together */
        Item it;
        FillItem(&it, T_WIN, 0, c->win[k].title, c->win[k].exe, NULL);
        it.hwnd = c->win[k].h;
        it.running = TRUE;
        lstrcpynW(it.target, c->win[k].exe, MAX_PATH);
        BuildWinIcon(&it);
        InsertItemStruct(pos, &it, TRUE);
        ch = TRUE;
    }
    return ch;
}

static void ScanRunning(void) {
    ScanCtx c;
    ZeroMemory(&c, sizeof c);
    BindTick();
    if (cfg.dots || cfg.showRunning || cfg.minAnim || cfg.multiMode) EnumWindows(ScanProc, (LPARAM)&c);
    BOOL ch = FALSE;
    for (int i = 0; i < nItems; i++) {   /* running flags first: the lists below shift item indexes */
        if (items[i].temp) continue;
        if (items[i].running != c.seen[i]) { items[i].running = c.seen[i]; ch = TRUE; }
    }
    if (SyncTemp(&c)) ch = TRUE;
    if (SyncWin(&c)) ch = TRUE;
    if (g_ntrayApps && SyncTrayApps()) ch = TRUE;
    for (int i = 0; i < nItems; i++) if (items[i].temp && !items[i].running) { items[i].running = TRUE; ch = TRUE; }
    for (int i = 0; i < nItems; i++)
        if (items[i].type == T_TRASH) {
            SHQUERYRBINFO q;
            ZeroMemory(&q, sizeof q);
            q.cbSize = sizeof q;
            if (SUCCEEDED(SHQueryRecycleBinW(NULL, &q))) {
                int full = q.i64NumItems > 0;
                if (full != g_trashFull) { g_trashFull = full; LoadItemIcon(&items[i]); ch = TRUE; }
            }
        }
    BOOL oldDark = g_dark;
    RefreshAccent();
    UpdateTheme();
    if (oldDark != g_dark) ch = TRUE;
    if (UpdateFrost()) ch = TRUE;
    if (ch) Kick();
}

typedef struct { const Item *it; DWORD key; BOOL isUrl, close, done; } ActCtx;
static BOOL CALLBACK ActProc(HWND h, LPARAM lp) {
    ActCtx *c = (ActCtx *)lp;
    if (!IsAppWindow(h) || h == g_hwnd) return TRUE;
    if (c->isUrl) { DWORD k; if (BoundKey(h, &k) < 0 || k != c->key) return TRUE; }
    else {
        WinId w; WinIdOf(h, &w);
        /* Runtime icons already identify the actual executable; folder/parent fallback could select a sibling app. */
        if (c->it->temp && c->it->type == T_FILE) { if (!ItemMatchExact(c->it, &w)) return TRUE; }
        else if (!ItemMatchAny(c->it, &w)) return TRUE;
    }
    if (c->close) { PostMessageW(h, WM_CLOSE, 0, 0); return TRUE; }
    if (IsIconic(h)) ShowWindow(h, SW_RESTORE);
    SetForegroundWindow(h);
    c->done = TRUE;
    return FALSE;
}
static BOOL ActivateItem(const Item *it, BOOL close) {   /* bring every window of the item to front / close them */
    ActCtx c = { it, 0, it->type == T_URL, close, FALSE };
    if (c.isUrl) c.key = ItemKey(it);
    EnumWindows(ActProc, (LPARAM)&c);
    return c.done;
}

/* ---- fullscreen detection ----
   SHQueryUserNotificationState is the authoritative answer: Windows reports a full-screen presentation,
   a D3D game or a video there and stays quiet for a merely maximised window, for Alt+Tab and for the task
   switcher. The geometry fallback only accepts a window that covers the *whole monitor*: a maximised
   window lives on the work area and must never make the dock step aside.

   0 = no fullscreen content (dock stays)
   1 = media / browser fullscreen  (dock hides, an intentional rest at the edge may peek it back)
   2 = game or other fullscreen    (dock hides, no edge peek at all: aiming down in a game is not a summon) */
#define QUNS_BUSY_ 2
#define QUNS_D3D_ 3
#define QUNS_PRES_ 4
typedef HRESULT (WINAPI *pQUNS)(int *);
static int ShellQuns(void) {
    static pQUNS fn;
    static BOOL tried;
    if (!tried) {
        tried = TRUE;
        fn = (pQUNS)GetProcAddress(GetModuleHandleW(L"shell32.dll"), "SHQueryUserNotificationState");
    }
    int st = -1;
    if (!fn || FAILED(fn(&st))) return -1;
    return st;
}
static BOOL ShellChromeClass(const WCHAR *cls) {
    static const WCHAR *bad[] = { L"Progman", L"WorkerW", L"Shell_TrayWnd", L"Shell_SecondaryTrayWnd",
        L"MultitaskingViewFrame", L"XamlExplorerHostIslandWindow", L"Windows.UI.Core.CoreWindow",
        L"TaskListThumbnailWnd", L"ForegroundStaging", L"NotifyIconOverflowWindow", L"TopLevelWindowForOverflowXamlIsland",
        L"ApplicationManagerWindow", L"Windows.Internal.Shell.TabProxyWindow", L"Shell_InputSwitchTopLevelWindow",
        L"DPIHelperWindow", L"ScreenSaverWindow", L"LockScreenBackstopFrame", L"TaskSwitcherWnd",
        L"MetroGhostWindow", L"GhostWindow", L"Xaml_WindowedPopupClass", L"Windows.UI.Composition.DesktopWindowContentBridge" };
    for (int i = 0; i < (int)(sizeof bad / sizeof bad[0]); i++) if (!lstrcmpW(cls, bad[i])) return TRUE;
    return FALSE;
}
typedef struct { const Item *it; HWND best; DWORD bestArea; } HiddenCtx;
static BOOL CALLBACK HiddenProc(HWND h, LPARAM lp) {
    HiddenCtx *c = (HiddenCtx *)lp;
    if (!h || h == g_hwnd || h == g_hset || h == g_sb || h == g_lpWnd) return TRUE;
    LONG ex = GetWindowLongW(h, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) return TRUE;
    if (GetWindow(h, GW_OWNER)) return TRUE;
    WCHAR cls[64];
    if (!GetClassNameW(h, cls, 64)) return TRUE;
    if (ShellChromeClass(cls)) return TRUE;
    WCHAR title[256];
    if (GetWindowTextW(h, title, 256) <= 0) return TRUE;
    RECT r;
    if (!GetWindowRect(h, &r)) return TRUE;
    int w = r.right - r.left, hh = r.bottom - r.top;
    if (w < 160 || hh < 100) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (!pid) return TRUE;
    WinId wid;
    ZeroMemory(&wid, sizeof wid);
    wid.pid = pid;
    if (!ExeOfWindow(h, wid.exe)) return TRUE;
    if (!ItemMatchAny(c->it, &wid)) return TRUE;
    DWORD area = (DWORD)w * (DWORD)hh;
    if (area > c->bestArea) { c->bestArea = area; c->best = h; }
    return TRUE;
}
static BOOL RevealHiddenWindow(const Item *it) {
    HiddenCtx c = { it, NULL, 0 };
    EnumWindows(HiddenProc, (LPARAM)&c);
    if (!c.best || !IsWindow(c.best)) return FALSE;
    if (IsIconic(c.best)) ShowWindow(c.best, SW_RESTORE);
    else ShowWindow(c.best, SW_SHOWNORMAL);
    SetForegroundWindow(c.best);
    return TRUE;
}

static BOOL MediaExeName(const WCHAR *fn) {
    static const WCHAR *n[] = { L"vlc.exe", L"mpv.exe", L"mpc-hc.exe", L"mpc-hc64.exe", L"mpc-be.exe", L"mpc-be64.exe",
        L"potplayermini.exe", L"potplayermini64.exe", L"kmplayer.exe", L"gom.exe", L"gomplayer.exe", L"wmplayer.exe",
        L"quicktimeplayer.exe", L"video.ui.exe", L"applicationframehost.exe", L"mplayer.exe", L"5kplayer.exe",
        L"mediaplayer.exe", L"zplayer.exe", L"mpcvr.exe", L"vlc-x64.exe" };
    for (int i = 0; i < (int)(sizeof n / sizeof n[0]); i++) if (!lstrcmpiW(fn, n[i])) return TRUE;
    return FALSE;
}
static BOOL BrowserExeName(const WCHAR *fn) {
    static const WCHAR *n[] = { L"msedge.exe", L"chrome.exe", L"firefox.exe", L"brave.exe", L"vivaldi.exe",
        L"opera.exe", L"opera_gx.exe", L"chromium.exe", L"360se.exe", L"360chromex.exe", L"qqbrowser.exe",
        L"sogouexplorer.exe", L"wechatbrowser.exe" };
    for (int i = 0; i < (int)(sizeof n / sizeof n[0]); i++) if (!lstrcmpiW(fn, n[i])) return TRUE;
    return FALSE;
}
static BOOL RecorderExeName(const WCHAR *fn) {
    static const WCHAR *n[] = { L"obs64.exe", L"obs32.exe", L"obs.exe", L"bandicam.exe", L"bdcam.exe", L"fraps.exe",
        L"action.exe", L"camtasia.exe", L"camtasiastudio.exe", L"screenrecorder.exe", L"screenrec.exe",
        L"screentogif.exe", L"captura.exe", L"dxtory.exe", L"playclaw.exe", L"snagit32.exe", L"amdow.exe",
        L"amddvr.exe", L"nvidia share.exe", L"sharex.exe", L"loilo.exe", L"evcapture.exe", L"recordit.exe",
        L"ocam.exe", L"icecream.exe", L"gamebar.exe" };
    for (int i = 0; i < (int)(sizeof n / sizeof n[0]); i++) if (!lstrcmpiW(fn, n[i])) return TRUE;
    return FALSE;
}
static BOOL CALLBACK RecEnum(HWND h, LPARAM lp) {
    if (!h || !IsWindow(h) || !IsWindowVisible(h) || GetWindow(h, GW_OWNER)) return TRUE;
    WCHAR exe[MAX_PATH];
    if (!ExeOfWindow(h, exe)) return TRUE;
    if (RecorderExeName(PathFindFileNameW(exe))) { *(BOOL *)lp = TRUE; return FALSE; }
    return TRUE;
}
/* A screen recorder being around means "the user is recording": the dock must stay visible even when the
   recorded app is fullscreen. Game Bar is excluded from the process scan because it runs all the time;
   it only counts while it is itself the foreground window. */
static BOOL RecorderActive(HWND fg) {
    static DWORD tick;
    static BOOL active;
    static HWND cachedFg;
    DWORD now = GetTickCount();
    if (fg != cachedFg || !tick || now - tick > 1500) {
        cachedFg = fg; tick = now; active = FALSE;
        WCHAR exe[MAX_PATH] = L"";
        if (fg && ExeOfWindow(fg, exe) && RecorderExeName(PathFindFileNameW(exe))) active = TRUE;
        if (!active) EnumWindows(RecEnum, (LPARAM)&active);
    }
    return active;
}
static int FullscreenKind(void) {
    if (gLpOpen) return 0;
    HWND f = GetForegroundWindow();
    if (!f || f == g_hwnd || f == g_hset || f == g_lpWnd || f == g_sb || f == GetDesktopWindow() || f == GetShellWindow()) return 0;
    if (!IsWindowVisible(f) || IsIconic(f) || GetAncestor(f, GA_ROOT) != f) return 0;
    WCHAR cls[64];
    GetClassNameW(f, cls, 64);
    if (ShellChromeClass(cls)) return 0;       /* shell windows, Alt+Tab, task switchers, capture frames */
    LONG ex = GetWindowLongW(f, GWL_EXSTYLE);
    if (ex & (WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE)) return 0;   /* overlays: recorders, OSD, capture frames */
    RECT r;
    if (FAILED(DwmGetWindowAttribute(f, 9 /* DWMWA_EXTENDED_FRAME_BOUNDS */, &r, sizeof r))) { if (!GetWindowRect(f, &r)) return 0; }
    HMONITOR hm = MonitorFromWindow(f, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof mi };
    if (!GetMonitorInfoW(hm, &mi)) return 0;
    /* only a window on the dock's own monitor can take the dock's screen space */
    if (mi.rcMonitor.left != g_mi.rcMonitor.left || mi.rcMonitor.top != g_mi.rcMonitor.top ||
        mi.rcMonitor.right != g_mi.rcMonitor.right || mi.rcMonitor.bottom != g_mi.rcMonitor.bottom) return 0;
    /* must cover the monitor itself, not just the work area */
    if (r.left > mi.rcMonitor.left + 1 || r.top > mi.rcMonitor.top + 1 ||
        r.right < mi.rcMonitor.right - 1 || r.bottom < mi.rcMonitor.bottom - 1) return 0;
    {   /* a *framed* maximised window is never fullscreen content */
        WINDOWPLACEMENT wp = { sizeof wp };
        LONG stf = GetWindowLongW(f, GWL_STYLE);
        if (GetWindowPlacement(f, &wp) && wp.showCmd == SW_SHOWMAXIMIZED && (stf & (WS_CAPTION | WS_THICKFRAME)))
            return 0;
    }
    if (RecorderActive(f)) return 0;           /* recording / a recorder preview: leave the dock alone */
    WCHAR exe[MAX_PATH] = L"";
    ExeOfWindow(f, exe);
    BOOL media = BrowserExeName(PathFindFileNameW(exe)) || MediaExeName(PathFindFileNameW(exe));
    int q = ShellQuns();
    if (q == QUNS_D3D_) return 2;              /* exclusive D3D game: never peek */
    if (q >= 0 && q != QUNS_BUSY_ && q != QUNS_PRES_) return 0;   /* the shell says nothing is full screen */
    LONG st = GetWindowLongW(f, GWL_STYLE);
    if (st & WS_CAPTION) return 0;             /* a maximised window keeps its caption: never a fullscreen app */
    return media ? 1 : 2;                      /* video / browser may peek, everything else must not */
}
static BOOL IsFullscreenForeground(void) { return FullscreenKind() != 0; }

/* does any normal window cover the dock area? (smart hide) */
typedef struct { RECT bar; BOOL hit; } OvCtx;
static BOOL CALLBACK OvProc(HWND h, LPARAM lp) {
    OvCtx *c = (OvCtx *)lp;
    if (h == g_hwnd || h == g_hset || h == g_pv || !IsWindowVisible(h) || IsIconic(h)) return TRUE;
    if (GetWindowLongW(h, GWL_EXSTYLE) & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return TRUE;
    if (GetWindow(h, GW_OWNER) || GetWindowTextLengthW(h) == 0) return TRUE;
    int cloaked = 0;
    DwmGetWindowAttribute(h, 14, &cloaked, sizeof cloaked);
    if (cloaked) return TRUE;
    WCHAR cls[64];
    GetClassNameW(h, cls, 64);
    if (!lstrcmpW(cls, L"Progman") || !lstrcmpW(cls, L"WorkerW") || !lstrcmpW(cls, L"Shell_TrayWnd")) return TRUE;
    RECT r, o;
    if (!GetWindowRect(h, &r)) return TRUE;
    if (IntersectRect(&o, &r, &c->bar)) { c->hit = TRUE; return FALSE; }
    return TRUE;
}

/* fast fullscreen watch: the dock slides out the moment a fullscreen app takes the screen and slides
   back in as soon as it is gone (no more waiting for the 2.5 s running-app scan) */
#define MAXGAMES 32
static WCHAR g_games[MAXGAMES][80];
static int g_ngames = -1;
static void LoadGames(void) {
    if (g_ngames >= 0) return;
    g_ngames = GetPrivateProfileIntW(L"Games", L"N", 0, g_ini);
    if (g_ngames > MAXGAMES) g_ngames = MAXGAMES;
    for (int i = 0; i < g_ngames; i++) {
        WCHAR k[16] = L"";
        wsprintfW(k, L"G%d", i);
        GetPrivateProfileStringW(L"Games", k, L"", g_games[i], 80, g_ini);
    }
}
static HWND g_lastExternalForeground;
static BOOL GameActive(void) {
    if (!cfg.gameGuard) return FALSE;
    if (cfg.gameAuto && ShellQuns() == QUNS_D3D_) return TRUE;   /* exclusive Direct3D game */
    HWND f = GetForegroundWindow();
    if (!f || f == g_hwnd) return FALSE;
    WCHAR exe[MAX_PATH] = L"";
    ExeOfWindow(f, exe);
    const WCHAR *nm = PathFindFileNameW(exe);
    if (!nm || !nm[0]) return FALSE;
    LoadGames();
    for (int i = 0; i < g_ngames; i++) if (g_games[i][0] && !lstrcmpiW(g_games[i], nm)) return TRUE;
    return FALSE;
}

static void FsWatch(void) {
    if (!g_hwnd) return;
    HWND fgNow = GetForegroundWindow();
    DWORD fgPid = 0; if (fgNow) GetWindowThreadProcessId(fgNow, &fgPid);
    if (fgNow && fgPid != GetCurrentProcessId() && IsWindowVisible(fgNow)) g_lastExternalForeground = fgNow;
    DWORD now = GetTickCount();
    if (now - g_launchTick < 1500) return;   /* grace period after launching: no fullscreen flicker */
    int kind = FullscreenKind();
    if (GameActive()) kind = 2;   /* game mode: never reveal the dock edge over a game */
    BOOL kindChanged = (g_fsKind != kind);
    g_fsKind = kind;
    if (kind) {
        POINT mp;
        GetCursorPos(&mp);
        RECT mr = g_mi.rcMonitor;
        BOOL peek = FALSE;
        if (kind == 1) {   /* media / browser fullscreen only: the pointer must rest at the very edge */
            int band = Sc(3);
            BOOL along = (mp.x >= winX - Sc(4) && mp.x < winX + winW + Sc(4));
            switch (cfg.edge) {
            case E_TOP: peek = mp.y <= mr.top + band && along; break;
            case E_LEFT: peek = mp.x <= mr.left + band; break;
            case E_RIGHT: peek = mp.x >= mr.right - band; break;
            default: peek = mp.y >= mr.bottom - band && along; break;
            }
            if (peek) {   /* 260 ms of rest: a video scrub bar or a lucky aim does not count as a summon */
                if (!gFsEdgeTick) gFsEdgeTick = now;
                if (now - gFsEdgeTick < 260) peek = FALSE;
            } else gFsEdgeTick = 0;
        } else gFsEdgeTick = 0;
        /* Ctrl+Alt+D summoned the dock: keep it out over the fullscreen app so it can actually be used */
        if (gSummonTick && now - gSummonTick < SUMMON_MS) peek = TRUE;
        if (peek || (gFsPeek && (CursorNearDock(mp) || now - gFsNear < 900))) {
            gFsNear = now;
            gFsPeek = TRUE;
            if (gFsTgt) {   /* slide back in for a moment */
                gFsTgt = 0;
                if (gFsHidden) { gFsHidden = FALSE; ShowWindow(g_hwnd, SW_SHOWNA); }
                UpdateGeometry();
                SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
                Kick();
            }
            if (kindChanged) { UpdateGeometry(); Kick(); }
            return;
        }
        if (!gFsTgt) {   /* a fullscreen app took over: step aside completely */
            gFsPeek = FALSE;
            gFsEdgeTick = 0;
            gFsTgt = 1;
            SetupAppBar(FALSE, 0);   /* give the reserved strip back so the fullscreen window can use the whole screen */
            Kick();
        }
        if (kindChanged) { UpdateGeometry(); Kick(); }
        return;
    }
    gFsPeek = FALSE;
    gFsEdgeTick = 0;
    if (gFsTgt) {
        gFsTgt = 0;
        if (gFsHidden) { gFsHidden = FALSE; ShowWindow(g_hwnd, SW_SHOWNA); }
        UpdateGeometry();            /* re-apply the app bar reservation, if the user asked for one */
        Kick();
    }
    /* Leaving a fullscreen video / game must bring the reservation back even when the dock never finished
       sliding out (a peeked dock left gFsTgt at 0). Without this the work area stayed full and maximised
       windows overlapped the dock until some unrelated geometry change happened. */
    if (kindChanged) { UpdateGeometry(); Kick(); }
}

/* ======================= launching ======================= */
typedef struct { int type, appmode; WCHAR path[1024]; WCHAR args[260]; } LaunchJob;

static BOOL BrowserExe(const WCHAR *name, WCHAR *out) {
    WCHAR key[300];
    wsprintfW(key, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\%s", name);
    HKEY roots[2] = { HKEY_LOCAL_MACHINE, HKEY_CURRENT_USER };
    for (int i = 0; i < 2; i++) {
        DWORD n = MAX_PATH * sizeof(WCHAR);
        out[0] = 0;
        if (RegGetValueW(roots[i], key, NULL, RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ, NULL, out, &n) == ERROR_SUCCESS && out[0]) {
            if (out[0] == L'"') { memmove(out, out + 1, lstrlenW(out) * sizeof(WCHAR)); WCHAR *q = wcschr(out, L'"'); if (q) *q = 0; }
            if (GetFileAttributesW(out) != INVALID_FILE_ATTRIBUTES) return TRUE;
        }
    }
    return FALSE;
}

/* open a link; "app window" = stand-alone window without tabs (Edge / Chrome --app).
   browser: per-item override ("edge"/"chrome"/"firefox"/path); empty = the global choice in Settings > General */
static void OpenUrlData(const WCHAR *url0, int appmode, const WCHAR *browser) {
    WCHAR url[1100], args[1300], exe[MAX_PATH] = L"", full[MAX_PATH];
    BOOL isFox = FALSE;
    lstrcpynW(url, url0, 1100);
    if (!wcsstr(url, L"://") && StrCmpNIW(url, L"mailto:", 7) != 0 && StrCmpNIW(url, L"shell:", 6) != 0) { WCHAR t[1100]; wsprintfW(t, L"https://%s", url); lstrcpynW(url, t, 1100); }
    int b = cfg.browser;   /* 0 auto (Edge first), 1 Edge, 2 Chrome, 3 Firefox, 4 system default */
    if (browser[0] && lstrcmpiW(browser, L"default") != 0) {
        if (!lstrcmpiW(browser, L"edge")) b = 1; else if (!lstrcmpiW(browser, L"chrome")) b = 2; else if (!lstrcmpiW(browser, L"firefox")) b = 3;
        else { lstrcpynW(exe, browser, MAX_PATH); b = -1; }
    }
    if (b == 4 && !appmode) { ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL); return; }
    if (b == 4) b = 0;
    if (b == 0 || b == 1) lstrcpyW(exe, L"msedge.exe");
    else if (b == 2) lstrcpyW(exe, L"chrome.exe");
    else if (b == 3) { lstrcpyW(exe, L"firefox.exe"); isFox = TRUE; }
    if (b >= 0 && BrowserExe(exe, full)) lstrcpyW(exe, full);
    /* Chromium: "--app=URL" already opens a standalone app window. Adding "--new-window" makes some
       versions fall back to a normal browser window (the web app then looked like it did not open). */
    if (appmode && !isFox) wsprintfW(args, L"--app=\"%s\"", url); else wsprintfW(args, L"\"%s\"", url);
    if ((INT_PTR)ShellExecuteW(NULL, L"open", exe, args, NULL, SW_SHOWNORMAL) > 32) return;
    if (appmode && b <= 1) {
        WCHAR c2[MAX_PATH] = L"chrome.exe";
        if (BrowserExe(L"chrome.exe", full)) lstrcpyW(c2, full);
        if ((INT_PTR)ShellExecuteW(NULL, L"open", c2, args, NULL, SW_SHOWNORMAL) > 32) return;
    }
    ShellExecuteW(NULL, L"open", url, NULL, NULL, SW_SHOWNORMAL);
}

static void LaunchData(int type, int appmode, const WCHAR *path, const WCHAR *args) {
    if (type == T_URL) OpenUrlData(path, appmode, args);
    else if (type == T_TRASH) ShellExecuteW(NULL, L"open", L"shell:RecycleBinFolder", NULL, NULL, SW_SHOWNORMAL);
    else if (!lstrcmpW(path, L"mdock:start")) {
        keybd_event(VK_LWIN, 0, 0, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
    } else {
        WCHAR dir[MAX_PATH];
        lstrcpynW(dir, path, MAX_PATH);
        if (type == T_FILE && StrCmpNIW(path, L"shell:", 6) != 0 && StrCmpNIW(path, L"::{", 3) != 0) PathRemoveFileSpecW(dir); else dir[0] = 0;
        ShellExecuteW(NULL, L"open", path, args[0] ? args : NULL, dir[0] ? dir : NULL, SW_SHOWNORMAL);
    }
}

/* launching runs on a worker thread so slow shell / browser start-ups never stall the dock */
static DWORD WINAPI LaunchWork(LPVOID p) {
    LaunchJob *j = (LaunchJob *)p;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    LaunchData(j->type, j->appmode, j->path, j->args);
    CoUninitialize();
    HeapFree(GetProcessHeap(), 0, j);
    return 0;
}

static void LaunchAsync(const Item *it) {
    LaunchJob *j = (LaunchJob *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LaunchJob));
    if (!j) return;
    j->type = it->type; j->appmode = it->appmode;
    lstrcpynW(j->path, it->path, 1024);
    lstrcpynW(j->args, it->args, 260);
    if (!QueueUserWorkItem(LaunchWork, j, WT_EXECUTELONGFUNCTION)) { LaunchData(j->type, j->appmode, j->path, j->args); HeapFree(GetProcessHeap(), 0, j); }
}

static void LaunchPtr(Item *it) {
    if (HasWinKey(it) && ActivateItem(it, FALSE)) return;
    LaunchAsync(it);
}

static void LaunchItem(int i, BOOL force) {
    if (i < 0 || i >= nItems) return;
    LogEvent(L"dock-item-activate", (DWORD)i);
    Item *it = &items[i];
    if (it->type == T_SEP) return;
    if (it->type == T_TRAY) { ActivateTrayApp(it); return; }
    if (it->type == T_WIN) { ToggleWindowItem(i); return; }
    if (it->temp && it->type == T_FILE && !force) {
        /* A running-app icon represents an existing window, not an EXE launch. */
        if (!ActivateRunningItem(i)) SetTimer(g_hwnd, T_SCAN, 50, NULL);
        return;
    }
    if (it->type == T_GROUP) { if (GetTickCount() - g_popupClosedTick > 350) ShowGroup(i); return; }
    if (it->type == T_NOTE) { ShowNote(); return; }
    if (it->type == T_FILE && !lstrcmpW(it->path, L"mdock:launchpad")) { if (GetTickCount() - g_popupClosedTick > 350) ShowLaunchpad(); return; }
    if (it->type == T_FOLDER && cfg.folderFan && !force) { if (GetTickCount() - g_popupClosedTick > 350) ShowFan(i); return; }
    if ((HasWinKey(it) || (it->type == T_URL && it->appmode)) && !force) {
        if (cfg.clickToggle || cfg.minAnim) { if (ToggleApp(it)) return; }
        else if (ActivateItem(it, FALSE)) return;
    }
    if ((HasWinKey(it) || (it->type == T_URL && it->appmode)) && !force && RevealHiddenWindow(it)) {
        if (cfg.bounce) it->bounce = GetTickCount() | 1;
        Kick();
        return;
    }
    if (it->type == T_URL && it->appmode) BindStart(it);   /* remember which browser window this icon opens */
    if (cfg.bounce) it->bounce = GetTickCount() | 1;
    g_launchTick = GetTickCount();   /* a freshly launched app must not make the dock slide out and back */
    LaunchAsync(it);
    SetTimer(g_hwnd, T_SCAN, 1200, NULL);
    Kick();
}

/* ======================= rendering ======================= */
static float MapActualToBase(float mx) {
    if (prevN != nItems || nItems == 0) return mx;
    if (nItems == 1) return baseC[0] + (mx - actC[0]);
    if (mx <= actC[0]) return baseC[0] + (mx - actC[0]);
    if (mx >= actC[nItems - 1]) return baseC[nItems - 1] + (mx - actC[nItems - 1]);
    for (int i = 0; i < nItems - 1; i++)
        if (mx >= actC[i] && mx < actC[i + 1]) {
            float d = actC[i + 1] - actC[i];
            float t = d > 0.001f ? (mx - actC[i]) / d : 0;
            return baseC[i] + t * (baseC[i + 1] - baseC[i]);
        }
    return mx;
}

static int HitIndex(float u, float v) {
    if (gHideCur > 0.5f) return -1;
    if (v < barTop - M.Z - 16 || v > LH) return -1;
    for (int i = 0; i < nItems; i++)
        if (items[i].cur > 1 && u >= slotL[i] - M.g / 2 && u < slotR[i] + M.g / 2) return i;
    float ex = (float)Sc(10);   /* separators are thin: extra hit margin, icons keep priority */
    for (int i = 0; i < nItems; i++)
        if (items[i].type == T_SEP && items[i].cur > 1 && u >= slotL[i] - M.g / 2 - ex && u < slotR[i] + M.g / 2 + ex) return i;
    return -1;
}

static float BounceOff(Item *it, BOOL *active) {
    if (!it->bounce) return 0;
    DWORD t = GetTickCount() - it->bounce;
    if (t > 1100) { it->bounce = 0; return 0; }
    *active = TRUE;
    float x = t / 1100.f;
    return fabsf(sinf(x * PI_F * 2.f)) * (1.f - x * 0.55f) * M.S * 0.48f;
}

static void DrawBubbleAt(GpGraphics *g, const WCHAR *text, float ix, float iy, float iw, float ih) {
    float w = TextWidth(g, text, g_font) + 24, h = 28, x, y;
    switch (cfg.edge) {
    case E_TOP: x = ix + iw / 2 - w / 2; y = iy + ih + 8; break;
    case E_LEFT: x = ix + iw + 10; y = iy + ih / 2 - h / 2; break;
    case E_RIGHT: x = ix - 10 - w; y = iy + ih / 2 - h / 2; break;
    default: x = ix + iw / 2 - w / 2; y = iy - 8 - h; break;
    }
    if (x < 4) x = 4;
    if (x + w > winW - 4) x = winW - 4 - w;
    if (y < 4) y = 4;
    if (y + h > winH - 4) y = winH - 4 - h;
    FillRR(g, ARGBC(228, 38, 38, 42), x, y, w, h, 9);
    StrokeRR(g, ARGBC(50, 255, 255, 255), 1, x, y, w, h, 9);
    DrawCentered(g, text, g_font, x, y, w, h, ARGBC(255, 245, 245, 247));
}

static void RefreshAccent(void) {
    DWORD col = 0;
    BOOL op = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&col, &op))) {
        float r = ((col >> 16) & 255) / 255.f, g = ((col >> 8) & 255) / 255.f, b = (col & 255) / 255.f;
        float mx = r > g ? (r > b ? r : b) : (g > b ? g : b), mn = r < g ? (r < b ? r : b) : (g < b ? g : b), d = mx - mn, h = 0;
        if (d > 0.001f) {
            if (mx == r) h = fmodf((g - b) / d, 6.f); else if (mx == g) h = (b - r) / d + 2; else h = (r - g) / d + 4;
            h *= 60; if (h < 0) h += 360;
        }
        g_accH = h;
    }
}

/* ---- true frosted glass: a click-through, rounded "backdrop" window under the dock that DWM blurs live ---- */
typedef struct { int AccentState, AccentFlags; DWORD GradientColor; int AnimationId; } ACCENTPOLICY;
typedef struct { int Attribute; PVOID Data; ULONG Size; } WCAD;
typedef BOOL (WINAPI *pSetWCA)(HWND, WCAD *);
static pSetWCA g_swca;
static BOOL g_blurOK;
static HWND g_back;
static RECT g_backRc;
static BOOL g_dtActive;

static LRESULT CALLBACK BackProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_NCHITTEST) return HTTRANSPARENT;
    if (m == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    if (m == WM_ERASEBKGND) { RECT rc; GetClientRect(h, &rc); FillRect((HDC)w, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH)); return 1; }
    return DefWindowProcW(h, m, w, l);
}

/* position a blurred, rounded backdrop just below `above` covering the screen rects rs[0..n).
   The DWM blur is applied to the whole square window on Windows 11 (SetWindowRgn does not clip the
   accent blur), so the window itself is inset by ~30% of the corner radius: the square window then
   sits completely inside the rounded panel and no square corner can ever show. A rounded region is
   still applied (it is pixel-perfect where the system honours it, and it is applied *before* the very
   first show so the opening frame is never square). */
static void PlaceBackdrop(HWND bk, HWND above, const RECT *rs, const int *rads, int n) {
    RECT in[4];
    int ir[4];
    for (int i = 0; i < n; i++) {
        int d = (int)(rads[i] * 0.30f) + 2;   /* 0.293 * r keeps a square corner inside a rounded one */
        if (d > 12) d = 12;
        if (d < 2) d = 2;
        RECT r = rs[i];
        InflateRect(&r, -d, -d);
        if (r.right - r.left < 10 || r.bottom - r.top < 10) { r = rs[i]; InflateRect(&r, -1, -1); d = 1; }
        in[i] = r;
        ir[i] = rads[i] - d;
        if (ir[i] < 0) ir[i] = 0;
    }
    RECT u = in[0];
    for (int i = 1; i < n; i++) UnionRect(&u, &u, &in[i]);
    HRGN rg = NULL;
    for (int i = 0; i < n; i++) {
        RECT r = in[i];
        HRGN g1 = CreateRoundRectRgn(r.left - u.left, r.top - u.top, r.right - u.left + 1, r.bottom - u.top + 1, ir[i] * 2, ir[i] * 2);
        if (!rg) rg = g1; else { CombineRgn(rg, rg, g1, RGN_OR); DeleteObject(g1); }
    }
    int w = u.right - u.left, h = u.bottom - u.top;
    if (w < 8) w = 8;
    if (h < 8) h = 8;
    if (!IsWindowVisible(bk)) {
        /* first appearance: give it its final size and shape while hidden, then reveal it below `above` */
        SetWindowPos(bk, above, u.left, u.top, w, h, SWP_NOACTIVATE | SWP_NOZORDER);
        if (rg) SetWindowRgn(bk, rg, TRUE);
        SetWindowPos(bk, above, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    } else {
        SetWindowPos(bk, above, u.left, u.top, w, h, SWP_NOACTIVATE | SWP_NOZORDER);
        if (rg) SetWindowRgn(bk, rg, TRUE);
        SetWindowPos(bk, above, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
}

static void UpdateBackdrop(void) {
    /* The dock paints its own frosted glass inside the layered window (UpdateFrost + DrawBar). A second
       DWM blur window is no longer used: it could never be clipped to the rounded panel on Windows 11
       (the accent blur always fills the whole window rect), which is what produced the square frost
       corners and the white blocks while the bar was being resized. */
    if (g_back && IsWindowVisible(g_back)) ShowWindow(g_back, SW_HIDE);
}

/* MiniDock is a desktop accessory: it must never claim a taskbar button. The dock itself is a tool
   window (the shell skips it), and ITaskbarList::DeleteTab also removes a button Windows may already
   have created - for example when the guardian restarted the app while Explorer was rebuilding the bar. */
static ITaskbarList *g_tbl;
static void HideTaskbarButton(HWND h) {
    if (!h || !cfg.hideTaskbar) return;
    if (!g_tbl) {
        if (FAILED(CoCreateInstance(&CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, &IID_ITaskbarList, (void **)&g_tbl))) { g_tbl = NULL; return; }
        g_tbl->lpVtbl->HrInit(g_tbl);
    }
    g_tbl->lpVtbl->DeleteTab(g_tbl, h);
}

static void DrawGrad(GpGraphics *G_, float bx, float by, float bw, float bh, float rad, float op) {
    int c[3][3], n = 2;
    if (cfg.accentTone) {
        if (g_dark) { HsvRgb(g_accH, 0.42f, 0.34f, &c[0][0], &c[0][1], &c[0][2]); HsvRgb(fmodf(g_accH + 18, 360), 0.5f, 0.22f, &c[1][0], &c[1][1], &c[1][2]); }
        else { HsvRgb(g_accH, 0.16f, 0.97f, &c[0][0], &c[0][1], &c[0][2]); HsvRgb(fmodf(g_accH + 18, 360), 0.24f, 0.90f, &c[1][0], &c[1][1], &c[1][2]); }
    } else if (cfg.bgColor1 >= 0) {
        c[0][0] = GetRValue(cfg.bgColor1); c[0][1] = GetGValue(cfg.bgColor1); c[0][2] = GetBValue(cfg.bgColor1);
        if (cfg.bgColor2 >= 0) { c[1][0] = GetRValue(cfg.bgColor2); c[1][1] = GetGValue(cfg.bgColor2); c[1][2] = GetBValue(cfg.bgColor2); }
        else { c[1][0] = c[0][0] * 3 / 4; c[1][1] = c[0][1] * 3 / 4; c[1][2] = c[0][2] * 3 / 4; }
    } else if (g_dark) { c[0][0] = 58; c[0][1] = 60; c[0][2] = 68; c[1][0] = 30; c[1][1] = 31; c[1][2] = 36; }
    else { c[0][0] = 255; c[0][1] = 255; c[0][2] = 255; c[1][0] = 232; c[1][1] = 235; c[1][2] = 243; }
    if (cfg.bgColor3 >= 0 && !cfg.accentTone) { c[2][0] = GetRValue(cfg.bgColor3); c[2][1] = GetGValue(cfg.bgColor3); c[2][2] = GetBValue(cfg.bgColor3); n = 3; }
    int a[3] = { (int)(op * cfg.gradA1 * 2.55f), (int)(op * cfg.gradA2 * 2.55f), (int)(op * cfg.gradA3 * 2.55f) };
    GpBrush *bb;
    if (cfg.bgMode == 0) GdipCreateSolidFill(ARGBC(a[0], c[0][0], c[0][1], c[0][2]), &bb);
    else {
        float ang = cfg.gradAngle * PI_F / 180.f, dx = cosf(ang), dy = sinf(ang);
        float half = (fabsf(dx) * bw + fabsf(dy) * bh) / 2, cx = bx + bw / 2, cy = by + bh / 2;
        PointF p1 = { cx - dx * half, cy - dy * half }, p2 = { cx + dx * half, cy + dy * half };
        GdipCreateLineBrush(&p1, &p2, ARGBC(a[0], c[0][0], c[0][1], c[0][2]), ARGBC(a[n - 1], c[n - 1][0], c[n - 1][1], c[n - 1][2]), 0, &bb);
        if (n == 3) {
            ARGB cols[3] = { ARGBC(a[0], c[0][0], c[0][1], c[0][2]), ARGBC(a[1], c[1][0], c[1][1], c[1][2]), ARGBC(a[2], c[2][0], c[2][1], c[2][2]) };
            REAL pos[3] = { 0, 0.5f, 1 };
            GdipSetLinePresetBlend(bb, cols, pos, 3);
        }
    }
    FillRoundRect(G_, bb, bx, by, bw, bh, rad);
    GdipDeleteBrush(bb);
}

/* bar / clock background: solid, gradient (Material You tones) or frosted glass; opacity 0 = nothing at all */
/* the little control-centre glyph that sits in the bar next to the clock */
/* ---- macOS-style line icons: the same drawing for the status bar, the dock widgets and the control centre ---- */
static GpPen *MiPen(ARGB c, float w) {
    GpPen *p;
    GdipCreatePen1(c, w, 2, &p);
    GdipSetPenStartCap(p, 2); GdipSetPenEndCap(p, 2); GdipSetPenLineJoin(p, 2);   /* round caps and joins */
    return p;
}
static void MIcoWifi(GpGraphics *g, float cx, float cy, float s, ARGB c) {
    GpPen *p = MiPen(c, s * 0.11f);
    float by = cy + s * 0.34f;
    for (int i = 0; i < 3; i++) { float r = s * (0.22f + i * 0.21f); GdipDrawArc(g, p, cx - r, by - r, 2 * r, 2 * r, 228, 84); }
    FillRR(g, c, cx - s * 0.075f, by - s * 0.075f, s * 0.15f, s * 0.15f, s * 0.075f);
    GdipDeletePen(p);
}
static void MIcoBt(GpGraphics *g, float cx, float cy, float s, ARGB c) {
    GpPen *p = MiPen(c, s * 0.11f);
    float a = s * 0.19f, h = s * 0.34f, k = h * 0.46f;   /* 0.68 s overall, the same optical height as wifi / volume */
    GdipDrawLine(g, p, cx, cy - h, cx, cy + h);
    GdipDrawLine(g, p, cx, cy - h, cx + a, cy - k);
    GdipDrawLine(g, p, cx + a, cy - k, cx - a, cy + k);
    GdipDrawLine(g, p, cx, cy + h, cx + a, cy + k);
    GdipDrawLine(g, p, cx + a, cy + k, cx - a, cy - k);
    GdipDeletePen(p);
}
static void MIcoVol(GpGraphics *g, float cx, float cy, float s, ARGB c) {
    GpPen *p = MiPen(c, s * 0.10f);
    float x0 = cx - s * 0.44f, xa = x0 + s * 0.18f, xb = x0 + s * 0.42f;
    GdipDrawLine(g, p, x0, cy - s * 0.14f, xa, cy - s * 0.14f);
    GdipDrawLine(g, p, xa, cy - s * 0.14f, xb, cy - s * 0.34f);
    GdipDrawLine(g, p, xb, cy - s * 0.34f, xb, cy + s * 0.34f);
    GdipDrawLine(g, p, xb, cy + s * 0.34f, xa, cy + s * 0.14f);
    GdipDrawLine(g, p, xa, cy + s * 0.14f, x0, cy + s * 0.14f);
    GdipDrawLine(g, p, x0, cy + s * 0.14f, x0, cy - s * 0.14f);
    for (int i = 0; i < 2; i++) { float r = s * (0.20f + i * 0.18f); GdipDrawArc(g, p, xb - r + s * 0.04f, cy - r, 2 * r, 2 * r, -46, 92); }
    GdipDeletePen(p);
}
static void MIcoVolMute(GpGraphics *g, float cx, float cy, float s, ARGB c) {   /* speaker + x: muted */
    GpPen *p = MiPen(c, s * 0.10f);
    float x0 = cx - s * 0.46f, xa = x0 + s * 0.16f, xb = x0 + s * 0.36f;
    GdipDrawLine(g, p, x0, cy - s * 0.13f, xa, cy - s * 0.13f);
    GdipDrawLine(g, p, xa, cy - s * 0.13f, xb, cy - s * 0.32f);
    GdipDrawLine(g, p, xb, cy - s * 0.32f, xb, cy + s * 0.32f);
    GdipDrawLine(g, p, xb, cy + s * 0.32f, xa, cy + s * 0.13f);
    GdipDrawLine(g, p, xa, cy + s * 0.13f, x0, cy + s * 0.13f);
    GdipDrawLine(g, p, x0, cy + s * 0.13f, x0, cy - s * 0.13f);
    float ex = cx + s * 0.24f, ey = cy, r = s * 0.15f;
    GdipDrawLine(g, p, ex - r, ey - r, ex + r, ey + r);
    GdipDrawLine(g, p, ex + r, ey - r, ex - r, ey + r);
    GdipDeletePen(p);
}
static void MIcoCC(GpGraphics *g, float cx, float cy, float s, ARGB c) {   /* two toggles */
    float w = s * 0.86f, h = s * 0.30f, x = cx - w / 2;
    for (int i = 0; i < 2; i++) {
        float y = cy - s * 0.37f + i * s * 0.44f;
        StrokeRR(g, c, s * 0.09f, x, y, w, h, h / 2);
        float kx = i == 0 ? x + w - h * 0.5f : x + h * 0.5f;
        FillRR(g, c, kx - h * 0.28f, y + h * 0.22f, h * 0.56f, h * 0.56f, h * 0.28f);
    }
}
static void MIcoBell(GpGraphics *g, float cx, float cy, float s, ARGB c) {
    GpPen *p = MiPen(c, s * 0.10f);
    GdipDrawArc(g, p, cx - s * 0.30f, cy - s * 0.42f, s * 0.60f, s * 0.60f, 180, 180);
    GdipDrawLine(g, p, cx - s * 0.30f, cy - s * 0.12f, cx - s * 0.38f, cy + s * 0.22f);
    GdipDrawLine(g, p, cx + s * 0.30f, cy - s * 0.12f, cx + s * 0.38f, cy + s * 0.22f);
    GdipDrawLine(g, p, cx - s * 0.46f, cy + s * 0.24f, cx + s * 0.46f, cy + s * 0.24f);
    GdipDrawArc(g, p, cx - s * 0.12f, cy + s * 0.22f, s * 0.24f, s * 0.22f, 0, 180);
    GdipDeletePen(p);
}
static void MIcoChev(GpGraphics *g, float cx, float cy, float s, ARGB c) {   /* "^" */
    GpPen *p = MiPen(c, s * 0.12f);
    GdipDrawLine(g, p, cx - s * 0.30f, cy + s * 0.12f, cx, cy - s * 0.16f);
    GdipDrawLine(g, p, cx, cy - s * 0.16f, cx + s * 0.30f, cy + s * 0.12f);
    GdipDeletePen(p);
}
static void MIcoBattery(GpGraphics *g, float x, float y, float w, float h, int pct, BOOL chg, ARGB c) {
    float bw = w - h * 0.20f;
    StrokeRR(g, c, h * 0.085f, x, y, bw, h, h * 0.30f);
    FillRR(g, c, x + bw + h * 0.04f, y + h * 0.32f, h * 0.12f, h * 0.36f, h * 0.06f);
    float pad = h * 0.17f, fw = (bw - 2 * pad) * (pct < 0 ? 0 : pct > 100 ? 100 : pct) / 100.f;
    ARGB fc = chg ? ARGBC(255, 52, 199, 89) : (pct <= 20 ? ARGBC(255, 255, 69, 58) : c);
    if (fw > 1) FillRR(g, fc, x + pad, y + pad, fw, h - 2 * pad, h * 0.14f);
    if (chg) {   /* bolt */
        GpPen *p = MiPen(g_dark ? ARGBC(255, 20, 20, 24) : ARGBC(255, 255, 255, 255), h * 0.10f);
        float cx = x + bw / 2, cy = y + h / 2;
        GdipDrawLine(g, p, cx + h * 0.10f, cy - h * 0.26f, cx - h * 0.12f, cy + h * 0.02f);
        GdipDrawLine(g, p, cx - h * 0.12f, cy + h * 0.02f, cx + h * 0.12f, cy + h * 0.02f);
        GdipDrawLine(g, p, cx + h * 0.12f, cy + h * 0.02f, cx - h * 0.10f, cy + h * 0.26f);
        GdipDeletePen(p);
    }
}

static void DrawCC(GpGraphics *g, float x, float y, float w, float h) {
    float s = (h < w ? h : w) * 0.52f;
    if (s < 12) s = 12;
    MIcoCC(g, x + w / 2, y + h / 2, s, g_dark ? ARGBC(235, 245, 245, 250) : ARGBC(232, 30, 32, 40));
}

static void DrawBar(GpGraphics *G_, float bx, float by, float bw, float bh, float rad, float tx, float ty) {
    if (cfg.opacity <= 0) return;
    float op = cfg.opacity / 100.f;
    if (cfg.bgMode == 2) {
        if (g_blurOK) {   /* live DWM blur sits underneath; just add a light tint */
            int al = (int)(op * (g_dark ? 120 : 150));
            FillRR(G_, g_dark ? ARGBC(al, 22, 23, 28) : ARGBC(al, 252, 252, 255), bx, by, bw, bh, rad);
        } else if (g_frost) {   /* fallback: captured + blurred backdrop */
            GpPath *cp;
            GdipCreatePath(0, &cp);
            DrawRoundRectPath(cp, bx, by, bw, bh, rad);
            GdipSetClipPath(G_, cp, 0);
            float cm[25] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1 };   /* the blurred snapshot is opaque: the glass must never turn into clear glass */
            GdipSetImageAttributesColorMatrix(g_attrFrost, 1, TRUE, cm, NULL, 0);
            UINT sw = 0, sh = 0;
            GdipGetImageWidth(g_frost, &sw);
            GdipGetImageHeight(g_frost, &sh);
            GdipDrawImageRectRect(G_, g_frost, -tx, -ty, (float)winW, (float)winH, 0, 0, (REAL)sw, (REAL)sh, 2, g_attrFrost, NULL, NULL);
            GdipResetClip(G_);
            GdipDeletePath(cp);
        }
    } else DrawGrad(G_, bx, by, bw, bh, rad, op);
    if (cfg.border) {
        ARGB bc = cfg.borderColor >= 0 ? ARGBC(230, GetRValue(cfg.borderColor), GetGValue(cfg.borderColor), GetBValue(cfg.borderColor))
                                       : (g_dark ? ARGBC(120, 255, 255, 255) : ARGBC(95, 0, 0, 0));
        float bwid = (float)(cfg.borderWidth < 1 ? 1 : Pxs(cfg.borderWidth));
        StrokeRR(G_, bc, bwid, bx, by, bw, bh, rad);
    }
}

/* ---------------- self-drawn frosted glass for popups ----------------
   On Windows 11 the DWM accent blur fills the whole window rectangle and ignores window regions, which
   is what left square "frosted corners" outside the rounded panels. Popups therefore blur their own
   background: the desktop behind the panel is grabbed once (the panel is still hidden), blurred and
   tinted, then painted clipped to the rounded rectangle, so the frost ends exactly at the rounded edge. */
typedef struct { HDC dc; HBITMAP bmp; HGDIOBJ old; BYTE *bits; GpBitmap *gp; int w, h; } FrostSnap;

static void FrostSnapFree(FrostSnap *f) {
    if (f->gp) { GdipDisposeImage(f->gp); f->gp = NULL; }
    if (f->dc) {
        if (f->old) SelectObject(f->dc, f->old);
        if (f->bmp) DeleteObject(f->bmp);
        DeleteDC(f->dc);
    }
    ZeroMemory(f, sizeof *f);
}

static BOOL FrostSnapGrab(FrostSnap *f, const RECT *scr) {
    FrostSnapFree(f);
    int sw = scr->right - scr->left, sh = scr->bottom - scr->top;
    if (sw < 16 || sh < 16) return FALSE;
    int w = sw / 3, h = sh / 3;
    if (w < 8) w = 8;
    if (h < 8) h = 8;
    HDC scrDC = GetDC(NULL);
    f->dc = CreateCompatibleDC(scrDC);
    if (!f->dc) { ReleaseDC(NULL, scrDC); return FALSE; }
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof bi);
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h; bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
    f->bmp = CreateDIBSection(f->dc, &bi, DIB_RGB_COLORS, (void **)&f->bits, NULL, 0);
    if (!f->bmp) { ReleaseDC(NULL, scrDC); FrostSnapFree(f); return FALSE; }
    f->old = SelectObject(f->dc, f->bmp);
    SetStretchBltMode(f->dc, HALFTONE);
    StretchBlt(f->dc, 0, 0, w, h, scrDC, scr->left, scr->top, sw, sh, SRCCOPY);
    ReleaseDC(NULL, scrDC);
    GdiFlush();
    BYTE *tmp = (BYTE *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)w * h * 4);
    if (tmp) {
        int r = cfg.blur < 1 ? 1 : cfg.blur;
        if (r > 6) r = 6;
        BlurPass(f->bits, tmp, w, h, r, TRUE);
        BlurPass(tmp, f->bits, w, h, r, FALSE);
        HeapFree(GetProcessHeap(), 0, tmp);
    }
    {   /* the same neutral tint the dock uses */
        int gop = cfg.grpOpacity < 20 ? 20 : cfg.grpOpacity > 100 ? 100 : cfg.grpOpacity;
        int tr = g_dark ? 26 : 248, tg = g_dark ? 27 : 249, tb = g_dark ? 32 : 252, tt = (g_dark ? 105 : 140) * gop / 100;
        for (int i = 0; i < w * h; i++) {
            BYTE *q = f->bits + i * 4;
            q[0] = (BYTE)((q[0] * (255 - tt) + tb * tt) / 255);
            q[1] = (BYTE)((q[1] * (255 - tt) + tg * tt) / 255);
            q[2] = (BYTE)((q[2] * (255 - tt) + tr * tt) / 255);
            q[3] = 255;
        }
    }
    if (GdipCreateBitmapFromScan0(w, h, w * 4, PXF_PARGB, f->bits, &f->gp) != 0) { f->gp = NULL; FrostSnapFree(f); return FALSE; }
    f->w = w; f->h = h;
    return TRUE;
}

static void FrostSnapPaint(GpGraphics *g, FrostSnap *f, float x, float y, float w, float h, float rad, float op) {
    if (!f->gp || op <= 0.01f) return;
    GpPath *cp;
    GdipCreatePath(0, &cp);
    DrawRoundRectPath(cp, x, y, w, h, rad);
    GdipSetClipPath(g, cp, 0);
    float cm[25] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, op, 0, 0, 0, 0, 0, 1 };
    GdipSetImageAttributesColorMatrix(g_attrFrost, 1, TRUE, cm, NULL, 0);
    GdipDrawImageRectRect(g, f->gp, x, y, w, h, 0, 0, (REAL)f->w, (REAL)f->h, 2, g_attrFrost, NULL, NULL);
    GdipResetClip(g);
    GdipDeletePath(cp);
}

static void DrawReflection(GpGraphics *G_, GpBitmap *ic, float lx, float ry, float sz) {
    UINT sw = 0, sh = 0;
    GdipGetImageWidth(ic, &sw);
    GdipGetImageHeight(ic, &sh);
    float frac = cfg.reflSize / 100.f, rh = sz * frac;
    float base = cfg.reflOpacity / 100.f;
    if (rh < 2.f || sh < 4 || sw < 1) return;
    const int N = 12;   /* many thin strips: the reflection fades out smoothly instead of in two hard bands */
    if (g_reflIn) GdipSetClipRect(G_, 0, barTop, (float)LW, barBottom - barTop + 4, 0);
    for (int i = 0; i < N; i++) {
        float v0 = (float)i / N, v1 = (float)(i + 1) / N;
        float a = base * (cfg.reflGrad == 1 ? (1.f - v0) : cfg.reflGrad == 2 ? (v0 < 0.55f ? 1.f : (1.f - v0) / 0.45f) : (1.f - v0) * (1.f - v0));
        if (a < 0.008f) continue;
        float cm[25] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, a, 0, 0, 0, 0, 0, 1 };
        GdipSetImageAttributesColorMatrix(g_attrRefl, 1, TRUE, cm, NULL, 0);
        float srcTop = sh - sh * frac * v1;
        float srcH = sh * frac * (v1 - v0);
        float dstTop = ry + rh * v0, dstH = rh * (v1 - v0) + 0.6f;
        float pts[6] = { lx, dstTop + dstH, lx + sz, dstTop + dstH, lx, dstTop };   /* vertical flip */
        GdipDrawImagePointsRect(G_, ic, pts, 3, 0, srcTop, (REAL)sw, srcH, 2, g_attrRefl, NULL, NULL);
    }
    if (g_reflIn) GdipResetClip(G_);
}

/* ---- widgets next to the bar: clock + notification bell (order / side configurable) ---- */

typedef struct { HWND h; WCHAR exe[MAX_PATH]; WCHAR title[96]; DWORD t; } NotifyEnt;
static NotifyEnt g_nf[16];
static int g_nnf;
static UINT g_shellMsg;

static void NotifyMarkItems(void) {   /* red dot on the dock icon of every app that is asking for attention */
    for (int i = 0; i < nItems; i++) {
        BOOL f = FALSE;
        for (int k = 0; k < g_nnf && !f; k++) if (items[i].target[0] && !lstrcmpiW(items[i].target, g_nf[k].exe)) f = TRUE;
        items[i].notify = f;
    }
}

static void NotifyAdd(HWND h) {
    if (!h || !IsWindow(h) || h == g_hwnd) return;
    for (int k = 0; k < g_nnf; k++) if (g_nf[k].h == h) { g_nf[k].t = GetTickCount(); return; }
    HWND root = GetAncestor(h, GA_ROOT);
    if (GetForegroundWindow() == root) return;   /* the user is already looking at it */
    if (g_nnf >= 16) { memmove(&g_nf[0], &g_nf[1], sizeof(NotifyEnt) * 15); g_nnf = 15; }
    NotifyEnt *e = &g_nf[g_nnf++];
    ZeroMemory(e, sizeof *e);
    e->h = h; e->t = GetTickCount();
    {   /* UWP apps live in ApplicationFrameHost.exe: remember the real app, not the host */
        WinId wid;
        WinIdOf(h, &wid);
        if (wid.exe[0]) lstrcpynW(e->exe, wid.exe, MAX_PATH);
        else ExeOfWindow(h, e->exe);
    }
    GetWindowTextW(h, e->title, 96);
    NotifyMarkItems();
    Kick();
}

static void NotifyRemove(HWND h) {
    for (int k = 0; k < g_nnf; k++)
        if (g_nf[k].h == h) { memmove(&g_nf[k], &g_nf[k + 1], sizeof(NotifyEnt) * (g_nnf - k - 1)); g_nnf--; NotifyMarkItems(); Kick(); return; }
}
static void NotifyRemoveExe(const WCHAR *exe) {   /* the window handle may be stale: match the app instead */
    if (!exe || !exe[0]) return;
    for (int k = g_nnf - 1; k >= 0; k--)
        if (!lstrcmpiW(g_nf[k].exe, exe)) { memmove(&g_nf[k], &g_nf[k + 1], sizeof(NotifyEnt) * (g_nnf - k - 1)); g_nnf--; NotifyMarkItems(); Kick(); }
}

static void DrawNotify(GpGraphics *G_, float x, float y, float w, float h) {
    float cx = x + w / 2, cy = y + h / 2, s = (w < h ? w : h) * 0.46f;
    ARGB c = g_dark ? ARGBC(240, 240, 240, 245) : ARGBC(240, 40, 42, 52);
    MIcoBell(G_, cx, cy, s, c);
    if (g_nnf > 0) {
        WCHAR t[8];
        wsprintfW(t, L"%d", g_nnf);
        float br = s * 0.30f;
        FillRR(G_, ARGBC(255, 255, 59, 48), cx + s * 0.18f, cy - s * 0.62f, br * 2, br * 2, br);
        DrawCentered(G_, t, g_fS, cx + s * 0.18f, cy - s * 0.62f, br * 2, br * 2, ARGBC(255, 255, 255, 255));
    }
}

typedef struct { WCHAR t[48]; int grp; } ClkLine;
static int ClockLines(ClkLine *L) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    static const WCHAR *wk[7] = { L"日", L"一", L"二", L"三", L"四", L"五", L"六" };
    BOOL hasT = cfg.clockShow != 2, hasD = cfg.clockShow != 1, comp = !Horiz();
    ClkLine T, D[2];
    int nd = 0, n = 0;
    ZeroMemory(&T, sizeof T); ZeroMemory(D, sizeof D);
    if (cfg.clockSec) wsprintfW(T.t, L"%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
    else wsprintfW(T.t, L"%02d:%02d", st.wHour, st.wMinute);
    if (hasD) {
        if (comp) {   /* vertical dock: short date, weekday on its own line */
            if (cfg.dateFmt == 0) wsprintfW(D[0].t, L"%d月%d日", st.wMonth, st.wDay);
            else if (cfg.dateFmt == 1) wsprintfW(D[0].t, L"%02d/%02d", st.wMonth, st.wDay);
            else wsprintfW(D[0].t, L"%02d-%02d", st.wMonth, st.wDay);
            nd = 1;
            if (cfg.clockWeek) { wsprintfW(D[1].t, L"周%s", wk[st.wDayOfWeek]); nd = 2; }
        } else {
            if (cfg.dateFmt == 0) wsprintfW(D[0].t, L"%d年%d月%d日", st.wYear, st.wMonth, st.wDay);
            else if (cfg.dateFmt == 1) wsprintfW(D[0].t, L"%04d/%02d/%02d", st.wYear, st.wMonth, st.wDay);
            else wsprintfW(D[0].t, L"%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
            if (cfg.clockWeek) { lstrcatW(D[0].t, L" 周"); lstrcatW(D[0].t, wk[st.wDayOfWeek]); }
            nd = 1;
        }
        for (int i = 0; i < nd; i++) D[i].grp = 1;
    }
    if (hasT && hasD && cfg.clockOrder) { for (int i = 0; i < nd; i++) L[n++] = D[i]; L[n++] = T; }
    else { if (hasT) L[n++] = T; for (int i = 0; i < nd; i++) L[n++] = D[i]; }
    return n;
}

static float Damp(float f, float k) { return 1.f - powf(1.f - f, k); }   /* frame-rate independent easing */
static BOOL RenderFrame(void) {
    if (!g_hwnd || !g_g) return FALSE;
    static DWORD sPrevFrame;
    DWORD nowMs = GetTickCount();
    float dtf = sPrevFrame ? (float)(nowMs - sPrevFrame) : 15.f;
    sPrevFrame = nowMs;
    if (dtf < 0.5f) dtf = 0.5f;
    if (dtf > 60.f) dtf = 60.f;
    float dk = dtf / 15.f;   /* 1.0 == the 15 ms cadence the easing constants were tuned at */
    CalcMetrics();
    if (M.lw != LW || M.lh != LH) UpdateGeometry();
    int n = nItems;
    BOOL anim = FALSE;
    float S = M.S, Z = M.Z, g = M.g, sepW = M.sepW, padX = M.padX, pad = M.pad;

    float ht = (float)(gForceHide ? 1 : gHideTgt);
    if (fabsf(ht - gHideCur) > 0.004f) { gHideCur += (ht - gHideCur) * Damp(0.28f, dk); anim = TRUE; } else gHideCur = ht;
    {   /* fullscreen app: slide the whole dock down out of the screen instead of blinking it away */
        float ft = (float)gFsTgt;
        if (fabsf(ft - gFsOff) > 0.004f) { gFsOff += (ft - gFsOff) * Damp(gFsTgt ? 0.30f : 0.26f, dk); anim = TRUE; }
        else gFsOff = ft;
    }
    float hda = HideAmt();

    float baseW = 2 * padX;
    for (int i = 0; i < n; i++) baseW += (items[i].type == T_SEP ? sepW : S * Asp(&items[i]));
    if (n > 1) baseW += g * (n - 1);
    if (n == 0) baseW = 2 * padX + S * 4;
    float bl = (LW - baseW - g_ce) / 2 + (cfg.wSide ? g_ce : 0), x = bl + padX;
    for (int i = 0; i < n; i++) {
        float w = items[i].type == T_SEP ? sepW : S * Asp(&items[i]);
        baseC[i] = x + w / 2;
        x += w + g;
    }

    BOOL mag = cfg.zoom && hover && dragIdx < 0 && gapIdx < 0 && n > 0 && mouseU >= 0 && HideAmt() < 0.5f;
    static float zmMouse = -1e9f, zmBase;
    float u = 0;
    if (mag) {   /* recompute the zoom centre only when the mouse moved: holding still lets the sizes settle, no flicker */
        if (fabsf(mouseU - zmMouse) >= 0.75f) { zmBase = MapActualToBase(mouseU); zmMouse = mouseU; }
        u = zmBase;
    } else zmMouse = -1e9f;
    float R = (S + g) * 2.6f;
    for (int i = 0; i < n; i++) {
        Item *it = &items[i];
        float tgt;
        if (it->type == T_SEP) tgt = sepW;
        else {
            float w = 0;
            if (mag) { float d = fabsf(baseC[i] - u); if (d < R) w = 0.5f * (1.f + cosf(PI_F * d / R)); }
            tgt = S + (Z - S) * w;
        }
        if (i == dragIdx) tgt = removing ? 0.f : (it->type == T_SEP ? sepW : S);
        float d = tgt - it->cur;
        if (fabsf(d) > 0.3f) { it->cur += d * Damp(0.38f, dk); anim = TRUE; } else it->cur = tgt;
    }
    float gapTgt = gapIdx >= 0 ? S : 0;
    if (fabsf(gapTgt - gapCur) > 0.3f) { gapCur += (gapTgt - gapCur) * Damp(0.4f, dk); anim = TRUE; } else gapCur = gapTgt;
    float G = (gapCur > 0.5f) ? gapCur + (n > 0 ? g : 0) : 0;

    float tot = 2 * padX + G;
    for (int i = 0; i < n; i++) tot += items[i].cur * Asp(&items[i]);
    if (n > 1) tot += g * (n - 1);
    if (n == 0) tot += S * 4;
    barW = tot;
    if (barW > LW - 4 - g_ce) barW = LW - 4 - g_ce;
    barL = (LW - barW - g_ce) / 2 + (cfg.wSide ? g_ce : 0);
    x = barL + padX;
    for (int i = 0; i < n; i++) {
        if (gapIdx == i && G > 0) x += G;
        slotL[i] = x;
        slotR[i] = x + items[i].cur * Asp(&items[i]);
        actC[i] = (slotL[i] + slotR[i]) / 2;
        x += items[i].cur * Asp(&items[i]) + g;
    }
    prevN = n;
    if (gMoved) {
        for (int i = 0; i < n; i++) items[i].slide += items[i].prevL - slotL[i];
        gMoved = FALSE;
    }
    for (int i = 0; i < n; i++) {
        if (fabsf(items[i].slide) > 0.5f) { items[i].slide *= powf(0.62f, dk); anim = TRUE; } else items[i].slide = 0;
    }
    barBottom = (float)LH - M.margin - 2;
    barTop = barBottom - M.barH;
    hot = (hover && dragIdx < 0) ? HitIndex(mouseU, mouseV) : -1;

    float iconBottom = barBottom - pad - 2 - (g_reflIn ? g_reflH : 0);
    float off = hda * (M.barH + M.margin + 14), tx = 0, ty = 0;
    switch (cfg.edge) { case E_TOP: ty = -off; break; case E_LEFT: tx = -off; break; case E_RIGHT: tx = off; break; default: ty = off; }
    GpGraphics *G_ = g_g;
    float bx, by, bw, bh, rad = (float)Pxs(cfg.radius);
    if (rad > M.barH / 2) rad = M.barH / 2;
    MapR(barL, barTop, barW, M.barH, &bx, &by, &bw, &bh);
    {   /* widgets: logical left edge of each, right or left of the bar */
        float gapW = g + 6;
        if (!Horiz() && g_wn > 0) {
            float u = cfg.wSide ? (barL - gapW - M.barH) : (barL + barW + gapW);
            for (int k = 0; k < g_wn; k++) g_wU[g_wk[k]] = u;
        } else if (!cfg.wSide) { float u = barL + barW + gapW; for (int k = 0; k < g_wn; k++) { g_wU[g_wk[k]] = u; u += g_wWid[g_wk[k]] + gapW; } }
        else { float u = barL - gapW; for (int k = g_wn - 1; k >= 0; k--) { u -= g_wWid[g_wk[k]]; g_wU[g_wk[k]] = u; u -= gapW; } }
    }

    GdipGraphicsClear(G_, 0);
    if (hda < 0.995f) {
        GdipResetWorldTransform(G_);
        GdipTranslateWorldTransform(G_, tx, ty, 0);

        /* while something is being dragged over the dock, make the drop area reliably hit-testable */
        if (g_dtActive) {
            float zx, zy, zw, zh;
            MapR(barL - 10, barTop - Z - 22, barW + 20, M.barH + Z + 22 + 6, &zx, &zy, &zw, &zh);
            FillRR(G_, ARGBC(4, 0, 0, 0), zx, zy, zw, zh, 4);
        }

        DrawBar(G_, bx, by, bw, bh, rad, tx, ty);
        if (n == 0) DrawCentered(G_, L"将应用 / 文件夹 / 网址拖到这里", g_font, bx, by, bw, bh, g_dark ? ARGBC(190, 255, 255, 255) : ARGBC(190, 40, 40, 40));

        if (g_wn > 0 && cfg.wMerge && Horiz()) {   /* merged: every widget lives in one capsule */
            int wl = g_wk[0], wr = g_wk[g_wn - 1];
            float ux0 = g_wU[wl], ux1 = g_wU[wr] + g_wWid[wr];
            float dx0, dy0, dw0, dh0;
            MapR(ux0, barTop, ux1 - ux0, M.barH, &dx0, &dy0, &dw0, &dh0);
            DrawBar(G_, dx0, dy0, dw0, dh0, rad, tx, ty);
        }
        if (!Horiz() && g_wn > 0) {
            int wid = g_wk[0];
            float cx0, cy0, cw0, ch0;
            MapR(g_wU[wid], barTop, M.barH, M.barH, &cx0, &cy0, &cw0, &ch0);
            DrawBar(G_, cx0, cy0, cw0, ch0, rad, tx, ty);
            for (int k = 0; k < g_wn; k++) {
                int id = g_wk[k];
                g_wScr[id].left = winX + (int)(cx0 + tx); g_wScr[id].top = winY + (int)(cy0 + ty);
                g_wScr[id].right = g_wScr[id].left + (int)cw0; g_wScr[id].bottom = g_wScr[id].top + (int)ch0;
            }
            ARGB hc = g_dark ? ARGBC(235,245,245,250) : ARGBC(235,40,42,50);
            float q = cw0 * 0.16f, gapq = cw0 * 0.10f, gx = cx0 + cw0*0.5f - q - gapq*0.5f, gy = cy0 + ch0*0.5f - q - gapq*0.5f;
            FillRR(G_, hc, gx, gy, q, q, q*0.28f); FillRR(G_, hc, gx+q+gapq, gy, q, q, q*0.28f);
            FillRR(G_, hc, gx, gy+q+gapq, q, q, q*0.28f); FillRR(G_, hc, gx+q+gapq, gy+q+gapq, q, q, q*0.28f);
        } else for (int wk2 = 0; wk2 < g_wn; wk2++) {
            int wid = g_wk[wk2];
            float cx0, cy0, cw0, ch0;
            MapR(g_wU[wid], barTop, (float)g_wWid[wid], M.barH, &cx0, &cy0, &cw0, &ch0);
            if (!cfg.wMerge) DrawBar(G_, cx0, cy0, cw0, ch0, rad, tx, ty);   /* separate widget capsules */
            g_wScr[wid].left = winX + (int)(cx0 + tx); g_wScr[wid].top = winY + (int)(cy0 + ty);
            g_wScr[wid].right = g_wScr[wid].left + (int)cw0; g_wScr[wid].bottom = g_wScr[wid].top + (int)ch0;
            if (wid == W_NOTIFY) { DrawNotify(G_, cx0, cy0, cw0, ch0); continue; }
            if (wid == W_CC) { DrawCC(G_, cx0, cy0, cw0, ch0); continue; }
            if (wid == W_RES) { DrawRes(G_, cx0, cy0, cw0, ch0); continue; }
            if (wid == W_TOOL) { DrawTool(G_, cx0, cy0, cw0, ch0); continue; }
            if (wid == W_POWER) { DrawPowerWidget(G_, cx0, cy0, cw0, ch0); continue; }
            if (wid == W_MODE) { DrawMode(G_, cx0, cy0, cw0, ch0); continue; }
            if (wid == W_TRAY) { DrawTray(G_, cx0, cy0, cw0, ch0); continue; }
            ClkLine L[4];
            int nl = ClockLines(L);
            int cc = g_dark ? cfg.clockColDark : cfg.clockColLight;
            ARGB tc = cc >= 0 ? ARGBC(255, GetRValue(cc), GetGValue(cc), GetBValue(cc)) : (g_dark ? ARGBC(245, 240, 240, 245) : ARGBC(245, 30, 30, 36));
            ARGB tc2 = cc >= 0 ? ARGBC(215, GetRValue(cc), GetGValue(cc), GetBValue(cc)) : (g_dark ? ARGBC(200, 220, 220, 228) : ARGBC(200, 60, 60, 70));
            float lh = (ch0 - 8) / (nl > 0 ? nl : 1);
            for (int k = 0; k < nl; k++) {
                BOOL tl = L[k].grp == 0;
                GpFont *f = tl ? (Horiz() ? (cfg.clockBoldT ? g_fontBold : g_fTR) : (cfg.clockBoldT ? g_fDB : g_fDR)) : (cfg.clockBoldD ? g_fDB : g_fDR);
                DrawCentered(G_, L[k].t, f, cx0, cy0 + 4 + k * lh, cw0, lh, tl ? tc : tc2);
                if (k > 0 && L[k].grp != L[k - 1].grp) {   /* thin divider between the time half and the date half */
                    GpPen *lp;
                    GdipCreatePen1(g_dark ? ARGBC(90, 255, 255, 255) : ARGBC(95, 60, 60, 70), 1.f, 2, &lp);
                    GdipDrawLine(G_, lp, cx0 + cw0 * 0.14f, cy0 + 4 + k * lh, cx0 + cw0 * 0.86f, cy0 + 4 + k * lh);
                    GdipDeletePen(lp);
                }
            }
        }

        /* separators: auto colour contrasts with the dock theme, or a custom colour */
        ARGB sc = cfg.sepColor >= 0 ? ARGBC(235, GetRValue(cfg.sepColor), GetGValue(cfg.sepColor), GetBValue(cfg.sepColor))
                                    : (g_dark ? ARGBC(190, 255, 255, 255) : ARGBC(165, 0, 0, 0));
        GpPen *sp;
        GdipCreatePen1(sc, (REAL)(cfg.sepWidth < 1 ? 1 : Pxs(cfg.sepWidth)) * 0.9f, 2, &sp);
        for (int i = 0; i < n; i++) {
            if (items[i].type != T_SEP || items[i].cur < 2 || i == dragIdx) continue;
            float cx = actC[i] + items[i].slide, lx, ly, lw, lh;
            MapR(cx, iconBottom - S * 0.88f, 0, S * 0.84f, &lx, &ly, &lw, &lh);
            GdipDrawLine(G_, sp, lx, ly, lx + lw, ly + lh);
        }
        GdipDeletePen(sp);

        if (mergeT >= 0 && mergeT < n && items[mergeT].cur > 1) {
            float hx, hy, hw, hh;
            MapR(slotL[mergeT] - 5, iconBottom - items[mergeT].cur - 5, items[mergeT].cur * Asp(&items[mergeT]) + 10, items[mergeT].cur + 10, &hx, &hy, &hw, &hh);
            FillRR(G_, ARGBC(70, 255, 255, 255), hx, hy, hw, hh, hw * 0.24f);
            StrokeRR(G_, ARGBC(190, 255, 255, 255), 2.f, hx, hy, hw, hh, hw * 0.24f);
            if (g_dtActive) {   /* say what releasing the mouse will do */
                Item *mi = &items[mergeT];
                WCHAR tip[160];
                if (mi->type == T_GROUP) wsprintfW(tip, L"放入“%s”（只保存位置链接）", mi->name);
                else if (mi->type == T_TRASH) lstrcpyW(tip, L"移到回收站");
                else wsprintfW(tip, L"用“%s”打开", mi->name);
                DrawBubbleAt(G_, tip, hx, hy, hw, hh);
            }
        } else if (g_dtActive && gapIdx >= 0 && n > 0) {
            int ai = gapIdx < n ? gapIdx : n - 1;
            float hx, hy, hw, hh;
            MapR(slotL[ai], iconBottom - items[ai].cur, items[ai].cur * Asp(&items[ai]), items[ai].cur, &hx, &hy, &hw, &hh);
            DrawBubbleAt(G_, L"松开：固定到 Dock（拖到图标上则放入应用组）", hx, hy, hw, hh);
        }

        /* icons, optional plate behind each icon */
        if (g_reflH > 0) {
            float o1 = cfg.reflOpacity / 100.f;
            float c1[25] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, o1, 0, 0, 0, 0, 0, 1 };
            float c2[25] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, o1 * 0.36f, 0, 0, 0, 0, 0, 1 };
            GdipSetImageAttributesColorMatrix(g_attrRefl, 1, TRUE, c1, NULL, 0);
            GdipSetImageAttributesColorMatrix(g_attrRefl2, 1, TRUE, c2, NULL, 0);
        }
        ARGB plateC = cfg.plateMode ? PlateArgb() : 0;
        BOOL bAct = FALSE;
        for (int i = 0; i < n; i++) {
            Item *it = &items[i];
            if (it->type == T_SEP || i == dragIdx || it->cur < 1 || !it->icon) continue;
            GpBitmap *ico = ItemIco(it);
            float bo = BounceOff(it, &bAct), sz = it->cur, lx = slotL[i] + it->slide;
            float ix, iy, iw, ih;
            MapR(lx, iconBottom - sz - bo, sz * Asp(it), sz, &ix, &iy, &iw, &ih);
            if (cfg.plateMode && it->type != T_NOTE) {   /* app groups get the same plate as every other icon */
                float pr = sz * cfg.plateRadius / 100.f;
                if (pr > sz / 2) pr = sz / 2;
                FillRR(G_, plateC, ix, iy, iw, ih, pr);
                if (cfg.plateMode == 2) StrokeRR(G_, ARGBC(60, 255, 255, 255), 1.f, ix, iy, iw, ih, pr);
                float in = sz * 0.13f;
                DrawImg(G_, ico, ix + in, iy + in, iw - 2 * in, ih - 2 * in, FALSE);
            } else DrawImg(G_, ico, ix, iy, iw, ih, FALSE);
            if (g_reflH > 0 && !bAct && Asp(it) == 1.f) DrawReflection(G_, ico, lx, g_reflIn ? (iconBottom + Pxs(cfg.reflGap)) : (barBottom + Pxs(cfg.reflGap)), sz);
            if (it->notify && !(cfg.notifyW && 0)) {   /* an app asks for attention (taskbar flash / unread): red dot */
                float bx2, by2, bw2, bh2, br2 = sz * 0.13f;
                MapR(lx + sz * Asp(it) - br2 * 1.6f, iconBottom - sz - bo - br2 * 0.2f, br2 * 2, br2 * 2, &bx2, &by2, &bw2, &bh2);
                FillRR(G_, ARGBC(255, 255, 255, 255), bx2 - 1.5f, by2 - 1.5f, bw2 + 3, bh2 + 3, (bw2 + 3) / 2);
                FillRR(G_, ARGBC(255, 255, 59, 48), bx2, by2, bw2, bh2, bw2 / 2);
            }
            if (it->running && cfg.dots) {
                float ph = S / 12.f, pw = cfg.dotStyle == 0 ? S * 0.34f : S / 10.f;
                if (ph < 3.6f) ph = 3.6f;
                if (cfg.dotStyle != 0) ph = pw;
                if (pw < 7) pw = 7;
                float dx, dy, dw, dh;
                MapR(lx + sz / 2 - pw / 2, barBottom - 2.5f - ph - (g_reflIn ? g_reflH : 0), pw, ph, &dx, &dy, &dw, &dh);
                float rr = (dw < dh ? dw : dh) / 2;
                ARGB dc = cfg.dotColor >= 0 ? ARGBC(255, GetRValue(cfg.dotColor), GetGValue(cfg.dotColor), GetBValue(cfg.dotColor)) : (g_dark ? ARGBC(255, 255, 255, 255) : ARGBC(255, 24, 26, 34));
                FillRR(G_, ARGBC(120, 0, 0, 0), dx - 1, dy - 1, dw + 2, dh + 2, rr + 1);
                FillRR(G_, dc, dx, dy, dw, dh, rr);
            }
        }
        if (bAct) anim = TRUE;

        if (hot >= 0 && cfg.labels && !g_pv && items[hot].type != T_SEP && hover && gapIdx < 0 && items[hot].cur > 1 && !gMenuOpen) {
            float ix, iy, iw, ih;
            MapR(slotL[hot], iconBottom - items[hot].cur, items[hot].cur * Asp(&items[hot]), items[hot].cur, &ix, &iy, &iw, &ih);
            DrawBubbleAt(G_, items[hot].name, ix, iy, iw, ih);
        }

        GdipResetWorldTransform(G_);
        if (dragIdx >= 0 && dragIdx < n) {
            if (items[dragIdx].type == T_SEP) {
                GpPen *gp;
                GdipCreatePen1(g_dark ? ARGBC(230, 255, 255, 255) : ARGBC(230, 30, 30, 40), 2.4f, 2, &gp);
                float hl = S * 0.42f;
                if (Horiz()) GdipDrawLine(G_, gp, (REAL)dragPtA.x, dragPtA.y - hl, (REAL)dragPtA.x, dragPtA.y + hl);
                else GdipDrawLine(G_, gp, dragPtA.x - hl, (REAL)dragPtA.y, dragPtA.x + hl, (REAL)dragPtA.y);
                GdipDeletePen(gp);
            } else if (items[dragIdx].icon) {
                float sz = S * 1.15f;
                DrawImg(G_, items[dragIdx].icon, dragPtA.x - sz / 2, dragPtA.y - sz * 0.6f, sz, sz, TRUE);
                if (removing) DrawBubbleAt(G_, L"移除", dragPtA.x - sz / 2, dragPtA.y - sz * 0.6f, sz, sz);
            }
        }
    }

    /* screen rectangles of the bar (and clock) for the blur backdrop */
    g_barScr.valid = TRUE;
    g_barScr.rad = (int)rad;
    g_barRest.left = winX + (int)bx; g_barRest.top = winY + (int)by;
    g_barRest.right = g_barRest.left + (int)bw; g_barRest.bottom = g_barRest.top + (int)bh;
    g_barScr.bar.left = winX + (int)(bx + tx); g_barScr.bar.top = winY + (int)(by + ty);
    g_barScr.bar.right = g_barScr.bar.left + (int)bw; g_barScr.bar.bottom = g_barScr.bar.top + (int)bh;
    g_barScr.nw = g_wn;
    for (int k = 0; k < g_wn && k < 2; k++) g_barScr.w[k] = g_wScr[g_wk[k]];

    POINT pt = { winX, winY }, src = { 0, 0 };
    SIZE sz = { winW, winH };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(g_hwnd, NULL, &pt, &sz, g_memdc, &src, 0, &bf, ULW_ALPHA);
    UpdateBackdrop();
    if (gFsTgt && gFsOff > 0.995f && !gFsHidden) {   /* fully slid out: take the window off screen without a blink */
        gFsHidden = TRUE;
        ShowWindow(g_hwnd, SW_HIDE);
        UpdateBackdrop();
    }
    return anim;
}

/* Animation frame pacing: ask the monitor for its refresh rate instead of a fixed 15 ms timer, and
   raise the process timer resolution while animating so a high-Hz panel gets one frame per refresh. */
static int AnimInterval(void) {
    HDC dc = GetDC(NULL);
    int hz = dc ? GetDeviceCaps(dc, VREFRESH) : 60;
    if (dc) ReleaseDC(NULL, dc);
    if (hz < 30 || hz > 1000) hz = 60;
    int ms = 1000 / hz;
    if (ms < 2) ms = 2;   /* below ~500 fps the extra wake-ups only burn CPU */
    if (ms > 22) ms = 22;
    return ms;
}
static void Kick(void) {
    if (!g_hwnd) return;
    if (!gAnimOn) {
        gAnimOn = TRUE;
        timeBeginPeriod(1);
        SetTimer(g_hwnd, T_ANIM, AnimInterval(), NULL);
    }
    KillTimer(g_hwnd, T_TRIM);
}

/* screen rect of an icon (for popups) */
static void IconScreenRect(int idx, RECT *rc) {
    float iconBottom = barBottom - M.pad - 2 - (g_reflIn ? g_reflH : 0);
    float ix, iy, iw, ih;
    MapR(slotL[idx], iconBottom - items[idx].cur, items[idx].cur * Asp(&items[idx]), items[idx].cur, &ix, &iy, &iw, &ih);
    rc->left = winX + (int)ix; rc->top = winY + (int)iy;
    rc->right = rc->left + (int)iw; rc->bottom = rc->top + (int)ih;
}

/* ======================= themed dialog helpers (unified look for every dialog) ======================= */
static HBRUSH g_dbBg, g_dbEdit;
static BOOL g_dbDark = -1;
static COLORREF DlgBgC(void) { return g_uiDark ? RGB(32, 33, 38) : RGB(246, 247, 251); }
static COLORREF DlgTxtC(void) { return g_uiDark ? RGB(236, 237, 242) : RGB(28, 30, 38); }
static COLORREF DlgEditC(void) { return g_uiDark ? RGB(48, 50, 58) : RGB(255, 255, 255); }
static void DlgBrushes(void) {
    if (g_dbDark == g_uiDark && g_dbBg) return;
    if (g_dbBg) DeleteObject(g_dbBg);
    if (g_dbEdit) DeleteObject(g_dbEdit);
    g_dbBg = CreateSolidBrush(DlgBgC());
    g_dbEdit = CreateSolidBrush(DlgEditC());
    g_dbDark = g_uiDark;
}
static void DrawOwnerButton(DRAWITEMSTRUCT *d) {
    GpGraphics *g = NULL;
    GdipCreateFromHDC(d->hDC, &g);
    if (!g) return;
    SetupG(g);
    RECT rc = d->rcItem;
    FillRect(d->hDC, &rc, g_dbBg);
    BOOL prim = GetPropW(d->hwndItem, L"mdp") != NULL, sel = GetPropW(d->hwndItem, L"mds") != NULL, red = GetPropW(d->hwndItem, L"mdr") != NULL;
    BOOL pressed = (d->itemState & ODS_SELECTED) != 0, dis = (d->itemState & ODS_DISABLED) != 0;
    ARGB bg, fg;
    if (prim || sel) { bg = pressed ? ARGBC(255, 0, 110, 225) : ARGBC(255, 10, 132, 255); fg = ARGBC(255, 255, 255, 255); }
    else if (red) { bg = g_uiDark ? ARGBC(255, 70, 40, 44) : ARGBC(255, 253, 232, 232); fg = ARGBC(255, 255, 69, 58); }
    else { bg = g_uiDark ? (pressed ? ARGBC(255, 78, 80, 92) : ARGBC(255, 58, 60, 70)) : (pressed ? ARGBC(255, 214, 218, 230) : ARGBC(255, 232, 235, 243)); fg = g_uiDark ? ARGBC(255, 236, 237, 242) : ARGBC(255, 28, 30, 38); }
    if (dis) fg = ARGBC(120, 140, 142, 152);
    float w = (float)(rc.right - rc.left), h = (float)(rc.bottom - rc.top);
    FillRR(g, bg, 0.5f, 0.5f, w - 1, h - 1, 9);
    WCHAR t[96];
    GetWindowTextW(d->hwndItem, t, 96);
    DrawCentered(g, t, g_font, 0, 0, w, h, fg);
    GdipDeleteGraphics(g);
}
/* returns TRUE (and the LRESULT in *out) when the message is a themed-dialog message */
static BOOL DlgCommon(UINT m, WPARAM w, LPARAM l, LRESULT *out) {
    DlgBrushes();
    switch (m) {
    case WM_CTLCOLORDLG: case WM_CTLCOLORSTATIC: case WM_CTLCOLORBTN:
        SetTextColor((HDC)w, DlgTxtC()); SetBkColor((HDC)w, DlgBgC()); *out = (LRESULT)g_dbBg; return TRUE;
    case WM_CTLCOLOREDIT: case WM_CTLCOLORLISTBOX:
        SetTextColor((HDC)w, DlgTxtC()); SetBkColor((HDC)w, DlgEditC()); *out = (LRESULT)g_dbEdit; return TRUE;
    case WM_DRAWITEM:
        if (((DRAWITEMSTRUCT *)l)->CtlType == ODT_BUTTON) { DrawOwnerButton((DRAWITEMSTRUCT *)l); *out = TRUE; return TRUE; }
        return FALSE;
    }
    return FALSE;
}
static HWND MkBtn(HWND parent, const WCHAR *t, int x, int y, int w, int h, int id, int style) {
    HWND b = CreateWindowW(L"BUTTON", t, WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_OWNERDRAW | style, x, y, w, h, parent, (HMENU)(INT_PTR)id, g_inst, NULL);
    if (style & BS_DEFPUSHBUTTON) SetPropW(b, L"mdp", (HANDLE)1);
    return b;
}
static LRESULT DlgErase(HWND h, HDC dc) { RECT rc; GetClientRect(h, &rc); DlgBrushes(); FillRect(dc, &rc, g_dbBg); return 1; }
static void DarkTitleBar(HWND h) { BOOL d = g_uiDark; DwmSetWindowAttribute(h, 20, &d, sizeof d); }

/* ======================= input box ======================= */
typedef struct {
    HWND hw, e1, e2;
    const WCHAR *title, *l1, *l2;
    WCHAR *b1, *b2;
    int n1, n2, ok, done;
} InCtx;

static void SetFontAll(HWND parent) {
    for (HWND c = GetWindow(parent, GW_CHILD); c; c = GetWindow(c, GW_HWNDNEXT)) SendMessageW(c, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
}

static LRESULT CALLBACK InProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    InCtx *c = (InCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    { LRESULT r; if (m == WM_ERASEBKGND) return DlgErase(h, (HDC)w); if (DlgCommon(m, w, l, &r)) return r; }
    switch (m) {
    case WM_CREATE: {
        c = (InCtx *)((CREATESTRUCTW *)l)->lpCreateParams;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)c);
        c->hw = h;
        int y = Sc(14), W = Sc(420);
        CreateWindowW(L"STATIC", c->l1, WS_CHILD | WS_VISIBLE, Sc(16), y, W - Sc(32), Sc(20), h, NULL, g_inst, NULL);
        y += Sc(24);
        c->e1 = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", c->b1, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, Sc(16), y, W - Sc(32), Sc(26), h, (HMENU)1001, g_inst, NULL);
        y += Sc(36);
        if (c->l2) {
            CreateWindowW(L"STATIC", c->l2, WS_CHILD | WS_VISIBLE, Sc(16), y, W - Sc(32), Sc(20), h, NULL, g_inst, NULL);
            y += Sc(24);
            c->e2 = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", c->b2, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, Sc(16), y, W - Sc(32), Sc(26), h, (HMENU)1002, g_inst, NULL);
            y += Sc(36);
        }
        MkBtn(h, L"确定", W - Sc(188), y, Sc(80), Sc(30), IDOK, BS_DEFPUSHBUTTON);
        MkBtn(h, L"取消", W - Sc(100), y, Sc(80), Sc(30), IDCANCEL, 0);
        SetFontAll(h);
        SetFocus(c->e1);
        SendMessageW(c->e1, EM_SETSEL, 0, -1);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(w) == IDOK) {
            GetWindowTextW(c->e1, c->b1, c->n1);
            if (c->e2) GetWindowTextW(c->e2, c->b2, c->n2);
            c->ok = 1; c->done = 1;
            DestroyWindow(h);
        } else if (LOWORD(w) == IDCANCEL) { c->done = 1; DestroyWindow(h); }
        return 0;
    case WM_CLOSE: c->done = 1; DestroyWindow(h); return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static BOOL InputBox(HWND parent, const WCHAR *title, const WCHAR *l1, WCHAR *b1, int n1, const WCHAR *l2, WCHAR *b2, int n2) {
    InCtx c;
    ZeroMemory(&c, sizeof c);
    c.title = title; c.l1 = l1; c.l2 = l2; c.b1 = b1; c.b2 = b2; c.n1 = n1; c.n2 = n2;
    int W = Sc(420), H = Sc(l2 ? 200 : 136);
    RECT wa = g_mi.rcWork;
    RECT r = { 0, 0, W, H };
    AdjustWindowRectEx(&r, WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_TOPMOST);
    HWND h = CreateWindowExW(WS_EX_TOPMOST | WS_EX_DLGMODALFRAME, CLS_IN, title, WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                             (wa.left + wa.right - (r.right - r.left)) / 2, (wa.top + wa.bottom - (r.bottom - r.top)) / 2 - Sc(60),
                             r.right - r.left, r.bottom - r.top, parent, NULL, g_inst, &c);
    if (!h) return FALSE;
    DarkTitleBar(h);
    if (parent) EnableWindow(parent, FALSE);
    SetForegroundWindow(h);
    MSG msg;
    while (!c.done && GetMessageW(&msg, NULL, 0, 0) > 0)
        if (!IsDialogMessageW(h, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    if (parent) { EnableWindow(parent, TRUE); SetForegroundWindow(parent); }
    return c.ok;
}

/* ======================= modern popup menu ======================= */
#define CM_SEP 1
#define CM_CHECK 2
#define CM_DIS 4
#define CM_RED 8
#define CM_HDR 16
typedef struct { int id; WCHAR text[72]; int fl; WCHAR hint[28]; int gid; } CmItem;
#define SHM 16
typedef struct {
    Canvas cv;
    CmItem it[28];
    RECT scr;
    FrostSnap frost;
    int n, hover, result, done, px, py;
    float y[29], mw, mh, itemH, sepH;
} MenuCtx;

static int CmAdd(CmItem *a, int n, int id, const WCHAR *t, int fl) {
    a[n].id = id;
    lstrcpynW(a[n].text, t ? t : L"", 72);
    a[n].fl = fl;
    a[n].hint[0] = 0;
    a[n].gid = 0;
    return n + 1;
}
static int CmAddG(CmItem *a, int n, int id, const WCHAR *t, int fl, int gid) {
    n = CmAdd(a, n, id, t, fl);
    a[n - 1].gid = gid;
    return n;
}

static void MenuGlyph(GpGraphics *g, int gid, float x, float y, float s, ARGB c) {
    float u = s / 24.f, cx = x + s / 2, cy = y + s / 2;
    float sw = s * 0.10f;
    GpPen *p = MiPen(c, sw < 1.2f ? 1.2f : sw);
    switch (gid) {
    case 1:
        GdipDrawEllipse(g, p, cx - 8 * u, cy - 8 * u, 16 * u, 16 * u);
        FillRR(g, c, cx - 1.2f * u, cy - 5.0f * u, 2.4f * u, 2.4f * u, 1.2f * u);
        FillRR(g, c, cx - 1.2f * u, cy - 1.2f * u, 2.4f * u, 6.2f * u, 1.2f * u);
        break;
    case 2:
        GdipDrawEllipse(g, p, cx - 4.6f * u, cy - 4.6f * u, 9.2f * u, 9.2f * u);
        for (int k = 0; k < 8; k++) {
            float a = (float)k * PI_F / 4.f;
            GdipDrawLine(g, p, cx + cosf(a) * 6.0f * u, cy + sinf(a) * 6.0f * u, cx + cosf(a) * 7.8f * u, cy + sinf(a) * 7.8f * u);
        }
        break;
    case 3:
        FillRR(g, c, cx - 8 * u, cy - 4 * u, 16 * u, 8 * u, 3 * u);
        break;
    case 4:
        StrokeRR(g, c, sw, cx - 8 * u, cy - 7 * u, 16 * u, 12 * u, 2.4f * u);
        FillRR(g, c, cx - 5.0f * u, cy - 3.4f * u, 2.4f * u, 4.6f * u, 1.2f * u);
        FillRR(g, c, cx - 0.8f * u, cy - 1.0f * u, 2.4f * u, 5.6f * u, 1.2f * u);
        break;
    case 5:
        GdipDrawArc(g, p, cx - 7 * u, cy - 7 * u, 14 * u, 14 * u, 40, 250);
        GdipDrawLine(g, p, cx + 2.0f * u, cy - 6.6f * u, cx + 6.0f * u, cy - 6.6f * u);
        GdipDrawLine(g, p, cx + 6.0f * u, cy - 6.6f * u, cx + 2.0f * u, cy - 2.6f * u);
        GdipDrawLine(g, p, cx + 2.0f * u, cy - 2.6f * u, cx + 6.0f * u, cy - 2.6f * u);
        break;
    case 6:
        GdipDrawArc(g, p, cx - 7 * u, cy - 7 * u, 14 * u, 14 * u, 40, 280);
        GdipDrawLine(g, p, cx + 5.2f * u, cy - 6.8f * u, cx + 7.6f * u, cy - 2.6f * u);
        GdipDrawLine(g, p, cx + 7.6f * u, cy - 2.6f * u, cx + 3.2f * u, cy - 1.8f * u);
        break;
    case 7:
        GdipDrawArc(g, p, cx - 7 * u, cy - 6 * u, 14 * u, 14 * u, 300, 300);
        GdipDrawLine(g, p, cx, cy - 8 * u, cx, cy);
        break;
    case 8:
        StrokeRR(g, c, sw, cx - 6 * u, cy - 1 * u, 12 * u, 8 * u, 2 * u);
        GdipDrawArc(g, p, cx - 4 * u, cy - 8 * u, 8 * u, 8 * u, 180, 180);
        break;
    case 10:
        GdipDrawArc(g, p, cx - 7 * u, cy - 7 * u, 14 * u, 14 * u, 40, 250);
        GdipDrawLine(g, p, cx + 1.0f * u, cy - 7.0f * u, cx + 1.0f * u, cy - 1.0f * u);
        GdipDrawLine(g, p, cx - 1.6f * u, cy - 3.4f * u, cx + 1.0f * u, cy - 0.6f * u);
        GdipDrawLine(g, p, cx + 3.6f * u, cy - 3.4f * u, cx + 1.0f * u, cy - 0.6f * u);
        break;
    default:
        StrokeRR(g, c, sw, cx - 7 * u, cy - 7 * u, 10 * u, 14 * u, 2 * u);
        GdipDrawLine(g, p, cx - 1 * u, cy, cx + 7 * u, cy);
        GdipDrawLine(g, p, cx + 4 * u, cy - 3 * u, cx + 7 * u, cy);
        GdipDrawLine(g, p, cx + 4 * u, cy + 3 * u, cx + 7 * u, cy);
        break;
    }
    GdipDeletePen(p);
}

static void MenuDraw(MenuCtx *c, HWND h) {
    GpGraphics *g = c->cv.g;
    GdipGraphicsClear(g, 0);
    float ox = SHM, oy = SHM, pad = (float)Sc(6);
    for (int i = 8; i >= 1; i--) FillRR(g, ARGBC(5, 0, 0, 0), ox - i, oy - i + 4, c->mw + 2 * i, c->mh + 2 * i, 16.f + i);
    if (cfg.menuFrost && c->frost.gp) FrostSnapPaint(g, &c->frost, ox, oy, c->mw, c->mh, 16.f, 1.f);
    FillRR(g, (cfg.menuFrost && c->frost.gp) ? (g_menuDark ? ARGBC(34, 36, 37, 43) : ARGBC(40, 250, 250, 253)) : (g_menuDark ? ARGBC(244, 36, 37, 43) : ARGBC(244, 250, 250, 253)), ox, oy, c->mw, c->mh, 16);
    StrokeRR(g, g_menuDark ? ARGBC(70, 255, 255, 255) : ARGBC(45, 0, 0, 0), 1, ox, oy, c->mw, c->mh, 16);
    for (int i = 0; i < c->n; i++) {
        float y = oy + c->y[i];
        CmItem *t = &c->it[i];
        if (t->fl & CM_SEP) {
            GpPen *p;
            GdipCreatePen1(g_menuDark ? ARGBC(40, 255, 255, 255) : ARGBC(30, 0, 0, 0), 1, 2, &p);
            GdipDrawLine(g, p, ox + pad + 6, y + c->sepH / 2, ox + c->mw - pad - 6, y + c->sepH / 2);
            GdipDeletePen(p);
            continue;
        }
        if (t->fl & CM_HDR) {
            DrawTextBox(g, t->text, g_font, ox + pad + 14, y + 4, c->mw - 2 * pad - 20, c->itemH * 0.7f, g_menuDark ? ARGBC(150, 200, 202, 214) : ARGBC(160, 90, 94, 108), TRUE);
            continue;
        }
        BOOL hv = (i == c->hover) && !(t->fl & CM_DIS);
        if (hv) FillRR(g, ARGBC(255, 10, 132, 255), ox + pad, y, c->mw - 2 * pad, c->itemH, 9);
        ARGB col = hv ? ARGBC(255, 255, 255, 255) : (t->fl & CM_DIS) ? ARGBC(110, 128, 128, 128)
                 : (t->fl & CM_RED) ? ARGBC(255, 255, 69, 58) : g_menuDark ? ARGBC(255, 236, 236, 241) : ARGBC(255, 28, 28, 32);
        if (t->fl & CM_CHECK) {
            GpPen *p;
            GdipCreatePen1(col, 1.9f, 2, &p);
            float cx = ox + pad + 12, cy = y + c->itemH / 2;
            GdipDrawLine(g, p, cx, cy, cx + 3.5f, cy + 4);
            GdipDrawLine(g, p, cx + 3.5f, cy + 4, cx + 10, cy - 4);
            GdipDeletePen(p);
        }
        if (t->gid) MenuGlyph(g, t->gid, ox + pad + 10, y + (c->itemH - Sc(16)) / 2, Sc(16), col);
        DrawTextBox(g, t->text, g_font, ox + pad + (t->gid ? 36 : 30), y, c->mw - 2 * pad - (t->gid ? 42 : 36), c->itemH, col, TRUE);
        if (t->hint[0]) DrawTextBoxR(g, t->hint, g_font, ox + pad + 30, y, c->mw - 2 * pad - 44, c->itemH, hv ? ARGBC(200, 255, 255, 255) : ARGBC(130, 150, 152, 165));
    }
    CanvasPresent(&c->cv, h, c->px, c->py);
}

static int MenuHit(MenuCtx *c, int x, int y) {
    float lx = (float)x - SHM, ly = (float)y - SHM;
    if (lx < 0 || lx > c->mw || ly < 0 || ly > c->mh) return -1;
    for (int i = 0; i < c->n; i++)
        if (!(c->it[i].fl & (CM_SEP | CM_HDR)) && ly >= c->y[i] && ly < c->y[i + 1]) return (c->it[i].fl & CM_DIS) ? -1 : i;
    return -1;
}

static LRESULT CALLBACK MenuProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    MenuCtx *c = (MenuCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    if (!c) return DefWindowProcW(h, m, w, l);
    int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
    switch (m) {
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_MOUSEMOVE: {
        int hv = MenuHit(c, x, y);
        if (hv != c->hover) { c->hover = hv; MenuDraw(c, h); }
        return 0;
    }
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN: {
        float lx = (float)x - SHM, ly = (float)y - SHM;
        if (lx < 0 || lx > c->mw || ly < 0 || ly > c->mh) c->done = 1;
        return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP: {
        int hv = MenuHit(c, x, y);
        if (hv >= 0) { c->result = c->it[hv].id; c->done = 1; }
        return 0;
    }
    case WM_CAPTURECHANGED: c->done = 1; return 0;
    case WM_TIMER:
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) c->done = 1;
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static BOOL RectInside(POINT p, void *ctx) { return PtInRect((const RECT *)ctx, p); }

static int ShowMenu(const CmItem *items_, int n, int ax, int ay) {
    MenuCtx *c = (MenuCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(MenuCtx));
    if (!c) return 0;
    memcpy(c->it, items_, sizeof(CmItem) * n);
    c->n = n; c->hover = -1;
    c->itemH = (float)Sc(30); c->sepH = (float)Sc(8);
    float pad = (float)Sc(6), y = pad, mx = 0;
    for (int i = 0; i < n; i++) {
        c->y[i] = y;
        if (c->it[i].fl & CM_SEP) y += c->sepH;
        else if (c->it[i].fl & CM_HDR) y += c->itemH * 0.8f;
        else {
            y += c->itemH;
            float tw = TextWidth(g_g, c->it[i].text, g_font) + (c->it[i].hint[0] ? TextWidth(g_g, c->it[i].hint, g_font) + 24 : 0);
            if (tw > mx) mx = tw;
        }
    }
    c->y[n] = y;
    c->mh = y + pad;
    { BOOL icons = FALSE; for (int i = 0; i < n; i++) if (c->it[i].gid) icons = TRUE; if (icons) mx += (float)Sc(26); }
    c->mw = mx + 30 + 36 + 2 * pad;
    if (c->mw < Sc(190)) c->mw = (float)Sc(190);
    int W = (int)c->mw + 2 * SHM, H = (int)c->mh + 2 * SHM;
    RECT mr = g_mi.rcMonitor;
    int px, py;
    switch (g_menuEdge >= 0 ? g_menuEdge : cfg.edge) {
    case E_TOP: px = ax - W / 2; py = ay + 8; break;
    case E_LEFT: px = ax + 8; py = ay - H / 2; break;
    case E_RIGHT: px = ax - W - 8; py = ay - H / 2; break;
    default: px = ax - W / 2; py = ay - H - 4; break;
    }
    if (px + W > mr.right) px = mr.right - W;
    if (py + H > mr.bottom) py = mr.bottom - H;
    if (px < mr.left) px = mr.left;
    if (py < mr.top) py = mr.top;
    c->px = px; c->py = py;
    c->scr.left = px + SHM; c->scr.top = py + SHM; c->scr.right = px + SHM + (int)c->mw; c->scr.bottom = py + SHM + (int)c->mh;
    int res = 0;
    if (CanvasNew(&c->cv, W, H)) {
        HWND h = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CLS_MENU, L"", WS_POPUP, px, py, W, H, NULL, NULL, g_inst, NULL);
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)c);
        if (cfg.menuFrost) FrostSnapGrab(&c->frost, &c->scr);   /* blur of what is behind the menu, taken before it appears */
        MenuDraw(c, h);
        ShowWindow(h, SW_SHOWNA);
        SetCapture(h);
        SetTimer(h, 1, 50, NULL);
        GuardStart(RectInside, &c->scr, &c->done, h);
        MSG msg;
        while (!c->done && GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        GuardStop();
        SetWindowLongPtrW(h, GWLP_USERDATA, 0);
        if (GetCapture() == h) ReleaseCapture();
        DestroyWindow(h);
        res = c->result;
        FrostSnapFree(&c->frost);
        CanvasFree(&c->cv);
    }
    HeapFree(GetProcessHeap(), 0, c);
    return res;
}

/* ======================= group popup ======================= */
typedef struct {
    Canvas cv;
    Item *g;
    RECT btn;                  /* top-right view switch button */
    RECT scr;
    int gi, cols, rows, hover, done, inMenu, px, py, dragK, alpha, dy, list;
    BOOL dragging, ready, plusDown;
    POINT dp, down;
    DWORD t0;
    float cw, ch, pad, titleH, W, H;
    HWND bk;
    FrostSnap frost;
    int rad;
} GrpCtx;

static HWND g_grpWnd;
static void GrpDraw(GrpCtx *c, HWND h);
static void FanDragOut(const WCHAR *path, HWND hide);
static BOOL BuildItem(Item *it, const WCHAR *s0);

static void GrpLayout(GrpCtx *c) {
    int nk = c->g->nk + (c->g->nk < MAXK ? 1 : 0);   /* + the "add" cell */
    c->list = (c->g->appmode & 4) != 0;              /* bit 2 of appmode: show the group as a list */
    if (c->list) {
        c->cols = 1;
        c->rows = nk < 1 ? 1 : nk;
        c->cw = (float)Sc(320); c->ch = (float)Sc(46); c->pad = (float)Sc(14); c->titleH = (float)Sc(38);
        c->W = c->cw + 2 * c->pad;
        c->H = c->titleH + c->rows * c->ch + 2 * c->pad;
        return;
    }
    c->cols = nk < 4 ? (nk < 1 ? 3 : nk) : (nk > 12 ? 6 : 4);
    c->rows = nk < 1 ? 1 : (nk + c->cols - 1) / c->cols;
    c->cw = (float)Sc(100); c->ch = (float)Sc(112); c->pad = (float)Sc(16); c->titleH = (float)Sc(38);
    c->W = c->cols * c->cw + 2 * c->pad;
    if (c->W < Sc(250)) c->W = (float)Sc(250);
    c->H = c->titleH + c->rows * c->ch + 2 * c->pad;
}

static BOOL g_grpAnchorOn;      /* the group was opened from the floating group strip, not from the dock */
static RECT g_grpAnchor;
static void GrpPlace(GrpCtx *c, int *px, int *py) {
    int W = (int)c->W + 2 * SHM, H = (int)c->H + 2 * SHM;
    RECT mr = g_mi.rcMonitor;
    if (cfg.groupPos == 1) { *px = (mr.left + mr.right - W) / 2; *py = (mr.top + mr.bottom - H) / 2; return; }
    RECT ir;
    if (g_grpAnchorOn) ir = g_grpAnchor; else IconScreenRect(c->gi, &ir);
    switch (cfg.edge) {
    case E_TOP: *px = (ir.left + ir.right) / 2 - W / 2; *py = ir.bottom + 6; break;
    case E_LEFT: *px = ir.right + 6; *py = (ir.top + ir.bottom) / 2 - H / 2; break;
    case E_RIGHT: *px = ir.left - W - 6; *py = (ir.top + ir.bottom) / 2 - H / 2; break;
    default: *px = (ir.left + ir.right) / 2 - W / 2; *py = ir.top - H - 6; break;
    }
    if (*px + W > mr.right) *px = mr.right - W;
    if (*py + H > mr.bottom) *py = mr.bottom - H;
    if (*px < mr.left) *px = mr.left;
    if (*py < mr.top) *py = mr.top;
}

static void GrpSync(GrpCtx *c, HWND h) {
    c->scr.left = c->px + SHM; c->scr.top = c->py + SHM; c->scr.right = c->scr.left + (int)c->W; c->scr.bottom = c->scr.top + (int)c->H;
    (void)h;   /* the frost is painted inside the canvas, no second window is needed */
}

/* the group window is re-sized after kids are added / removed, without closing */
static void GrpRelayout(GrpCtx *c, HWND h) {
    GrpLayout(c);
    int W = (int)c->W + 2 * SHM, H = (int)c->H + 2 * SHM;
    if (W != c->cv.w || H != c->cv.h2) { CanvasFree(&c->cv); CanvasNew(&c->cv, W, H); }
    GrpPlace(c, &c->px, &c->py);
    SetWindowPos(h, NULL, c->px, c->py, W, H, SWP_NOACTIVATE | SWP_NOZORDER);
    GrpSync(c, h);
    GrpDraw(c, h);
}

static void GrpDraw(GrpCtx *c, HWND h) {
    GpGraphics *g = c->cv.g;
    GdipGraphicsClear(g, 0);
    float ox = SHM, oy = SHM;
    for (int i = 8; i >= 1; i--) FillRR(g, ARGBC(5, 0, 0, 0), ox - i, oy - i + 5, c->W + 2 * i, c->H + 2 * i, 22.f + i);
    BOOL frost = cfg.sbPopFrost && c->frost.gp != NULL;
    if (frost) FrostSnapPaint(g, &c->frost, ox, oy, c->W, c->H, 22.f, PopBlurOp());
    {
        int gp = cfg.grpOpacity < 20 ? 20 : cfg.grpOpacity > 100 ? 100 : cfg.grpOpacity;
        int al = (frost ? 150 : 255) * gp / 100;
        if (cfg.grpBg >= 0) FillRR(g, ARGBC(al, GetRValue(cfg.grpBg), GetGValue(cfg.grpBg), GetBValue(cfg.grpBg)), ox, oy, c->W, c->H, 22);
        else if (frost) FillRR(g, g_dark ? ARGBC(al * 125 / 150, 20, 21, 26) : ARGBC(al, 250, 250, 254), ox, oy, c->W, c->H, 22);
        else FillRR(g, g_dark ? ARGBC(al * 245 / 255, 38, 39, 46) : ARGBC(al * 245 / 255, 247, 248, 252), ox, oy, c->W, c->H, 22);
    }
    StrokeRR(g, g_dark ? ARGBC(70, 255, 255, 255) : ARGBC(45, 0, 0, 0), 1, ox, oy, c->W, c->H, 22);
    ARGB tc = g_dark ? ARGBC(255, 240, 240, 245) : ARGBC(255, 28, 28, 34);
    ARGB sub = g_dark ? ARGBC(120, 200, 202, 214) : ARGBC(140, 90, 94, 108);
    DrawCentered(g, c->g->name, g_fontBold, ox, oy + 2, c->W, c->titleH - Sc(14), tc);   /* title on its own line */
    DrawTextBox(g, (c->g->appmode & 1) ? L"临时组 · 重启后清空 · 可拖入 / 拖出文件 · 点击标题重命名" : L"点击标题重命名", g_font, ox + Sc(12), oy + c->titleH - Sc(14), c->W - Sc(76), Sc(14), sub, TRUE);   /* left-aligned hint, clear of the view button */
    {   /* view switch: the only obvious way to discover the list / grid modes */
        float bw = (float)Sc(34), bh = (float)Sc(24);
        c->btn.left = (int)(ox + c->W - bw - Sc(14)); c->btn.top = (int)(oy + Sc(8));
        c->btn.right = c->btn.left + (int)bw; c->btn.bottom = c->btn.top + (int)bh;
        FillRR(g, g_dark ? ARGBC(50, 255, 255, 255) : ARGBC(28, 0, 0, 0), (float)c->btn.left, (float)c->btn.top, bw, bh, Sc(7));
        ARGB gc = g_dark ? ARGBC(220, 240, 240, 246) : ARGBC(220, 40, 42, 52);
        if (c->list) {   /* currently list: show a grid glyph as the target */
            for (int i = 0; i < 4; i++) FillRR(g, gc, (float)c->btn.left + Sc(7) + (i % 2) * Sc(10), (float)c->btn.top + Sc(6) + (i / 2) * Sc(8), Sc(7), Sc(6), Sc(2));
        } else {
            for (int i = 0; i < 3; i++) FillRR(g, gc, (float)c->btn.left + Sc(8), (float)c->btn.top + Sc(6) + i * Sc(5), Sc(18), Sc(2.4f), Sc(1.2f));
        }
    }
    float is = (float)Sc(56);
    int total = c->g->nk + (c->g->nk < MAXK ? 1 : 0);
    if (c->list) {   /* one app per row: icon + name, like a menu */
        float rs_ = (float)Sc(32), x0 = ox + c->pad, y0 = oy + c->titleH + c->pad;
        for (int k = 0; k < total; k++) {
            float y = y0 + k * c->ch;
            BOOL isAdd = (k == c->g->nk);
            if (k == c->hover && !c->dragging) FillRR(g, g_dark ? ARGBC(45, 255, 255, 255) : ARGBC(26, 0, 0, 0), x0, y + 1, c->cw, c->ch - 3, 10);
            if (!isAdd && c->dragging && k == c->dragK) continue;
            if (isAdd) {
                float px0 = x0 + (c->ch - rs_) / 2 - 6, py0 = y + (c->ch - rs_) / 2;
                StrokeRR(g, g_dark ? ARGBC(130, 255, 255, 255) : ARGBC(130, 0, 0, 0), 1.4f, px0, py0, rs_ * 0.72f, rs_ * 0.72f, rs_ * 0.2f);
                GpPen *pp;
                GdipCreatePen1(g_dark ? ARGBC(200, 255, 255, 255) : ARGBC(200, 0, 0, 0), 2.f, 2, &pp);
                float cx0 = px0 + rs_ * 0.36f, cy0 = py0 + rs_ * 0.36f;
                GdipDrawLine(g, pp, cx0 - rs_ * 0.16f, cy0, cx0 + rs_ * 0.16f, cy0);
                GdipDrawLine(g, pp, cx0, cy0 - rs_ * 0.16f, cx0, cy0 + rs_ * 0.16f);
                GdipDeletePen(pp);
                DrawTextBox(g, L"添加应用 / 文件…", g_font, x0 + c->ch + 4, y, c->cw - c->ch - 8, c->ch, sub, TRUE);
                continue;
            }
            if (c->g->kids[k].icon) DrawIconPlated(g, c->g->kids[k].icon, x0 + (c->ch - rs_) / 2 - 6, y + (c->ch - rs_) / 2, rs_, (float)cfg.plateRadius);
            DrawTextBox(g, c->g->kids[k].name, g_font, x0 + c->ch + 4, y, c->cw - c->ch - 8, c->ch, tc, TRUE);
        }
    } else {
    for (int k = 0; k < total; k++) {
        int col = k % c->cols, row = k / c->cols;
        float x = ox + c->pad + col * c->cw, y = oy + c->titleH + c->pad + row * c->ch;
        if (k == c->g->nk) {   /* the "+" cell: add a file / folder / link from any location */
            if (k == c->hover) FillRR(g, g_dark ? ARGBC(50, 255, 255, 255) : ARGBC(30, 0, 0, 0), x, y, c->cw, c->ch, 12);
            float px0 = x + (c->cw - is) / 2, py0 = y + 8;
            StrokeRR(g, g_dark ? ARGBC(150, 255, 255, 255) : ARGBC(150, 0, 0, 0), 1.6f, px0, py0, is, is, is * 0.24f);
            GpPen *pp;
            GdipCreatePen1(g_dark ? ARGBC(210, 255, 255, 255) : ARGBC(210, 0, 0, 0), 2.4f, 2, &pp);
            GdipDrawLine(g, pp, px0 + is * 0.3f, py0 + is / 2, px0 + is * 0.7f, py0 + is / 2);
            GdipDrawLine(g, pp, px0 + is / 2, py0 + is * 0.3f, px0 + is / 2, py0 + is * 0.7f);
            GdipDeletePen(pp);
            DrawCentered(g, L"添加", g_font, x + 2, y + 8 + is + 4, c->cw - 4, (float)Sc(22), sub);
            continue;
        }
        BOOL drag = c->dragging && k == c->dragK;
        if (k == c->hover && !c->dragging) FillRR(g, g_dark ? ARGBC(50, 255, 255, 255) : ARGBC(30, 0, 0, 0), x, y, c->cw, c->ch, 12);
        if (drag) { FillRR(g, g_dark ? ARGBC(30, 255, 255, 255) : ARGBC(20, 0, 0, 0), x + 3, y + 3, c->cw - 6, c->ch - 6, 12); continue; }
        if (c->g->kids[k].icon) DrawIconPlated(g, c->g->kids[k].icon, x + (c->cw - is) / 2, y + 8, is, (float)cfg.plateRadius);
        DrawTextBox(g, c->g->kids[k].name, g_font, x + 4, y + 8 + is + 4, c->cw - 8, (float)Sc(40), tc, FALSE);   /* long names wrap to two lines + ellipsis */
    }
    }
    if (c->dragging && c->dragK >= 0 && c->dragK < c->g->nk && c->g->kids[c->dragK].icon) {
        float giz = c->list ? (float)Sc(34) : is;
        DrawImg(g, c->g->kids[c->dragK].icon, c->dp.x - giz / 2, c->dp.y - giz / 2, giz, giz, TRUE);
    }
    CanvasPresentA(&c->cv, h, c->px, c->py + c->dy, c->alpha);
}

static int GrpHit(GrpCtx *c, int x, int y) {
    float lx = x - SHM - c->pad, ly = y - SHM - c->titleH - c->pad;
    if (lx < 0 || ly < 0) return -1;
    if (c->list) {
        if (lx > c->cw) return -1;
        int row = (int)(ly / c->ch);
        return (row >= 0 && row < c->g->nk + (c->g->nk < MAXK ? 1 : 0)) ? row : -1;
    }
    int col = (int)(lx / c->cw), row = (int)(ly / c->ch);
    if (col >= c->cols) return -1;
    int k = row * c->cols + col;
    return (k >= 0 && k < c->g->nk + (c->g->nk < MAXK ? 1 : 0)) ? k : -1;
}

static void ShowGroupMenu(GrpCtx *c, HWND h, int k);
static void GrpPlus(GrpCtx *c, HWND h);

static LRESULT CALLBACK GrpProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    GrpCtx *c = (GrpCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    if (!c) return DefWindowProcW(h, m, w, l);
    int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
    switch (m) {
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_TIMER:
        if (w == 2) {
            float t = (GetTickCount() - c->t0) / 190.f;
            if (t >= 1.f) {
                KillTimer(h, 2);
                c->alpha = 255; c->dy = 0; c->ready = TRUE;
                GrpSync(c, h);
            } else { float e = 1.f - (1.f - t) * (1.f - t) * (1.f - t); c->alpha = (int)(255 * e); c->dy = (int)((1.f - e) * 22 * (cfg.edge == E_TOP ? -1 : 1)); }
            GrpDraw(c, h);
        }
        else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) c->done = 1;
        return 0;
    case WM_MOUSEMOVE: {
        if (c->dragK >= 0 && (w & MK_LBUTTON)) {
            if (!c->dragging && (abs(x - c->down.x) > 6 || abs(y - c->down.y) > 6)) c->dragging = TRUE;
            if (c->dragging) {
                c->dp.x = x; c->dp.y = y;
                float lx = x - SHM - c->pad, ly = y - SHM - c->titleH - c->pad;   /* live reorder: slot under the cursor */
                int t;
                if (c->list) {
                    t = (int)(ly / c->ch);
                    if (t < 0) t = 0;
                } else {
                    int col = (int)(lx / c->cw), row = (int)(ly / c->ch);
                    if (col < 0) col = 0;
                    if (col >= c->cols) col = c->cols - 1;
                    if (row < 0) row = 0;
                    t = row * c->cols + col;
                }
                if (t >= c->g->nk) t = c->g->nk - 1;
                if (t != c->dragK && t >= 0) {
                    Item tmp = c->g->kids[c->dragK];
                    if (c->dragK < t) memmove(&c->g->kids[c->dragK], &c->g->kids[c->dragK + 1], sizeof(Item) * (t - c->dragK));
                    else memmove(&c->g->kids[t + 1], &c->g->kids[t], sizeof(Item) * (c->dragK - t));
                    c->g->kids[t] = tmp;
                    c->dragK = t;
                }
                GrpDraw(c, h);
                /* temporary group: drag a file out of the popup to copy it elsewhere */
                if ((c->g->appmode & 1) && c->dragK >= 0 && c->dragK < c->g->nk && (x < 0 || y < 0 || x > (int)c->W + 2 * SHM || y > (int)c->H + 2 * SHM)) {
                    Item *kd = &c->g->kids[c->dragK];
                    if (kd->type == T_FILE || kd->type == T_FOLDER) {
                        WCHAR pth[MAX_PATH];
                        lstrcpynW(pth, kd->path, MAX_PATH);
                        c->dragK = -1; c->dragging = FALSE;
                        c->inMenu = 1;
                        ReleaseCapture();
                        FanDragOut(pth, h);
                        c->inMenu = 0;
                        c->done = 1;
                    }
                }
                return 0;
            }
        }
        int hv = GrpHit(c, x, y);
        if (hv != c->hover) { c->hover = hv; GrpDraw(c, h); }
        return 0;
    }
    case WM_LBUTTONDOWN: {
        int k = GrpHit(c, x, y);
        c->plusDown = (k >= 0 && k == c->g->nk);
        c->dragK = (k >= 0 && k < c->g->nk) ? k : -1; c->down.x = x; c->down.y = y; c->dragging = FALSE;
        if (k >= 0) SetCapture(h);
        return 0;
    }
    case WM_LBUTTONUP: {
        int k = GrpHit(c, x, y);
        BOOL was = c->dragging;
        if (!was && x >= c->btn.left && x <= c->btn.right && y >= c->btn.top && y <= c->btn.bottom) {
            c->g->appmode ^= 4;
            ScheduleSave();
            GrpRelayout(c, h);
            return 0;
        }
        int dk = c->dragK;
        c->dragK = -1; c->dragging = FALSE;
        if (GetCapture() == h) ReleaseCapture();
        if (c->plusDown) { c->plusDown = FALSE; if (k == c->g->nk) GrpPlus(c, h); }
        else if (was) { BuildGroupIcon(c->g); ScheduleSave(); Kick(); GrpDraw(c, h); }
        else if (dk >= 0 && k == dk) { LaunchPtr(&c->g->kids[k]); c->done = 1; }
        else if (k < 0 && y > SHM && y < SHM + c->titleH + 4 && x > SHM && x < SHM + c->W) {
            WCHAR nm[128];
            lstrcpynW(nm, c->g->name, 128);
            c->inMenu = 1;
            BOOL ok = InputBox(g_hset, L"应用组名称", L"名称：", nm, 128, NULL, NULL, 0);
            c->inMenu = 0;
            if (ok && nm[0]) { lstrcpynW(c->g->name, nm, 128); CleanName(c->g->name); ScheduleSave(); GrpDraw(c, h); }
        }
        return 0;
    }
    case WM_RBUTTONUP: { int rk = GrpHit(c, x, y); ShowGroupMenu(c, h, rk >= c->g->nk ? -1 : rk); return 0; }
    }
    return DefWindowProcW(h, m, w, l);
}

static void GrpPlus(GrpCtx *c, HWND h) {
    CmItem it[4];
    int n = 0;
    n = CmAdd(it, n, 1, L"添加应用 / 文件…", 0);
    n = CmAdd(it, n, 2, L"添加文件夹…", 0);
    n = CmAdd(it, n, 3, L"新建快捷方式（网址 / 应用）…", 0);
    POINT p;
    GetCursorPos(&p);
    c->inMenu = 1;
    int r = ShowMenu(it, n, p.x, p.y);
    WCHAR path[MAX_PATH] = L"";
    BOOL got = FALSE;
    Item ni;
    ZeroMemory(&ni, sizeof ni);
    if (r == 1) {
        OPENFILENAMEW o;
        ZeroMemory(&o, sizeof o);
        o.lStructSize = sizeof o; o.hwndOwner = g_hset;
        o.lpstrFile = path; o.nMaxFile = MAX_PATH;
        o.lpstrFilter = L"所有文件 (*.*)\0*.*\0";
        o.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_DONTADDTORECENT;
        got = GetOpenFileNameW(&o);
        if (got && BuildItem(&ni, path)) { if (!KidAdd(c->g, &ni)) FreeItem(&ni); }
    } else if (r == 2) {
        BROWSEINFOW b;
        ZeroMemory(&b, sizeof b);
        b.hwndOwner = g_hset; b.lpszTitle = L"选择要加入应用组的文件夹"; b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST pi = SHBrowseForFolderW(&b);
        if (pi) { got = SHGetPathFromIDListW(pi, path); CoTaskMemFree(pi); }
        if (got && BuildItem(&ni, path)) { if (!KidAdd(c->g, &ni)) FreeItem(&ni); }
    } else if (r == 3) {
        if (ShortcutDialog(g_hset, &ni)) { if (!KidAdd(c->g, &ni)) FreeItem(&ni); }
    }
    c->inMenu = 0;
    BuildGroupIcon(c->g);
    ScheduleSave();
    GrpRelayout(c, h);
    Kick();
}

static void ShowGroupMenu(GrpCtx *c, HWND h, int k) {
    CmItem it[12];
    int n = 0;
    if (k >= 0) {
        n = CmAdd(it, n, 1, L"打开", 0);
        if (c->g->kids[k].target[0]) {
            n = CmAdd(it, n, 6, L"切换到应用窗口", 0);
            n = CmAdd(it, n, 7, L"关闭应用", 0);
        }
        if (c->g->kids[k].type == T_FILE || c->g->kids[k].type == T_FOLDER)
            n = CmAdd(it, n, 8, L"在资源管理器中显示", 0);
        n = CmAdd(it, n, 2, L"移出到 Dock", 0);
        n = CmAdd(it, n, 3, L"从组中移除", CM_RED);
        n = CmAdd(it, n, 0, L"", CM_SEP);
    }
    n = CmAdd(it, n, 4, L"新建快捷方式（网址 / 应用）…", 0);
    n = CmAdd(it, n, 5, L"重命名应用组", 0);
    n = CmAdd(it, n, 9, c->list ? L"以图标网格显示" : L"以列表显示", 0);
    POINT p;
    GetCursorPos(&p);
    c->inMenu = 1;
    int r = ShowMenu(it, n, p.x, p.y);
    c->inMenu = 0;
    if (r == 1 && k >= 0) { LaunchPtr(&c->g->kids[k]); c->done = 1; }
    else if (r == 2 && k >= 0) { ExtractKid(c->gi, k); GrpRelayout(c, h); }
    else if (r == 3 && k >= 0) {
        if (c->g->kids[k].icon) GdipDisposeImage(c->g->kids[k].icon);
        memmove(&c->g->kids[k], &c->g->kids[k + 1], sizeof(Item) * (c->g->nk - k - 1));
        c->g->nk--;
        BuildGroupIcon(c->g);
        ScheduleSave();
        GrpRelayout(c, h);   /* stays open so several apps can be removed in a row */
    } else if (r == 4) {
        Item ni;
        if (ShortcutDialog(g_hset, &ni)) { if (!KidAdd(c->g, &ni)) FreeItem(&ni); ScheduleSave(); GrpRelayout(c, h); }
    } else if (r == 5) {
        WCHAR nm[128];
        lstrcpynW(nm, c->g->name, 128);
        if (InputBox(g_hset, L"应用组名称", L"名称：", nm, 128, NULL, NULL, 0) && nm[0]) { lstrcpynW(c->g->name, nm, 128); CleanName(c->g->name); ScheduleSave(); }
    } else if (r == 6 && k >= 0) { if (ActivateItem(&c->g->kids[k], FALSE)) c->done = 1; }
    else if (r == 7 && k >= 0) { ActivateItem(&c->g->kids[k], TRUE); GrpDraw(c, h); }
    else if (r == 8 && k >= 0) {
        Item *kd = &c->g->kids[k];
        WCHAR a[1200];
        if (kd->type == T_FILE) { wsprintfW(a, L"/select,\"%s\"", kd->path); ShellExecuteW(NULL, L"open", L"explorer.exe", a, NULL, SW_SHOWNORMAL); }
        else ShellExecuteW(NULL, L"open", kd->path, NULL, NULL, SW_SHOWNORMAL);
        c->done = 1;
    } else if (r == 9) {
        c->g->appmode ^= 4;
        ScheduleSave();
        GrpRelayout(c, h);
    }
    GrpDraw(c, h);
    Kick();
}

static void ShowGroup(int idx) {
    if (idx < 0 || idx >= nItems || items[idx].type != T_GROUP) return;
    Item *g = &items[idx];
    GrpCtx *c = (GrpCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(GrpCtx));
    if (!c) return;
    c->g = g; c->gi = idx; c->hover = -1; c->dragK = -1; c->rad = 22;
    GrpLayout(c);
    int W = (int)c->W + 2 * SHM, H = (int)c->H + 2 * SHM;
    GrpPlace(c, &c->px, &c->py);
    if (CanvasNew(&c->cv, W, H)) {
        HWND h = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CLS_GRP, L"", WS_POPUP, c->px, c->py, W, H, NULL, NULL, g_inst, NULL);
        g_grpWnd = h;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)c);
        if (cfg.groupFrost) {
            RECT fs;
            fs.left = c->px + SHM; fs.top = c->py + SHM;
            fs.right = fs.left + (int)c->W; fs.bottom = fs.top + (int)c->H;
            if (cfg.sbPopFrost) FrostSnapGrab(&c->frost, &fs);
        }
        c->alpha = 0; c->dy = 22; c->t0 = GetTickCount();
        GrpSync(c, h);
        GrpDraw(c, h);
        ShowWindow(h, SW_SHOWNA);
        gMenuOpen = TRUE;
        ClosePreview();
        SetTimer(h, 1, 50, NULL);
        SetTimer(h, 2, 15, NULL);
        GuardStart(RectInside, &c->scr, &c->done, h);
        MSG msg;
        while (!c->done && GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        GuardStop();
        gMenuOpen = FALSE;
        SetWindowLongPtrW(h, GWLP_USERDATA, 0);
        if (GetCapture() == h) ReleaseCapture();
        if (c->bk) DestroyWindow(c->bk);
        FrostSnapFree(&c->frost);
        DestroyWindow(h);
        g_grpWnd = NULL;
        CanvasFree(&c->cv);
    }
    HeapFree(GetProcessHeap(), 0, c);
    Kick();
}

/* ======================= app groups on their own strip =======================
   "应用组独立于 Dock 显示": with cfg.grpFloat the groups are also shown in a small always-on-top capsule
   next to the dock edge. It stays visible while the dock auto-hides, so a group is always one click away,
   just like the dock widgets. */
#define CLS_GPB L"MiniDockGroupBar"
typedef struct {
    Canvas cv;
    int W, H, hover, n;
    int idx[MAXI];
    RECT cell[MAXI];
    RECT scr;
} GpbCtx;
static HWND g_gpw;
static GpbCtx *g_gpb;
static DWORD g_gpbSig;

static void GpbPaint(void) {
    GpbCtx *c = g_gpb;
    if (!c || !c->cv.g || !c->cv.bits) return;
    GpGraphics *g = c->cv.g;
    GdipGraphicsClear(g, 0);
    float rad = Sc(15);
    int a = cfg.opacity < 30 ? 30 : (cfg.opacity > 100 ? 100 : cfg.opacity);
    int al = 255 * a / 100 * 88 / 100;
    FillRR(g, g_dark ? ARGBC(al, 24, 25, 30) : ARGBC(al, 248, 249, 253), 0, 0, (float)c->W, (float)c->H, rad);
    if (cfg.border) StrokeRR(g, g_dark ? ARGBC(90, 255, 255, 255) : ARGBC(70, 0, 0, 0), 1, 0, 0, (float)c->W, (float)c->H, rad);
    for (int k = 0; k < c->n; k++) {
        RECT *r = &c->cell[k];
        if (k == c->hover) FillRR(g, g_dark ? ARGBC(52, 255, 255, 255) : ARGBC(30, 0, 0, 0), (float)r->left, (float)r->top, (float)(r->right - r->left), (float)(r->bottom - r->top), Sc(10));
        DrawIconPlated(g, items[c->idx[k]].icon, (float)r->left + Sc(2), (float)r->top + Sc(2),
                       (float)(r->right - r->left) - 2 * Sc(2), (float)cfg.plateRadius);
    }
    CanvasPresentA(&c->cv, g_gpw, c->scr.left, c->scr.top, 255);
}
static LRESULT CALLBACK GpbProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    GpbCtx *c = (GpbCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    if (!c) return DefWindowProcW(h, m, w, l);
    int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
    switch (m) {
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_MOUSEMOVE: {
        int hv = -1;
        for (int k = 0; k < c->n; k++) if (x >= c->cell[k].left && x < c->cell[k].right && y >= c->cell[k].top && y < c->cell[k].bottom) hv = k;
        if (hv != c->hover) { c->hover = hv; GpbPaint(); }
        TRACKMOUSEEVENT te = { sizeof te, TME_LEAVE, h, 0 };
        TrackMouseEvent(&te);
        return 0;
    }
    case WM_MOUSELEAVE: if (c->hover != -1) { c->hover = -1; GpbPaint(); } return 0;
    case WM_LBUTTONUP: {
        for (int k = 0; k < c->n; k++)
            if (x >= c->cell[k].left && x < c->cell[k].right && y >= c->cell[k].top && y < c->cell[k].bottom) {
                g_grpAnchor.left = c->scr.left + c->cell[k].left;
                g_grpAnchor.top = c->scr.top + c->cell[k].top;
                g_grpAnchor.right = g_grpAnchor.left + (c->cell[k].right - c->cell[k].left);
                g_grpAnchor.bottom = g_grpAnchor.top + (c->cell[k].bottom - c->cell[k].top);
                g_grpAnchorOn = TRUE;
                int idx = c->idx[k];
                ShowGroup(idx);
                g_grpAnchorOn = FALSE;
                return 0;
            }
        return 0;
    }
    case WM_RBUTTONUP: {
        CmItem mi[3];
        int n = 0;
        n = CmAdd(mi, n, 1, L"应用组设置…", 0);
        n = CmAdd(mi, n, 2, L"不再独立显示应用组", 0);
        POINT p;
        GetCursorPos(&p);
        gMenuOpen = TRUE;
        int r = ShowMenu(mi, n, p.x, p.y);
        gMenuOpen = FALSE;
        if (r == 1) { ClosePreview(); ShowSettings(23); }
        else if (r == 2) { cfg.grpFloat = 0; ApplySettings(); }
        Kick();
        return 0;
    }
    }
    return DefWindowProcW(h, m, w, l);
}
static void GpbFree(void) {
    if (g_gpw) { DestroyWindow(g_gpw); g_gpw = NULL; }
    if (g_gpb) { CanvasFree(&g_gpb->cv); HeapFree(GetProcessHeap(), 0, g_gpb); g_gpb = NULL; }
    g_gpbSig = 0;
}
static void GrpBarSync(void) {
    if (!cfg.grpFloat || !g_hwnd || !g_barScr.valid) { if (!cfg.grpFloat) GpbFree(); return; }
    if (gMenuOpen) return;   /* a menu owns the screen: leave the strip where it is */
    int idx[MAXI], n = 0;
    for (int i = 0; i < nItems && n < MAXI; i++) if (items[i].type == T_GROUP && items[i].icon) idx[n++] = i;
    if (n == 0) { GpbFree(); return; }
    int isz = (int)(Pxs(cfg.iconSize) * 0.66f);
    if (isz < Sc(26)) isz = Sc(26);
    if (isz > Sc(64)) isz = Sc(64);
    int pad = Sc(7), gap = Sc(6);
    BOOL horiz = (cfg.edge == E_BOTTOM || cfg.edge == E_TOP);
    int W = horiz ? 2 * pad + n * isz + (n - 1) * gap : isz + 2 * pad;
    int H = horiz ? isz + 2 * pad : 2 * pad + n * isz + (n - 1) * gap;
    DWORD sig = 2166136261u;
    { int v[6] = { n, isz, cfg.border, g_dark, W, H }; for (int i = 0; i < 6; i++) { sig ^= (DWORD)v[i]; sig *= 16777619u; } }
    for (int k = 0; k < n; k++) { sig ^= (DWORD)idx[k]; sig *= 16777619u; }
    if (gFsTgt || gFsHidden) {   /* a fullscreen app owns the screen: the group strip steps aside too */
        if (g_gpw && IsWindowVisible(g_gpw)) ShowWindow(g_gpw, SW_HIDE);
        return;
    }
    if (g_gpw && !IsWindowVisible(g_gpw)) ShowWindow(g_gpw, SW_SHOWNA);
    RECT bar = g_barRest.right > g_barRest.left ? g_barRest : g_barScr.bar;
    int X, Y;
    if (horiz) { X = (bar.left + bar.right - W) / 2; Y = (cfg.edge == E_BOTTOM) ? bar.top - H - Sc(8) : bar.bottom + Sc(8); }
    else { X = (cfg.edge == E_LEFT) ? bar.right + Sc(8) : bar.left - W - Sc(8); Y = (bar.top + bar.bottom - H) / 2; }
    RECT wa = g_mi.rcWork;
    if (X + W > wa.right) X = wa.right - W;
    if (X < wa.left) X = wa.left;
    if (Y + H > wa.bottom) Y = wa.bottom - H;
    if (Y < wa.top) Y = wa.top;
    if (g_gpw && g_gpb && sig == g_gpbSig && g_gpb->scr.left == X && g_gpb->scr.top == Y) return;
    static BOOL reg;
    if (!reg) {
        WNDCLASSW wc;
        ZeroMemory(&wc, sizeof wc);
        wc.lpfnWndProc = GpbProc; wc.hInstance = g_inst; wc.lpszClassName = CLS_GPB; wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
        RegisterClassW(&wc);
        reg = TRUE;
    }
    if (g_gpw && g_gpb && (g_gpb->W != W || g_gpb->H != H)) {   /* size changed: rebuild the surface */
        DestroyWindow(g_gpw); g_gpw = NULL;
        CanvasFree(&g_gpb->cv);
        HeapFree(GetProcessHeap(), 0, g_gpb);
        g_gpb = NULL;
    }
    if (!g_gpw) {
        g_gpb = (GpbCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(GpbCtx));
        if (!g_gpb) return;
        g_gpb->W = W; g_gpb->H = H; g_gpb->hover = -1;
        if (!CanvasNew(&g_gpb->cv, W, H)) { HeapFree(GetProcessHeap(), 0, g_gpb); g_gpb = NULL; return; }
        g_gpw = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CLS_GPB, L"", WS_POPUP, X, Y, W, H, NULL, NULL, g_inst, NULL);
        if (!g_gpw) { CanvasFree(&g_gpb->cv); HeapFree(GetProcessHeap(), 0, g_gpb); g_gpb = NULL; return; }
        SetWindowLongPtrW(g_gpw, GWLP_USERDATA, (LONG_PTR)g_gpb);
        ShowWindow(g_gpw, SW_SHOWNA);
    } else SetWindowPos(g_gpw, HWND_TOPMOST, X, Y, W, H, SWP_NOACTIVATE);
    g_gpb->scr.left = X; g_gpb->scr.top = Y; g_gpb->scr.right = X + W; g_gpb->scr.bottom = Y + H;
    g_gpb->n = n;
    for (int k = 0; k < n; k++) {
        g_gpb->idx[k] = idx[k];
        RECT *r = &g_gpb->cell[k];
        if (horiz) { r->left = pad + k * (isz + gap); r->top = pad; r->right = r->left + isz; r->bottom = r->top + isz; }
        else { r->top = pad + k * (isz + gap); r->left = pad; r->bottom = r->top + isz; r->right = r->left + isz; }
    }
    GpbPaint();
    g_gpbSig = sig;
}

/* ======================= hover preview (DWM thumbnails) ======================= */
static HWND pvWin[10];
static HBITMAP pvShot[10];   /* PrintWindow snapshot per preview cell */
static int pvShotW[10], pvShotH[10];
static HBITMAP pvBg;   /* blurred backdrop for the window list */
static int pvBgW, pvBgH;
static HTHUMBNAIL pvTh[10];
static WCHAR pvTitle[10][96];
static RECT pvCell[10], pvClose[10];
static int pvN, pvHot = -1, pvCloseHot = -1, pvList;

typedef struct { const Item *it; DWORD key; BOOL isUrl; BOOL exact; WCHAR exe[MAX_PATH]; int n; } PvCtx;
static const WCHAR *ItemExeHint(const Item *it) {
    if (it->target[0]) return it->target;
    if (it->type == T_FILE && PathMatchSpecW(it->path, L"*.exe")) return it->path;
    return NULL;
}
static BOOL CALLBACK PvEnum(HWND h, LPARAM lp) {
    PvCtx *c = (PvCtx *)lp;
    if (c->n >= 10 || !IsAppWindow(h) || h == g_hwnd) return TRUE;
    if (c->isUrl) { DWORD k; if (BoundKey(h, &k) < 0 || k != c->key) return TRUE; }
    else {
        WinId w; WinIdOf(h, &w);
        if (!ItemMatchAny(c->it, &w)) return TRUE;
        if (c->exact && c->exe[0] && lstrcmpiW(w.exe, c->exe)) return TRUE;
    }
    pvWin[c->n] = h;
    GetWindowTextW(h, pvTitle[c->n], 96);
    c->n++;
    return TRUE;
}

void ClosePreview(void) {
    for (int i = 0; i < 10; i++) {
        if (pvTh[i]) { DwmUnregisterThumbnail(pvTh[i]); pvTh[i] = NULL; }
        if (pvShot[i]) { DeleteObject(pvShot[i]); pvShot[i] = NULL; }
        pvShotW[i] = pvShotH[i] = 0;
    }
    if (g_pv) { HWND h = g_pv; g_pv = NULL; DestroyWindow(h); }
    pvItem = -1; pvN = 0; pvHot = -1; pvCloseHot = -1; pvList = 0;
    if (pvBg) { DeleteObject(pvBg); pvBg = NULL; pvBgW = pvBgH = 0; }
    Kick();
}

static LRESULT CALLBACK PvProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        RECT rc;
        GetClientRect(h, &rc);
        if (pvList) {
            if (pvBg) {
                HDC bdc = CreateCompatibleDC(dc);
                HGDIOBJ ob = SelectObject(bdc, pvBg);
                SetStretchBltMode(dc, HALFTONE);
                StretchBlt(dc, 0, 0, rc.right, rc.bottom, bdc, 0, 0, pvBgW, pvBgH, SRCCOPY);
                SelectObject(bdc, ob);
                DeleteDC(bdc);
                HBRUSH tint = CreateSolidBrush(g_dark ? RGB(40, 42, 48) : RGB(250, 250, 252));
                RECT ar = rc;
                int alphaSave = 0; (void)alphaSave;
                HDC tdc = CreateCompatibleDC(dc);
                HBITMAP tbm = CreateCompatibleBitmap(dc, 1, 1);
                HGDIOBJ tob = SelectObject(tdc, tbm);
                RECT one = { 0, 0, 1, 1 };
                FillRect(tdc, &one, tint);
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)(g_dark ? 200 : 205), 0 };
                AlphaBlend(dc, 0, 0, rc.right, rc.bottom, tdc, 0, 0, 1, 1, bf);
                SelectObject(tdc, tob); DeleteObject(tbm); DeleteDC(tdc);
                DeleteObject(tint);
            }
            SetBkMode(dc, TRANSPARENT);
            SelectObject(dc, g_uiFont);
            SetTextColor(dc, g_dark ? RGB(238, 238, 243) : RGB(30, 30, 34));
            for (int i = 0; i < pvN; i++) {
                RECT r = pvCell[i];
                if (i == pvHot) {
                    HBRUSH hb = CreateSolidBrush(g_dark ? RGB(62, 64, 74) : RGB(226, 230, 240));
                    HPEN pn = (HPEN)GetStockObject(NULL_PEN);
                    HGDIOBJ ob = SelectObject(dc, hb), op2 = SelectObject(dc, pn);
                    RoundRect(dc, r.left, r.top, r.right, r.bottom, Sc(8), Sc(8));
                    SelectObject(dc, ob); SelectObject(dc, op2);
                    DeleteObject(hb);
                }
                HICON wi = (HICON)SendMessageW(pvWin[i], WM_GETICON, ICON_SMALL2, 0);
                if (!wi) wi = (HICON)SendMessageW(pvWin[i], WM_GETICON, ICON_SMALL, 0);
                if (!wi) wi = (HICON)GetClassLongPtrW(pvWin[i], GCLP_HICONSM);
                int iz = Sc(16), ix = r.left + Sc(10), iy = (r.top + r.bottom) / 2 - iz / 2;
                if (wi) DrawIconEx(dc, ix, iy, wi, iz, iz, 0, NULL, DI_NORMAL);
                else {
                    HBRUSH ib = CreateSolidBrush(g_dark ? RGB(120, 126, 140) : RGB(150, 156, 170));
                    HPEN ip = (HPEN)GetStockObject(NULL_PEN);
                    HGDIOBJ ob2 = SelectObject(dc, ib), op3 = SelectObject(dc, ip);
                    RoundRect(dc, ix, iy, ix + iz, iy + iz, Sc(4), Sc(4));
                    SelectObject(dc, ob2); SelectObject(dc, op3);
                    DeleteObject(ib);
                }
                RECT tr = { r.left + Sc(34), r.top, r.right - Sc(8), r.bottom };
                DrawTextW(dc, pvTitle[i], -1, &tr, DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
            }
            EndPaint(h, &ps);
            return 0;
        }        HBRUSH bg = CreateSolidBrush(g_dark ? RGB(36, 37, 43) : RGB(246, 247, 251));
        FillRect(dc, &rc, bg);
        DeleteObject(bg);
        SetBkMode(dc, TRANSPARENT);
        SelectObject(dc, g_uiFont);
        SetTextColor(dc, g_dark ? RGB(238, 238, 243) : RGB(30, 30, 34));
        for (int i = 0; i < pvN; i++) {
            if (i == pvHot) {
                HBRUSH hb = CreateSolidBrush(g_dark ? RGB(62, 64, 74) : RGB(222, 226, 238));
                HPEN pn = (HPEN)GetStockObject(NULL_PEN);
                SelectObject(dc, hb); SelectObject(dc, pn);
                RoundRect(dc, pvCell[i].left - Sc(4), pvCell[i].top - Sc(4), pvCell[i].right + Sc(4), pvCell[i].bottom + Sc(4), Sc(12), Sc(12));
                DeleteObject(hb);
            }
            {   /* close button, top right of every window cell */
                RECT cr = pvClose[i];
                int cx = (cr.left + cr.right) / 2, cy = (cr.top + cr.bottom) / 2, rr = Sc(8);
                if (i == pvCloseHot) {
                    HBRUSH hb = CreateSolidBrush(RGB(232, 76, 70));
                    SelectObject(dc, hb); SelectObject(dc, GetStockObject(NULL_PEN));
                    Ellipse(dc, cx - rr - Sc(2), cy - rr - Sc(2), cx + rr + Sc(2), cy + rr + Sc(2));
                    DeleteObject(hb);
                }
                HPEN xp = CreatePen(PS_SOLID, 2, i == pvCloseHot ? RGB(255, 255, 255) : (g_dark ? RGB(190, 192, 202) : RGB(100, 104, 118)));
                HGDIOBJ op = SelectObject(dc, xp);
                int a = Sc(4);
                MoveToEx(dc, cx - a, cy - a, NULL); LineTo(dc, cx + a + 1, cy + a + 1);
                MoveToEx(dc, cx - a, cy + a, NULL); LineTo(dc, cx + a + 1, cy - a - 1);
                SelectObject(dc, op); DeleteObject(xp);
            }
            {   /* snapshot of the real window content (replaces the DWM thumbnail) */
                RECT th2 = pvCell[i];
                th2.top += Sc(24);
                th2.bottom -= Sc(24);
                InflateRect(&th2, -Sc(4), -Sc(4));
                if (pvShot[i]) {
                    HDC mdc = CreateCompatibleDC(dc);
                    HGDIOBJ ob = SelectObject(mdc, pvShot[i]);
                    SetStretchBltMode(dc, HALFTONE);
                    SetBrushOrgEx(dc, 0, 0, NULL);
                    StretchBlt(dc, th2.left, th2.top, th2.right - th2.left, th2.bottom - th2.top,
                               mdc, 0, 0, pvShotW[i], pvShotH[i], SRCCOPY);
                    SelectObject(mdc, ob);
                    DeleteDC(mdc);
                } else {
                    HICON wi = (HICON)SendMessageW(pvWin[i], WM_GETICON, ICON_SMALL2, 0);
                    if (!wi) wi = (HICON)GetClassLongPtrW(pvWin[i], GCLP_HICONSM);
                    int iz = Sc(48);
                    int ix = (th2.left + th2.right) / 2 - iz / 2, iy = (th2.top + th2.bottom) / 2 - iz / 2;
                    if (wi) DrawIconEx(dc, ix, iy, wi, iz, iz, 0, NULL, DI_NORMAL);
                }
            }
            RECT tr = { pvCell[i].left, pvCell[i].bottom - Sc(22), pvCell[i].right, pvCell[i].bottom };
            DrawTextW(dc, pvTitle[i], -1, &tr, DT_SINGLELINE | DT_END_ELLIPSIS | DT_CENTER | DT_VCENTER);
        }
        EndPaint(h, &ps);
        return 0;
    }
    case WM_MOUSEMOVE: {
        POINT p = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        int hv = -1;
        int ch2 = -1;
        for (int i = 0; i < pvN; i++) { if (PtInRect(&pvCell[i], p)) hv = i; if (PtInRect(&pvClose[i], p)) ch2 = i; }
        if (hv != pvHot || ch2 != pvCloseHot) { pvHot = hv; pvCloseHot = ch2; InvalidateRect(h, NULL, TRUE); }
        return 0;
    }
    case WM_LBUTTONUP: {
        POINT p = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        for (int i = 0; i < pvN; i++)
            if (PtInRect(&pvClose[i], p)) { PostMessageW(pvWin[i], WM_CLOSE, 0, 0); ClosePreview(); return 0; }
        for (int i = 0; i < pvN; i++)
            if (PtInRect(&pvCell[i], p)) {
                HWND t = pvWin[i];
                if (IsIconic(t)) ShowWindow(t, SW_RESTORE);
                SetForegroundWindow(t);
                break;
            }
        ClosePreview();
        return 0;
    }
    case WM_MBUTTONUP: {
        POINT p = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        for (int i = 0; i < pvN; i++) if (PtInRect(&pvCell[i], p)) PostMessageW(pvWin[i], WM_CLOSE, 0, 0);
        ClosePreview();
        return 0;
    }
    }
    return DefWindowProcW(h, m, w, l);
}

static void ShowPreviewList(int idx, int n) {
    if (g_pv || n <= 0) return;
    int rowH = Sc(30), pad = Sc(10), W = Sc(300), H = pad * 2 + n * rowH;
    RECT ir;
    IconScreenRect(idx, &ir);
    int px, py;
    switch (cfg.edge) {
    case E_TOP: px = (ir.left + ir.right) / 2 - W / 2; py = ir.bottom + 10; break;
    case E_LEFT: px = ir.right + 10; py = (ir.top + ir.bottom) / 2 - H / 2; break;
    case E_RIGHT: px = ir.left - W - 10; py = (ir.top + ir.bottom) / 2 - H / 2; break;
    default: px = (ir.left + ir.right) / 2 - W / 2; py = ir.top - H - 10; break;
    }
    RECT mr = g_mi.rcMonitor;
    if (px + W > mr.right) px = mr.right - W;
    if (py + H > mr.bottom) py = mr.bottom - H;
    if (px < mr.left) px = mr.left;
    if (py < mr.top) py = mr.top;
    /* blurred backdrop behind the card, so the list gets the same frosted look as the menus */
    { HDC sdc = GetDC(NULL);
      HDC mdc = CreateCompatibleDC(sdc);
      BITMAPINFO bi; ZeroMemory(&bi, sizeof bi);
      bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER); bi.bmiHeader.biWidth = W; bi.bmiHeader.biHeight = -H; bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
      void *bits = NULL;
      HBITMAP bm = CreateDIBSection(mdc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
      if (bm) { HGDIOBJ ob = SelectObject(mdc, bm); BitBlt(mdc, 0, 0, W, H, sdc, px, py, SRCCOPY); SelectObject(mdc, ob); pvBg = bm; pvBgW = W; pvBgH = H; }
      DeleteDC(mdc); ReleaseDC(NULL, sdc); }
    HWND h = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CLS_PV, L"", WS_POPUP, px, py, W, H, NULL, NULL, g_inst, NULL);
    if (!h) return;
    g_pv = h;
    pvList = 1; pvN = n;
    SetWindowRgn(h, CreateRoundRectRgn(0, 0, W + 1, H + 1, Sc(12), Sc(12)), TRUE);
    for (int i = 0; i < n && i < 10; i++) {
        pvCell[i].left = pad; pvCell[i].top = pad + i * rowH;
        pvCell[i].right = W - pad; pvCell[i].bottom = pvCell[i].top + rowH;
        pvClose[i].left = pvClose[i].right = pvClose[i].top = pvClose[i].bottom = 0;
        pvTh[i] = NULL;
    }
    pvItem = idx;
    ShowWindow(h, SW_SHOWNA);
    Kick();
}

static void ShowPreview(int idx) {
    if (g_pv || idx < 0 || idx >= nItems || !(HasWinKey(&items[idx]) || (items[idx].type == T_URL && items[idx].appmode))) return;
    PvCtx c = { &items[idx], 0, items[idx].type == T_URL, 0, L"", 0 };
    if (c.isUrl) c.key = ItemKey(&items[idx]);
    else {
        const WCHAR *hint = ItemExeHint(&items[idx]);
        if (hint) { lstrcpynW(c.exe, hint, MAX_PATH); c.exact = TRUE; }
    }
    EnumWindows(PvEnum, (LPARAM)&c);
    if (c.n == 0 && c.exact) { c.exact = FALSE; c.n = 0; EnumWindows(PvEnum, (LPARAM)&c); }
    if (c.n == 0) return;
    if (c.n > 1 && cfg.pvList) { ShowPreviewList(idx, c.n); return; }
    pvN = c.n;
    int tw = Sc(200), th = Sc(125), lab = Sc(24), gap = Sc(12), pad = Sc(12), hdr = Sc(24);
    if (pvN > 4) { tw = Sc(160); th = Sc(100); gap = Sc(8); }   /* up to 10 cells must still fit on screen */
    {   int maxW = (g_mi.rcMonitor.right - g_mi.rcMonitor.left) - Sc(24);
        while (tw > Sc(90) && (pad * 2 + pvN * tw + (pvN - 1) * gap + Sc(8)) > maxW) { tw -= Sc(6); th = tw * 125 / 200; } }
    int W = pad * 2 + pvN * tw + (pvN - 1) * gap + Sc(8), H = pad * 2 + hdr + th + lab + Sc(8);
    RECT ir;
    IconScreenRect(idx, &ir);
    int px, py;
    switch (cfg.edge) {
    case E_TOP: px = (ir.left + ir.right) / 2 - W / 2; py = ir.bottom + 10; break;
    case E_LEFT: px = ir.right + 10; py = (ir.top + ir.bottom) / 2 - H / 2; break;
    case E_RIGHT: px = ir.left - W - 10; py = (ir.top + ir.bottom) / 2 - H / 2; break;
    default: px = (ir.left + ir.right) / 2 - W / 2; py = ir.top - H - 10; break;
    }
    RECT mr = g_mi.rcMonitor;
    if (px + W > mr.right) px = mr.right - W;
    if (py + H > mr.bottom) py = mr.bottom - H;
    if (px < mr.left) px = mr.left;
    if (py < mr.top) py = mr.top;
    HWND h = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CLS_PV, L"", WS_POPUP, px, py, W, H, NULL, NULL, g_inst, NULL);
    if (!h) { pvN = 0; return; }
    g_pv = h;
    SetWindowRgn(h, CreateRoundRectRgn(0, 0, W + 1, H + 1, Sc(22), Sc(22)), TRUE);
    int thumbs = 0;
    for (int i = 0; i < pvN; i++) {
        int x = pad + Sc(4) + i * (tw + gap), y = pad + Sc(4);
        pvCell[i].left = x; pvCell[i].top = y; pvCell[i].right = x + tw; pvCell[i].bottom = y + hdr + th + lab;
        pvClose[i].left = x + tw - hdr + Sc(2); pvClose[i].top = y; pvClose[i].right = x + tw; pvClose[i].bottom = y + hdr;
        pvTh[i] = NULL;
        pvShot[i] = NULL; pvShotW[i] = pvShotH[i] = 0;
        /* DWM thumbnail first: it is the live window content, scaled correctly inside the cell.
           PrintWindow is only a fallback (it often captures a partially painted frame -> black bars). */
        if (!pvTh[i] && SUCCEEDED(DwmRegisterThumbnail(h, pvWin[i], &pvTh[i]))) {
            SIZE ss = { 0, 0 };
            DwmQueryThumbnailSourceSize(pvTh[i], &ss);
            int dw = tw, dh = th;
            if (ss.cx > 0 && ss.cy > 0) {
                float sc2 = (float)tw / ss.cx;
                if (ss.cy * sc2 > th) sc2 = (float)th / ss.cy;
                dw = (int)(ss.cx * sc2); dh = (int)(ss.cy * sc2);
            }
            DWM_THUMBNAIL_PROPERTIES tp;
            ZeroMemory(&tp, sizeof tp);
            tp.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
            tp.rcDestination.left = x + (tw - dw) / 2; tp.rcDestination.top = y + hdr + (th - dh) / 2;
            tp.rcDestination.right = tp.rcDestination.left + dw; tp.rcDestination.bottom = tp.rcDestination.top + dh;
            tp.fVisible = TRUE; tp.opacity = 255;
            DwmUpdateThumbnailProperties(pvTh[i], &tp);
            thumbs++;
        }
        if (!pvTh[i] && !IsIconic(pvWin[i])) {
            RECT wr;
            if (GetWindowRect(pvWin[i], &wr)) {
                int cw = wr.right - wr.left, chh = wr.bottom - wr.top;
                if (cw > 8 && chh > 8) {
                    if (cw > 1600) { chh = chh * 1600 / cw; cw = 1600; }
                    HDC sdc = GetDC(NULL);
                    HDC mdc = CreateCompatibleDC(sdc);
                    ReleaseDC(NULL, sdc);
                    BITMAPINFO bi;
                    ZeroMemory(&bi, sizeof bi);
                    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                    bi.bmiHeader.biWidth = cw; bi.bmiHeader.biHeight = -chh;
                    bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32; bi.bmiHeader.biCompression = BI_RGB;
                    void *bits = NULL;
                    HBITMAP bm = CreateDIBSection(mdc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
                    if (bm) {
                        HGDIOBJ old = SelectObject(mdc, bm);
                        if (PrintWindow(pvWin[i], mdc, 2)) { pvShot[i] = bm; pvShotW[i] = cw; pvShotH[i] = chh; thumbs++; }
                        else DeleteObject(bm);
                        SelectObject(mdc, old);
                    }
                    DeleteDC(mdc);
                }
            }
        }
    }
    /* never abort: cells without a snapshot or thumbnail still show the window icon + title */
    pvItem = idx;
    ShowWindow(h, SW_SHOWNA);
    Kick();
}

/* ======================= OLE drop target ======================= */
static UINT cfUrlW, cfUrlA, cfShellIDList;
static BOOL g_dropOK, g_dropFiles;

static BOOL HasFmt(IDataObject *d, UINT cf) {
    FORMATETC fe = { (CLIPFORMAT)cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    return IDataObject_QueryGetData(d, &fe) == S_OK;
}
static BOOL AcceptData(IDataObject *d) {
    return HasFmt(d, CF_HDROP) || HasFmt(d, cfShellIDList) || HasFmt(d, cfUrlW) || HasFmt(d, cfUrlA) || HasFmt(d, CF_UNICODETEXT);
}

static int CenterHit(float u) {
    for (int i = 0; i < nItems; i++) {
        Item *it = &items[i];
        if ((it->type == T_FILE && PathMatchSpecW(it->path, L"*.exe;*.lnk")) || it->type == T_GROUP)
            if (it->cur > M.S * 0.5f && fabsf(u - actC[i]) < it->cur * Asp(it) * 0.3f) return i;
    }
    return -1;
}

static int g_gapCand = -2, g_gapCnt;
static BOOL g_dtOver;

static void DropPoint(POINTL pt, BOOL files) {
    POINT p = { pt.x, pt.y };
    ScreenToClient(g_hwnd, &p);
    float u, v;
    ToLogical((float)p.x, (float)p.y, &u, &v);
    int on = -1;
    if (files) {
        /* the Recycle Bin only accepts a drop on its own icon (dropping next to it still adds a shortcut) */
        for (int i = 0; i < nItems && on < 0; i++)
            if (items[i].type == T_TRASH && items[i].cur > M.S * 0.5f && fabsf(u - actC[i]) < items[i].cur * 0.5f && v >= barTop - M.Z - 8) on = i;
        for (int i = 0; i < nItems && on < 0; i++)   /* a group takes a drop anywhere on its icon (wide ones too) */
            if (items[i].type == T_GROUP && items[i].cur > M.S * 0.5f && fabsf(u - actC[i]) < items[i].cur * Asp(&items[i]) * 0.5f + 4) on = i;
        if (on < 0) on = CenterHit(u);
    }
    if (on >= 0) { if (mergeT != on) { mergeT = on; Kick(); } if (gapIdx != -1) { gapIdx = -1; Kick(); } g_gapCand = -2; return; }
    if (mergeT != -1) { mergeT = -1; Kick(); }
    float G = (gapCur > 0.5f) ? gapCur + M.g : 0;
    int idx = 0, pc = PinnedCount();
    for (int i = 0; i < nItems; i++) {
        float c = actC[i];
        if (G > 0) c += (gapIdx >= 0 && i >= gapIdx) ? -G / 2 : G / 2;
        if (c < u) idx = i + 1;
    }
    if (idx > pc) idx = pc;
    if (gapIdx < 0) { gapIdx = idx; g_gapCand = -2; Kick(); }
    else if (idx != gapIdx) {   /* hysteresis: the new slot must be seen twice in a row, so the gap never flickers */
        if (idx == g_gapCand) { if (++g_gapCnt >= 2) { gapIdx = idx; g_gapCand = -2; Kick(); } }
        else { g_gapCand = idx; g_gapCnt = 1; }
    } else g_gapCand = -2;
}

static HRESULT STDMETHODCALLTYPE DT_QI(IDropTarget *t, REFIID riid, void **ppv) {
    if (IsEqualIID(riid, &IID_IUnknown) || IsEqualIID(riid, &IID_IDropTarget)) { *ppv = t; return S_OK; }
    *ppv = NULL;
    return E_NOINTERFACE;
}
static ULONG STDMETHODCALLTYPE DT_AddRef(IDropTarget *t) { return 2; }
static ULONG STDMETHODCALLTYPE DT_Release(IDropTarget *t) { return 1; }
static HRESULT STDMETHODCALLTYPE DT_Enter(IDropTarget *t, IDataObject *d, DWORD k, POINTL pt, DWORD *eff) {
    g_dropOK = AcceptData(d);
    g_dropFiles = HasFmt(d, CF_HDROP);
    g_dtOver = TRUE; g_dtActive = TRUE;
    KillTimer(g_hwnd, T_DTLEAVE);
    if (g_dropOK) { DropPoint(pt, g_dropFiles); *eff = (mergeT >= 0 && items[mergeT].type == T_TRASH) ? DROPEFFECT_MOVE : DROPEFFECT_LINK; Kick(); } else *eff = DROPEFFECT_NONE;
    return S_OK;
}
static HRESULT STDMETHODCALLTYPE DT_Over(IDropTarget *t, DWORD k, POINTL pt, DWORD *eff) {
    g_dtOver = TRUE;
    KillTimer(g_hwnd, T_DTLEAVE);
    if (g_dropOK) { DropPoint(pt, g_dropFiles); *eff = (mergeT >= 0 && items[mergeT].type == T_TRASH) ? DROPEFFECT_MOVE : DROPEFFECT_LINK; } else *eff = DROPEFFECT_NONE;
    return S_OK;
}
static HRESULT STDMETHODCALLTYPE DT_Leave(IDropTarget *t) {
    g_dtOver = FALSE;
    SetTimer(g_hwnd, T_DTLEAVE, 160, NULL);   /* delayed, so skimming over the edge of the bar does not make the gap flicker */
    return S_OK;
}

static BOOL GetGlobalText(IDataObject *d, UINT cf, BOOL wide, WCHAR *out, int cch) {
    FORMATETC fe = { (CLIPFORMAT)cf, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM sm;
    if (IDataObject_GetData(d, &fe, &sm) != S_OK) return FALSE;
    BOOL ok = FALSE;
    void *p = GlobalLock(sm.hGlobal);
    if (p) {
        if (wide) lstrcpynW(out, (WCHAR *)p, cch); else MultiByteToWideChar(CP_ACP, 0, (char *)p, -1, out, cch);
        GlobalUnlock(sm.hGlobal);
        ok = out[0] != 0;
    }
    ReleaseStgMedium(&sm);
    return ok;
}

static HRESULT STDMETHODCALLTYPE DT_Drop(IDropTarget *t, IDataObject *d, DWORD k, POINTL pt, DWORD *eff) {
    int idx = gapIdx >= 0 ? gapIdx : PinnedCount();
    int on = mergeT;
    float startCur = gapCur;
    int first = idx;
    BOOL any = FALSE;
    FORMATETC fe = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM sm;
    if (IDataObject_GetData(d, &fe, &sm) == S_OK) {
        HDROP hd = (HDROP)GlobalLock(sm.hGlobal);
        if (hd) {
            UINT cnt = DragQueryFileW(hd, 0xFFFFFFFF, NULL, 0);
            if (on >= 0 && on < nItems && items[on].type == T_TRASH) {   /* move to the Recycle Bin (asks for confirmation) */
                WCHAR *list = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 65536 * sizeof(WCHAR));
                if (list) {
                    size_t pos = 0;
                    for (UINT i = 0; i < cnt; i++) {
                        WCHAR f[1024];
                        DragQueryFileW(hd, i, f, 1024);
                        size_t l = lstrlenW(f) + 1;
                        if (pos + l + 2 < 65536) { memcpy(list + pos, f, l * sizeof(WCHAR)); pos += l; }
                    }
                    PostMessageW(g_hwnd, WM_APP_TRASH, 0, (LPARAM)list);
                }
                any = TRUE; on = -2;
            } else if (on >= 0 && on < nItems && items[on].type == T_FILE) { /* drop files onto an app icon: open them with it */
                WCHAR params[3000] = L"";
                for (UINT i = 0; i < cnt; i++) {
                    WCHAR f[1024];
                    DragQueryFileW(hd, i, f, 1024);
                    if (lstrlenW(params) + lstrlenW(f) + 4 < 2990) { lstrcatW(params, L"\""); lstrcatW(params, f); lstrcatW(params, L"\" "); }
                }
                WCHAR dir[MAX_PATH];
                lstrcpynW(dir, items[on].path, MAX_PATH);
                PathRemoveFileSpecW(dir);
                ShellExecuteW(NULL, L"open", items[on].target[0] ? items[on].target : items[on].path, params, dir, SW_SHOWNORMAL);
                if (cfg.bounce) items[on].bounce = GetTickCount() | 1;
                any = TRUE; on = -2;
            } else if (on >= 0 && on < nItems && items[on].type == T_GROUP) {
                for (UINT i = 0; i < cnt; i++) {
                    WCHAR f[1024];
                    DragQueryFileW(hd, i, f, 1024);
                    Item kid;
                    if (BuildItem(&kid, f)) { if (!KidAdd(&items[on], &kid)) FreeItem(&kid); }
                }
                ScheduleSave();
                any = TRUE; on = -2;
            } else {
                for (UINT i = 0; i < cnt; i++) {
                    WCHAR f[1024];
                    DragQueryFileW(hd, i, f, 1024);
                    int r = AddFromString(f, idx);
                    if (r >= 0) { idx = r + 1; any = TRUE; }
                }
            }
            GlobalUnlock(sm.hGlobal);
        }
        ReleaseStgMedium(&sm);
    } else {
        WCHAR s[1024];
        if (GetGlobalText(d, cfUrlW, TRUE, s, 1024) || GetGlobalText(d, cfUrlA, FALSE, s, 1024) || GetGlobalText(d, CF_UNICODETEXT, TRUE, s, 1024)) {
            WCHAR *nl = wcspbrk(s, L"\r\n");
            if (nl) *nl = 0;
            int r = AddFromString(s, idx);
            if (r >= 0) { idx = r + 1; any = TRUE; }
        }
    }
    if (!any && on != -2 && HasFmt(d, cfShellIDList)) {   /* Start-menu items (apps, Edge/Chrome web apps) */
        FORMATETC fe2 = { (CLIPFORMAT)cfShellIDList, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
        STGMEDIUM sm2;
        if (IDataObject_GetData(d, &fe2, &sm2) == S_OK) {
            CIDA *ci = (CIDA *)GlobalLock(sm2.hGlobal);
            if (ci) {
                LPCITEMIDLIST parent = (LPCITEMIDLIST)((BYTE *)ci + ci->aoffset[0]);
                for (UINT i = 0; i < ci->cidl; i++) {
                    LPCITEMIDLIST child = (LPCITEMIDLIST)((BYTE *)ci + ci->aoffset[i + 1]);
                    LPITEMIDLIST full = ILCombine(parent, child);
                    if (full) {
                        Item it;
                        if (BuildFromPidl(&it, full)) {
                            int r = InsertItemStruct(idx, &it, FALSE);
                            if (r >= 0) { idx = r + 1; any = TRUE; ScheduleSave(); }
                        }
                        ILFree(full);
                    }
                }
                GlobalUnlock(sm2.hGlobal);
            }
            ReleaseStgMedium(&sm2);
        }
    }
    gapIdx = -1; mergeT = -1; gapCur = 0; g_dtActive = FALSE; g_dtOver = FALSE; g_gapCand = -2;
    KillTimer(g_hwnd, T_DTLEAVE);
    if (any) {
        if (on != -2) for (int i = first; i < idx && i < nItems; i++) items[i].cur = startCur / (idx - first);
        *eff = DROPEFFECT_LINK;
        UpdateGeometry();
    } else { *eff = DROPEFFECT_NONE; PostMessageW(g_hwnd, WM_APP_FAIL, 0, 0); }
    Kick();
    return S_OK;
}

static IDropTargetVtbl g_dtVtbl = { DT_QI, DT_AddRef, DT_Release, DT_Enter, DT_Over, DT_Leave, DT_Drop };
static IDropTarget g_dt = { &g_dtVtbl };

/* ======================= folder "fan" (mac style stack) ======================= */
typedef struct { WCHAR name[96]; WCHAR path[MAX_PATH]; FILETIME ft; } FanLite;
typedef struct {
    Canvas cv;
    int n, hover, done, px, py, press, alpha, inMenu;
    POINT down;
    DWORD t0;
    WCHAR name[13][96], path[13][MAX_PATH];
    GpBitmap *icon[13];
    float cx[13], cy[13], ang[13], isz, lw[13], lx[13], ly[13];
} FanCtx;

static int FanCmp(const void *a, const void *b) { return CompareFileTime(&((const FanLite *)b)->ft, &((const FanLite *)a)->ft); }

static void FanDraw(FanCtx *c, HWND h) {
    GpGraphics *g = c->cv.g;
    GdipGraphicsClear(g, 0);
    for (int k = c->n - 1; k >= 0; k--) {
        float sz = c->isz * (k == c->hover ? 1.14f : 1.f);
        if (c->icon[k]) DrawImg(g, c->icon[k], c->cx[k] - sz / 2, c->cy[k] - sz / 2, sz, sz, FALSE);
        {
            int fb = g_dark ? cfg.fanBgD : cfg.fanBgL, ff = g_dark ? cfg.fanFgD : cfg.fanFgL;
            ARGB bgc = fb >= 0 ? ARGBC(238, GetRValue(fb), GetGValue(fb), GetBValue(fb)) : (g_dark ? ARGBC(232, 36, 37, 42) : ARGBC(238, 248, 248, 251));
            ARGB fgc = ff >= 0 ? ARGBC(255, GetRValue(ff), GetGValue(ff), GetBValue(ff)) : (g_dark ? ARGBC(255, 246, 246, 248) : ARGBC(255, 28, 28, 34));
            FillRR(g, k == c->hover ? ARGBC(255, 10, 132, 255) : bgc, c->lx[k], c->ly[k], c->lw[k], 28, 10);
            if (k != c->hover && !g_dark && fb < 0) StrokeRR(g, ARGBC(46, 0, 0, 0), 1, c->lx[k], c->ly[k], c->lw[k], 28, 10);
            DrawCentered(g, c->name[k], g_font, c->lx[k], c->ly[k], c->lw[k], 28, k == c->hover ? ARGBC(255, 255, 255, 255) : fgc);
        }
    }
    CanvasPresentA(&c->cv, h, c->px, c->py, c->alpha);
}

static int FanHit(FanCtx *c, int x, int y) {
    for (int k = 0; k < c->n; k++) {
        float hs = c->isz / 2;
        if (x >= c->cx[k] - hs && x <= c->cx[k] + hs && y >= c->cy[k] - hs && y <= c->cy[k] + hs) return k;
        if (x >= c->lx[k] && x <= c->lx[k] + c->lw[k] && y >= c->ly[k] && y <= c->ly[k] + 28) return k;
    }
    return -1;
}

static BOOL FanInside(POINT p, void *ctx) { FanCtx *c = (FanCtx *)ctx; return FanHit(c, p.x - c->px, p.y - c->py) >= 0; }

/* drag a file out of the fan (copy by default, move when "cut" is chosen in settings) */
static HRESULT STDMETHODCALLTYPE DS_QI(IDropSource *t, REFIID r, void **p) {
    if (IsEqualIID(r, &IID_IUnknown) || IsEqualIID(r, &IID_IDropSource)) { *p = t; return S_OK; }
    *p = NULL;
    return E_NOINTERFACE;
}
static ULONG STDMETHODCALLTYPE DS_AddRef(IDropSource *t) { return 2; }
static ULONG STDMETHODCALLTYPE DS_Release(IDropSource *t) { return 1; }
static HRESULT STDMETHODCALLTYPE DS_Query(IDropSource *t, BOOL esc, DWORD ks) {
    if (esc) return DRAGDROP_S_CANCEL;
    if (!(ks & MK_LBUTTON)) return DRAGDROP_S_DROP;
    return S_OK;
}
static HRESULT STDMETHODCALLTYPE DS_Feedback(IDropSource *t, DWORD e) { return DRAGDROP_S_USEDEFAULTCURSORS; }
static IDropSourceVtbl g_dsVtbl = { DS_QI, DS_AddRef, DS_Release, DS_Query, DS_Feedback };
static IDropSource g_ds = { &g_dsVtbl };

static void FanDragOut(const WCHAR *path, HWND hide) {
    /* canonical shell data object for the file (works with Explorer, the desktop, chat windows ...) */
    IDataObject *pdo = NULL;
    IShellItem *psi = NULL;
    if (SUCCEEDED(SHCreateItemFromParsingName(path, NULL, &IID_IShellItem, (void **)&psi)) && psi) {
        IShellItem_BindToHandler(psi, NULL, &BHID_DataObject, &IID_IDataObject, (void **)&pdo);
        IShellItem_Release(psi);
    }
    LPITEMIDLIST pidl = NULL;
    if (!pdo && (pidl = ILCreateFromPathW(path)) != NULL)
        SHCreateDataObject(NULL, 1, (LPCITEMIDLIST *)&pidl, NULL, &IID_IDataObject, (void **)&pdo);
    if (pdo) {
        if (hide) ShowWindow(hide, SW_HIDE);   /* the popup must not sit between the cursor and the drop target */
        DWORD eff = 0;
        DoDragDrop(pdo, &g_ds, cfg.fanCut ? DROPEFFECT_MOVE : DROPEFFECT_COPY, &eff);
        IDataObject_Release(pdo);
    }
    if (pidl) ILFree(pidl);
}

static void FanMenu(FanCtx *c, int k) {
    if (k < 0 || k >= c->n - 1) return;
    CmItem it[6];
    int n = 0;
    n = CmAdd(it, n, 1, L"打开", 0);
    n = CmAdd(it, n, 2, L"在资源管理器中显示", 0);
    n = CmAdd(it, n, 3, L"复制路径", 0);
    n = CmAdd(it, n, 0, L"", CM_SEP);
    n = CmAdd(it, n, 4, L"移到回收站", CM_RED);
    POINT p;
    GetCursorPos(&p);
    c->inMenu = 1;
    int r = ShowMenu(it, n, p.x, p.y);
    c->inMenu = 0;
    if (r == 1) { Item t; ZeroMemory(&t, sizeof t); t.type = T_FILE; lstrcpynW(t.path, c->path[k], 1024); LaunchAsync(&t); c->done = 1; }
    else if (r == 2) { WCHAR a[1100]; wsprintfW(a, L"/select,\"%s\"", c->path[k]); ShellExecuteW(NULL, L"open", L"explorer.exe", a, NULL, SW_SHOWNORMAL); c->done = 1; }
    else if (r == 3 && OpenClipboard(g_hwnd)) {
        EmptyClipboard();
        size_t l = (lstrlenW(c->path[k]) + 1) * sizeof(WCHAR);
        HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, l);
        if (hg) { memcpy(GlobalLock(hg), c->path[k], l); GlobalUnlock(hg); SetClipboardData(CF_UNICODETEXT, hg); }
        CloseClipboard();
    } else if (r == 4) {
        WCHAR list[MAX_PATH + 2] = { 0 };
        lstrcpyW(list, c->path[k]);
        SHFILEOPSTRUCTW fo;
        ZeroMemory(&fo, sizeof fo);
        fo.wFunc = FO_DELETE; fo.pFrom = list; fo.fFlags = FOF_ALLOWUNDO;
        SHFileOperationW(&fo);
        c->done = 1;
    }
}

static LRESULT CALLBACK FanProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    FanCtx *c = (FanCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    if (!c) return DefWindowProcW(h, m, w, l);
    int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
    switch (m) {
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_TIMER:
        if (w == 2) {
            float t = (GetTickCount() - c->t0) / 160.f;
            if (t >= 1.f) { KillTimer(h, 2); c->alpha = 255; } else c->alpha = (int)(255 * (1.f - (1.f - t) * (1.f - t)));
            FanDraw(c, h);
        }
        else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) c->done = 1;
        return 0;
    case WM_MOUSEMOVE: {
        if (cfg.fanDrag && c->press >= 0 && (w & MK_LBUTTON) && c->press < c->n - 1 && (abs(x - c->down.x) > 6 || abs(y - c->down.y) > 6)) {
            int k = c->press;
            c->press = -1;
            if (GetCapture() == h) ReleaseCapture();
            FanDragOut(c->path[k], h);
            c->done = 1;
            return 0;
        }
        int hv = FanHit(c, x, y);
        if (hv != c->hover) { c->hover = hv; FanDraw(c, h); }
        return 0;
    }
    case WM_LBUTTONDOWN: c->press = FanHit(c, x, y); c->down.x = x; c->down.y = y; if (c->press >= 0) SetCapture(h); return 0;
    case WM_LBUTTONUP: {
        int k = FanHit(c, x, y), pr = c->press;
        c->press = -1;
        if (GetCapture() == h) ReleaseCapture();
        if (k >= 0 && k == pr) {
            Item t;
            ZeroMemory(&t, sizeof t);
            t.type = T_FILE;
            lstrcpynW(t.path, c->path[k], 1024);
            LaunchAsync(&t);
            c->done = 1;
        }
        return 0;
    }
    case WM_RBUTTONUP: FanMenu(c, FanHit(c, x, y)); return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static void ShowFan(int idx) {
    if (idx < 0 || idx >= nItems) return;
    Item *it = &items[idx];
    WCHAR pat[MAX_PATH + 4];
    PathCombineW(pat, it->path, L"*");
    FanLite *list = (FanLite *)HeapAlloc(GetProcessHeap(), 0, sizeof(FanLite) * 300);
    if (!list) return;
    int cnt = 0;
    WIN32_FIND_DATAW fd;
    HANDLE ff = FindFirstFileW(pat, &fd);
    if (ff != INVALID_HANDLE_VALUE) {
        do {
            if (fd.cFileName[0] == L'.' || (fd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM)) || cnt >= 300) continue;
            lstrcpynW(list[cnt].name, fd.cFileName, 96);
            PathCombineW(list[cnt].path, it->path, fd.cFileName);
            list[cnt].ft = fd.ftLastWriteTime;
            cnt++;
        } while (FindNextFileW(ff, &fd));
        FindClose(ff);
    }
    if (cnt == 0) { HeapFree(GetProcessHeap(), 0, list); ShellExecuteW(NULL, L"open", it->path, NULL, NULL, SW_SHOWNORMAL); return; }
    qsort(list, cnt, sizeof(FanLite), FanCmp);
    FanCtx *c = (FanCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(FanCtx));
    if (!c) { HeapFree(GetProcessHeap(), 0, list); return; }
    int n = cnt < 10 ? cnt : 10;
    c->hover = -1; c->press = -1;
    c->isz = (float)Sc(46);
    for (int k = 0; k < n; k++) {
        lstrcpynW(c->name[k], list[k].name, 96);
        lstrcpynW(c->path[k], list[k].path, MAX_PATH);
        c->icon[k] = ShellIconBitmap(list[k].path, NULL, 96);
    }
    lstrcpyW(c->name[n], L"在资源管理器中打开…");
    lstrcpynW(c->path[n], it->path, MAX_PATH);
    c->n = n + 1;
    HeapFree(GetProcessHeap(), 0, list);

    RECT ir;
    IconScreenRect(idx, &ir);
    float cx0 = (float)(ir.left + ir.right) / 2, cy0 = (float)(ir.top + ir.bottom) / 2;
    float dx = 0, dy = -1;
    switch (cfg.edge) { case E_TOP: dy = 1; break; case E_LEFT: dx = 1; dy = 0; break; case E_RIGHT: dx = -1; dy = 0; break; default: break; }
    RECT mr = g_mi.rcMonitor;
    float mc = Horiz() ? (mr.left + mr.right) / 2.f : (mr.top + mr.bottom) / 2.f;
    float sign = ((Horiz() ? cx0 : cy0) < mc) ? 1.f : -1.f;
    float arc = cfg.fanArc / 100.f, gapPx = (float)Sc(cfg.fanGap);
    if (cfg.fanReverse) arc = -arc;
    float minx = 1e9f, miny = 1e9f, maxx = -1e9f, maxy = -1e9f;
    for (int k = 0; k < c->n; k++) {
        float d = c->isz * 0.9f + k * (c->isz + gapPx), lat = -sign * arc * k * k * c->isz * 0.175f;   /* open outwards, like the macOS fan */
        c->cx[k] = cx0 + dx * d + (Horiz() ? lat : 0);
        c->cy[k] = cy0 + dy * d + (Horiz() ? 0 : lat);
        c->lw[k] = TextWidth(g_g, c->name[k], g_font) + 26;
        if (c->lw[k] > 420) c->lw[k] = 420;
        if (Horiz()) {
            /* Horizontal dock: put labels on the outside of the arc, away from the icon trail. */
            BOOL labelLeft = (fabsf(arc) < 0.01f) ? TRUE : ((-sign * arc) < 0.f);
            c->lx[k] = labelLeft ? c->cx[k] - c->isz / 2 - 12 - c->lw[k] : c->cx[k] + c->isz / 2 + 12;
            c->ly[k] = c->cy[k] - 14;
        } else {
            /* Vertical dock: labels sit beside the fan items, not underneath them where
               the monitor edge and neighbouring rows used to clip/cover the text. */
            c->lx[k] = dx > 0 ? c->cx[k] + c->isz / 2 + 12 : c->cx[k] - c->isz / 2 - 12 - c->lw[k];
            c->ly[k] = c->cy[k] - 14;
        }
        float l0 = c->cx[k] - c->isz / 2, r0 = c->cx[k] + c->isz / 2, t0 = c->cy[k] - c->isz / 2, b0 = c->cy[k] + c->isz / 2;
        if (c->lx[k] < l0) l0 = c->lx[k];
        if (c->lx[k] + c->lw[k] > r0) r0 = c->lx[k] + c->lw[k];
        if (c->ly[k] < t0) t0 = c->ly[k];
        if (c->ly[k] + 28 > b0) b0 = c->ly[k] + 28;
        if (l0 < minx) minx = l0;
        if (t0 < miny) miny = t0;
        if (r0 > maxx) maxx = r0;
        if (b0 > maxy) maxy = b0;
    }
    float shiftx = 0, shifty = 0;
    if (minx < mr.left + 4) shiftx = mr.left + 4 - minx;
    if (maxx > mr.right - 4) shiftx = mr.right - 4 - maxx;
    if (miny < mr.top + 4) shifty = mr.top + 4 - miny;
    if (maxy > mr.bottom - 4) shifty = mr.bottom - 4 - maxy;
    int W = (int)(maxx - minx) + 8, H = (int)(maxy - miny) + 8;
    c->px = (int)(minx + shiftx) - 4; c->py = (int)(miny + shifty) - 4;
    for (int k = 0; k < c->n; k++) {
        c->cx[k] += shiftx - c->px; c->cy[k] += shifty - c->py;
        c->lx[k] += shiftx - c->px; c->ly[k] += shifty - c->py;
    }
    c->icon[n] = it->icon;   /* folder icon for the "open" entry (not owned) */
    if (CanvasNew(&c->cv, W, H)) {
        HWND h = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CLS_FAN, L"", WS_POPUP, c->px, c->py, W, H, NULL, NULL, g_inst, NULL);
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)c);
        c->alpha = 0; c->t0 = GetTickCount();
        FanDraw(c, h);
        ShowWindow(h, SW_SHOWNA);
        gMenuOpen = TRUE;
        ClosePreview();
        SetTimer(h, 1, 50, NULL);
        SetTimer(h, 2, 15, NULL);
        GuardStart(FanInside, c, &c->done, h);
        MSG msg;
        while (!c->done && GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        GuardStop();
        gMenuOpen = FALSE;
        SetWindowLongPtrW(h, GWLP_USERDATA, 0);
        if (GetCapture() == h) ReleaseCapture();
        DestroyWindow(h);
        CanvasFree(&c->cv);
    }
    for (int k = 0; k < n; k++) if (c->icon[k]) GdipDisposeImage(c->icon[k]);
    HeapFree(GetProcessHeap(), 0, c);
    Kick();
}

/* ======================= quick notes manager (rich text, list, temporary saves, txt export) ======================= */
typedef struct { FILETIME ft; char *rtf; int rtfLen; WCHAR *txt; WCHAR title[72]; } Note;
#define MAXNOTES 300
static Note *g_notes;
static int g_nNotes, g_noteSel = -1;
static HWND g_noteWnd, g_noteEdit, g_noteList;
typedef struct { char *buf; int len, cap, pos; } StrmCtx;
#define NID_LIST 200
#define NID_EDIT 201
#define NID_B 301
#define NID_I 302
#define NID_BUL 303
#define NID_NEW 310
#define NID_SAVE 311
#define NID_DEL 312
#define NID_DELALL 313
#define NID_EXPSEL 314
#define NID_EXPALL 315
#define NID_LOC 316

static void NotePath(WCHAR *out) { PathCombineW(out, g_dir, L"notes.dat"); }

static void NotesSave(void) {
    WCHAR f[MAX_PATH + 8], tmp[MAX_PATH + 8];
    NotePath(f);
    lstrcpyW(tmp, f);
    lstrcatW(tmp, L".tmp");
    HANDLE h = CreateFileW(tmp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return;
    DWORD w;
    WriteFile(h, "MDN1\n", 5, &w, NULL);
    for (int i = 0; i < g_nNotes; i++) {
        int tl = WideCharToMultiByte(CP_UTF8, 0, g_notes[i].txt ? g_notes[i].txt : L"", -1, NULL, 0, NULL, NULL) - 1;
        char *u8 = (char *)HeapAlloc(GetProcessHeap(), 0, tl + 1);
        if (!u8) continue;
        WideCharToMultiByte(CP_UTF8, 0, g_notes[i].txt ? g_notes[i].txt : L"", -1, u8, tl + 1, NULL, NULL);
        char hd[80];
        wsprintfA(hd, "%08x%08x %d %d\n", g_notes[i].ft.dwHighDateTime, g_notes[i].ft.dwLowDateTime, g_notes[i].rtfLen, tl);
        WriteFile(h, hd, lstrlenA(hd), &w, NULL);
        WriteFile(h, g_notes[i].rtf, g_notes[i].rtfLen, &w, NULL);
        WriteFile(h, u8, tl, &w, NULL);
        WriteFile(h, "\n", 1, &w, NULL);
        HeapFree(GetProcessHeap(), 0, u8);
    }
    CloseHandle(h);
    MoveFileExW(tmp, f, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
}

static void NoteTitle(Note *n) {
    n->title[0] = 0;
    if (n->txt) {
        int k = 0;
        const WCHAR *p = n->txt;
        while (*p == L' ' || *p == L'\r' || *p == L'\n' || *p == L'\t') p++;
        while (*p && *p != L'\r' && *p != L'\n' && k < 70) n->title[k++] = *p++;
        n->title[k] = 0;
    }
    if (!n->title[0]) lstrcpyW(n->title, L"（空白速记）");
}

static void NotesLoad(void) {
    if (g_notes) return;
    g_notes = (Note *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Note) * MAXNOTES);
    WCHAR f[MAX_PATH];
    NotePath(f);
    BYTE *b = NULL;
    DWORD n = 0;
    if (!ReadWhole(f, &b, &n)) return;
    if (n > 5 && !memcmp(b, "MDN1\n", 5)) {
        DWORD pos = 5;
        while (pos < n && g_nNotes < MAXNOTES) {
            unsigned hi = 0, lo = 0;
            int rl = 0, tl = 0;
            char hd[80];
            int k = 0;
            while (pos < n && b[pos] != '\n' && k < 79) hd[k++] = (char)b[pos++];
            hd[k] = 0;
            pos++;
            if (sscanf(hd, "%8x%8x %d %d", &hi, &lo, &rl, &tl) != 4 || pos + rl + tl > n) break;
            Note *nt = &g_notes[g_nNotes];
            nt->ft.dwHighDateTime = hi; nt->ft.dwLowDateTime = lo;
            nt->rtf = (char *)HeapAlloc(GetProcessHeap(), 0, rl + 1);
            memcpy(nt->rtf, b + pos, rl);
            nt->rtf[rl] = 0;
            nt->rtfLen = rl;
            pos += rl;
            int wl = MultiByteToWideChar(CP_UTF8, 0, (char *)b + pos, tl, NULL, 0);
            nt->txt = (WCHAR *)HeapAlloc(GetProcessHeap(), 0, (wl + 1) * sizeof(WCHAR));
            MultiByteToWideChar(CP_UTF8, 0, (char *)b + pos, tl, nt->txt, wl);
            nt->txt[wl] = 0;
            pos += tl + 1;
            NoteTitle(nt);
            g_nNotes++;
        }
    }
    HeapFree(GetProcessHeap(), 0, b);
}

static DWORD CALLBACK StrmOut(DWORD_PTR ck, LPBYTE pb, LONG cb, LONG *pcb) {
    StrmCtx *c = (StrmCtx *)ck;
    if (c->len + cb + 1 > c->cap) { c->cap = (c->len + cb + 1) * 2; c->buf = (char *)HeapReAlloc(GetProcessHeap(), 0, c->buf, c->cap); }
    memcpy(c->buf + c->len, pb, cb);
    c->len += cb;
    *pcb = cb;
    return 0;
}
static DWORD CALLBACK StrmIn(DWORD_PTR ck, LPBYTE pb, LONG cb, LONG *pcb) {
    StrmCtx *c = (StrmCtx *)ck;
    LONG n = c->len - c->pos < cb ? c->len - c->pos : cb;
    memcpy(pb, c->buf + c->pos, n);
    c->pos += n;
    *pcb = n;
    return 0;
}

static void NoteApplyTheme(void) {
    if (!g_noteEdit) return;
    SendMessageW(g_noteEdit, EM_SETBKGNDCOLOR, 0, g_uiDark ? RGB(40, 41, 48) : RGB(255, 255, 255));
    CHARFORMAT2W cf;
    ZeroMemory(&cf, sizeof cf);
    cf.cbSize = sizeof cf;
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = g_uiDark ? RGB(236, 237, 242) : RGB(28, 30, 38);
    SendMessageW(g_noteEdit, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
    SendMessageW(g_noteEdit, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
}

static void NoteLoadIntoEditor(int i) {
    if (i < 0 || i >= g_nNotes) { SetWindowTextW(g_noteEdit, L""); NoteApplyTheme(); return; }
    StrmCtx c = { g_notes[i].rtf, g_notes[i].rtfLen, g_notes[i].rtfLen, 0 };
    EDITSTREAM es = { (DWORD_PTR)&c, 0, StrmIn };
    SendMessageW(g_noteEdit, EM_STREAMIN, SF_RTF, (LPARAM)&es);
    NoteApplyTheme();
}

static void NoteRefreshList(void) {
    SendMessageW(g_noteList, LB_RESETCONTENT, 0, 0);
    for (int i = 0; i < g_nNotes; i++) SendMessageW(g_noteList, LB_ADDSTRING, 0, (LPARAM)g_notes[i].title);
    if (g_noteSel >= 0 && g_noteSel < g_nNotes) SendMessageW(g_noteList, LB_SETCURSEL, g_noteSel, 0);
}

static BOOL NoteCommit(void) {   /* temporary save into the notes store (not a txt file) */
    int tl = GetWindowTextLengthW(g_noteEdit);
    if (tl <= 0) return FALSE;
    WCHAR *txt = (WCHAR *)HeapAlloc(GetProcessHeap(), 0, (tl + 2) * sizeof(WCHAR));
    GetWindowTextW(g_noteEdit, txt, tl + 1);
    StrmCtx c = { (char *)HeapAlloc(GetProcessHeap(), 0, 4096), 0, 4096, 0 };
    EDITSTREAM es = { (DWORD_PTR)&c, 0, StrmOut };
    SendMessageW(g_noteEdit, EM_STREAMOUT, SF_RTF, (LPARAM)&es);
    Note *n;
    if (g_noteSel >= 0 && g_noteSel < g_nNotes) n = &g_notes[g_noteSel];
    else if (g_nNotes < MAXNOTES) { n = &g_notes[g_nNotes]; g_noteSel = g_nNotes++; }
    else { HeapFree(GetProcessHeap(), 0, txt); HeapFree(GetProcessHeap(), 0, c.buf); return FALSE; }
    if (n->rtf) HeapFree(GetProcessHeap(), 0, n->rtf);
    if (n->txt) HeapFree(GetProcessHeap(), 0, n->txt);
    n->rtf = c.buf; n->rtfLen = c.len; n->txt = txt;
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &n->ft);
    NoteTitle(n);
    NotesSave();
    NoteRefreshList();
    return TRUE;
}

static void NoteRethemed(void);
static void NoteWriteTxt(const WCHAR *file, int from, int to) {
    HANDLE h = CreateFileW(file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) { MessageBoxW(g_noteWnd, L"无法写入该位置。", L"速记", MB_OK | MB_ICONWARNING); return; }
    DWORD w;
    static const BYTE bom[3] = { 0xEF, 0xBB, 0xBF };
    WriteFile(h, bom, 3, &w, NULL);
    for (int i = from; i <= to; i++) {
        FILETIME lft;
        SYSTEMTIME st;
        FileTimeToLocalFileTime(&g_notes[i].ft, &lft);
        FileTimeToSystemTime(&lft, &st);
        WCHAR hd[64];
        wsprintfW(hd, L"[%04d-%02d-%02d %02d:%02d]\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
        const WCHAR *parts[3] = { hd, g_notes[i].txt ? g_notes[i].txt : L"", L"\r\n\r\n" };
        for (int p = 0; p < 3; p++) {
            int bl = WideCharToMultiByte(CP_UTF8, 0, parts[p], -1, NULL, 0, NULL, NULL) - 1;
            char *u = (char *)HeapAlloc(GetProcessHeap(), 0, bl + 1);
            if (u) { WideCharToMultiByte(CP_UTF8, 0, parts[p], -1, u, bl + 1, NULL, NULL); WriteFile(h, u, bl, &w, NULL); HeapFree(GetProcessHeap(), 0, u); }
        }
    }
    CloseHandle(h);
}

/* rich text -> Markdown: bold -> **, italic -> *, bullet paragraphs -> "- " (a hidden RichEdit reads the formatting) */
static WCHAR *NoteToMd(int idx) {
    if (idx < 0 || idx >= g_nNotes || !g_notes[idx].rtf) return NULL;
    HWND e = CreateWindowExW(0, MSFTEDIT_CLASS, L"", WS_CHILD | ES_MULTILINE, 0, 0, 300, 100, g_noteWnd, NULL, g_inst, NULL);
    if (!e) return NULL;
    StrmCtx c = { g_notes[idx].rtf, g_notes[idx].rtfLen, g_notes[idx].rtfLen, 0 };
    EDITSTREAM es = { (DWORD_PTR)&c, 0, StrmIn };
    SendMessageW(e, EM_STREAMIN, SF_RTF, (LPARAM)&es);
    GETTEXTLENGTHEX gl = { GTL_DEFAULT, 1200 };
    int len = (int)SendMessageW(e, EM_GETTEXTLENGTHEX, (WPARAM)&gl, 0);
    if (len < 0) len = 0;
    WCHAR *t = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (len + 4) * sizeof(WCHAR));
    WCHAR *out = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (len * 4 + 64) * sizeof(WCHAR));
    if (!t || !out) { DestroyWindow(e); if (t) HeapFree(GetProcessHeap(), 0, t); if (out) HeapFree(GetProcessHeap(), 0, out); return NULL; }
    GETTEXTEX gt;
    ZeroMemory(&gt, sizeof gt);
    gt.cb = (len + 2) * sizeof(WCHAR); gt.flags = GT_DEFAULT; gt.codepage = 1200;
    SendMessageW(e, EM_GETTEXTEX, (WPARAM)&gt, (LPARAM)t);
    int oi = 0;
    BOOL bold = FALSE, ital = FALSE, bullet = FALSE;
    for (int i = 0; i < len; i++) {
        BOOL ls = (i == 0 || t[i - 1] == L'\r' || t[i - 1] == L'\n');
        if (ls) {
            CHARRANGE cr0 = { i, i };
            SendMessageW(e, EM_EXSETSEL, 0, (LPARAM)&cr0);
            PARAFORMAT2 pf;
            ZeroMemory(&pf, sizeof pf);
            pf.cbSize = sizeof pf;
            SendMessageW(e, EM_GETPARAFORMAT, 0, (LPARAM)&pf);
            bullet = (pf.wNumbering == PFN_BULLET);
            if (bullet) { out[oi++] = L'-'; out[oi++] = L' '; }
        }
        if (t[i] == L'\r' || t[i] == L'\n') {
            if (ital) { out[oi++] = L'*'; ital = FALSE; }
            if (bold) { out[oi++] = L'*'; out[oi++] = L'*'; bold = FALSE; }
            if (t[i] == L'\r' || (i > 0 && t[i - 1] != L'\r')) { if (!bullet) { out[oi++] = L' '; out[oi++] = L' '; } out[oi++] = L'\r'; out[oi++] = L'\n'; }
            continue;
        }
        CHARRANGE cr = { i, i + 1 };
        SendMessageW(e, EM_EXSETSEL, 0, (LPARAM)&cr);
        CHARFORMAT2W cf;
        ZeroMemory(&cf, sizeof cf);
        cf.cbSize = sizeof cf;
        SendMessageW(e, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
        BOOL nb = (cf.dwEffects & CFE_BOLD) != 0, ni = (cf.dwEffects & CFE_ITALIC) != 0;
        if (t[i] == L' ') { nb = bold; ni = ital; }   /* never open / close a marker on a blank */
        if (ital && !ni) { out[oi++] = L'*'; ital = FALSE; }
        if (bold && !nb) { out[oi++] = L'*'; out[oi++] = L'*'; bold = FALSE; }
        if (!bold && nb) { out[oi++] = L'*'; out[oi++] = L'*'; bold = TRUE; }
        if (!ital && ni) { out[oi++] = L'*'; ital = TRUE; }
        out[oi++] = t[i];
    }
    if (ital) out[oi++] = L'*';
    if (bold) { out[oi++] = L'*'; out[oi++] = L'*'; }
    out[oi] = 0;
    HeapFree(GetProcessHeap(), 0, t);
    DestroyWindow(e);
    return out;
}

static void NoteWriteMd(const WCHAR *file, int from, int to) {
    HANDLE h = CreateFileW(file, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) { MessageBoxW(g_noteWnd, L"无法写入该位置。", L"速记", MB_OK | MB_ICONWARNING); return; }
    DWORD w;
    static const BYTE bom[3] = { 0xEF, 0xBB, 0xBF };
    WriteFile(h, bom, 3, &w, NULL);
    for (int i = from; i <= to; i++) {
        FILETIME lft;
        SYSTEMTIME st;
        FileTimeToLocalFileTime(&g_notes[i].ft, &lft);
        FileTimeToSystemTime(&lft, &st);
        WCHAR hd[64];
        wsprintfW(hd, L"## %04d-%02d-%02d %02d:%02d\r\n\r\n", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
        WCHAR *md = NoteToMd(i);
        const WCHAR *parts[3] = { hd, md ? md : (g_notes[i].txt ? g_notes[i].txt : L""), L"\r\n\r\n" };
        for (int p = 0; p < 3; p++) {
            int bl = WideCharToMultiByte(CP_UTF8, 0, parts[p], -1, NULL, 0, NULL, NULL) - 1;
            char *u = (char *)HeapAlloc(GetProcessHeap(), 0, bl + 1);
            if (u) { WideCharToMultiByte(CP_UTF8, 0, parts[p], -1, u, bl + 1, NULL, NULL); WriteFile(h, u, bl, &w, NULL); HeapFree(GetProcessHeap(), 0, u); }
        }
        if (md) HeapFree(GetProcessHeap(), 0, md);
    }
    CloseHandle(h);
}

static void NoteRethemed(void) {   /* follow light / dark switches at once */
    if (!g_noteWnd) return;
    BOOL d = g_uiDark;
    DwmSetWindowAttribute(g_noteWnd, 20, &d, sizeof d);
    NoteApplyTheme();
    InvalidateRect(g_noteWnd, NULL, TRUE);
    RedrawWindow(g_noteWnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

static void NoteExport(HWND h, BOOL all) {
    if (g_nNotes == 0) { MessageBoxW(h, L"还没有保存过速记。", L"速记", MB_OK | MB_ICONINFORMATION); return; }
    if (!all && (g_noteSel < 0 || g_noteSel >= g_nNotes)) { MessageBoxW(h, L"请先在左侧选择一条速记。", L"速记", MB_OK | MB_ICONINFORMATION); return; }
    WCHAR f[MAX_PATH];
    if (all) lstrcpyW(f, L"速记");
    else { lstrcpynW(f, g_notes[g_noteSel].title, 40); for (WCHAR *p = f; *p; p++) if (wcschr(L"\\/:*?\"<>|", *p)) *p = L'_'; }
    OPENFILENAMEW o;
    ZeroMemory(&o, sizeof o);
    o.lStructSize = sizeof o; o.hwndOwner = h;
    o.lpstrFilter = L"文本文件 (*.txt)\0*.txt\0Markdown (*.md)\0*.md\0";
    o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.lpstrInitialDir = g_noteDir;
    o.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if (!GetSaveFileNameW(&o)) return;
    BOOL md = (o.nFilterIndex == 2) || PathMatchSpecW(f, L"*.md");
    if (!PathFindExtensionW(f)[0]) lstrcatW(f, md ? L".md" : L".txt");
    if (md) { if (all) NoteWriteMd(f, 0, g_nNotes - 1); else NoteWriteMd(f, g_noteSel, g_noteSel); }
    else if (all) NoteWriteTxt(f, 0, g_nNotes - 1); else NoteWriteTxt(f, g_noteSel, g_noteSel);
}

static void NoteLocMenu(HWND h) {
    CmItem it[8];
    int n = 0;
    n = CmAdd(it, n, 1, L"导出到：桌面", 0);
    n = CmAdd(it, n, 2, L"导出到：文档", 0);
    n = CmAdd(it, n, 3, L"导出到：下载", 0);
    n = CmAdd(it, n, 4, L"选择其它文件夹…", 0);
    n = CmAdd(it, n, 0, L"", CM_SEP);
    WCHAR cur[MAX_PATH + 16];
    wsprintfW(cur, L"当前：%s", g_noteDir);
    n = CmAdd(it, n, 0, cur, CM_DIS);
    POINT p;
    GetCursorPos(&p);
    g_menuEdge = E_BOTTOM;
    int r = ShowMenu(it, n, p.x, p.y);
    g_menuEdge = -1;
    WCHAR d[MAX_PATH] = L"";
    if (r == 1) SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, d);
    else if (r == 2) SHGetFolderPathW(NULL, CSIDL_PERSONAL, NULL, 0, d);
    else if (r == 3) { PWSTR kp = NULL; if (SUCCEEDED(SHGetKnownFolderPath(&FOLDERID_Downloads, 0, NULL, &kp)) && kp) { lstrcpynW(d, kp, MAX_PATH); CoTaskMemFree(kp); } }
    else if (r == 4) {
        BROWSEINFOW b;
        ZeroMemory(&b, sizeof b);
        b.hwndOwner = h; b.lpszTitle = L"选择速记导出的默认文件夹"; b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST pi = SHBrowseForFolderW(&b);
        if (pi) { SHGetPathFromIDListW(pi, d); CoTaskMemFree(pi); }
    }
    if (d[0]) { lstrcpynW(g_noteDir, d, MAX_PATH); ScheduleSave(); }
}

static void NoteFormat(int id) {
    CHARFORMAT2W cf;
    ZeroMemory(&cf, sizeof cf);
    cf.cbSize = sizeof cf;
    if (id == NID_BUL) {
        PARAFORMAT2 pf;
        ZeroMemory(&pf, sizeof pf);
        pf.cbSize = sizeof pf;
        SendMessageW(g_noteEdit, EM_GETPARAFORMAT, 0, (LPARAM)&pf);
        BOOL on = (pf.wNumbering == PFN_BULLET);
        pf.dwMask = PFM_NUMBERING | PFM_OFFSET;
        pf.wNumbering = on ? 0 : PFN_BULLET;
        pf.dxOffset = on ? 0 : 360;
        SendMessageW(g_noteEdit, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
    } else {
        DWORD bit = id == NID_B ? CFE_BOLD : CFE_ITALIC;
        SendMessageW(g_noteEdit, EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
        BOOL on = (cf.dwEffects & bit) != 0;
        ZeroMemory(&cf, sizeof cf);
        cf.cbSize = sizeof cf;
        cf.dwMask = id == NID_B ? CFM_BOLD : CFM_ITALIC;
        cf.dwEffects = on ? 0 : bit;
        SendMessageW(g_noteEdit, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cf);
    }
    SetFocus(g_noteEdit);
}

static LRESULT CALLBACK NoteProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    LRESULT r;
    if (m == WM_ERASEBKGND) return DlgErase(h, (HDC)w);
    if (m != WM_DRAWITEM && DlgCommon(m, w, l, &r)) return r;
    switch (m) {
    case WM_CREATE: {
        LoadLibraryW(L"Msftedit.dll");
        NotesLoad();
        int pad = Sc(14), lw = Sc(236), top = Sc(14);
        g_noteList = CreateWindowExW(0, L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_OWNERDRAWFIXED, pad, top, lw, Sc(440), h, (HMENU)NID_LIST, g_inst, NULL);
        MkBtn(h, L"B", pad + lw + Sc(14), top, Sc(40), Sc(32), NID_B, 0);
        MkBtn(h, L"I", pad + lw + Sc(58), top, Sc(40), Sc(32), NID_I, 0);
        MkBtn(h, L"• 列表", pad + lw + Sc(102), top, Sc(70), Sc(32), NID_BUL, 0);
        g_noteEdit = CreateWindowExW(0, L"RICHEDIT50W", L"", WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN, pad + lw + Sc(14), top + Sc(42), Sc(470), Sc(398), h, (HMENU)NID_EDIT, g_inst, NULL);
        int by = Sc(466);
        MkBtn(h, L"新建", pad, by, Sc(76), Sc(32), NID_NEW, 0);
        MkBtn(h, L"临时保存", pad + Sc(84), by, Sc(96), Sc(32), NID_SAVE, BS_DEFPUSHBUTTON);
        MkBtn(h, L"删除", pad + Sc(188), by, Sc(70), Sc(32), NID_DEL, 0);
        MkBtn(h, L"全部删除", pad + Sc(266), by, Sc(90), Sc(32), NID_DELALL, 0);
        SetPropW(GetDlgItem(h, NID_DEL), L"mdr", (HANDLE)1);
        SetPropW(GetDlgItem(h, NID_DELALL), L"mdr", (HANDLE)1);
        MkBtn(h, L"导出所选…", pad + Sc(380), by, Sc(100), Sc(32), NID_EXPSEL, 0);
        MkBtn(h, L"导出全部…", pad + Sc(488), by, Sc(100), Sc(32), NID_EXPALL, 0);
        MkBtn(h, L"导出位置 ▾", pad + Sc(596), by, Sc(116), Sc(32), NID_LOC, 0);
        SetFontAll(h);
        NoteApplyTheme();
        NoteRefreshList();
        SetFocus(g_noteEdit);
        return 0;
    }
    case WM_MEASUREITEM: {
        MEASUREITEMSTRUCT *mi = (MEASUREITEMSTRUCT *)l;
        if (mi->CtlType == ODT_LISTBOX) { mi->itemHeight = Sc(54); return TRUE; }
        break;
    }
    case WM_DRAWITEM: {
        DRAWITEMSTRUCT *d = (DRAWITEMSTRUCT *)l;
        if (d->CtlType == ODT_LISTBOX) {
            if ((int)d->itemID < 0 || (int)d->itemID >= g_nNotes) return TRUE;
            DlgBrushes();
            FillRect(d->hDC, &d->rcItem, g_dbEdit);
            GpGraphics *g = NULL;
            GdipCreateFromHDC(d->hDC, &g);
            if (g) {
                SetupG(g);
                float x = (float)d->rcItem.left, y = (float)d->rcItem.top, w2 = (float)(d->rcItem.right - d->rcItem.left), h2 = (float)(d->rcItem.bottom - d->rcItem.top);
                BOOL sel = (d->itemState & ODS_SELECTED) != 0;
                if (sel) FillRR(g, ARGBC(255, 10, 132, 255), x + 4, y + 3, w2 - 8, h2 - 6, 9);
                ARGB t1 = sel ? ARGBC(255, 255, 255, 255) : (g_uiDark ? ARGBC(255, 236, 237, 242) : ARGBC(255, 28, 30, 38));
                ARGB t2 = sel ? ARGBC(200, 255, 255, 255) : (g_uiDark ? ARGBC(255, 146, 150, 162) : ARGBC(255, 122, 126, 140));
                DrawTextBox(g, g_notes[d->itemID].title, g_fontBold, x + 14, y + 6, w2 - 28, 22, t1, TRUE);
                FILETIME lft;
                SYSTEMTIME st;
                FileTimeToLocalFileTime(&g_notes[d->itemID].ft, &lft);
                FileTimeToSystemTime(&lft, &st);
                WCHAR tm[48];
                wsprintfW(tm, L"%04d-%02d-%02d %02d:%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);
                DrawTextBox(g, tm, g_font, x + 14, y + 28, w2 - 28, 20, t2, TRUE);
                GdipDeleteGraphics(g);
            }
            return TRUE;
        }
        DrawOwnerButton(d);
        return TRUE;
    }
    case WM_COMMAND: {
        int id = LOWORD(w);
        if (id == NID_LIST && HIWORD(w) == LBN_SELCHANGE) { g_noteSel = (int)SendMessageW(g_noteList, LB_GETCURSEL, 0, 0); NoteLoadIntoEditor(g_noteSel); }
        else if (HIWORD(w) != BN_CLICKED) return 0;
        else if (id == NID_NEW) { g_noteSel = -1; SendMessageW(g_noteList, LB_SETCURSEL, (WPARAM)-1, 0); NoteLoadIntoEditor(-1); SetFocus(g_noteEdit); }
        else if (id == NID_SAVE) { if (NoteCommit()) InvalidateRect(g_noteList, NULL, TRUE); }
        else if (id == NID_DEL) {
            if (g_noteSel >= 0 && g_noteSel < g_nNotes) {
                if (g_notes[g_noteSel].rtf) HeapFree(GetProcessHeap(), 0, g_notes[g_noteSel].rtf);
                if (g_notes[g_noteSel].txt) HeapFree(GetProcessHeap(), 0, g_notes[g_noteSel].txt);
                memmove(&g_notes[g_noteSel], &g_notes[g_noteSel + 1], sizeof(Note) * (g_nNotes - g_noteSel - 1));
                g_nNotes--;
                g_noteSel = -1;
                NotesSave(); NoteRefreshList(); NoteLoadIntoEditor(-1);
            }
        } else if (id == NID_DELALL) {
            if (g_nNotes && MessageBoxW(h, L"确定要删除全部速记吗？此操作不可恢复。", L"速记", MB_YESNO | MB_ICONWARNING) == IDYES) {
                for (int i = 0; i < g_nNotes; i++) { if (g_notes[i].rtf) HeapFree(GetProcessHeap(), 0, g_notes[i].rtf); if (g_notes[i].txt) HeapFree(GetProcessHeap(), 0, g_notes[i].txt); }
                ZeroMemory(g_notes, sizeof(Note) * MAXNOTES);
                g_nNotes = 0; g_noteSel = -1;
                NotesSave(); NoteRefreshList(); NoteLoadIntoEditor(-1);
            }
        } else if (id == NID_EXPSEL) NoteExport(h, FALSE);
        else if (id == NID_EXPALL) NoteExport(h, TRUE);
        else if (id == NID_LOC) NoteLocMenu(h);
        else if (id == NID_B || id == NID_I || id == NID_BUL) NoteFormat(id);
        return 0;
    }
    case WM_CLOSE:
        if (GetWindowTextLengthW(g_noteEdit) > 0) NoteCommit();   /* never lose what was typed */
        DestroyWindow(h);
        return 0;
    case WM_DESTROY: g_noteWnd = NULL; g_noteEdit = NULL; g_noteList = NULL; return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static void ShowNote(void) {
    if (g_noteWnd) { ShowWindow(g_noteWnd, SW_SHOWNORMAL); SetForegroundWindow(g_noteWnd); return; }
    UpdateTheme();
    int W = Sc(760), H = Sc(520);
    RECT r = { 0, 0, W, H };
    AdjustWindowRect(&r, WS_CAPTION | WS_SYSMENU, FALSE);
    RECT wa = g_mi.rcWork;
    g_noteWnd = CreateWindowExW(WS_EX_TOPMOST, CLS_NOTE, L"速记", WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                                (wa.left + wa.right - (r.right - r.left)) / 2, (wa.top + wa.bottom - (r.bottom - r.top)) / 2, r.right - r.left, r.bottom - r.top, NULL, NULL, g_inst, NULL);
    if (g_noteWnd) { DarkTitleBar(g_noteWnd); SetForegroundWindow(g_noteWnd); }
}

/* ======================= icon dialog (file or link, fit mode, preview) ======================= */
typedef struct { HWND hw, e, de; WCHAR text[1024]; WCHAR dark[1024]; int fit, ok, reset, done, darkMode; GpBitmap *prev; } IcoCtx;
#define ICO_EDIT 100
#define ICO_BROWSE 102
#define ICO_FIT0 111
#define ICO_RESET 120
#define ICO_DARK 130
#define ICO_DARKB 131
#define ICO_DARKMODE 132

static void IcoPreview(IcoCtx *c) {
    if (c->prev) { GdipDisposeImage(c->prev); c->prev = NULL; }
    WCHAR t[1024];
    GetWindowTextW(c->e, t, 1024);
    StrTrimW(t, L" \t\"");
    if (t[0] && !IsUrlStr(t) && GetFileAttributesW(t) != INVALID_FILE_ATTRIBUTES) {
        GpBitmap *s = NULL;
        if (GdipCreateBitmapFromFile(t, &s) == 0 && s) { c->prev = FitBitmap(s, 112, c->fit, 0.94f); GdipDisposeImage(s); }
    }
    RECT rc = { Sc(392), Sc(150), Sc(392) + 130, Sc(150) + 130 };
    InvalidateRect(c->hw, &rc, TRUE);
}

static void SegSet(HWND h, int first, int count, int sel) {
    for (int i = 0; i < count; i++) {
        HWND b = GetDlgItem(h, first + i);
        if (i == sel) SetPropW(b, L"mds", (HANDLE)1); else RemovePropW(b, L"mds");
        InvalidateRect(b, NULL, TRUE);
    }
}

static LRESULT CALLBACK IcoProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    IcoCtx *c = (IcoCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    LRESULT r;
    if (m == WM_ERASEBKGND) return DlgErase(h, (HDC)w);
    if (DlgCommon(m, w, l, &r)) return r;
    switch (m) {
    case WM_CREATE: {
        c = (IcoCtx *)((CREATESTRUCTW *)l)->lpCreateParams;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)c);
        c->hw = h;
        CreateWindowW(L"STATIC", L"图标图片（本地文件路径，或图片链接）：", WS_CHILD | WS_VISIBLE, Sc(18), Sc(14), Sc(500), Sc(20), h, NULL, g_inst, NULL);
        c->e = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", c->text, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, Sc(18), Sc(40), Sc(410), Sc(28), h, (HMENU)ICO_EDIT, g_inst, NULL);
        MkBtn(h, L"浏览…", Sc(438), Sc(39), Sc(84), Sc(30), ICO_BROWSE, 0);
        CreateWindowW(L"STATIC", L"建议：正方形 PNG（透明背景），512×512 以上效果最佳。图片会自动缩放到 Dock 图标大小，并可去掉多余的黑边 / 白边 / 透明边。", WS_CHILD | WS_VISIBLE, Sc(18), Sc(80), Sc(500), Sc(44), h, NULL, g_inst, NULL);
        CreateWindowW(L"STATIC", L"适配方式：", WS_CHILD | WS_VISIBLE, Sc(18), Sc(134), Sc(200), Sc(20), h, NULL, g_inst, NULL);
        static const WCHAR *fits[3] = { L"自动（去除透明 / 纯色边，推荐）", L"完整保留（不裁切，可能有留白）", L"居中裁切并填满（圆角方形）" };
        for (int i = 0; i < 3; i++) MkBtn(h, fits[i], Sc(18), Sc(160) + i * Sc(40), Sc(340), Sc(34), ICO_FIT0 + i, 0);
        SegSet(h, ICO_FIT0, 3, c->fit);
        CreateWindowW(L"STATIC", L"深色模式图标（可选，留空表示继续使用上面的图标）：", WS_CHILD | WS_VISIBLE, Sc(18), Sc(280), Sc(500), Sc(20), h, NULL, g_inst, NULL);
        c->de = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", c->dark, WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, Sc(18), Sc(306), Sc(410), Sc(28), h, (HMENU)ICO_DARK, g_inst, NULL);
        MkBtn(h, L"浏览…", Sc(438), Sc(305), Sc(84), Sc(30), ICO_DARKB, 0);
        CreateWindowW(L"BUTTON", L"启用系统深色模式自动切换图标", WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
                      Sc(18), Sc(342), Sc(330), Sc(22), h, (HMENU)ICO_DARKMODE, g_inst, NULL);
        SendDlgItemMessageW(h, ICO_DARKMODE, BM_SETCHECK, c->darkMode ? BST_CHECKED : BST_UNCHECKED, 0);
        MkBtn(h, L"确定", Sc(246), Sc(368), Sc(80), Sc(32), IDOK, BS_DEFPUSHBUTTON);
        MkBtn(h, L"恢复默认图标", Sc(334), Sc(368), Sc(108), Sc(32), ICO_RESET, 0);
        MkBtn(h, L"取消", Sc(450), Sc(368), Sc(72), Sc(32), IDCANCEL, 0);
        SetFontAll(h);
        SetFocus(c->e);
        IcoPreview(c);
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        GpGraphics *g = NULL;
        GdipCreateFromHDC(dc, &g);
        if (g) {
            SetupG(g);
            float px = (float)Sc(392), py = (float)Sc(150);
            FillRR(g, g_uiDark ? ARGBC(255, 52, 54, 62) : ARGBC(255, 232, 234, 240), px, py, 130, 130, 16);
            if (c && c->prev) DrawImg(g, c->prev, px + 9, py + 9, 112, 112, FALSE);
            else DrawCentered(g, L"预览", g_font, px, py, 130, 130, ARGBC(255, 140, 143, 155));
            GdipDeleteGraphics(g);
        }
        EndPaint(h, &ps);
        return 0;
    }
    case WM_COMMAND: {
        int id = LOWORD(w);
        if (id == ICO_EDIT && HIWORD(w) == EN_CHANGE) IcoPreview(c);
        else if (HIWORD(w) != BN_CLICKED) return 0;
        else if (id >= ICO_FIT0 && id < ICO_FIT0 + 3) { c->fit = id - ICO_FIT0; SegSet(h, ICO_FIT0, 3, c->fit); IcoPreview(c); }
        else if (id == ICO_DARKB) {
            WCHAR f[MAX_PATH] = L"";
            OPENFILENAMEW o;
            ZeroMemory(&o, sizeof o);
            o.lStructSize = sizeof o; o.hwndOwner = h;
            o.lpstrFilter = L"图片 / 图标\0*.png;*.ico;*.jpg;*.jpeg;*.bmp;*.gif\0所有文件\0*.*\0";
            o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.Flags = OFN_FILEMUSTEXIST;
            if (GetOpenFileNameW(&o)) SetWindowTextW(c->de, f);
        } else if (id == ICO_BROWSE) {
            WCHAR f[MAX_PATH] = L"";
            OPENFILENAMEW o;
            ZeroMemory(&o, sizeof o);
            o.lStructSize = sizeof o; o.hwndOwner = h;
            o.lpstrFilter = L"图片 / 图标\0*.png;*.ico;*.jpg;*.jpeg;*.bmp;*.gif\0所有文件\0*.*\0";
            o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.Flags = OFN_FILEMUSTEXIST;
            if (GetOpenFileNameW(&o)) SetWindowTextW(c->e, f);
        } else if (id == IDOK) {
            GetWindowTextW(c->e, c->text, 1024);
            GetWindowTextW(c->de, c->dark, 1024);
            /* The checkbox used to be painted from cfg but its new state was never read. */
            c->darkMode = (IsDlgButtonChecked(h, ICO_DARKMODE) == BST_CHECKED);
            c->ok = 1; c->done = 1; DestroyWindow(h);
        }
        else if (id == ICO_RESET) { c->reset = 1; c->ok = 1; c->done = 1; DestroyWindow(h); }
        else if (id == IDCANCEL) { c->done = 1; DestroyWindow(h); }
        return 0;
    }
    case WM_CLOSE: c->done = 1; DestroyWindow(h); return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static void ShowIconDialog(HWND parent, int i) {
    if (i < 0 || i >= nItems) return;
    Item *it = &items[i];
    IcoCtx c;
    ZeroMemory(&c, sizeof c);
    c.fit = it->fit;
    c.darkMode = (cfg.iconDarkMode != 0);
    lstrcpynW(c.text, it->icon_src, 1024);
    lstrcpynW(c.dark, it->icon_dark, 1024);
    RECT wa = g_mi.rcWork;
    RECT r = { 0, 0, Sc(540), Sc(438) };
    AdjustWindowRectEx(&r, WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_TOPMOST);
    HWND h = CreateWindowExW(WS_EX_TOPMOST | WS_EX_DLGMODALFRAME, CLS_ICOD, L"设置图标", WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                             (wa.left + wa.right - (r.right - r.left)) / 2, (wa.top + wa.bottom - (r.bottom - r.top)) / 2 - Sc(40), r.right - r.left, r.bottom - r.top, parent, NULL, g_inst, &c);
    if (!h) return;
    DarkTitleBar(h);
    if (parent) EnableWindow(parent, FALSE);
    SetForegroundWindow(h);
    MSG msg;
    while (!c.done && GetMessageW(&msg, NULL, 0, 0) > 0)
        if (!IsDialogMessageW(h, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    if (parent) { EnableWindow(parent, TRUE); SetForegroundWindow(parent); }
    if (c.prev) GdipDisposeImage(c.prev);
    if (!c.ok || i >= nItems) return;
    it = &items[i];
    cfg.iconDarkMode = c.darkMode ? (cfg.iconDarkMode ? cfg.iconDarkMode : 1) : 0;
    StrTrimW(c.text, L" \t\"");
    if (c.reset) { it->icon_src[0] = 0; it->fit = 0; LoadItemIcon(it); }
    else if (c.text[0] && lstrcmpiW(c.text, it->icon_src) != 0) {
        it->fit = c.fit;
        WCHAR dst[MAX_PATH], nm[64];
        if (IsUrlStr(c.text)) {
            wsprintfW(nm, L"c_%08x.png", Fnv(c.text) ^ GetTickCount());
            PathCombineW(dst, g_iconDir, nm);
            lstrcpynW(it->icon_src, dst, 512);
            it->fav = 0;
            StartDownload(1, c.text, dst);
        } else if (GetFileAttributesW(c.text) != INVALID_FILE_ATTRIBUTES) {
            wsprintfW(nm, L"c_%08x%s", Fnv(c.text) ^ GetTickCount(), PathFindExtensionW(c.text));
            PathCombineW(dst, g_iconDir, nm);
            if (CopyFileW(c.text, dst, FALSE)) { lstrcpynW(it->icon_src, dst, 512); LoadItemIcon(it); }
        }
    } else if (it->icon_src[0] && c.fit != it->fit) { it->fit = c.fit; LoadItemIcon(it); }
    StrTrimW(c.dark, L" \t\"");
    if (lstrcmpiW(c.dark, it->icon_dark) != 0) {
        if (!c.dark[0]) { it->icon_dark[0] = 0; }
        else if (GetFileAttributesW(c.dark) != INVALID_FILE_ATTRIBUTES) {
            WCHAR dst[MAX_PATH], nm[64];
            wsprintfW(nm, L"d_%08x%s", Fnv(c.dark) ^ GetTickCount(), PathFindExtensionW(c.dark));
            PathCombineW(dst, g_iconDir, nm);
            if (CopyFileW(c.dark, dst, FALSE)) lstrcpynW(it->icon_dark, dst, 512);
        }
        LoadItemIcon(it);
    }
    ScheduleSave();
    Kick();
}

/* ======================= "new shortcut" dialog (application / folder / web address, browser, icon) ======================= */
typedef struct { HWND hw; WCHAR loc[1024], name[128], icon[1024], browser[MAX_PATH]; int appmode, bsel, ok, done; } ScCtx;
#define SD_LOC 101
#define SD_APP 102
#define SD_DIR 103
#define SD_NAME 104
#define SD_ICON 105
#define SD_ICONB 106
#define SD_MODE0 111
#define SD_BROW 113

static const WCHAR *g_brNames[5] = { L"默认浏览器", L"Microsoft Edge", L"Google Chrome", L"Mozilla Firefox", L"自定义浏览器…" };

static void ScUpdateRows(ScCtx *c) {
    WCHAR t[1024];
    GetWindowTextW(GetDlgItem(c->hw, SD_LOC), t, 1024);
    StrTrimW(t, L" \t\"");
    BOOL url = IsUrlStr(t);
    static const int ids[] = { 120, SD_MODE0, SD_MODE0 + 1, 121, SD_BROW };
    for (int i = 0; i < 5; i++) { HWND w = GetDlgItem(c->hw, ids[i]); if (w) ShowWindow(w, url ? SW_SHOW : SW_HIDE); }
    HWND hint = GetDlgItem(c->hw, 123);
    if (hint) ShowWindow(hint, url ? SW_HIDE : SW_SHOW);
}

static LRESULT CALLBACK ScProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    ScCtx *c = (ScCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    LRESULT r;
    if (m == WM_ERASEBKGND) return DlgErase(h, (HDC)w);
    if (DlgCommon(m, w, l, &r)) return r;
    switch (m) {
    case WM_CREATE: {
        c = (ScCtx *)((CREATESTRUCTW *)l)->lpCreateParams;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)c);
        c->hw = h;
        CreateWindowW(L"STATIC", L"位置（应用路径、文件夹或网址）：", WS_CHILD | WS_VISIBLE, Sc(18), Sc(14), Sc(500), Sc(20), h, NULL, g_inst, NULL);
        CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, Sc(18), Sc(40), Sc(380), Sc(28), h, (HMENU)SD_LOC, g_inst, NULL);
        MkBtn(h, L"应用…", Sc(406), Sc(39), Sc(60), Sc(30), SD_APP, 0);
        MkBtn(h, L"文件夹…", Sc(472), Sc(39), Sc(70), Sc(30), SD_DIR, 0);
        CreateWindowW(L"STATIC", L"名称（可留空，默认取自文件名或域名）：", WS_CHILD | WS_VISIBLE, Sc(18), Sc(82), Sc(500), Sc(20), h, NULL, g_inst, NULL);
        CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, Sc(18), Sc(108), Sc(524), Sc(28), h, (HMENU)SD_NAME, g_inst, NULL);
        CreateWindowW(L"STATIC", L"网址打开方式：", WS_CHILD | WS_VISIBLE, Sc(18), Sc(152), Sc(160), Sc(20), h, (HMENU)120, g_inst, NULL);
        MkBtn(h, L"浏览器标签页", Sc(18), Sc(178), Sc(150), Sc(32), SD_MODE0, 0);
        MkBtn(h, L"应用窗口（独立窗口）", Sc(176), Sc(178), Sc(180), Sc(32), SD_MODE0 + 1, 0);
        CreateWindowW(L"STATIC", L"使用的浏览器：", WS_CHILD | WS_VISIBLE, Sc(18), Sc(222), Sc(160), Sc(20), h, (HMENU)121, g_inst, NULL);
        MkBtn(h, g_brNames[0], Sc(18), Sc(248), Sc(260), Sc(32), SD_BROW, 0);
        CreateWindowW(L"STATIC", L"提示：应用路径后面可直接写启动参数，例如  C:\\Tools\\app.exe -x", WS_CHILD | WS_VISIBLE, Sc(18), Sc(158), Sc(520), Sc(44), h, (HMENU)123, g_inst, NULL);
        CreateWindowW(L"STATIC", L"图标（可选：图片文件路径或图片链接）：", WS_CHILD | WS_VISIBLE, Sc(18), Sc(294), Sc(500), Sc(20), h, (HMENU)122, g_inst, NULL);
        CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL, Sc(18), Sc(320), Sc(440), Sc(28), h, (HMENU)SD_ICON, g_inst, NULL);
        MkBtn(h, L"浏览…", Sc(466), Sc(319), Sc(76), Sc(30), SD_ICONB, 0);
        MkBtn(h, L"创建", Sc(376), Sc(372), Sc(80), Sc(32), IDOK, BS_DEFPUSHBUTTON);
        MkBtn(h, L"取消", Sc(464), Sc(372), Sc(78), Sc(32), IDCANCEL, 0);
        c->appmode = 1;
        SegSet(h, SD_MODE0, 2, 1);
        SetFontAll(h);
        SetFocus(GetDlgItem(h, SD_LOC));
        ScUpdateRows(c);
        return 0;
    }
    case WM_COMMAND: {
        int id = LOWORD(w);
        if (id == SD_LOC && HIWORD(w) == EN_CHANGE) { ScUpdateRows(c); return 0; }
        if (HIWORD(w) != BN_CLICKED) return 0;
        if (id == SD_APP || id == SD_DIR) {
            WCHAR f[MAX_PATH] = L"";
            if (id == SD_APP) {
                OPENFILENAMEW o;
                ZeroMemory(&o, sizeof o);
                o.lStructSize = sizeof o; o.hwndOwner = h;
                o.lpstrFilter = L"应用程序与快捷方式\0*.exe;*.lnk;*.url\0所有文件\0*.*\0";
                o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.Flags = OFN_FILEMUSTEXIST | OFN_NODEREFERENCELINKS;
                if (GetOpenFileNameW(&o)) SetDlgItemTextW(h, SD_LOC, f);
            } else {
                BROWSEINFOW b;
                ZeroMemory(&b, sizeof b);
                b.hwndOwner = h; b.lpszTitle = L"选择文件夹"; b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
                LPITEMIDLIST pi = SHBrowseForFolderW(&b);
                if (pi) { if (SHGetPathFromIDListW(pi, f)) SetDlgItemTextW(h, SD_LOC, f); CoTaskMemFree(pi); }
            }
        } else if (id == SD_ICONB) {
            WCHAR f[MAX_PATH] = L"";
            OPENFILENAMEW o;
            ZeroMemory(&o, sizeof o);
            o.lStructSize = sizeof o; o.hwndOwner = h;
            o.lpstrFilter = L"图片 / 图标\0*.png;*.ico;*.jpg;*.jpeg;*.bmp;*.gif\0所有文件\0*.*\0";
            o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.Flags = OFN_FILEMUSTEXIST;
            if (GetOpenFileNameW(&o)) SetDlgItemTextW(h, SD_ICON, f);
        } else if (id == SD_MODE0 || id == SD_MODE0 + 1) { c->appmode = id - SD_MODE0; SegSet(h, SD_MODE0, 2, c->appmode); }
        else if (id == SD_BROW) {
            CmItem it[6];
            int n = 0;
            for (int k = 0; k < 5; k++) n = CmAdd(it, n, k + 1, g_brNames[k], c->bsel == k ? CM_CHECK : 0);
            RECT rc;
            GetWindowRect(GetDlgItem(h, SD_BROW), &rc);
            g_menuEdge = E_TOP;
            int r2 = ShowMenu(it, n, (rc.left + rc.right) / 2, rc.bottom + 2);
            g_menuEdge = -1;
            if (r2 > 0) {
                c->bsel = r2 - 1;
                if (c->bsel == 4) {
                    WCHAR f[MAX_PATH] = L"";
                    OPENFILENAMEW o;
                    ZeroMemory(&o, sizeof o);
                    o.lStructSize = sizeof o; o.hwndOwner = h;
                    o.lpstrFilter = L"浏览器程序\0*.exe\0"; o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.Flags = OFN_FILEMUSTEXIST;
                    if (GetOpenFileNameW(&o)) { lstrcpynW(c->browser, f, MAX_PATH); SetWindowTextW(GetDlgItem(h, SD_BROW), PathFindFileNameW(f)); }
                    else c->bsel = 0;
                } else SetWindowTextW(GetDlgItem(h, SD_BROW), g_brNames[c->bsel]);
                InvalidateRect(GetDlgItem(h, SD_BROW), NULL, TRUE);
            }
        } else if (id == IDOK) {
            GetDlgItemTextW(h, SD_LOC, c->loc, 1024);
            GetDlgItemTextW(h, SD_NAME, c->name, 128);
            GetDlgItemTextW(h, SD_ICON, c->icon, 1024);
            c->ok = 1; c->done = 1; DestroyWindow(h);
        } else if (id == IDCANCEL) { c->done = 1; DestroyWindow(h); }
        return 0;
    }
    case WM_CLOSE: c->done = 1; DestroyWindow(h); return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

static BOOL ShortcutDialog(HWND parent, Item *out) {
    ScCtx c;
    ZeroMemory(&c, sizeof c);
    RECT wa = g_mi.rcWork;
    RECT r = { 0, 0, Sc(560), Sc(420) };
    AdjustWindowRectEx(&r, WS_CAPTION | WS_SYSMENU, FALSE, WS_EX_TOPMOST);
    HWND h = CreateWindowExW(WS_EX_TOPMOST | WS_EX_DLGMODALFRAME, L"MiniDockShortcut", L"新建快捷方式", WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
                             (wa.left + wa.right - (r.right - r.left)) / 2, (wa.top + wa.bottom - (r.bottom - r.top)) / 2 - Sc(30), r.right - r.left, r.bottom - r.top, parent, NULL, g_inst, &c);
    if (!h) return FALSE;
    DarkTitleBar(h);
    if (parent) EnableWindow(parent, FALSE);
    SetForegroundWindow(h);
    MSG msg;
    while (!c.done && GetMessageW(&msg, NULL, 0, 0) > 0)
        if (!IsDialogMessageW(h, &msg)) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    if (parent) { EnableWindow(parent, TRUE); SetForegroundWindow(parent); }
    if (!c.ok) return FALSE;
    StrTrimW(c.loc, L" \t\"");
    if (!c.loc[0]) return FALSE;
    BOOL ok = FALSE;
    if (IsUrlStr(c.loc)) {
        if (BuildItem(out, c.loc)) {
            out->appmode = c.appmode;
            const WCHAR *bs[4] = { L"", L"edge", L"chrome", L"firefox" };
            lstrcpynW(out->args, c.bsel < 4 ? bs[c.bsel] : c.browser, 260);
            ok = TRUE;
        }
    } else {
        WCHAR path[1024], args[260] = L"";
        lstrcpynW(path, c.loc, 1024);
        if (GetFileAttributesW(path) == INVALID_FILE_ATTRIBUTES) {
            WCHAR *p = StrStrIW(path, L".exe");
            if (p && p[4] == L' ') { lstrcpynW(args, p + 5, 260); p[4] = 0; StrTrimW(args, L" "); }
        }
        if (BuildItem(out, path)) { lstrcpynW(out->args, args, 260); ok = TRUE; }
    }
    if (!ok) { MessageBoxW(parent, L"找不到这个位置，请检查路径或网址是否正确。", L"MiniDock", MB_OK | MB_ICONWARNING); return FALSE; }
    if (c.name[0]) { lstrcpynW(out->name, c.name, 128); CleanName(out->name); }
    StrTrimW(c.icon, L" \t\"");
    if (c.icon[0]) {
        WCHAR dst[MAX_PATH], nm[64];
        if (IsUrlStr(c.icon)) {
            wsprintfW(nm, L"c_%08x.png", Fnv(c.icon) ^ GetTickCount());
            PathCombineW(dst, g_iconDir, nm);
            lstrcpynW(out->icon_src, dst, 512);
            StartDownload(1, c.icon, dst);
        } else if (GetFileAttributesW(c.icon) != INVALID_FILE_ATTRIBUTES) {
            wsprintfW(nm, L"c_%08x%s", Fnv(c.icon) ^ GetTickCount(), PathFindExtensionW(c.icon));
            PathCombineW(dst, g_iconDir, nm);
            if (CopyFileW(c.icon, dst, FALSE)) { lstrcpynW(out->icon_src, dst, 512); LoadItemIcon(out); }
        }
    }
    return TRUE;
}

/* ======================= minimise / restore with a shrink-into-the-dock animation ======================= */
static HWINEVENTHOOK g_minHook;
static HWND g_anim, g_animWin;
static HTHUMBNAIL g_animTh;
static RECT g_animFrom, g_animTo;
static int g_animItem = -1, g_animMode;
static BOOL g_animBusy, g_animDirect;
static BOOL g_animFade;      /* icon-fly style: fade + slide instead of resizing every frame */
static BOOL g_fadeLayered;   /* we added WS_EX_LAYERED for the fade and must remove it afterwards */
static BOOL g_flyTopmost;    /* we raised the window for the flight and must drop it back */

static float AnimEase(float t);
/* --- genie: the window is captured once and warped down into its dock icon --- */
static BOOL g_genieOn;
static GpBitmap *g_genieSrc;
static HDC g_gnSrcDC;
static HBITMAP g_gnSrcBmp;
static HGDIOBJ g_gnSrcOld;
static BYTE *g_gnSrcBits;
static int g_genieSW, g_genieSH;
static HDC g_gnDC;
static HBITMAP g_gnBmp;
static HGDIOBJ g_gnOld;
static BYTE *g_gnBits;
static GpBitmap *g_gnSurf;
static GpGraphics *g_gnG;
static int g_gnW, g_gnH;
static RECT g_gnRect;

static void GenieFree(void) {
    if (g_gnG) { GdipDeleteGraphics(g_gnG); g_gnG = NULL; }
    if (g_gnSurf) { GdipDisposeImage(g_gnSurf); g_gnSurf = NULL; }
    if (g_gnDC) {
        if (g_gnOld) SelectObject(g_gnDC, g_gnOld);
        if (g_gnBmp) DeleteObject(g_gnBmp);
        DeleteDC(g_gnDC);
    }
    g_gnDC = NULL; g_gnBmp = NULL; g_gnOld = NULL; g_gnBits = NULL; g_gnW = g_gnH = 0;
    if (g_genieSrc) { GdipDisposeImage(g_genieSrc); g_genieSrc = NULL; }
    if (g_gnSrcDC) {
        if (g_gnSrcOld) SelectObject(g_gnSrcDC, g_gnSrcOld);
        if (g_gnSrcBmp) DeleteObject(g_gnSrcBmp);
        DeleteDC(g_gnSrcDC);
    }
    g_gnSrcDC = NULL; g_gnSrcBmp = NULL; g_gnSrcOld = NULL; g_gnSrcBits = NULL;
    g_genieOn = FALSE;
}

static BOOL GenieSurface(int w, int h) {
    if (g_gnSurf && w == g_gnW && h == g_gnH) return TRUE;
    if (g_gnG) { GdipDeleteGraphics(g_gnG); g_gnG = NULL; }
    if (g_gnSurf) { GdipDisposeImage(g_gnSurf); g_gnSurf = NULL; }
    if (g_gnDC) {
        if (g_gnOld) SelectObject(g_gnDC, g_gnOld);
        if (g_gnBmp) DeleteObject(g_gnBmp);
        DeleteDC(g_gnDC);
    }
    HDC scr = GetDC(NULL);
    g_gnDC = CreateCompatibleDC(scr);
    ReleaseDC(NULL, scr);
    if (!g_gnDC) return FALSE;
    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof bi);
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w; bi.bmiHeader.biHeight = -h; bi.bmiHeader.biPlanes = 1; bi.bmiHeader.biBitCount = 32;
    g_gnBmp = CreateDIBSection(g_gnDC, &bi, DIB_RGB_COLORS, (void **)&g_gnBits, NULL, 0);
    if (!g_gnBmp) return FALSE;
    g_gnOld = SelectObject(g_gnDC, g_gnBmp);
    if (GdipCreateBitmapFromScan0(w, h, w * 4, PXF_PARGB, g_gnBits, &g_gnSurf) != 0) return FALSE;
    if (GdipGetImageGraphicsContext(g_gnSurf, &g_gnG) != 0) return FALSE;
    SetupG(g_gnG);
    g_gnW = w; g_gnH = h;
    return TRUE;
}

/* t: 0 = window in place, 1 = collapsed into the dock icon */
static void GenieFrame(float t) {
    if (!g_genieOn || !g_genieSrc) return;
    int bw = g_gnRect.right - g_gnRect.left, bh = g_gnRect.bottom - g_gnRect.top;
    if (!GenieSurface(bw, bh)) return;
    float e = AnimEase(t);
    float winX = (float)(g_animFrom.left - g_gnRect.left), winY = (float)(g_animFrom.top - g_gnRect.top);
    float winW = (float)(g_animFrom.right - g_animFrom.left), winH = (float)(g_animFrom.bottom - g_animFrom.top);
    float icx = (float)(g_animTo.left + g_animTo.right) / 2.f - g_gnRect.left;
    float icy = (float)(g_animTo.top + g_animTo.bottom) / 2.f - g_gnRect.top;
    float iw = (float)(g_animTo.right - g_animTo.left), ih = (float)(g_animTo.bottom - g_animTo.top);
    float wcx = winX + winW / 2.f;
    GdipGraphicsClear(g_gnG, 0);
    const int N = 40;
    for (int i = 0; i < N; i++) {
        float v0 = (float)i / N, v1 = (float)(i + 1) / N;
        float wTop = winW + (iw - winW) * e;          /* the top edge keeps its width longer */
        float wBot = winW + (iw - winW) * (e * e);    /* the bottom collapses into the icon first */
        float wv = wTop + (wBot - wTop) * v1;
        float cxv = wcx + (icx - wcx) * e * (0.25f + 0.75f * v1);
        float hTot = winH + (ih - winH) * e;
        float yd = winY + (icy - ih - winY) * e + v0 * hTot;   /* whole window slides down to the icon */
        float hd = hTot / N;
        if (wv < 4.f) wv = 4.f;
        if (hd < 1.f) hd = 1.f;
        GdipDrawImageRectRect(g_gnG, g_genieSrc, cxv - wv / 2.f, yd, wv, hd + 1.f,
                              0, v0 * g_genieSH, (REAL)g_genieSW, (v1 - v0) * g_genieSH, 2, g_attr, NULL, NULL);
    }
    POINT pt = { g_gnRect.left, g_gnRect.top };
    SIZE sz = { bw, bh };
    POINT src = { 0, 0 };
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(g_anim, NULL, &pt, &sz, g_gnDC, &src, 0, &bf, ULW_ALPHA);
}

/* Restore a window for the animation. SetWindowPlacement first (it honours "restore to maximised"),
   then ShowWindow, then verify. Some windows ignore a plain SW_SHOWMAXIMIZED while they are still
   finishing their own minimise - that is what made a dock click animate but leave the window minimised. */
static BOOL g_coverUnderRestore;   /* the snapshot already covers the restore handoff */
static void RestoreWindowForAnimation(HWND w, BOOL max) {
    if (!w || !IsWindow(w)) return;
    if (IsIconic(w)) {
        WINDOWPLACEMENT wp = { sizeof wp };
        if (GetWindowPlacement(w, &wp)) {
            wp.showCmd = max ? SW_SHOWMAXIMIZED : SW_SHOWNOACTIVATE;
            SetWindowPlacement(w, &wp);
        }
        if (IsIconic(w)) ShowWindow(w, SW_SHOWNOACTIVATE);
    }
    if (max && !IsZoomed(w)) ShowWindow(w, SW_MAXIMIZE);
    if (IsIconic(w)) {
        ShowWindowAsync(w, SW_RESTORE);
        PostMessageW(w, WM_SYSCOMMAND, SC_RESTORE, 0);   /* last resort for a window that ignored ShowWindow */
    }
}

extern void WgcCancelCapture(void);
static void AnimFinish(void) {
    if (g_animTh && g_animMode == 1) WgcCancelCapture();
    /* The real window state is committed by the caller at the correct edge of the
       animation. This routine releases the private preview/token and its resources;
       it never restores or resizes the app HWND. */
    if (g_genieOn) GenieFree();
    if (g_anim) ShowWindow(g_anim, SW_HIDE); /* never expose an empty popup after removing its thumbnail */
    if (g_animTh) { DwmUnregisterThumbnail(g_animTh); g_animTh = NULL; }
    if (g_anim) { DestroyWindow(g_anim); g_anim = NULL; }
    if (g_animWin && IsWindow(g_animWin)) {
        if (g_animDirect && g_animFade && g_animMode == 1) SetLayeredWindowAttributes(g_animWin, 0, 255, LWA_ALPHA);
        /* keep WS_EX_LAYERED here: removing it repaints the window and that is the flicker at the end of the fly-out */
        if (g_flyTopmost)
            SetWindowPos(g_animWin, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    g_animFade = FALSE; g_fadeLayered = FALSE; g_flyTopmost = FALSE;
    g_animWin = NULL; g_animDirect = FALSE;
    if (g_animItem >= 0 && g_animItem < nItems && cfg.bounce && g_animMode == 0) { items[g_animItem].bounce = GetTickCount() | 1; Kick(); }
    g_animItem = -1;
}

static float AnimEase(float t) {
    if (cfg.minStyle == MIN_STYLE_ICON_FLY) { float c1 = 1.05f, c3 = c1 + 1, q = t - 1; return 1 + c3 * q * q * q + c1 * q * q; }   /* phone-like: soft overshoot */
    return t < 0.5f ? 4 * t * t * t : 1 - powf(-2 * t + 2, 3) / 2;                                                  /* Mac-like: smooth in-out */
}

static void AnimFrame(float t) {
    if (!g_anim) return;
    if (g_genieOn) { GenieFrame(t); return; }
    float e = AnimEase(t);
    if (g_animDirect && g_animWin && IsWindow(g_animWin)) {
        float fw = (float)(g_animFrom.right - g_animFrom.left), fh = (float)(g_animFrom.bottom - g_animFrom.top);
        float tw = (float)(g_animTo.right - g_animTo.left), th = (float)(g_animTo.bottom - g_animTo.top);
        if (g_animFade) {
            /* icon-fly style: keep the final size and fly the whole window with a fade.
               SetLayeredWindowAttributes is expensive, so only touch it when alpha really changes. */
            static int lastA = -1;
            float cx0 = (g_animFrom.left + g_animFrom.right) / 2.f, cy0 = (g_animFrom.top + g_animFrom.bottom) / 2.f;
            float cx1 = (g_animTo.left + g_animTo.right) / 2.f, cy1 = (g_animTo.top + g_animTo.bottom) / 2.f;
            float cx = cx0 + (cx1 - cx0) * e, cy = cy0 + (cy1 - cy0) * e;
            float a = g_animMode == 0 ? 1.f - e : e;
            int ai = (int)(255 * (a < 0 ? 0 : a > 1 ? 1 : a));
            if (g_fadeLayered && (lastA < 0 || abs(ai - lastA) >= 6)) { SetLayeredWindowAttributes(g_animWin, 0, (BYTE)ai, LWA_ALPHA); lastA = ai; }
            SetWindowPos(g_animWin, HWND_TOPMOST, (int)(cx - tw / 2 + 0.5f), (int)(cy - th / 2 + 0.5f),
                         (int)(tw + 0.5f), (int)(th + 0.5f),
                         SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
            return;
        }
        float w = fw + (tw - fw) * e, h = fh + (th - fh) * e;
        float x0 = (float)g_animFrom.left, y0 = (float)g_animFrom.top;
        float x1 = (float)g_animTo.left, y1 = (float)g_animTo.top;
        float x = x0 + (x1 - x0) * e, y = y0 + (y1 - y0) * e;
        if (w < 8.f) w = 8.f; if (h < 8.f) h = 8.f;
        /* Move/resize the real HWND itself. HWND_TOPMOST keeps the flying window above all others. */
        SetWindowPos(g_animWin, HWND_TOPMOST, (int)(x + 0.5f), (int)(y + 0.5f),
                     (int)(w + 0.5f), (int)(h + 0.5f),
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
        return;
    }
    float fw = (float)(g_animFrom.right - g_animFrom.left), fh = (float)(g_animFrom.bottom - g_animFrom.top);
    float tw = (float)(g_animTo.right - g_animTo.left), th = (float)(g_animTo.bottom - g_animTo.top);
    float w = fw + (tw - fw) * e, h = fh + (th - fh) * e;
    float cx0 = (g_animFrom.left + g_animFrom.right) / 2.f, cy0 = (g_animFrom.top + g_animFrom.bottom) / 2.f;
    float cx1 = (g_animTo.left + g_animTo.right) / 2.f, cy1 = (g_animTo.top + g_animTo.bottom) / 2.f;
    float cx = cx0 + (cx1 - cx0) * e, cy = cy0 + (cy1 - cy0) * e;
    if (w < 8) w = 8;
    if (h < 8) h = 8;
    DWM_THUMBNAIL_PROPERTIES tp;
    ZeroMemory(&tp, sizeof tp);
    tp.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_OPACITY | DWM_TNP_VISIBLE;
    tp.rcDestination.right = (int)(w + 0.5f); tp.rcDestination.bottom = (int)(h + 0.5f);
    tp.opacity = 255; /* keep the compress-into-Dock effect opaque, as with the real-window path */
    tp.fVisible = TRUE;
    DwmUpdateThumbnailProperties(g_animTh, &tp);
    SetWindowPos(g_anim, g_hwnd, (int)(cx - w / 2 + 0.5f), (int)(cy - h / 2 + 0.5f), (int)(w + 0.5f), (int)(h + 0.5f),
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING | SWP_SHOWWINDOW);   /* show only after content is ready */
}

static int ItemOfWin(HWND h) {
    DWORD key;
    /* separate-window icons know their exact window */
    for (int i = 0; i < nItems; i++) if ((items[i].type == T_WIN || items[i].type == T_TRAY) && items[i].hwnd == h) return i;
    if (BoundKey(h, &key) >= 0) { int i = ItemOfKey(key); if (i >= 0) return i; }
    WinId w;
    WinIdOf(h, &w);
    if (cfg.groupRunning) for (int i = 0; i < nItems; i++)
        if (items[i].temp && items[i].type == T_FILE && ItemMatchExact(&items[i], &w)) return i;
    for (int i = 0; i < nItems; i++) if (!items[i].temp && items[i].type != T_WIN && ItemMatchAny(&items[i], &w)) return i;
    /* an app that is only shown because it is running (temporary icon) has its icon on the dock too:
       without this the minimise animation flew to the taskbar instead of the dock (a temp icon was skipped) */
    for (int i = 0; i < nItems; i++) if (items[i].temp && items[i].type != T_WIN && items[i].type != T_SEP && ItemMatchAny(&items[i], &w)) return i;
    return -1;
}

/* the on-screen rectangle the window has (or will have) when shown; a window that was maximised comes back MAXIMISED, so the
   animation must end at the work area, not at the small "normal" rectangle (that was the source of the double jump) */
static BOOL WindowShownRect(HWND w, RECT *r) {
    if (IsIconic(w)) {
        WINDOWPLACEMENT wp = { sizeof wp };
        if (!GetWindowPlacement(w, &wp)) return FALSE;
        if (wp.flags & WPF_RESTORETOMAXIMIZED) {
            MONITORINFO mi = { sizeof mi };
            GetMonitorInfoW(MonitorFromWindow(w, MONITOR_DEFAULTTONEAREST), &mi);
            *r = mi.rcWork;
            /* DWM maximised windows stick out by the resize border on all four sides */
            int fb = GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXPADDEDBORDER);
            InflateRect(r, fb, fb);
            if (r->left < mi.rcMonitor.left) r->left = mi.rcMonitor.left;
            if (r->top < mi.rcMonitor.top) r->top = mi.rcMonitor.top;
            if (r->right > mi.rcMonitor.right) r->right = mi.rcMonitor.right;
            if (r->bottom > mi.rcMonitor.bottom) r->bottom = mi.rcMonitor.bottom;
        } else {
            *r = wp.rcNormalPosition;
            if (!(GetWindowLongW(w, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) {
                MONITORINFO mi = { sizeof mi };
                if (GetMonitorInfoW(MonitorFromWindow(w, MONITOR_DEFAULTTONEAREST), &mi))
                    OffsetRect(r, mi.rcWork.left - mi.rcMonitor.left, mi.rcWork.top - mi.rcMonitor.top);
            }
        }
    } else GetWindowRect(w, r); /* direct HWND animation uses the outer window rect, not DWM frame bounds */
    return (r->right - r->left) >= 40 && (r->bottom - r->top) >= 40 && r->left > -20000;
}

/* mode 0: shrink into the icon; mode 1: grow out of the icon. */
static BOOL FlyBegin(HWND w, RECT from, RECT to, int mode, int item) {
    if (g_anim || g_animBusy || !cfg.minAnim || !w || !IsWindow(w)) return FALSE;
    BOOL smooth = cfg.minStyle == MIN_STYLE_SMOOTH;
    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT;
    if (smooth) exStyle |= WS_EX_TOPMOST;
    g_anim = CreateWindowExW(exStyle,
                             CLS_ANIM, L"", WS_POPUP, 0, 0, 1, 1,
                             NULL, NULL, g_inst, NULL);
    if (!g_anim) return FALSE;
    g_animFrom = from; g_animTo = to; g_animItem = item; g_animMode = mode; g_animWin = w;
    g_animFade = (cfg.minStyle == MIN_STYLE_ICON_FLY);
    g_fadeLayered = FALSE;
    g_flyTopmost = FALSE;
    g_animDirect = FALSE;
    if (smooth && SUCCEEDED(DwmRegisterThumbnail(g_anim, w, &g_animTh)) && g_animTh) {
        SIZE source = {0};
        DWM_THUMBNAIL_PROPERTIES preview = {0};
        preview.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_SOURCECLIENTAREAONLY;
        SetRect(&preview.rcDestination, 0, 0, 1, 1);
        preview.fVisible = FALSE;
        preview.fSourceClientAreaOnly = FALSE;
        if (SUCCEEDED(DwmQueryThumbnailSourceSize(g_animTh, &source)) && source.cx > 1 && source.cy > 1 &&
            SUCCEEDED(DwmUpdateThumbnailProperties(g_animTh, &preview))) {
            if (mode == 0 && !IsIconic(w)) {
                RECT visible;
                if (SUCCEEDED(DwmGetWindowAttribute(w, 9, &visible, sizeof visible))) g_animFrom = visible;
            }
            AnimFrame(0);
            return TRUE;
        }
    }
    if (g_animTh) { DwmUnregisterThumbnail(g_animTh); g_animTh = NULL; }
    g_flyTopmost = (GetWindowLongW(w, GWL_EXSTYLE) & WS_EX_TOPMOST) == 0;
    if (g_animFade) {
        LONG ex = GetWindowLongW(w, GWL_EXSTYLE);
        if (!(ex & WS_EX_LAYERED)) { SetWindowLongW(w, GWL_EXSTYLE, ex | WS_EX_LAYERED); g_fadeLayered = TRUE; }
        SetLayeredWindowAttributes(w, 0, 255, LWA_ALPHA);
    }
    g_animDirect = TRUE;
    AnimFrame(0);
    return TRUE;
}

static BOOL HdrActive(void);
extern BOOL WgcPrimeHdrCapture(HWND hwnd, int w, int h);
extern void WgcDrainHdrCapture(void);

/* Pace the private preview against DWM; timer resolution is raised only during the flight. */
static void FlyRun(float ms) {
    BOOL canPrime = g_animMode == 1 && !g_animDirect;
    BOOL primed = FALSE;
    LARGE_INTEGER fq, t0, t;
    QueryPerformanceFrequency(&fq);
    QueryPerformanceCounter(&t0);
    g_animBusy = TRUE;
    BOOL highResolution = !g_animDirect && timeBeginPeriod(1) == TIMERR_NOERROR;
    for (;;) {
        QueryPerformanceCounter(&t);
        float el = (float)((double)(t.QuadPart - t0.QuadPart) * 1000.0 / (double)fq.QuadPart) / ms;
        if (el >= 1.f || !g_anim || !IsWindow(g_animWin)) break;
        AnimFrame(el);
        if (!g_animDirect) {
            MSG paint;
            while (PeekMessageW(&paint, NULL, WM_PAINT, WM_PAINT, PM_REMOVE)) DispatchMessageW(&paint);
        }
        if (FAILED(DwmFlush())) Sleep(8);
        /* Give the preview time to appear before queuing capture work. */
        if (canPrime && el >= 0.10f) {
            canPrime = FALSE;
            primed = HdrActive() && WgcPrimeHdrCapture(g_anim,
                g_animTo.right - g_animTo.left, g_animTo.bottom - g_animTo.top);
        }
        if (primed) WgcDrainHdrCapture();
    }
    AnimFrame(1.f);   /* land exactly on the final rectangle before removing the preview */
    g_animBusy = FALSE;
    if (highResolution) timeEndPeriod(1);
    /* The caller commits the real window state before removing the covering preview. */
}

typedef struct { const Item *it; DWORD key; BOOL isUrl; HWND first; } TopCtx;
static BOOL CALLBACK TopProc(HWND h, LPARAM lp) {
    TopCtx *c = (TopCtx *)lp;
    if (!IsAppWindow(h) || h == g_hwnd) return TRUE;
    if (c->isUrl) { DWORD k; if (BoundKey(h, &k) < 0 || k != c->key) return TRUE; }
    else {
        WinId w; WinIdOf(h, &w);
        if (c->it->temp && c->it->type == T_FILE) { if (!ItemMatchExact(c->it, &w)) return TRUE; }
        else if (!ItemMatchAny(c->it, &w)) return TRUE;
    }
    c->first = h;
    return FALSE;
}

/* A compositor restore leaves the real window minimized until the preview lands.
   The compatibility path instead moves the real HWND from the icon while restoring it. */
static BOOL PrepareRestoreAtIcon(HWND w, RECT icon, BOOL max, RECT *target) {
    if (!w || !IsWindow(w)) return FALSE;
    if (!g_animDirect) {
        /* Animate to visible bounds from the start, not the invisible resize border.
           Retargeting an outer rect to a DWM frame after the last frame caused a snap. */
        SIZE source = {0};
        BOOL sourceKnown = SUCCEEDED(DwmQueryThumbnailSourceSize(g_animTh, &source));
        if (max) {
            MONITORINFO mi = { sizeof mi };
            if (!GetMonitorInfoW(MonitorFromWindow(w, MONITOR_DEFAULTTONEAREST), &mi)) return FALSE;
            *target = mi.rcWork;
            if (!(GetWindowLongW(w, GWL_STYLE) & (WS_CAPTION | WS_THICKFRAME)) && sourceKnown &&
                source.cx >= mi.rcMonitor.right - mi.rcMonitor.left - 2 &&
                source.cy >= mi.rcMonitor.bottom - mi.rcMonitor.top - 2) *target = mi.rcMonitor;
        } else {
            if (!WindowShownRect(w, target)) return FALSE;
            if (sourceKnown) {
                int dx = target->right - target->left - source.cx;
                int dy = target->bottom - target->top - source.cy;
                if (source.cx > 1 && source.cy > 1 && dx >= 0 && dy >= 0 && dx <= Sc(64) && dy <= Sc(64)) {
                    int left = dx / 2, bottom = left < dy ? left : dy;
                    target->left += left; target->right -= dx - left;
                    target->top += dy - bottom; target->bottom -= bottom;
                }
            }
        }
        return TRUE; /* leave the actual window minimized until flight ends */
    }
    WINDOWPLACEMENT wp = { sizeof wp };
    if (!GetWindowPlacement(w, &wp)) return FALSE;
    if (max) {
        wp.showCmd = SW_SHOWMAXIMIZED;
        SetWindowPlacement(w, &wp);
        ShowWindow(w, SW_SHOWMAXIMIZED);
        ShowWindow(w, SW_HIDE);
        if (!GetWindowRect(w, target)) return FALSE;
    } else {
        ShowWindow(w, SW_SHOWNOACTIVATE);
        ShowWindow(w, SW_HIDE);
        if (!GetWindowRect(w, target)) return FALSE;
    }
    /* Back to a normal window *while hidden*: a maximised HWND ignores SetWindowPos sizing, which is
       exactly why the fly-out used to be skipped for maximised windows and the window just appeared. */
    wp.showCmd = SW_SHOWNOACTIVATE;
    SetWindowPlacement(w, &wp);
    if (IsZoomed(w)) ShowWindow(w, SW_RESTORE);
    ShowWindow(w, SW_HIDE);
    if (g_animFade) {
        int tw = target->right - target->left, th = target->bottom - target->top;
        int cx = (icon.left + icon.right) / 2, cy = (icon.top + icon.bottom) / 2;
        SetWindowPos(w, NULL, cx - tw / 2, cy - th / 2, tw, th,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
        if (g_fadeLayered) SetLayeredWindowAttributes(w, 0, 0, LWA_ALPHA);
    } else {
        SetWindowPos(w, NULL, icon.left, icon.top,
                     icon.right - icon.left, icon.bottom - icon.top,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
    }
    ShowWindow(w, SW_SHOWNOACTIVATE);
    SetForegroundWindow(w);
    return TRUE;
}

static void CommitMinimizeKeepPlacement(HWND w, const WINDOWPLACEMENT *original) {
    if (!w || !IsWindow(w)) return;
    ShowWindow(w, SW_MINIMIZE);
    if (original) {
        WINDOWPLACEMENT wp = *original;
        wp.length = sizeof wp;
        wp.showCmd = SW_SHOWMINIMIZED;
        SetWindowPlacement(w, &wp); /* keep the pre-animation restore rectangle */
    }
}


static void RestoreUnderPreview(HWND w, BOOL max) {
    if (!w || !IsWindow(w)) return;
    g_animBusy = TRUE;
    SetTransitionsDisabled(w, TRUE, TRUE); /* refresh immediately before committing the restore */
    RestoreWindowForAnimation(w, max);
    SetForegroundWindow(w); /* activation and its non-client repaint stay under the cover */
    DWORD_PTR ignored;
    if (!g_coverUnderRestore) {
        SendMessageTimeoutW(w, WM_PAINT, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT, 40, &ignored);
        DwmFlush(); DwmFlush(); /* retain the landed preview across the paint/activation handoff */
    }
    g_animBusy = FALSE;
}

/* ---- first-frame handoff: freeze the last preview frame until the restored window paints ----
   A restored GPU window (Firefox/WebRender, Chromium, XAML) repaints its own surface from
   black for a few frames. The DWM thumbnail mirrors that live, so keeping the preview up
   cannot hide it. Instead the landed preview frame is copied once, shown in a no-activate
   layered window above the restored window, and removed as soon as a tiny transparent probe
   hole in the snapshot shows painted content again. The snapshot is freed immediately. */
/* HDR snapshot: WGC capture + DirectComposition scRGB (hdr_snap.cpp) */
extern BOOL HdrSnapShow(HWND src, int x, int y, int w, int h);
extern void WgcPrewarm(void);
extern void WgcShutdown(void);
extern void HdrSnapDestroy(void);
extern HWND HdrSnapWindow(void);
extern void HdrSnapProbeInfo(int *px, int *py, int *ref, int *wx, int *wy);

/* HDR output check + WGC snapshot grab (wgc_capture.c) */
extern BOOL WgcGrabWindow(HWND hwnd, void *bits, int w, int h);
static BOOL HdrActive(void) {
    UINT32 npath = 0, nmode = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &npath, &nmode) != ERROR_SUCCESS) return FALSE;
    DISPLAYCONFIG_PATH_INFO *paths = (DISPLAYCONFIG_PATH_INFO *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)npath * sizeof(DISPLAYCONFIG_PATH_INFO));
    DISPLAYCONFIG_MODE_INFO *modes = (DISPLAYCONFIG_MODE_INFO *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)nmode * sizeof(DISPLAYCONFIG_MODE_INFO));
    BOOL hdr = FALSE;
    if (paths && modes && QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &npath, paths, &nmode, modes, NULL) == ERROR_SUCCESS) {
        for (UINT32 i = 0; i < npath; i++) {
            DISPLAYCONFIG_GET_ADVANCED_COLOR_INFO a;
            ZeroMemory(&a, sizeof a);
            a.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_ADVANCED_COLOR_INFO;
            a.header.size = sizeof a;
            a.header.adapterId = paths[i].targetInfo.adapterId;
            a.header.id = paths[i].targetInfo.id;
            if (DisplayConfigGetDeviceInfo(&a.header) == ERROR_SUCCESS && ((a.value >> 1) & 1)) { hdr = TRUE; break; }
        }
    }
    if (paths) HeapFree(GetProcessHeap(), 0, paths);
    if (modes) HeapFree(GetProcessHeap(), 0, modes);
    return hdr;
}

typedef struct {
    HWND wnd;
    HDC mem;
    HBITMAP dib;
    HGDIOBJ old;
    void *bits;
    RECT rect;
    int probeX, probeY;   /* probe hole inside the snapshot */
    int probeRef;         /* average luminance of that area in the frozen frame */
    BOOL probe;
    BOOL hdrMode;         /* snapshot is a DirectComposition HDR window (hdr_snap.cpp) */
} SnapCtx;
static SnapCtx g_snap;

static int SnapLum(const BYTE *p) { return (p[2] * 299 + p[1] * 587 + p[0] * 114) / 1000; }
static int SnapColorLum(COLORREF c) { return (GetRValue(c) * 299 + GetGValue(c) * 587 + GetBValue(c) * 114) / 1000; }

static void SnapFree(void) {
    if (g_snap.hdrMode) { HdrSnapDestroy(); ZeroMemory(&g_snap, sizeof g_snap); return; }
    if (g_snap.wnd) { ShowWindow(g_snap.wnd, SW_HIDE); DestroyWindow(g_snap.wnd); }
    if (g_snap.mem) {
        if (g_snap.old) SelectObject(g_snap.mem, g_snap.old);
        if (g_snap.dib) DeleteObject(g_snap.dib);
        DeleteDC(g_snap.mem);
    }
    ZeroMemory(&g_snap, sizeof g_snap);
}

static BOOL SnapBegin(const RECT *r) {
    if (!r) return FALSE;
    int w = r->right - r->left, h = r->bottom - r->top;
    if (w < 32 || h < 32) return FALSE;
    SnapFree();
    HDC screen = GetDC(NULL);
    if (!screen) return FALSE;
    /* HDR: show the captured HDR frame as real scRGB content via DirectComposition, so DWM
       does not re-map an SDR snapshot and turn the window white. */
    BOOL hdr = HdrActive();
    if (hdr && g_anim && HdrSnapShow(g_anim, r->left, r->top, w, h)) {
        int px = 0, py = 0, pref = 0, wx = 0, wy = 0;
        HdrSnapProbeInfo(&px, &py, &pref, &wx, &wy);
        g_snap.hdrMode = TRUE;
        g_snap.wnd = HdrSnapWindow();
        g_snap.probe = TRUE;
        g_snap.probeX = px; g_snap.probeY = py; g_snap.probeRef = pref;
        g_snap.rect = *r;
        ReleaseDC(NULL, screen);
        return TRUE;
    }
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP dib = NULL;
    void *bits = NULL;
    HGDIOBJ old = NULL;
    HWND wnd = NULL;
    BOOL ok = FALSE;
    if (mem) {
        BITMAPINFO bi;
        ZeroMemory(&bi, sizeof bi);
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = w;
        bi.bmiHeader.biHeight = -h;
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        dib = CreateDIBSection(mem, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
    }
    if (dib && bits) {
        old = SelectObject(mem, dib);
        DwmFlush();   /* the landed preview frame must be on screen before the copy */
        BOOL grabbed = FALSE;
        /* The GDI screen grab whitens colours on HDR outputs (same issue as normal
           screenshots). Capture the preview window through Windows.Graphics.Capture
           instead: it returns the SDR content the user actually sees in the flight. */
        if (hdr && g_anim) grabbed = WgcGrabWindow(g_anim, bits, w, h);
        if (!grabbed) {
            BitBlt(mem, 0, 0, w, h, screen, r->left, r->top, SRCCOPY);
            GdiFlush();
        }
        BYTE *b = (BYTE *)bits;
        INT_PTR np = (INT_PTR)w * h;
        for (INT_PTR i = 0; i < np; i++) b[i * 4 + 3] = 255;   /* the screen copy has no alpha */
        /* bright 16x16 cell: cut a small transparent probe hole there */
        int bx = -1, by = -1, best = 0;
        for (int y = 8; y + 16 < h; y += 16)
            for (int x = 8; x + 16 < w; x += 16) {
                int sum = 0;
                for (int yy = 0; yy < 16; yy += 4)
                    for (int xx = 0; xx < 16; xx += 4) sum += SnapLum(b + (((INT_PTR)(y + yy) * w) + x + xx) * 4);
                if (sum > best) { best = sum; bx = x; by = y; }
            }
        if (bx >= 0 && best >= 205 * 16 * 3) {   /* average cell luminance of at least ~48 */
            int ref = 0;
            for (int yy = 0; yy < 4; yy++)
                for (int xx = 0; xx < 4; xx++) {
                    BYTE *q = b + (((INT_PTR)(by + yy) * w) + bx + xx) * 4;
                    ref += SnapLum(q);
                    q[3] = 0;
                }
            g_snap.probe = TRUE;
            g_snap.probeX = bx + 1;
            g_snap.probeY = by + 1;
            g_snap.probeRef = ref / 16;
        }
        wnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
                              CLS_ANIM, L"", WS_POPUP, r->left, r->top, w, h, NULL, NULL, g_inst, NULL);
        if (wnd) {
            POINT pt = { r->left, r->top }, src = { 0, 0 };
            SIZE sz = { w, h };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            if (UpdateLayeredWindow(wnd, NULL, &pt, &sz, mem, &src, 0, &bf, ULW_ALPHA)) {
                ShowWindow(wnd, SW_SHOWNA);
                g_snap.wnd = wnd;
                g_snap.mem = mem;
                g_snap.dib = dib;
                g_snap.old = old;
                g_snap.bits = bits;
                g_snap.rect = *r;
                ok = TRUE;
            } else DestroyWindow(wnd);
        }
    }
    if (!ok) {
        if (mem) {
            if (dib) {
                if (old) SelectObject(mem, old);
                DeleteObject(dib);
            }
            DeleteDC(mem);
        }
    }
    ReleaseDC(NULL, screen);
    if (ok) {
        DwmFlush();   /* make sure the snapshot is composed before the window is restored */
            }
    return ok;
}

static BOOL SnapPainted(void) {
    if (!g_snap.wnd) return TRUE;
    if (!g_snap.probe) return FALSE;
    HDC screen = GetDC(NULL);
    if (!screen) return TRUE;
    int sum = 0, n = 0;
    for (int yy = 0; yy < 4; yy += 2)
        for (int xx = 0; xx < 4; xx += 2) {
            COLORREF c = GetPixel(screen, g_snap.rect.left + g_snap.probeX + xx, g_snap.rect.top + g_snap.probeY + yy);
            if (c != CLR_INVALID) { sum += SnapColorLum(c); n++; }
        }
    ReleaseDC(NULL, screen);
    if (!n) return TRUE;
    int need = g_snap.probeRef / 3;
    if (need < 18) need = 18;
    return (sum / n) >= need;
}

static void SnapWaitPaint(DWORD maxMs) {
    if (!g_snap.wnd) return;
    if (!g_snap.probe) {   /* a mostly dark frame: no reliable probe, keep the cover only briefly */
        Sleep(120);
        return;
    }
    DWORD t0 = GetTickCount();
    for (;;) {
        if (SnapPainted()) break;
        if (GetTickCount() - t0 >= maxMs) break;
        MSG m;
        while (PeekMessageW(&m, NULL, 0, 0, PM_REMOVE)) { TranslateMessage(&m); DispatchMessageW(&m); }
        Sleep(6);
    }
}

/* click on a window: minimised -> restore, frontmost -> minimise, behind others -> bring to front */
static HWND g_selfMin;
static DWORD g_selfMinTick;
static void ToggleWindowIdx(HWND w, int idx) {
    LogEvent(IsIconic(w) ? L"window-restore-request" : L"window-toggle-request", (DWORD)(idx < 0 ? 0 : idx));
    RECT icon = { 0, 0, 0, 0 };
    if (idx >= 0) IconScreenRect(idx, &icon);
    if (IsIconic(w)) {
        RECT target = {0};
        BOOL max = FALSE;
        WINDOWPLACEMENT original = { sizeof original };
        if (GetWindowPlacement(w, &original)) max = (original.flags & WPF_RESTORETOMAXIMIZED) != 0;
        BOOL canFly = cfg.minAnim && idx >= 0 &&
                      (icon.right - icon.left) > 4 && (icon.bottom - icon.top) > 4;
        if (canFly && FlyBegin(w, icon, icon, 1, idx)) {
            if (!PrepareRestoreAtIcon(w, icon, max, &target)) {
                AnimFinish();
                RestoreWindowForAnimation(w, max);
                SetForegroundWindow(w);
                DisableTransitions(w, FALSE);
                return;
            }
            /* The direct path captures the restored outer rectangle; the compositor path
               computes its visible landing rectangle before the flight begins. */
            g_animTo = target;
            BOOL fade = g_animFade;
            BOOL compositor = !g_animDirect;
            FlyRun((float)(cfg.animSpeed < 200 ? 200 : cfg.animSpeed > 1000 ? 1000 : cfg.animSpeed));
            /* A live thumbnail cannot hide the black frames the restored app paints while it
               rebuilds its surface. Freeze the landed preview frame over the restored window
               and drop it once the window has painted again. */
            BOOL covered = FALSE;
            g_coverUnderRestore = FALSE;
            if (compositor) {
                /* freeze the landed preview frame over the restored window, then drop it
                   once the window has painted again */
                covered = SnapBegin(&g_animTo);
                g_coverUnderRestore = covered;
                RestoreUnderPreview(w, max);
            }
            AnimFinish();
            if (covered) {
                SnapWaitPaint(350);
                SnapFree();
                g_coverUnderRestore = FALSE;
            }
            original.length = sizeof original;
            original.showCmd = max ? SW_SHOWMAXIMIZED : SW_SHOWNOACTIVATE;
            if (!compositor && fade) {
                /* maximise changes the window state, so do it only for maximised windows and while transparent;
                   a normal window is already at its final rectangle, so re-applying placement would make it jump */
                if (max) {
                    SetLayeredWindowAttributes(w, 0, 0, LWA_ALPHA);
                    SetWindowPlacement(w, &original);
                    for (int k = 1; k <= 6; k++) { SetLayeredWindowAttributes(w, 0, (BYTE)(255 * k / 6), LWA_ALPHA); Sleep(10); }
                }
            } else if (!compositor && max) {
                /* one SetWindowPlacement does placement + maximise; a second ShowWindow(SW_MAXIMIZE)
                   resized the window again and left the black client area behind */
                SetWindowPlacement(w, &original);
            }
            if (!compositor) SetForegroundWindow(w);
            LogEvent(L"restore-flyout-start", (DWORD)idx);
        } else {
            RestoreWindowForAnimation(w, max);
            SetForegroundWindow(w);
            DisableTransitions(w, FALSE);
        }
    } else {
        HWND fg = GetForegroundWindow();
        if (fg && (fg == w || GetAncestor(fg, GA_ROOT) == w)) {
            RECT r;
            WINDOWPLACEMENT original = { sizeof original };
            BOOL havePlacement = GetWindowPlacement(w, &original);
            BOOL fly = idx >= 0 && cfg.minAnim && WindowShownRect(w, &r) &&
                       (icon.right-icon.left)>4 && (icon.bottom-icon.top)>4 && FlyBegin(w, r, icon, 0, idx);
            if (cfg.minAnim) DisableTransitions(w, TRUE);
            g_selfMin = w; g_selfMinTick = GetTickCount(); /* suppress Explorer's competing minimize hook */
            BOOL fade = fly && g_animFade, weLayer = fly && g_fadeLayered;
            BOOL compositor = fly && !g_animDirect;
            if (compositor) CommitMinimizeKeepPlacement(w, havePlacement ? &original : NULL);
            if (fly) FlyRun((float)(cfg.animSpeed < 200 ? 200 : cfg.animSpeed > 1000 ? 1000 : cfg.animSpeed));
            if (fly) AnimFinish();
            if (!compositor) CommitMinimizeKeepPlacement(w, havePlacement ? &original : NULL);
            if (!compositor && fade) {   /* the window is hidden now: restore normal opacity and drop the temporary layered style */
                SetLayeredWindowAttributes(w, 0, 255, LWA_ALPHA);
                if (weLayer) {
                    LONG ex = GetWindowLongW(w, GWL_EXSTYLE);
                    if (ex & WS_EX_LAYERED) SetWindowLongW(w, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
                }
            }
        } else SetForegroundWindow(w);
    }
}

/* ClickToggle controls repeat-click minimization. An iconic window still gets
   the selected restore animation whenever MinAnim is enabled. */
static void ActivateDockWindow(HWND w, int idx) {
    BOOL minimized = IsIconic(w);
    if (cfg.clickToggle || (cfg.minAnim && minimized)) {
        ToggleWindowIdx(w, idx);
        return;
    }
    if (minimized) ShowWindowAsync(w, SW_RESTORE);
    SetForegroundWindow(w);
}

static void ToggleWindowItem(int i) { if (i >= 0 && i < nItems && items[i].hwnd && IsWindow(items[i].hwnd)) ToggleWindowIdx(items[i].hwnd, i); }

static BOOL ToggleApp(const Item *it) {
    TopCtx c = { it, 0, it->type == T_URL, NULL };
    if (c.isUrl) c.key = ItemKey(it);
    EnumWindows(TopProc, (LPARAM)&c);
    if (!c.first) return FALSE;
    int idx = (int)(it - items);
    ActivateDockWindow(c.first, (idx >= 0 && idx < nItems) ? idx : ItemOfWin(c.first));
    return TRUE;
}

static BOOL ActivateRunningItem(int i) {
    Item *it = &items[i];
    HWND window = GetForegroundWindow();
    WinId id;
    if (window && IsAppWindow(window)) { WinIdOf(window, &id); if (!ItemMatchExact(it, &id)) window = NULL; }
    else window = NULL;
    if (!window && it->hwnd && IsAppWindow(it->hwnd)) {
        WinIdOf(it->hwnd, &id);
        if (ItemMatchExact(it, &id)) window = it->hwnd;
    }
    if (!window) {
        TopCtx c = { it, 0, FALSE, NULL };
        EnumWindows(TopProc, (LPARAM)&c);
        window = c.first;
    }
    if (!window) return FALSE;
    it->hwnd = window;
    ActivateDockWindow(window, i);
    return TRUE;
}

/* the minimise animation must not run inside the WinEvent callback: blocking that callback for the
   whole 290 ms animation blocks USER32's event dispatch (it crashed there). The callback only records
   what to animate and posts a message; the animation runs in the normal message loop. */
static HWND g_minPend;
static int g_minIdx;
static RECT g_minRect;
static DWORD g_minTick;
static WINDOWPLACEMENT g_minPlacement;
static BOOL g_minPlacementValid;
static void CALLBACK MinProc(HWINEVENTHOOK hk, DWORD ev, HWND w, LONG idObj, LONG idChild, DWORD tid, DWORD ms) {
    if (ev != EVENT_SYSTEM_MINIMIZESTART || idObj != OBJID_WINDOW || !w || g_anim || g_animBusy || !cfg.minAnim || HideAmt() > 0.5f) return;
    if (w == g_selfMin && GetTickCount() - g_selfMinTick < 3000) { g_selfMin = NULL; return; }
    if (!IsAppWindow(w) || w == g_hwnd || w == g_hset) return;
    int idx = ItemOfWin(w);
    if (idx < 0) return;
    RECT r;
    if (!WindowShownRect(w, &r)) return;
    g_minPlacement.length = sizeof g_minPlacement;
    g_minPlacementValid = GetWindowPlacement(w, &g_minPlacement);
    /* Create the overlay now so it covers the window before the system minimise animation starts, but
       keep the blocking frame loop out of this WinEvent callback. */
    if (cfg.minAnim) DisableTransitions(w, TRUE);
    RECT icon;
    IconScreenRect(idx, &icon);
    if (FlyBegin(w, r, icon, 0, idx)) {
        g_minPend = w; g_minIdx = idx; g_minRect = r; g_minTick = GetTickCount();
        /* The direct fallback cancels native minimization until its flight ends.
           A compositor preview can keep the real window minimized throughout the flight. */
        if (g_animDirect) ShowWindow(w, SW_SHOWNOACTIVATE);
        PostMessageW(g_hwnd, WM_APP_MINANIM, 0, 0);
    } else { g_minPlacementValid = FALSE; DisableTransitions(w, FALSE); }
}

/* keep the dock above every freshly activated window: a launching app can no longer cover the dock for a moment */
static HWINEVENTHOOK g_fgHook;
static void CALLBACK FgProc(HWINEVENTHOOK hk, DWORD ev, HWND w, LONG idObj, LONG idChild, DWORD tid, DWORD ms) {
    if (!g_hwnd || w == g_hwnd || gFsHidden) return;
    SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    /* Mark the app's window while it is still on screen: a minimise later must not play the system
       "fly to the taskbar" animation next to the dock's own animation (that looked like two windows). */
    if (cfg.minAnim && w && w != g_hset && w != g_sb && w != g_lpWnd) DisableTransitions(w, TRUE);
}

static void SetMinHook(BOOL on) {
    if (!g_fgHook) g_fgHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, NULL, FgProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (on && !g_minHook) g_minHook = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZESTART, NULL, MinProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    else if (!on && g_minHook) { UnhookWinEvent(g_minHook); g_minHook = NULL; if (g_anim && !g_animBusy) AnimFinish(); }
    if (on) {
        /* the window that is already in front when the dock starts must not play the system
           "fly to the taskbar" minimise animation either */
        HWND fg = GetForegroundWindow();
        if (fg && fg != g_hwnd && fg != g_hset) DisableTransitions(fg, TRUE);
    }
    if (!on) RestoreTransitions();
}

/* ======================= popups: calendar (lunar + Ganzhi) and notification list ======================= */
static const unsigned g_lunarInfo[201] = {0x04bd8,0x04ae0,0x0a570,0x054d5,0x0d260,0x0d950,0x16554,0x056a0,0x09ad0,0x055d2,
0x04ae0,0x0a5b6,0x0a4d0,0x0d250,0x1d255,0x0b540,0x0d6a0,0x0ada2,0x095b0,0x14977,
0x04970,0x0a4b0,0x0b4b5,0x06a50,0x06d40,0x1ab54,0x02b60,0x09570,0x052f2,0x04970,
0x06566,0x0d4a0,0x0ea50,0x16a95,0x05ad0,0x02b60,0x186e3,0x092e0,0x1c8d7,0x0c950,
0x0d4a0,0x1d8a6,0x0b550,0x056a0,0x1a5b4,0x025d0,0x092d0,0x0d2b2,0x0a950,0x0b557,
0x06ca0,0x0b550,0x15355,0x04da0,0x0a5b0,0x14573,0x052b0,0x0a9a8,0x0e950,0x06aa0,
0x0aea6,0x0ab50,0x04b60,0x0aae4,0x0a570,0x05260,0x0f263,0x0d950,0x05b57,0x056a0,
0x096d0,0x04dd5,0x04ad0,0x0a4d0,0x0d4d4,0x0d250,0x0d558,0x0b540,0x0b6a0,0x195a6,
0x095b0,0x049b0,0x0a974,0x0a4b0,0x0b27a,0x06a50,0x06d40,0x0af46,0x0ab60,0x09570,
0x04af5,0x04970,0x064b0,0x074a3,0x0ea50,0x06b58,0x05ac0,0x0ab60,0x096d5,0x092e0,
0x0c960,0x0d954,0x0d4a0,0x0da50,0x07552,0x056a0,0x0abb7,0x025d0,0x092d0,0x0cab5,
0x0a950,0x0b4a0,0x0baa4,0x0ad50,0x055d9,0x04ba0,0x0a5b0,0x15176,0x052b0,0x0a930,
0x07954,0x06aa0,0x0ad50,0x05b52,0x04b60,0x0a6e6,0x0a4e0,0x0d260,0x0ea65,0x0d530,
0x05aa0,0x076a3,0x096d0,0x04afb,0x04ad0,0x0a4d0,0x1d0b6,0x0d250,0x0d520,0x0dd45,
0x0b5a0,0x056d0,0x055b2,0x049b0,0x0a577,0x0a4b0,0x0aa50,0x1b255,0x06d20,0x0ada0,
0x14b63,0x09370,0x049f8,0x04970,0x064b0,0x168a6,0x0ea50,0x06aa0,0x1a6c4,0x0aae0,
0x092e0,0x0d2e3,0x0c960,0x0d557,0x0d4a0,0x0da50,0x05d55,0x056a0,0x0a6d0,0x055d4,
0x052d0,0x0a9b8,0x0a950,0x0b4a0,0x0b6a6,0x0ad50,0x055a0,0x0aba4,0x0a5b0,0x052b0,
0x0b273,0x06930,0x07337,0x06aa0,0x0ad50,0x14b55,0x04b60,0x0a570,0x054e4,0x0d160,
0x0e968,0x0d520,0x0daa0,0x16aa6,0x056d0,0x04ae0,0x0a9d4,0x0a2d0,0x0d150,0x0f252,
0x0d520};
static const WCHAR *g_gan = L"甲乙丙丁戊己庚辛壬癸";
static const WCHAR *g_zhi = L"子丑寅卯辰巳午未申酉戌亥";
static const WCHAR *g_zodiac = L"鼠牛虎兔龙蛇马羊猴鸡狗猪";
static const WCHAR *g_termName[24] = { L"小寒", L"大寒", L"立春", L"雨水", L"惊蛰", L"春分", L"清明", L"谷雨", L"立夏", L"小满", L"芒种", L"夏至",
                                       L"小暑", L"大暑", L"立秋", L"处暑", L"白露", L"秋分", L"寒露", L"霜降", L"立冬", L"小雪", L"大雪", L"冬至" };
static const long g_termInfo[24] = { 0, 21208, 42467, 63836, 85337, 107014, 128867, 150921, 173149, 195551, 218072, 240693,
                                     263343, 285989, 308563, 331033, 353350, 375494, 397447, 419210, 440795, 462224, 483532, 504758 };
static const WCHAR *g_lday[30] = { L"初一", L"初二", L"初三", L"初四", L"初五", L"初六", L"初七", L"初八", L"初九", L"初十", L"十一", L"十二", L"十三", L"十四", L"十五",
                                   L"十六", L"十七", L"十八", L"十九", L"二十", L"廿一", L"廿二", L"廿三", L"廿四", L"廿五", L"廿六", L"廿七", L"廿八", L"廿九", L"三十" };
static const WCHAR *g_lmon[12] = { L"正", L"二", L"三", L"四", L"五", L"六", L"七", L"八", L"九", L"十", L"冬", L"腊" };
static const WCHAR *g_wkName[7] = { L"日", L"一", L"二", L"三", L"四", L"五", L"六" };

static int DaysFromCivil(int y, int m, int d) {   /* days since 1970-01-01 */
    y -= m <= 2;
    int era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = (unsigned)(y - era * 400), doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1, doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (int)doe - 719468;
}
static void CivilFromDays(int z, int *y, int *m, int *d) {
    z += 719468;
    int era = (z >= 0 ? z : z - 146096) / 146097;
    unsigned doe = (unsigned)(z - era * 146097), yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int yy = (int)yoe + era * 400;
    unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100), mp = (5 * doy + 2) / 153;
    *d = (int)(doy - (153 * mp + 2) / 5 + 1);
    *m = (int)(mp < 10 ? mp + 3 : mp - 9);
    *y = yy + (*m <= 2);
}
static int WeekdayOf(int y, int m, int d) { return ((DaysFromCivil(y, m, d) % 7) + 11) % 7; }   /* 0 = Sunday */

static int LeapMonth(int y) { return g_lunarInfo[y - 1900] & 0xf; }
static int LeapDays(int y) { return LeapMonth(y) ? ((g_lunarInfo[y - 1900] & 0x10000) ? 30 : 29) : 0; }
static int LMonthDays(int y, int m) { return (g_lunarInfo[y - 1900] & (0x10000 >> m)) ? 30 : 29; }
static int LYearDays(int y) {
    int s = 348;
    for (unsigned i = 0x8000; i > 0x8; i >>= 1) s += (g_lunarInfo[y - 1900] & i) ? 1 : 0;
    return s + LeapDays(y);
}
typedef struct { int y, m, d, leap; } Lunar;
static BOOL SolarToLunar(int y, int m, int d, Lunar *o) {
    if (y < 1901 || y > 2099) return FALSE;
    int off = DaysFromCivil(y, m, d) - DaysFromCivil(1900, 1, 31), i, temp = 0;
    for (i = 1900; i < 2101 && off > 0; i++) { temp = LYearDays(i); off -= temp; }
    if (off < 0) { off += temp; i--; }
    int year = i, leap = LeapMonth(i), isLeap = 0;
    for (i = 1; i < 13 && off > 0; i++) {
        if (leap > 0 && i == leap + 1 && !isLeap) { --i; isLeap = 1; temp = LeapDays(year); } else temp = LMonthDays(year, i);
        if (isLeap && i == leap + 1) isLeap = 0;
        off -= temp;
    }
    if (off == 0 && leap > 0 && i == leap + 1) { if (isLeap) isLeap = 0; else { isLeap = 1; --i; } }
    if (off < 0) { off += temp; --i; }
    o->y = year; o->m = i; o->d = off + 1; o->leap = isLeap;
    return TRUE;
}
static void TermDate(int y, int n, int *m, int *d) {   /* the date of solar term n (0 = Xiaohan) in year y */
    double ms = 31556925974.7 * (y - 1900) + (double)g_termInfo[n] * 60000.0;
    double days = (double)DaysFromCivil(1900, 1, 6) + (2.0 * 60 + 5) / 1440.0 + ms / 86400000.0;
    int yy;
    CivilFromDays((int)floor(days), &yy, m, d);
}

/* sub-label of a calendar cell: festival > solar term > lunar month name on day 1 > lunar day */
static void CalSub(int y, int m, int d, WCHAR *out, BOOL *hl) {
    Lunar L;
    out[0] = 0; *hl = FALSE;
    static const struct { int m, d; const WCHAR *n; } sf[] = { { 1, 1, L"元旦" }, { 2, 14, L"情人节" }, { 5, 1, L"劳动节" }, { 6, 1, L"儿童节" }, { 10, 1, L"国庆节" }, { 12, 25, L"圣诞节" } };
    for (int i = 0; i < 6; i++) if (sf[i].m == m && sf[i].d == d) { lstrcpyW(out, sf[i].n); *hl = TRUE; }
    BOOL ok = SolarToLunar(y, m, d, &L);
    if (ok && !out[0]) {
        if (L.m == 1 && L.d == 1 && !L.leap) { lstrcpyW(out, L"春节"); *hl = TRUE; }
        else if (L.m == 1 && L.d == 15 && !L.leap) { lstrcpyW(out, L"元宵"); *hl = TRUE; }
        else if (L.m == 5 && L.d == 5 && !L.leap) { lstrcpyW(out, L"端午"); *hl = TRUE; }
        else if (L.m == 7 && L.d == 7 && !L.leap) { lstrcpyW(out, L"七夕"); *hl = TRUE; }
        else if (L.m == 8 && L.d == 15 && !L.leap) { lstrcpyW(out, L"中秋"); *hl = TRUE; }
        else if (L.m == 9 && L.d == 9 && !L.leap) { lstrcpyW(out, L"重阳"); *hl = TRUE; }
        else if (L.m == 12 && !L.leap && L.d == LMonthDays(L.y, 12)) { lstrcpyW(out, L"除夕"); *hl = TRUE; }
    }
    if (!out[0]) {
        int tm, td;
        TermDate(y, (m - 1) * 2, &tm, &td);
        if (tm == m && td == d) { lstrcpyW(out, g_termName[(m - 1) * 2]); *hl = TRUE; }
        else { TermDate(y, (m - 1) * 2 + 1, &tm, &td); if (tm == m && td == d) { lstrcpyW(out, g_termName[(m - 1) * 2 + 1]); *hl = TRUE; } }
    }
    if (!out[0] && ok) {
        if (L.d == 1) wsprintfW(out, L"%s%s月", L.leap ? L"闰" : L"", g_lmon[L.m - 1]);
        else lstrcpyW(out, g_lday[L.d - 1]);
    }
}

/* Ganzhi (sexagenary) calendar: year changes at Lichun, month at each "jie" solar term */
static void GanzhiText(int y, int m, int d, WCHAR *out) {
    int lm, ld;
    TermDate(y, 2, &lm, &ld);
    int gy = (m > lm || (m == lm && d >= ld)) ? y : y - 1;
    int ys = ((gy - 4) % 10 + 10) % 10, yz = ((gy - 4) % 12 + 12) % 12;
    int tm, td;
    TermDate(y, (m - 1) * 2, &tm, &td);
    int effMo = d >= td ? m : m - 1;
    int branch = ((effMo % 12) + 12) % 12, offs = (branch - 2 + 12) % 12, ms = (((ys % 5) * 2 + 2) % 10 + offs) % 10;
    int di = ((DaysFromCivil(y, m, d) + 17) % 60 + 60) % 60;
    wsprintfW(out, L"%c%c年  %c%c月  %c%c日", g_gan[ys], g_zhi[yz], g_gan[ms], g_zhi[branch], g_gan[di % 10], g_zhi[di % 12]);
}

/* ---- generic popup next to a dock widget ---- */
#define CLS_POP L"MiniDockPopup"
#define PM 8
typedef struct PopCtx {
    Canvas cv;
    int W, H, done, alpha, ready, dy, px, py;
    RECT scr;
    HWND bk;
    BOOL live;                     /* redraw on the 50 ms timer (resource monitor) */
    FrostSnap frost;
    DWORD t0;
    void *u;
    void (*draw)(struct PopCtx *, GpGraphics *);
    void (*mv)(struct PopCtx *, int, int);
    void (*up)(struct PopCtx *, int, int);
    void (*rup)(struct PopCtx *, int, int);
    void (*wheel)(struct PopCtx *, int);
} PopCtx;

static float PopBlurOp(void) { float v = cfg.sbPopBlur < 0 ? 0.f : cfg.sbPopBlur > 100 ? 1.f : cfg.sbPopBlur / 100.f; return 0.06f + 0.94f * v; }
static void PopPaint(PopCtx *c, HWND h) {
    GpGraphics *g = c->cv.g;
    GdipGraphicsClear(g, 0);
    BOOL frost = cfg.sbPopFrost && c->frost.gp != NULL;
    if (frost) FrostSnapPaint(g, &c->frost, PM, PM, (float)(c->W - 2 * PM), (float)(c->H - 2 * PM), 22.f, PopBlurOp());
    int gp = cfg.grpOpacity < 20 ? 20 : cfg.grpOpacity > 100 ? 100 : cfg.grpOpacity;
    int al = (frost ? 150 : 255) * gp / 100;
    if (frost) FillRR(g, g_dark ? ARGBC(al * 125 / 150, 20, 21, 26) : ARGBC(al, 250, 250, 254), PM, PM, (float)(c->W - 2 * PM), (float)(c->H - 2 * PM), 22);
    else FillRR(g, g_dark ? ARGBC(al * 247 / 255, 38, 39, 46) : ARGBC(al * 247 / 255, 247, 248, 252), PM, PM, (float)(c->W - 2 * PM), (float)(c->H - 2 * PM), 22);
    StrokeRR(g, g_dark ? ARGBC(60, 255, 255, 255) : ARGBC(40, 0, 0, 0), 1, PM, PM, (float)(c->W - 2 * PM), (float)(c->H - 2 * PM), 22);
    if (c->draw) c->draw(c, g);
    CanvasPresentA(&c->cv, h, c->px, c->py + c->dy, c->alpha);
}

static LRESULT CALLBACK PopProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    PopCtx *c = (PopCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    if (!c) return DefWindowProcW(h, m, w, l);
    int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
    switch (m) {
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_LBUTTONDOWN: SetCapture(h); return 0;
    case WM_MOUSEMOVE: if (c->mv) c->mv(c, x, y); PopPaint(c, h); return 0;
    case WM_LBUTTONUP: if (GetCapture() == h) ReleaseCapture(); if (c->up) c->up(c, x, y); if (!c->done) PopPaint(c, h); return 0;
    case WM_RBUTTONUP: if (c->rup) c->rup(c, x, y); if (!c->done) PopPaint(c, h); return 0;
    case WM_MOUSEWHEEL: if (c->wheel) c->wheel(c, (short)HIWORD(w) > 0 ? -1 : 1); PopPaint(c, h); return 0;
    case WM_TIMER:
        if (w == 2) {
            float t = (GetTickCount() - c->t0) / 190.f;
            if (t >= 1.f) {
                KillTimer(h, 2);
                c->alpha = 255; c->dy = 0; c->ready = TRUE;
                if (c->bk) { int r0 = 22; PlaceBackdrop(c->bk, h, &c->scr, &r0, 1); }
            } else { float e = 1.f - (1.f - t) * (1.f - t) * (1.f - t); c->alpha = (int)(255 * e); c->dy = (int)((1.f - e) * 18 * (cfg.edge == E_TOP ? -1 : 1)); }
            PopPaint(c, h);
        } else if (c->live) PopPaint(c, h);   /* live widgets (resource monitor) */
        else if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) c->done = 1;
        return 0;
    case WM_SETCURSOR: SetCursor(LoadCursor(NULL, IDC_ARROW)); return TRUE;
    }
    return DefWindowProcW(h, m, w, l);
}

/* Place a popup next to its anchor without covering it: below a top bar, above a bottom dock, beside a
   vertical dock. The old code always used the *dock* edge, so a popup opened from the status bar was put
   above the top of the screen and then clamped to the corner - which is what looked misaligned. */
static void PopPlace(RECT a, int W, int H, int *px, int *py) {
    RECT mr = g_mi.rcMonitor;
    int ax = (a.left + a.right) / 2, ay = (a.top + a.bottom) / 2;
    int gap = Sc(8);
    BOOL topBar = a.bottom <= mr.top + Sc(64);
    BOOL verticalDock = (cfg.edge == E_LEFT || cfg.edge == E_RIGHT) && !topBar;
    if (verticalDock) {
        *px = (cfg.edge == E_LEFT) ? a.right + gap : a.left - W - gap;
        *py = ay - H / 2;
    } else {
        int below = mr.bottom - a.bottom, above = a.top - mr.top;
        *px = ax - W / 2;
        if (topBar || (below >= H + gap && below > above)) *py = a.bottom + gap;
        else *py = a.top - H - gap;
    }
    if (*px + W > mr.right) *px = mr.right - W;
    if (*py + H > mr.bottom) *py = mr.bottom - H;
    if (*px < mr.left) *px = mr.left;
    if (*py < mr.top) *py = mr.top;
}

static void PopRun(const RECT *anchor, int W, int H, PopCtx *c) {
    static HWND busy;
    static BOOL reg;
    if (busy) return;
    if (!reg) {
        WNDCLASSW wc;
        ZeroMemory(&wc, sizeof wc);
        wc.lpfnWndProc = PopProc; wc.hInstance = g_inst; wc.lpszClassName = CLS_POP; wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);
        reg = TRUE;
    }
    c->W = W; c->H = H;
    PopPlace(*anchor, W, H, &c->px, &c->py);
    if (!CanvasNew(&c->cv, W, H)) return;
    HWND h = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CLS_POP, L"", WS_POPUP, c->px, c->py, W, H, NULL, NULL, g_inst, NULL);
    busy = h;
    SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)c);
    c->scr.left = c->px + PM; c->scr.top = c->py + PM; c->scr.right = c->px + W - PM; c->scr.bottom = c->py + H - PM;
    if (cfg.sbPopFrost) FrostSnapGrab(&c->frost, &c->scr);
    c->alpha = 0; c->dy = 18 * (cfg.edge == E_TOP ? -1 : 1); c->t0 = GetTickCount();
    PopPaint(c, h);
    ShowWindow(h, SW_SHOWNA);
    gMenuOpen = TRUE;
    ClosePreview();
    SetTimer(h, 1, 50, NULL);
    SetTimer(h, 2, 15, NULL);
    GuardStart(RectInside, &c->scr, &c->done, h);
    MSG msg;
    while (!c->done && GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    GuardStop();
    gMenuOpen = FALSE;
    SetWindowLongPtrW(h, GWLP_USERDATA, 0);
    if (c->bk) DestroyWindow(c->bk);
    FrostSnapFree(&c->frost);
    DestroyWindow(h);
    busy = NULL;
    CanvasFree(&c->cv);
    g_popupClosedTick = GetTickCount();
    gLastNear = GetTickCount();
    Kick();
}

/* ---- calendar ---- */
typedef struct { int y, m, selY, selM, selD, hover; RECT nav[5], cell[42]; int cy[42], cm[42], cd[42]; } CalU;

static void CalDraw(PopCtx *c, GpGraphics *g) {
    CalU *u = (CalU *)c->u;
    SYSTEMTIME now;
    GetLocalTime(&now);
    ARGB tc = g_dark ? ARGBC(255, 240, 240, 245) : ARGBC(255, 28, 28, 34), sub = g_dark ? ARGBC(255, 150, 154, 168) : ARGBC(255, 112, 116, 130);
    ARGB acc = ARGBC(255, 10, 132, 255), red = ARGBC(255, 230, 74, 70);
    float x0 = (float)(PM + Sc(16)), y0 = (float)(PM + Sc(14)), cw = (float)(c->W - 2 * PM - Sc(32)), bw = (float)Sc(30), hh = (float)Sc(34);
    WCHAR t[64];
    /* navigation: [<] year [>]   [<] month [>]   [today] */
    float nx[5] = { x0, x0 + bw + Sc(62), x0 + Sc(132), x0 + Sc(132) + bw + Sc(48), x0 + cw - Sc(58) };
    float nw[5] = { bw, bw, bw, bw, (float)Sc(58) };
    for (int i = 0; i < 5; i++) {
        u->nav[i].left = (int)nx[i]; u->nav[i].top = (int)y0; u->nav[i].right = (int)(nx[i] + nw[i]); u->nav[i].bottom = (int)(y0 + hh);
        if (u->hover == 100 + i || i == 4) FillRR(g, i == 4 ? (u->hover == 104 ? acc : ARGBC(46, 10, 132, 255)) : (g_dark ? ARGBC(40, 255, 255, 255) : ARGBC(22, 0, 0, 0)), nx[i], y0, nw[i], hh, hh / 2);
    }
    DrawCentered(g, L"‹", g_fontBold, nx[0], y0, bw, hh, tc);
    wsprintfW(t, L"%d年", u->y);
    DrawCentered(g, t, g_fontBold, nx[0] + bw, y0, (float)Sc(62), hh, tc);
    DrawCentered(g, L"›", g_fontBold, nx[1], y0, bw, hh, tc);
    DrawCentered(g, L"‹", g_fontBold, nx[2], y0, bw, hh, tc);
    wsprintfW(t, L"%d月", u->m);
    DrawCentered(g, t, g_fontBold, nx[2] + bw, y0, (float)Sc(48), hh, tc);
    DrawCentered(g, L"›", g_fontBold, nx[3], y0, bw, hh, tc);
    DrawCentered(g, L"今天", g_font, nx[4], y0, nw[4], hh, u->hover == 104 ? ARGBC(255, 255, 255, 255) : acc);
    /* weekdays (Monday first) */
    float wy = y0 + hh + Sc(8), cwid = cw / 7, ch = (float)Sc(47), gy = wy + Sc(26);
    for (int i = 0; i < 7; i++) DrawCentered(g, g_wkName[(i + 1) % 7], g_fDR, x0 + i * cwid, wy, cwid, (float)Sc(24), i >= 5 ? red : sub);
    /* 6 x 7 grid */
    int off = (WeekdayOf(u->y, u->m, 1) + 6) % 7, base = DaysFromCivil(u->y, u->m, 1) - off;
    for (int k = 0; k < 42; k++) {
        int yy, mm, dd;
        CivilFromDays(base + k, &yy, &mm, &dd);
        u->cy[k] = yy; u->cm[k] = mm; u->cd[k] = dd;
        float x = x0 + (k % 7) * cwid, y = gy + (k / 7) * ch;
        u->cell[k].left = (int)x; u->cell[k].top = (int)y; u->cell[k].right = (int)(x + cwid); u->cell[k].bottom = (int)(y + ch);
        BOOL inMonth = mm == u->m, isToday = yy == now.wYear && mm == now.wMonth && dd == now.wDay, isSel = yy == u->selY && mm == u->selM && dd == u->selD;
        float r = (cwid < ch ? cwid : ch) * 0.5f - 2;
        float cx = x + cwid / 2, cy = y + ch / 2;
        if (isToday) FillRR(g, acc, cx - r, cy - r, r * 2, r * 2, r);
        else if (u->hover == k) FillRR(g, g_dark ? ARGBC(40, 255, 255, 255) : ARGBC(22, 0, 0, 0), cx - r, cy - r, r * 2, r * 2, r);
        if (isSel && !isToday) StrokeRR(g, acc, 1.8f, cx - r, cy - r, r * 2, r * 2, r);
        else if (isSel) StrokeRR(g, ARGBC(255, 255, 255, 255), 1.6f, cx - r + 2, cy - r + 2, r * 2 - 4, r * 2 - 4, r - 2);
        WCHAR dn[8], sl[24];
        BOOL hl;
        wsprintfW(dn, L"%d", dd);
        CalSub(yy, mm, dd, sl, &hl);
        int wd = k % 7;
        ARGB nc = isToday ? ARGBC(255, 255, 255, 255) : (!inMonth ? ARGBC(110, g_dark ? 200 : 90, g_dark ? 200 : 90, g_dark ? 210 : 100) : (wd >= 5 ? red : tc));
        ARGB sc2 = isToday ? ARGBC(230, 255, 255, 255) : (!inMonth ? ARGBC(90, g_dark ? 200 : 90, g_dark ? 200 : 90, g_dark ? 210 : 100) : (hl ? red : sub));
        DrawCentered(g, dn, g_fTR, x, y + 3, cwid, ch * 0.55f, nc);
        DrawCentered(g, sl, g_fS, x, y + ch * 0.56f, cwid, ch * 0.4f, sc2);
    }
    /* info panel for the selected day */
    float iy = gy + 6 * ch + Sc(6);
    GpPen *lp;
    GdipCreatePen1(g_dark ? ARGBC(60, 255, 255, 255) : ARGBC(60, 0, 0, 0), 1.f, 2, &lp);
    GdipDrawLine(g, lp, x0, iy, x0 + cw, iy);
    GdipDeletePen(lp);
    iy += Sc(6);
    float lh = (float)Sc(22);
    Lunar L;
    WCHAR l1[80], l2[120], l3[80], l4[100];
    wsprintfW(l1, L"%d年%d月%d日  星期%s%s", u->selY, u->selM, u->selD, g_wkName[WeekdayOf(u->selY, u->selM, u->selD)],
              (u->selY == now.wYear && u->selM == now.wMonth && u->selD == now.wDay) ? L"  · 今天" : L"");
    if (SolarToLunar(u->selY, u->selM, u->selD, &L)) {
        int zi = ((L.y - 4) % 12 + 12) % 12, gi = ((L.y - 4) % 10 + 10) % 10;
        wsprintfW(l2, L"农历  %c%c年（%c） %s%s月%s", g_gan[gi], g_zhi[zi], g_zodiac[zi], L.leap ? L"闰" : L"", g_lmon[L.m - 1], g_lday[L.d - 1]);
    } else lstrcpyW(l2, L"农历  （仅支持 1901–2099 年）");
    WCHAR gz[60];
    GanzhiText(u->selY, u->selM, u->selD, gz);
    wsprintfW(l3, L"干支历  %s", gz);
    {   /* next solar term */
        int sd = DaysFromCivil(u->selY, u->selM, u->selD), found = 0;
        l4[0] = 0;
        for (int yy = u->selY; yy <= u->selY + 1 && !found; yy++)
            for (int n = 0; n < 24 && !found; n++) {
                int tm, td;
                TermDate(yy, n, &tm, &td);
                int tdays = DaysFromCivil(yy, tm, td);
                if (tdays > sd) { wsprintfW(l4, L"下一个节气  %s  %d月%d日（还有 %d 天）", g_termName[n], tm, td, tdays - sd); found = 1; }
            }
    }
    DrawTextBox(g, l1, g_fontBold, x0, iy, cw, lh, tc, TRUE);
    DrawTextBox(g, l2, g_font, x0, iy + lh, cw, lh, tc, TRUE);
    DrawTextBox(g, l3, g_font, x0, iy + 2 * lh, cw, lh, sub, TRUE);
    DrawTextBox(g, l4, g_font, x0, iy + 3 * lh, cw, lh, sub, TRUE);
}

static void CalMove(PopCtx *c, int x, int y) {
    CalU *u = (CalU *)c->u;
    POINT p = { x, y };
    u->hover = -1;
    for (int i = 0; i < 5; i++) if (PtInRect(&u->nav[i], p)) u->hover = 100 + i;
    for (int k = 0; k < 42; k++) if (PtInRect(&u->cell[k], p)) u->hover = k;
}
static void CalStep(CalU *u, int dy, int dm) {
    u->m += dm; u->y += dy;
    while (u->m > 12) { u->m -= 12; u->y++; }
    while (u->m < 1) { u->m += 12; u->y--; }
    if (u->y < 1901) u->y = 1901;
    if (u->y > 2098) u->y = 2098;
}
static void CalUp(PopCtx *c, int x, int y) {
    CalU *u = (CalU *)c->u;
    CalMove(c, x, y);
    int hv = u->hover;
    if (hv == 100) CalStep(u, -1, 0);
    else if (hv == 101) CalStep(u, 1, 0);
    else if (hv == 102) CalStep(u, 0, -1);
    else if (hv == 103) CalStep(u, 0, 1);
    else if (hv == 104) { SYSTEMTIME n; GetLocalTime(&n); u->y = u->selY = n.wYear; u->m = u->selM = n.wMonth; u->selD = n.wDay; }
    else if (hv >= 0 && hv < 42) {
        u->selY = u->cy[hv]; u->selM = u->cm[hv]; u->selD = u->cd[hv];
        if (u->cm[hv] != u->m) { u->y = u->cy[hv]; u->m = u->cm[hv]; }
    }
}
static void CalWheel(PopCtx *c, int dir) { CalStep((CalU *)c->u, 0, dir); }

static void ShowCalendar(void) {
    if (GetTickCount() - g_popupClosedTick < 350) return;
    CalU *u = (CalU *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CalU));
    PopCtx *c = (PopCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PopCtx));
    if (!u || !c) return;
    SYSTEMTIME n;
    GetLocalTime(&n);
    u->y = u->selY = n.wYear; u->m = u->selM = n.wMonth; u->selD = n.wDay; u->hover = -1;
    c->u = u; c->draw = CalDraw; c->mv = CalMove; c->up = CalUp; c->wheel = CalWheel;
    PopRun(&g_wScr[W_CLOCK], Sc(392), Sc(556), c);
    HeapFree(GetProcessHeap(), 0, c);
    HeapFree(GetProcessHeap(), 0, u);
}

/* ---- notification list (taskbar-flash / unread requests of apps such as WeChat, QQ, Telegram, mail) ---- */
typedef struct { int n, hover; NotifyEnt e[16]; GpBitmap *ic[16]; RECT row[16], clr; } NfU;

static void NfDraw(PopCtx *c, GpGraphics *g) {
    NfU *u = (NfU *)c->u;
    ARGB tc = g_dark ? ARGBC(255, 240, 240, 245) : ARGBC(255, 28, 28, 34), sub = g_dark ? ARGBC(255, 150, 154, 168) : ARGBC(255, 112, 116, 130), acc = ARGBC(255, 10, 132, 255);
    float x0 = (float)(PM + Sc(16)), y0 = (float)(PM + Sc(12)), cw = (float)(c->W - 2 * PM - Sc(32));
    DrawTextBox(g, L"通知", g_fontTitle, x0, y0, cw * 0.5f, (float)Sc(36), tc, TRUE);
    u->clr.left = (int)(x0 + cw - Sc(84)); u->clr.top = (int)y0; u->clr.right = (int)(x0 + cw); u->clr.bottom = (int)(y0 + Sc(34));
    if (u->n > 0) {
        if (u->hover == 100) FillRR(g, ARGBC(40, 10, 132, 255), (float)u->clr.left, (float)u->clr.top, (float)Sc(84), (float)Sc(34), Sc(17.f));
        DrawCentered(g, L"全部清除", g_font, (float)u->clr.left, (float)u->clr.top, (float)Sc(84), (float)Sc(34), acc);
    }
    float y = y0 + Sc(46), rh = (float)Sc(54);
    if (u->n == 0) {
        DrawCentered(g, L"没有新的通知", g_fontBold, x0, y, cw, (float)Sc(40), tc);
        DrawTextBox(g, L"这里收集应用的任务栏闪烁 / 未读提醒，例如微信、QQ、Telegram、邮件。系统自带的通知弹窗内容无法被第三方程序读取。", g_font, x0, y + Sc(44), cw, (float)Sc(96), sub, FALSE);
        return;
    }
    for (int i = 0; i < u->n; i++) {
        float ry = y + i * (rh + 4);
        u->row[i].left = (int)x0; u->row[i].top = (int)ry; u->row[i].right = (int)(x0 + cw); u->row[i].bottom = (int)(ry + rh);
        if (u->hover == i) FillRR(g, g_dark ? ARGBC(40, 255, 255, 255) : ARGBC(22, 0, 0, 0), x0, ry, cw, rh, 12);
        if (!u->ic[i]) u->ic[i] = ShellIconBitmap(u->e[i].exe, NULL, 64);
        if (u->ic[i]) DrawImg(g, u->ic[i], x0 + 8, ry + (rh - Sc(36)) / 2, (float)Sc(36), (float)Sc(36), FALSE);
        WCHAR nm[128];
        lstrcpynW(nm, PathFindFileNameW(u->e[i].exe), 128);
        PathRemoveExtensionW(nm);
        DrawTextBox(g, u->e[i].title[0] ? u->e[i].title : nm, g_fontBold, x0 + Sc(54), ry + 6, cw - Sc(64), rh * 0.5f, tc, TRUE);
        DrawTextBox(g, nm, g_font, x0 + Sc(54), ry + rh * 0.5f, cw - Sc(64), rh * 0.42f, sub, TRUE);
    }
}
static void NfMove(PopCtx *c, int x, int y) {
    NfU *u = (NfU *)c->u;
    POINT p = { x, y };
    u->hover = -1;
    if (u->n > 0 && PtInRect(&u->clr, p)) u->hover = 100;
    for (int i = 0; i < u->n; i++) if (PtInRect(&u->row[i], p)) u->hover = i;
}
static void ActivateTopWindow(HWND w) {
    if (!w || !IsWindow(w)) return;
    if (IsIconic(w)) ShowWindow(w, SW_RESTORE);
    AllowSetForegroundWindow(ASFW_ANY);
    DWORD fg = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
    DWORD me = GetCurrentThreadId();
    if (fg && fg != me) AttachThreadInput(fg, me, TRUE);
    SetForegroundWindow(w);
    BringWindowToTop(w);
    if (fg && fg != me) AttachThreadInput(fg, me, FALSE);
}
static HWND FindAppWindowByExe(const WCHAR *exe) {
    if (!exe || !exe[0]) return NULL;
    /* a plain loop: it only runs when a notification row is clicked */
    HWND all[256];
    int n = 0;
    for (HWND h = GetTopWindow(NULL); h && n < 256; h = GetWindow(h, GW_HWNDNEXT)) all[n++] = h;
    for (int i = 0; i < n; i++) {
        if (!IsAppWindow(all[i])) continue;
        WCHAR e[MAX_PATH];
        if (ExeOfWindow(all[i], e) && !lstrcmpiW(e, exe)) return all[i];
    }
    return NULL;
}
static void NfUp(PopCtx *c, int x, int y) {
    NfU *u = (NfU *)c->u;
    NfMove(c, x, y);
    if (u->hover == 100) { g_nnf = 0; NotifyMarkItems(); Kick(); c->done = 1; }
    else if (u->hover >= 0 && u->hover < u->n) {
        HWND t = u->e[u->hover].h;
        WCHAR exe[MAX_PATH];
        lstrcpynW(exe, u->e[u->hover].exe, MAX_PATH);
        if (!t || !IsWindow(t) || !IsWindowVisible(t)) t = FindAppWindowByExe(exe);   /* the handle went stale: find the app again */
        if (t) ActivateTopWindow(t);
        else if (exe[0] && lstrcmpiW(PathFindFileNameW(exe), L"ApplicationFrameHost.exe")) {   /* really start that app */
            AllowSetForegroundWindow(ASFW_ANY);
            ShellExecuteW(NULL, L"open", exe, NULL, NULL, SW_SHOWNORMAL);
        }
        NotifyRemove(u->e[u->hover].h);
        NotifyRemoveExe(u->e[u->hover].exe);
        c->done = 1;
    }
}

/* ---- control centre: the Windows switches people actually use, in the dock's own design ---- */
typedef struct {
    int hover, mute, energy, vol, bri, drag;
    int n, id[9];
    int wired, dragIdx, pressIdx, moved;
    RECT tile[9], vs, bs;
} CcU;
static void CcOrderLoad(int out[8]);
static void CcOrderSave(const int in[8]);

static int g_ccVol = 50;
static int g_ccBri = -1;
/* Query the active Windows power scheme instead of assuming the saver is initially off. */
static BOOL CcEnergyIsSaver(void) {
    typedef DWORD (WINAPI *PFN_PowerGetActiveScheme)(HKEY, GUID **);
    HMODULE hp = LoadLibraryW(L"powrprof.dll");
    if (!hp) return FALSE;
    PFN_PowerGetActiveScheme fn = (PFN_PowerGetActiveScheme)GetProcAddress(hp, "PowerGetActiveScheme");
    GUID *active = NULL;
    BOOL saver = FALSE;
    if (fn && fn(NULL, &active) == ERROR_SUCCESS && active) {
        GUID gs = { 0xa1841308, 0x3541, 0x4fab, { 0xbc, 0x81, 0xf7, 0x15, 0x56, 0xf2, 0x0b, 0x4a } };
        saver = IsEqualGUID(active, &gs);
        LocalFree(active);
    }
    FreeLibrary(hp);
    return saver;
}

/* Switch the active Windows power scheme through the documented power API; verify by reading it back. */
static BOOL CcEnergySet(BOOL on) {
    typedef DWORD (WINAPI *PFN_PowerSetActiveScheme)(HKEY, GUID *);
    GUID sav = { 0xa1841308, 0x3541, 0x4fab, { 0xbc, 0x81, 0xf7, 0x15, 0x56, 0xf2, 0x0b, 0x4a } };
    GUID bal = { 0x381b4222, 0xf694, 0x41f0, { 0x96, 0x85, 0xff, 0x5b, 0xb2, 0x60, 0xdf, 0x2e } };
    BOOL ok = FALSE;
    HMODULE hp = LoadLibraryW(L"powrprof.dll");
    if (hp) {
        PFN_PowerSetActiveScheme fn = (PFN_PowerSetActiveScheme)GetProcAddress(hp, "PowerSetActiveScheme");
        if (fn) ok = fn(NULL, on ? &sav : &bal) == ERROR_SUCCESS;
        FreeLibrary(hp);
    }
    if (!ok) {
        WCHAR args[96];
        wsprintfW(args, L"/setactive %s", on ? L"SCHEME_MIN" : L"SCHEME_BALANCED");
        STARTUPINFOW si; PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof si); si.cb = sizeof si;
        WCHAR cmd[MAX_PATH + 32];
        UINT n = GetSystemDirectoryW(cmd, MAX_PATH);
        if (n > 0 && n < MAX_PATH - 16) {
            lstrcatW(cmd, L"\\powercfg.exe");
            if (CreateProcessW(cmd, args, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
                WaitForSingleObject(pi.hProcess, 3000);
                DWORD ec = 1;
                GetExitCodeProcess(pi.hProcess, &ec);
                CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
                ok = ec == 0;
            }
        }
    }
    for (int i = 0; i < 12; i++) {
        if (CcEnergyIsSaver() == on) return TRUE;
        Sleep(50);
    }
    return FALSE;
}
static ARGB CcAccent(void) {
    int r, g_, b;
    HsvRgb(g_accH, 0.72f, 0.95f, &r, &g_, &b);
    return ARGBC(255, r, g_, b);
}

/* ---- brightness through the monitor DDC API (no admin, no WMI) ---- */
typedef struct { HANDLE h; WCHAR desc[128]; } CcMon;
static HMODULE g_dxva;
static BOOL (WINAPI *pMonCount)(HMONITOR, LPDWORD);
static BOOL (WINAPI *pMonGet)(HMONITOR, DWORD, CcMon *);
static BOOL (WINAPI *pMonFree)(DWORD, CcMon *);
static BOOL (WINAPI *pBriGet)(HANDLE, LPDWORD, LPDWORD, LPDWORD);
static BOOL (WINAPI *pBriSet)(HANDLE, DWORD);

/* ---- real system volume through CoreAudio ---- */
static const GUID kCLSID_MMDeviceEnum = { 0xBCDE0395, 0xE52F, 0x467C, { 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E } };
static const GUID kIID_MMDeviceEnum = { 0xA95664D2, 0x9614, 0x4F35, { 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6 } };
static const GUID kIID_EndpointVol = { 0x5CDF2C82, 0x841E, 0x4546, { 0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A } };
static IAudioEndpointVolume *g_epv;

static BOOL CcVolInit(void) {
    if (g_epv) return TRUE;
    IMMDeviceEnumerator *en = NULL;
    if (FAILED(CoCreateInstance(&kCLSID_MMDeviceEnum, NULL, CLSCTX_ALL, &kIID_MMDeviceEnum, (void **)&en))) return FALSE;
    IMMDevice *dev = NULL;
    BOOL ok = FALSE;
    if (SUCCEEDED(IMMDeviceEnumerator_GetDefaultAudioEndpoint(en, eRender, eConsole, &dev))) {
        if (SUCCEEDED(IMMDevice_Activate(dev, &kIID_EndpointVol, CLSCTX_ALL, NULL, (void **)&g_epv))) ok = TRUE;
        IMMDevice_Release(dev);
    }
    IMMDeviceEnumerator_Release(en);
    return ok;
}

static int CcVolGet(void) {
    if (!CcVolInit()) return -1;
    float v = 0;
    if (FAILED(IAudioEndpointVolume_GetMasterVolumeLevelScalar(g_epv, &v))) return -1;
    return (int)(v * 100.f + 0.5f);
}

static void CcVolumeSet(int target) {
    if (target < 0) target = 0;
    if (target > 100) target = 100;
    g_ccVol = target;
    if (!CcVolInit()) return;
    IAudioEndpointVolume_SetMasterVolumeLevelScalar(g_epv, target / 100.f, NULL);
}
static BOOL CcMuteGet(void) {
    if (!CcVolInit()) return FALSE;
    BOOL m = FALSE;
    if (FAILED(IAudioEndpointVolume_GetMute(g_epv, &m))) return FALSE;
    return m;
}
static void CcMuteSet(BOOL m) {
    if (!CcVolInit()) return;
    IAudioEndpointVolume_SetMute(g_epv, m, NULL);
}

static BOOL CcEthUp(void) {
    /* WiFi is preferred: a machine with a connected Wi-Fi adapter is NOT "wired", even if a virtual
       ethernet adapter (Hyper-V / WSL / VPN) also has an address. */
    IP_ADAPTER_INFO buf[16];
    ULONG sz = sizeof buf;
    if (GetAdaptersInfo(buf, &sz) != ERROR_SUCCESS) return FALSE;
    BOOL wifiUp = FALSE, ethUp = FALSE;
    for (IP_ADAPTER_INFO *a = buf; a; a = a->Next) {
        if (a->AddressLength == 0) continue;   /* no address = not connected */
        if (a->Type == 71) wifiUp = TRUE;      /* MIB_IF_TYPE / IF_TYPE_IEEE80211 */
        else if (a->Type == 6) ethUp = TRUE;   /* IF_TYPE_ETHERNET_CSMACD */
    }
    return ethUp && !wifiUp;
}

static void CcGlyph(GpGraphics *g, int kind, float x, float y, float s, ARGB c) {
    GpPen *p;
    GdipCreatePen1(c, s * 0.13f, 2, &p);   /* bolder, consistent stroke weight */
    switch (kind) {
    case 0: MIcoWifi(g, x + s / 2, y + s / 2, s, c); break;
    case 1: MIcoBt(g, x + s / 2, y + s / 2, s, c); break;
    case 2:   /* crescent moon (two arcs, not a "C") */
        GdipDrawArc(g, p, x + s * 0.06f, y + s * 0.06f, s * 0.88f, s * 0.88f, -25, 260);
        GdipDrawArc(g, p, x + s * 0.24f, y + s * 0.02f, s * 0.86f, s * 0.86f, 175, -125);
        break;
    case 3:   /* location pin */
        GdipDrawEllipse(g, p, x + s * 0.20f, y + s * 0.12f, s * 0.60f, s * 0.60f);
        FillRR(g, c, x + s * 0.45f, y + s * 0.37f, s * 0.12f, s * 0.12f, s * 0.06f);
        GdipDrawLine(g, p, x + s * 0.38f, y + s * 0.66f, x + s * 0.50f, y + s * 0.94f);
        GdipDrawLine(g, p, x + s * 0.62f, y + s * 0.66f, x + s * 0.50f, y + s * 0.94f);
        break;
    case 4: MIcoVol(g, x + s / 2, y + s / 2, s, c); break;
    case 10: MIcoVolMute(g, x + s / 2, y + s / 2, s, c); break;   /* muted */
    case 5:   /* gear */
        GdipDrawEllipse(g, p, x + s * 0.24f, y + s * 0.24f, s * 0.52f, s * 0.52f);
        for (int i = 0; i < 4; i++) {
            float a = (float)i * 3.14159f / 2.f;
            GdipDrawLine(g, p, x + s * 0.50f + cosf(a) * s * 0.34f, y + s * 0.50f + sinf(a) * s * 0.34f,
                         x + s * 0.50f + cosf(a) * s * 0.46f, y + s * 0.50f + sinf(a) * s * 0.46f);
        }
        break;
    case 6:   /* leaf = energy saver */
        GdipDrawArc(g, p, x + s * 0.16f, y + s * 0.14f, s * 0.68f, s * 0.68f, 120, 200);
        GdipDrawLine(g, p, x + s * 0.22f, y + s * 0.78f, x + s * 0.76f, y + s * 0.24f);
        break;
    case 7:   /* hotspot: dot + arcs */
        FillRR(g, c, x + s * 0.44f, y + s * 0.56f, s * 0.12f, s * 0.12f, s * 0.06f);
        GdipDrawArc(g, p, x + s * 0.28f, y + s * 0.36f, s * 0.44f, s * 0.44f, 200, 140);
        GdipDrawArc(g, p, x + s * 0.14f, y + s * 0.20f, s * 0.72f, s * 0.72f, 200, 140);
        break;
    case 9:   /* sun = brightness */
        FillRR(g, c, x + s * 0.42f, y + s * 0.42f, s * 0.16f, s * 0.16f, s * 0.08f);
        for (int i = 0; i < 8; i++) {
            float a = (float)i * 3.14159f / 4.f;
            GdipDrawLine(g, p, x + s * 0.50f + cosf(a) * s * 0.26f, y + s * 0.50f + sinf(a) * s * 0.26f,
                         x + s * 0.50f + cosf(a) * s * 0.40f, y + s * 0.50f + sinf(a) * s * 0.40f);
        }
        break;
    default:  /* ethernet: plug */
        FillRR(g, c, x + s * 0.24f, y + s * 0.30f, s * 0.52f, s * 0.34f, s * 0.06f);
        GdipDrawLine(g, p, x + s * 0.36f, y + s * 0.64f, x + s * 0.36f, y + s * 0.80f);
        GdipDrawLine(g, p, x + s * 0.50f, y + s * 0.64f, x + s * 0.50f, y + s * 0.80f);
        GdipDrawLine(g, p, x + s * 0.64f, y + s * 0.64f, x + s * 0.64f, y + s * 0.80f);
        break;
    }
    GdipDeletePen(p);
}

static void CcSlider(GpGraphics *g, RECT *r, int val, int kind, ARGB tc, ARGB sub) {
    float x = (float)r->left, y = (float)r->top, w = (float)(r->right - r->left), h = (float)(r->bottom - r->top);
    CcGlyph(g, kind, x, y + (h - Sc(22)) / 2, Sc(22), sub);
    float tx = x + Sc(30), tw = w - Sc(30), ty = y + h / 2 - Sc(3);
    FillRR(g, g_dark ? ARGBC(70, 255, 255, 255) : ARGBC(40, 0, 0, 0), tx, ty, tw, Sc(6), Sc(3));
    float fw = tw * (val < 0 ? 0 : val) / 100.f;
    if (fw > 2) FillRR(g, CcAccent(), tx, ty, fw, Sc(6), Sc(3));
    float kx = tx + fw;
    if (kx < tx + Sc(7)) kx = tx + Sc(7);
    if (kx > tx + tw - Sc(7)) kx = tx + tw - Sc(7);
    FillRR(g, ARGBC(255, 255, 255, 255), kx - Sc(7), y + h / 2 - Sc(7), Sc(14), Sc(14), Sc(7));
    StrokeRR(g, g_dark ? ARGBC(60, 0, 0, 0) : ARGBC(50, 0, 0, 0), 1, kx - Sc(7), y + h / 2 - Sc(7), Sc(14), Sc(14), Sc(7));
}

/* Open a Windows settings / shell page.
   Only ShellExecute is used here. Starting a second explorer.exe to open the URI is NOT a safe fallback:
   the running shell already owns the session, the new Explorer cannot initialise and Windows shows the
   "应用程序无法正常启动 (0x0000142)" box. ShellExecute also returns small "error" values (e.g. 31, a DDE
   timeout) while the handler is still starting, so the return value must not trigger another launch. */
static BOOL OpenSettingsPage(const WCHAR *uri) {
    if (!uri || !uri[0]) return FALSE;
    SHELLEXECUTEINFOW ei;
    ZeroMemory(&ei, sizeof ei);
    ei.cbSize = sizeof ei;
    ei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
    ei.lpVerb = L"open";
    ei.lpFile = uri;
    ei.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&ei)) return TRUE;
    if ((INT_PTR)ShellExecuteW(NULL, L"open", uri, NULL, NULL, SW_SHOWNORMAL) > 32) return TRUE;
    return FALSE;
}
/* Open the Windows 11 quick-settings panel (Win+A): the switches can then be toggled right there
   instead of being sent to the Settings app. SendInput is used because keybd_event is ignored by the
   shell in some sessions. */
static BOOL OpenQuickSettings(void) {
    INPUT in[4];
    ZeroMemory(in, sizeof in);
    in[0].type = INPUT_KEYBOARD; in[0].ki.wVk = VK_LWIN;
    in[1].type = INPUT_KEYBOARD; in[1].ki.wVk = 'A';
    in[2].type = INPUT_KEYBOARD; in[2].ki.wVk = 'A'; in[2].ki.dwFlags = KEYEVENTF_KEYUP;
    in[3].type = INPUT_KEYBOARD; in[3].ki.wVk = VK_LWIN; in[3].ki.dwFlags = KEYEVENTF_KEYUP;
    UINT n = SendInput(4, in, sizeof(INPUT));
    return n == 4;
}

/* classic applets: used when the ms-settings handler cannot start (the URI silently does nothing there) */
static void OpenCplApplet(const WCHAR *cpl, const WCHAR *page) {
    if (page && page[0]) { if ((INT_PTR)ShellExecuteW(NULL, L"open", cpl, page, NULL, SW_SHOWNORMAL) > 32) return; }
    ShellExecuteW(NULL, L"open", cpl, NULL, NULL, SW_SHOWNORMAL);
}

static void CcDraw(PopCtx *c, GpGraphics *g) {
    CcU *u = (CcU *)c->u;
    static const WCHAR *nm[9] = { L"无线网络", L"蓝牙", L"深色模式", L"定位", L"静音", L"设置", L"节能模式", L"移动热点", L"有线网络" };
    ARGB tc = g_dark ? ARGBC(255, 242, 242, 246) : ARGBC(255, 28, 28, 34);
    ARGB sub = g_dark ? ARGBC(150, 200, 202, 214) : ARGBC(150, 90, 94, 108);
    float W = (float)c->W - 2 * PM, x0 = (float)PM + Sc(12);
    float tw = (W - Sc(24) - 3 * Sc(8)) / 4.f, th = Sc(66);   /* 4 columns × 2 rows */
    DrawCentered(g, L"控制中心", g_fontBold, (float)PM, (float)PM + Sc(6), W, Sc(28), tc);
    float y0 = (float)PM + Sc(40);
    int rows = (u->n + 3) / 4;
    if (rows < 1) rows = 1;
    for (int k = 0; k < u->n; k++) {
        int id = u->id[k];
        int col = k % 4, row = k / 4;
        float x = x0 + col * (tw + Sc(8)), y = y0 + row * (th + Sc(10));
        u->tile[k].left = (int)x; u->tile[k].top = (int)y; u->tile[k].right = (int)(x + tw); u->tile[k].bottom = (int)(y + th);
        BOOL on = (id == 2 && g_dark) || (id == 4 && u->mute) || (id == 6 && u->energy);
        ARGB bg = on ? CcAccent() : (g_dark ? ARGBC(58, 255, 255, 255) : ARGBC(22, 0, 0, 0));
        if (u->hover == k) bg = on ? CcAccent() : (g_dark ? ARGBC(88, 255, 255, 255) : ARGBC(38, 0, 0, 0));
        FillRR(g, bg, x, y, tw, th, Sc(14));
        int gid = (id == 0 && u->wired) ? 8 : id;
        if (id == 4 && u->mute) gid = 10;   /* speaker + x when muted */
        CcGlyph(g, id == 2 ? (on ? 2 : 9) : gid, x + (tw - Sc(22)) / 2, y + Sc(8), Sc(22), on ? ARGBC(255, 255, 255, 255) : tc);   /* dark mode: moon when on, sun when off */
        DrawCentered(g, (id == 0 && u->wired) ? L"有线网络" : nm[id], g_font, x, y + Sc(34), tw, Sc(20), on ? ARGBC(240, 255, 255, 255) : (g_dark ? ARGBC(235, 230, 233, 244) : ARGBC(235, 60, 64, 78)));
    }
    /* the volume slider follows the real number of tile rows (2 rows only when no tile is disabled) */
    float sy = y0 + rows * (th + Sc(10)) + Sc(2);
    u->vs.left = (int)x0; u->vs.top = (int)sy; u->vs.right = (int)(x0 + W - Sc(24)); u->vs.bottom = (int)(sy + Sc(28));
    CcSlider(g, &u->vs, u->vol, u->mute ? 10 : 4, tc, sub);
    float by = (float)u->vs.bottom + Sc(4);
    SYSTEM_POWER_STATUS ps;
    WCHAR bat[80] = L"电池 --";
    if (GetSystemPowerStatus(&ps) && ps.BatteryLifePercent != 255) wsprintfW(bat, L"电量 %d%%%s", ps.BatteryLifePercent, ps.ACLineStatus == 1 ? L" · 已接通电源" : L"");
    DrawTextBox(g, bat, g_font, x0, by, W - Sc(24), Sc(24), sub, TRUE);
}

static void CcMove(PopCtx *c, int x, int y) {
    CcU *u = (CcU *)c->u;
    int hv = -1;
    for (int k = 0; k < u->n; k++) if (x >= u->tile[k].left && x <= u->tile[k].right && y >= u->tile[k].top && y <= u->tile[k].bottom) hv = k;
    u->hover = hv;
    BOOL down = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
    if (!down) { u->drag = 0; u->dragIdx = -1; u->pressIdx = -1; return; }
    POINT p = { x, y };
    if (PtInRect(&u->vs, p) || (u->drag == 1 && y > u->vs.top - 8 && y < u->vs.bottom + 8)) {
        u->drag = 1;
        int v = (int)((x - u->vs.left - Sc(30)) * 100 / (u->vs.right - u->vs.left - Sc(30) - Sc(4)));
        if (v < 0) v = 0;
        if (v > 100) v = 100;
        if (v != u->vol) { u->vol = v; CcVolumeSet(v); }   /* move the knob too, not just the volume */
        return;
    }
    /* drag a tile to reorder it */
    if (u->pressIdx < 0 && hv >= 0) { u->pressIdx = hv; u->dragIdx = hv; u->moved = 0; }
    else if (u->dragIdx >= 0 && hv >= 0 && hv != u->dragIdx) {
        int t = u->id[u->dragIdx]; u->id[u->dragIdx] = u->id[hv]; u->id[hv] = t;
        u->dragIdx = hv; u->moved = 1;
    }
}
static void CcUp(PopCtx *c, int x, int y) {
    CcU *u = (CcU *)c->u;
    if (u->moved) { u->moved = 0; u->pressIdx = -1; u->dragIdx = -1; CcOrderSave(u->id); return; }   /* a tile was dragged: just save the new order */
    u->pressIdx = -1; u->dragIdx = -1;
    int k = -1;
    for (int i = 0; i < u->n; i++) if (x >= u->tile[i].left && x <= u->tile[i].right && y >= u->tile[i].top && y <= u->tile[i].bottom) k = i;
    if (k < 0) return;
    switch (u->id[k]) {
    case 0:   /* wired connections must open adapter settings, never Wi-Fi configuration */
        if (u->wired) OpenCplApplet(L"ncpa.cpl", NULL);
        else if (!OpenQuickSettings()) { if (!OpenSettingsPage(L"ms-settings:network-wifi")) OpenCplApplet(L"ncpa.cpl", NULL); }
        c->done = 1;
        break;
    case 1:   /* Bluetooth */
        if (!OpenQuickSettings()) { if (!OpenSettingsPage(L"ms-settings:bluetooth")) OpenCplApplet(L"bthprops.cpl", NULL); }
        c->done = 1;
        break;
    case 7: OpenSettingsPage(L"ms-settings:network-mobilehotspot"); c->done = 1; break;   /* mobile hotspot */
    case 2: {   /* dark mode */
        DWORD v = SystemDark() ? 1 : 0;
        HKEY key;
        BOOL ok = FALSE;
        if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize", 0, KEY_SET_VALUE, &key) == ERROR_SUCCESS) {
            RegSetValueExW(key, L"AppsUseLightTheme", 0, REG_DWORD, (const BYTE *)&v, sizeof v);
            RegSetValueExW(key, L"SystemUsesLightTheme", 0, REG_DWORD, (const BYTE *)&v, sizeof v);
            RegCloseKey(key);
            ok = TRUE;
        }
        if (ok) {
            UpdateTheme();
            SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 200, NULL);
            Kick();
        } else OpenSettingsPage(L"ms-settings:personalization-colors");   /* never leave a click without a result */
        break;
    }
    case 3: if (!OpenSettingsPage(L"ms-settings:privacy-location")) OpenCplApplet(L"control.exe", L"/name Microsoft.LocationPrivacySettings"); c->done = 1; break;
    case 4:   /* mute: use the real endpoint mute so the icon always matches the system */
        u->mute = !u->mute;
        CcMuteSet(u->mute);
        break;
    case 5: if (!OpenSettingsPage(L"ms-settings:")) OpenCplApplet(L"control.exe", L"/name Microsoft.ControlPanel"); c->done = 1; break;
    case 6:   /* open the Windows quick-settings/control centre, the same panel as Win+A */
        OpenQuickSettings();
        c->done = 1;
        break;
    default: OpenSettingsPage(L"ms-settings:network-status"); c->done = 1; break;
    }
}

/* ---- timer / stopwatch / todo widget ---- */
typedef struct {
    int mode;            /* 0 = countdown, 1 = stopwatch */
    int running, hover;
    DWORD t0;
    int remainMs, stopMs;
    int nT;
    WCHAR todo[16][96];
    int done[16];
    RECT rb[5], tr[17];
} ToolU;

#define TOOL_TOTAL_DEF (5 * 60 * 1000)

static int g_toolTotal = TOOL_TOTAL_DEF;
static int g_toolAddPending;

static void ToolLoad(ToolU *u) {
    u->nT = GetPrivateProfileIntW(L"Tool", L"N", 0, g_ini);
    if (u->nT > 16) u->nT = 16;
    for (int i = 0; i < u->nT; i++) {
        WCHAR k[16], v[160] = L"";
        wsprintfW(k, L"T%d", i);
        GetPrivateProfileStringW(L"Tool", k, L"", v, 160, g_ini);
        int d = 0;
        if (v[0] == L'1') { d = 1; memmove(v, v + 2, (lstrlenW(v) - 1) * sizeof(WCHAR)); }
        u->done[i] = d;
        lstrcpynW(u->todo[i], v, 96);
    }
    g_toolTotal = GetPrivateProfileIntW(L"Tool", L"Total", TOOL_TOTAL_DEF / 1000, g_ini) * 1000;
    u->remainMs = GetPrivateProfileIntW(L"Tool", L"Remain", g_toolTotal / 1000, g_ini) * 1000;
}

static void ToolSave(ToolU *u) {
    WCHAR k[16], v[160];
    wsprintfW(v, L"%d", u->nT);
    WritePrivateProfileStringW(L"Tool", L"N", v, g_ini);
    for (int i = 0; i < u->nT; i++) {
        wsprintfW(k, L"T%d", i);
        wsprintfW(v, L"%d|%s", u->done[i] ? 1 : 0, u->todo[i]);
        WritePrivateProfileStringW(L"Tool", k, v, g_ini);
    }
    wsprintfW(v, L"%d", g_toolTotal / 1000);
    WritePrivateProfileStringW(L"Tool", L"Total", v, g_ini);
    wsprintfW(v, L"%d", (u->remainMs < 0 ? 0 : u->remainMs) / 1000);
    WritePrivateProfileStringW(L"Tool", L"Remain", v, g_ini);
}

static int ToolRemain(ToolU *u) {
    if (u->mode == 1) return u->stopMs + (u->running ? (int)(GetTickCount() - u->t0) : 0);
    return u->remainMs - (u->running ? (int)(GetTickCount() - u->t0) : 0);
}

static void ToolDraw(PopCtx *c, GpGraphics *g) {
    ToolU *u = (ToolU *)c->u;
    ARGB tc = g_dark ? ARGBC(255, 242, 242, 246) : ARGBC(255, 28, 28, 34);
    ARGB sub = g_dark ? ARGBC(150, 200, 202, 214) : ARGBC(150, 90, 94, 108);
    float W = (float)c->W - 2 * PM, x0 = (float)PM + Sc(14);
    int t = ToolRemain(u);
    if (t < 0) t = 0;
    if (u->mode == 0 && u->running && t == 0) { u->running = 0; u->remainMs = 0; }
    WCHAR big[32];
    if (u->mode == 1) wsprintfW(big, L"%02d:%02d.%02d", t / 60000, (t / 1000) % 60, (t % 1000) / 10);
    else wsprintfW(big, L"%02d:%02d", t / 60000, (t / 1000) % 60);
    DrawCentered(g, L"计时器 · 待办", g_fontBold, (float)PM, (float)PM + Sc(6), W, Sc(26), tc);
    /* mode segments */
    float sy = (float)PM + Sc(36);
    u->rb[0].left = (int)(x0); u->rb[0].top = (int)sy; u->rb[0].right = (int)(x0 + Sc(64)); u->rb[0].bottom = (int)(sy + Sc(26));
    float x1 = x0 + Sc(68);
    u->rb[1].left = (int)x1; u->rb[1].top = (int)sy; u->rb[1].right = (int)(x1 + Sc(64)); u->rb[1].bottom = (int)(sy + Sc(26));
    FillRR(g, u->mode == 0 ? CcAccent() : (g_dark ? ARGBC(60, 255, 255, 255) : ARGBC(26, 0, 0, 0)), (float)u->rb[0].left, sy, Sc(64), Sc(26), Sc(13));
    FillRR(g, u->mode == 1 ? CcAccent() : (g_dark ? ARGBC(60, 255, 255, 255) : ARGBC(26, 0, 0, 0)), (float)u->rb[1].left, sy, Sc(64), Sc(26), Sc(13));
    DrawCentered(g, L"计时", g_font, (float)u->rb[0].left, sy, Sc(64), Sc(26), u->mode == 0 ? ARGBC(255, 255, 255, 255) : sub);
    DrawCentered(g, L"秒表", g_font, (float)u->rb[1].left, sy, Sc(64), Sc(26), u->mode == 1 ? ARGBC(255, 255, 255, 255) : sub);
    DrawCentered(g, big, g_fontTitle, x0, sy + Sc(34), W - Sc(28), Sc(64), tc);
    /* start / reset */
    float by = sy + Sc(104);
    u->rb[2].left = (int)x0; u->rb[2].top = (int)by; u->rb[2].right = (int)(x0 + W / 2 - Sc(8)); u->rb[2].bottom = (int)(by + Sc(34));
    u->rb[3].left = (int)(x0 + W / 2 + Sc(8)); u->rb[3].top = (int)by; u->rb[3].right = (int)(x0 + W - Sc(28)); u->rb[3].bottom = (int)(by + Sc(34));
    FillRR(g, CcAccent(), (float)u->rb[2].left, by, (float)(u->rb[2].right - u->rb[2].left), Sc(34), Sc(12));
    FillRR(g, g_dark ? ARGBC(60, 255, 255, 255) : ARGBC(26, 0, 0, 0), (float)u->rb[3].left, by, (float)(u->rb[3].right - u->rb[3].left), Sc(34), Sc(12));
    DrawCentered(g, u->running ? L"暂停" : L"开始", g_fontBold, (float)u->rb[2].left, by, (float)(u->rb[2].right - u->rb[2].left), Sc(34), ARGBC(255, 255, 255, 255));
    DrawCentered(g, L"重置", g_font, (float)u->rb[3].left, by, (float)(u->rb[3].right - u->rb[3].left), Sc(34), tc);
    /* todo */
    float ty = by + Sc(46);
    DrawTextBox(g, L"待办事项", g_fontBold, x0, ty, W, Sc(22), tc, TRUE);
    ty += Sc(24);
    for (int i = 0; i < u->nT && i < 16; i++) {
        u->tr[i].left = (int)x0; u->tr[i].top = (int)ty; u->tr[i].right = (int)(x0 + W - Sc(28)); u->tr[i].bottom = (int)(ty + Sc(24));
        if (u->hover == 100 + i) FillRR(g, g_dark ? ARGBC(40, 255, 255, 255) : ARGBC(20, 0, 0, 0), x0, ty, W - Sc(28), Sc(24), Sc(8));
        FillRR(g, u->done[i] ? CcAccent() : (g_dark ? ARGBC(70, 255, 255, 255) : ARGBC(40, 0, 0, 0)), x0 + Sc(2), ty + Sc(6), Sc(12), Sc(12), Sc(6));
        if (u->done[i]) DrawCentered(g, L"✓", g_font, x0 + Sc(2), ty + Sc(4), Sc(12), Sc(14), ARGBC(255, 255, 255, 255));
        DrawTextBox(g, u->todo[i], g_font, x0 + Sc(22), ty, W - Sc(60), Sc(24), u->done[i] ? sub : tc, TRUE);
        DrawTextBoxR(g, L"✕", g_font, x0, ty, W - Sc(30), Sc(24), sub);
        ty += Sc(25);
    }
    u->tr[u->nT].left = (int)x0; u->tr[u->nT].top = (int)ty; u->tr[u->nT].right = (int)(x0 + W - Sc(28)); u->tr[u->nT].bottom = (int)(ty + Sc(24));
    if (u->hover == 199) FillRR(g, g_dark ? ARGBC(40, 255, 255, 255) : ARGBC(20, 0, 0, 0), x0, ty, W - Sc(28), Sc(24), Sc(8));
    DrawTextBox(g, L"＋ 添加待办…", g_font, x0 + Sc(2), ty, W - Sc(40), Sc(24), sub, TRUE);
}

static void ToolMove(PopCtx *c, int x, int y) {
    ToolU *u = (ToolU *)c->u;
    POINT p = { x, y };
    int hv = -1;
    for (int i = 0; i < 4; i++) if (PtInRect(&u->rb[i], p)) hv = i;
    for (int i = 0; i <= u->nT && i < 17; i++) if (PtInRect(&u->tr[i], p)) hv = i == u->nT ? 199 : 100 + i;
    u->hover = hv;
}

static void ToolUp(PopCtx *c, int x, int y) {
    ToolU *u = (ToolU *)c->u;
    POINT p = { x, y };
    if (PtInRect(&u->rb[0], p)) { u->mode = 0; u->running = 0; return; }
    if (PtInRect(&u->rb[1], p)) { u->mode = 1; u->running = 0; return; }
    if (PtInRect(&u->rb[2], p)) {
        if (u->running) {   /* pause */
            if (u->mode == 0) u->remainMs = ToolRemain(u); else u->stopMs = ToolRemain(u);
            if (u->remainMs < 0) u->remainMs = 0;
            u->running = 0;
        } else {
            u->t0 = GetTickCount();
            u->running = 1;
        }
        return;
    }
    if (PtInRect(&u->rb[3], p)) {
        u->running = 0;
        if (u->mode == 0) u->remainMs = g_toolTotal; else u->stopMs = 0;
        return;
    }
    for (int i = 0; i <= u->nT && i < 17; i++) {
        if (!PtInRect(&u->tr[i], p)) continue;
        if (i == u->nT) {   /* add: close the panel first, the input dialog needs the mouse */
            g_toolAddPending = 1;
            c->done = 1;
        } else if (x > u->tr[i].right - Sc(26)) {   /* delete */
            memmove(&u->todo[i], &u->todo[i + 1], sizeof(u->todo[0]) * (u->nT - i - 1));
            memmove(&u->done[i], &u->done[i + 1], sizeof(int) * (u->nT - i - 1));
            u->nT--;
            ToolSave(u);
        } else {
            u->done[i] = !u->done[i];
            ToolSave(u);
        }
        return;
    }
}

static void ShowTool(void) {
    if (GetTickCount() - g_popupClosedTick < 350) return;
    ToolU *u = (ToolU *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ToolU));
    PopCtx *c = (PopCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PopCtx));
    if (!u || !c) return;
    ToolLoad(u);
    u->hover = -1;
    c->u = u; c->draw = ToolDraw; c->mv = ToolMove; c->up = ToolUp; c->live = TRUE;
    PopRun(&g_wScr[W_TOOL], Sc(360), Sc(470), c);
    if (g_toolAddPending) {
        g_toolAddPending = 0;
        WCHAR nm[96] = L"";
        if (InputBox(g_hset, L"添加待办", L"内容：", nm, 96, NULL, NULL, 0)) {
            StrTrimW(nm, L" \t");
            if (nm[0]) {
                ToolU *v = (ToolU *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ToolU));
                if (v) { ToolLoad(v); if (v->nT < 16) { lstrcpynW(v->todo[v->nT], nm, 96); v->done[v->nT] = 0; v->nT++; ToolSave(v); } HeapFree(GetProcessHeap(), 0, v); }
            }
        }
    }
    HeapFree(GetProcessHeap(), 0, c);
    HeapFree(GetProcessHeap(), 0, u);
}

static void DrawTool(GpGraphics *g, float x, float y, float w, float h) {
    float s = (w < h ? w : h) * 0.46f, cx = x + w / 2, cy = y + h / 2;
    ARGB c = g_dark ? ARGBC(235, 240, 240, 246) : ARGBC(232, 30, 32, 40);
    GpPen *p;
    GdipCreatePen1(c, s > 24 ? 2.6f : 2.1f, 2, &p);
    GdipDrawEllipse(g, p, cx - s / 2, cy - s / 2, s, s);
    GdipDrawLine(g, p, cx, cy - s * 0.32f, cx, cy);
    GdipDrawLine(g, p, cx, cy, cx + s * 0.28f, cy + s * 0.18f);
    GdipDeletePen(p);
}
/* ---- resource monitor ---- */
static int ResCpuPercent(void) {
    static ULONGLONG pi, pk, pu;
    FILETIME fi, fk, fu;
    if (!GetSystemTimes(&fi, &fk, &fu)) return 0;
    ULONGLONG i = ((ULONGLONG)fi.dwHighDateTime << 32) | fi.dwLowDateTime;
    ULONGLONG k = ((ULONGLONG)fk.dwHighDateTime << 32) | fk.dwLowDateTime;
    ULONGLONG u = ((ULONGLONG)fu.dwHighDateTime << 32) | fu.dwLowDateTime;
    int pct = 0;
    if (pi) {
        ULONGLONG di = i - pi, dk = k - pk, du = u - pu, tot = dk + du;
        if (tot > 0) pct = (int)((tot - di) * 100 / tot);
    }
    pi = i; pk = k; pu = u;
    return pct < 0 ? 0 : pct > 100 ? 100 : pct;
}

static void DrawPowerWidget(GpGraphics *g, float x, float y, float w, float h) {
    ARGB c = g_dark ? ARGBC(235, 238, 240, 246) : ARGBC(235, 40, 44, 54);
    float d = (h < w ? h : w) * 0.62f, cx = x + w / 2, cy = y + h / 2;
    GpPen *p;
    GdipCreatePen1(c, d * 0.10f, 2, &p);
    GdipDrawArc(g, p, cx - d / 2, cy - d / 2 + d * 0.06f, d, d, -50, 280);
    GdipDrawLine(g, p, cx, cy - d * 0.56f, cx, cy - d * 0.10f);
    GdipDeletePen(p);
}

static void DrawRes(GpGraphics *g, float x, float y, float w, float h) {
    WCHAR t[64];
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof ms;
    GlobalMemoryStatusEx(&ms);
    if (cfg.wideMask & (1 << W_RES)) wsprintfW(t, L"CPU %d%%   RAM %d%%", ResCpuPercent(), (int)ms.dwMemoryLoad);   /* merged double-width view */
    else wsprintfW(t, L"CPU %d%%", ResCpuPercent());
    DrawCentered(g, t, g_fS, x, y, w, h, g_dark ? ARGBC(232, 240, 240, 245) : ARGBC(232, 30, 32, 40));
}

typedef struct { int cpu, mem, disk; ULONGLONG memUsed, memTot, dFree, dTot; } ResU;

static void ResSample(ResU *u) {
    u->cpu = ResCpuPercent();
    MEMORYSTATUSEX ms;
    ms.dwLength = sizeof ms;
    if (GlobalMemoryStatusEx(&ms)) {
        u->mem = (int)ms.dwMemoryLoad;
        u->memTot = ms.ullTotalPhys / (1024 * 1024);
        u->memUsed = (ms.ullTotalPhys - ms.ullAvailPhys) / (1024 * 1024);
    }
    ULARGE_INTEGER fr, tt;
    if (GetDiskFreeSpaceExW(L"C:\\", &fr, &tt, NULL)) {
        u->dFree = fr.QuadPart / (1024ULL * 1024 * 1024);
        u->dTot = tt.QuadPart / (1024ULL * 1024 * 1024);
        u->disk = u->dTot ? (int)((u->dTot - u->dFree) * 100 / u->dTot) : 0;
    }
}

static void ResBar(GpGraphics *g, float x, float y, float w, const WCHAR *label, int pct, const WCHAR *detail, ARGB tc, ARGB sub) {
    DrawTextBox(g, label, g_font, x, y - Sc(2), Sc(120), Sc(22), tc, TRUE);
    DrawTextBoxR(g, detail, g_font, x, y - Sc(2), w, Sc(22), sub);
    float by = y + Sc(22);
    FillRR(g, g_dark ? ARGBC(70, 255, 255, 255) : ARGBC(35, 0, 0, 0), x, by, w, Sc(6), Sc(3));
    float fw = w * (pct < 0 ? 0 : pct > 100 ? 100 : pct) / 100.f;
    if (fw > 2) FillRR(g, CcAccent(), x, by, fw, Sc(6), Sc(3));
}

static void ResDraw(PopCtx *c, GpGraphics *g) {
    ResU *u = (ResU *)c->u;
    ResSample(u);
    ARGB tc = g_dark ? ARGBC(255, 242, 242, 246) : ARGBC(255, 28, 28, 34);
    ARGB sub = g_dark ? ARGBC(150, 200, 202, 214) : ARGBC(150, 90, 94, 108);
    float W = (float)c->W - 2 * PM, x0 = (float)PM + Sc(14);
    DrawCentered(g, L"资源监视器", g_fontBold, (float)PM, (float)PM + Sc(8), W, Sc(28), tc);
    float y = (float)PM + Sc(50);
    WCHAR d[64];
    wsprintfW(d, L"%d%%", u->cpu);
    ResBar(g, x0, y, W - Sc(28), L"处理器", u->cpu, d, tc, sub);
    y += Sc(52);
    wsprintfW(d, L"%I64u / %I64u MB", u->memUsed, u->memTot);
    ResBar(g, x0, y, W - Sc(28), L"内存", u->mem, d, tc, sub);
    y += Sc(52);
    wsprintfW(d, L"可用 %I64u / %I64u GB", u->dFree, u->dTot);
    ResBar(g, x0, y, W - Sc(28), L"磁盘（C:）", u->disk, d, tc, sub);
    y += Sc(52);
    DrawTextBox(g, L"GPU 需要系统计数器支持，暂未采集", g_font, x0, y, W - Sc(28), Sc(22), sub, TRUE);
}

static void ShowResMon(void) {
    if (GetTickCount() - g_popupClosedTick < 350) return;
    ResU *u = (ResU *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(ResU));
    PopCtx *c = (PopCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PopCtx));
    if (!u || !c) return;
    ResSample(u);
    c->u = u; c->draw = ResDraw; c->live = TRUE;
    PopRun(&g_wScr[W_RES], Sc(340), PM * 2 + Sc(50) + 3 * Sc(52) + Sc(30), c);
    HeapFree(GetProcessHeap(), 0, c);
    HeapFree(GetProcessHeap(), 0, u);
}

/* ---- dock focus modes: a mode is a full snapshot of the configuration ---- */
static void ProfileDir(WCHAR *out) {
    PathCombineW(out, g_dir, L"profiles");
    CreateDirectoryW(out, NULL);
}

static void ProfilePath(const WCHAR *name, WCHAR *out) {
    WCHAR d[MAX_PATH];
    ProfileDir(d);
    PathCombineW(out, d, name);
    lstrcatW(out, L".ini");
}

static void ProfileCurrent(WCHAR *out, int cch) {
    WCHAR f[MAX_PATH];
    out[0] = 0;
    PathCombineW(f, g_dir, L"profile.txt");
    GetPrivateProfileStringW(L"M", L"Name", L"", out, cch, f);
}

static void ProfileSetCurrent(const WCHAR *name) {
    WCHAR f[MAX_PATH];
    PathCombineW(f, g_dir, L"profile.txt");
    WritePrivateProfileStringW(L"M", L"Name", name, f);
}

static void ReloadAll(void) {
    if (g_hwnd) WTSUnRegisterSessionNotification(g_hwnd);
    for (int i = 0; i < nItems; i++) FreeItem(&items[i]);
    nItems = 0;
    LoadConfig();
    LoadItems();
    UpdateTheme();
    RefreshAccent();
    ApplySettings();
    UpdateGeometry();
    Kick();
}

static void ProfileSwitch(const WCHAR *name) {
    if (!name || !name[0]) return;
    WCHAR cur[64], src[MAX_PATH], old[MAX_PATH], bak[MAX_PATH];
    ProfileCurrent(cur, 64);
    SaveNow();
    ProfilePath(name, src);
    if (GetFileAttributesW(src) == INVALID_FILE_ATTRIBUTES) {   /* first use: snapshot the current layout */
        CopyFileW(g_ini, src, FALSE);
        ProfileSetCurrent(name);
        return;
    }
    if (cur[0] && lstrcmpW(cur, name) != 0) { ProfilePath(cur, old); CopyFileW(g_ini, old, FALSE); }
    lstrcpyW(bak, g_ini); lstrcatW(bak, L".bak");
    CopyFileW(g_ini, bak, FALSE);
    if (CopyFileW(src, g_ini, FALSE)) {
        ProfileSetCurrent(name);
        ReloadAll();
    }
}

static void ShowModeMenu(void) {
    WCHAR d[MAX_PATH], pat[MAX_PATH], names[8][64];
    ProfileDir(d);
    PathCombineW(pat, d, L"*.ini");
    int n = 0;
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pat, &fd);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
            if (n >= 8) break;
            lstrcpynW(names[n], fd.cFileName, 64);
            WCHAR *dot = wcsrchr(names[n], L'.');
            if (dot) *dot = 0;
            n++;
        } while (FindNextFileW(h, &fd));
        FindClose(h);
    }
    WCHAR cur[64];
    ProfileCurrent(cur, 64);
    CmItem it[12];
    int m = 0;
    for (int i = 0; i < n; i++) {
        WCHAR label[96];
        wsprintfW(label, L"%s%s", names[i], lstrcmpW(names[i], cur) == 0 ? L"   ✓" : L"");
        m = CmAdd(it, m, 10 + i, label, 0);
    }
    if (n) m = CmAdd(it, m, 0, L"", CM_SEP);
    m = CmAdd(it, m, 1, L"保存当前布局为新模式…", 0);
    if (cur[0]) m = CmAdd(it, m, 2, L"用当前布局覆盖此模式", 0);
    POINT p;
    GetCursorPos(&p);
    gMenuOpen = TRUE;
    int r = ShowMenu(it, m, p.x, p.y);
    gMenuOpen = FALSE;
    if (r == 1) {
        WCHAR nm[64] = L"";
        if (InputBox(g_hset, L"新建专注模式", L"名称（如 工作 / 开发 / 娱乐）：", nm, 64, NULL, NULL, 0)) {
            StrTrimW(nm, L" \t");
            if (nm[0]) {
                WCHAR dst[MAX_PATH];
                SaveNow();
                ProfilePath(nm, dst);
                CopyFileW(g_ini, dst, FALSE);
                ProfileSetCurrent(nm);
            }
        }
    } else if (r == 2 && cur[0]) {
        WCHAR dst[MAX_PATH];
        SaveNow();
        ProfilePath(cur, dst);
        CopyFileW(g_ini, dst, FALSE);
    } else if (r >= 10 && r - 10 < n) {
        ProfileSwitch(names[r - 10]);
    }
    Kick();
}

static void DrawMode(GpGraphics *g, float x, float y, float w, float h) {
    float s = (w < h ? w : h) * 0.50f, cx = x + w / 2, cy = y + h / 2;
    ARGB c = g_dark ? ARGBC(235, 240, 240, 246) : ARGBC(232, 30, 32, 40);
    GpPen *p;
    GdipCreatePen1(c, s > 24 ? 2.4f : 2.0f, 2, &p);
    GdipDrawEllipse(g, p, cx - s * 0.42f, cy - s * 0.42f, s * 0.84f, s * 0.84f);   /* round plate */
    GdipDeletePen(p);
    GpPath *mp = NULL;
    if (GdipCreatePath(0, &mp) == 0) {   /* solid crescent moon inside */
        float R = s * 0.26f;
        GdipAddPathArc(mp, cx - R, cy - R, R * 2, R * 2, 45, 280);
        GdipAddPathArc(mp, cx - R * 0.55f, cy - R * 0.80f, R * 1.2f, R * 1.6f, 225, 150);
        GdipClosePathFigure(mp);
        GpBrush *fb = NULL;
        GdipCreateSolidFill(c, &fb);
        if (fb) { GdipFillPath(g, fb, mp); GdipDeleteBrush(fb); }
        GdipDeletePath(mp);
    }
}
/* saved tile order (8 ids), so the user can rearrange the control centre */

static void CcOrderLoad(int out[8]) {
    for (int i = 0; i < 8; i++) out[i] = i;
    WCHAR v[128] = L"";
    GetPrivateProfileStringW(L"Cc", L"Order", L"", v, 128, g_ini);
    if (!v[0]) return;
    int n = 0, seen[8] = { 0 };
    WCHAR *p = v;
    while (*p && n < 8) {
        int id = (int)wcstol(p, &p, 10);
        if (id >= 0 && id < 8 && !seen[id]) { seen[id] = 1; out[n++] = id; }
        while (*p && *p != L',') p++;
        if (*p == L',') p++;
    }
    for (int id = 0; id < 8 && n < 8; id++) if (!seen[id]) { seen[id] = 1; out[n++] = id; }   /* fill gaps */
}

static void CcOrderSave(const int in[8]) {
    WCHAR v[64];
    int o = 0, seen[8] = { 0 };
    for (int i = 0; i < 8; i++) {
        int id = in[i];
        if (id < 0 || id > 7 || seen[id]) continue;
        seen[id] = 1;
        o += wsprintfW(v + o, L"%s%d", o ? L"," : L"", id);
    }
    for (int id = 0; id < 8; id++) if (!seen[id]) o += wsprintfW(v + o, L"%s%d", o ? L"," : L"", id);
    WritePrivateProfileStringW(L"Cc", L"Order", v, g_ini);
}

static void ShowCtrlCenter(void) {
    if (GetTickCount() - g_popupClosedTick < 350) return;
    CcU *u = (CcU *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(CcU));
    PopCtx *c = (PopCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PopCtx));
    if (!u || !c) return;
    u->hover = -1;
    u->hover = -1;
    u->wired = CcEthUp();
    CcOrderLoad(u->id);
    for (int k = 0; k < 8; k++) u->id[u->n++] = u->id[k];   /* all eight switches; drag a tile to reorder */
    u->vol = CcVolGet(); if (u->vol < 0) u->vol = g_ccVol;
    u->mute = CcMuteGet();
    u->energy = CcEnergyIsSaver();
    u->bri = g_ccBri;
    c->u = u; c->draw = CcDraw; c->mv = CcMove; c->up = CcUp;
    int rows = (u->n + 3) / 4;
    if (rows < 1) rows = 1;
    PopRun(&g_wScr[W_CC], Sc(360), PM * 2 + Sc(40) + rows * (Sc(66) + Sc(10)) + Sc(2) + Sc(28) + Sc(4) + Sc(26) + Sc(10), c);
    g_ccBri = u->bri;
    HeapFree(GetProcessHeap(), 0, c);
    HeapFree(GetProcessHeap(), 0, u);
}

typedef struct { int hover; RECT r[3]; } PowerU;
static void DoPowerAction(int k) {
    if (k == 2) { LockWorkStation(); return; }
    HANDLE tok = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &tok)) {
        TOKEN_PRIVILEGES tp;
        ZeroMemory(&tp, sizeof tp);
        tp.PrivilegeCount = 1;
        if (LookupPrivilegeValueW(NULL, SE_SHUTDOWN_NAME, &tp.Privileges[0].Luid)) {
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(tok, FALSE, &tp, sizeof tp, NULL, NULL);
        }
        CloseHandle(tok);
    }
    ExitWindowsEx((k == 0 ? EWX_POWEROFF : EWX_REBOOT) | EWX_FORCEIFHUNG, SHTDN_REASON_FLAG_PLANNED | SHTDN_REASON_MAJOR_OPERATINGSYSTEM);
}
static void PowerDraw(PopCtx *c, GpGraphics *g) {
    PowerU *u = (PowerU *)c->u;
    static const WCHAR *nm[3] = { L"关机", L"重启", L"锁屏" };
    ARGB tc = g_dark ? ARGBC(255, 242, 242, 246) : ARGBC(255, 28, 28, 34);
    float W = (float)c->W - 2 * PM, x0 = (float)PM + Sc(12);
    DrawCentered(g, L"电源", g_fontBold, (float)PM, (float)PM + Sc(6), W, Sc(26), tc);
    for (int k = 0; k < 3; k++) {
        float y = (float)PM + Sc(40) + k * Sc(48);
        u->r[k].left = (int)x0; u->r[k].top = (int)y; u->r[k].right = (int)(x0 + W - Sc(24)); u->r[k].bottom = (int)(y + Sc(40));
        ARGB bg = (u->hover == k) ? (g_dark ? ARGBC(80, 255, 255, 255) : ARGBC(40, 0, 0, 0)) : (g_dark ? ARGBC(46, 255, 255, 255) : ARGBC(18, 0, 0, 0));
        FillRR(g, bg, x0, y, W - Sc(24), Sc(40), Sc(12));
        float gx = x0 + Sc(14), gy = y + Sc(10), gs = Sc(20);
        GpPen *p;
        GdipCreatePen1(tc, Sc(2), 2, &p);
        GdipDrawArc(g, p, gx, gy + Sc(2), gs, gs, -50, 280);
        GdipDrawLine(g, p, gx + gs / 2, gy - Sc(2), gx + gs / 2, gy + Sc(8));
        GdipDeletePen(p);
        DrawTextBox(g, nm[k], g_font, x0 + Sc(44), y + Sc(9), W - Sc(70), Sc(24), tc, TRUE);
    }
}
static void PowerMove(PopCtx *c, int x, int y) {
    PowerU *u = (PowerU *)c->u;
    int hv = -1;
    for (int k = 0; k < 3; k++) if (x >= u->r[k].left && x <= u->r[k].right && y >= u->r[k].top && y <= u->r[k].bottom) hv = k;
    u->hover = hv;
}
static void PowerUp(PopCtx *c, int x, int y) {
    PowerU *u = (PowerU *)c->u;
    for (int k = 0; k < 3; k++) if (x >= u->r[k].left && x <= u->r[k].right && y >= u->r[k].top && y <= u->r[k].bottom) {
        if (k == 2) { LockWorkStation(); c->done = 1; return; }
        if (MessageBoxW(NULL, k == 0 ? L"确定要关机吗？" : L"确定要重启吗？", L"MiniDock", MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES) { c->done = 1; DoPowerAction(k); }
        return;
    }
}

static void ShowPowerMenu(void) {
    if (GetTickCount() - g_popupClosedTick < 350) return;
    PowerU *u = (PowerU *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PowerU));
    PopCtx *c = (PopCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PopCtx));
    if (!u || !c) return;
    u->hover = -1;
    c->u = u; c->draw = PowerDraw; c->mv = PowerMove; c->up = PowerUp;
    PopRun(&g_wScr[W_POWER], Sc(240), PM * 2 + Sc(40) + 3 * Sc(48) + Sc(6), c);
    HeapFree(GetProcessHeap(), 0, c);
    HeapFree(GetProcessHeap(), 0, u);
}

static void ShowNotifyList(void) {
    if (GetTickCount() - g_popupClosedTick < 350) return;
    NfU *u = (NfU *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NfU));
    PopCtx *c = (PopCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(PopCtx));
    if (!u || !c) return;
    for (int i = g_nnf - 1; i >= 0; i--) u->e[u->n++] = g_nf[i];   /* newest first */
    u->hover = -1;
    c->u = u; c->draw = NfDraw; c->mv = NfMove; c->up = NfUp;
    int rows = u->n > 0 ? u->n : 3;
    PopRun(&g_wScr[W_NOTIFY], Sc(380), PM * 2 + Sc(70) + rows * (Sc(54) + 4) + Sc(20), c);
    for (int i = 0; i < u->n; i++) if (u->ic[i]) GdipDisposeImage(u->ic[i]);
    HeapFree(GetProcessHeap(), 0, c);
    HeapFree(GetProcessHeap(), 0, u);
}

/* ---- system tray: open the Windows notification area (overflow), Win+B as a reliable fallback ---- */
/* the overflow flyout is an XAML island on Windows 11 and a classic window on Windows 10 */
static BOOL TrayFlyoutOpen(void) {
    static const WCHAR *cls[] = { L"TopLevelWindowForOverflowXamlIsland", L"NotifyIconOverflowWindow" };
    for (int i = 0; i < 2; i++) {
        HWND h = FindWindowW(cls[i], NULL);
        if (h && IsWindowVisible(h)) return TRUE;
    }
    return FALSE;
}
static void TrayOpen(void) {
    HWND st = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!st) return;
    ShowWindow(st, SW_SHOW);
    /* Win+B focuses the notification area on both Windows 10 and 11; Enter opens the hidden icons.
       (The old code clicked the classic Toolbar32 chevron, which succeeds on Windows 11 without opening
       anything - so the Win+B fallback never ran. Do Win+B first, then use the chevron only as a fallback.) */
    keybd_event(VK_LWIN, 0, KEYEVENTF_EXTENDEDKEY, 0);
    keybd_event('B', 0, 0, 0);
    keybd_event('B', 0, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_LWIN, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
    Sleep(200);
    keybd_event(VK_RETURN, 0, 0, 0);
    keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
    Sleep(350);
    if (TrayFlyoutOpen()) return;
    /* fallback: the classic notification-area toolbar (Windows 10 and some shells) */
    HWND tn = FindWindowExW(st, NULL, L"TrayNotifyWnd", NULL);
    HWND sp = tn ? FindWindowExW(tn, NULL, L"SysPager", NULL) : NULL;
    HWND tb = sp ? FindWindowExW(sp, NULL, L"ToolbarWindow32", NULL) : NULL;
    if (!tb) return;
    int cnt = (int)SendMessageW(tb, TB_BUTTONCOUNT, 0, 0);
    if (cnt <= 0) return;
    RECT r;
    if (!SendMessageW(tb, TB_GETITEMRECT, cnt - 1, (LPARAM)&r)) return;
    POINT pt = { (r.left + r.right) / 2, (r.top + r.bottom) / 2 };
    ClientToScreen(tb, &pt);
    SetCursorPos(pt.x, pt.y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

static void DrawTray(GpGraphics *g, float x, float y, float w, float h) {
    MIcoChev(g, x + w / 2, y + h / 2, (w < h ? w : h) * 0.50f, g_dark ? ARGBC(235, 240, 240, 246) : ARGBC(232, 30, 32, 40));
}

/* ---- macOS-style top status bar: time / CPU / control centre / tray, replaces the dock widgets ---- */
#define CLS_SB L"MiniDockStatusBar"
#define T_SB 13
static HWND g_sb;
static Canvas g_sbCv;
static void SbAppBar(BOOL on);
static void SbTick(void);
static void SbDraw(HWND h);

static void SbGeom(int *x, int *y, int *w, int *h) {
    int side = cfg.sbFloat ? Sc(cfg.sbSideGap) : 0;
    *h = Sc(cfg.sbHeight);
    *w = (g_mi.rcMonitor.right - g_mi.rcMonitor.left) - 2 * side;
    *x = g_mi.rcMonitor.left + side;
    int base = cfg.sbMaxAvoid ? g_mi.rcMonitor.top : g_mi.rcWork.top;
    *y = base + cfg.sbTopGap;
}

static RECT g_sbR[12];   /* 0 MiniDock, 1 clock, 2 CC, 3 tray, 4 cpu, 5 battery, 6 bell, 7 wifi, 8 volume, 9 bluetooth, 10 IME, 11 focus mode */
static BOOL g_sbContentSuppressed;
static BOOL g_sbAbDirty;
static int g_sbHover = -1;
static void ShowSystemMenu(void);
static void ShowWifiPopup(void);
static void ShowBtPopup(void);
static void ShowVolPopup(void);
static void ToggleIme(void);

/* Status-bar policy is independent of the Dock's game mode and recorder exemptions.
   Return 2 only for true fullscreen video/game content, 1 for a maximized window. */
static int SbForegroundMode(void) {
    HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == g_hwnd || foreground == g_hset || foreground == g_sb ||
        foreground == g_lpWnd || foreground == GetDesktopWindow() || foreground == GetShellWindow() ||
        !IsWindowVisible(foreground) || IsIconic(foreground) || GetAncestor(foreground, GA_ROOT) != foreground) return 0;
    LONG extended = GetWindowLongW(foreground, GWL_EXSTYLE);
    if (extended & (WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE)) return 0;
    WCHAR cls[64]; GetClassNameW(foreground, cls, 64);
    if (ShellChromeClass(cls)) return 0;
    int cloaked = 0;
    DwmGetWindowAttribute(foreground, 14, &cloaked, sizeof cloaked);
    if (cloaked) return 0;
    MONITORINFO monitor = { sizeof monitor };
    if (!GetMonitorInfoW(MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST), &monitor) ||
        !EqualRect(&monitor.rcMonitor, &g_mi.rcMonitor)) return 0;
    RECT frame;
    if (FAILED(DwmGetWindowAttribute(foreground, 9, &frame, sizeof frame)) && !GetWindowRect(foreground, &frame)) return 0;
    BOOL maximized = IsZoomed(foreground);
    BOOL fullscreen = frame.left <= monitor.rcMonitor.left + 1 && frame.top <= monitor.rcMonitor.top + 1 &&
        frame.right >= monitor.rcMonitor.right - 1 && frame.bottom >= monitor.rcMonitor.bottom - 1;
    /* Maximization, including filling the monitor with an auto-hidden taskbar, is not
       fullscreen. A true fullscreen surface also removes its normal window frame. */
    if (GetWindowLongW(foreground, GWL_STYLE) & (WS_CAPTION | WS_THICKFRAME)) fullscreen = FALSE;
    if (!fullscreen) return maximized ? 1 : 0;
    WinId identity; WinIdOf(foreground, &identity);
    const WCHAR *exe = PathFindFileNameW(identity.exe);
    BOOL video = MediaExeName(exe) && lstrcmpiW(exe, L"applicationframehost.exe");
    if (BrowserExeName(exe)) video = TRUE; /* only reached for a real fullscreen browser surface */
    BOOL game = fullscreen && ShellQuns() == QUNS_D3D_;
    LoadGames();
    for (int i = 0; !game && i < g_ngames; i++) if (g_games[i][0] && !lstrcmpiW(exe, g_games[i])) game = TRUE;
    if (!game) {
        static const WCHAR *games[] = { L"League of Legends.exe", L"LeagueClientUx.exe", L"VALORANT-Win64-Shipping.exe",
            L"dota2.exe", L"cs2.exe", L"csgo.exe", L"Overwatch.exe", L"r5apex.exe", L"TslGame.exe",
            L"GenshinImpact.exe", L"YuanShen.exe", L"StarRail.exe", L"ZenlessZoneZero.exe", L"eldenring.exe",
            L"gta5.exe", L"FortniteClient-Win64-Shipping.exe", L"b1-Win64-Shipping.exe" };
        for (int i = 0; i < (int)(sizeof games / sizeof games[0]); i++) if (!lstrcmpiW(exe, games[i])) { game = TRUE; break; }
    }
    if (!game) game = !lstrcmpW(cls, L"UnityWndClass") || !lstrcmpW(cls, L"UnrealWindow") ||
        !lstrcmpW(cls, L"Valve001") || !lstrcmpW(cls, L"GLFW30");
    return video || game ? 2 : (maximized ? 1 : 0);
}

static int SbBattery(int *chg) {
    SYSTEM_POWER_STATUS ps;
    if (!GetSystemPowerStatus(&ps) || ps.BatteryLifePercent == 255) return -1;
    if (chg) *chg = (ps.ACLineStatus == 1);
    return ps.BatteryLifePercent;
}

/* small icon cache for the notification apps shown in the status bar */
typedef struct { DWORD key; GpBitmap *bmp; } SbIcoEnt;
static SbIcoEnt g_sbIco[8];
static GpBitmap *SbNotifyIcon(const WCHAR *exe) {
    if (!exe || !exe[0]) return NULL;
    DWORD k = Fnv(exe);
    for (int i = 0; i < 8; i++) if (g_sbIco[i].bmp && g_sbIco[i].key == k) return g_sbIco[i].bmp;
    GpBitmap *b = ShellIconBitmap(exe, NULL, 48);
    if (!b) return NULL;
    for (int i = 0; i < 8; i++) if (!g_sbIco[i].bmp) { g_sbIco[i].key = k; g_sbIco[i].bmp = b; return b; }
    GdipDisposeImage(g_sbIco[0].bmp);
    g_sbIco[0].key = k; g_sbIco[0].bmp = b;
    return b;
}

static int SbNetState(void) {   /* 0 wifi, 1 wired, 2 offline */
    IP_ADAPTER_INFO buf[16];
    ULONG sz = sizeof buf;
    if (GetAdaptersInfo(buf, &sz) != ERROR_SUCCESS) return 2;
    BOOL w = FALSE, e = FALSE;
    for (IP_ADAPTER_INFO *a = buf; a; a = a->Next) {
        if (a->AddressLength == 0) continue;
        if (a->Type == 71) w = TRUE; else if (a->Type == 6) e = TRUE;
    }
    return w ? 0 : (e ? 1 : 2);
}

static const WCHAR *ImeLabel(void) {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hwnd || fg == g_sb) fg = GetShellWindow();
    DWORD tid = GetWindowThreadProcessId(fg, NULL);
    GUITHREADINFO gi; ZeroMemory(&gi, sizeof gi); gi.cbSize = sizeof gi;
    if (tid && GetGUIThreadInfo(tid, &gi) && gi.hwndFocus) {
        fg = gi.hwndFocus;
        tid = GetWindowThreadProcessId(fg, NULL); /* the focused child can belong to a different thread */
    }
    HKL kl = GetKeyboardLayout(tid);
    if (PRIMARYLANGID(LOWORD((ULONG_PTR)kl)) != LANG_CHINESE) return L"EN";
    /* HIMC access is thread-bound. Ask the target's default IME window to read its own
       state instead of treating a failed cross-thread ImmGetContext as English. */
    HWND ime = fg ? ImmGetDefaultIMEWnd(fg) : NULL;
    DWORD_PTR open = 0, conversion = 0;
    UINT flags = SMTO_ABORTIFHUNG | SMTO_BLOCK | SMTO_ERRORONEXIT;
    if (ime && SendMessageTimeoutW(ime, WM_IME_CONTROL, 0x0005 /* IMC_GETOPENSTATUS */, 0, flags, 20, &open)) {
        if (!open) return L"EN";
        if (SendMessageTimeoutW(ime, WM_IME_CONTROL, 0x0001 /* IMC_GETCONVERSIONMODE */, 0, flags, 20, &conversion))
            return (conversion & IME_CMODE_NATIVE) ? L"中" : L"EN";
    }
    /* Keep an IMM fallback only for a context owned by our own UI thread. */
    if (fg && tid == GetCurrentThreadId()) {
        HIMC imc = ImmGetContext(fg);
        if (imc) {
            DWORD conv = 0, sent = 0;
            BOOL known = ImmGetConversionStatus(imc, &conv, &sent);
            BOOL native = ImmGetOpenStatus(imc) && (conv & IME_CMODE_NATIVE);
            ImmReleaseContext(fg, imc);
            if (known) return native ? L"中" : L"EN";
        }
    }
    return L"IME"; /* unavailable is not evidence of English mode */
}
static void ToggleIme(void) {
    HWND fg = GetForegroundWindow();
    if (!fg || fg == g_hwnd || fg == g_sb) fg = g_hwnd;
    HKL list[16];
    int n = GetKeyboardLayoutList(16, list);
    HKL cur = GetKeyboardLayout(GetWindowThreadProcessId(fg, NULL));
    HKL next = NULL;
    int curLang = PRIMARYLANGID(LOWORD((DWORD)(ULONG_PTR)cur));
    for (int i = 0; i < n; i++) {
        int lang = PRIMARYLANGID(LOWORD((DWORD)(ULONG_PTR)list[i]));
        if (lang != curLang && (lang == LANG_ENGLISH || lang == LANG_CHINESE)) { next = list[i]; break; }
    }
    if (next) PostMessageW(fg, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)next);
    else {   /* only one layout installed: fall back to the standard conversion toggle */
        keybd_event(VK_CONTROL, 0, 0, 0);
        keybd_event(VK_SPACE, 0, 0, 0);
        keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
    }
    if (g_sb) { g_sbHover = -1; SbDraw(g_sb); SetTimer(g_sb, 9, 500, NULL); }   /* refresh once the app switched */
}

static void ShowSystemMenu(void) {
    CmItem it[16];
    int n = 0;
    n = CmAddG(it, n, 201, L"关于本机", 0, 1);
    n = CmAdd(it, n, 0, L"", CM_SEP);
    n = CmAddG(it, n, 202, L"系统设置", 0, 2);
    n = CmAddG(it, n, 203, L"Dock 设置", 0, 3);
    if (cfg.mnTaskMgr) n = CmAddG(it, n, 204, L"任务管理器", 0, 4);
    n = CmAdd(it, n, 0, L"", CM_SEP);
    n = CmAddG(it, n, 205, L"休眠", 0, 10);
    n = CmAddG(it, n, 206, L"睡眠", 0, 5);
    n = CmAddG(it, n, 207, L"重新启动", 0, 6);
    n = CmAddG(it, n, 208, L"关机", CM_RED, 7);
    n = CmAdd(it, n, 0, L"", CM_SEP);
    n = CmAddG(it, n, 209, L"锁定屏幕", 0, 8);
    n = CmAddG(it, n, 210, L"注销", 0, 9);
    POINT p;
    GetCursorPos(&p);
    gMenuOpen = TRUE;
    int r = ShowMenu(it, n, p.x, p.y);
    gMenuOpen = FALSE;
    switch (r) {
    case 201: OpenSettingsPage(L"ms-settings:about"); break;
    case 202: OpenSettingsPage(L"ms-settings:"); break;
    case 203: ShowSettings(0); break;
    case 204: ShellExecuteW(NULL, L"open", L"taskmgr.exe", NULL, NULL, SW_SHOWNORMAL); break;
    case 205: SetSuspendState(TRUE, FALSE, FALSE); break;
    case 206: SetSuspendState(FALSE, FALSE, FALSE); break;
    case 207: DoPowerAction(1); break;
    case 208: DoPowerAction(0); break;
    case 209: LockWorkStation(); break;
    case 210: ExitWindowsEx(EWX_LOGOFF, 0); break;
    }
}

static void ShowWifiPopup(void) { OpenQuickSettings(); }
static void ShowBtPopup(void) { OpenSettingsPage(L"ms-settings:bluetooth"); }
static void ShowVolPopup(void) { OpenQuickSettings(); }

static void SbDraw(HWND h) {
    if (!g_sbCv.g) return;
    GpGraphics *g = g_sbCv.g;
    int W = g_sbCv.w, H = g_sbCv.h2;
    GdipGraphicsClear(g, 0);
    BOOL dk = cfg.sbFollowSys ? SystemDark() : g_dark;
    ARGB tc = dk ? ARGBC(255, 240, 240, 245) : ARGBC(255, 28, 28, 34);
    ARGB sub = dk ? ARGBC(220, 210, 212, 222) : ARGBC(220, 90, 94, 108);
    int a = cfg.sbOpacity * 255 / 100;
    BOOL maxUnder = FALSE;
    if (cfg.sbAutoOpaque) {
        HWND fg2 = GetForegroundWindow();
        if (fg2 && fg2 != g_sb && fg2 != g_hwnd) {
            WINDOWPLACEMENT wp2 = { sizeof wp2 };
            if (GetWindowPlacement(fg2, &wp2) && wp2.showCmd == SW_SHOWMAXIMIZED) maxUnder = TRUE;
        }
    }
    if (maxUnder) a = 255;             /* a maximised window reaches the top: solid/blurred bar instead of a transparent one */
    if (a < 1) a = 1;   /* retain a barely visible hit-test pixel; visually this is effectively transparent */
    float rad = (float)cfg.sbRadius;
    ARGB bgc;
    if (cfg.sbColor >= 0) bgc = cfg.sbFrost ? ARGBC(a, GetRValue(cfg.sbColor), GetGValue(cfg.sbColor), GetBValue(cfg.sbColor)) : ARGBC(255, GetRValue(cfg.sbColor), GetGValue(cfg.sbColor), GetBValue(cfg.sbColor));
    else bgc = cfg.sbFrost ? (dk ? ARGBC(a, 30, 32, 40) : ARGBC(a, 240, 242, 248)) : (dk ? ARGBC(255, 30, 32, 40) : ARGBC(255, 240, 242, 248));
    FillRR(g, bgc, 0, 0, (float)W, (float)H, rad);   /* full width: flush with the screen edges */
    if (cfg.sbBorder && rad > 0) StrokeRR(g, dk ? ARGBC(50, 255, 255, 255) : ARGBC(40, 0, 0, 0), 1, 0, 0, (float)W, (float)H, rad);

    /* LEFT: only "MiniDock" settings, flush with the top-left corner */
    g_sbR[0].left = 0; g_sbR[0].top = 0; g_sbR[0].right = Sc(120); g_sbR[0].bottom = H;
    DrawTextBox(g, L"MiniDock", g_fontBold, Sc(16), 0, Sc(104), (float)H, tc, TRUE);

    /* RIGHT (right to left): notification app icons, bell, date + time, battery, volume, wifi, bluetooth,
       CPU, tray. Every item is centred on the same line, and the control centre has no extra button: the
       Wi-Fi / Bluetooth / volume / battery items open it directly. */
    /* every item is laid out on exactly the same line: text is centred in the full-height box, so the icons
       use the same geometric centre (no +0.5 px offset and no per-icon baseline drift) */
    float gap = (float)Sc(9), ico = (float)Sc(17), x = (float)W - Sc(12);
    float cyc = (float)H / 2.f;
    float iyc = cyc - (float)Sc(1);   /* the drawn line icons need a 1 px optical lift to sit level with the text */
    SYSTEMTIME st;
    GetLocalTime(&st);
    static const WCHAR *wk[7] = { L"日", L"一", L"二", L"三", L"四", L"五", L"六" };
    WCHAR d[48], t[16];
    wsprintfW(d, L"%d月%d日 周%s", st.wMonth, st.wDay, wk[st.wDayOfWeek]);
    wsprintfW(t, L"%02d:%02d", st.wHour, st.wMinute);
    if (g_sbHover >= 0 && g_sbHover < 12 && !IsRectEmpty(&g_sbR[g_sbHover]))
        FillRR(g, dk ? ARGBC(46, 255, 255, 255) : ARGBC(26, 0, 0, 0), (float)g_sbR[g_sbHover].left, 3.f,
               (float)(g_sbR[g_sbHover].right - g_sbR[g_sbHover].left), (float)H - 6.f, 8.f);
    for (int i = 1; i < 12; i++) SetRectEmpty(&g_sbR[i]);   /* keep [0]: the MiniDock button */
#define SB_RECT(i, l, r) { g_sbR[i].left = (int)(l) - Sc(3); g_sbR[i].right = (int)(r) + Sc(3); g_sbR[i].top = 0; g_sbR[i].bottom = H; }

    if (g_nnf > 0 && cfg.notifyW) {   /* the apps that ask for attention: their own icon, softly blinking */
        int nshow = g_nnf < 4 ? g_nnf : 4;
        float isz = ico + Sc(2);
        float x0 = x;
        for (int k = 0; k < nshow; k++) {
            NotifyEnt *e = &g_nf[g_nnf - 1 - k];
            x -= isz;
            float by = iyc - isz / 2;
            BOOL on = ((GetTickCount() / 480) & 1) != 0;
            if (on) FillRR(g, ARGBC(dk ? 60 : 44, 255, 255, 255), x - 2, by - 2, isz + 4, isz + 4, (isz + 4) * 0.3f);
            GpBitmap *ib = SbNotifyIcon(e->exe);
            if (ib) DrawImg(g, ib, x, by, isz, isz, FALSE);
            else FillRR(g, ARGBC(255, 255, 59, 48), x + isz * 0.25f, by + isz * 0.25f, isz * 0.5f, isz * 0.5f, isz * 0.25f);
            if (k < nshow - 1) x -= Sc(5);
        }
        SB_RECT(6, x, x0);
        x -= gap;
    } else if (cfg.notifyW) {   /* notification bell */
        x -= ico;
        MIcoBell(g, x + ico / 2, iyc, ico, tc);
        SB_RECT(6, x, x + ico);
        x -= gap;
    }
    if (cfg.clock) {
        float tw = TextWidth(g, t, g_fontBold) + 2, dw = TextWidth(g, d, g_font) + 2;
        x -= tw; DrawTextBox(g, t, g_fontBold, x, 0, tw, (float)H, tc, TRUE);
        float tx0 = x;
        x -= Sc(6) + dw; DrawTextBox(g, d, g_font, x, 0, dw, (float)H, tc, TRUE);
        SB_RECT(1, x, tx0 + tw);
        x -= gap;
    }
    int chg = 0, bp = cfg.sbBattery ? SbBattery(&chg) : -1;
    if (bp >= 0) {   /* battery % + icon */
        WCHAR btxt[8];
        wsprintfW(btxt, L"%d%%", bp);
        float pw = TextWidth(g, btxt, g_font) + 2, bbw = (float)Sc(25), bbh = (float)Sc(12);
        x -= bbw; MIcoBattery(g, x, iyc - bbh / 2, bbw, bbh, bp, chg != 0, tc);
        x -= Sc(5) + pw; DrawTextBox(g, btxt, g_font, x, 0, pw, (float)H, tc, TRUE);
        SB_RECT(5, x, x + pw + Sc(5) + bbw);
        x -= gap;
    }
    if (cfg.ctrlCenter) {
        x -= ico;
        if (CcMuteGet()) MIcoVolMute(g, x + ico / 2, iyc, ico, tc); else MIcoVol(g, x + ico / 2, iyc, ico, tc);
        SB_RECT(8, x, x + ico);
        x -= gap;
        int net = SbNetState();
        x -= ico;
        if (net == 1) CcGlyph(g, 8, x, iyc - ico / 2, ico, tc); else MIcoWifi(g, x + ico / 2, iyc, ico, tc);
        if (net == 2) { GpPen *np = MiPen(tc, ico * 0.10f); GdipDrawLine(g, np, x + ico * 0.12f, iyc - ico * 0.40f, x + ico * 0.88f, iyc + ico * 0.46f); GdipDeletePen(np); }
        SB_RECT(7, x, x + ico);
        x -= gap;
        x -= ico; MIcoBt(g, x + ico / 2, iyc, ico, tc);
        SB_RECT(9, x, x + ico);
        x -= gap;
    }
    /* IME indicator: 中 / EN, click toggles the input method */
    { WCHAR im[4]; lstrcpynW(im, ImeLabel(), 4); float iw = TextWidth(g, im, g_fontBold) + 4;
      x -= iw; DrawTextBox(g, im, g_fontBold, x, 0, iw, (float)H, tc, TRUE);
      SB_RECT(10, x, x + iw); x -= gap; }
    x -= ico; { GpPen *cp = MiPen(tc, ico * 0.10f); float q = ico * 0.34f;
      GdipDrawRectangle(g, cp, x + ico * 0.12f, iyc - ico * 0.38f, q, q);
      GdipDrawRectangle(g, cp, x + ico * 0.54f, iyc - ico * 0.38f, q, q);
      GdipDrawRectangle(g, cp, x + ico * 0.12f, iyc + ico * 0.04f, q, q);
      GdipDrawRectangle(g, cp, x + ico * 0.54f, iyc + ico * 0.04f, q, q);
      GdipDeletePen(cp); }                                                 /* separate control-centre button */
    SB_RECT(2, x, x + ico);
    x -= gap;
    if (cfg.resMon) {
        WCHAR cpu[16];
        wsprintfW(cpu, L"CPU %d%%", ResCpuPercent());
        float cw = TextWidth(g, cpu, g_font) + 2;
        x -= cw; DrawTextBox(g, cpu, g_font, x, 0, cw, (float)H, sub, TRUE);
        SB_RECT(4, x, x + cw);
        x -= gap;
    }
    if (cfg.modeW) { x -= ico; DrawMode(g, x, iyc - ico / 2, ico, ico); SB_RECT(11, x, x + ico); x -= gap; }
    if (cfg.trayW) { x -= ico; MIcoChev(g, x + ico / 2, iyc, ico, tc); SB_RECT(3, x, x + ico); }
#undef SB_RECT

    if (g_nnf > 0) SetTimer(h, 2, 480, NULL); else KillTimer(h, 2);   /* let the notification icons blink */
    RECT position;
    if (GetWindowRect(h, &position)) CanvasPresentA(&g_sbCv, h, position.left, position.top, 255);
}

static void SbAnchor(int wid, int cx0, int cy0) {
    RECT r;
    GetWindowRect(g_sb, &r);
    g_wScr[wid].left = r.left + cx0 - Sc(8); g_wScr[wid].top = r.top + cy0 - Sc(8);
    g_wScr[wid].right = r.left + cx0 + Sc(8); g_wScr[wid].bottom = r.top + cy0 + Sc(8);
}

static LRESULT CALLBACK SbProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
    case WM_LBUTTONUP: {
        int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
        POINT p = { x, y };
        if (PtInRect(&g_sbR[0], p)) ShowSystemMenu();
        else if (PtInRect(&g_sbR[1], p)) { SbAnchor(W_CLOCK, (g_sbR[1].left + g_sbR[1].right) / 2, Sc(18)); ShowCalendar(); }
        else if (PtInRect(&g_sbR[3], p)) TrayOpen();
        else if (PtInRect(&g_sbR[4], p)) { SbAnchor(W_RES, (g_sbR[4].left + g_sbR[4].right) / 2, Sc(18)); ShowResMon(); }
        else if (PtInRect(&g_sbR[6], p)) { SbAnchor(W_NOTIFY, (g_sbR[6].left + g_sbR[6].right) / 2, Sc(18)); ShowNotifyList(); }
        else if (PtInRect(&g_sbR[7], p)) { if (CcEthUp()) OpenCplApplet(L"ncpa.cpl", NULL); else { SbAnchor(W_CC, (g_sbR[7].left + g_sbR[7].right) / 2, Sc(18)); ShowWifiPopup(); } }
        else if (PtInRect(&g_sbR[8], p)) { SbAnchor(W_CC, (g_sbR[8].left + g_sbR[8].right) / 2, Sc(18)); ShowVolPopup(); }
        else if (PtInRect(&g_sbR[9], p)) { SbAnchor(W_CC, (g_sbR[9].left + g_sbR[9].right) / 2, Sc(18)); ShowBtPopup(); }
        else if (PtInRect(&g_sbR[10], p)) ToggleIme();
        else if (PtInRect(&g_sbR[11], p)) ShowModeMenu();
        else if (PtInRect(&g_sbR[2], p) || PtInRect(&g_sbR[5], p)) { SbAnchor(W_CC, (g_sbR[2].left + g_sbR[2].right) / 2, Sc(18)); ShowCtrlCenter(); }
        return 0;
    }
    case WM_RBUTTONUP: {   /* context menu for the exact top-bar widget under the pointer */
        int hit = -1;
        POINT cp = { GET_X_LPARAM(l), GET_Y_LPARAM(l) };
        for (int i = 1; i < 12; i++) if (!IsRectEmpty(&g_sbR[i]) && PtInRect(&g_sbR[i], cp)) { hit = i; break; }
        CmItem it[8]; int n = 0;
        if (hit == 10) {
            n = CmAdd(it, n, 10, L"系统输入法设置…", 0);
            n = CmAdd(it, n, 1, L"状态栏设置…", 0);
        } else {
            BOOL closable = (hit == 1 || hit == 2 || hit == 3 || hit == 4 || hit == 5 || hit == 6 || hit == 7 || hit == 8 || hit == 9 || hit == 11);
            if (closable) { n = CmAdd(it, n, 20 + hit, L"关闭此小组件", CM_RED); n = CmAdd(it, n, 0, L"", CM_SEP); }
            n = CmAdd(it, n, 1, L"状态栏设置…", 0);
            n = CmAdd(it, n, 2, cfg.sbFullscreenHide ? L"全屏视频/游戏时隐藏（已开启）" : L"全屏视频/游戏时隐藏", 0);
            n = CmAdd(it, n, 3, L"关闭状态栏", CM_RED);
            if (cfg.cmMerge) n = CmAdd(it, n, 4, cfg.multiMode ? L"窗口独立显示" : L"窗口合并显示", cfg.multiMode ? CM_CHECK : 0);
            if (cfg.mnTaskMgr) n = CmAdd(it, n, 5, L"任务管理器", 0);
        }
        POINT p; GetCursorPos(&p); gMenuOpen = TRUE;
        int r = ShowMenu(it, n, p.x, p.y); gMenuOpen = FALSE;
        if (r == 10) ShellExecuteW(NULL, L"open", L"ms-settings:regionlanguage", NULL, NULL, SW_SHOWNORMAL);
        else if (r == 1) ShowSettings(30);
        else if (r == 2) { cfg.sbFullscreenHide = !cfg.sbFullscreenHide; ScheduleSave(); SbTick(); }
        else if (r == 3) { cfg.statusBar = 0; ApplySettings(); ScheduleSave(); SbTick(); }
        else if (r == 4) { cfg.multiMode = !cfg.multiMode; ApplySettings(); ScheduleSave(); }
        else if (r == 5) ShellExecuteW(NULL, L"open", L"taskmgr.exe", NULL, NULL, SW_SHOWNORMAL);
        else if (r >= 21 && r <= 31) {
            int hidx = r - 20;
            if (hidx == 1) cfg.clock = 0;
            else if (hidx == 2 || hidx == 7 || hidx == 8 || hidx == 9) cfg.ctrlCenter = 0;
            else if (hidx == 3) cfg.trayW = 0;
            else if (hidx == 4) cfg.resMon = 0;
            else if (hidx == 5) cfg.sbBattery = 0;
            else if (hidx == 6) cfg.notifyW = 0;
            else if (hidx == 11) cfg.modeW = 0;
            ApplySettings(); ScheduleSave(); SbTick();
        }
        return 0;
    }
    case WM_MOUSEMOVE: {
        int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
        POINT p = { x, y };
        int hv = -1;
        for (int i = 0; i < 12; i++) if (!IsRectEmpty(&g_sbR[i]) && PtInRect(&g_sbR[i], p)) hv = i;
        if (hv != g_sbHover) { g_sbHover = hv; SbDraw(h); }
        TRACKMOUSEEVENT te;
        ZeroMemory(&te, sizeof te);
        te.cbSize = sizeof te; te.dwFlags = TME_LEAVE; te.hwndTrack = h;
        TrackMouseEvent(&te);
        return 0;
    }
    case WM_MOUSELEAVE: if (g_sbHover != -1) { g_sbHover = -1; SbDraw(h); } return 0;
    case WM_TIMER:
        if (w == 9) KillTimer(h, 9);
        if (!g_sbContentSuppressed && IsWindowVisible(h)) SbDraw(h);
        return 0;
    case WM_APP_AB:
        /* Reconcile changed Shell reservations on the existing tick. SbAppBar caches the
           accepted rectangle so its own SETPOS notification cannot create a feedback loop. */
        if (w == ABN_POSCHANGED) g_sbAbDirty = TRUE;
        if (w == ABN_FULLSCREENAPP) SbTick();
        return 0;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    }
    return DefWindowProcW(h, m, w, l);
}

static void SbShow(void) {
    if (g_sb) return;
    static BOOL reg;
    if (!reg) {
        WNDCLASSW wc;
        ZeroMemory(&wc, sizeof wc);
        wc.lpfnWndProc = SbProc; wc.hInstance = g_inst; wc.lpszClassName = CLS_SB; wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);
        reg = TRUE;
    }
    int X, Y, W, H;
    SbGeom(&X, &Y, &W, &H);
    if (!CanvasNew(&g_sbCv, W, H)) return;
    SetupG(g_sbCv.g);
    g_sb = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CLS_SB, L"", WS_POPUP, X, Y, W, H, NULL, NULL, g_inst, NULL);
    if (!g_sb) { CanvasFree(&g_sbCv); return; }
    SbDraw(g_sb);
    SetTimer(g_sb, 1, 300, NULL);    /* poll IME conversion changes (including Shift) with a short, low-cost refresh */
}

static void SbHide(void) {
    if (!g_sb) return;
    SbAppBar(FALSE);
    KillTimer(g_sb, 1);
    DestroyWindow(g_sb);
    g_sb = NULL;
    CanvasFree(&g_sbCv);
}

static BOOL g_sbAbReg;
static RECT g_sbAbRect;
static BOOL SbHiddenTopTaskbar(void) {
    if (cfg.taskHide != 2) return FALSE;
    APPBARDATA taskbar = { sizeof taskbar };
    RECT intersection;
    return SHAppBarMessage(ABM_GETTASKBARPOS, &taskbar) && taskbar.uEdge == ABE_TOP &&
        IntersectRect(&intersection, &taskbar.rc, &g_mi.rcMonitor);
}
static void SbAppBar(BOOL on) {
    if (!g_sb) return;
    if (g_sbContentSuppressed) on = FALSE;
    APPBARDATA ab;
    ZeroMemory(&ab, sizeof ab);
    ab.cbSize = sizeof ab;
    ab.hWnd = g_sb;
    g_sbAbDirty = FALSE;
    if (!on) {
        if (g_sbAbReg) { SHAppBarMessage(ABM_REMOVE, &ab); g_sbAbReg = FALSE; }
        return;
    }
    BOOL fresh = !g_sbAbReg;
    if (fresh) {
        ab.uCallbackMessage = WM_APP_AB;
        if (!SHAppBarMessage(ABM_NEW, &ab)) return;   /* retried from SbTick */
        g_sbAbReg = TRUE;
    }
    int H = Sc(cfg.sbHeight), thick = cfg.sbTopGap + H;
    RECT m = g_mi.rcMonitor;
    ab.uEdge = ABE_TOP;
    ab.rc = m;
    ab.rc.bottom = ab.rc.top + thick;
    if (!SHAppBarMessage(ABM_QUERYPOS, &ab)) { SbAppBar(FALSE); return; }
    BOOL hiddenTop = SbHiddenTopTaskbar();
    if (hiddenTop) {
        /* The hidden taskbar may still retain its auto-hide reveal strip. Together the
           reveal strip and our AppBar must end at the drawn status bar's bottom edge. */
        ab.rc.bottom = m.top + thick;
        if (ab.rc.bottom <= ab.rc.top) { SbAppBar(FALSE); return; }
    } else ab.rc.bottom = ab.rc.top + thick;
    if (!fresh && EqualRect(&ab.rc, &g_sbAbRect)) return;
    if (!SHAppBarMessage(ABM_SETPOS, &ab)) { SbAppBar(FALSE); return; }
    g_sbAbRect = ab.rc;
    int side = cfg.sbFloat ? Sc(cfg.sbSideGap) : 0;
    int W = (ab.rc.right - ab.rc.left) - 2 * side;
    int Y = (hiddenTop ? m.top : ab.rc.top) + cfg.sbTopGap;
    SetWindowPos(g_sb, NULL, ab.rc.left + side, Y, W, H, SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static int g_sbX0 = -99999, g_sbY0 = -99999;
static void SbTick(void) {
    if (!cfg.statusBar) {
        if (g_sb) { SbAppBar(FALSE); SbHide(); }
        g_sbContentSuppressed = FALSE;
        return;
    }
    int mode = SbForegroundMode();
    if (cfg.sbFullscreenHide && mode == 2) {
        g_sbContentSuppressed = TRUE;
        if (g_sb) {
            ShowWindow(g_sb, SW_HIDE);
            KillTimer(g_sb, 1);
            KillTimer(g_sb, 2);
            SbAppBar(FALSE); /* release the top work-area strip as well as hiding the pixels */
        }
        return;
    }
    BOOL restoring = g_sbContentSuppressed;
    g_sbContentSuppressed = FALSE;
    int X, Y, W, H;
    SbGeom(&X, &Y, &W, &H);
    if (!g_sb) { SbShow(); g_sbX0 = -99999; g_sbY0 = -99999; }
    if (!g_sb) return;
    if (g_sbCv.w != W || g_sbCv.h2 != H) { SbHide(); SbShow(); g_sbX0 = -99999; g_sbY0 = -99999; }
    if (!g_sb) return;
    BOOL geoChanged = (X != g_sbX0 || Y != g_sbY0);
    BOOL reserve = cfg.sbMaxAvoid;
    if (g_sbAbReg != reserve || (reserve && (geoChanged || g_sbAbDirty))) SbAppBar(reserve);
    RECT actual;
    if (!GetWindowRect(g_sb, &actual) || actual.left != X || actual.top != Y)
        SetWindowPos(g_sb, NULL, X, Y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    if (!IsWindowVisible(g_sb)) ShowWindow(g_sb, SW_SHOWNOACTIVATE);
    if (restoring) { SetTimer(g_sb, 1, 300, NULL); SbDraw(g_sb); }
    g_sbX0 = X; g_sbY0 = Y;
}

/* ======================= launchpad: paged app grid (swipe, drag to reorder, merge into folders, custom items) ======================= */
#define CLS_LP L"MiniDockLaunchpad"
#define LP_MAXA 700
#define LP_MAXF 60
#define LP_MAXT 760
#define LP_MAXC 24
typedef struct { WCHAR name[80]; WCHAR path[MAX_PATH]; GpBitmap *icon; } LpApp;
typedef struct { WCHAR name[48]; int n; int app[LP_MAXC]; } LpFold;
typedef struct { int fold, idx; } LpTop;   /* fold 0: idx = app, fold 1: idx = folder */
typedef struct {
    Canvas cv;
    LpApp *a; int na;
    LpFold *f; int nf;
    LpTop *top; int nt;
    WCHAR (*hid)[MAX_PATH]; int nh;
    int done, ready, W, H, alpha, inMenu;
    int cols, rows, per, page, hover, nv, openFold, fhover;
    float off, cw, ch, is, gx, gy;
    WCHAR q[48];
    int vis[LP_MAXA];
    int press, pressFold, dragging, swiping, dragSrc;
    POINT down, cur;
    float offStart;
    DWORD t0, edgeT;
    HWND bk;
    RECT scr, panel;
    FrostSnap frost;
    int cat;              /* 0 all, 1 office, 2 system, 3 tools */
    RECT catR[9], addR;
    WCHAR tagName[8][32]; int ntag;
    DWORD lastDraw;       /* redraw throttle: the full-screen canvas is expensive on Windows 10 */
    WCHAR pressPath[MAX_PATH];   /* the entry that was pressed, so a re-order cannot launch the wrong app */
    int pressOk;
    /* two pre-rendered pages: a swipe / page slide only blits them instead of laying out 30 icons and
       30 labels again on every frame (the main reason the launchpad stuttered on Windows 10) */
    GpBitmap *cache[2];
    int cachePage[2];
    DWORD cacheSig;
} LpCtx;

static void LpPath(WCHAR *out) { PathCombineW(out, g_dir, L"launchpad.dat"); }

static int LpFindApp(LpCtx *c, const WCHAR *path, const WCHAR *name) {
    for (int i = 0; i < c->na; i++) if (!lstrcmpiW(c->a[i].path, path) || (name && !lstrcmpiW(c->a[i].name, name))) return i;
    return -1;
}
static int LpAddApp(LpCtx *c, const WCHAR *path, const WCHAR *name) {
    int i = LpFindApp(c, path, name);
    if (i >= 0) return i;
    if (c->na >= LP_MAXA) return -1;
    LpApp *a = &c->a[c->na];
    ZeroMemory(a, sizeof *a);
    lstrcpynW(a->path, path, MAX_PATH);
    lstrcpynW(a->name, name, 80);
    return c->na++;
}
static BOOL LpHidden(LpCtx *c, const WCHAR *path) { for (int i = 0; i < c->nh; i++) if (!lstrcmpiW(c->hid[i], path)) return TRUE; return FALSE; }

static int LpCmp(const void *a, const void *b) { return lstrcmpiW(((const LpApp *)a)->name, ((const LpApp *)b)->name); }

static void LpScan(LpCtx *c, LpApp *tmp, int *nt, const WCHAR *dir, int depth) {
    WCHAR pat[MAX_PATH];
    PathCombineW(pat, dir, L"*");
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pat, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.cFileName[0] == L'.') continue;
        WCHAR full[MAX_PATH];
        PathCombineW(full, dir, fd.cFileName);
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { if (depth < 4) LpScan(c, tmp, nt, full, depth + 1); continue; }
        if (!PathMatchSpecW(fd.cFileName, L"*.lnk") || *nt >= LP_MAXA) continue;
        WCHAR nm[80];
        lstrcpynW(nm, fd.cFileName, 80);
        PathRemoveExtensionW(nm);
        if (StrStrIW(nm, L"uninstall") || StrStrIW(nm, L"卸载") || StrStrIW(nm, L"readme") || StrStrIW(nm, L"帮助") || StrStrIW(nm, L"help")) continue;
        lstrcpynW(tmp[*nt].name, nm, 80);
        lstrcpynW(tmp[*nt].path, full, MAX_PATH);
        (*nt)++;
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static void LpSave(LpCtx *c) {
    WCHAR f[MAX_PATH];
    LpPath(f);
    size_t cap = (size_t)(c->nt + c->nh + 8) * (MAX_PATH + 160) + (size_t)c->nf * LP_MAXC * (MAX_PATH + 160);
    WCHAR *b = (WCHAR *)HeapAlloc(GetProcessHeap(), 0, cap * sizeof(WCHAR));
    if (!b) return;
    int o = 0;
    for (int i = 0; i < c->nt; i++) {
        LpTop *t = &c->top[i];
        if (!t->fold) o += wsprintfW(b + o, L"T\t%s\t%s\n", c->a[t->idx].path, c->a[t->idx].name);
        else {
            LpFold *fo = &c->f[t->idx];
            o += wsprintfW(b + o, L"F\t%s\n", fo->name);
            for (int k = 0; k < fo->n; k++) o += wsprintfW(b + o, L"f\t%s\t%s\n", c->a[fo->app[k]].path, c->a[fo->app[k]].name);
            o += wsprintfW(b + o, L"E\n");
        }
    }
    for (int i = 0; i < c->nh; i++) o += wsprintfW(b + o, L"H\t%s\n", c->hid[i]);
    int bl = WideCharToMultiByte(CP_UTF8, 0, b, o, NULL, 0, NULL, NULL);
    char *u = (char *)HeapAlloc(GetProcessHeap(), 0, bl + 4);
    if (u) {
        WideCharToMultiByte(CP_UTF8, 0, b, o, u, bl, NULL, NULL);
        HANDLE h = CreateFileW(f, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(h, u, bl, &w, NULL); CloseHandle(h); }
        HeapFree(GetProcessHeap(), 0, u);
    }
    HeapFree(GetProcessHeap(), 0, b);
}

static void LpLoad(LpCtx *c) {
    WCHAR f[MAX_PATH];
    LpPath(f);
    BYTE *raw = NULL;
    DWORD n = 0;
    if (ReadWhole(f, &raw, &n) && raw) {
        raw[n] = 0;
        int wl = MultiByteToWideChar(CP_UTF8, 0, (char *)raw, (int)n, NULL, 0);
        WCHAR *w = (WCHAR *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, (wl + 4) * sizeof(WCHAR));
        if (w) {
            MultiByteToWideChar(CP_UTF8, 0, (char *)raw, (int)n, w, wl);
            int curF = -1;
            WCHAR *line = w;
            while (line && *line) {
                WCHAR *nl = wcschr(line, L'\n');
                if (nl) *nl = 0;
                int ll = lstrlenW(line);
                if (ll && line[ll - 1] == L'\r') line[ll - 1] = 0;
                WCHAR *p1 = wcschr(line, L'\t'), *p2 = p1 ? wcschr(p1 + 1, L'\t') : NULL;
                if (line[0] == L'T' && p1 && p2 && c->nt < LP_MAXT) {
                    *p2 = 0;
                    int ai = LpAddApp(c, p1 + 1, p2 + 1);
                    if (ai >= 0) { c->top[c->nt].fold = 0; c->top[c->nt].idx = ai; c->nt++; }
                } else if (line[0] == L'F' && p1 && c->nf < LP_MAXF && c->nt < LP_MAXT) {
                    curF = c->nf++;
                    ZeroMemory(&c->f[curF], sizeof(LpFold));
                    lstrcpynW(c->f[curF].name, p1 + 1, 48);
                    c->top[c->nt].fold = 1; c->top[c->nt].idx = curF; c->nt++;
                } else if (line[0] == L'f' && p1 && p2 && curF >= 0 && c->f[curF].n < LP_MAXC) {
                    *p2 = 0;
                    int ai = LpAddApp(c, p1 + 1, p2 + 1);
                    if (ai >= 0) c->f[curF].app[c->f[curF].n++] = ai;
                } else if (line[0] == L'E') curF = -1;
                else if (line[0] == L'H' && p1 && c->nh < 200) lstrcpynW(c->hid[c->nh++], p1 + 1, MAX_PATH);
                line = nl ? nl + 1 : NULL;
            }
            HeapFree(GetProcessHeap(), 0, w);
        }
        HeapFree(GetProcessHeap(), 0, raw);
    }
    /* Start-menu apps that are not placed yet are appended (sorted); removed ones stay removed */
    LpApp *tmp = (LpApp *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LpApp) * LP_MAXA);
    if (!tmp) return;
    int nt = 0;
    WCHAR d1[MAX_PATH], d2[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_COMMON_PROGRAMS, NULL, 0, d1))) LpScan(c, tmp, &nt, d1, 0);
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_PROGRAMS, NULL, 0, d2))) LpScan(c, tmp, &nt, d2, 0);
    qsort(tmp, nt, sizeof(LpApp), LpCmp);
    for (int i = 0; i < nt; i++) {
        if (i > 0 && !lstrcmpiW(tmp[i].name, tmp[i - 1].name)) continue;
        if (LpHidden(c, tmp[i].path)) continue;
        int ex = LpFindApp(c, tmp[i].path, tmp[i].name);
        if (ex >= 0) {   /* already known: is it placed on the grid or inside a folder? */
            BOOL placed = FALSE;
            for (int k = 0; k < c->nt && !placed; k++) if (!c->top[k].fold && c->top[k].idx == ex) placed = TRUE;
            for (int k = 0; k < c->nf && !placed; k++) for (int j = 0; j < c->f[k].n; j++) if (c->f[k].app[j] == ex) placed = TRUE;
            if (placed) continue;
        }
        int ai = ex >= 0 ? ex : LpAddApp(c, tmp[i].path, tmp[i].name);
        if (ai >= 0 && c->nt < LP_MAXT) { c->top[c->nt].fold = 0; c->top[c->nt].idx = ai; c->nt++; }
    }
    HeapFree(GetProcessHeap(), 0, tmp);
}

static GpBitmap *LpIcon(LpApp *a) {
    if (!a->icon) {
        if (PathMatchSpecW(a->path, L"*.lnk")) a->icon = LnkIconBitmap(a->path, 128);
        if (!a->icon && !wcsstr(a->path, L"://")) a->icon = ShellIconBitmap(a->path, NULL, 128);
        if (!a->icon) a->icon = LetterTile(a->name, 128);
    }
    return a->icon;
}

static void LpLayout(LpCtx *c) {
    c->cols = cfg.lpCols < 3 ? 3 : cfg.lpCols > 12 ? 12 : cfg.lpCols;
    c->rows = cfg.lpRows < 2 ? 2 : cfg.lpRows > 8 ? 8 : cfg.lpRows;
    c->per = c->cols * c->rows;
    float side = (float)Sc(70), topH = (float)Sc(124), botH = (float)Sc(146);   /* leave room for the page dots under the grid */
    c->cw = (c->W - 2 * side) / c->cols;
    c->ch = (c->H - topH - botH) / c->rows;
    c->is = fminf((float)Sc(88), fminf(c->cw * 0.56f, c->ch * 0.5f));
    c->gx = side; c->gy = topH;
}

static BOOL LpFiltered(LpCtx *c) { return c->q[0] || c->cat != 0; }

/* a quick-and-dirty category classifier: office / system / tools */
static int LpCat(const LpApp *a) {
    const WCHAR *p = a->path, *n = a->name;
    if (StrStrIW(p, L"\\Windows\\") || StrStrIW(p, L"System32") || StrStrIW(p, L"shell:") ||
        StrStrIW(n, L"开始") || StrStrIW(n, L"设置") || StrStrIW(n, L"回收站") || StrStrIW(n, L"此电脑") ||
        StrStrIW(n, L"控制面板") || StrStrIW(n, L"资源管理器")) return 2;
    if (StrStrIW(n, L"word") || StrStrIW(n, L"excel") || StrStrIW(n, L"powerpoint") || StrStrIW(n, L"ppt") ||
        StrStrIW(n, L"office") || StrStrIW(n, L"wps") || StrStrIW(n, L"onenote") || StrStrIW(n, L"outlook") ||
        StrStrIW(n, L"onedrive") || StrStrIW(n, L"notion")) return 1;
    return 3;
}

static int LpCatOf(LpCtx *c, const LpApp *a) {   /* a per-app override stored in the ini wins over the classifier */
    int v = GetPrivateProfileIntW(L"LpCat", a->name, -1, g_ini);
    if (v >= 1 && v <= c->ntag) return v;
    int av = LpCat(a);
    return av > c->ntag ? c->ntag : (av < 1 ? 1 : av);
}

static void LpSetCat(LpCtx *c, int ai, int cat) {
    if (cat < 0) WritePrivateProfileStringW(L"LpCat", c->a[ai].name, NULL, g_ini);
    else { WCHAR v[8]; wsprintfW(v, L"%d", cat); WritePrivateProfileStringW(L"LpCat", c->a[ai].name, v, g_ini); }
}

static void LpTagLoad(LpCtx *c) {
    c->ntag = GetPrivateProfileIntW(L"LpTags", L"N", 3, g_ini);
    if (c->ntag < 1) c->ntag = 1;
    if (c->ntag > 8) c->ntag = 8;
    for (int i = 0; i < c->ntag; i++) {
        WCHAR k[8];
        wsprintfW(k, L"T%d", i + 1);
        GetPrivateProfileStringW(L"LpTags", k, L"", c->tagName[i], 32, g_ini);
        if (!c->tagName[i][0]) lstrcpynW(c->tagName[i], i == 0 ? L"办公" : i == 1 ? L"系统" : i == 2 ? L"工具" : L"标签", 32);
    }
}

static void LpTagSave(LpCtx *c) {
    WCHAR v[8];
    wsprintfW(v, L"%d", c->ntag);
    WritePrivateProfileStringW(L"LpTags", L"N", v, g_ini);
    for (int i = 0; i < c->ntag; i++) {
        WCHAR k[8];
        wsprintfW(k, L"T%d", i + 1);
        WritePrivateProfileStringW(L"LpTags", k, c->tagName[i], g_ini);
    }
}

static void LpCatFilter(LpCtx *c) {
    c->nv = 0;
    for (int i = 0; i < c->na && c->nv < LP_MAXA; i++)
        if (!LpHidden(c, c->a[i].path) && LpCatOf(c, &c->a[i]) == c->cat) c->vis[c->nv++] = i;
}

static void LpFilter(LpCtx *c) {
    c->nv = 0;
    if (!c->q[0]) return;
    for (int i = 0; i < c->na && c->nv < LP_MAXA; i++) if (StrStrIW(c->a[i].name, c->q) && !LpHidden(c, c->a[i].path)) c->vis[c->nv++] = i;
}
static int LpCount(LpCtx *c) { return LpFiltered(c) ? c->nv : c->nt; }
static const WCHAR *LpTopName(LpCtx *c, const LpTop *t) { return t->fold ? c->f[t->idx].name : c->a[t->idx].name; }
static void LpSortAZ(LpCtx *c) {   /* reorder the launchpad alphabetically (saved: drag again to change) */
    for (int i = 1; i < c->nt; i++) {
        LpTop t = c->top[i];
        int j = i - 1;
        while (j >= 0 && lstrcmpiW(LpTopName(c, &c->top[j]), LpTopName(c, &t)) > 0) { c->top[j + 1] = c->top[j]; j--; }
        c->top[j + 1] = t;
    }
}
static int LpPages(LpCtx *c) { int n = (LpCount(c) + c->per - 1) / c->per; return n < 1 ? 1 : n; }

static void LpDrawFolderIcon(GpGraphics *g, LpCtx *c, LpFold *fo, float x, float y, float s) {
    FillRR(g, g_dark ? ARGBC(120, 255, 255, 255) : ARGBC(190, 255, 255, 255), x, y, s, s, s * 0.24f);
    StrokeRR(g, g_dark ? ARGBC(70, 255, 255, 255) : ARGBC(50, 0, 0, 0), 1, x, y, s, s, s * 0.24f);
    float cell = s * 0.26f, gap = s * 0.05f, o = (s - 3 * cell - 2 * gap) / 2;
    for (int k = 0; k < fo->n && k < 9; k++) {
        GpBitmap *ic = LpIcon(&c->a[fo->app[k]]);
        if (ic) DrawImg(g, ic, x + o + (k % 3) * (cell + gap), y + o + (k / 3) * (cell + gap), cell, cell, FALSE);
    }
}

/* draw top-level entry (or search hit) number k at screen position (x, y) of its cell */
static void LpDrawCell(LpCtx *c, GpGraphics *g, int k, float x, float y, ARGB tc, BOOL ghost) {
    float ix = x + (c->cw - c->is) / 2, iy = y + (c->ch - c->is - Sc(34)) / 2;
    const WCHAR *name;
    if (LpFiltered(c)) {
        LpApp *a = &c->a[c->vis[k]];
        GpBitmap *ic = LpIcon(a);
        if (ic) DrawIconPlated(g, ic, ix, iy, c->is, (float)cfg.plateRadius);
        name = a->name;
    } else {
        LpTop *t = &c->top[k];
        if (t->fold) { LpDrawFolderIcon(g, c, &c->f[t->idx], ix, iy, c->is); name = c->f[t->idx].name; }
        else { LpApp *a = &c->a[t->idx]; GpBitmap *ic = LpIcon(a); if (ic) DrawIconPlated(g, ic, ix, iy, c->is, (float)cfg.plateRadius); name = a->name; }
    }
    DrawTextBox(g, name, g_font, x + 4, iy + c->is + 6, c->cw - 8, (float)Sc(36), ghost ? ARGBC(110, 200, 200, 200) : tc, FALSE);
}

static DWORD LpCacheSig(LpCtx *c) {
    DWORD h = 2166136261u;
    int v[11] = { c->nt, c->nv, c->cat, c->cols, c->rows, (int)c->is, (int)c->cw, (int)c->ch,
                  g_dark, cfg.plateMode, cfg.plateRadius };
    for (int i = 0; i < 11; i++) { h ^= (DWORD)v[i]; h *= 16777619u; }
    h ^= Fnv(c->q); h *= 16777619u;
    for (int i = 0; i < c->nt; i++) { h ^= (DWORD)(c->top[i].fold * 100003 + c->top[i].idx); h *= 16777619u; }
    for (int i = 0; i < c->nv; i++) { h ^= (DWORD)c->vis[i] + 7u; h *= 16777619u; }
    for (int i = 0; i < c->nf; i++) { h ^= (DWORD)c->f[i].n + 13u; h *= 16777619u; }
    return h;
}
static void LpCacheFree(LpCtx *c) {
    for (int i = 0; i < 2; i++) {
        if (c->cache[i]) { GdipDisposeImage(c->cache[i]); c->cache[i] = NULL; }
        c->cachePage[i] = -1;
    }
    c->cacheSig = 0;
}
static GpBitmap *LpPageImage(LpCtx *c, int page) {
    DWORD sig = LpCacheSig(c);
    if (!c->cacheSig || sig != c->cacheSig) LpCacheFree(c);
    c->cacheSig = sig;
    for (int i = 0; i < 2; i++) if (c->cache[i] && c->cachePage[i] == page) return c->cache[i];
    int slot = c->cache[0] ? (c->cache[1] ? (c->cachePage[0] == c->page ? 1 : 0) : 1) : 0;
    if (c->cache[slot]) { GdipDisposeImage(c->cache[slot]); c->cache[slot] = NULL; }
    GpBitmap *b = NULL;
    GpGraphics *g = NULL;
    if (GdipCreateBitmapFromScan0(c->W, c->H, 0, PXF_PARGB, NULL, &b) != 0) return NULL;
    if (GdipGetImageGraphicsContext(b, &g) != 0) { GdipDisposeImage(b); return NULL; }
    SetupG(g);
    GdipGraphicsClear(g, 0);
    ARGB tc = g_dark ? ARGBC(255, 240, 240, 245) : ARGBC(255, 28, 28, 34);
    int cnt = LpCount(c);
    for (int k = 0; k < c->per; k++) {
        int idx = page * c->per + k;
        if (idx >= cnt) break;
        float x = c->gx + (k % c->cols) * c->cw, y = c->gy + (k / c->cols) * c->ch;
        LpDrawCell(c, g, idx, x, y, tc, FALSE);
    }
    GdipDeleteGraphics(g);
    c->cache[slot] = b;
    c->cachePage[slot] = page;
    return b;
}

static BOOL LpCellAt(LpCtx *c, int x, int y, int *cell, BOOL *center) {
    float lx = x - c->gx + c->page * (float)c->W - c->off, ly = y - c->gy;
    (void)lx;
    float px = x - c->gx - (c->page * (float)c->W - c->off) * -1.f;
    px = (float)x - c->gx;   /* the page is settled while the user clicks / drops */
    if (px < 0 || ly < 0) return FALSE;
    int col = (int)(px / c->cw), row = (int)(ly / c->ch);
    if (col >= c->cols || row >= c->rows) return FALSE;
    *cell = row * c->cols + col;
    float cx = c->gx + col * c->cw + c->cw / 2, cy = c->gy + row * c->ch + c->ch / 2 - Sc(10);
    *center = fabsf(x - cx) < c->cw * 0.26f && fabsf(y - cy) < c->is * 0.5f;
    return TRUE;
}

static void LpFoldPanelLayout(LpCtx *c) {
    LpFold *fo = &c->f[c->openFold];
    int pc = fo->n < 5 ? (fo->n < 1 ? 1 : fo->n) : 5, pr = (fo->n + 4) / 5;
    if (pr < 1) pr = 1;
    float pw = pc * c->cw + Sc(60), ph = pr * c->ch + Sc(96);
    c->panel.left = (int)((c->W - pw) / 2); c->panel.top = (int)((c->H - ph) / 2);
    c->panel.right = (int)(c->panel.left + pw); c->panel.bottom = (int)(c->panel.top + ph);
}

static void LpDraw(LpCtx *c, HWND h) {
    c->lastDraw = GetTickCount();
    GpGraphics *g = c->cv.g;
    GdipGraphicsClear(g, 0);
    int lpo = cfg.lpOpacity < 0 ? 0 : (cfg.lpOpacity > 100 ? 100 : cfg.lpOpacity);
    if (cfg.lpFrost && c->frost.gp && !c->dragging) {   /* skip the blur while dragging so the icon tracks the cursor */
        FrostSnapPaint(g, &c->frost, 0, 0, (float)c->W, (float)c->H, 0.f, lpo / 100.f);
        FillRR(g, g_dark ? ARGBC(132 * lpo / 100, 12, 13, 18) : ARGBC(128 * lpo / 100, 238, 241, 248), 0, 0, (float)c->W, (float)c->H, 0.f);
    } else {
        FillRR(g, g_dark ? ARGBC(232 * lpo / 100, 12, 13, 18) : ARGBC(232 * lpo / 100, 238, 241, 248), 0, 0, (float)c->W, (float)c->H, 0);
    }
    ARGB tc = g_dark ? ARGBC(255, 240, 240, 245) : ARGBC(255, 28, 28, 34);
    ARGB sub = g_dark ? ARGBC(150, 210, 212, 222) : ARGBC(160, 70, 74, 88);
    float sw = (float)Sc(380), sh = (float)Sc(44), sx = (c->W - sw) / 2, sy = (float)Sc(40);
    FillRR(g, g_dark ? ARGBC(70, 255, 255, 255) : ARGBC(150, 255, 255, 255), sx, sy, sw, sh, sh / 2);
    StrokeRR(g, g_dark ? ARGBC(60, 255, 255, 255) : ARGBC(40, 0, 0, 0), 1, sx, sy, sw, sh, sh / 2);
    DrawCentered(g, c->q[0] ? c->q : L"搜索应用…（直接输入文字）", g_font, sx + Sc(16), sy, sw - Sc(32), sh, c->q[0] ? tc : sub);
    if (cfg.lpTabs) {   /* category tabs: 全部 + custom tags, plus a "+" to add more */
        int nt = 1 + c->ntag;
        float ty0 = sy + sh + Sc(10), tw2 = (sw - (nt + 0) * Sc(8)) / (nt + 1);
        for (int t = 0; t < nt; t++) {
            float tx = sx + t * (tw2 + Sc(8));
            c->catR[t].left = (int)tx; c->catR[t].top = (int)ty0; c->catR[t].right = (int)(tx + tw2); c->catR[t].bottom = (int)(ty0 + Sc(30));
            BOOL on = (c->cat == t);
            FillRR(g, on ? CcAccent() : (g_dark ? ARGBC(70, 255, 255, 255) : ARGBC(150, 255, 255, 255)), tx, ty0, tw2, Sc(30), Sc(9));
            DrawCentered(g, t == 0 ? L"全部" : c->tagName[t - 1], g_font, tx, ty0, tw2, Sc(30), on ? ARGBC(255, 255, 255, 255) : sub);
        }
        float ax2 = sx + nt * (tw2 + Sc(8));
        c->addR.left = (int)ax2; c->addR.top = (int)ty0; c->addR.right = (int)(ax2 + tw2); c->addR.bottom = (int)(ty0 + Sc(30));
        FillRR(g, g_dark ? ARGBC(70, 255, 255, 255) : ARGBC(150, 255, 255, 255), ax2, ty0, tw2, Sc(30), Sc(9));
        DrawCentered(g, L"＋", g_font, ax2, ty0, tw2, Sc(30), sub);
    } else {
        for (int t = 0; t < 9; t++) ZeroMemory(&c->catR[t], sizeof(RECT));
        ZeroMemory(&c->addR, sizeof(RECT));
        if (c->cat) { c->cat = 0; LpCatFilter(c); }
    }
    /* "+" button: add apps / files / links, new folder */
    float ax = (float)Sc(40), ay = (float)Sc(40);   /* A–Z sort button, mirrors the + button */
    FillRR(g, c->hover == -3 ? (g_dark ? ARGBC(90, 255, 255, 255) : ARGBC(80, 0, 0, 0)) : (g_dark ? ARGBC(60, 255, 255, 255) : ARGBC(40, 0, 0, 0)), ax, ay, sh, sh, sh / 2);
    DrawCentered(g, L"A–Z", g_fontBold, ax, ay, sh, sh, tc);
    float bx = c->W - Sc(84.f), by = (float)Sc(40);
    FillRR(g, c->hover == -2 ? (g_dark ? ARGBC(90, 255, 255, 255) : ARGBC(80, 0, 0, 0)) : (g_dark ? ARGBC(60, 255, 255, 255) : ARGBC(40, 0, 0, 0)), bx, by, sh, sh, sh / 2);
    DrawCentered(g, L"＋", g_fontBold, bx, by, sh, sh, tc);
    int pages = LpPages(c), cnt = LpCount(c);
    if (!c->dragging) {
        /* cached pages: the highlight is painted first, the pre-rendered icons / labels are blitted on top */
        if (c->hover >= 0 && c->hover < cnt) {
            int p = c->hover / c->per, k = c->hover % c->per;
            if (p >= c->page - 1 && p <= c->page + 1) {
                float shift = p * (float)c->W - c->off;
                float x = c->gx + (k % c->cols) * c->cw + shift, y = c->gy + (k / c->cols) * c->ch;
                FillRR(g, g_dark ? ARGBC(46, 255, 255, 255) : ARGBC(34, 0, 0, 0), x + 4, y + 2, c->cw - 8, c->ch - 8, 16);
            }
        }
        for (int p = c->page - 1; p <= c->page + 1; p++) {
            if (p < 0 || p >= pages) continue;
            float shift = p * (float)c->W - c->off;
            if (shift >= c->W || shift <= -c->W) continue;   /* fully off screen: do not render / cache it */
            GpBitmap *img = LpPageImage(c, p);
            if (img) GdipDrawImageRectRect(g, img, shift, 0, (REAL)c->W, (REAL)c->H, 0, 0, (REAL)c->W, (REAL)c->H, 2, g_attr, NULL, NULL);
            else for (int k = 0; k < c->per; k++) {   /* no memory for the cache: draw that page the slow way */
                int idx = p * c->per + k;
                if (idx >= cnt) break;
                float x = c->gx + (k % c->cols) * c->cw + shift, y = c->gy + (k / c->cols) * c->ch;
                LpDrawCell(c, g, idx, x, y, tc, FALSE);
            }
        }
    } else {
        for (int p = c->page - 1; p <= c->page + 1; p++) {
            if (p < 0 || p >= pages) continue;
            float shift = p * (float)c->W - c->off;
            if (shift >= c->W || shift <= -c->W) continue;
            for (int k = 0; k < c->per; k++) {
                int idx = p * c->per + k;
                if (idx >= cnt) break;
                float x = c->gx + (k % c->cols) * c->cw + shift, y = c->gy + (k / c->cols) * c->ch;
                if (idx == c->hover) FillRR(g, g_dark ? ARGBC(46, 255, 255, 255) : ARGBC(34, 0, 0, 0), x + 4, y + 2, c->cw - 8, c->ch - 8, 16);
                LpDrawCell(c, g, idx, x, y, tc, idx == c->dragSrc);
            }
        }
    }
    if (cnt == 0) DrawCentered(g, c->q[0] ? L"没有找到匹配的应用" : L"起动台是空的，点右上角“＋”添加", g_font, 0, c->H / 2.f - 20, (float)c->W, 40, sub);
    if (pages > 1) {
        float dx = c->W / 2.f - (pages - 1) * Sc(11);
        for (int p = 0; p < pages && p < 40; p++) FillRR(g, p == c->page ? tc : sub, dx + p * Sc(22) - 4, c->H - Sc(108.f), 8, 8, 4);
    }
    if (c->dragging) {   /* the icon follows the cursor */
        float is = c->is * 1.1f, x = c->cur.x - is / 2, y = c->cur.y - is / 2;
        int cell; BOOL ctr;
        if (LpCellAt(c, c->cur.x, c->cur.y, &cell, &ctr) && ctr && c->page * c->per + cell < c->nt && c->page * c->per + cell != c->dragSrc)
            StrokeRR(g, ARGBC(230, 10, 132, 255), 2.5f, c->gx + (cell % c->cols) * c->cw + c->cw * 0.5f - c->is * 0.62f, c->gy + (cell / c->cols) * c->ch + (c->ch - c->is - Sc(34)) / 2 - c->is * 0.12f, c->is * 1.24f, c->is * 1.24f, c->is * 0.3f);
        LpTop *t = &c->top[c->dragSrc];
        if (t->fold) LpDrawFolderIcon(g, c, &c->f[t->idx], x, y, is);
        else { GpBitmap *ic = LpIcon(&c->a[t->idx]); if (ic) DrawImg(g, ic, x, y, is, is, FALSE); }
    }
    if (c->openFold >= 0) {   /* folder panel */
        LpFold *fo = &c->f[c->openFold];
        LpFoldPanelLayout(c);
        FillRR(g, ARGBC(120, 0, 0, 0), 0, 0, (float)c->W, (float)c->H, 0);
        float px = (float)c->panel.left, py = (float)c->panel.top, pw = (float)(c->panel.right - c->panel.left), ph = (float)(c->panel.bottom - c->panel.top);
        FillRR(g, g_dark ? ARGBC(246, 40, 41, 48) : ARGBC(246, 246, 247, 252), px, py, pw, ph, 28);
        StrokeRR(g, g_dark ? ARGBC(60, 255, 255, 255) : ARGBC(40, 0, 0, 0), 1, px, py, pw, ph, 28);
        DrawCentered(g, fo->name, g_fontBold, px, py + Sc(14), pw, (float)Sc(40), tc);
        DrawCentered(g, L"点击名称可重命名 · 右键图标可移出文件夹", g_fS, px, py + Sc(50), pw, (float)Sc(20), sub);
        for (int k = 0; k < fo->n; k++) {
            float x = px + Sc(30) + (k % 5) * c->cw, y = py + Sc(76) + (k / 5) * c->ch;
            if (k == c->fhover) FillRR(g, g_dark ? ARGBC(46, 255, 255, 255) : ARGBC(34, 0, 0, 0), x + 4, y + 2, c->cw - 8, c->ch - 8, 16);
            LpApp *a = &c->a[fo->app[k]];
            GpBitmap *ic = LpIcon(a);
            float iy = y + (c->ch - c->is - Sc(34)) / 2;
            if (ic) DrawImg(g, ic, x + (c->cw - c->is) / 2, iy, c->is, c->is, FALSE);
            DrawTextBox(g, a->name, g_font, x + 4, iy + c->is + 6, c->cw - 8, (float)Sc(36), tc, FALSE);
        }
    }
    CanvasPresentA(&c->cv, h, c->scr.left, c->scr.top, c->alpha);
}

/* present the canvas that is already painted: a fade or a slide frame must not repaint 30 icons,
   30 labels and a full-screen blurred backdrop (that is what made the launchpad stutter on Windows 10) */
static void LpPresent(LpCtx *c, HWND h) { CanvasPresentA(&c->cv, h, c->scr.left, c->scr.top, c->alpha); }

static void LpRemoveTop(LpCtx *c, int k) { memmove(&c->top[k], &c->top[k + 1], sizeof(LpTop) * (c->nt - k - 1)); c->nt--; }
static void LpInsertTop(LpCtx *c, int k, LpTop t) { if (k > c->nt) k = c->nt; memmove(&c->top[k + 1], &c->top[k], sizeof(LpTop) * (c->nt - k)); c->top[k] = t; c->nt++; }

static void LpFolderCleanup(LpCtx *c) {   /* folders with 0 apps vanish, 1 app dissolves */
    for (int k = c->nt - 1; k >= 0; k--) {
        if (!c->top[k].fold) continue;
        LpFold *fo = &c->f[c->top[k].idx];
        if (fo->n == 0) LpRemoveTop(c, k);
        else if (fo->n == 1) { c->top[k].fold = 0; c->top[k].idx = fo->app[0]; }
    }
}

static void LpDrop(LpCtx *c, int x, int y) {
    int cell; BOOL ctr;
    int src = c->dragSrc;
    if (!LpCellAt(c, x, y, &cell, &ctr)) return;
    int tk = c->page * c->per + cell;
    if (tk == src) return;
    if (tk < c->nt && ctr && !c->top[src].fold) {
        LpTop *tg = &c->top[tk];
        int ai = c->top[src].idx;
        if (tg->fold) {
            LpFold *fo = &c->f[tg->idx];
            if (fo->n < LP_MAXC) { fo->app[fo->n++] = ai; LpRemoveTop(c, src); }
        } else if (c->nf < LP_MAXF) {
            int fi = c->nf++;
            ZeroMemory(&c->f[fi], sizeof(LpFold));
            lstrcpyW(c->f[fi].name, L"文件夹");
            c->f[fi].app[0] = tg->idx; c->f[fi].app[1] = ai; c->f[fi].n = 2;
            tg->fold = 1; tg->idx = fi;
            LpRemoveTop(c, src);
        }
    } else {   /* reorder: the dragged entry ends up where it was dropped */
        LpTop t = c->top[src];
        LpRemoveTop(c, src);
        if (tk > c->nt) tk = c->nt;
        LpInsertTop(c, tk, t);
    }
    LpFolderCleanup(c);
    LpSave(c);
}

static void LpAddCustom(LpCtx *c, int kind) {
    WCHAR path[1024] = L"", nm[80] = L"";
    BOOL got = FALSE;
    if (kind == 1) {
        OPENFILENAMEW o;
        ZeroMemory(&o, sizeof o);
        o.lStructSize = sizeof o; o.hwndOwner = g_hset; o.lpstrFile = path; o.nMaxFile = MAX_PATH;
        o.lpstrFilter = L"所有文件 (*.*)\0*.*\0";
        o.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_DONTADDTORECENT;
        got = GetOpenFileNameW(&o);
    } else if (kind == 2) {
        BROWSEINFOW b;
        ZeroMemory(&b, sizeof b);
        b.hwndOwner = g_hset; b.lpszTitle = L"选择要加入起动台的文件夹"; b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST pi = SHBrowseForFolderW(&b);
        if (pi) { got = SHGetPathFromIDListW(pi, path); CoTaskMemFree(pi); }
    } else if (kind == 3) got = InputBox(g_hset, L"添加网址", L"网址：", path, 1024, NULL, NULL, 0) && path[0];
    if (!got || c->nt >= LP_MAXT) return;
    if (kind == 3) { lstrcpynW(nm, path, 80); WCHAR *s2 = wcsstr(nm, L"://"); if (s2) memmove(nm, s2 + 3, (lstrlenW(s2 + 3) + 1) * sizeof(WCHAR)); WCHAR *sl = wcschr(nm, L'/'); if (sl) *sl = 0; if (!wcsstr(path, L"://")) { WCHAR t[1024]; wsprintfW(t, L"https://%s", path); lstrcpynW(path, t, MAX_PATH); } }
    else { FileDescr(path, nm, 80); if (!nm[0]) { lstrcpynW(nm, PathFindFileNameW(path), 80); PathRemoveExtensionW(nm); } }
    int ai = LpAddApp(c, path, nm);
    if (ai < 0) return;
    c->top[c->nt].fold = 0; c->top[c->nt].idx = ai; c->nt++;
    LpSave(c);
}

static void LpLaunchApp(LpCtx *c, int ai) {
    ShellExecuteW(NULL, L"open", c->a[ai].path, NULL, NULL, SW_SHOWNORMAL);
    c->done = 1;
}

static int LpFolderHit(LpCtx *c, int x, int y) {
    LpFold *fo = &c->f[c->openFold];
    for (int k = 0; k < fo->n; k++) {
        float cx = c->panel.left + Sc(30) + (k % 5) * c->cw, cy = c->panel.top + Sc(76) + (k / 5) * c->ch;
        if (x >= cx && x < cx + c->cw && y >= cy && y < cy + c->ch) return k;
    }
    return -1;
}

static void LpSettle(LpCtx *c, float dx) {
    int pages = LpPages(c);
    if (dx < -c->W * 0.12f && c->page < pages - 1) c->page++;
    else if (dx > c->W * 0.12f && c->page > 0) c->page--;
}

static LRESULT CALLBACK LpProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    LpCtx *c = (LpCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    if (!c) return DefWindowProcW(h, m, w, l);
    int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
    switch (m) {
    case WM_TIMER: {
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) { if (c->openFold >= 0) { c->openFold = -1; } else c->done = 1; return 0; }   /* Esc closes it even while the search field has focus */
        BOOL redraw = FALSE, present = FALSE;
        if (!c->ready) {   /* opening fade: only the window alpha changes, the canvas stays as painted */
            float t = (GetTickCount() - c->t0) / 200.f;
            if (t >= 1.f) { c->alpha = 255; c->ready = 1; present = TRUE; }
            else { c->alpha = (int)(255 * (1.f - (1.f - t) * (1.f - t))); present = TRUE; }
        }
        float target = c->page * (float)c->W;
        if (!c->swiping && fabsf(c->off - target) > 0.5f) {
            c->off += (target - c->off) * 0.26f;
            if (fabsf(c->off - target) < 0.6f) c->off = target;
            if (GetTickCount() - c->lastDraw >= 22) redraw = TRUE;   /* ~45 fps is enough for a page slide */
            else present = TRUE;
        }
        if (!c->swiping && !c->dragging && fabsf(c->off - target) <= 0.5f) {
            for (int i = 0; i < 2; i++)   /* settled: release the neighbour page (the cache can be 30+ MB on 4K) */
                if (c->cache[i] && c->cachePage[i] != c->page) { GdipDisposeImage(c->cache[i]); c->cache[i] = NULL; c->cachePage[i] = -1; }
        }
        if (c->dragging && c->openFold < 0) {   /* hold an icon at the left / right edge to flip the page */
            int pages = LpPages(c);
            if (c->cur.x < Sc(50) && c->page > 0) { if (!c->edgeT) c->edgeT = GetTickCount(); else if (GetTickCount() - c->edgeT > 550) { c->page--; c->edgeT = 0; } }
            else if (c->cur.x > c->W - Sc(50) && c->page < pages - 1) { if (!c->edgeT) c->edgeT = GetTickCount(); else if (GetTickCount() - c->edgeT > 550) { c->page++; c->edgeT = 0; } }
            else c->edgeT = 0;
        }
        if (redraw) LpDraw(c, h);
        else if (present) LpPresent(c, h);
        return 0;
    }
    case WM_COMMAND:
        if (LOWORD(w) == 777 && HIWORD(w) == EN_CHANGE && g_lpEdit) {
            WCHAR t[64];
            GetWindowTextW(g_lpEdit, t, 64);
            lstrcpynW(c->q, t, 47);
            c->page = 0; c->off = 0; c->openFold = -1;
            LpFilter(c);
            LpDraw(c, h);
        }
        return 0;
    case WM_LBUTTONDOWN:
        SetCapture(h);
        c->down.x = c->cur.x = x; c->down.y = c->cur.y = y;
        c->dragging = 0; c->swiping = 0; c->press = -1; c->pressFold = -1;
        if (c->openFold < 0 && y > Sc(42) && y < Sc(82) && abs(x - c->W / 2) < Sc(150)) { SetFocus(h); return 0; }   /* actual search field hit area */
        if (c->openFold >= 0) { c->pressFold = LpFolderHit(c, x, y); return 0; }
        { int cell; BOOL ctr; if (LpCellAt(c, x, y, &cell, &ctr) && c->page * c->per + cell < LpCount(c)) c->press = c->page * c->per + cell; }
        c->pressOk = 0;
        if (c->press >= 0 && !LpFiltered(c) && c->top[c->press].fold) c->pressOk = 2;   /* a folder opens, it cannot launch the wrong app */
        else if (c->press >= 0) {
            int ai = LpFiltered(c) ? c->vis[c->press] : c->top[c->press].idx;
            if (ai >= 0 && ai < c->na) { lstrcpynW(c->pressPath, c->a[ai].path, MAX_PATH); c->pressOk = 1; }
        }
        c->offStart = c->off;
        return 0;
    case WM_MOUSEMOVE: {
        c->cur.x = x; c->cur.y = y;
        if (GetCapture() == h && (w & MK_LBUTTON) && c->openFold < 0) {
            int dx = x - c->down.x, dy = y - c->down.y;
            if (!c->dragging && !c->swiping) {
                if (c->press >= 0 && !c->q[0] && (abs(dx) > Sc(8) || abs(dy) > Sc(8))) { c->dragging = 1; c->dragSrc = c->press; c->edgeT = 0; }
                else if (c->press < 0 && abs(dx) > Sc(10) && abs(dx) > abs(dy)) c->swiping = 1;
                else if (c->press >= 0 && c->q[0] && abs(dx) > Sc(14) && abs(dx) > abs(dy)) { c->swiping = 1; c->press = -1; }
            }
            if (c->swiping) c->off = c->offStart - dx;
            if (GetTickCount() - c->lastDraw >= 16) LpDraw(c, h);   /* at most ~60 fps: no redraw per mouse event */
            return 0;
        }
        int hv = -1, fh = -1;
        if (c->openFold >= 0) fh = LpFolderHit(c, x, y);
        else {
            int cell; BOOL ctr;
            if (LpCellAt(c, x, y, &cell, &ctr) && c->page * c->per + cell < LpCount(c)) hv = c->page * c->per + cell;
            if (x >= c->W - Sc(84.f) && x < c->W - Sc(84.f) + Sc(44) && y >= Sc(40) && y < Sc(84)) hv = -2;
            else if (x >= Sc(40) && x < Sc(84) && y >= Sc(40) && y < Sc(84)) hv = -3;
        }
        if (hv != c->hover || fh != c->fhover) { c->hover = hv; c->fhover = fh; LpDraw(c, h); }
        return 0;
    }
    case WM_LBUTTONUP: {
        if (GetCapture() == h) ReleaseCapture();
        if (c->dragging) { c->dragging = 0; LpDrop(c, x, y); c->hover = -1; LpDraw(c, h); return 0; }
        if (c->swiping) { c->swiping = 0; LpSettle(c, (float)(x - c->down.x)); LpDraw(c, h); return 0; }
        if (abs(x - c->down.x) > Sc(8) || abs(y - c->down.y) > Sc(8)) return 0;
        if (c->openFold >= 0) {
            LpFold *fo = &c->f[c->openFold];
            int k = LpFolderHit(c, x, y);
            POINT p = { x, y };
            if (k >= 0 && k == c->pressFold) { LpLaunchApp(c, fo->app[k]); return 0; }
            if (!PtInRect(&c->panel, p)) { c->done = 1; return 0; }
            if (y < c->panel.top + Sc(50)) {   /* rename */
                WCHAR nm[48];
                lstrcpynW(nm, fo->name, 48);
                c->inMenu = 1;
                if (InputBox(h, L"重命名文件夹", L"名称：", nm, 48, NULL, NULL, 0) && nm[0]) { lstrcpynW(fo->name, nm, 48); LpSave(c); }
                c->inMenu = 0;
                SetForegroundWindow(h);
                LpDraw(c, h);
            }
            return 0;
        }
        if (x >= Sc(40) && x < Sc(84) && y >= Sc(40) && y < Sc(84)) {   /* A–Z */
            LpSortAZ(c);
            LpSave(c);
            c->page = 0;
            LpDraw(c, h);
            return 0;
        }
        if (cfg.lpTabs) for (int t = 0; t < 1 + c->ntag; t++) if (x >= c->catR[t].left && x <= c->catR[t].right && y >= c->catR[t].top && y <= c->catR[t].bottom) {   /* category tab */
            c->cat = t;
            LpCatFilter(c);
            c->page = 0; c->off = 0;
            LpDraw(c, h);
            return 0;
        }
        if (c->ntag < 8 && x >= c->addR.left && x <= c->addR.right && y >= c->addR.top && y <= c->addR.bottom) {   /* add a tag */
            WCHAR nm[32] = L"";
            c->inMenu = 1;
            BOOL ok = InputBox(h, L"添加标签组", L"名称：", nm, 32, NULL, NULL, 0);
            c->inMenu = 0;
            SetForegroundWindow(h);
            if (ok) { StrTrimW(nm, L" \t"); if (nm[0]) { lstrcpynW(c->tagName[c->ntag], nm, 32); c->ntag++; LpTagSave(c); } }
            LpDraw(c, h);
            return 0;
        }
        if (x >= c->W - Sc(84.f) && x < c->W - Sc(84.f) + Sc(44) && y >= Sc(40) && y < Sc(84)) {   /* + */
            CmItem it[5];
            int n = 0;
            n = CmAdd(it, n, 1, L"添加应用 / 文件…", 0);
            n = CmAdd(it, n, 2, L"添加文件夹…", 0);
            n = CmAdd(it, n, 3, L"添加网址…", 0);
            POINT p = { x, y };
            ClientToScreen(h, &p);
            c->inMenu = 1;
            int r = ShowMenu(it, n, p.x, p.y);
            if (r >= 1 && r <= 3) LpAddCustom(c, r);
            c->inMenu = 0;
            SetForegroundWindow(h);
            LpDraw(c, h);
            return 0;
        }
        if (y > Sc(42) && y < Sc(82) && abs(x - c->W / 2) < Sc(150)) return 0; /* search field is a control, not blank background */
        int cell; BOOL ctr;
        int pages = LpPages(c);
        if (LpCellAt(c, x, y, &cell, &ctr) && c->press >= 0 && c->page * c->per + cell == c->press) {
            if (LpFiltered(c)) {
                int ai = c->vis[c->press];
                if (c->pressOk != 1 || ai < 0 || ai >= c->na || lstrcmpiW(c->a[ai].path, c->pressPath) != 0) return 0;
                LpLaunchApp(c, ai);
            } else if (c->top[c->press].fold) { c->openFold = c->top[c->press].idx; c->fhover = -1; LpDraw(c, h); }
            else {
                int ai = c->top[c->press].idx;
                if (c->pressOk != 1 || ai < 0 || ai >= c->na || lstrcmpiW(c->a[ai].path, c->pressPath) != 0) return 0;
                LpLaunchApp(c, ai);
            }
            return 0;
        }
        if (pages > 1 && y > c->H - Sc(120) && y < c->H - Sc(84)) {   /* only the dot strip, not the icons above it */
            float dx = c->W / 2.f - (pages - 1) * Sc(11);
            int p = (int)((x - dx + Sc(11)) / Sc(22));
            if (p >= 0 && p < pages) { c->page = p; LpDraw(c, h); }
            return 0;
        }
        /* Any click not consumed by a real control or app cell is a blank-area click. */
        c->done = 1;
        return 0;
    }
    case WM_RBUTTONUP: {
        for (int t = 1; t < 1 + c->ntag; t++) if (x >= c->catR[t].left && x <= c->catR[t].right && y >= c->catR[t].top && y <= c->catR[t].bottom) {   /* rename / delete a tag */
            CmItem it[2];
            int n = 0;
            n = CmAdd(it, n, 1, L"重命名…", 0);
            n = CmAdd(it, n, 2, L"删除标签", CM_RED);
            POINT p = { x, y };
            ClientToScreen(h, &p);
            c->inMenu = 1;
            int r = ShowMenu(it, n, p.x, p.y);
            c->inMenu = 0;
            SetForegroundWindow(h);
            if (r == 1) {
                WCHAR nm[32];
                lstrcpynW(nm, c->tagName[t - 1], 32);
                c->inMenu = 1;
                if (InputBox(h, L"重命名标签", L"名称：", nm, 32, NULL, NULL, 0)) { StrTrimW(nm, L" \t"); if (nm[0]) lstrcpynW(c->tagName[t - 1], nm, 32); }
                c->inMenu = 0;
                SetForegroundWindow(h);
                LpTagSave(c);
            } else if (r == 2 && c->ntag > 1) {
                for (int j = t - 1; j < c->ntag - 1; j++) lstrcpynW(c->tagName[j], c->tagName[j + 1], 32);
                c->ntag--;
                LpTagSave(c);
                if (c->cat >= c->ntag + 1) c->cat = 0;
            }
            LpDraw(c, h);
            return 0;
        }
        if (c->openFold >= 0) {
            LpFold *fo = &c->f[c->openFold];
            int k = LpFolderHit(c, x, y);
            if (k < 0) {
                CmItem bgm[2]; int bn = 0;
                bn = CmAdd(bgm, bn, 1, L"启动台设置", 0);
                bn = CmAdd(bgm, bn, 2, L"关闭启动台", 0);
                POINT bp = { x, y }; ClientToScreen(h, &bp);
                c->inMenu = 1; int br = ShowMenu(bgm, bn, bp.x, bp.y); c->inMenu = 0;
                SetForegroundWindow(h);
                if (br == 1) { c->done = 1; ShowSettings(3); }
                else if (br == 2) c->done = 1;
                return 0;
            }
            CmItem it[5];
            int n = 0;
            n = CmAdd(it, n, 1, L"移出文件夹", 0);
            n = CmAdd(it, n, 3, L"添加到 Dock 栏", 0);
            n = CmAdd(it, n, 2, L"从起动台移除", CM_RED);
            n = CmAdd(it, n, 4, L"启动台设置", 0);
            POINT p = { x, y };
            ClientToScreen(h, &p);
            c->inMenu = 1;
            int r = ShowMenu(it, n, p.x, p.y);
            c->inMenu = 0;
            SetForegroundWindow(h);
            if (r == 4) { c->done = 1; ShowSettings(3); return 0; }
            if (r == 3) {   /* add to the dock, keep it in the launchpad folder */
                AddFromString(c->a[fo->app[k]].path, -1);
                UpdateGeometry();
                ScheduleSave();
                Kick();
                LpDraw(c, h);
                return 0;
            }
            if (r) {
                int ai = fo->app[k];
                memmove(&fo->app[k], &fo->app[k + 1], sizeof(int) * (fo->n - k - 1));
                fo->n--;
                if (r == 1 && c->nt < LP_MAXT) { c->top[c->nt].fold = 0; c->top[c->nt].idx = ai; c->nt++; }
                else if (r == 2 && c->nh < 200) lstrcpynW(c->hid[c->nh++], c->a[ai].path, MAX_PATH);
                if (fo->n <= 1) c->openFold = -1;
                LpFolderCleanup(c);
                LpSave(c);
            }
            LpDraw(c, h);
            return 0;
        }
        int cell; BOOL ctr;
        if (!LpCellAt(c, x, y, &cell, &ctr) || c->page * c->per + cell >= LpCount(c)) {
            if (!LpCellAt(c, x, y, &cell, &ctr)) {   /* right-click on the empty background: launchpad settings */
                CmItem it[2];
                int n = 0;
                n = CmAdd(it, n, 1, L"启动台设置", 0);
                n = CmAdd(it, n, 2, L"关闭启动台", 0);
                POINT p = { x, y };
                ClientToScreen(h, &p);
                c->inMenu = 1;
                int r = ShowMenu(it, n, p.x, p.y);
                c->inMenu = 0;
                SetForegroundWindow(h);
                if (r == 1) { c->done = 1; ShowSettings(3); }
                else if (r == 2) c->done = 1;
                return 0;
            }
            c->done = 1;
            return 0;
        }
        int k = c->page * c->per + cell;
        CmItem it[9];
        int n = 0;
        if (LpFiltered(c)) { n = CmAdd(it, n, 1, L"打开", 0); n = CmAdd(it, n, 7, L"移动到标签组…", 0); n = CmAdd(it, n, 6, L"添加到 Dock 栏", 0); n = CmAdd(it, n, 4, L"从起动台移除", CM_RED); }
        else if (c->top[k].fold) { n = CmAdd(it, n, 1, L"打开", 0); n = CmAdd(it, n, 3, L"重命名…", 0); n = CmAdd(it, n, 5, L"解散文件夹", 0); }
        else { n = CmAdd(it, n, 1, L"打开", 0); n = CmAdd(it, n, 7, L"移动到标签组…", 0); n = CmAdd(it, n, 6, L"添加到 Dock 栏", 0); n = CmAdd(it, n, 3, L"重命名…", 0); n = CmAdd(it, n, 4, L"从起动台移除", CM_RED); }
        n = CmAdd(it, n, 8, L"启动台设置", 0);
        POINT p = { x, y };
        ClientToScreen(h, &p);
        c->inMenu = 1;
        int r = ShowMenu(it, n, p.x, p.y);
        c->inMenu = 0;
        SetForegroundWindow(h);
        if (r == 8) { c->done = 1; ShowSettings(3); return 0; }
        if (r == 7) {   /* move to a tag group */
            int ai = LpFiltered(c) ? c->vis[k] : c->top[k].idx;
            CmItem m2[4];
            int m = 0;
            m = CmAdd(m2, m, 1, L"办公", 0);
            m = CmAdd(m2, m, 2, L"系统", 0);
            m = CmAdd(m2, m, 3, L"工具", 0);
            m = CmAdd(m2, m, 4, L"自动分类（清除）", 0);
            POINT p2 = { x, y };
            ClientToScreen(h, &p2);
            c->inMenu = 1;
            int r2 = ShowMenu(m2, m, p2.x, p2.y);
            c->inMenu = 0;
            SetForegroundWindow(h);
            if (r2 >= 1 && r2 <= 4) { LpSetCat(c, ai, r2 == 4 ? -1 : r2); if (c->cat) LpCatFilter(c); }
            LpDraw(c, h);
            return 0;
        }
        if (r == 1) {
            if (LpFiltered(c)) LpLaunchApp(c, c->vis[k]);
            else if (c->top[k].fold) c->openFold = c->top[k].idx;
            else LpLaunchApp(c, c->top[k].idx);
        } else if (r == 3 && !LpFiltered(c)) {
            WCHAR nm[80];
            WCHAR *dst = c->top[k].fold ? c->f[c->top[k].idx].name : c->a[c->top[k].idx].name;
            lstrcpynW(nm, dst, 80);
            if (InputBox(h, L"重命名", L"名称：", nm, 80, NULL, NULL, 0) && nm[0]) { lstrcpynW(dst, nm, c->top[k].fold ? 48 : 80); LpSave(c); }
            SetForegroundWindow(h);
        } else if (r == 6 && !c->top[k].fold && (LpFiltered(c) ? c->vis[k] : c->top[k].idx) >= 0) {
            AddFromString(c->a[LpFiltered(c) ? c->vis[k] : c->top[k].idx].path, -1);
            UpdateGeometry();
            ScheduleSave();
            Kick();
        } else if (r == 4) {
            const WCHAR *path = LpFiltered(c) ? c->a[c->vis[k]].path : c->a[c->top[k].idx].path;
            if (c->nh < 200) lstrcpynW(c->hid[c->nh++], path, MAX_PATH);
            if (!LpFiltered(c)) LpRemoveTop(c, k);
            LpSave(c);
        } else if (r == 5 && !LpFiltered(c)) {
            LpFold *fo = &c->f[c->top[k].idx];
            LpTop first = { 0, fo->n > 0 ? fo->app[0] : 0 };
            int cnt = fo->n, apps[LP_MAXC];
            memcpy(apps, fo->app, sizeof(int) * cnt);
            LpRemoveTop(c, k);
            for (int i = cnt - 1; i >= 0; i--) { first.idx = apps[i]; LpInsertTop(c, k, first); }
            LpSave(c);
        }
        LpDraw(c, h);
        return 0;
    }
    case WM_MOUSEWHEEL: {
        int pages = LpPages(c);
        c->page += (short)HIWORD(w) > 0 ? -1 : 1;
        if (c->page < 0) c->page = 0;
        if (c->page >= pages) c->page = pages - 1;
        return 0;
    }
    case WM_KEYDOWN:
        if (w == VK_ESCAPE) { if (c->openFold >= 0) { c->openFold = -1; LpDraw(c, h); } else c->done = 1; }
        else if (w == VK_LEFT && c->page > 0) c->page--;
        else if (w == VK_RIGHT && c->page + 1 < LpPages(c)) c->page++;
        else if (w == VK_RETURN && c->q[0] && c->nv > 0) LpLaunchApp(c, c->vis[0]);
        return 0;
    case WM_CHAR: {
        int n = lstrlenW(c->q);
        if (w == 8) { if (n > 0) c->q[n - 1] = 0; }
        else if (w >= 32 && n < 46) { c->q[n] = (WCHAR)w; c->q[n + 1] = 0; }
        else return 0;
        c->page = 0; c->off = 0; c->openFold = -1;
        LpFilter(c);
        LpDraw(c, h);
        return 0;
    }
    case WM_ACTIVATE: if (LOWORD(w) == WA_INACTIVE && c->ready && !c->inMenu) c->done = 1; return 0;
    case WM_SETCURSOR: SetCursor(LoadCursor(NULL, IDC_ARROW)); return TRUE;
    }
    return DefWindowProcW(h, m, w, l);
}

static void ShowLaunchpad(void) {
    LogEvent(L"launchpad-open", 0);
    static HWND busy;
    if (busy) { SetForegroundWindow(busy); return; }
    static BOOL reg;
    if (!reg) {
        WNDCLASSW wc;
        ZeroMemory(&wc, sizeof wc);
        wc.lpfnWndProc = LpProc; wc.hInstance = g_inst; wc.lpszClassName = CLS_LP; wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);
        reg = TRUE;
    }
    LpCtx *c = (LpCtx *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LpCtx));
    if (!c) return;
    c->a = (LpApp *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LpApp) * LP_MAXA);
    c->f = (LpFold *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LpFold) * LP_MAXF);
    c->top = (LpTop *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LpTop) * (LP_MAXT + 2));
    c->hid = (WCHAR (*)[MAX_PATH])HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(WCHAR) * MAX_PATH * 200);
    if (!c->a || !c->f || !c->top || !c->hid) goto cleanup;
    LpLoad(c);
    LpTagLoad(c);
    c->scr = g_mi.rcMonitor;
    c->W = c->scr.right - c->scr.left; c->H = c->scr.bottom - c->scr.top;
    c->hover = -1; c->openFold = -1; c->fhover = -1; c->press = -1;
    LpLayout(c);
    if (CanvasNew(&c->cv, c->W, c->H)) {
        HWND h = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, CLS_LP, L"", WS_POPUP, c->scr.left, c->scr.top, c->W, c->H, NULL, NULL, g_inst, NULL);
        busy = h;
        SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)c);
        if (cfg.lpFrost) { RECT fr = c->scr; FrostSnapGrab(&c->frost, &fr); }   /* self-drawn blur, captured once before the window shows */
        c->t0 = GetTickCount();
        /* no child EDIT: a child control on an UpdateLayeredWindow is invisible and unreliable;
           the launchpad takes the keyboard itself and WM_CHAR builds the search query. */
        LpDraw(c, h);
        ShowWindow(h, SW_SHOW);
        {   /* really take the keyboard, otherwise the type-to-search is dead */
            DWORD fg = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
            DWORD me = GetCurrentThreadId();
            if (fg && fg != me) AttachThreadInput(fg, me, TRUE);
            keybd_event(VK_MENU, 0, 0, 0);   /* tapping Alt lifts the foreground lock, so SetForegroundWindow really works */
            keybd_event(VK_MENU, 0, 2, 0);
            SetForegroundWindow(h);
            SetFocus(h);
            if (fg && fg != me) AttachThreadInput(fg, me, FALSE);
        }
        g_lpWnd = h;
        gMenuOpen = TRUE;
        gLpOpen = TRUE;
        ClosePreview();
        SetTimer(h, 1, 15, NULL);
        MSG msg;
        while (!c->done && GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
        LpFolderCleanup(c);
        LpSave(c);
        gMenuOpen = FALSE;
        gLpOpen = FALSE;
        SetWindowLongPtrW(h, GWLP_USERDATA, 0);
        if (g_lpEdit) { DestroyWindow(g_lpEdit); g_lpEdit = NULL; }
        if (c->bk) DestroyWindow(c->bk);
        FrostSnapFree(&c->frost);
        DestroyWindow(h);
        g_lpWnd = NULL;
        busy = NULL;
        CanvasFree(&c->cv);
        g_popupClosedTick = GetTickCount();
        LogEvent(L"launchpad-close", 0);
    }
cleanup:
    LpCacheFree(c);
    for (int i = 0; c->a && i < c->na; i++) if (c->a[i].icon) GdipDisposeImage(c->a[i].icon);
    if (c->a) HeapFree(GetProcessHeap(), 0, c->a);
    if (c->f) HeapFree(GetProcessHeap(), 0, c->f);
    if (c->top) HeapFree(GetProcessHeap(), 0, c->top);
    if (c->hid) HeapFree(GetProcessHeap(), 0, c->hid);
    HeapFree(GetProcessHeap(), 0, c);
    gLastNear = GetTickCount();
    Kick();
}

/* ======================= guardian: restarts the dock if a cleaner tool kills it ======================= */
static DWORD g_guardPid;
static HANDLE g_quitEvt;

static void SpawnGuardian(void) {
    if (!cfg.guardian || g_guardPid) return;
    WCHAR cmd[MAX_PATH + 64];
    wsprintfW(cmd, L"\"%s\" --guard %lu", g_exe, GetCurrentProcessId());
    STARTUPINFOW si;
    ZeroMemory(&si, sizeof si);
    si.cb = sizeof si;
    PROCESS_INFORMATION pi;
    if (CreateProcessW(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW | DETACHED_PROCESS, NULL, NULL, &si, &pi)) {
        g_guardPid = pi.dwProcessId;
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    }
}

static int GuardMain(DWORD mainPid) {
    HANDLE evt = CreateEventW(NULL, TRUE, FALSE, L"Local\\MiniDock_Wangcy_Quit");
    HANDLE ph = OpenProcess(SYNCHRONIZE, FALSE, mainPid);
    if (!ph) return 0;
    WaitForSingleObject(ph, INFINITE);
    CloseHandle(ph);
    if (evt && WaitForSingleObject(evt, 0) == WAIT_OBJECT_0) return 0;   /* the user chose "Exit" */
    if (GetSystemMetrics(SM_SHUTTINGDOWN)) return 0;                     /* shutting down: do not come back */
    Sleep(1200);
    /* Do not turn a crash loop into a stream of new windows: remember the last restart, and give up when it
       keeps dying. This is what made the dock look like it "starts by itself" over and over. */
    WCHAR tf[MAX_PATH], stamp[MAX_PATH];
    DWORD n = GetTempPathW(MAX_PATH, tf);
    if (!n) return 0;
    PathCombineW(stamp, tf, L"MiniDock.guard");
    FILETIME now;
    GetSystemTimeAsFileTime(&now);
    ULONGLONG nowv = ((ULONGLONG)now.dwHighDateTime << 32) | now.dwLowDateTime;
    ULONGLONG lastv = 0;
    int count = 0;
    HANDLE f = CreateFileW(stamp, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f != INVALID_HANDLE_VALUE) {
        char buf[64] = { 0 };
        DWORD rd = 0;
        ReadFile(f, buf, 60, &rd, NULL);
        CloseHandle(f);
        unsigned long long a = 0;
        int c = 0;
        if (sscanf(buf, "%llu %d", &a, &c) == 2) { lastv = a; count = c; }
    }
    if (lastv && nowv > lastv && (nowv - lastv) < 100000000ULL) count++;   /* < 10 s ago */
    else count = 1;
    if (count > 6) return 0;   /* it keeps dying: stop restarting and let the user start it manually */
    f = CreateFileW(stamp, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (f != INVALID_HANDLE_VALUE) {
        char buf[64];
        int len = wsprintfA(buf, "%I64u %d", nowv, count);
        DWORD wr = 0;
        WriteFile(f, buf, (DWORD)len, &wr, NULL);
        CloseHandle(f);
    }
    WCHAR exe[MAX_PATH];
    GetModuleFileNameW(NULL, exe, MAX_PATH);
    ShellExecuteW(NULL, L"open", exe, NULL, NULL, SW_SHOWNORMAL);
    return 0;
}

/* ======================= add helpers ======================= */
enum {
    IDM_OPEN = 100, IDM_QUITAPP, IDM_SHOWFOLDER, IDM_WEBAPP, IDM_RENAME, IDM_REMOVE, IDM_ADDSEP_AFTER, IDM_SETTINGS, IDM_EXIT,
    IDM_AUTOSTART, IDM_ZOOM, IDM_ADDURL, IDM_ADDFILE, IDM_ADDFOLDER, IDM_ADDSEP_END, IDM_DELSEP, IDM_PIN, IDM_ICON_FILE,
    IDM_ICON_URL, IDM_ICON_RESET, IDM_UNGROUP, IDM_EMPTYBIN, IDM_ADDGROUP, IDM_ADDTRASH, IDM_NEWINST, IDM_LOCK, IDM_HIDE,
    IDM_SETICON, IDM_NEWSC, IDM_FOLDERFAN, IDM_NOTES, IDM_OPENFOLDER, IDM_NOTEOFF, IDM_ADDTEMPGRP, IDM_CLEARTEMP, IDM_LAUNCHPAD, IDM_GROUPWIDE, IDM_CLOSEAPP, IDM_COPYFILE, IDM_CLOSEWIN,
    IDM_TASKMGR, IDM_MULTIMODE, IDM_TRAYREMOVE
};

typedef struct { HMENU menu; HWND windows[128]; int count; } TrayPicker;

static BOOL CALLBACK TrayPickerProc(HWND h, LPARAM lp) {
    TrayPicker *p = (TrayPicker *)lp;
    if (p->count >= 128 || !IsAppWindow(h)) return TRUE;
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId()) return TRUE;
    WCHAR exe[MAX_PATH], title[256], label[400];
    if (!ExeOfWindow(h, exe) || TrayAppIndex(exe) >= 0) return TRUE;
    TrayWindowTitle(h, title);
    _snwprintf(label, 399, L"%s  [%s]", title, PathFindFileNameW(exe));
    label[399] = 0;
    if (AppendMenuW(p->menu, MF_STRING, p->count + 1, label)) p->windows[p->count++] = h;
    return TRUE;
}

static void AddTrayApp(HWND parent) {
    if (g_ntrayApps >= MAXTRAYAPPS) {
        MessageBoxW(parent, L"最多选择 16 个托盘常驻应用。", L"托盘应用常驻", MB_OK | MB_ICONINFORMATION);
        return;
    }
    TrayPicker picker;
    ZeroMemory(&picker, sizeof picker);
    picker.menu = CreatePopupMenu();
    if (!picker.menu) return;
    EnumWindows(TrayPickerProc, (LPARAM)&picker);
    POINT pt;
    GetCursorPos(&pt);
    int choice = 0;
    if (picker.count) choice = TrackPopupMenu(picker.menu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, parent, NULL);
    else MessageBoxW(parent, L"请先打开要添加的应用主窗口，再进行选择。", L"托盘应用常驻", MB_OK | MB_ICONINFORMATION);
    DestroyMenu(picker.menu);
    if (choice < 1 || choice > picker.count) return;
    HWND h = picker.windows[choice - 1];
    TrayApp a;
    ZeroMemory(&a, sizeof a);
    if (!IsAppWindow(h) || !ExeOfWindow(h, a.exe) || TrayAppIndex(a.exe) >= 0) return;
    GetClassNameW(h, a.cls, 128);
    TrayWindowTitle(h, a.title);
    if (!a.cls[0] || !a.title[0]) return;
    if (!BindTrayWindow(&a, h)) {
        MessageBoxW(parent, L"无法跟踪该应用进程，请确认 Dock 与应用的运行权限一致。", L"托盘应用常驻", MB_OK | MB_ICONINFORMATION);
        return;
    }
    FileDescr(a.exe, a.name, 128);
    if (!a.name[0]) lstrcpynW(a.name, PathFindFileNameW(a.exe), 128);
    CleanName(a.name);
    g_trayApps[g_ntrayApps++] = a;
    ScheduleSave();
}

static int CacheFor(int z) { return z <= 96 ? 96 : z <= 128 ? 128 : z <= 192 ? 192 : 256; }

static void DoAddUrl(HWND parent) {
    WCHAR url[1024] = L"https://", name[128] = L"";
    if (!InputBox(parent, L"添加网页应用", L"网址：", url, 1024, L"名称（可留空，默认使用域名）：", name, 128)) return;
    StrTrimW(url, L" \t");
    if (!url[0]) return;
    if (!IsUrlStr(url)) { WCHAR t[1024]; wsprintfW(t, L"https://%s", url); lstrcpyW(url, t); }
    int r = AddFromString(url, -1);
    if (r >= 0 && name[0]) { lstrcpynW(items[r].name, name, 128); CleanName(items[r].name); ScheduleSave(); }
    UpdateGeometry();
    Kick();
}

static void DoNewShortcut(HWND parent) {
    Item it;
    if (ShortcutDialog(parent, &it)) {
        InsertItemStruct(-1, &it, FALSE);
        ScheduleSave();
        UpdateGeometry();
        Kick();
    }
}

static void DoAddFile(HWND parent) {
    WCHAR f[MAX_PATH] = L"";
    OPENFILENAMEW o;
    ZeroMemory(&o, sizeof o);
    o.lStructSize = sizeof o; o.hwndOwner = parent;
    o.lpstrFilter = L"应用程序与快捷方式\0*.exe;*.lnk;*.url\0所有文件\0*.*\0";
    o.lpstrFile = f; o.nMaxFile = MAX_PATH;
    o.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NODEREFERENCELINKS;
    o.lpstrTitle = L"选择要固定到 Dock 的应用或文件";
    if (GetOpenFileNameW(&o)) { AddFromString(f, -1); UpdateGeometry(); Kick(); }
}

static void DoAddFolder(HWND parent) {
    BROWSEINFOW b;
    ZeroMemory(&b, sizeof b);
    b.hwndOwner = parent;
    b.lpszTitle = L"选择要固定到 Dock 的文件夹";
    b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    LPITEMIDLIST pi = SHBrowseForFolderW(&b);
    if (pi) {
        WCHAR p[MAX_PATH];
        if (SHGetPathFromIDListW(pi, p)) { AddFromString(p, -1); UpdateGeometry(); Kick(); }
        CoTaskMemFree(pi);
    }
}

static void DoAddSep(int idx) {
    Item s;
    FillItem(&s, T_SEP, 0, L"", L"", NULL);
    InsertItemStruct(idx, &s, FALSE);
    ScheduleSave();
    UpdateGeometry();
    Kick();
}

/* Ctrl+Alt+D: bring the dock out from any window, from any full-screen app, from anywhere. A second
   press puts it away again. Summoning sets a short grace period so auto-hide cannot pull it straight
   back in while the pointer is still on its way to the dock. */
static void ToggleHide(void) {
    static DWORD last;
    DWORD now = GetTickCount();
    if (now - last < 300) return;   /* a key repeat or a second hook path must not make it flicker */
    last = now;
    BOOL hidden = gForceHide || gHideTgt || gFsTgt || gFsHidden || gHideCur > 0.4f;
    if (hidden) {
        gForceHide = FALSE;
        gHideTgt = 0;
        gFsTgt = 0;
        gFsPeek = TRUE;          /* FsWatch must not slide it away again while it is being used */
        gFsNear = GetTickCount();
        gFsEdgeTick = 0;
        if (gFsHidden) { gFsHidden = FALSE; ShowWindow(g_hwnd, SW_SHOWNA); }
        gSummonTick = GetTickCount();
        gLastNear = gSummonTick;
        if (cfg.mode != 0) SetTimer(g_hwnd, T_HIDE, 120, NULL);
        UpdateGeometry();
        ShowWindow(g_hwnd, SW_SHOWNA);
        SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        if (cfg.bgMode == 2) UpdateFrost();
    } else {
        gForceHide = TRUE;
        gFsPeek = FALSE;
    }
    Kick();
}

static int AddCommon(CmItem *m, int n) {
    if (cfg.mnNotes) n = CmAdd(m, n, IDM_NOTES, L"速记工具", cfg.notes ? CM_CHECK : 0);
    if (cfg.mnAuto) n = CmAdd(m, n, IDM_AUTOSTART, L"开机自启动", GetAutostart() ? CM_CHECK : 0);
    if (cfg.cmMerge) n = CmAdd(m, n, IDM_MULTIMODE, cfg.multiMode ? L"窗口独立显示" : L"窗口合并显示", cfg.multiMode ? CM_CHECK : 0);
    if (cfg.mnTaskMgr) n = CmAdd(m, n, IDM_TASKMGR, L"任务管理器", 0);
    if (cfg.cmDock && cfg.cmIcon) n = CmAdd(m, n, IDM_SETTINGS, L"Dock 设置…", 0);
    n = CmAdd(m, n, 0, L"", CM_SEP);
    n = CmAdd(m, n, IDM_EXIT, L"退出 Dock", CM_RED);
    return n;
}

static void HandleCmd(int id) {
    int i = g_menuItem;
    switch (id) {
    case IDM_OPEN: LaunchItem(i, FALSE); break;
    case IDM_TRAYREMOVE:
        if (i >= 0 && i < nItems && items[i].type == T_TRAY) {
            RemoveTrayApp(TrayAppIndex(items[i].path));
            ScanRunning(); UpdateGeometry(); Kick();
        }
        break;
    case IDM_NEWINST: LaunchItem(i, TRUE); break;
    case IDM_CLOSEWIN: if (i >= 0 && items[i].hwnd) PostMessageW(items[i].hwnd, WM_CLOSE, 0, 0); break;
    case IDM_COPYFILE:
        if (i >= 0 && (items[i].type == T_FILE || items[i].type == T_FOLDER)) {   /* the icon is only a pointer: this copies the real file to the clipboard */
            IShellItem *psi = NULL;
            IDataObject *pdo = NULL;
            if (SUCCEEDED(SHCreateItemFromParsingName(items[i].path, NULL, &IID_IShellItem, (void **)&psi)) && psi) {
                IShellItem_BindToHandler(psi, NULL, &BHID_DataObject, &IID_IDataObject, (void **)&pdo);
                IShellItem_Release(psi);
            }
            if (pdo) { OleSetClipboard(pdo); OleFlushClipboard(); IDataObject_Release(pdo); }
        }
        break;
    case IDM_QUITAPP: if (i >= 0) ActivateItem(&items[i], TRUE); break;
    case IDM_WEBAPP: if (i >= 0) { items[i].appmode = !items[i].appmode; ScheduleSave(); } break;
    case IDM_RENAME:
        if (i >= 0) {
            WCHAR nm[128];
            lstrcpynW(nm, items[i].name, 128);
            if (InputBox(g_hset, L"重命名", L"名称：", nm, 128, NULL, NULL, 0) && nm[0]) {
                lstrcpynW(items[i].name, nm, 128);
                CleanName(items[i].name);
                ScheduleSave();
                Kick();
            }
        }
        break;
    case IDM_REMOVE:
    case IDM_DELSEP: if (i >= 0) RemoveItem(i); UpdateGeometry(); break;
    case IDM_UNGROUP: DissolveGroup(i); UpdateGeometry(); break;
    case IDM_EMPTYBIN: SHEmptyRecycleBinW(g_hwnd, NULL, 0); g_trashFull = -1; SetTimer(g_hwnd, T_SCAN, 500, NULL); break;
    case IDM_PIN:
        if (i >= 0 && items[i].temp) {
            int ts = PinnedCount();
            MoveItem(i, ts);
            items[ts].temp = 0;
            ScheduleSave();
            SetTimer(g_hwnd, T_SCAN, 300, NULL);
            Kick();
        }
        break;
    case IDM_SETICON: if (i >= 0) ShowIconDialog(g_hset, i); break;
    case IDM_NEWSC: DoNewShortcut(g_hset); break;
    case IDM_OPENFOLDER: if (i >= 0) LaunchItem(i, TRUE); break;
    case IDM_ADDSEP_AFTER: DoAddSep(i >= 0 ? i + 1 : -1); break;
    case IDM_ADDGROUP: NewEmptyGroup(FALSE); UpdateGeometry(); Kick(); break;
    case IDM_ADDTEMPGRP: NewEmptyGroup(TRUE); UpdateGeometry(); Kick(); break;
    case IDM_GROUPWIDE:
        if (i >= 0 && items[i].type == T_GROUP) { items[i].appmode ^= 2; BuildGroupIcon(&items[i]); ScheduleSave(); UpdateGeometry(); Kick(); }
        break;
    case IDM_CLEARTEMP:
        if (i >= 0 && items[i].type == T_GROUP) {
            for (int k = 0; k < items[i].nk; k++) if (items[i].kids[k].icon) GdipDisposeImage(items[i].kids[k].icon);
            items[i].nk = 0;
            BuildGroupIcon(&items[i]);
            ScheduleSave();
            Kick();
        }
        break;
    case IDM_TASKMGR: ShellExecuteW(NULL, L"open", L"taskmgr.exe", NULL, NULL, SW_SHOWNORMAL); break;
    case IDM_MULTIMODE: cfg.multiMode = !cfg.multiMode; ApplySettings(); ScheduleSave(); break;
    case IDM_SETTINGS: if (i >= 0 && i < nItems && items[i].type == T_FILE && !lstrcmpW(items[i].path, L"mdock:launchpad")) ShowSettings(4); else ShowSettings(0); break;
    case IDM_NOTES: cfg.notes = !cfg.notes; ApplySettings(); break;
    case IDM_NOTEOFF: cfg.notes = 0; ApplySettings(); break;
    case IDM_AUTOSTART: SetAutostart(!GetAutostart()); break;
    case IDM_EXIT: DestroyWindow(g_hwnd); break;
    }
}

/* a lean right-click menu: item actions on top, a few global entries below */
static void ShowMenuAt(int item, BOOL fromTray) {
    CmItem m[24];
    int n = 0;
    g_menuItem = item;
    if (item >= 0 && !fromTray) {
        Item *it = &items[item];
        if (it->type == T_SEP) {
            if (!it->temp) { n = CmAdd(m, n, IDM_DELSEP, L"删除分割线", 0); n = CmAdd(m, n, 0, L"", CM_SEP); }
        } else if (it->type == T_TRAY) {
            n = CmAdd(m, n, IDM_OPEN, L"显示 / 最小化绑定的主窗口", 0);
            n = CmAdd(m, n, IDM_TRAYREMOVE, L"从托盘常驻名单移除", 0);
            n = CmAdd(m, n, 0, L"", CM_SEP);
        } else if (it->type == T_WIN) {
            n = CmAdd(m, n, IDM_OPEN, L"显示 / 最小化此窗口", 0);
            n = CmAdd(m, n, IDM_CLOSEWIN, L"关闭此窗口", CM_RED);
            n = CmAdd(m, n, 0, L"", CM_SEP);
        } else if (it->type == T_GROUP) {
            n = CmAdd(m, n, IDM_OPEN, L"打开", 0);
            if (cfg.cmRename) n = CmAdd(m, n, IDM_RENAME, L"重命名…", 0);
            if (cfg.cmIcon) n = CmAdd(m, n, IDM_SETICON, L"更改图标…", 0);
            n = CmAdd(m, n, IDM_GROUPWIDE, (it->appmode & 2) ? L"恢复为标准图标" : L"显示为宽图标（8 个缩略图）", 0);
            if (it->appmode & 1) n = CmAdd(m, n, IDM_CLEARTEMP, L"清空临时组", 0);
            n = CmAdd(m, n, IDM_UNGROUP, L"解散应用组", 0);
            if (cfg.cmSep) n = CmAdd(m, n, IDM_ADDSEP_AFTER, L"在右侧添加分割线", 0);
            n = CmAdd(m, n, IDM_REMOVE, L"从 Dock 移除", CM_RED);
            n = CmAdd(m, n, 0, L"", CM_SEP);
        } else if (it->type == T_NOTE) {
            n = CmAdd(m, n, IDM_OPEN, L"打开速记", 0);
            n = CmAdd(m, n, IDM_NOTEOFF, L"关闭速记工具", CM_RED);
            n = CmAdd(m, n, 0, L"", CM_SEP);
        } else if (it->type == T_TRASH) {
            n = CmAdd(m, n, IDM_OPEN, L"打开回收站", 0);
            n = CmAdd(m, n, IDM_EMPTYBIN, L"清空回收站", 0);
            n = CmAdd(m, n, IDM_REMOVE, L"从 Dock 移除", CM_RED);
            n = CmAdd(m, n, 0, L"", CM_SEP);
        } else if (it->type == T_FILE && !lstrcmpW(it->path, L"mdock:launchpad")) {
            n = CmAdd(m, n, IDM_OPEN, L"打开启动台", 0);
            n = CmAdd(m, n, IDM_SETTINGS, L"启动台设置…", 0);
            n = CmAdd(m, n, 0, L"", CM_SEP);
        } else {
            n = CmAdd(m, n, IDM_OPEN, it->running && it->type == T_FILE ? L"显示 / 最小化" : L"打开", 0);
            if (it->type == T_FOLDER) n = CmAdd(m, n, IDM_OPENFOLDER, L"在资源管理器中打开", 0);
            if (it->running && it->target[0]) n = CmAdd(m, n, IDM_QUITAPP, L"关闭（退出此应用）", 0);
            if (!it->temp && (it->type == T_FILE || it->type == T_FOLDER) && wcsncmp(it->path, L"mdock:", 6) != 0 && wcsncmp(it->path, L"shell:", 6) != 0 && !(it->path[0] == L'{'))
                if (cfg.cmCopy) n = CmAdd(m, n, IDM_COPYFILE, L"复制（复制源文件，可粘贴到其他位置）", 0);
            if (it->type == T_URL) n = CmAdd(m, n, IDM_WEBAPP, L"以应用窗口打开", it->appmode ? CM_CHECK : 0);
            if (it->temp) n = CmAdd(m, n, IDM_PIN, L"固定到 Dock", 0);
            else {
                n = CmAdd(m, n, 0, L"", CM_SEP);
                if (cfg.cmRename) n = CmAdd(m, n, IDM_RENAME, L"重命名…", 0);
                if (cfg.cmIcon) n = CmAdd(m, n, IDM_SETICON, L"设置图标…", 0);
                if (cfg.cmSep) n = CmAdd(m, n, IDM_ADDSEP_AFTER, L"在右侧添加分割线", 0);
                n = CmAdd(m, n, IDM_REMOVE, L"从 Dock 移除（不会删除源文件）", CM_RED);
            }
            n = CmAdd(m, n, 0, L"", CM_SEP);
        }
    } else if (cfg.mnAdd) {
        n = CmAdd(m, n, IDM_NEWSC, L"新建快捷方式…", 0);
        n = CmAdd(m, n, IDM_ADDGROUP, L"新建应用组", 0);
        n = CmAdd(m, n, IDM_ADDTEMPGRP, L"新建临时周转组（重启后清空）", 0);
        n = CmAdd(m, n, 0, L"", CM_SEP);
    }
    n = AddCommon(m, n);
    POINT p;
    GetCursorPos(&p);
    gMenuOpen = TRUE;
    ClosePreview();
    int cmd = ShowMenu(m, n, p.x, p.y);
    gMenuOpen = FALSE;
    gLastNear = GetTickCount();
    if (cmd) HandleCmd(cmd);
    g_menuItem = -1;
    Kick();
}

/* ======================= settings window: custom drawn, double buffered ======================= */
enum { R_HDR, R_TOGGLE, R_SLIDER, R_COMBO, R_BTN, R_BTN2, R_COLOR, R_TEXT, R_ITEM, R_STAT, R_LINK };
enum {
    A_NONE = 0, A_AUTOSTART = 1, A_SYS0 = 10 /* .. 15 */, A_ADDURL = 20, A_ADDFILE, A_ADDFOLDER, A_NEWSC, A_NEWGROUP, A_ADDSEP,
    A_EXP0, A_EXP1, A_EXP2, A_IMPORT, A_RESET, A_OPENDIR, A_LOWMODE, A_RECMODE, A_PICKDIR, A_AUTOMATCH, A_LOCCHANGE, A_LOCRESET,
    A_STYLEEXP, A_STYLEIMP, A_OPENNOTE, A_NEWTEMP, A_LPRESET, A_CHECKUPD, A_OPENREPO, A_OPENUPD, A_HIGHMODE, A_ALLWIDGETON, A_ALLWIDGETOFF, A_RESETALL,
    A_HKEDIT, A_GAMEADD, A_PICKDARKDIR, A_DARKMATCH, A_LOGEXPORT = 60, A_LOGCLEAR = 61, A_TRAYADD = 62,
    A_ITEM = 1000, A_DARKITEM = 1300, A_PEXP = 2000, A_PIMP = 2100, A_TRAYREMOVE = 2300
};
typedef struct {
    int type, page, act, act2, mn, mx, nopts, depVal, depNeg, perf;
    const WCHAR *label, *label2, *suffix;
    int *v, *dep;
    const WCHAR **opts;
    float y, h;
    WCHAR dyn[200];
} Row;
typedef struct { int row, page; } Vis;

static Row g_row[620];
static Vis g_vis[620];
static int g_nrow, g_nvis, g_page, g_sbHot = -1, g_rowHot = -1, g_rowDrag = -1, g_scroll;
static HBITMAP g_sbmp, g_sold;
static HDC g_sdc;
static int g_sw, g_sh;
static GpStringFormat *g_fmtW;
static HWND g_search;
static WCHAR g_q[64];
static BOOL g_searching;
static RECT g_backRc;
static BOOL HasTabs(void) { return (g_page == 1 || g_page == 21 || g_page == 4 || g_page == 30) && !g_searching; }

#define SBW 208
#define NPAGE 10
#define NENT 8
#define P_ABOUT 8
static const WCHAR *g_pageName[NPAGE] = { L"通用", L"个性化", L"位置与显示", L"交互动效", L"状态栏与小组件", L"图标自定义", L"性能", L"配置与备份", L"关于", L"右键菜单配置" };
static const int g_ent[NENT] = { 0, 1, 3, 5, 4, 9, 6, 8 };   /* sidebar order */
#define SUBBASE 20
#define NSUB 11
static const struct { int page, parent; const WCHAR *name; } g_sub[NSUB] = {
    { 20, 1, L"背景与渐变" }, { 21, 1, L"图标个性化" }, { 22, 3, L"弧形展开" }, { 23, 3, L"应用组" }, { 24, 4, L"时间与日期样式" },
    { 25, 1, L"倒影设置" }, { 27, 5, L"批量设置图标" }, { 28, 5, L"系统图标" }, { 26, 4, L"速记" }, { 29, 2, L"显示模式" }, { 30, 4, L"状态栏设置" } };
static int ParentOf(int p) { if (p < SUBBASE) return p; for (int i = 0; i < NSUB; i++) if (g_sub[i].page == p) return g_sub[i].parent; return p; }
static const WCHAR *SubName(int p) { for (int i = 0; i < NSUB; i++) if (g_sub[i].page == p) return g_sub[i].name; return L""; }

static WCHAR g_batchDir[MAX_PATH];
static WCHAR g_darkBatchDir[MAX_PATH];
static WCHAR g_match[MAXI][MAX_PATH];
static WCHAR g_darkMatch[MAXI][MAX_PATH];
static int g_mstate[MAXI], g_darkState[MAXI];

static const WCHAR *o_theme[3] = { L"深色", L"浅色", L"跟随系统" };
static const WCHAR *o_uitheme[3] = { L"浅色", L"深色", L"跟随系统" };
static const WCHAR *o_bg[3] = { L"纯色", L"渐变（Material You）", L"磨砂玻璃（实时模糊）" };
static const WCHAR *o_edge[4] = { L"屏幕底部", L"屏幕顶部", L"屏幕左侧", L"屏幕右侧" };
static const WCHAR *o_mode[3] = { L"常驻显示", L"自动隐藏", L"智能隐藏（被窗口遮挡时隐藏）" };
static const WCHAR *o_plate[3] = { L"无底盘", L"纯色底盘", L"毛玻璃底盘" };
static const WCHAR *o_plateTheme[3] = { L"浅色", L"深色", L"跟随 Dock 主题" };
static const WCHAR *o_dot[2] = { L"横条", L"圆点" };
static const WCHAR *o_multi[2] = { L"合并显示（悬停预览窗口）", L"独立显示（每个文档一个图标）" };
static const WCHAR *o_wside[2] = { L"Dock 右侧（默认）", L"Dock 左侧" };
static const WCHAR *o_worder[2] = { L"时间与日期在前，通知在后", L"通知在前，时间与日期在后" };
static const WCHAR *o_browser[5] = { L"自动（Edge 优先）", L"Microsoft Edge", L"Google Chrome", L"Mozilla Firefox", L"系统默认浏览器（不使用应用窗口）" };
static const WCHAR *o_minstyle[] = { L"经典压缩飞入", L"经典图标飞入", L"流畅压缩（支持 HDR）" };
static const WCHAR *o_corder[2] = { L"时间在上，日期在下", L"日期在上，时间在下" };
static const WCHAR *o_dfmt[3] = { L"2026年10月22日", L"2026/10/22", L"2026-10-22" };
static const WCHAR *o_reflmode[2] = { L"倒影在 Dock 栏外（下方）", L"倒影在 Dock 栏内（不超出栏）" };
static const WCHAR *o_reflgrad[3] = { L"平滑淡出", L"线性渐变", L"均匀半透明" };
static const WCHAR *o_darkmode[3] = { L"关闭", L"跟随系统深色模式", L"按时间段" };
static const WCHAR *o_taskhide[3] = { L"关闭隐藏（正常显示任务栏）", L"自动隐藏（鼠标滑过仍显示）", L"永久隐藏（鼠标滑过不显示）" };
static const WCHAR *o_clock[3] = { L"时间 + 日期（上下分栏）", L"仅显示时间", L"仅显示日期" };
static const WCHAR *o_cut[2] = { L"复制", L"剪切（移动）" };
static const WCHAR *o_gpos[2] = { L"Dock 上方", L"屏幕中央" };
static WCHAR o_monN[8][24];
static const WCHAR *o_mon[8];
static int g_nmonOpt;

/* ---- row builders ---- */
static Row *NewRow(int page, int type, const WCHAR *label, int h96) {
    Row *r = &g_row[g_nrow++];
    ZeroMemory(r, sizeof *r);
    r->page = page; r->type = type; r->label = label; r->h = (float)Sc(h96);
    return r;
}
static Row *LastRow(void) { return &g_row[g_nrow - 1]; }
static void RHdr(int p, const WCHAR *t) { NewRow(p, R_HDR, t, 40); }
static void RTog(int p, const WCHAR *t, int *v, const WCHAR *note) { Row *r = NewRow(p, R_TOGGLE, t, note ? 58 : 48); r->v = v; r->label2 = note; }
static void RTogA(int p, const WCHAR *t, int act, const WCHAR *note) { Row *r = NewRow(p, R_TOGGLE, t, note ? 58 : 48); r->act = act; r->label2 = note; }
static void RSl(int p, const WCHAR *t, int *v, int mn, int mx, const WCHAR *suf) { Row *r = NewRow(p, R_SLIDER, t, 50); r->v = v; r->mn = mn; r->mx = mx; r->suffix = suf; }
static void RCmb(int p, const WCHAR *t, int *v, const WCHAR **o, int n) { Row *r = NewRow(p, R_COMBO, t, 52); r->v = v; r->opts = o; r->nopts = n; }
static void RBtn(int p, const WCHAR *t, int act) { Row *r = NewRow(p, R_BTN, t, 48); r->act = act; }
static void RBtn2(int p, const WCHAR *t1, int a1, const WCHAR *t2, int a2) { Row *r = NewRow(p, R_BTN2, t1, 52); r->act = a1; r->label2 = t2; r->act2 = a2; }
static void RCol(int p, const WCHAR *t, int *v) { Row *r = NewRow(p, R_COLOR, t, 50); r->v = v; }
static void RTxt(int p, const WCHAR *t, int h96) { NewRow(p, R_TEXT, t, h96); }
static void RLink(int p, const WCHAR *t, int target, const WCHAR *note) { Row *r = NewRow(p, R_LINK, t, note ? 58 : 48); r->mn = target; r->label2 = note; }
static void Dep(int *d, int val, int neg) { Row *r = LastRow(); r->dep = d; r->depVal = val; r->depNeg = neg; }
static void Perf(int lv) { LastRow()->perf = lv; }
static void PageFooter(int p) { RHdr(p, L"本页设置"); RBtn2(p, L"导出本页设置…", A_PEXP + p, L"导入本页设置…", A_PIMP + p); }

static BOOL GetSpecial(int act) {
    if (act == A_AUTOSTART) return GetAutostart();
    if (act >= A_SYS0 && act < A_SYS0 + SYS_N) return SysFind(act - A_SYS0) >= 0;
    return FALSE;
}
static void SetSpecial(int act, BOOL on) {
    if (act == A_AUTOSTART) SetAutostart(on);
    else if (act >= A_SYS0 && act < A_SYS0 + SYS_N) { if (on) SysAdd(act - A_SYS0); else SysRemove(act - A_SYS0); UpdateGeometry(); Kick(); }
}

/* ---- batch icon replacement ---- */
static void NormName(const WCHAR *s, WCHAR *o) {
    int n = 0;
    for (; *s && n < 120; s++) if (*s != L' ' && *s != L'_' && *s != L'-' && *s != L'.') o[n++] = *s;
    o[n] = 0;
    CharLowerW(o);
}
static BOOL IsImgExt(const WCHAR *f) { return PathMatchSpecW(f, L"*.png;*.ico;*.jpg;*.jpeg;*.bmp;*.gif"); }

static void RunMatch(void) {
    for (int i = 0; i < MAXI; i++) { g_match[i][0] = 0; g_mstate[i] = 0; }
    if (!g_batchDir[0]) return;
    WCHAR pat[MAX_PATH];
    PathCombineW(pat, g_batchDir, L"*");
    for (int i = 0; i < nItems; i++) {
        if (items[i].type == T_SEP || items[i].temp) continue;
        WCHAR nn[130], fn[130];
        NormName(items[i].name, nn);
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW(pat, &fd);
        int best = 0;
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || !IsImgExt(fd.cFileName)) continue;
                WCHAR stem[MAX_PATH];
                lstrcpynW(stem, fd.cFileName, MAX_PATH);
                PathRemoveExtensionW(stem);
                NormName(stem, fn);
                int sc = 0;
                if (!lstrcmpW(fn, nn)) sc = 3;
                else if (nn[0] && fn[0] && (wcsstr(fn, nn) || wcsstr(nn, fn)) && lstrlenW(fn) >= 2) sc = 1;
                if (sc > best) { best = sc; PathCombineW(g_match[i], g_batchDir, fd.cFileName); }
            } while (FindNextFileW(h, &fd));
            FindClose(h);
        }
        g_mstate[i] = best ? 1 : 2;
    }
}

static void RunDarkMatch(void) {
    for (int i = 0; i < MAXI; i++) { g_darkMatch[i][0] = 0; g_darkState[i] = 0; }
    if (!g_darkBatchDir[0]) return;
    WCHAR pat[MAX_PATH];
    PathCombineW(pat, g_darkBatchDir, L"*");
    for (int i = 0; i < nItems; i++) {
        if (items[i].type == T_SEP || items[i].temp) continue;
        WCHAR nn[130], fn[130];
        NormName(items[i].name, nn);
        WIN32_FIND_DATAW fd;
        HANDLE h = FindFirstFileW(pat, &fd);
        int best = 0;
        if (h != INVALID_HANDLE_VALUE) {
            do {
                if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) || !IsImgExt(fd.cFileName)) continue;
                WCHAR stem[MAX_PATH];
                lstrcpynW(stem, fd.cFileName, MAX_PATH);
                PathRemoveExtensionW(stem);
                NormName(stem, fn);
                int score = (!lstrcmpW(fn, nn)) ? 3 : ((nn[0] && fn[0] && (wcsstr(fn, nn) || wcsstr(nn, fn)) && lstrlenW(fn) >= 2) ? 1 : 0);
                if (score > best) { best = score; PathCombineW(g_darkMatch[i], g_darkBatchDir, fd.cFileName); }
            } while (FindNextFileW(h, &fd));
            FindClose(h);
        }
        g_darkState[i] = best ? 1 : 2;
    }
}

static BOOL ApplyIconFile(int i, const WCHAR *file) {
    WCHAR dst[MAX_PATH], nm[64];
    wsprintfW(nm, L"c_%08x%s", Fnv(file) ^ GetTickCount() ^ (i * 7919), PathFindExtensionW(file));
    PathCombineW(dst, g_iconDir, nm);
    if (!CopyFileW(file, dst, FALSE)) return FALSE;
    lstrcpynW(items[i].icon_src, dst, 512);
    items[i].fit = 0;
    LoadItemIcon(&items[i]);
    return TRUE;
}

static BOOL ApplyDarkIconFile(int i, const WCHAR *file) {
    WCHAR dst[MAX_PATH], nm[64];
    if (i < 0 || i >= nItems || !file || !file[0]) return FALSE;
    wsprintfW(nm, L"d_%08x%s", Fnv(file) ^ GetTickCount() ^ (i * 6151), PathFindExtensionW(file));
    PathCombineW(dst, g_iconDir, nm);
    if (!CopyFileW(file, dst, FALSE)) return FALSE;
    lstrcpynW(items[i].icon_dark, dst, 512);
    LoadItemIcon(&items[i]);
    return TRUE;
}

/* ---- build all rows (the information architecture lives here) ---- */
static void BuildRows(void) {
    g_nrow = 0;
    GetMon();
    g_nmonOpt = g_nmon < 1 ? 1 : g_nmon;
    for (int i = 0; i < g_nmonOpt; i++) { wsprintfW(o_monN[i], i == 0 ? L"主显示器" : L"显示器 %d", i + 1); o_mon[i] = o_monN[i]; }
    int P;

    P = 0;   /* ---- 通用 ---- */
    RHdr(P, L"启动");
    RTogA(P, L"开机自启动", A_AUTOSTART, NULL);
    RTog(P, L"锁定 Dock 布局", &cfg.lock, L"防止误拖动、误移除图标");
    RTog(P, L"显示未固定的运行中应用", &cfg.showRunning, L"在分割线之后显示正在运行的应用，可拖入固定"); Perf(2);
    RTog(P, L"分组中的运行应用也在右侧显示", &cfg.groupRunning, L"需开启上方的运行中应用显示。保留组内快捷方式，点击右侧图标恢复已有窗口");
    RTog(P, L"全局快捷键", &cfg.hotkey, L"随时显示 / 隐藏 Dock，快捷键可以自定义");
    { WCHAR hk[96], lbl[128]; HkText(hk, 96); wsprintfW(lbl, L"修改快捷键（当前：%s）", hk); static WCHAR hkl[160]; lstrcpyW(hkl, lbl); RBtn(P, hkl, A_HKEDIT); }
    RTog(P, L"防止被清理工具关闭", &cfg.guardian, L"启动一个极小的守护进程（约 1 MB），被结束后自动重新打开"); Perf(1);
    RTog(P, L"隐藏系统托盘图标", &cfg.trayHide, L"打开后右下角通知区域也不再显示 MiniDock 图标（仍可用快捷键或右键 Dock 打开设置）");
    RTog(P, L"右键菜单显示“任务管理器”", &cfg.mnTaskMgr, NULL);
    RHdr(P, L"托盘应用常驻");
    RTxt(P, L"自选应用运行时显示在 Dock 最右侧，隐藏到系统托盘后保留，退出后移除。请打开应用主窗口后添加；点击右侧图标可恢复该窗口。", 72);
    RBtn(P, L"从已打开的应用窗口添加…", A_TRAYADD);
    for (int i = 0; i < g_ntrayApps; i++) {
        RBtn(P, L"", A_TRAYREMOVE + i);
        Row *r = LastRow();
        _snwprintf(r->dyn, 199, L"移除 %s（%s）", g_trayApps[i].name, PathFindFileNameW(g_trayApps[i].exe));
        r->dyn[199] = 0;
        r->label = r->dyn;
    }
    RHdr(P, L"网页应用");
    RTog(P, L"网页默认以应用窗口打开", &cfg.webApp, L"使用 Edge / Chrome 的独立窗口模式，不再显示为浏览器标签页");
    RCmb(P, L"网页使用的浏览器", &cfg.browser, o_browser, 5);
    RHdr(P, L"任务栏");
    RCmb(P, L"隐藏系统任务栏", &cfg.taskHide, o_taskhide, 3);
    RTxt(P, L"任务栏隐藏仅在 MiniDock 运行时生效，正常退出后恢复 Windows 任务栏。", 40);
    RTog(P, L"任务栏显示在屏幕顶部", &cfg.taskTop, L"把系统任务栏移到屏幕顶部；需要重启资源管理器生效，且不能与顶部状态栏同时使用"); Dep(&cfg.statusBar, 1, 1);
    RTog(P, L"Ctrl + 悬停临时显示任务栏", &cfg.taskPeek, L"仅对“永久隐藏”有效：按住 Ctrl 把鼠标移到任务栏边缘时临时显示"); Dep(&cfg.taskHide, 2, 0);
    RTog(P, L"任务栏透明", &cfg.taskTrans, L"让任务栏底板半透明；关闭后恢复系统原样"); Dep(&cfg.taskHide, 0, 0);
    RSl(P, L"任务栏不透明度", &cfg.taskAlpha, 20, 100, L" %"); Dep(&cfg.taskTrans, 1, 0);
    RHdr(P, L"界面主题");
    RCmb(P, L"设置窗口主题", &cfg.uiTheme, o_uitheme, 3);
    RCmb(P, L"右键菜单主题", &cfg.menuTheme, o_uitheme, 3);
    RTog(P, L"右键菜单毛玻璃", &cfg.menuFrost, L"右键 Dock / 状态栏 / 托盘图标弹出的菜单使用磨砂玻璃背景"); Perf(1);
    PageFooter(P);

    P = 1;   /* ---- 外观 ---- */
    RHdr(P, L"尺寸");
    RSl(P, L"图标大小", &cfg.iconSize, 24, 128, L" px");
    RSl(P, L"图标间距", &cfg.spacing, 0, 30, L" px");
    RSl(P, L"栏内边距（栏高）", &cfg.pad, 2, 30, L" px");
    RSl(P, L"图标距左右边缘", &cfg.hPad, 0, 40, L" px");
    RTog(P, L"分割线上下拖动等比缩放 Dock", &cfg.sepScale, L"关闭后悬停分割线不会进入缩放，仍可拖动排序");
    RSl(P, L"栏圆角", &cfg.radius, 0, 40, L" px");
    RHdr(P, L"主题与背景");
    RCmb(P, L"Dock 主题", &cfg.theme, o_theme, 3);
    RCmb(P, L"背景材质", &cfg.bgMode, o_bg, 3);
    RSl(P, L"不透明度（0 = 无背景，图标悬浮）", &cfg.opacity, 0, 100, L" %");
    RLink(P, L"背景与渐变设置", 20, L"颜色、渐变方向、单个颜色的不透明度、跟随系统强调色");
    RHdr(P, L"栏边框与分割线");
    RTog(P, L"显示栏边框", &cfg.border, NULL);
    RCol(P, L"边框颜色", &cfg.borderColor); Dep(&cfg.border, 1, 0);
    RSl(P, L"边框粗细", &cfg.borderWidth, 1, 6, L" px"); Dep(&cfg.border, 1, 0);
    RHdr(P, L"图标倒影");
    RTog(P, L"显示图标倒影", &cfg.reflect, L"仅底部停靠时有效；在下方可调倒影长度、间距与浓度"); Perf(1);
    RLink(P, L"倒影设置", 25, NULL); Dep(&cfg.reflect, 1, 0);

    P = 20;  /* ---- 背景与渐变 ---- */
    RTxt(P, L"磨砂玻璃使用系统实时模糊（占用极低），无需调节；纯色 / 渐变可在下面设置颜色。", 36); Dep(&cfg.bgMode, 2, 0);
    RHdr(P, L"颜色");
    RTog(P, L"跟随系统强调色", &cfg.accentTone, L"根据 Windows 强调色生成 Material You 风格色调"); Dep(&cfg.bgMode, 2, 1);
    RCol(P, L"颜色 1", &cfg.bgColor1); Dep(&cfg.bgMode, 2, 1);
    RSl(P, L"颜色 1 不透明度", &cfg.gradA1, 0, 100, L" %"); Dep(&cfg.bgMode, 2, 1);
    RCol(P, L"颜色 2（渐变终点）", &cfg.bgColor2); Dep(&cfg.bgMode, 1, 0);
    RSl(P, L"颜色 2 不透明度", &cfg.gradA2, 0, 100, L" %"); Dep(&cfg.bgMode, 1, 0);
    RCol(P, L"颜色 3（可选，渐变中间色）", &cfg.bgColor3); Dep(&cfg.bgMode, 1, 0);
    RSl(P, L"颜色 3 不透明度", &cfg.gradA3, 0, 100, L" %"); Dep(&cfg.bgMode, 1, 0);
    RSl(P, L"渐变方向", &cfg.gradAngle, 0, 360, L"°"); Dep(&cfg.bgMode, 1, 0);
    RHdr(P, L"兼容模式");
    RSl(P, L"模糊强度（仅系统不支持实时模糊时使用）", &cfg.blur, 1, 14, L""); Dep(&cfg.bgMode, 2, 0);
    RSl(P, L"场景切换后毛玻璃刷新间隔", &cfg.frostHold, 1, 10, L" 秒");

    P = 21;  /* ---- 图标样式 ---- */
    RHdr(P, L"图标底盘（macOS 风格圆角方块）");
    RCmb(P, L"底盘类型", &cfg.plateMode, o_plate, 3);
    RCmb(P, L"底盘配色", &cfg.plateTheme, o_plateTheme, 3); Dep(&cfg.plateMode, 0, 1);
    RSl(P, L"底盘不透明度", &cfg.plateOpacity, 0, 100, L" %"); Dep(&cfg.plateMode, 0, 1);
    RSl(P, L"底盘圆角（0 方形 · 22 圆角方块 · 50 圆形）", &cfg.plateRadius, 0, 50, L" %"); Dep(&cfg.plateMode, 0, 1);
    RHdr(P, L"分割线");
    RCol(P, L"分割线颜色", &cfg.sepColor);
    RSl(P, L"分割线两侧图标间距", &cfg.sepGap, 0, 40, L" px");
    RSl(P, L"分割线粗细", &cfg.sepWidth, 1, 6, L" px");
    RTxt(P, L"间距越小，分割线两边的图标越靠近；分割线的可点击范围已经加大。", 44);
    RHdr(P, L"运行中指示");
    RTog(P, L"显示运行中指示", &cfg.dots, L"需要定时扫描窗口"); Perf(2);
    RCmb(P, L"指示样式", &cfg.dotStyle, o_dot, 2); Dep(&cfg.dots, 1, 0);
    RCol(P, L"指示颜色", &cfg.dotColor); Dep(&cfg.dots, 1, 0);
    RHdr(P, L"名称标签");
    RTog(P, L"悬停显示名称标签", &cfg.labels, NULL);
    RHdr(P, L"深色模式图标");
    RCmb(P, L"图标深色版本", &cfg.iconDarkMode, o_darkmode, 3);
    RSl(P, L"深色开始（分钟，19:00 = 1140）", &cfg.iconDarkFrom, 0, 1439, L""); Dep(&cfg.iconDarkMode, 2, 2);
    RSl(P, L"深色结束（分钟，07:00 = 420）", &cfg.iconDarkTo, 0, 1439, L""); Dep(&cfg.iconDarkMode, 2, 2);
    RTxt(P, L"在“图标自定义 → 批量设置图标”中点击任意应用，可以单独为它选择深色图标；没有深色图标的程序继续使用原图标。", 44);

    P = 1;   /* ---- Dock 位置与显示（并入“个性化”） ---- */
    RHdr(P, L"位置");
    RCmb(P, L"Dock 位置（居中）", &cfg.edge, o_edge, 4);
    RCmb(P, L"显示在", &cfg.monitor, o_mon, g_nmonOpt);
    RSl(P, L"距屏幕边缘", &cfg.margin, 0, 150, L" px");
    RHdr(P, L"显示方式");
    RCmb(P, L"显示模式", &cfg.mode, o_mode, 3);
    RTog(P, L"最大化窗口避让 Dock", &cfg.reserve, L"像 macOS 一样为 Dock 预留桌面空间"); Dep(&cfg.mode, 0, 0);
    PageFooter(P);

    P = 3;   /* ---- 动画与交互 ---- */
    RHdr(P, L"悬停");
    RTog(P, L"悬停放大图标", &cfg.zoom, NULL);
    RSl(P, L"放大后大小", &cfg.zoomSize, 32, 220, L" px"); Dep(&cfg.zoom, 1, 0);
    RTog(P, L"悬停运行中的应用显示窗口预览", &cfg.preview, L"使用系统缩略图，仅悬停时占用"); Perf(1);
    RTog(P, L"多窗口应用悬停显示窗口列表", &cfg.pvList, L"开启后，同一应用多个窗口时悬停弹出窗口列表（点击切换）；关闭则平铺缩略图"); Dep(&cfg.preview, 1, 0);
    RHdr(P, L"窗口缩放");
    RTog(P, L"再次点击前台应用时最小化", &cfg.clickToggle, L"开启后再次点击前台应用的 Dock 图标会将它最小化。窗口恢复动画由下面的动画开关控制");
    RTog(P, L"启动应用时图标弹跳", &cfg.bounce, NULL);
    RTog(P, L"最小化 / 还原动画（飞入飞出 Dock）", &cfg.minAnim, L"窗口缩入图标，点击图标还原时从图标展开，并取消系统的最小化动画"); Perf(1);
    RSl(P, L"飞入飞出动画时长", &cfg.animSpeed, 200, 1000, L" ms"); Dep(&cfg.minAnim, 1, 0);
    RCmb(P, L"动画风格", &cfg.minStyle, o_minstyle, ARRAYSIZE(o_minstyle)); Dep(&cfg.minAnim, 1, 0);
    RTxt(P, L"两种经典风格沿用原版窗口动画，开销较低。流畅压缩使用合成预览，HDR 下会抓取快照以减少恢复闪烁和泛白，使用更多内存和显存。", 60);
    RHdr(P, L"起动台");
    RSl(P, L"每页列数", &cfg.lpCols, 3, 12, L" 列");
    RSl(P, L"每页行数", &cfg.lpRows, 2, 8, L" 行");
    RTog(P, L"起动台磨砂玻璃背景", &cfg.lpFrost, L"开启后背景为模糊桌面；关闭可降低资源占用"); Perf(1);
    RSl(P, L"启动台背景不透明度", &cfg.lpOpacity, 0, 100, L" %");
    RTog(P, L"允许启动台标签切换", &cfg.lpTabs, L"关闭后启动台固定显示全部应用，不再显示顶部分类标签");
    RTxt(P, L"起动台左右滑动翻页（鼠标拖动 / 滚轮 / 方向键）。拖动图标可以调整位置，拖到另一个图标上会合并成文件夹（点开文件夹可重命名，右键里可移出），拖到屏幕左右边缘会翻页；右上角“＋”可添加自定义的应用、文件、网址。", 72);
    RBtn(P, L"重置起动台布局", A_LPRESET);
    RHdr(P, L"文件夹与应用组");
    RTog(P, L"点击文件夹弧形展开", &cfg.folderFan, L"像 macOS 一样列出最近的文件，点击即可打开");
    RLink(P, L"弧形展开设置", 22, NULL); Dep(&cfg.folderFan, 1, 0);
    RLink(P, L"应用组设置", 23, NULL);
    RHdr(P, L"多窗口应用");
    RCmb(P, L"同一应用打开多个文档 / 窗口时", &cfg.multiMode, o_multi, 2); Perf(1);
    RTxt(P, L"合并显示：只显示一个应用图标，悬停弹出各窗口预览（可单独关闭）。独立显示（类似 macOS）：每个 Word / Excel / PowerPoint 文档和每个资源管理器窗口各占一个 Dock 图标，可单独切换、最小化、关闭。", 72);
    RHdr(P, L"游戏模式");
    RTog(P, L"游戏模式防误触", &cfg.gameGuard, L"游戏运行时，鼠标滑到屏幕边缘也不会弹出 Dock，避免误触");
    RTog(P, L"自动识别全屏游戏", &cfg.gameAuto, L"独占全屏（Direct3D）游戏自动生效；其它游戏可手动加入列表");
    RBtn(P, L"将当前前台程序加入 / 移出游戏列表", A_GAMEADD);
    RTxt(P, L"点击可将前台程序加入或移出游戏名单。顶部状态栏在游戏全屏或最大化时自动隐藏；游戏模式开关单独控制 Dock 防误触。", 38);
    PageFooter(P);

    P = 22;  /* ---- 弧形展开 ---- */
    RSl(P, L"弧度（0 = 垂直展开）", &cfg.fanArc, 0, 100, L"");
    RTog(P, L"反方向展开", &cfg.fanReverse, L"反转文件夹弧形展开的弯曲方向");
    RSl(P, L"纵向间隔", &cfg.fanGap, 0, 40, L" px");
    RTog(P, L"允许把文件从弧形展开中拖出", &cfg.fanDrag, L"关闭后，点击文件只会打开它，不会误触拖拽");
    RCmb(P, L"拖出文件时", &cfg.fanCut, o_cut, 2); Dep(&cfg.fanDrag, 1, 0);
    RHdr(P, L"文件名标签颜色");
    RCol(P, L"浅色模式：标签背景", &cfg.fanBgL);
    RCol(P, L"浅色模式：字体颜色", &cfg.fanFgL);
    RCol(P, L"深色模式：标签背景", &cfg.fanBgD);
    RCol(P, L"深色模式：字体颜色", &cfg.fanFgD);
    RTxt(P, L"展开后点击空白处关闭；可把图标拖出到桌面或资源管理器，右键图标可打开、定位、复制路径或移到回收站。", 44);

    P = 23;  /* ---- 应用组 ---- */
    RCmb(P, L"打开位置", &cfg.groupPos, o_gpos, 2);
    RTog(P, L"应用组独立于 Dock 显示", &cfg.grpFloat, L"开启后所有应用组在 Dock 之外单独排成一条小栏，Dock 自动隐藏时也能直接点开"); Perf(1);
    RTog(P, L"应用组毛玻璃背景", &cfg.groupFrost, L"使用系统实时模糊；如影响性能可关闭"); Perf(1);
    RSl(P, L"背景不透明度", &cfg.grpOpacity, 20, 100, L" %");
    RCol(P, L"背景颜色（自动跟随主题）", &cfg.grpBg);
    RTxt(P, L"把一个图标拖到另一个图标上，停留片刻即可合成应用组；组内图标可拖动排序，末尾的“＋”可添加任意位置的文件 / 文件夹 / 网址，右键可移除或新建快捷方式；组内保存的是位置链接，移除不会删除源文件。右键组图标可更改图标。“临时周转组”重启或关机后自动清空，可把文件从组里拖出到别处。", 96);

    P = 4;   /* ---- 小组件 ---- */
    RBtn2(P, L"一键全部开启", A_ALLWIDGETON, L"一键全部关闭", A_ALLWIDGETOFF);
    RHdr(P, L"小组件布局");
    RTxt(P, L"Dock 停靠在左侧或右侧时，小组件会收纳为一个 1×1 入口；点击入口可查看并打开所有已启用的小组件。", 42);
    RTog(P, L"合并小组件为一个胶囊", &cfg.wMerge, L"开启：所有小组件放进一个圆角胶囊；关闭：每个小组件一个独立图标，之间留间隔"); Perf(1);
    RTog(P, L"macOS 式顶部状态栏", &cfg.statusBar, L"左侧 MiniDock 设置、右侧时间/日期/控制中心/托盘/CPU/电量/WiFi；开启后 Dock 不再显示这些小组件"); Perf(1);
    RLink(P, L"状态栏设置", 30, NULL); Dep(&cfg.statusBar, 1, 0);
    RCmb(P, L"小组件位置", &cfg.wSide, o_wside, 2);
    RCmb(P, L"小组件顺序", &cfg.wOrder, o_worder, 2);
    RTxt(P, L"点击“时间与日期”会在其上方弹出日历（支持切换年月、定位今天、显示农历和干支历）；右键小组件可以关闭它、移动到另一侧或交换顺序。", 44);
    RHdr(P, L"通知小组件");
    RTog(P, L"在 Dock 上显示通知小组件", &cfg.notifyW, L"收集微信、QQ、Telegram、邮件等应用的任务栏闪烁 / 未读提醒，并在对应图标上显示红点"); Perf(1);
    RTxt(P, L"说明：系统自带的通知弹窗内容无法被第三方程序读取，所以这里接管的是“应用请求你注意”的提醒，点击即可跳到对应窗口。", 44);
    RHdr(P, L"控制中心");
    RTog(P, L"在 Dock 上显示控制中心", &cfg.ctrlCenter, L"WiFi、蓝牙、深色模式、定位、静音、设置，外加音量与电量"); Perf(1);
    RTog(P, L"在 Dock 上显示资源监视器", &cfg.resMon, L"CPU / 内存 / 磁盘占用，点击展开详情"); Perf(1);
    RTog(P, L"在 Dock 上显示专注模式切换", &cfg.modeW, L"一键切换不同的 Dock 布局（工作 / 开发 / 娱乐…），整套配置分别保存"); Perf(1);
    RTog(P, L"在 Dock 上显示系统托盘", &cfg.trayW, L"点击打开 Windows 系统托盘（隐藏图标弹出），任务栏隐藏时也能用"); Perf(1);
    RHdr(P, L"时间与日期");
    RTog(P, L"在 Dock 右侧显示时间与日期", &cfg.clock, L"每 20 秒刷新一次；显示秒数时每秒刷新"); Perf(1);
    RLink(P, L"时间与日期样式", 24, NULL); Dep(&cfg.clock, 1, 0);
    RHdr(P, L"小工具");
    RTog(P, L"速记", &cfg.notes, L"在 Dock 上添加“速记”图标：富文本、临时保存、导出 txt");
    RBtn(P, L"打开速记", A_OPENNOTE); Dep(&cfg.notes, 1, 0);
    RLink(P, L"速记说明", 26, NULL); Dep(&cfg.notes, 1, 0);
    RTog(P, L"计时器 / 待办", &cfg.toolW, L"倒计时、秒表与待办清单，待办会自动保存"); Perf(1);
    RTog(P, L"电源小组件（关机 / 重启 / 锁屏）", &cfg.powerW, L"一个 1×1 的电源图标，点击后在小框内选择关机、重启或锁屏");
    PageFooter(P);

    P = 30;  /* ---- 状态栏设置 ---- */
    RTog(P, L"macOS 式顶部状态栏", &cfg.statusBar, L"开启后左侧为 MiniDock 菜单，右侧为时间 / 控制中心 / 托盘 / 电量等"); Perf(1);
    RTxt(P, L"开启顶部状态栏后，这里会显示它全部的样式、位置与显示项配置。", 40); Dep(&cfg.statusBar, 0, 0);
    int sbCfgStart = g_nrow;
    RTxt(P, L"视频或游戏进入真正全屏时隐藏，退出全屏后恢复。浏览器、播放器和游戏普通打开或窗口最大化时均保持显示；其他游戏可加入游戏名单。", 64);
    RTog(P, L"全屏视频/游戏时隐藏", &cfg.sbFullscreenHide, L"仅视频或游戏真正全屏时隐藏；普通窗口和窗口最大化时始终显示，退出全屏后恢复");
    RTog(P, L"弹窗毛玻璃", &cfg.sbPopFrost, L"控制中心 / 通知 / 时间日期 / MiniDock 菜单等弹窗使用磨砂玻璃；关闭后为纯色卡片");
    RSl(P, L"毛玻璃模糊程度", &cfg.sbPopBlur, 0, 100, L" %"); Dep(&cfg.sbPopFrost, 1, 0);
    RTog(P, L"磨砂底盘", &cfg.sbFrost, L"开启：半透明磨砂底色；关闭：纯色底盘"); Perf(1);
    RCol(P, L"纯色底盘颜色（关闭磨砂时生效）", &cfg.sbColor);
    RSl(P, L"不透明度", &cfg.sbOpacity, 0, 100, L" %");
    RSl(P, L"圆角", &cfg.sbRadius, 0, 24, L" px");
    RTog(P, L"显示边框线", &cfg.sbBorder, L"关闭后状态栏不再描一圈细边框，和屏幕边缘更干净地融合");
    RTog(P, L"电池小组件", &cfg.sbBattery, L"关闭后顶部状态栏不再显示电池图标与电量");
    RSl(P, L"高度", &cfg.sbHeight, 22, 48, L" px");
    RSl(P, L"距屏幕顶部距离", &cfg.sbTopGap, 0, 80, L" px");
    RTog(P, L"跟随系统深色模式", &cfg.sbFollowSys, L"关闭则跟随 Dock 的主题");
    RTog(P, L"悬浮状态栏", &cfg.sbFloat, L"开启后状态栏两侧留边、独立圆角，不与屏幕边缘贴合");
    RSl(P, L"左右边距（悬浮时）", &cfg.sbSideGap, 0, 80, L" px"); Dep(&cfg.sbFloat, 1, 0);
    RTog(P, L"桌面图标与最大化窗口避让", &cfg.sbMaxAvoid, L"开启后状态栏像任务栏一样占据顶部，桌面图标与最大化窗口自动下移"); Perf(1);
    RTog(P, L"最大化窗口时状态栏不透明（模糊/纯色）", &cfg.sbAutoOpaque, L"窗口最大化时状态栏变成不透明，避免透明条压住窗口内容；取消最大化后恢复透明");
    RTxt(P, L"顶部状态栏与「任务栏置顶」不要同时开启：任务栏在顶部时状态栏会自动排到任务栏下方，若两者同时置顶会互相遮挡。", 56);
    for (int i = sbCfgStart; i < g_nrow; i++) { g_row[i].dep = &cfg.statusBar; g_row[i].depVal = 1; g_row[i].depNeg = 0; }

    P = 24;  /* ---- 时间与日期样式 ---- */
    RCmb(P, L"显示内容", &cfg.clockShow, o_clock, 3);
    RCmb(P, L"排列顺序（同时显示时）", &cfg.clockOrder, o_corder, 2); Dep(&cfg.clockShow, 0, 0);
    RTog(P, L"时间显示秒数", &cfg.clockSec, L"每秒刷新一次，占用略增"); Dep(&cfg.clockShow, 2, 1); Perf(2);
    RCmb(P, L"日期格式", &cfg.dateFmt, o_dfmt, 3); Dep(&cfg.clockShow, 1, 1);
    RTog(P, L"日期显示周几", &cfg.clockWeek, NULL); Dep(&cfg.clockShow, 1, 1);
    RTog(P, L"时间加粗", &cfg.clockBoldT, NULL); Dep(&cfg.clockShow, 2, 1);
    RTog(P, L"日期加粗", &cfg.clockBoldD, NULL); Dep(&cfg.clockShow, 1, 1);
    RCol(P, L"浅色模式字体颜色", &cfg.clockColLight);
    RCol(P, L"深色模式字体颜色", &cfg.clockColDark);

    P = 25;  /* ---- 倒影设置 ---- */
    RCmb(P, L"倒影位置", &cfg.reflMode, o_reflmode, 2);
    RCmb(P, L"渐隐方式", &cfg.reflGrad, o_reflgrad, 3);
    RSl(P, L"倒影大小", &cfg.reflSize, 10, 80, L" %");
    RSl(P, L"与 Dock 的距离", &cfg.reflGap, 0, 30, L" px");
    RSl(P, L"倒影不透明度", &cfg.reflOpacity, 0, 100, L" %");

    P = 26;  /* ---- 速记 ---- */
    RTxt(P, L"点击 Dock 上的“速记”图标打开：左侧是已临时保存的速记列表，点击即可预览与修改；支持加粗、倾斜和项目符号；“临时保存”只保存在速记里，可随时删除，也可导出为 txt（导出位置可选桌面、文档、下载或自定义文件夹）。关闭窗口时未保存的内容会自动临时保存。", 130);

    P = 5;   /* ---- 图标与项目 ---- */
    RHdr(P, L"添加");
    RBtn(P, L"添加网页应用…", A_ADDURL);
    RBtn(P, L"添加应用 / 文件…", A_ADDFILE);
    RBtn(P, L"添加文件夹…", A_ADDFOLDER);
    RBtn(P, L"新建快捷方式（应用链接 / 网址）…", A_NEWSC);
    RBtn(P, L"新建应用组", A_NEWGROUP);
    RBtn(P, L"新建临时周转组（重启 / 关机后自动清空）", A_NEWTEMP);
    RBtn(P, L"添加分割线", A_ADDSEP);
    RHdr(P, L"常用与美化");
    RLink(P, L"系统图标（开始、设置、资源管理器、回收站…）", 28, NULL);
    RLink(P, L"从文件夹批量设置图标", 27, NULL);
    RLink(P, L"深色图标批量设置", 27, L"为 Dock 中的每个图标单独选择深色版本，或按名称从文件夹匹配");

    P = 28;  /* ---- 系统图标 ---- */
    RTxt(P, L"勾选后会添加到 Dock：开始、设置、资源管理器、此电脑、浏览器、起动台在最左侧，回收站在最右侧。起动台会以全屏网格列出所有应用，支持直接输入搜索。", 50);
    for (int k = 0; k < SYS_N; k++) RTogA(P, g_sysName[k], A_SYS0 + k, NULL);

    P = 27;  /* ---- 批量设置图标 ---- */
    RHdr(P, L"从文件夹批量设置图标");
    RBtn2(P, L"选择图标文件夹…", A_PICKDIR, L"自动匹配并应用", A_AUTOMATCH);
    if (g_batchDir[0]) NewRow(P, R_TEXT, g_batchDir, 32);
    else RTxt(P, L"选择存放图标（png / ico / jpg）的文件夹，文件名与 Dock 上的应用名称相同即可自动匹配；找不到对应图标的应用会逐项提示。也可点击下面任意一行单独更换。", 62);
    RHdr(P, L"Dock 上的图标");
    for (int i = 0; i < nItems; i++) {
        if (items[i].type == T_SEP || items[i].temp || g_nrow > 300) continue;
        Row *r = NewRow(P, R_ITEM, NULL, 48);
        r->act = A_ITEM + i;
        lstrcpynW(r->dyn, items[i].name, 200);
        r->label = r->dyn;
    }
    RHdr(P, L"深色模式图标（批量设置）");
    RBtn2(P, L"选择深色图标文件夹…", A_PICKDARKDIR, L"匹配并应用深色图标", A_DARKMATCH);
    if (g_darkBatchDir[0]) NewRow(P, R_TEXT, g_darkBatchDir, 32);
    else RTxt(P, L"可为每个 Dock 图标单独选择深色图标，或选择文件夹按 Dock 图标名称自动匹配；支持 PNG、JPG、JPEG、ICO。留空的项目不更改现有深色图标。", 58);
    for (int i = 0; i < nItems; i++) {
        if (items[i].type == T_SEP || items[i].temp || g_nrow > 600) continue;
        Row *r = NewRow(P, R_ITEM, NULL, 48);
        r->act = A_DARKITEM + i;
        lstrcpynW(r->dyn, items[i].name, 200);
        r->label = r->dyn;
    }

    P = 9;   /* ---- 右键菜单配置 ---- */
    RHdr(P, L"Dock 右键菜单（右击 Dock 图标 / 空白处）");
    RTog(P, L"速记工具开关", &cfg.mnNotes, NULL);
    RTog(P, L"开机自启动开关", &cfg.mnAuto, NULL);
    RTog(P, L"空白处右键显示“新建快捷方式 / 应用组 / 临时组”", &cfg.mnAdd, NULL);
    RTog(P, L"菜单显示“任务管理器”", &cfg.mnTaskMgr, L"关闭后 Dock 与顶部状态栏的右键菜单不再出现任务管理器");
    RHdr(P, L"Dock 图标右键菜单项目");
    RTog(P, L"显示窗口合并 / 独立显示", &cfg.cmMerge, NULL);
    RTog(P, L"显示在右侧添加分割线", &cfg.cmSep, NULL);
    RTog(P, L"显示设置图标（Dock 设置 / 更改图标入口）", &cfg.cmIcon, L"关闭后 Dock 右键菜单不再出现“Dock 设置…”与图标设置入口");
    RTog(P, L"显示重命名", &cfg.cmRename, NULL);
    RTog(P, L"显示复制", &cfg.cmCopy, NULL);
    RTog(P, L"显示 Dock 设置", &cfg.cmDock, NULL);
    RHdr(P, L"系统右键菜单（资源管理器 / 桌面）");
    RTog(P, L"在文件和文件夹的右键菜单中添加“固定到 MiniDock”", &cfg.pinMenu, L"右键任意文件 / 文件夹即可固定到 Dock（Windows 11 在“显示更多选项”里）");
    RTxt(P, L"“退出 Dock”始终保留；Dock 设置可通过上方开关从右键菜单隐藏。图标自身的打开 / 关闭等基本操作保留。", 64);
    PageFooter(P);

    P = 6;   /* ---- 性能 ---- */
    RHdr(P, L"当前占用");
    NewRow(P, R_STAT, L"内存（工作集 / 私有）", 48);
    RHdr(P, L"预设");
    RBtn2(P, L"低资源占用模式", A_LOWMODE, L"适中模式（推荐）", A_RECMODE);
    RBtn(P, L"高资源占用模式（全部特效）", A_HIGHMODE);
    RTxt(P, L"低资源：纯色背景，关闭磨砂玻璃、运行中扫描、窗口预览、时间、倒影、最小化动画与守护进程。适中：磨砂玻璃 + 窗口预览 + 运行指示 + 最小化动画。高资源：在适中基础上再开启时间、图标倒影等效果。各开关可在对应选项卡单独调整。空闲时没有常驻动画定时器。", 98);
    RHdr(P, L"高耗能特效总开关");
    RTog(P, L"节能模式（关闭全部高耗能特效）", &cfg.ecoMode, L"关闭磨砂玻璃、窗口预览、最小化动画与菜单模糊；倒影保持自己的设置"); Perf(1);
    RTog(P, L"状态栏弹窗毛玻璃", &cfg.sbPopFrost, L"关闭后状态栏的日历/控制中心等弹窗改为纯色，降低占用"); Perf(1);
    RTog(P, L"低电量自动节能", &cfg.batEco, L"使用电池且电量低于阈值时自动开启节能模式，接通电源后恢复"); Dep(&cfg.ecoMode, 0, 0);
    RSl(P, L"低电量阈值", &cfg.batLow, 5, 60, L" %"); Dep(&cfg.batEco, 1, 0);
    RHdr(P, L"高耗能特效集中开关");
    RCmb(P, L"Dock 背景材质", &cfg.bgMode, o_bg, 3); Perf(3);
    RTog(P, L"起动台磨砂玻璃", &cfg.lpFrost, L"关闭后起动台为纯色压暗"); Perf(3);
    RTog(P, L"应用组毛玻璃", &cfg.groupFrost, L"关闭后应用组为纯色背景"); Perf(2);
    RTog(P, L"最小化 / 还原动画", &cfg.minAnim, L"飞入飞出 Dock 的动画"); Perf(2);
    RTog(P, L"悬停窗口预览", &cfg.preview, L"悬停运行中应用时显示缩略图"); Perf(2);
    RTog(P, L"悬停放大", &cfg.zoom, NULL); Perf(1);
    RTog(P, L"图标倒影", &cfg.reflect, L"仅底部停靠时显示"); Perf(2);
    RTog(P, L"启动弹跳", &cfg.bounce, NULL); Perf(1);
    RTog(P, L"时间与日期小组件", &cfg.clock, L"需要定时刷新"); Perf(2);
    RTog(P, L"通知小组件", &cfg.notifyW, L"持续监听任务栏提醒"); Perf(1);
    RTog(P, L"控制中心", &cfg.ctrlCenter, L"含音量/网络状态查询"); Perf(1);
    RTog(P, L"资源监视器", &cfg.resMon, L"定时采样 CPU / 内存 / 磁盘"); Perf(2);
    RTog(P, L"计时器 / 待办", &cfg.toolW, L"倒计时刷新"); Perf(1);
    RTog(P, L"专注模式切换", &cfg.modeW, L"切换整套布局"); Perf(1);
    RTog(P, L"系统托盘", &cfg.trayW, L"点击打开系统托盘"); Perf(1);
    RTog(P, L"顶部状态栏", &cfg.statusBar, L"独立窗口 + 定时刷新"); Perf(2);
    RTog(P, L"状态栏磨砂", &cfg.sbFrost, NULL); Dep(&cfg.statusBar, 1, 0); Perf(1);
    RTog(P, L"运行中应用扫描", &cfg.showRunning, L"定时枚举窗口"); Perf(2);
    RTog(P, L"守护进程", &cfg.guardian, L"额外的守护进程"); Perf(1);
    RTog(P, L"右键菜单毛玻璃", &cfg.menuFrost, L"打开菜单时截取并模糊背景"); Perf(2);
    RTog(P, L"运行中指示", &cfg.dots, L"图标下方的小圆点 / 横条，依赖窗口扫描"); Perf(1);
    RTog(P, L"多窗口独立显示", &cfg.multiMode, L"Office 文档 / 资源管理器窗口各占一个图标，并定时截取缩略图"); Perf(2);
    RTog(P, L"时间显示秒数", &cfg.clockSec, L"每秒刷新一次"); Perf(2);
    RTog(P, L"弧形展开文件夹", &cfg.folderFan, L"点击文件夹时枚举并加载文件图标"); Perf(1);
    RTog(P, L"状态栏避让窗口", &cfg.sbMaxAvoid, L"向系统登记为 AppBar，最大化窗口会避开状态栏"); Dep(&cfg.statusBar, 1, 0); Perf(1);

    P = P_ABOUT;   /* ---- 关于（含配置与备份） ---- */
    RTxt(P, L"ABOUT", 300);
    RHdr(P, L"检查更新");
    RTxt(P, L"UPDATE", 40);
    RBtn2(P, L"检查更新", A_CHECKUPD, L"前往下载最新版本", A_OPENUPD);
    RTog(P, L"每天自动检查一次更新", &cfg.updChk, L"仅访问 GitHub 的 Releases 接口，不上传任何信息");
    RBtn(P, L"打开 GitHub 仓库（wang01116/MiniDock）", A_OPENREPO);
    RHdr(P, L"配置文件位置");
    NewRow(P, R_TEXT, g_dir, 32);
    RBtn2(P, L"更改位置…", A_LOCCHANGE, L"恢复默认位置", A_LOCRESET);
    RBtn(P, L"打开配置文件夹", A_OPENDIR);
    RHdr(P, L"问题诊断与日志");
    RTog(P, L"开启日志收集", &cfg.logCollect, L"仅记录程序运行事件、系统状态与错误码；不记录窗口标题、文件路径、输入内容或剪贴板。关闭后停止新增日志");
    RTxt(P, L"日志保存在配置文件夹 MiniDock.log。遇到问题时开启收集、复现问题，再点击导出并上传日志。", 44);
    RBtn2(P, L"导出诊断日志…", A_LOGEXPORT, L"清空日志…", A_LOGCLEAR);
    RHdr(P, L"全部配置");
    RBtn(P, L"导出完整配置（设置 + 图标 + 应用组）…", A_EXP0);
    RBtn(P, L"仅导出应用与图标…", A_EXP2);
    RBtn(P, L"仅导出设置参数…", A_EXP1);
    RBtn(P, L"导入配置…", A_IMPORT);
    RHdr(P, L"Dock 样式（大小、缩放、动画、显示条、底色、不透明度、图标底盘）");
    RBtn2(P, L"导出 Dock 样式…", A_STYLEEXP, L"导入 Dock 样式…", A_STYLEIMP);
    RHdr(P, L"重置");
    RBtn(P, L"恢复默认设置", A_RESET);
    RBtn(P, L"删除所有配置并恢复默认（图标、应用组、起动台、设置）…", A_RESETALL);

}

/* ---- visibility / layout ---- */
static ARGB cBg, cSide, cCard, cLine, cText, cSub, cHover, cTrack;
/* hand-drawn vector icons for the settings sidebar (no font glyphs) */
/* One consistent, modern line-icon set for the sidebar: 24-unit grid, round caps, one stroke weight,
   one optical centre. Order matches g_ent[]: 通用 / 个性化 / 交互动效 / 图标自定义 / 状态栏与小组件 /
   右键菜单配置 / 性能 / 关于 */
static int EntIndex(int page) { for (int i = 0; i < NENT; i++) if (g_ent[i] == page) return i; return 0; }
static void DrawTabIcon(GpGraphics *g, int i, float x, float y, float s, ARGB c) {
    float sw = s * 0.09f;
    if (sw < 1.3f) sw = 1.3f;
    GpPen *p = MiPen(c, sw);
    float cx = x + s / 2, cy = y + s / 2;
    float u = s / 24.f;
    switch (i) {
    case 0:   /* 通用: three sliders */
        for (int k = 0; k < 3; k++) {
            float ly = (float)(k - 1) * 6.f;
            float kx = (k == 0) ? -3.2f : (k == 1 ? 3.6f : -0.8f);
            GdipDrawLine(g, p, cx - 8.f * u, cy + ly * u, cx + 8.f * u, cy + ly * u);
            FillRR(g, c, cx + (kx - 2.3f) * u, cy + (ly - 2.3f) * u, 4.6f * u, 4.6f * u, 2.3f * u);
        }
        break;
    case 1:   /* 个性化: paint brush */
        GdipDrawLine(g, p, cx - 6.6f * u, cy + 6.6f * u, cx + 3.0f * u, cy - 3.0f * u);
        GdipDrawLine(g, p, cx + 3.0f * u, cy - 3.0f * u, cx + 6.2f * u, cy - 6.2f * u);
        FillRR(g, c, cx + 4.6f * u, cy - 8.4f * u, 3.6f * u, 3.6f * u, 1.8f * u);
        break;
    case 2:   /* 交互动效: motion chevrons */
        for (int k = 0; k < 3; k++) {
            float ox = (float)k * 5.0f - 6.4f;
            GdipDrawLine(g, p, cx + ox * u, cy - 5.2f * u, cx + (ox + 3.6f) * u, cy);
            GdipDrawLine(g, p, cx + (ox + 3.6f) * u, cy, cx + ox * u, cy + 5.2f * u);
        }
        break;
    case 3:   /* 图标自定义: app tile with a spark */
        StrokeRR(g, c, sw, cx - 7.8f * u, cy - 7.8f * u, 15.6f * u, 15.6f * u, 4.6f * u);
        GdipDrawLine(g, p, cx, cy - 4.0f * u, cx, cy + 4.0f * u);
        GdipDrawLine(g, p, cx - 4.0f * u, cy, cx + 4.0f * u, cy);
        break;
    case 4:   /* 状态栏与小组件 */
        FillRR(g, c, cx - 8.f * u, cy - 7.6f * u, 16.f * u, 3.6f * u, 1.8f * u);
        StrokeRR(g, c, sw, cx - 8.f * u, cy - 1.8f * u, 7.2f * u, 8.6f * u, 2.4f * u);
        StrokeRR(g, c, sw, cx + 0.8f * u, cy - 1.8f * u, 7.2f * u, 8.6f * u, 2.4f * u);
        break;
    case 5:   /* 右键菜单: panel with a ticked entry */
        StrokeRR(g, c, sw, cx - 8.f * u, cy - 8.f * u, 16.f * u, 13.f * u, 3.f * u);
        for (int k = 0; k < 2; k++)
            GdipDrawLine(g, p, cx - 5.4f * u, cy - 4.6f * u + k * 3.8f * u, cx + 1.6f * u, cy - 4.6f * u + k * 3.8f * u);
        GdipDrawLine(g, p, cx + 3.0f * u, cy + 2.4f * u, cx + 4.8f * u, cy + 4.2f * u);
        GdipDrawLine(g, p, cx + 4.8f * u, cy + 4.2f * u, cx + 8.2f * u, cy - 0.6f * u);
        break;
    case 6:   /* 性能: gauge */
        GdipDrawArc(g, p, cx - 8.f * u, cy - 6.4f * u, 16.f * u, 16.f * u, 180, 180);
        GdipDrawLine(g, p, cx, cy + 2.4f * u, cx + 4.8f * u, cy - 3.0f * u);
        FillRR(g, c, cx - 1.7f * u, cy + 0.7f * u, 3.4f * u, 3.4f * u, 1.7f * u);
        break;
    default:  /* 关于: info */
        GdipDrawEllipse(g, p, cx - 8.f * u, cy - 8.f * u, 16.f * u, 16.f * u);
        FillRR(g, c, cx - 1.4f * u, cy - 5.4f * u, 2.8f * u, 2.8f * u, 1.4f * u);
        FillRR(g, c, cx - 1.4f * u, cy - 1.2f * u, 2.8f * u, 6.6f * u, 1.4f * u);
        break;
    }
    GdipDeletePen(p);
}

static FrostSnap g_setFrost;
static void SetFrostGrab(void);
static void UiColors(void) {
    if (g_uiDark) { cBg = ARGBC(255, 24, 25, 29); cSide = ARGBC(255, 32, 33, 38); cCard = ARGBC(255, 40, 41, 47); cLine = ARGBC(255, 58, 60, 68); cText = ARGBC(255, 236, 237, 242); cSub = ARGBC(255, 146, 150, 162); cHover = ARGBC(255, 50, 52, 60); cTrack = ARGBC(255, 78, 81, 92); }
    else { cBg = ARGBC(255, 243, 244, 248); cSide = ARGBC(255, 234, 236, 242); cCard = ARGBC(255, 255, 255, 255); cLine = ARGBC(255, 232, 234, 240); cText = ARGBC(255, 28, 30, 38); cSub = ARGBC(255, 122, 126, 140); cHover = ARGBC(255, 244, 246, 251); cTrack = ARGBC(255, 212, 216, 228); }
}

static float g_x0, g_cw, g_viewH, g_contentH;

static BOOL RowShown(const Row *r) {
    if (!r->dep || g_searching) return TRUE;
    return r->depNeg ? (*r->dep != r->depVal) : (*r->dep == r->depVal);
}
static BOOL RowMatches(const Row *r) {
    if (r->type == R_HDR || r->type == R_TEXT || r->type == R_STAT || r->type == R_ITEM) return FALSE;
    return (r->label && StrStrIW(r->label, g_q)) || (r->label2 && r->type != R_BTN2 && StrStrIW(r->label2, g_q)) || (r->type == R_BTN2 && r->label2 && StrStrIW(r->label2, g_q));
}

static void LayoutRows(void) {
    g_nvis = 0;
    float y = (float)Sc(78) + (HasTabs() ? Sc(50) : 0) - g_scroll;
    if (g_searching) {
        int lastPage = -1;
        for (int i = 0; i < g_nrow && g_nvis < 410; i++) {
            if (!RowMatches(&g_row[i])) continue;
            if (g_row[i].page != lastPage) {
                lastPage = g_row[i].page;
                g_vis[g_nvis].row = -1; g_vis[g_nvis++].page = lastPage;
                y += Sc(40);
            }
            g_row[i].y = y;
            y += g_row[i].h;
            g_vis[g_nvis].row = i; g_vis[g_nvis++].page = g_row[i].page;
        }
    } else {
        for (int i = 0; i < g_nrow && g_nvis < 410; i++) {
            if (g_row[i].page != g_page || !RowShown(&g_row[i])) continue;
            g_row[i].y = y;
            y += g_row[i].h;
            g_vis[g_nvis].row = i; g_vis[g_nvis++].page = g_page;
        }
    }
    g_contentH = y + g_scroll - Sc(78);
}

static void RowRects(Row *r, RectF *a, RectF *b) {
    float xr = g_x0 + g_cw - Sc(16);
    a->X = a->Y = a->Width = a->Height = 0;
    *b = *a;
    switch (r->type) {
    case R_TOGGLE: a->X = xr - Sc(46); a->Y = r->y + (r->h - Sc(26)) / 2; a->Width = (float)Sc(46); a->Height = (float)Sc(26); break;
    case R_SLIDER:
        a->X = g_x0 + g_cw * 0.44f; a->Y = r->y; a->Width = xr - Sc(84) - a->X; a->Height = r->h;
        b->X = xr - Sc(72); b->Y = r->y + (r->h - Sc(30)) / 2; b->Width = (float)Sc(72); b->Height = (float)Sc(30); break;
    case R_COMBO: a->X = xr - Sc(250); a->Y = r->y + (r->h - Sc(34)) / 2; a->Width = (float)Sc(250); a->Height = (float)Sc(34); break;
    case R_BTN: case R_ITEM: case R_LINK: a->X = g_x0; a->Y = r->y; a->Width = g_cw; a->Height = r->h; break;
    case R_BTN2:
        a->X = xr - Sc(304); a->Y = r->y + (r->h - Sc(34)) / 2; a->Width = (float)Sc(150); a->Height = (float)Sc(34);
        b->X = xr - Sc(150); b->Y = a->Y; b->Width = (float)Sc(150); b->Height = a->Height; break;
    case R_COLOR:
        a->X = xr - Sc(40); a->Y = r->y + (r->h - Sc(30)) / 2; a->Width = (float)Sc(40); a->Height = (float)Sc(30);
        b->X = xr - Sc(120); b->Y = a->Y; b->Width = (float)Sc(70); b->Height = a->Height; break;
    }
}
static BOOL InRect(const RectF *r, float x, float y) { return x >= r->X && x < r->X + r->Width && y >= r->Y && y < r->Y + r->Height; }

/* ---- painting ---- */
static void DrawToggle(GpGraphics *g, RectF *a, BOOL on) {
    FillRR(g, on ? ARGBC(255, 52, 199, 89) : cTrack, a->X, a->Y, a->Width, a->Height, a->Height / 2);
    float k = a->Height - 6;
    FillRR(g, ARGBC(255, 255, 255, 255), on ? a->X + a->Width - k - 3 : a->X + 3, a->Y + 3, k, k, k / 2);
}
static void DrawChevronDown(GpGraphics *g, float x, float y, ARGB c) {
    GpPen *p;
    GdipCreatePen1(c, 1.6f, 2, &p);
    GdipDrawLine(g, p, x - 4, y - 2, x, y + 2);
    GdipDrawLine(g, p, x, y + 2, x + 4, y - 2);
    GdipDeletePen(p);
}
static void DrawChevronRight(GpGraphics *g, float x, float y, ARGB c) {
    GpPen *p;
    GdipCreatePen1(c, 1.6f, 2, &p);
    GdipDrawLine(g, p, x - 2, y - 4, x + 2, y);
    GdipDrawLine(g, p, x + 2, y, x - 2, y + 4);
    GdipDeletePen(p);
}
static void DrawChevronLeft(GpGraphics *g, float x, float y, ARGB c) {
    GpPen *p;
    GdipCreatePen1(c, 1.8f, 2, &p);
    GdipDrawLine(g, p, x + 3, y - 6, x - 2, y);
    GdipDrawLine(g, p, x - 2, y, x + 3, y + 6);
    GdipDeletePen(p);
}

static void PaintRow(GpGraphics *g, HDC dc, int i, int H, BOOL first) {
    Row *r = &g_row[i];
    float xl = g_x0 + Sc(18), xr = g_x0 + g_cw - Sc(16);
    RectF a, b;
    RowRects(r, &a, &b);
    BOOL hot = (i == g_rowHot);
    if (r->type == R_HDR) { DrawTextBox(g, r->label, g_fontBold, g_x0 + 4, r->y + Sc(12), g_cw, Sc(26), cSub, TRUE); return; }
    if (!first && r->type != R_HDR) {
        GpPen *p;
        GdipCreatePen1(cLine, 1, 2, &p);
        GdipDrawLine(g, p, xl, r->y, xr, r->y);
        GdipDeletePen(p);
    }
    if (hot && (r->type == R_BTN || r->type == R_ITEM || r->type == R_LINK)) FillRR(g, cHover, g_x0 + 3, r->y + 2, g_cw - 6, r->h - 4, 10);
    BOOL note = r->label2 && (r->type == R_TOGGLE || r->type == R_LINK);
    float ly = r->y + (note ? Sc(8) : 0);
    float lh = note ? (float)Sc(24) : r->h;
    switch (r->type) {
    case R_TOGGLE: {
        BOOL on = r->v ? *r->v : GetSpecial(r->act);
        DrawTextBox(g, r->label, g_font, xl, ly, g_cw * 0.7f, lh, cText, TRUE);
        if (r->label2) DrawTextBox(g, r->label2, g_font, xl, ly + Sc(22), g_cw * 0.82f, Sc(22), cSub, TRUE);
        DrawToggle(g, &a, on);
        break;
    }
    case R_SLIDER: {
        DrawTextBox(g, r->label, g_font, xl, r->y, g_cw * 0.42f, r->h, cText, TRUE);
        float t = (float)(*r->v - r->mn) / (r->mx - r->mn);
        float cy = r->y + r->h / 2;
        FillRR(g, cTrack, a.X, cy - 2.5f, a.Width, 5, 2.5f);
        FillRR(g, ARGBC(255, 10, 132, 255), a.X, cy - 2.5f, a.Width * t, 5, 2.5f);
        float kx = a.X + a.Width * t;
        FillRR(g, ARGBC(60, 0, 0, 0), kx - 10, cy - 8, 20, 20, 10);
        FillRR(g, ARGBC(255, 255, 255, 255), kx - 9, cy - 9, 18, 18, 9);
        StrokeRR(g, ARGBC(70, 120, 120, 130), 1, kx - 9, cy - 9, 18, 18, 9);
        WCHAR v[32];
        wsprintfW(v, L"%d%s", *r->v, r->suffix ? r->suffix : L"");
        FillRR(g, g_uiDark ? ARGBC(255, 55, 57, 66) : ARGBC(255, 238, 240, 246), b.X, b.Y, b.Width, b.Height, 8);
        DrawCentered(g, v, g_font, b.X, b.Y, b.Width, b.Height, cText);
        break;
    }
    case R_COMBO: {
        DrawTextBox(g, r->label, g_font, xl, r->y, g_cw * 0.45f, r->h, cText, TRUE);
        int cur = *r->v < r->nopts ? *r->v : 0;
        FillRR(g, g_uiDark ? ARGBC(255, 55, 57, 66) : ARGBC(255, 238, 240, 246), a.X, a.Y, a.Width, a.Height, 9);
        DrawTextBox(g, r->opts[cur], g_font, a.X + 12, a.Y, a.Width - 36, a.Height, cText, TRUE);
        DrawChevronDown(g, a.X + a.Width - 16, a.Y + a.Height / 2, cSub);
        break;
    }
    case R_BTN:
        DrawTextBox(g, r->label, g_font, xl, r->y, g_cw * 0.82f, r->h, cText, TRUE);
        DrawChevronRight(g, xr - 6, r->y + r->h / 2, cSub);
        break;
    case R_LINK:
        DrawTextBox(g, r->label, g_font, xl, ly, g_cw * 0.8f, lh, cText, TRUE);
        if (r->label2) DrawTextBox(g, r->label2, g_font, xl, ly + Sc(22), g_cw * 0.82f, Sc(22), cSub, TRUE);
        DrawChevronRight(g, xr - 6, r->y + r->h / 2, cSub);
        break;
    case R_BTN2:
        FillRR(g, ARGBC(255, 10, 132, 255), a.X, a.Y, a.Width, a.Height, 9);
        DrawCentered(g, r->label, g_font, a.X, a.Y, a.Width, a.Height, ARGBC(255, 255, 255, 255));
        FillRR(g, g_uiDark ? ARGBC(255, 62, 64, 74) : ARGBC(255, 232, 235, 243), b.X, b.Y, b.Width, b.Height, 9);
        DrawCentered(g, r->label2, g_font, b.X, b.Y, b.Width, b.Height, cText);
        break;
    case R_COLOR: {
        DrawTextBox(g, r->label, g_font, xl, r->y, g_cw * 0.5f, r->h, cText, TRUE);
        int c = *r->v;
        DrawCentered(g, c >= 0 ? L"重置" : L"自动", g_font, b.X, b.Y, b.Width, b.Height, cSub);
        {
            float sx = floorf(a.X), sy = floorf(a.Y), sw = floorf(a.Width), sh = floorf(a.Height);
            GdipSetSmoothingMode(g, 5);   /* 8x4 anti-aliasing + pixel-snapped rect: clean rounded corners */
            GdipSetPixelOffsetMode(g, 2);
            FillRR(g, c >= 0 ? ARGBC(255, GetRValue(c), GetGValue(c), GetBValue(c)) : cTrack, sx, sy, sw, sh, 9);
            StrokeRR(g, cLine, 1.f, sx, sy, sw, sh, 9);
            GdipSetSmoothingMode(g, 4);
            GdipSetPixelOffsetMode(g, 4);
        }
        break;
    }
    case R_TEXT: {
        if (!lstrcmpW(r->label, L"ABOUT")) {
            float cx = g_x0 + g_cw / 2;
            HICON ic = LoadIconW(g_inst, MAKEINTRESOURCEW(1));
            HICON big = (HICON)LoadImageW(g_inst, MAKEINTRESOURCEW(1), IMAGE_ICON, 256, 256, LR_DEFAULTCOLOR);
            GpBitmap *ib = big ? HiconToGp(big) : NULL;
            if (ib) {   /* GDI+ high-quality scaling: no jagged edges */
                GdipSetInterpolationMode(g, 7);
                DrawImg(g, ib, cx - Sc(36), r->y + Sc(22), (float)Sc(72), (float)Sc(72), FALSE);
                GdipDisposeImage(ib);
            } else DrawIconEx(dc, (int)(cx - Sc(36)), (int)(r->y + Sc(22)), ic, Sc(72), Sc(72), 0, NULL, DI_NORMAL);
            if (big) DestroyIcon(big);
            DrawCentered(g, L"MiniDock", g_fontTitle, g_x0, r->y + Sc(100), g_cw, Sc(36), cText);
            DrawCentered(g, L"版本 " APPVER L"  ·  64 位  ·  免安装", g_font, g_x0, r->y + Sc(138), g_cw, Sc(24), cSub);
            RectF tr = { g_x0 + Sc(40), r->y + Sc(176), g_cw - Sc(80), Sc(116) };
            GpBrush *br;
            GdipCreateSolidFill(cText, &br);
            GdipDrawString(g, L"为 Windows 11 打造的极简 Dock 栏。\n固定应用、文件夹和网页，应用组与临时周转组，起动台，日历与通知小组件，窗口预览和流畅动画。每个功能都能单独关闭，空闲时只占几十 MB 内存。\n\n作者 Wangcy  ·  开源于 GitHub", -1, g_font, &tr, g_fmtW, br);
            GdipDeleteBrush(br);
        } else {
            RectF tr = { xl, r->y + Sc(6), g_cw - Sc(36), r->h - Sc(8) };
            GpBrush *br;
            GdipCreateSolidFill(g_updNew && !lstrcmpW(r->label, L"UPDATE") ? ARGBC(255, 10, 132, 255) : cSub, &br);
            GdipDrawString(g, !lstrcmpW(r->label, L"UPDATE") ? (g_updText[0] ? g_updText : L"点击“检查更新”获取 GitHub 上的最新版本信息。") : r->label, -1, g_font, &tr, g_fmtW, br);
            GdipDeleteBrush(br);
        }
        break;
    }
    case R_STAT: {
        PROCESS_MEMORY_COUNTERS_EX pm;
        ZeroMemory(&pm, sizeof pm);
        pm.cb = sizeof pm;
        GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pm, sizeof pm);
        WCHAR v[80];
        wsprintfW(v, L"%d.%d MB / %d.%d MB", (int)(pm.WorkingSetSize >> 20), (int)((pm.WorkingSetSize >> 10) % 1024 * 10 / 1024),
                  (int)(pm.PrivateUsage >> 20), (int)((pm.PrivateUsage >> 10) % 1024 * 10 / 1024));
        DrawTextBox(g, r->label, g_font, xl, r->y, g_cw * 0.5f, r->h, cText, TRUE);
        DrawTextBoxR(g, v, g_fontBold, xl, r->y, g_cw - Sc(36), r->h, cText);
        break;
    }
    case R_ITEM: {
        BOOL darkItem = r->act >= A_DARKITEM && r->act < A_DARKITEM + MAXI;
        int ii = darkItem ? (r->act - A_DARKITEM) : (r->act - A_ITEM);
        DrawTextBox(g, r->label, g_font, xl, r->y, g_cw * 0.45f, r->h, cText, TRUE);
        const WCHAR *st = darkItem ? (L"点击选择深色图标") : L"点击更换图标";
        ARGB sc = cSub;
        WCHAR tmp[MAX_PATH];
        int state = (ii >= 0 && ii < nItems) ? (darkItem ? g_darkState[ii] : g_mstate[ii]) : 0;
        const WCHAR *match = (ii >= 0 && ii < nItems) ? (darkItem ? g_darkMatch[ii] : g_match[ii]) : L"";
        if (ii >= 0 && ii < nItems && state == 1) { lstrcpynW(tmp, PathFindFileNameW(match), MAX_PATH); st = tmp; sc = ARGBC(255, 52, 199, 89); }
        else if (ii >= 0 && ii < nItems && state == 2) { st = L"未找到对应图标"; sc = ARGBC(255, 255, 69, 58); }
        else if (ii >= 0 && ii < nItems && state == 3) { st = L"已设置"; sc = ARGBC(255, 52, 199, 89); }
        else if (darkItem && ii >= 0 && ii < nItems && items[ii].icon_dark[0]) st = L"已设置深色图标";
        DrawTextBoxR(g, st, g_font, xl, r->y, g_cw - Sc(52), r->h, sc);
        DrawChevronRight(g, xr - 6, r->y + r->h / 2, cSub);
        break;
    }
    }
    if (r->perf && r->label && (r->type == R_TOGGLE)) {   /* small cost tag next to the switch label */
        static const WCHAR *pn[4] = { L"", L"占用 低", L"占用 中", L"占用 高" };
        ARGB pc = r->perf == 1 ? ARGBC(255, 52, 199, 89) : r->perf == 2 ? ARGBC(255, 255, 159, 10) : ARGBC(255, 255, 69, 58);
        float tw = TextWidth(g, r->label, g_font);
        DrawTextBox(g, pn[r->perf], g_font, xl + tw + Sc(12), ly + Sc(1), Sc(80), lh, pc, TRUE);
    }
}

static void PaintSettings(HDC dc, int W, int H) {
    UiColors();
    GpGraphics *g = NULL;
    GdipCreateFromHDC(dc, &g);
    if (!g) return;
    SetupG(g);
    if (cfg.uiFrost && g_setFrost.gp) {
        FrostSnapPaint(g, &g_setFrost, 0, 0, (float)W, (float)H, 0.f, 1.f);
        int r0 = (cBg >> 16) & 255, g0 = (cBg >> 8) & 255, b0 = cBg & 255;
        FillRR(g, ARGBC(200, r0, g0, b0), 0, 0, (float)W, (float)H, 0.f);   /* translucent tint over the blur */
    } else GdipGraphicsClear(g, cBg);
    float sbw = (float)Sc(SBW);
    g_x0 = sbw + Sc(30);
    g_cw = W - g_x0 - Sc(30);
    g_viewH = (float)H - Sc(78);
    LayoutRows();
    /* cards: consecutive non-header rows */
    for (int i = 0; i < g_nvis;) {
        if (g_vis[i].row < 0 || g_row[g_vis[i].row].type == R_HDR) { i++; continue; }
        int j = i;
        while (j + 1 < g_nvis && g_vis[j + 1].row >= 0 && g_row[g_vis[j + 1].row].type != R_HDR) j++;
        float y0 = g_row[g_vis[i].row].y, y1 = g_row[g_vis[j].row].y + g_row[g_vis[j].row].h;
        if (y1 > Sc(66) && y0 < H) { FillRR(g, cCard, g_x0, y0, g_cw, y1 - y0, 14); StrokeRR(g, cLine, 1, g_x0, y0, g_cw, y1 - y0, 14); }
        i = j + 1;
    }
    float cy = (float)Sc(78) - g_scroll;
    for (int k = 0; k < g_nvis; k++) {
        if (g_vis[k].row < 0) {   /* search-result crumb */
            WCHAR cr[100];
            int p = g_vis[k].page;
            if (p >= SUBBASE) wsprintfW(cr, L"%s › %s", g_pageName[ParentOf(p)], SubName(p)); else lstrcpynW(cr, g_pageName[p], 100);
            DrawTextBox(g, cr, g_fontBold, g_x0 + 4, cy + Sc(12), g_cw, Sc(26), cSub, TRUE);
            cy += Sc(40);
            continue;
        }
        Row *r = &g_row[g_vis[k].row];
        cy = r->y + r->h;
        if (r->y + r->h < Sc(64) || r->y > H) continue;
        BOOL first = (k == 0) || g_vis[k - 1].row < 0 || g_row[g_vis[k - 1].row].type == R_HDR;
        PaintRow(g, dc, g_vis[k].row, H, first);
    }
    /* top strip, sidebar, titles, scrollbar are painted over the (clipped) rows */
    GpBrush *bb;
    GdipCreateSolidFill(cBg, &bb);
    GdipFillRectangle(g, bb, sbw, 0, W - sbw, (REAL)(HasTabs() ? Sc(120) : Sc(66)));
    GdipDeleteBrush(bb);
    GdipCreateSolidFill(cSide, &bb);
    GdipFillRectangle(g, bb, 0, 0, sbw, (REAL)H);
    GdipDeleteBrush(bb);
    {
        GpPen *p;
        GdipCreatePen1(cLine, 1, 2, &p);
        GdipDrawLine(g, p, sbw, 0, sbw, (REAL)H);
        GdipDeletePen(p);
    }
    DrawTextBox(g, L"MiniDock", g_fontTitle, (float)Sc(24), (float)Sc(18), sbw - Sc(30), (float)Sc(44), cText, TRUE);
    int top = ParentOf(g_page);
    for (int i = 0; i < NENT; i++) {
        float iy = (float)(Sc(78) + i * Sc(42)), ix = (float)Sc(12), iw = sbw - Sc(24), ih = (float)Sc(36);
        BOOL sel = (g_ent[i] == top) && !g_searching;
        if (sel) FillRR(g, ARGBC(255, 10, 132, 255), ix, iy, iw, ih, 10);
        else if (i == g_sbHot) FillRR(g, g_uiDark ? ARGBC(255, 48, 50, 58) : ARGBC(255, 222, 226, 237), ix, iy, iw, ih, 10);
        DrawTabIcon(g, i, ix + Sc(8), iy + Sc(9), Sc(16), sel ? ARGBC(255, 255, 255, 255) : cSub);
        DrawTextBox(g, g_pageName[g_ent[i]], g_font, ix + Sc(32), iy, iw - Sc(32), ih, sel ? ARGBC(255, 255, 255, 255) : cText, TRUE);
    }
    DrawTextBox(g, L"v" APPVER, g_font, (float)Sc(24), (float)(H - Sc(40)), sbw - Sc(30), (float)Sc(24), cSub, TRUE);
    /* title (with a back arrow on sub pages) */
    g_backRc.left = g_backRc.right = g_backRc.top = g_backRc.bottom = 0;
    if (g_searching) DrawTextBox(g, L"搜索结果", g_fontTitle, g_x0, (float)Sc(18), g_cw * 0.5f, (float)Sc(44), cText, TRUE);
    else if (g_page >= SUBBASE && g_page != 21 && g_page != 30) {
        DrawChevronLeft(g, g_x0 + 8, (float)Sc(40), ARGBC(255, 10, 132, 255));
        WCHAR t[100];
        wsprintfW(t, L"%s  ›  %s", g_pageName[top], SubName(g_page));
        DrawTextBox(g, t, g_fontTitle, g_x0 + Sc(24), (float)Sc(18), g_cw * 0.62f, (float)Sc(44), cText, TRUE);
        g_backRc.left = (int)g_x0 - 4; g_backRc.top = Sc(14); g_backRc.right = (int)(g_x0 + g_cw * 0.6f); g_backRc.bottom = Sc(62);
    } else {
        float chip = (float)Sc(30);
        FillRR(g, g_uiDark ? ARGBC(255, 42, 44, 52) : ARGBC(255, 235, 238, 246), g_x0, (float)Sc(24), chip, chip, 9);
        DrawTabIcon(g, EntIndex(ParentOf(g_page)), g_x0 + Sc(7), (float)Sc(31), Sc(16), g_uiDark ? cText : ARGBC(255, 10, 132, 255));
        DrawTextBox(g, g_pageName[ParentOf(g_page)], g_fontTitle, g_x0 + chip + Sc(10), (float)Sc(18), g_cw * 0.5f, (float)Sc(44), cText, TRUE);
    }
    if (HasTabs()) {   /* two wide rounded tabs: Dock bar / icons */
        float tw = (g_cw - Sc(12)) / 2, ty0 = (float)Sc(70), th0 = (float)Sc(42);
        BOOL sbGroup = (g_page == 4 || g_page == 30);
        const WCHAR *tn[2];
        tn[0] = sbGroup ? L"小组件设置" : L"Dock栏个性化";
        tn[1] = sbGroup ? L"状态栏设置" : L"图标个性化";
        for (int t = 0; t < 2; t++) {
            float tx0 = g_x0 + t * (tw + Sc(12));
            BOOL on = sbGroup ? (g_page == (t == 0 ? 4 : 30)) : (g_page == (t == 0 ? 1 : 21));
            FillRR(g, on ? ARGBC(255, 10, 132, 255) : cCard, tx0, ty0, tw, th0, 14);
            if (!on) StrokeRR(g, cLine, 1, tx0, ty0, tw, th0, 14);
            DrawCentered(g, tn[t], g_font, tx0, ty0, tw, th0, on ? ARGBC(255, 255, 255, 255) : cText);
        }
    }
    /* search box background */
    {
        float sx = g_x0 + g_cw - Sc(230), sy = (float)Sc(22);
        FillRR(g, g_uiDark ? ARGBC(255, 52, 54, 62) : ARGBC(255, 232, 235, 243), sx, sy, (float)Sc(230), (float)Sc(32), 10);
        GpPen *p;
        GdipCreatePen1(cSub, 2.2f, 2, &p);
        float lens = (float)Sc(13), lx = sx + (float)Sc(11), ly = sy + ((float)Sc(32) - lens) / 2.f;
        GdipDrawEllipse(g, p, lx, ly, lens, lens);      /* lens, exactly centred in the box at every DPI */
        GdipDrawLine(g, p, lx + lens * 0.72f, ly + lens * 0.72f, lx + lens * 1.18f, ly + lens * 1.18f);
        GdipDeletePen(p);
    }
    if (g_contentH > g_viewH) {
        float th = g_viewH * g_viewH / g_contentH, ty = Sc(78) + (g_viewH - th) * g_scroll / (g_contentH - g_viewH + Sc(24));
        FillRR(g, g_uiDark ? ARGBC(120, 255, 255, 255) : ARGBC(110, 0, 0, 0), W - 9.f, ty, 4, th, 2);
    }
    GdipDeleteGraphics(g);
}


/* ---- interaction ---- */
static void DoNewShortcut(HWND parent);

static int RowHit(float x, float y, int *part) {
    for (int k = 0; k < g_nvis; k++) {
        if (g_vis[k].row < 0) continue;
        Row *r = &g_row[g_vis[k].row];
        if (r->type == R_HDR || r->type == R_TEXT || r->type == R_STAT) continue;
        if (y < (HasTabs() ? Sc(120) : Sc(66)) || y < r->y || y >= r->y + r->h || x < g_x0 || x > g_x0 + g_cw) continue;
        RectF a, b;
        RowRects(r, &a, &b);
        *part = 0;
        if (r->type == R_SLIDER) { if (InRect(&b, x, y)) *part = 3; else if (x >= a.X - 10 && x <= a.X + a.Width + 10) *part = 2; }
        else if (r->type == R_TOGGLE) *part = (InRect(&a, x, y) || x > g_x0 + g_cw * 0.5f) ? 1 : 0;
        else if (r->type == R_BTN2) { if (InRect(&a, x, y)) *part = 5; else if (InRect(&b, x, y)) *part = 6; }
        else if (r->type == R_COLOR) { if (InRect(&a, x, y)) *part = 7; else if (InRect(&b, x, y)) *part = 8; }
        else *part = 1;
        return g_vis[k].row;
    }
    return -1;
}

static void SetSlider(Row *r, float x) {
    RectF a, b;
    RowRects(r, &a, &b);
    float t = (x - a.X) / a.Width;
    if (t < 0) t = 0;
    if (t > 1) t = 1;
    *r->v = r->mn + (int)(t * (r->mx - r->mn) + 0.5f);
}

static void SettingsRedraw(void) { if (g_hset) InvalidateRect(g_hset, NULL, FALSE); }

/* ---- scoped (per-tab / style) export & import ---- */
static const WCHAR *KeyOfPtr(const int *p) {
    for (int i = 0; i < NTAB; i++) if (g_tab[i].p == p) return g_tab[i].k;
    return NULL;
}
static BOOL InScope(int page, const int *tops, int nt) {
    int top = ParentOf(page);
    for (int i = 0; i < nt; i++) if (tops[i] == top) return TRUE;
    return FALSE;
}

static void ExportScope(HWND h, const int *tops, int nt, const WCHAR *defName) {
    WCHAR f[MAX_PATH];
    lstrcpyW(f, defName);
    OPENFILENAMEW o;
    ZeroMemory(&o, sizeof o);
    o.lStructSize = sizeof o; o.hwndOwner = h;
    o.lpstrFilter = L"MiniDock 设置 (*.mdset)\0*.mdset\0";
    o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.lpstrDefExt = L"mdset";
    o.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    if (!GetSaveFileNameW(&o)) return;
    WBuf w = { (WCHAR *)HeapAlloc(GetProcessHeap(), 0, 8192 * sizeof(WCHAR)), 0, 8192 };
    if (!w.b) return;
    w.b[0] = 0;
    WCat(&w, L"[Settings]\r\n");
    for (int i = 0; i < g_nrow; i++) {
        Row *r = &g_row[i];
        if (!r->v || !InScope(r->page, tops, nt)) continue;
        const WCHAR *k = KeyOfPtr(r->v);
        if (!k) continue;
        WCHAR t[96];
        wsprintfW(t, L"%s=%d\r\n", k, *r->v);
        WCat(&w, t);
    }
    HANDLE hf = CreateFileW(f, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hf != INVALID_HANDLE_VALUE) {
        WORD bom = 0xFEFF;
        DWORD wr;
        WriteFile(hf, &bom, 2, &wr, NULL);
        WriteFile(hf, w.b, (DWORD)(w.n * sizeof(WCHAR)), &wr, NULL);
        CloseHandle(hf);
        MessageBoxW(h, L"已导出。", L"MiniDock", MB_OK | MB_ICONINFORMATION);
    }
    HeapFree(GetProcessHeap(), 0, w.b);
}

static void ImportScope(HWND h, const int *tops, int nt) {
    WCHAR f[MAX_PATH] = L"";
    OPENFILENAMEW o;
    ZeroMemory(&o, sizeof o);
    o.lStructSize = sizeof o; o.hwndOwner = h;
    o.lpstrFilter = L"MiniDock 设置 (*.mdset)\0*.mdset\0";
    o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.Flags = OFN_FILEMUSTEXIST;
    if (!GetOpenFileNameW(&o)) return;
    int n = 0;
    for (int i = 0; i < g_nrow; i++) {
        Row *r = &g_row[i];
        if (!r->v || !InScope(r->page, tops, nt)) continue;
        const WCHAR *k = KeyOfPtr(r->v);
        if (!k) continue;
        int v = (int)GetPrivateProfileIntW(L"Settings", k, -99999, f);
        if (v != -99999) { *r->v = v; n++; }
    }
    ApplySettings();
    ReloadAllIcons();
    BuildRows();
    WCHAR msg[64];
    wsprintfW(msg, n ? L"已导入 %d 项设置。" : L"该文件中没有适用于此处的设置。", n);
    MessageBoxW(h, msg, L"MiniDock", MB_OK | MB_ICONINFORMATION);
}

/* ---- moving the data folder ---- */
static void DefaultDataDir(WCHAR *out) {
    WCHAR ap[MAX_PATH];
    SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, 0, ap);
    PathCombineW(out, ap, L"MiniDock");
}

static void CopyDirFiles(const WCHAR *from, const WCHAR *to) {
    WCHAR pat[MAX_PATH];
    PathCombineW(pat, from, L"*");
    WIN32_FIND_DATAW fd;
    HANDLE h = FindFirstFileW(pat, &fd);
    if (h == INVALID_HANDLE_VALUE) return;
    do {
        if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
        WCHAR a[MAX_PATH], b[MAX_PATH];
        PathCombineW(a, from, fd.cFileName);
        PathCombineW(b, to, fd.cFileName);
        CopyFileW(a, b, FALSE);
    } while (FindNextFileW(h, &fd));
    FindClose(h);
}

static void RemapIconPaths(const WCHAR *oldDir, const WCHAR *newDir) {
    size_t ol = lstrlenW(oldDir);
    for (int i = 0; i < nItems; i++) {
        Item *tabs[1] = { &items[i] };
        (void)tabs;
        for (int k = -1; k < items[i].nk; k++) {
            Item *it = k < 0 ? &items[i] : &items[i].kids[k];
            if (it->icon_src[0] && !StrCmpNIW(it->icon_src, oldDir, (int)ol)) {
                WCHAR t[512];
                wsprintfW(t, L"%s%s", newDir, it->icon_src + ol);
                lstrcpynW(it->icon_src, t, 512);
            }
        }
    }
}

static void MoveDataDir(HWND h, const WCHAR *newDir) {
    if (!lstrcmpiW(newDir, g_dir)) return;
    SaveNow();
    WCHAR oldIcons[MAX_PATH], newIcons[MAX_PATH], t1[MAX_PATH], t2[MAX_PATH];
    lstrcpyW(oldIcons, g_iconDir);
    CreateDirectoryW(newDir, NULL);
    PathCombineW(newIcons, newDir, L"icons");
    CreateDirectoryW(newIcons, NULL);
    CopyDirFiles(oldIcons, newIcons);
    PathCombineW(t1, g_dir, L"notes.dat"); PathCombineW(t2, newDir, L"notes.dat");
    CopyFileW(t1, t2, FALSE);
    RemapIconPaths(oldIcons, newIcons);
    WCHAR def[MAX_PATH], loc[MAX_PATH];
    DefaultDataDir(def);
    CreateDirectoryW(def, NULL);
    PathCombineW(loc, def, L"location.txt");
    if (!lstrcmpiW(newDir, def)) DeleteFileW(loc);
    else {
        HANDLE f = CreateFileW(loc, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (f != INVALID_HANDLE_VALUE) { DWORD w; WriteFile(f, newDir, lstrlenW(newDir) * 2, &w, NULL); CloseHandle(f); }
    }
    lstrcpyW(g_dir, newDir);
    PathCombineW(g_ini, g_dir, L"MiniDock.ini");
    lstrcpyW(g_iconDir, newIcons);
    SaveNow();
    BuildRows();
    MessageBoxW(h, L"配置文件位置已更改，并已复制现有配置与图标。", L"MiniDock", MB_OK | MB_ICONINFORMATION);
}

/* delete every stored configuration (settings, pinned icons, application groups, launchpad layout,
   custom icons) and start over with the built-in defaults */
static void ResetEverything(HWND h) {
    ClosePreview();
    for (int i = nItems - 1; i >= 0; i--) FreeItem(&items[i]);
    nItems = 0;
    prevN = -1;
    WCHAR f[MAX_PATH], pat[MAX_PATH];
    PathCombineW(f, g_dir, L"launchpad.dat"); DeleteFileW(f);
    PathCombineW(f, g_dir, L"MiniDock.ini"); DeleteFileW(f);
    PathCombineW(f, g_dir, L"notes.dat"); DeleteFileW(f);
    PathCombineW(f, g_dir, L"profile.txt"); DeleteFileW(f);
    {   /* focus-mode snapshots */
        WCHAR pd[MAX_PATH], pp[MAX_PATH];
        ProfileDir(pd);
        PathCombineW(pp, pd, L"*");
        WIN32_FIND_DATAW fd;
        HANDLE ff = FindFirstFileW(pp, &fd);
        if (ff != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                WCHAR a[MAX_PATH];
                PathCombineW(a, pd, fd.cFileName);
                DeleteFileW(a);
            } while (FindNextFileW(ff, &fd));
            FindClose(ff);
        }
    }
    {   /* custom icon copies live only in our own icon folder */
        WIN32_FIND_DATAW fd;
        PathCombineW(pat, g_iconDir, L"*");
        HANDLE ff = FindFirstFileW(pat, &fd);
        if (ff != INVALID_HANDLE_VALUE) {
            do {
                if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
                WCHAR a[MAX_PATH];
                PathCombineW(a, g_iconDir, fd.cFileName);
                DeleteFileW(a);
            } while (FindNextFileW(ff, &fd));
            FindClose(ff);
        }
    }
    g_dpiCfg = g_dpi;
    DefaultCfg();
    SaveSettingsOnly();
    SaveNow();
    ApplySettings();
    ReloadAllIcons();
    UpdateGeometry();
    Kick();
    MessageBoxW(h, L"已删除所有配置并恢复默认。\n\n现在可以重新把应用拖到 Dock 上，或在“图标与项目”里一键添加常用图标。", L"MiniDock", MB_OK | MB_ICONINFORMATION);
}

/* ---- hotkey capture dialog ---- */
typedef struct { int done, ok, mods, vk; } HkCtx;
static const WCHAR *VkName(int vk, WCHAR *buf) {
    if (vk >= 'A' && vk <= 'Z') { buf[0] = (WCHAR)vk; buf[1] = 0; return buf; }
    if (vk >= '0' && vk <= '9') { buf[0] = (WCHAR)vk; buf[1] = 0; return buf; }
    if (vk >= VK_F1 && vk <= VK_F24) { wsprintfW(buf, L"F%d", vk - VK_F1 + 1); return buf; }
    switch (vk) {
    case VK_SPACE: return L"Space"; case VK_TAB: return L"Tab"; case VK_RETURN: return L"Enter";
    case VK_ESCAPE: return L"Esc"; case VK_BACK: return L"Backspace"; case VK_DELETE: return L"Delete";
    case VK_INSERT: return L"Insert"; case VK_HOME: return L"Home"; case VK_END: return L"End";
    case VK_PRIOR: return L"PageUp"; case VK_NEXT: return L"PageDown";
    case VK_OEM_3: return L"`"; case VK_OEM_MINUS: return L"-"; case VK_OEM_PLUS: return L"=";
    case VK_OEM_4: return L"["; case VK_OEM_6: return L"]"; case VK_OEM_5: return L"\\";
    case VK_OEM_1: return L";"; case VK_OEM_7: return L"'"; case VK_OEM_COMMA: return L",";
    case VK_OEM_PERIOD: return L"."; case VK_OEM_2: return L"/";
    }
    wsprintfW(buf, L"VK %d", vk);
    return buf;
}
static void HkText(WCHAR *out, int n) {
    WCHAR kb[16];
    out[0] = 0;
    if (cfg.hkMods & MOD_CONTROL) lstrcatW(out, L"Ctrl + ");
    if (cfg.hkMods & MOD_ALT) lstrcatW(out, L"Alt + ");
    if (cfg.hkMods & MOD_SHIFT) lstrcatW(out, L"Shift + ");
    if (cfg.hkMods & MOD_WIN) lstrcatW(out, L"Win + ");
    lstrcatW(out, VkName(cfg.hkVk, kb));
}
static LRESULT CALLBACK HkProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    HkCtx *c = (HkCtx *)GetWindowLongPtrW(h, GWLP_USERDATA);
    if (!c) return DefWindowProcW(h, m, w, l);
    switch (m) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {
        if (w == VK_ESCAPE) { c->done = 1; DestroyWindow(h); return 0; }
        if (w == VK_CONTROL || w == VK_MENU || w == VK_SHIFT || w == VK_LWIN || w == VK_RWIN) return 0;
        int mods = 0;
        if (GetKeyState(VK_CONTROL) & 0x8000) mods |= MOD_CONTROL;
        if (GetKeyState(VK_MENU) & 0x8000) mods |= MOD_ALT;
        if (GetKeyState(VK_SHIFT) & 0x8000) mods |= MOD_SHIFT;
        if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000) mods |= MOD_WIN;
        if (!mods && !(w >= VK_F1 && w <= VK_F24)) { SetWindowTextW(h, L"请至少加一个修饰键（Ctrl / Alt / Shift / Win）"); return 0; }
        c->mods = mods; c->vk = (int)w; c->ok = 1; c->done = 1;
        DestroyWindow(h);
        return 0;
    }
    case WM_CLOSE: c->done = 1; DestroyWindow(h); return 0;
    }
    return DefWindowProcW(h, m, w, l);
}
static void CaptureHotkey(HWND owner) {
    static BOOL reg;
    if (!reg) {
        WNDCLASSW wc;
        ZeroMemory(&wc, sizeof wc);
        wc.lpfnWndProc = HkProc; wc.hInstance = g_inst; wc.lpszClassName = L"MiniDockHotkey";
        RegisterClassW(&wc);
        reg = TRUE;
    }
    HkCtx c;
    ZeroMemory(&c, sizeof c);
    int W = Sc(420), H = Sc(120);
    HWND h = CreateWindowExW(WS_EX_TOPMOST | WS_EX_DLGMODALFRAME, L"MiniDockHotkey", L"设置显示 / 隐藏快捷键",
                             WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE, (g_mi.rcWork.left + g_mi.rcWork.right - W) / 2,
                             (g_mi.rcWork.top + g_mi.rcWork.bottom - H) / 2, W, H, owner, NULL, g_inst, &c);
    if (!h) return;
    SetWindowLongPtrW(h, GWLP_USERDATA, (LONG_PTR)&c);
    HWND lbl = CreateWindowExW(0, L"STATIC", L"请按下新的快捷键组合（Esc 取消）", WS_CHILD | WS_VISIBLE | SS_CENTER, Sc(16), Sc(28), W - Sc(32), Sc(30), h, NULL, g_inst, NULL);
    SendMessageW(lbl, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
    ShowWindow(h, SW_SHOW);
    SetForegroundWindow(h);
    SetFocus(h);
    MSG msg;
    while (!c.done && GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    if (c.ok && c.vk) {
        cfg.hkMods = c.mods;
        cfg.hkVk = c.vk;
        EnsureHotkey();
        ScheduleSave();
        Kick();
    }
}

static void DoAct(HWND h, int act) {
    if (act >= A_PEXP && act < A_PEXP + 10) { int t = act - A_PEXP; WCHAR n[64]; wsprintfW(n, L"MiniDock-%s.mdset", g_pageName[t]); ExportScope(h, &t, 1, n); return; }
    if (act >= A_PIMP && act < A_PIMP + 10) { int t = act - A_PIMP; ImportScope(h, &t, 1); return; }
    switch (act) {
    case A_ADDURL: DoAddUrl(h); break;
    case A_ADDFILE: DoAddFile(h); break;
    case A_ADDFOLDER: DoAddFolder(h); break;
    case A_NEWSC: DoNewShortcut(h); break;
    case A_NEWGROUP: NewEmptyGroup(FALSE); UpdateGeometry(); Kick(); break;
    case A_NEWTEMP: NewEmptyGroup(TRUE); UpdateGeometry(); Kick(); break;
    case A_ADDSEP: DoAddSep(-1); break;
    case A_EXP0: ExportConfig(h, 0); break;
    case A_EXP1: ExportConfig(h, 1); break;
    case A_EXP2: ExportConfig(h, 2); break;
    case A_IMPORT: ImportConfig(h); BuildRows(); break;
    case A_STYLEEXP: { int t[2] = { 1, 3 }; ExportScope(h, t, 2, L"MiniDock-Dock样式.mdset"); break; }
    case A_STYLEIMP: { int t[2] = { 1, 3 }; ImportScope(h, t, 2); break; }
    case A_RESET:
        if (MessageBoxW(h, L"确定恢复默认的外观与行为设置吗？（已固定的图标不会被删除）", L"MiniDock", MB_YESNO | MB_ICONQUESTION) == IDYES) { DefaultCfg(); ApplySettings(); ReloadAllIcons(); BuildRows(); }
        break;
    case A_RESETALL:
        if (MessageBoxW(h, L"确定删除所有配置并恢复默认吗？\n\n会清除：全部设置、Dock 上的图标与应用组、起动台布局、自定义图标。\n此操作不可撤销，建议先用“导出完整配置”备份。", L"MiniDock", MB_YESNO | MB_ICONWARNING) != IDYES) break;
        ResetEverything(h);
        BuildRows();
        break;
    case A_OPENDIR: ShellExecuteW(NULL, L"open", g_dir, NULL, NULL, SW_SHOWNORMAL); break;
    case A_LOGEXPORT: LogExport(h); break;
    case A_LOGCLEAR: LogClear(h); break;
    case A_OPENNOTE: ShowNote(); break;
    case A_LPRESET: { WCHAR lf[MAX_PATH]; PathCombineW(lf, g_dir, L"launchpad.dat"); DeleteFileW(lf); } break;
    case A_CHECKUPD: StartUpdateCheck(FALSE); SettingsRedraw(); break;
    case A_OPENREPO: ShellExecuteW(NULL, L"open", GH_REPO, NULL, NULL, SW_SHOWNORMAL); break;
    case A_OPENUPD: ShellExecuteW(NULL, L"open", g_updUrl[0] ? g_updUrl : GH_REPO L"/releases", NULL, NULL, SW_SHOWNORMAL); break;
    case A_LOCCHANGE: {
        BROWSEINFOW b;
        ZeroMemory(&b, sizeof b);
        b.hwndOwner = h; b.lpszTitle = L"选择新的配置文件夹"; b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST pi = SHBrowseForFolderW(&b);
        if (pi) { WCHAR d[MAX_PATH]; if (SHGetPathFromIDListW(pi, d)) { WCHAR full[MAX_PATH]; PathCombineW(full, d, L"MiniDock"); MoveDataDir(h, full); } CoTaskMemFree(pi); }
        break;
    }
    case A_LOCRESET: { WCHAR d[MAX_PATH]; DefaultDataDir(d); MoveDataDir(h, d); break; }
    case A_LOWMODE:
        if (cfg.bgMode == 2) cfg.bgMode = 0;
        cfg.reflect = 0; cfg.clock = 0; cfg.preview = 0; cfg.showRunning = 0; cfg.dots = 0; cfg.minAnim = 0; cfg.groupFrost = 0; cfg.guardian = 0;
        ApplySettings();
        break;
    case A_RECMODE:
        cfg.bgMode = 2; cfg.reflect = 0; cfg.clock = 0; cfg.preview = 1; cfg.showRunning = 1; cfg.dots = 1; cfg.minAnim = 1; cfg.groupFrost = 1; cfg.guardian = 1;
        ApplySettings();
        break;
    case A_HIGHMODE:
        cfg.bgMode = 2; cfg.reflect = 1; cfg.clock = 1; cfg.preview = 1; cfg.showRunning = 1; cfg.dots = 1; cfg.minAnim = 1; cfg.groupFrost = 1; cfg.guardian = 1; cfg.zoom = 1;
        ApplySettings();
        break;
    case A_HKEDIT: CaptureHotkey(h); BuildRows(); if (g_hset) InvalidateRect(g_hset, NULL, FALSE); break;
    case A_TRAYADD:
        AddTrayApp(h);
        ScanRunning(); UpdateGeometry(); Kick(); BuildRows(); SettingsRedraw();
        break;
    case A_GAMEADD: {
        HWND f = GetForegroundWindow();
        DWORD fpid = 0; if (f) GetWindowThreadProcessId(f, &fpid);
        if (!f || fpid == GetCurrentProcessId()) f = g_lastExternalForeground;
        WCHAR exe[MAX_PATH] = L"";
        if (f && IsWindow(f)) ExeOfWindow(f, exe);
        const WCHAR *nm = PathFindFileNameW(exe);
        if (nm && nm[0]) {
            LoadGames();
            int at = -1;
            for (int i = 0; i < g_ngames; i++) if (!lstrcmpiW(g_games[i], nm)) { at = i; break; }
            if (at >= 0) {
                for (int i = at; i + 1 < g_ngames; i++) lstrcpynW(g_games[i], g_games[i + 1], 80);
                g_ngames--;
                WritePrivateProfileStringW(L"Games", NULL, NULL, g_ini);
                WCHAR v[16], k[16]; wsprintfW(v, L"%d", g_ngames); WritePrivateProfileStringW(L"Games", L"N", v, g_ini);
                for (int i = 0; i < g_ngames; i++) { wsprintfW(k, L"G%d", i); WritePrivateProfileStringW(L"Games", k, g_games[i], g_ini); }
                MessageBoxW(h, nm, L"已从游戏列表移除", MB_OK | MB_ICONINFORMATION);
            } else if (g_ngames < MAXGAMES) {
                lstrcpynW(g_games[g_ngames], nm, 80);
                WCHAR k[16], v[16]; wsprintfW(v, L"%d", g_ngames + 1); WritePrivateProfileStringW(L"Games", L"N", v, g_ini);
                wsprintfW(k, L"G%d", g_ngames); WritePrivateProfileStringW(L"Games", k, nm, g_ini); g_ngames++;
                MessageBoxW(h, nm, L"已加入游戏列表", MB_OK | MB_ICONINFORMATION);
            } else MessageBoxW(h, nm, L"游戏列表已满", MB_OK | MB_ICONINFORMATION);
        }
        BuildRows();
        if (g_hset) InvalidateRect(g_hset, NULL, FALSE);
        break;
    }
    case A_ALLWIDGETON:
        cfg.clock = 1; cfg.notes = 1; cfg.notifyW = 1; cfg.ctrlCenter = 1; cfg.resMon = 1; cfg.modeW = 1; cfg.trayW = 1; cfg.toolW = 1; cfg.powerW = 1;
        ApplySettings(); break;   /* reflections keep their own setting */
    case A_ALLWIDGETOFF:
        cfg.clock = 0; cfg.notes = 0; cfg.notifyW = 0; cfg.ctrlCenter = 0; cfg.resMon = 0; cfg.modeW = 0; cfg.trayW = 0; cfg.toolW = 0; cfg.powerW = 0;
        ApplySettings(); break;
    case A_PICKDARKDIR: {
        BROWSEINFOW b;
        ZeroMemory(&b, sizeof b);
        b.hwndOwner = h; b.lpszTitle = L"选择深色模式图标文件夹"; b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST pi = SHBrowseForFolderW(&b);
        if (pi) { SHGetPathFromIDListW(pi, g_darkBatchDir); CoTaskMemFree(pi); RunDarkMatch(); BuildRows(); }
        break;
    }
    case A_DARKMATCH: {
        if (!g_darkBatchDir[0]) { MessageBoxW(h, L"请先选择深色图标文件夹。", L"MiniDock", MB_OK | MB_ICONWARNING); break; }
        RunDarkMatch();
        int ok = 0, miss = 0;
        for (int i = 0; i < nItems; i++) {
            if (items[i].type == T_SEP || items[i].temp) continue;
            if (g_darkState[i] == 1 && ApplyDarkIconFile(i, g_darkMatch[i])) { g_darkState[i] = 3; ok++; }
            else if (g_darkState[i] == 2) miss++;
        }
        cfg.iconDarkMode = cfg.iconDarkMode ? cfg.iconDarkMode : 1;
        ScheduleSave(); Kick(); BuildRows();
        WCHAR msg[160]; wsprintfW(msg, L"已设置 %d 个深色图标。%s", ok, miss ? L"部分项目未找到匹配文件。" : L"");
        MessageBoxW(h, msg, L"深色图标批量设置", MB_OK | (miss ? MB_ICONWARNING : MB_ICONINFORMATION));
        break;
    }
    case A_PICKDIR: {
        BROWSEINFOW b;
        ZeroMemory(&b, sizeof b);
        b.hwndOwner = h; b.lpszTitle = L"选择存放图标的文件夹"; b.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
        LPITEMIDLIST pi = SHBrowseForFolderW(&b);
        if (pi) { SHGetPathFromIDListW(pi, g_batchDir); CoTaskMemFree(pi); RunMatch(); BuildRows(); }
        break;
    }
    case A_AUTOMATCH: {
        if (!g_batchDir[0]) { MessageBoxW(h, L"请先选择图标所在的文件夹。", L"MiniDock", MB_OK | MB_ICONWARNING); break; }
        RunMatch();
        int ok = 0, miss = 0;
        WCHAR list[600] = L"";
        for (int i = 0; i < nItems; i++) {
            if (items[i].type == T_SEP || items[i].temp) continue;
            if (g_mstate[i] == 1 && ApplyIconFile(i, g_match[i])) { g_mstate[i] = 3; ok++; }
            else if (g_mstate[i] == 2) { miss++; if (miss <= 8) { lstrcatW(list, L"\n · "); lstrcatW(list, items[i].name); } }
        }
        ScheduleSave();
        Kick();
        BuildRows();
        WCHAR msg[900];
        if (miss) wsprintfW(msg, L"已更换 %d 个图标。\n\n有 %d 个应用在该文件夹中没有找到对应图标：%s%s", ok, miss, list, miss > 8 ? L"\n · …" : L"");
        else wsprintfW(msg, L"已更换 %d 个图标。", ok);
        MessageBoxW(h, msg, L"批量设置图标", MB_OK | (miss ? MB_ICONWARNING : MB_ICONINFORMATION));
        break;
    }
    default:
        if (act >= A_TRAYREMOVE && act < A_TRAYREMOVE + MAXTRAYAPPS) {
            RemoveTrayApp(act - A_TRAYREMOVE);
            ScanRunning(); UpdateGeometry(); Kick(); BuildRows(); SettingsRedraw();
            break;
        }
        if ((act >= A_ITEM && act < A_ITEM + 200) || (act >= A_DARKITEM && act < A_DARKITEM + 200)) {
            BOOL darkItem = (act >= A_DARKITEM && act < A_DARKITEM + 200);
            int ii = darkItem ? act - A_DARKITEM : act - A_ITEM;
            if (ii >= 0 && ii < nItems) {
                WCHAR f[MAX_PATH] = L"";
                OPENFILENAMEW o;
                ZeroMemory(&o, sizeof o);
                o.lStructSize = sizeof o; o.hwndOwner = h;
                o.lpstrFilter = L"PNG / JPG / ICO\0*.png;*.jpg;*.jpeg;*.ico\0所有文件\0*.*\0";
                o.lpstrFile = f; o.nMaxFile = MAX_PATH; o.Flags = OFN_FILEMUSTEXIST;
                BOOL ok = GetOpenFileNameW(&o) && (darkItem ? ApplyDarkIconFile(ii, f) : ApplyIconFile(ii, f));
                if (ok) {
                    if (darkItem) { g_darkState[ii] = 3; cfg.iconDarkMode = cfg.iconDarkMode ? cfg.iconDarkMode : 1; }
                    else g_mstate[ii] = 3;
                    ScheduleSave(); Kick(); BuildRows(); SettingsRedraw();
                }
            }
        }
        break;
    }
}

static void ClampScroll(void) {
    float mx = g_contentH - g_viewH + Sc(24);
    if (mx < 0) mx = 0;
    if (g_scroll > mx) g_scroll = (int)mx;
    if (g_scroll < 0) g_scroll = 0;
}

static void SwitchPage(int p) {
    g_page = p;
    g_scroll = 0;
    if (g_search && g_searching) { g_searching = FALSE; g_q[0] = 0; SetWindowTextW(g_search, L""); }
    if (p == 27) { RunMatch(); RunDarkMatch(); }
    BuildRows();
    KillTimer(g_hset, 1);
    if (p == 6) SetTimer(g_hset, 1, 1000, NULL);
    SettingsRedraw();
}

static void DarkTitle(HWND h) { BOOL d = g_uiDark; DwmSetWindowAttribute(h, 20, &d, sizeof d); }
static HBRUSH g_sbrush;
static BOOL g_sbrushDark = -1;

static LRESULT CALLBACK SetProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    switch (m) {
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(h, &ps);
        RECT rc;
        GetClientRect(h, &rc);
        if (!g_sbmp || g_sw != rc.right || g_sh != rc.bottom) {
            if (g_sbmp) { SelectObject(g_sdc, g_sold); DeleteObject(g_sbmp); }
            g_sbmp = CreateCompatibleBitmap(dc, rc.right, rc.bottom);
            g_sold = (HBITMAP)SelectObject(g_sdc, g_sbmp);
            g_sw = rc.right; g_sh = rc.bottom;
        }
        PaintSettings(g_sdc, rc.right, rc.bottom);
        ClampScroll();
        BitBlt(dc, 0, 0, rc.right, rc.bottom, g_sdc, 0, 0, SRCCOPY);
        EndPaint(h, &ps);
        return 0;
    }
    case WM_CTLCOLOREDIT: {
        if (g_sbrushDark != g_uiDark || !g_sbrush) {
            if (g_sbrush) DeleteObject(g_sbrush);
            g_sbrush = CreateSolidBrush(g_uiDark ? RGB(52, 54, 62) : RGB(232, 235, 243));
            g_sbrushDark = g_uiDark;
        }
        SetTextColor((HDC)w, g_uiDark ? RGB(236, 237, 242) : RGB(28, 30, 38));
        SetBkColor((HDC)w, g_uiDark ? RGB(52, 54, 62) : RGB(232, 235, 243));
        return (LRESULT)g_sbrush;
    }
    case WM_COMMAND:
        if ((HWND)l == g_search && HIWORD(w) == EN_CHANGE) {
            GetWindowTextW(g_search, g_q, 64);
            g_searching = g_q[0] != 0;
            g_scroll = 0;
            SettingsRedraw();
        }
        return 0;
    case WM_SETTINGCHANGE:
        if (l && !lstrcmpW((LPCWSTR)l, L"ImmersiveColorSet")) { UpdateTheme(); DarkTitle(h); if (g_search) InvalidateRect(g_search, NULL, TRUE); SettingsRedraw(); }
        return 0;
    case WM_GETMINMAXINFO: {
        MINMAXINFO *mm = (MINMAXINFO *)l;
        mm->ptMinTrackSize.x = Sc(720); mm->ptMinTrackSize.y = Sc(520);
        return 0;
    }
    case WM_MOVE: SetFrostGrab(); return 0;
    case WM_SIZE:
        SetFrostGrab();
        if (g_search && w != SIZE_MINIMIZED) {
            float x0 = (float)(Sc(SBW) + Sc(30)), cw = (float)(LOWORD(l) - Sc(SBW) - Sc(60));
            MoveWindow(g_search, (int)(x0 + cw - Sc(230) + Sc(32)), Sc(28), Sc(190), Sc(20), TRUE);
        }
        ClampScroll();
        SettingsRedraw();
        return 0;
    case WM_TIMER: SettingsRedraw(); return 0;
    case WM_MOUSEMOVE: {
        float x = (float)GET_X_LPARAM(l), y = (float)GET_Y_LPARAM(l);
        if (g_rowDrag >= 0) {
            SetSlider(&g_row[g_rowDrag], x);
            UpdateGeometry();
            Kick();
            SettingsRedraw();
            return 0;
        }
        int sh = -1, part = 0, rh = -1;
        if (x < Sc(SBW)) { int i = (int)((y - Sc(78)) / Sc(42)); if (y >= Sc(78) && i >= 0 && i < NENT) sh = i; }
        else rh = RowHit(x, y, &part);
        if (sh != g_sbHot || rh != g_rowHot) {
            g_sbHot = sh; g_rowHot = rh;
            SettingsRedraw();
            TRACKMOUSEEVENT te = { sizeof te, TME_LEAVE, h, 0 };
            TrackMouseEvent(&te);
        }
        return 0;
    }
    case WM_MOUSELEAVE: g_sbHot = -1; g_rowHot = -1; SettingsRedraw(); return 0;
    case WM_MOUSEWHEEL:
        g_scroll -= (int)((short)HIWORD(w) / 120.0 * Sc(54));
        ClampScroll();
        SettingsRedraw();
        return 0;
    case WM_LBUTTONDOWN: {
        float x = (float)GET_X_LPARAM(l), y = (float)GET_Y_LPARAM(l);
        SetFocus(h);
        if (x < Sc(SBW)) {
            int i = (int)((y - Sc(78)) / Sc(42));
            if (y >= Sc(78) && i >= 0 && i < NENT && ParentOf(g_page) != g_ent[i]) SwitchPage(g_ent[i]);
            else if (y >= Sc(78) && i >= 0 && i < NENT && g_page >= SUBBASE) SwitchPage(g_ent[i]);
            return 0;
        }
        if (HasTabs() && y >= Sc(70) && y < Sc(112) && x >= g_x0 && x <= g_x0 + g_cw) {
            BOOL sbg = (g_page == 4 || g_page == 30);
            int t = x > g_x0 + g_cw / 2 ? (sbg ? 30 : 21) : (sbg ? 4 : 1);
            if (t != g_page) SwitchPage(t);
            return 0;
        }
        if (g_page >= SUBBASE && g_page != 21 && !g_searching && x >= g_backRc.left && x <= g_backRc.right && y >= g_backRc.top && y <= g_backRc.bottom) { SwitchPage(ParentOf(g_page)); return 0; }
        int part = 0, i = RowHit(x, y, &part);
        if (i < 0) return 0;
        Row *r = &g_row[i];
        RectF a, b;
        RowRects(r, &a, &b);
        if (r->type == R_TOGGLE && part == 1) {
            if (r->v) *r->v = !*r->v; else SetSpecial(r->act, !GetSpecial(r->act));
            ApplySettings();
            SettingsRedraw();
        } else if (r->type == R_SLIDER) {
            if (part == 2) { g_rowDrag = i; SetCapture(h); SetSlider(r, x); UpdateGeometry(); Kick(); SettingsRedraw(); }
            else if (part == 3) {
                WCHAR t[40], p[96];
                wsprintfW(t, L"%d", *r->v);
                wsprintfW(p, L"输入数值（%d – %d）：", r->mn, r->mx);
                if (InputBox(h, r->label, p, t, 40, NULL, NULL, 0)) {
                    int v = _wtoi(t);
                    if (v < r->mn) v = r->mn;
                    if (v > r->mx) v = r->mx;
                    *r->v = v;
                    ApplySettings();
                    SettingsRedraw();
                }
            }
        } else if (r->type == R_COMBO) {
            CmItem mi[12];
            int n = 0;
            for (int k = 0; k < r->nopts && k < 12; k++) n = CmAdd(mi, n, k + 1, r->opts[k], *r->v == k ? CM_CHECK : 0);
            POINT pt = { (int)(a.X + a.Width / 2), (int)(a.Y + a.Height + 4) };
            ClientToScreen(h, &pt);
            g_menuEdge = E_TOP;
            int res = ShowMenu(mi, n, pt.x, pt.y);
            g_menuEdge = -1;
            if (res > 0) {
                BOOL dock = (r->v == &cfg.theme || r->v == &cfg.plateTheme || r->v == &cfg.plateMode);
                *r->v = res - 1;
                ApplySettings();
                if (dock) RebuildGroups();
                SettingsRedraw();
            }
        } else if (r->type == R_LINK) { SwitchPage(r->mn); }
        else if (r->type == R_BTN || (r->type == R_BTN2 && part == 5)) { DoAct(h, r->act); SettingsRedraw(); }
        else if (r->type == R_BTN2 && part == 6) { DoAct(h, r->act2); SettingsRedraw(); }
        else if (r->type == R_ITEM) { DoAct(h, r->act); BuildRows(); SettingsRedraw(); }
        else if (r->type == R_COLOR) {
            if (part == 7) {
                static COLORREF custom[16];
                CHOOSECOLORW cc;
                ZeroMemory(&cc, sizeof cc);
                cc.lStructSize = sizeof cc; cc.hwndOwner = h; cc.lpCustColors = custom;
                cc.rgbResult = *r->v >= 0 ? (COLORREF)*r->v : RGB(60, 120, 230);
                cc.Flags = CC_FULLOPEN | CC_RGBINIT;
                if (ChooseColorW(&cc)) { *r->v = (int)cc.rgbResult; if (r->v == &cfg.bgColor1) cfg.accentTone = 0; ApplySettings(); }
            } else if (part == 8) { *r->v = -1; ApplySettings(); }
            SettingsRedraw();
        }
        return 0;
    }
    case WM_LBUTTONUP:
        if (g_rowDrag >= 0) { g_rowDrag = -1; ReleaseCapture(); ApplySettings(); SettingsRedraw(); }
        return 0;
    case WM_CLOSE: DestroyWindow(h); return 0;
    case WM_DESTROY:
        KillTimer(h, 1);
        if (g_sbmp) { SelectObject(g_sdc, g_sold); DeleteObject(g_sbmp); g_sbmp = NULL; }
        g_hset = NULL; g_search = NULL;
        SaveNow();
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

/* hand-drawn vector icons for the settings sidebar (no font glyphs) */

static FrostSnap g_setFrost;
static POINT g_setFrostPos = { 1, 1 };
static void SetFrostGrab(void) {
    if (!cfg.uiFrost || !g_hset) { FrostSnapFree(&g_setFrost); return; }
    RECT r;
    if (!GetWindowRect(g_hset, &r)) return;
    POINT np = { r.left, r.top };
    if (g_setFrost.gp && np.x == g_setFrostPos.x && np.y == g_setFrostPos.y && (r.right - r.left) == g_setFrost.w * 3 && (r.bottom - r.top) == g_setFrost.h * 3) return;
    g_setFrostPos = np;
    SetWindowDisplayAffinity(g_hset, 0x11);
    FrostSnapGrab(&g_setFrost, &r);
    SetWindowDisplayAffinity(g_hset, 0);
}
static void ForceForeground(HWND w) {
    if (!w || !IsWindow(w)) return;
    if (IsIconic(w)) ShowWindow(w, SW_RESTORE);
    ShowWindow(w, SW_SHOW);
    DWORD fg = GetWindowThreadProcessId(GetForegroundWindow(), NULL);
    DWORD me = GetCurrentThreadId();
    if (fg && fg != me) AttachThreadInput(fg, me, TRUE);
    SetForegroundWindow(w);
    BringWindowToTop(w);
    SetWindowPos(w, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
    if (fg && fg != me) AttachThreadInput(fg, me, FALSE);
}

static void ShowSettings(int page) {
    if (g_hset) {
        ForceForeground(g_hset);
        if (page >= 0) SwitchPage(page);
        return;
    }
    int W = Sc(900), H = Sc(680);
    RECT wa = g_mi.rcWork;
    /* keep the dialog inside the work area (which already excludes the reserved dock strip and the top
       status bar) so it can never sit on top of the dock */
    if (W > wa.right - wa.left - Sc(40)) W = wa.right - wa.left - Sc(40);
    if (H > wa.bottom - wa.top - Sc(40)) H = wa.bottom - wa.top - Sc(40);
    if (W < Sc(560)) W = Sc(560);
    if (H < Sc(420)) H = Sc(420);
    RECT r = { 0, 0, W, H };
    AdjustWindowRect(&r, WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, FALSE);
    g_page = page < 0 ? 0 : page;
    g_scroll = 0; g_searching = FALSE; g_q[0] = 0;
    if (!g_sdc) { HDC d = GetDC(NULL); g_sdc = CreateCompatibleDC(d); ReleaseDC(NULL, d); }
    UpdateTheme();
    BuildRows();
    {
        int ww = r.right - r.left, hh = r.bottom - r.top;
        int px = wa.left + ((wa.right - wa.left) - ww) / 2;
        int py = wa.top + ((wa.bottom - wa.top) - hh) / 3;
        if (px < wa.left + 8) px = wa.left + 8;
        if (px + ww > wa.right - 8) px = wa.right - 8 - ww;
        if (py < wa.top + 8) py = wa.top + 8;
        if (py + hh > wa.bottom - 8) py = wa.bottom - 8 - hh;
        if (py < wa.top + 8) py = wa.top + 8;
        g_hset = CreateWindowExW(0, CLS_SET, L"MiniDock 设置", WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME | WS_CLIPCHILDREN,
                                 px, py, ww, hh, NULL, NULL, g_inst, NULL);
    }
    if (!g_hset) return;
    HideTaskbarButton(g_hset);
    {
        float x0 = (float)(Sc(SBW) + Sc(30)), cw = (float)(W - Sc(SBW) - Sc(30) - Sc(30));
        g_search = CreateWindowExW(0, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL, (int)(x0 + cw - Sc(230) + Sc(32)), Sc(28), Sc(190), Sc(20), g_hset, (HMENU)900, g_inst, NULL);
        SendMessageW(g_search, WM_SETFONT, (WPARAM)g_uiFont, TRUE);
        SendMessageW(g_search, EM_SETCUEBANNER, TRUE, (LPARAM)L"搜索设置");
    }
    DarkTitle(g_hset);
    ShowWindow(g_hset, SW_SHOWNORMAL);
    SetForegroundWindow(g_hset);
    SetFrostGrab();
    InvalidateRect(g_hset, NULL, FALSE);
    if (g_page == 6) SetTimer(g_hset, 1, 1000, NULL);
    /* the dock is a 常驻 accessory: opening the settings must never make it disappear */
    if (cfg.mode == 0 && g_hwnd) {
        gForceHide = FALSE;
        gHideTgt = 0;
        if (gFsTgt) { gFsTgt = 0; }
        if (gFsHidden) { gFsHidden = FALSE; ShowWindow(g_hwnd, SW_SHOWNA); }
        gFsPeek = TRUE; gFsNear = GetTickCount();
        UpdateGeometry();
        SetWindowPos(g_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        Kick();
    }
}

/* ======================= tray ======================= */
static NOTIFYICONDATAW g_nid;
static void TrayBalloon(const WCHAR *title, const WCHAR *text) {
    NOTIFYICONDATAW n = g_nid;
    n.uFlags = NIF_INFO;
    lstrcpynW(n.szInfoTitle, title, 64);
    lstrcpynW(n.szInfo, text, 256);
    n.dwInfoFlags = NIIF_INFO;
    Shell_NotifyIconW(NIM_MODIFY, &n);
}

static void AddTray(void) {
    ZeroMemory(&g_nid, sizeof g_nid);
    g_nid.cbSize = sizeof g_nid;
    g_nid.hWnd = g_hwnd;
    g_nid.uID = 1;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_APP_TRAY;
    g_nid.hIcon = LoadIconW(g_inst, MAKEINTRESOURCEW(1));
    lstrcpyW(g_nid.szTip, L"MiniDock");
    if (!cfg.trayHide) Shell_NotifyIconW(NIM_ADD, &g_nid);
}

/* ======================= settings -> runtime ======================= */
static BOOL g_hkReg;
static HHOOK g_hkHook;
/* Fallback for the global hotkey: RegisterHotKey fails while another program (a game, an IME, a recorder)
   owns the chord, and that is exactly when the shortcut is needed most. A low-level keyboard hook still
   sees the key in every application, even when the registration is refused. */
static BOOL g_hotDown;
static DWORD g_hotDownTick;
static LRESULT CALLBACK HotKeyProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION && g_hwnd) {
        const KBDLLHOOKSTRUCT *k = (const KBDLLHOOKSTRUCT *)lp;
        if (k && k->vkCode == cfg.hkVk) {
            if (wp == WM_KEYUP || wp == WM_SYSKEYUP) { g_hotDown = FALSE; return CallNextHookEx(g_hkHook, code, wp, lp); }
            if (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN) {
                /* ignore auto-repeat, but never latch forever if a key-up was lost */
                if (g_hotDown && GetTickCount() - g_hotDownTick < 1200) return 1;
                g_hotDown = TRUE; g_hotDownTick = GetTickCount();
                BOOL mod = TRUE;
                if ((cfg.hkMods & MOD_CONTROL) && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) mod = FALSE;
                if ((cfg.hkMods & MOD_ALT) && !(GetAsyncKeyState(VK_MENU) & 0x8000)) mod = FALSE;
                if ((cfg.hkMods & MOD_SHIFT) && !(GetAsyncKeyState(VK_SHIFT) & 0x8000)) mod = FALSE;
                if ((cfg.hkMods & MOD_WIN) && !(GetAsyncKeyState(VK_LWIN) & 0x8000) && !(GetAsyncKeyState(VK_RWIN) & 0x8000)) mod = FALSE;
                if (!(k->flags & LLKHF_INJECTED) && mod) {
                    PostMessageW(g_hwnd, WM_HOTKEY, 1, 0);
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(g_hkHook, code, wp, lp);
}
static void HookHotkey(BOOL on) {
    if (on && !g_hkHook && g_hwnd) g_hkHook = SetWindowsHookExW(WH_KEYBOARD_LL, HotKeyProc, NULL, 0);
    else if (!on && g_hkHook) { UnhookWindowsHookEx(g_hkHook); g_hkHook = NULL; }
}
/* Registering a global hotkey can fail while another program still owns it (games, screen recorders,
   IMEs). Keep trying in the background instead of silently losing the shortcut for the whole session. */
static void EnsureHotkey(void) {
    if (!cfg.hotkey) { if (g_hkReg) { UnregisterHotKey(g_hwnd, 1); g_hkReg = FALSE; } HookHotkey(FALSE); return; }
    if (g_hkReg) { HookHotkey(FALSE); return; }
    g_hkReg = RegisterHotKey(g_hwnd, 1, cfg.hkMods | MOD_NOREPEAT, cfg.hkVk);
    if (!g_hkReg) g_hkReg = RegisterHotKey(g_hwnd, 1, cfg.hkMods, cfg.hkVk);
    HookHotkey(!g_hkReg);
}
static BOOL g_taskbarStateSaved;
static LPARAM g_taskbarOriginalState = ABS_ALWAYSONTOP;
static LPARAM g_taskbarAppliedState;
static BOOL g_taskbarWindowsTouched;
static int g_taskTopApplied = -1;
/* Explorer owns the taskbar position and rejects ABM_SETPOS from other processes, so the edge lives in
   HKCU\...\Explorer\StuckRects3 (Settings[12]: 0 left, 1 top, 2 right, 3 bottom) and needs an Explorer restart. */
static void SetTaskbarEdge(int top) {
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\StuckRects3", 0, KEY_READ | KEY_WRITE, &k) != ERROR_SUCCESS) return;
    BYTE buf[128];
    DWORD sz = sizeof buf, type = 0;
    if (RegQueryValueExW(k, L"Settings", NULL, &type, buf, &sz) == ERROR_SUCCESS && sz > 12) {
        buf[12] = (BYTE)(top ? 1 : 3);
        RegSetValueExW(k, L"Settings", 0, REG_BINARY, buf, sz);
    }
    RegCloseKey(k);
}
static void RestartExplorer(void) {
    STARTUPINFOW si; PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof si); si.cb = sizeof si;
    WCHAR kill[] = L"taskkill.exe /f /im explorer.exe";
    if (CreateProcessW(NULL, kill, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        WaitForSingleObject(pi.hProcess, 4000);
        CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    }
    Sleep(400);
    WCHAR run[] = L"explorer.exe";
    if (CreateProcessW(NULL, run, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    }
}
static void ApplyTaskbarEdge(void) {
    if (g_taskTopApplied < 0) { g_taskTopApplied = cfg.taskTop; return; }
    if (g_taskTopApplied == cfg.taskTop) return;
    BOOL top = cfg.taskTop != 0;
    SetTaskbarEdge(top);
    g_taskTopApplied = cfg.taskTop;
    if (!g_hwnd) return;
    const WCHAR *msg = top
        ? L"任务栏已设为显示在屏幕顶部。\n\n该位置需要重启资源管理器才会生效，现在重启吗？\n（会关闭已打开的文件夹窗口，请先保存工作）"
        : L"任务栏已恢复为显示在屏幕底部。\n\n需要重启资源管理器才会生效，现在重启吗？\n（会关闭已打开的文件夹窗口，请先保存工作）";
    if (MessageBoxW(g_hwnd, msg, L"MiniDock", MB_YESNO | MB_ICONQUESTION) == IDYES) RestartExplorer();
}
static void ApplyTaskbarWindow(HWND t, BOOL show) {
    if (!t || !IsWindow(t)) return;
    g_taskbarWindowsTouched = TRUE;
    LONG ex = GetWindowLongW(t, GWL_EXSTYLE);
    if (cfg.taskTrans && cfg.taskHide == 0) {
        if (!(ex & WS_EX_LAYERED)) SetWindowLongW(t, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        SetLayeredWindowAttributes(t, 0, (BYTE)(255 * (cfg.taskAlpha < 20 ? 20 : cfg.taskAlpha > 100 ? 100 : cfg.taskAlpha) / 100), LWA_ALPHA);
    } else if (ex & WS_EX_LAYERED) {
        SetWindowLongW(t, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
        SetWindowPos(t, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
    if (!show) ShowWindow(t, SW_HIDE);
    else if (cfg.taskHide == 1) {
        /* Let Explorer own the reveal/hide animation after ABM_SETSTATE. */
        ShowWindow(t, SW_SHOWNA);
    } else {
        ShowWindow(t, SW_SHOWNA);
        SetWindowPos(t, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }
}
static LPARAM TaskbarStateForMode(LPARAM original) {
    LPARAM state = original & ~ABS_AUTOHIDE;
    /* Merely hiding a normal top taskbar leaves its full height reserved. Use the
       Shell's auto-hide state to release that strip before hiding its window. */
    if (cfg.taskHide == 1 || (cfg.statusBar && cfg.sbMaxAvoid && SbHiddenTopTaskbar())) state |= ABS_AUTOHIDE;
    return state;
}
static void RestoreTaskbarState(void) {
    if (!g_taskbarStateSaved) return;
    APPBARDATA ab = { sizeof ab };
    ab.hWnd = FindWindowW(L"Shell_TrayWnd", NULL);
    if (!ab.hWnd) return;
    if ((LPARAM)SHAppBarMessage(ABM_GETSTATE, &ab) != g_taskbarAppliedState) return;
    ab.lParam = g_taskbarOriginalState;
    SHAppBarMessage(ABM_SETSTATE, &ab); /* undo only our temporary state, not later user changes */
}
static BOOL CALLBACK ShowTaskbarOnExit(HWND window, LPARAM unused) {
    (void)unused;
    WCHAR cls[64];
    if (!GetClassNameW(window, cls, 64)) return TRUE;
    if (!lstrcmpW(cls, L"Shell_TrayWnd") || !lstrcmpW(cls, L"Shell_SecondaryTrayWnd")) {
        ShowWindow(window, SW_SHOWNA);
        SetWindowPos(window, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        APPBARDATA ab = { sizeof ab }; ab.hWnd = window;
        SHAppBarMessage(ABM_WINDOWPOSCHANGED, &ab);
    }
    return TRUE;
}
static void RestoreTaskbar(void) {
    if (!g_taskbarWindowsTouched && !g_taskbarStateSaved) return;
    RestoreTaskbarState();
    EnumWindows(ShowTaskbarOnExit, 0); /* restore all secondary taskbars, not only the first one */
    g_taskbarWindowsTouched = g_taskbarStateSaved = FALSE;
}
static void ApplyTaskbar(void) {
    HWND t = FindWindowW(L"Shell_TrayWnd", NULL);
    HWND s = FindWindowW(L"Shell_SecondaryTrayWnd", NULL);
    if (t && !g_taskbarStateSaved) {
        APPBARDATA get;
        ZeroMemory(&get, sizeof get); get.cbSize = sizeof get; get.hWnd = t;
        /* Zero is a valid original state (not an error); preserve it exactly. */
        g_taskbarOriginalState = SHAppBarMessage(ABM_GETSTATE, &get);
        g_taskbarStateSaved = TRUE;
    }
    if (t) {
        APPBARDATA ab;
        ZeroMemory(&ab, sizeof ab); ab.cbSize = sizeof ab; ab.hWnd = t;
        ab.lParam = TaskbarStateForMode(g_taskbarStateSaved ? g_taskbarOriginalState : ABS_ALWAYSONTOP);
        g_taskbarAppliedState = ab.lParam;
        SHAppBarMessage(ABM_SETSTATE, &ab);
        SHAppBarMessage(ABM_WINDOWPOSCHANGED, &ab);
    }
    ApplyTaskbarEdge();   /* taskbar on top is applied through StuckRects3 + an Explorer restart */
    BOOL show = cfg.taskHide != 2;
    ApplyTaskbarWindow(t, show);
    ApplyTaskbarWindow(s, show);
    if (s) { APPBARDATA ab; ZeroMemory(&ab, sizeof ab); ab.cbSize = sizeof ab; ab.hWnd = s; SHAppBarMessage(ABM_WINDOWPOSCHANGED, &ab); }
    if (g_hwnd) UpdateGeometry();
}

static void ApplySettings(void) {
    static DWORD lastGrpSig;
    static int lastLog = -1;
    static BOOL lastSmooth;
    if (lastLog != cfg.logCollect) { lastLog = cfg.logCollect; if (cfg.logCollect) LogEvent(L"log-collection-enabled", 0); }
    static int lastTrayHide = -1, lastMinAnim = -1, lastNotes = -1, lastClock = -1, lastNotifyW = -1, lastPin = -2;
    if (cfg.zoomSize < cfg.iconSize) cfg.zoomSize = cfg.iconSize;
    int need = CacheFor(Pxs(cfg.zoom ? (cfg.zoomSize > cfg.iconSize ? cfg.zoomSize : cfg.iconSize) : cfg.iconSize));
    if (need > g_cacheSize) { g_cacheSize = need; ReloadAllIcons(); }
    UpdateTheme();
    RefreshAccent();
    if (cfg.mode == 0) { gHideTgt = 0; KillTimer(g_hwnd, T_HIDE); }
    else { SetTimer(g_hwnd, T_HIDE, 120, NULL); gLastNear = GetTickCount(); }
    if (cfg.trayHide != lastTrayHide) { lastTrayHide = cfg.trayHide; if (cfg.trayHide) Shell_NotifyIconW(NIM_DELETE, &g_nid); else AddTray(); }
    EnsureHotkey();
    if (cfg.minAnim != lastMinAnim) { lastMinAnim = cfg.minAnim; SetMinHook(cfg.minAnim); }
    BOOL smooth = cfg.minAnim && cfg.minStyle == MIN_STYLE_SMOOTH;
    if (smooth != lastSmooth) {
        lastSmooth = smooth;
        if (smooth) { if (HdrActive()) WgcPrewarm(); }
        else WgcShutdown();
    }
    { UINT ms = (UINT)(cfg.frostHold < 1 ? 1 : cfg.frostHold > 10 ? 10 : cfg.frostHold) * 1000; if (g_hwnd) SetTimer(g_hwnd, T_FROST, ms, NULL); }
    HideTaskbarButton(g_hwnd);
    {   /* rebuilding every group icon is expensive: only do it when something they are drawn from changed
           (this used to run on every slider movement, which is what made the settings page stutter) */
        DWORD sig = 2166136261u;
        int v[9] = { cfg.plateMode, cfg.plateTheme, cfg.plateRadius, cfg.plateOpacity, cfg.grpBg, cfg.grpOpacity, cfg.groupFrost, g_dark, nItems };
        for (int i = 0; i < 9; i++) { sig ^= (DWORD)v[i]; sig *= 16777619u; }
        if (sig != lastGrpSig) { lastGrpSig = sig; RebuildGroups(); }
    }
    ApplyTaskbar();
    {
        static int lastRunning = -1, lastGroupRunning = -1;
        if (g_hwnd && (lastRunning != cfg.showRunning || lastGroupRunning != cfg.groupRunning)) {
            lastRunning = cfg.showRunning; lastGroupRunning = cfg.groupRunning;
            ScanRunning();
        }
    }
    if (cfg.guardian) SpawnGuardian();
    if (cfg.notes != lastNotes) { lastNotes = cfg.notes; if (cfg.notes) AddNote(); else RemoveNote(); }
    if (cfg.clock != lastClock) {
        lastClock = cfg.clock;
        if (cfg.clock) SetTimer(g_hwnd, T_CLOCK, cfg.clockSec ? 1000 : 20000, NULL); else KillTimer(g_hwnd, T_CLOCK);
    }
    if (cfg.notifyW != lastNotifyW) {
        lastNotifyW = cfg.notifyW;
        RegisterShellHookWindow(g_hwnd);   /* the red dots and the status-bar icons need it even without the widget */
    }
    if (lastPin != cfg.pinMenu) { lastPin = cfg.pinMenu; SetPinMenu(cfg.pinMenu != 0); }
    ClosePreview();
    UpdateGeometry();
    { static int lastEco = -1; if (lastEco < 0) lastEco = cfg.ecoMode; else if (cfg.ecoMode != lastEco) {
        lastEco = cfg.ecoMode;
        if (cfg.ecoMode) { cfg.bgMode = 0; cfg.groupFrost = 0; cfg.lpFrost = 0; cfg.uiFrost = 0; cfg.menuFrost = 0; cfg.preview = 0; cfg.minAnim = 0; }
        else { cfg.bgMode = 2; cfg.groupFrost = 1; cfg.lpFrost = 1; cfg.uiFrost = 1; cfg.menuFrost = 1; cfg.preview = 1; cfg.minAnim = 1; }
      } }
    if (cfg.bgMode == 2) UpdateFrost();
    Kick();
    ScheduleSave();
    if (g_hset) { DarkTitle(g_hset); InvalidateRect(g_hset, NULL, FALSE); }
    NoteRethemed();
}

/* ======================= system context menu entry + update check ======================= */
static void SetPinMenu(BOOL on) {   /* "Pin to MiniDock" in the Explorer / desktop right-click menu (per user, no admin rights needed) */
    static const WCHAR *roots[2] = { L"Software\\Classes\\*\\shell\\MiniDockPin", L"Software\\Classes\\Directory\\shell\\MiniDockPin" };
    WCHAR exe[MAX_PATH], cmd[MAX_PATH + 24], ico[MAX_PATH + 4];
    GetModuleFileNameW(NULL, exe, MAX_PATH);
    wsprintfW(cmd, L"\"%s\" --pin \"%%1\"", exe);
    wsprintfW(ico, L"\"%s\"", exe);
    for (int i = 0; i < 2; i++) {
        if (!on) { SHDeleteKeyW(HKEY_CURRENT_USER, roots[i]); continue; }
        HKEY k;
        if (RegCreateKeyExW(HKEY_CURRENT_USER, roots[i], 0, NULL, 0, KEY_WRITE, NULL, &k, NULL) == ERROR_SUCCESS) {
            RegSetValueExW(k, NULL, 0, REG_SZ, (const BYTE *)L"固定到 MiniDock", (DWORD)(sizeof(L"固定到 MiniDock")));
            RegSetValueExW(k, L"Icon", 0, REG_SZ, (const BYTE *)ico, (DWORD)((lstrlenW(ico) + 1) * sizeof(WCHAR)));
            RegCloseKey(k);
        }
        WCHAR sub[300];
        wsprintfW(sub, L"%s\\command", roots[i]);
        if (RegCreateKeyExW(HKEY_CURRENT_USER, sub, 0, NULL, 0, KEY_WRITE, NULL, &k, NULL) == ERROR_SUCCESS) {
            RegSetValueExW(k, NULL, 0, REG_SZ, (const BYTE *)cmd, (DWORD)((lstrlenW(cmd) + 1) * sizeof(WCHAR)));
            RegCloseKey(k);
        }
    }
}

/* add a file / folder / link to the dock (before the recycle bin if there is one) */
static void PinPath(const WCHAR *path) {
    if (!path || !path[0] || nItems >= MAXI - 1) return;
    for (int i = 0; i < nItems; i++) if (items[i].type != T_GROUP && lstrcmpiW(items[i].path, path) == 0) return;   /* already pinned */
    Item it;
    ZeroMemory(&it, sizeof it);
    if (!BuildItem(&it, path)) return;
    int pos = nItems;
    for (int i = 0; i < nItems; i++) if (items[i].type == T_TRASH) { pos = i; break; }
    InsertItemStruct(pos, &it, FALSE);
    ScheduleSave();
    UpdateGeometry();
    Kick();
}

static volatile LONG g_updBusy;
static void TrayBalloon(const WCHAR *title, const WCHAR *text);

static int VerNum(const char *s) {   /* "v1.4.0" -> 10400 */
    int v[3] = { 0, 0, 0 }, k = 0;
    while (*s && (*s < '0' || *s > '9')) s++;
    while (*s && k < 3) {
        if (*s >= '0' && *s <= '9') v[k] = v[k] * 10 + (*s - '0');
        else if (*s == '.') k++;
        else break;
        s++;
    }
    return v[0] * 10000 + v[1] * 100 + v[2];
}

static DWORD WINAPI UpdThread(LPVOID pp) {
    WCHAR dir[MAX_PATH], tmp[MAX_PATH], url[220];
    GetTempPathW(MAX_PATH, dir);
    PathCombineW(tmp, dir, L"minidock_release.json");
    DeleteFileW(tmp);
    wsprintfW(url, L"https://api.github.com/repos/wang01116/MiniDock/releases/latest?_=%lu", GetTickCount());
    HRESULT hr = URLDownloadToFileW(NULL, url, tmp, 0, NULL);
    lstrcpyW(g_updUrl, GH_REPO L"/releases");
    g_updNew = FALSE;
    char *buf = NULL;
    DWORD n = 0;
    if (SUCCEEDED(hr)) {
        HANDLE h = CreateFileW(tmp, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (h != INVALID_HANDLE_VALUE) {
            DWORD sz = GetFileSize(h, NULL);
            if (sz > 0 && sz < 4 * 1024 * 1024) { buf = (char *)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sz + 1); if (buf) ReadFile(h, buf, sz, &n, NULL); }
            CloseHandle(h);
        }
    }
    DeleteFileW(tmp);
    if (!buf) lstrcpyW(g_updText, L"检查失败：无法连接 GitHub，请检查网络后重试。");
    else {
        char *t = strstr(buf, "\"tag_name\"");
        char tag[64] = "";
        if (t) {
            t = strchr(t + 10, ':');
            if (t) { t = strchr(t, '"'); if (t) { t++; int i = 0; while (*t && *t != '"' && i < 60) tag[i++] = *t++; tag[i] = 0; } }
        }
        char *hu = strstr(buf, "\"html_url\"");
        if (hu && t) {
            hu = strchr(hu + 10, ':');
            if (hu) { hu = strchr(hu, '"'); if (hu) { hu++; char u8[300]; int i = 0; while (*hu && *hu != '"' && i < 298) u8[i++] = *hu++; u8[i] = 0; MultiByteToWideChar(CP_UTF8, 0, u8, -1, g_updUrl, 320); } }
        }
        if (!tag[0]) lstrcpyW(g_updText, L"暂无已发布的版本（仓库还没有 Releases）。");
        else {
            WCHAR wt[64];
            MultiByteToWideChar(CP_UTF8, 0, tag, -1, wt, 64);
            char cur[16];
            WideCharToMultiByte(CP_UTF8, 0, APPVER, -1, cur, 16, NULL, NULL);
            if (VerNum(tag) > VerNum(cur)) { g_updNew = TRUE; wsprintfW(g_updText, L"发现新版本 %s（当前 " APPVER L"）。点击下面的“前往下载”。", wt); }
            else wsprintfW(g_updText, L"已是最新版本（当前 " APPVER L"，GitHub 最新为 %s）。", wt);
        }
        HeapFree(GetProcessHeap(), 0, buf);
    }
    InterlockedExchange(&g_updBusy, 0);
    if (g_hwnd) PostMessageW(g_hwnd, WM_APP_UPD, g_updAuto, 0);
    return 0;
}

static void StartUpdateCheck(BOOL autoMode) {
    if (InterlockedCompareExchange(&g_updBusy, 1, 0) != 0) return;
    g_updAuto = autoMode;
    lstrcpyW(g_updText, L"正在检查更新…");
    HANDLE t = CreateThread(NULL, 0, UpdThread, NULL, 0, NULL);
    if (t) CloseHandle(t); else InterlockedExchange(&g_updBusy, 0);
}

/* ======================= dock window ======================= */
static int WidgetAt(float u, float v) {
    if (!Horiz() && g_wn > 0) {
        int id = g_wk[0];
        if (u >= g_wU[id] && u <= g_wU[id] + M.barH && v >= barTop - 4 && v <= barTop + M.barH + 4) return W_HUB;
    }
    for (int k = 0; k < g_wn; k++) {
        int id = g_wk[k];
        if (u >= g_wU[id] && u <= g_wU[id] + g_wWid[id] && v >= barTop - 4 && v <= barTop + M.barH + 4) return id;
    }
    return -1;
}

static void ShowWidgetPopup(int id) {
    switch (id) {
    case W_CLOCK: ShowCalendar(); break;
    case W_NOTIFY: ShowNotifyList(); break;
    case W_CC: ShowCtrlCenter(); break;
    case W_RES: ShowResMon(); break;
    case W_TOOL: ShowTool(); break;
    case W_MODE: ShowModeMenu(); break;
    case W_TRAY: TrayOpen(); break;
    case W_POWER: ShowPowerMenu(); break;
    }
}
static void ShowWidgetHub(void) {
    CmItem m[W_N]; int n = 0;
    static const WCHAR *names[W_N] = { L"时间与日期", L"通知", L"控制中心", L"资源监视器", L"计时器 / 待办", L"专注模式", L"系统托盘", L"电源" };
    for (int k = 0; k < g_wn; k++) n = CmAdd(m, n, 100 + g_wk[k], names[g_wk[k]], 0);
    if (!n) return;
    /* All enabled widgets share the compact launcher anchor on side docks. */
    RECT a = g_wScr[g_wk[0]];
    for (int k = 0; k < g_wn; k++) g_wScr[g_wk[k]] = a;
    POINT p; GetCursorPos(&p); gMenuOpen = TRUE; ClosePreview();
    int r = ShowMenu(m, n, p.x, p.y); gMenuOpen = FALSE; gLastNear = GetTickCount();
    if (r >= 100 && r < 100 + W_N) ShowWidgetPopup(r - 100);
}

static void ShowWidgetMenu(int id) {
    CmItem m[8];
    int n = 0;
    n = CmAdd(m, n, 1, L"关闭此小组件", CM_RED);
    n = CmAdd(m, n, 2, cfg.wSide ? L"小组件移到 Dock 右侧" : L"小组件移到 Dock 左侧", 0);
    if (g_wn > 1) n = CmAdd(m, n, 3, L"交换小组件顺序", 0);
    n = CmAdd(m, n, 4, L"小组件设置…", 0);
    n = CmAdd(m, n, 5, (cfg.wideMask & (1 << id)) ? L"恢复单倍宽度" : L"双倍宽度（和应用组一样占用两个图标位）", 0);
    POINT p;
    GetCursorPos(&p);
    gMenuOpen = TRUE;
    ClosePreview();
    int r = ShowMenu(m, n, p.x, p.y);
    gMenuOpen = FALSE;
    gLastNear = GetTickCount();
    if (r == 1) { if (id == W_CLOCK) cfg.clock = 0; else if (id == W_CC) cfg.ctrlCenter = 0; else if (id == W_RES) cfg.resMon = 0; else if (id == W_TOOL) cfg.toolW = 0; else if (id == W_MODE) cfg.modeW = 0; else if (id == W_TRAY) cfg.trayW = 0; else if (id == W_POWER) cfg.powerW = 0; else cfg.notifyW = 0; }
    else if (r == 2) cfg.wSide = !cfg.wSide;
    else if (r == 3) cfg.wOrder = !cfg.wOrder;
    else if (r == 5) cfg.wideMask ^= (1 << id);
    else if (r == 4) { ShowSettings(4); Kick(); return; }
    if (r) ApplySettings();
    Kick();
}

static void EndDrag(BOOL commit) {
    int di = dragIdx, mt = mergeT;
    BOOL rem = removing;
    dragging = FALSE; removing = FALSE; dragIdx = -1; mergeT = -1;
    if (commit && di >= 0 && di < nItems) {
        if (rem) { if (!items[di].temp && g_remTick && GetTickCount() - g_remTick > 450) RemoveItem(di); }
        else if (mt >= 0 && mt < nItems) MergeInto(di, mt);
        else ScheduleSave();
        UpdateGeometry();
    }
    g_remTick = 0;
    Kick();
}

static BOOL CursorNearDock(POINT p);
static BOOL DragAwayFromDock(POINT sp) {
    RECT r = g_barRest;
    if (r.right <= r.left || r.bottom <= r.top) return FALSE;
    int pad = Sc(22);
    switch (cfg.edge) {
    case E_TOP: return sp.y > r.bottom + pad;
    case E_LEFT: return sp.x > r.right + pad;
    case E_RIGHT: return sp.x < r.left - pad;
    default: return sp.y < r.top - pad;
    }
}

/* the on-screen box that holds the whole visible dock: the icon bar *and* the widget capsules.
   Auto-hide must treat "mouse over a widget" exactly like "mouse over an icon", otherwise the dock
   hides the moment the pointer reaches the clock and pops back out from the edge at once. */
static void DockVisibleRect(RECT *out) {
    float u0 = barL, u1 = barL + barW;
    for (int k = 0; k < g_wn; k++) {
        int id = g_wk[k];
        if (g_wU[id] < u0) u0 = g_wU[id];
        if (g_wU[id] + g_wWid[id] > u1) u1 = g_wU[id] + g_wWid[id];
    }
    float over = (cfg.zoom ? (M.Z - M.S) : 0) + Sc(6);
    float v0 = barTop - over, v1 = barBottom + Sc(3);
    float x, y, w, h;
    MapR(u0, v0, u1 - u0, v1 - v0, &x, &y, &w, &h);
    out->left = winX + (int)x; out->top = winY + (int)y;
    out->right = out->left + (int)(w + 0.5f); out->bottom = out->top + (int)(h + 0.5f);
}
static BOOL CursorNearDock(POINT p) {
    RECT r;
    DockVisibleRect(&r);
    int pad = Sc(8);
    InflateRect(&r, pad, pad);
    if (r.right - r.left < 8 || r.bottom - r.top < 8) {   /* not laid out yet */
        r.left = winX; r.top = winY; r.right = winX + winW; r.bottom = winY + winH;
    }
    return PtInRect(&r, p) != 0;
}

static void OnAutoHide(void) {
    if (cfg.mode == 0 || gForceHide) return;
    POINT p;
    GetCursorPos(&p);
    RECT mr = g_mi.rcMonitor;
    DWORD now = GetTickCount();
    if (g_fsKind) {
        /* A fullscreen app (a game especially) must never be summoned by touching the bottom edge while
           aiming. Only FsWatch's deliberate peek, or Ctrl+Alt+D, may bring the dock out. */
        if (gFsPeek || (gSummonTick && now - gSummonTick < SUMMON_MS)) {
            gLastNear = now;
            if (gHideTgt) { gHideTgt = 0; Kick(); }
        }
        return;
    }
    if (gSummonTick && now - gSummonTick < SUMMON_MS) {   /* the hotkey just summoned it: keep it out */
        gLastNear = now;
        if (gHideTgt) { gHideTgt = 0; Kick(); }
        return;
    }
    BOOL edgeHit = FALSE;
    int band = Sc(2);
    switch (cfg.edge) {
    case E_TOP: edgeHit = p.y <= mr.top + band && p.x >= winX && p.x < winX + winW; break;
    case E_LEFT: edgeHit = p.x <= mr.left + band && p.y >= winY && p.y < winY + winH; break;
    case E_RIGHT: edgeHit = p.x >= mr.right - band && p.y >= winY && p.y < winY + winH; break;
    default: edgeHit = p.y >= mr.bottom - band && p.x >= winX && p.x < winX + winW; break;
    }
    BOOL near_ = gHideTgt ? edgeHit : CursorNearDock(p);
    if (near_ || gMenuOpen || dragging || gapIdx >= 0 || g_pv) {
        gLastNear = now;
        if (gHideTgt) { gHideTgt = 0; Kick(); }
        return;
    }
    if (cfg.mode == 2) {
        OvCtx oc;
        float bx, by, bw, bh;
        MapR(barL, barTop, barW, M.barH + M.margin, &bx, &by, &bw, &bh);
        oc.bar.left = winX + (int)bx; oc.bar.top = winY + (int)by;
        oc.bar.right = oc.bar.left + (int)bw; oc.bar.bottom = oc.bar.top + (int)bh;
        oc.hit = FALSE;
        EnumWindows(OvProc, (LPARAM)&oc);
        if (!oc.hit) { if (gHideTgt) { gHideTgt = 0; Kick(); } gLastNear = now; return; }
    }
    if (!gHideTgt && now - gLastNear > 520) { gHideTgt = 1; hover = FALSE; mouseU = -1; Kick(); }
}

static DWORD g_hotTick;
static int g_lastHot = -2;

static int g_mergeCand = -1;
static DWORD g_mergeTick;

static void OnHoverTimer(HWND h) {
    POINT p;
    GetCursorPos(&p);
    if (dragging && g_mergeCand >= 0 && mergeT != g_mergeCand && GetTickCount() - g_mergeTick > 220) { mergeT = g_mergeCand; Kick(); }
    if (!gMenuOpen && !dragging) {
        POINT c = p;
        ScreenToClient(h, &c);
        float u, v;
        ToLogical((float)c.x, (float)c.y, &u, &v);
        BOOL inZone = HideAmt() < 0.5f && u >= barL - 4 && u <= barL + barW + 4 && v >= barTop - (cfg.zoom ? M.Z : M.S) - 14 && v <= LH && c.x >= -2 && c.y >= -2 && c.x <= winW + 2 && c.y <= winH + 2;
        if (inZone) {
            hover = TRUE;
            if (u != mouseU || v != mouseV) { mouseU = u; mouseV = v; Kick(); }
        } else if (hover) { hover = FALSE; mouseU = -1; mouseV = -1; hot = -1; Kick(); }
    }
    if (hot != g_lastHot) { g_lastHot = hot; g_hotTick = GetTickCount(); }
    if (g_pv) {
        RECT pr;
        GetWindowRect(g_pv, &pr);
        BOOL inPv = PtInRect(&pr, p);
        if (!inPv && !(hover && hot == pvItem)) ClosePreview();
    } else if (cfg.preview && hover && hot >= 0 && !gMenuOpen && !dragging && hot < nItems && (HasWinKey(&items[hot]) || (items[hot].type == T_URL && items[hot].appmode)) && items[hot].running && items[hot].type != T_WIN && GetTickCount() - g_hotTick > 450) {
        ShowPreview(hot);
    }
    if (!hover && !g_pv) KillTimer(h, T_HOVER);
}

static int pressW = -1;
static DWORD pressKey;          /* identity of the icon that was pressed: a scan must never re-target a click */
static WCHAR pressName[128];
static LRESULT CALLBACK DockProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (g_shellMsg && m == g_shellMsg) {   /* shell hook: taskbar flashes -> notification widget / red dots */
        if (w == 0x8006) NotifyAdd((HWND)l);                                  /* HSHELL_FLASH */
        else if ((w & 0x7fff) == 4 || (w & 0x7fff) == 2) NotifyRemove((HWND)l);   /* activated / destroyed */
        return 0;
    }
    switch (m) {
    case WM_COPYDATA: {   /* "Pin to MiniDock" from the Explorer right-click menu */
        COPYDATASTRUCT *cds = (COPYDATASTRUCT *)l;
        if (cds && cds->dwData == 0x4D44 && cds->lpData && cds->cbData >= sizeof(WCHAR)) {
            WCHAR pth[MAX_PATH];
            lstrcpynW(pth, (const WCHAR *)cds->lpData, MAX_PATH);
            PinPath(pth);
        }
        return TRUE;
    }
    case WM_APP_UPD:
        if (g_hset) InvalidateRect(g_hset, NULL, FALSE);
        if (w && g_updNew) TrayBalloon(L"MiniDock 有新版本", g_updText);
        return 0;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: { PAINTSTRUCT ps; BeginPaint(h, &ps); EndPaint(h, &ps); return 0; }
    case WM_MOUSEMOVE: {
        int x = GET_X_LPARAM(l), y = GET_Y_LPARAM(l);
        float u, v;
        ToLogical((float)x, (float)y, &u, &v);
        if (sepDrag) {   /* drag the separator up/down to resize (fine steps, no idle flicker) */
            POINT sp;
            GetCursorPos(&sp);   /* screen coordinates: the dock window moves/grows while scaling, client coordinates would feed back */
            BOOL side = (cfg.edge == E_LEFT || cfg.edge == E_RIGHT);
            float grow = (float)((side ? sp.x : sp.y) - sepScaleY0);
            if (cfg.edge == E_BOTTOM || cfg.edge == E_RIGHT) grow = -grow;
            int ns = sepScaleIcon0 + (int)floorf(grow * 0.5f + 0.5f);   /* any size from 24 to 128, one pixel at a time */
            if (ns < 24) ns = 24;
            if (ns > 128) ns = 128;
            if (ns != cfg.iconSize) {   /* rebuild only when the size really changes: holding still cannot flicker */
                cfg.iconSize = ns;
                cfg.zoomSize = (int)((float)sepScaleZoom0 * (float)ns / (float)(sepScaleIcon0 > 0 ? sepScaleIcon0 : 1));
                if (cfg.zoomSize < 32) cfg.zoomSize = 32;
                if (cfg.zoomSize > 220) cfg.zoomSize = 220;
                UpdateGeometry();
                Kick();
            }
            return 0;
        }
        if (!gMenuOpen) { mouseU = u; mouseV = v; }
        if (!dragging) {
            int hi = HitIndex(u, v);
            if (hi >= 0 && items[hi].type == T_SEP && cfg.sepScale) SetCursor(LoadCursor(NULL, IDC_SIZENS));   /* resize cursor over the divider */
        }
        if (!hover && !gMenuOpen) {
            hover = TRUE;
            SetTimer(h, T_HOVER, 40, NULL);
            TRACKMOUSEEVENT te = { sizeof te, TME_LEAVE, h, 0 };
            TrackMouseEvent(&te);
        }
        if (GetCapture() == h && pressIdx >= 0) {
            if (!dragging && !cfg.lock && (abs(x - pressPt.x) > 9 || abs(y - pressPt.y) > 9) && !(items[pressIdx].temp && items[pressIdx].type == T_SEP) && items[pressIdx].type != T_WIN && items[pressIdx].type != T_TRAY) {
                dragging = TRUE; dragIdx = pressIdx; ClosePreview();
            }
            if (dragging) {
                dragPtA.x = x; dragPtA.y = y;
                {   /* measure against the whole bar rectangle: the old 1.1-icon threshold fired while a
                       normal re-order drag merely left the icon row, which deleted icons by accident */
                    POINT sp;
                    GetCursorPos(&sp);
                    removing = DragAwayFromDock(sp);
                }
                if (removing) { if (!g_remTick) g_remTick = GetTickCount(); } else g_remTick = 0;
                mergeT = -1;
                if (!removing) {
                    Item *d = &items[dragIdx];
                    int cand = -1;
                    for (int j = 0; j < nItems && cand < 0; j++) {
                        Item *t = &items[j];
                        if (j == dragIdx || t->type == T_SEP || d->type == T_SEP || d->type == T_GROUP) continue;
                        if (t->cur > M.S * 0.5f && fabsf(u - actC[j]) < t->cur * 0.40f) cand = j;
                    }
                    if (cand != g_mergeCand) { g_mergeCand = cand; g_mergeTick = GetTickCount(); }
                    if (cand >= 0) { if (GetTickCount() - g_mergeTick > 220) mergeT = cand; }   /* hover over an icon: hold, then merge into a group */
                    else if (!(d->temp && d->type == T_SEP)) {
                        int ni = 0, pc = PinnedCount();
                        float ub = MapActualToBase(u);
                        for (int i = 0; i < nItems; i++) if (i != dragIdx && baseC[i] < ub) ni++;   /* base slots do not slide: no flip-flop at a boundary */
                        BOOL pin = FALSE;
                        if (d->temp) { if (ni <= pc) pin = TRUE; }
                        else if (pc < nItems && ni > pc - 1) ni = pc - 1;
                        if (ni != dragIdx) {
                            MoveItem(dragIdx, ni);
                            dragIdx = ni; pressIdx = ni;
                            if (pin) { items[ni].temp = 0; SetTimer(h, T_SCAN, 300, NULL); }
                            RenderFrame();
                        } else if (pin) { items[ni].temp = 0; SetTimer(h, T_SCAN, 300, NULL); }
                    }
                }
            }
        }
        Kick();
        return 0;
    }
    case WM_MOUSELEAVE:
        if (!dragging && !gMenuOpen) { hover = FALSE; mouseU = -1; hot = -1; Kick(); }
        return 0;
    case WM_LBUTTONDOWN: {
        float u, v;
        ToLogical((float)GET_X_LPARAM(l), (float)GET_Y_LPARAM(l), &u, &v);
        pressIdx = HitIndex(u, v);
        pressW = pressIdx < 0 ? WidgetAt(u, v) : -1;
        pressKey = 0; pressName[0] = 0;
        if (pressIdx >= 0) {
            pressKey = Fnv(items[pressIdx].path);
            lstrcpynW(pressName, items[pressIdx].name, 128);
        }
        g_remTick = 0;
        pressPt.x = GET_X_LPARAM(l); pressPt.y = GET_Y_LPARAM(l);
        if (pressIdx >= 0 && items[pressIdx].type == T_SEP && cfg.sepScale) {   /* separator: drag up/down scales the dock */
            sepDrag = TRUE;
            { POINT sp0; GetCursorPos(&sp0); sepScaleY0 = (cfg.edge == E_LEFT || cfg.edge == E_RIGHT) ? sp0.x : sp0.y; }
            sepScaleIcon0 = cfg.iconSize;
            sepScaleZoom0 = cfg.zoomSize;
            pressIdx = -1;
            SetCapture(h);
            SetCursor(LoadCursor(NULL, IDC_SIZENS));
            return 0;
        }
        if (pressIdx >= 0 || pressW >= 0) SetCapture(h);
        return 0;
    }
    case WM_LBUTTONUP: {
        if (sepDrag) {
            sepDrag = FALSE;
            if (GetCapture() == h) ReleaseCapture();
            ScheduleSave();
            Kick();
            return 0;
        }
        BOOL was = dragging;
        int pi = pressIdx, pw = pressW;
        pressIdx = -1; pressW = -1;
        if (was) EndDrag(TRUE);   /* commit first: ReleaseCapture() would otherwise cancel the drag via WM_CAPTURECHANGED */
        if (GetCapture() == h) ReleaseCapture();
        if (was) {
            POINT cp;
            GetCursorPos(&cp);
            ScreenToClient(h, &cp);
            float u, v;
            ToLogical((float)cp.x, (float)cp.y, &u, &v);
            if (u < barL || u > barL + barW || v < barTop - M.Z - 16 || v > LH) { hover = FALSE; mouseU = -1; hot = -1; Kick(); }
        } else if (pi >= 0) {
            float u, v;
            ToLogical((float)GET_X_LPARAM(l), (float)GET_Y_LPARAM(l), &u, &v);
            /* The 2.5 s running-app scan can insert / remove icons through a click, and the hover zoom can
               slide the icon a little; re-hit-testing the release point therefore dropped perfectly normal
               clicks (a not-running icon would "do nothing"). The pressed item is identified by its path and
               name, so launch that item as long as it is still the same one. */
            (void)u; (void)v;
            if (pi < nItems && Fnv(items[pi].path) == pressKey && !lstrcmpW(items[pi].name, pressName))
                { ClosePreview(); LaunchItem(pi, (w & MK_SHIFT) != 0); } /* use this click's modifiers, not stale keyboard-queue state */
        } else if (pw >= 0) {   /* a widget: clock -> calendar, bell -> notification list */
            float u, v;
            ToLogical((float)GET_X_LPARAM(l), (float)GET_Y_LPARAM(l), &u, &v);
            if (WidgetAt(u, v) == pw) { ClosePreview(); if (pw == W_HUB) ShowWidgetHub(); else ShowWidgetPopup(pw); }
        }
        return 0;
    }
    case WM_MBUTTONUP: {
        float u, v;
        ToLogical((float)GET_X_LPARAM(l), (float)GET_Y_LPARAM(l), &u, &v);
        int hi = HitIndex(u, v);
        if (hi >= 0) LaunchItem(hi, TRUE);
        return 0;
    }
    case WM_CAPTURECHANGED:
        if (dragging) EndDrag(FALSE);
        return 0;
    case WM_RBUTTONUP: {
        float u, v;
        ToLogical((float)GET_X_LPARAM(l), (float)GET_Y_LPARAM(l), &u, &v);
        int wi = HitIndex(u, v) < 0 ? WidgetAt(u, v) : -1;
        if (wi == W_HUB) ShowWidgetHub(); else if (wi >= 0) ShowWidgetMenu(wi); else ShowMenuAt(HitIndex(u, v), FALSE);
        return 0;
    }
    case WM_TIMER:
        if (w == T_UPD) {
            KillTimer(h, T_UPD);
            FILETIME ft;
            GetSystemTimeAsFileTime(&ft);
            cfg.updLast = (int)(((((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime)) / 864000000000ULL);
            SaveSettingsOnly();
            StartUpdateCheck(TRUE);
            return 0;
        }
        if (w == T_ANIM) {
            if (!RenderFrame()) { KillTimer(h, T_ANIM); gAnimOn = FALSE; timeEndPeriod(1); }
        } else if (w == T_SCAN) {
            KillTimer(h, T_SCAN);
            SetTimer(h, T_SCAN, 3500, NULL);
            if (cfg.hotkey && !g_hkReg) EnsureHotkey();
            {   /* low battery: switch the high-cost effects off automatically, restore them when charging */
                static BOOL autoEco;
                SYSTEM_POWER_STATUS bps;
                if (GetSystemPowerStatus(&bps) && cfg.batEco && bps.BatteryLifePercent != 255) {
                    BOOL low = bps.ACLineStatus == 0 && bps.BatteryLifePercent <= cfg.batLow;
                    if (low && !cfg.ecoMode) { autoEco = TRUE; cfg.ecoMode = 1; ApplySettings(); }
                    else if (!low && autoEco && bps.ACLineStatus == 1 && cfg.ecoMode) { autoEco = FALSE; cfg.ecoMode = 0; ApplySettings(); }
                }
            }
            if (cfg.taskHide == 2) {
                HWND tb = FindWindowW(L"Shell_TrayWnd", NULL);
                HWND st = FindWindowW(L"Shell_SecondaryTrayWnd", NULL);
                BOOL show = FALSE;
                if (tb && cfg.taskPeek && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) {
                    RECT tr, mr = g_mi.rcMonitor;
                    POINT mp;
                    GetCursorPos(&mp);
                    if (GetWindowRect(tb, &tr)) {
                        if (tr.top <= mr.top + 4) show = mp.y < tr.bottom + 40;
                        else if (tr.left <= mr.left + 4) show = mp.x < tr.right + 40;
                        else if (tr.right >= mr.right - 4) show = mp.x > tr.left - 40;
                        else show = mp.y > tr.top - 40;
                    }
                }
                if (tb) SetWindowPos(tb, show ? HWND_TOP : HWND_BOTTOM, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | (show ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
                if (st) SetWindowPos(st, show ? HWND_TOP : HWND_BOTTOM, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | (show ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));
            }
            if (cfg.reserve && cfg.mode == 0 && !g_fsKind && !gFsTgt) {
                static DWORD lastFix;
                DWORD nowt = GetTickCount();
                if (!AppBarSpaceReserved() && nowt - lastFix > 3000) {   /* stale registration: rebuild it */
                    lastFix = nowt;
                    SetupAppBar(FALSE, 0);
                    g_abReg = FALSE; g_abKey = 0;
                    UpdateGeometry();
                } else if (!g_abReg) UpdateGeometry();
            }
            if (cfg.taskHide != 0 || (cfg.reserve && cfg.mode == 0)) ApplyWorkArea();   /* self-heal the work area */
            GrpBarSync();
            if (!gFsTgt) {
                ScanRunning();
                if (!gMenuOpen && !gLpOpen && !g_grpWnd && !g_pv) SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
        }
        else if (w == T_FS) {
            FsWatch();
            if (cfg.reserve || cfg.statusBar) {   /* react to maximise/restore within one 400 ms tick instead of the 3.5 s scan */
                static int lastZoom = -1;
                HWND fg = GetForegroundWindow();
                int zoom = (fg && fg != g_hwnd && fg != g_sb && IsZoomed(fg)) ? 1 : 0;
                if (zoom != lastZoom) { lastZoom = zoom; UpdateGeometry(); SbTick(); }
            }
        }
        else if (w == T_FROST) { if (UpdateFrost()) Kick(); SetTimer(h, T_FROST, (UINT)(cfg.frostHold < 1 ? 1 : cfg.frostHold > 10 ? 10 : cfg.frostHold) * 1000, NULL); }
        else if (w == T_HIDE) OnAutoHide();
        else if (w == T_HOVER) OnHoverTimer(h);
        else if (w == T_SAVE) { KillTimer(h, T_SAVE); SaveNow(); }
        else if (w == T_MIN) KillTimer(g_hwnd, T_MIN);
        else if (w == T_DTLEAVE) {
            KillTimer(h, T_DTLEAVE);
            if (!g_dtOver) { gapIdx = -1; mergeT = -1; g_dtActive = FALSE; g_gapCand = -2; Kick(); }
        }
        else if (w == T_CLOCK) Kick();
        else if (w == T_SB) SbTick();
        else if (w == T_GEOM) {   /* one geometry update per burst of desktop / DPI / appbar messages */
            KillTimer(h, T_GEOM);
            UpdateGeometry();
            Kick();
        }
        else if (w == T_TRIM) KillTimer(g_hwnd, T_TRIM);   /* trimming the working set here only caused page faults */
        return 0;
    case WM_APP_SUMMON:
        if (gForceHide || gHideTgt || gFsTgt || gFsHidden || gHideCur > 0.4f) ToggleHide();
        else { SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE); Kick(); }
        return 0;
    case WM_APP_MINANIM: {   /* complete the fly-in outside the WinEvent callback */
        DWORD tick = g_minTick;
        HWND mw = g_minPend;
        g_minPend = NULL; g_minIdx = -1; g_minTick = 0;
        if (GetTickCount() - tick > 500 || g_animBusy || !cfg.minAnim || !mw || !IsWindow(mw)) {
            if (g_anim && !g_animBusy) AnimFinish();
            if (mw && IsWindow(mw)) { g_selfMin = mw; g_selfMinTick = GetTickCount(); CommitMinimizeKeepPlacement(mw, g_minPlacementValid ? &g_minPlacement : NULL); }
            g_minPlacementValid = FALSE;
            return 0;
        }
        BOOL compositor = g_anim && !g_animDirect;
        g_selfMin = mw; g_selfMinTick = GetTickCount();
        if (compositor && IsWindow(mw)) CommitMinimizeKeepPlacement(mw, g_minPlacementValid ? &g_minPlacement : NULL);
        if (g_anim) FlyRun((float)(cfg.animSpeed < 200 ? 200 : cfg.animSpeed > 1000 ? 1000 : cfg.animSpeed));
        if (g_anim) AnimFinish();
        if (!compositor && IsWindow(mw)) CommitMinimizeKeepPlacement(mw, g_minPlacementValid ? &g_minPlacement : NULL);
        g_minPlacementValid = FALSE;
        return 0;
    }
    case WM_HOTKEY: ToggleHide(); return 0;
    case WM_APP_TRASH: {
        WCHAR *list = (WCHAR *)l;
        if (list) {
            SHFILEOPSTRUCTW fo;
            ZeroMemory(&fo, sizeof fo);
            fo.hwnd = h; fo.wFunc = FO_DELETE; fo.pFrom = list; fo.fFlags = FOF_ALLOWUNDO;
            SHFileOperationW(&fo);
            HeapFree(GetProcessHeap(), 0, list);
            g_trashFull = -1;
            SetTimer(h, T_SCAN, 400, NULL);
        }
        return 0;
    }
    case 0x02E0:   /* WM_DPICHANGED: the dock is now on a monitor with another scaling factor */
    {
        int nd = (int)LOWORD(w);
        if (nd >= 72 && nd <= 480 && nd != g_dpi) { g_dpi = nd; RebuildDockFonts(); }
        UpdateGeometry();
        Kick();
        return 0;
    }
    case WM_DISPLAYCHANGE:
        LogEvent(L"display-change", (DWORD)LOWORD(l));
        /* fall through */
    case WM_SETTINGCHANGE:
        if (m == WM_SETTINGCHANGE && l && !lstrcmpW((LPCWSTR)l, L"ImmersiveColorSet")) {   /* system dark / light switch: react at once */
            BOOL od = g_dark;
            UpdateTheme();
            RefreshAccent();
            RebuildGroups();
            if (od != g_dark) UpdateFrost();
            if (g_hset) { DarkTitle(g_hset); InvalidateRect(g_hset, NULL, FALSE); }
            NoteRethemed();
            Kick();
            return 0;
        }
        if (!SetTimer(h, T_GEOM, 350, NULL)) { UpdateGeometry(); Kick(); }
        return 0;
    case WM_POWERBROADCAST:
        if (w == PBT_APMRESUMEAUTOMATIC || w == PBT_APMRESUMESUSPEND) {
            LogEvent(L"system-resume", (DWORD)w);
            /* Explorer/DWM may rebuild the desktop after resume; force a clean appbar re-registration. */
            SetupAppBar(FALSE, 0);
            g_abReg = FALSE; g_abKey = 0;
            SetTimer(h, T_GEOM, 900, NULL);
            return TRUE;
        }
        break;
    case WM_WTSSESSION_CHANGE:
        if (w == WTS_SESSION_UNLOCK || w == WTS_SESSION_LOGON) {
            LogEvent(w == WTS_SESSION_UNLOCK ? L"session-unlock" : L"session-logon", 0);
            SetupAppBar(FALSE, 0);
            g_abReg = FALSE; g_abKey = 0;
            SetTimer(h, T_GEOM, 700, NULL);
            SetTimer(h, T_SCAN, 1200, NULL);
            return 0;
        }
        return 0;
    case WM_APP_FAIL:
        MessageBoxW(g_hset, L"无法识别这个拖入的项目（例如无法获取网址的网页应用快捷方式）。\n\n请改用右键 →“新建快捷方式”，输入应用路径或网址来添加。", L"MiniDock", MB_OK | MB_ICONINFORMATION);
        return 0;
    case WM_APP_AB:
        /* Only re-lay-out the window. Resetting g_abKey here made every ABN_POSCHANGED trigger another
           ABM_SETPOS, which triggers another ABN_POSCHANGED - an endless storm that the shell eventually
           ignored (and which could stall Explorer). */
        if (w == ABN_POSCHANGED) { UpdateGeometry(); Kick(); }
        else if (w == ABN_FULLSCREENAPP && !l) {   /* a fullscreen app just ended: rebuild the reservation */
            SetupAppBar(FALSE, 0);
            g_abReg = FALSE; g_abKey = 0;
            UpdateGeometry();
            Kick();
        }
        return 0;
    case WM_APP_ICON:
        g_noDl = TRUE;
        for (int i = 0; i < nItems; i++) {
            if (!items[i].fav && (items[i].type == T_URL || items[i].icon_src[0])) LoadItemIcon(&items[i]);
            for (int k = 0; k < items[i].nk; k++)
                if (!items[i].kids[k].fav && (items[i].kids[k].type == T_URL || items[i].kids[k].icon_src[0])) LoadItemIcon(&items[i].kids[k]);
            if (items[i].type == T_GROUP) BuildGroupIcon(&items[i]);
        }
        g_noDl = FALSE;
        Kick();
        return 0;
    case WM_APP_TRAY:
        if (LOWORD(l) == WM_RBUTTONUP || LOWORD(l) == WM_CONTEXTMENU) { g_menuEdge = E_BOTTOM; ShowMenuAt(-1, TRUE); g_menuEdge = -1; }
        else if (LOWORD(l) == WM_LBUTTONDBLCLK) ShowSettings(0);
        return 0;
    case WM_DESTROY:
        LogEvent(L"shutdown", 0);
        WgcShutdown();
        SaveNow();
        GpbFree();
        ClearTrayApps();
        if (g_quitEvt) SetEvent(g_quitEvt);
        SetMinHook(FALSE);
        SetupAppBar(FALSE, 0);
        SbHide(); /* unregister the top AppBar before its window disappears with the process */
        ApplyDockReserveFallback(FALSE);
        RestoreWorkArea();
        RestoreTaskbar(); /* let Explorer recompute the final area after releasing our own reservations */
        if (g_hkReg) { UnregisterHotKey(h, 1); g_hkReg = FALSE; }
        HookHotkey(FALSE);
        ClosePreview();
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
        RevokeDragDrop(h);
        PostQuitMessage(0);
        return 0;
    }
    if (m == g_taskbarMsg && m) {   /* Explorer restarted: the app bar registration is gone */
        AddTray();
        g_abKey = 0;
        g_sbAbReg = FALSE; g_sbAbDirty = TRUE;
        ApplyTaskbar();
        UpdateGeometry();
        SbTick();
        Kick();
        return 0;
    }
    return DefWindowProcW(h, m, w, l);
}

/* ======================= entry ======================= */
int WINAPI WinMain(HINSTANCE hi, HINSTANCE hp, LPSTR cmd, int show) {
    g_inst = hi;
    {   /* guardian mode: wait for the dock process and restart it if it was killed */
        const WCHAR *cl = GetCommandLineW();
        const WCHAR *gp = wcsstr(cl, L"--guard");
        if (gp) return GuardMain((DWORD)_wtoi(gp + 7));
    }
    WCHAR pinArg[MAX_PATH] = L"";
    {   /* "--pin <path>": started from the Explorer right-click entry "Pin to MiniDock" */
        const WCHAR *cl = GetCommandLineW(), *pp = wcsstr(cl, L"--pin");
        if (pp) {
            pp += 5;
            while (*pp == L' ') pp++;
            if (*pp == L'"') { pp++; const WCHAR *e = wcschr(pp, L'"'); int n = e ? (int)(e - pp) : lstrlenW(pp); if (n >= MAX_PATH) n = MAX_PATH - 1; lstrcpynW(pinArg, pp, n + 1); }
            else lstrcpynW(pinArg, pp, MAX_PATH);
        }
    }
    HANDLE mtx = CreateMutexW(NULL, TRUE, L"Local\\MiniDock_Wangcy_Singleton");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        HWND ex = FindWindowW(CLS_DOCK, NULL);
        if (ex && pinArg[0]) {
            COPYDATASTRUCT cd = { 0x4D44, (DWORD)((lstrlenW(pinArg) + 1) * sizeof(WCHAR)), pinArg };
            SendMessageTimeoutW(ex, WM_COPYDATA, 0, (LPARAM)&cd, SMTO_ABORTIFHUNG, 3000, NULL);
        } else if (ex) PostMessageW(ex, WM_APP_SUMMON, 0, 0);   /* already running: bring the dock out, do not open settings */
        return 0;
    }
    g_quitEvt = CreateEventW(NULL, TRUE, FALSE, L"Local\\MiniDock_Wangcy_Quit");
    if (g_quitEvt) ResetEvent(g_quitEvt);
    HDC sdc = GetDC(NULL);
    g_dpi = GetDeviceCaps(sdc, LOGPIXELSX);
    ReleaseDC(NULL, sdc);
    {   /* Windows 11 composites layered windows much faster than Windows 10: adapt the frame interval */
        typedef LONG (WINAPI *pRtlGetVersion)(OSVERSIONINFOW *);
        OSVERSIONINFOW vi;
        ZeroMemory(&vi, sizeof vi);
        vi.dwOSVersionInfoSize = sizeof vi;
        HMODULE nt = GetModuleHandleW(L"ntdll.dll");
        pRtlGetVersion rtl = nt ? (pRtlGetVersion)GetProcAddress(nt, "RtlGetVersion") : NULL;
        if (rtl && rtl(&vi) == 0) g_osBuild = vi.dwBuildNumber;
        g_win11 = (g_osBuild >= 22000);
    }
    {   /* Windows may put a background accessory into efficiency mode, which throttles its timers and
           repaints. Opt out explicitly: this is what "总是被杀后台 / 卡顿" often comes down to. */
        SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);
        typedef BOOL (WINAPI *pSetProcInfo)(HANDLE, int, void *, DWORD);
        HMODULE k32 = GetModuleHandleW(L"kernel32.dll");
        pSetProcInfo spi = k32 ? (pSetProcInfo)GetProcAddress(k32, "SetProcessInformation") : NULL;
        if (spi) {
            struct { unsigned Version, ControlMask, StateMask; } pt;
            pt.Version = 1;
            pt.ControlMask = 0x1 | 0x4;   /* execution speed + timer resolution */
            pt.StateMask = 0;             /* 0 = throttling disabled */
            spi(GetCurrentProcess(), 4 /* ProcessPowerThrottling */, &pt, sizeof pt);
        }
    }
    OleInitialize(NULL);
    INITCOMMONCONTROLSEX ic = { sizeof ic, ICC_BAR_CLASSES };
    InitCommonControlsEx(&ic);
    GdiplusStartupInput gi = { 1, NULL, FALSE, FALSE };
    GdiplusStartup(&g_gdipTok, &gi, NULL);

    InitPaths();
    LoadConfig();
    LogInit();
    LogEvent(L"startup", GetCurrentProcessId());
    LogEvent(L"app-version-1.7.4-hdrsnap.15", 0);
    LogEvent(L"windows-build", g_osBuild);
    LogEvent(L"display-dpi", (DWORD)g_dpi);
    LogEvent(L"display-width", (DWORD)GetSystemMetrics(SM_CXSCREEN));
    LogEvent(L"display-height", (DWORD)GetSystemMetrics(SM_CYSCREEN));
    g_cacheSize = CacheFor(Pxs(cfg.zoom ? (cfg.zoomSize > cfg.iconSize ? cfg.zoomSize : cfg.iconSize) : cfg.iconSize));

    GdipCreateImageAttributes(&g_attr);
    GdipSetImageAttributesWrapMode(g_attr, 3, 0, FALSE);
    GdipCreateImageAttributes(&g_attrFade);
    GdipSetImageAttributesWrapMode(g_attrFade, 3, 0, FALSE);
    {
        float cm[25] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.78f, 0, 0, 0, 0, 0, 1 };
        GdipSetImageAttributesColorMatrix(g_attrFade, 1, TRUE, cm, NULL, 0);
    }
    GdipCreateImageAttributes(&g_attrFrost);
    GdipSetImageAttributesWrapMode(g_attrFrost, 3, 0, FALSE);
    GdipCreateImageAttributes(&g_attrRefl);
    GdipSetImageAttributesWrapMode(g_attrRefl, 3, 0, FALSE);
    GdipCreateImageAttributes(&g_attrRefl2);
    GdipSetImageAttributesWrapMode(g_attrRefl2, 3, 0, FALSE);
    {
        float c1[25] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.30f, 0, 0, 0, 0, 0, 1 };
        float c2[25] = { 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0.11f, 0, 0, 0, 0, 0, 1 };
        GdipSetImageAttributesColorMatrix(g_attrRefl, 1, TRUE, c1, NULL, 0);
        GdipSetImageAttributesColorMatrix(g_attrRefl2, 1, TRUE, c2, NULL, 0);
    }
    RebuildDockFonts();
    GdipCreateStringFormat(0, 0, &g_fmtR);
    GdipSetStringFormatAlign(g_fmtR, 2);
    GdipSetStringFormatLineAlign(g_fmtR, 1);
    GdipSetStringFormatTrimming(g_fmtR, 3);
    GdipCreateStringFormat(0, 0, &g_fmtW);
    GdipCreateStringFormat(0, 0, &g_fmtC);
    GdipSetStringFormatAlign(g_fmtC, 1);
    GdipSetStringFormatLineAlign(g_fmtC, 1);
    GdipSetStringFormatTrimming(g_fmtC, 3);
    GdipCreateStringFormat(0x1000, 0, &g_fmtL);
    GdipSetStringFormatLineAlign(g_fmtL, 1);

    cfUrlW = RegisterClipboardFormatW(L"UniformResourceLocatorW");
    cfUrlA = RegisterClipboardFormatW(L"UniformResourceLocator");
    cfShellIDList = RegisterClipboardFormatW(CFSTR_SHELLIDLIST);
    g_taskbarMsg = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW wc;
    ZeroMemory(&wc, sizeof wc);
    wc.hInstance = hi;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(hi, MAKEINTRESOURCEW(1));
    wc.lpfnWndProc = DockProc; wc.lpszClassName = CLS_DOCK; RegisterClassW(&wc);
    wc.lpfnWndProc = SetProc; wc.lpszClassName = CLS_SET; wc.hbrBackground = NULL; RegisterClassW(&wc);
    wc.lpfnWndProc = InProc; wc.lpszClassName = CLS_IN; wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); RegisterClassW(&wc);
    wc.hbrBackground = NULL;
    wc.lpfnWndProc = MenuProc; wc.lpszClassName = CLS_MENU; RegisterClassW(&wc);
    wc.lpfnWndProc = GrpProc; wc.lpszClassName = CLS_GRP; RegisterClassW(&wc);
    wc.lpfnWndProc = PvProc; wc.lpszClassName = CLS_PV; RegisterClassW(&wc);
    wc.lpfnWndProc = FanProc; wc.lpszClassName = CLS_FAN; RegisterClassW(&wc);
    wc.lpfnWndProc = BackProc; wc.lpszClassName = CLS_BACK; wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); RegisterClassW(&wc);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpfnWndProc = ScProc; wc.lpszClassName = L"MiniDockShortcut"; RegisterClassW(&wc);
    wc.lpfnWndProc = NoteProc; wc.lpszClassName = CLS_NOTE; wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1); RegisterClassW(&wc);
    wc.lpfnWndProc = IcoProc; wc.lpszClassName = CLS_ICOD; RegisterClassW(&wc);
    wc.hbrBackground = NULL;   /* animation token window; the real application HWND is animated directly */
    wc.lpfnWndProc = DefWindowProcW; wc.lpszClassName = CLS_ANIM; RegisterClassW(&wc);

    HDC dc = GetDC(NULL);
    g_memdc = CreateCompatibleDC(dc);
    g_fdc = CreateCompatibleDC(dc);
    ReleaseDC(NULL, dc);

    GetMon();
    LoadItems();
    for (int i = 0; i < nItems; i++) items[i].cur = 0;
    UpdateGeometry();

    g_hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, CLS_DOCK, L"MiniDock", WS_POPUP,
                             winX, winY, winW, winH, NULL, NULL, hi, NULL);
    HideTaskbarButton(g_hwnd);
    WTSRegisterSessionNotification(g_hwnd, NOTIFY_FOR_THIS_SESSION);
    RegisterDragDrop(g_hwnd, &g_dt);
    g_shellMsg = RegisterWindowMessageW(L"SHELLHOOK");
    RegisterShellHookWindow(g_hwnd);   /* taskbar flashes -> notification widget, red dots and status bar icons */
    if (pinArg[0]) PinPath(pinArg);
    if (cfg.updChk) {   /* silent update check, at most once a day */
        FILETIME ft;
        GetSystemTimeAsFileTime(&ft);
        int today = (int)(((((ULONGLONG)ft.dwHighDateTime << 32) | ft.dwLowDateTime)) / 864000000000ULL);
        if (cfg.updLast != today) SetTimer(g_hwnd, T_UPD, 25000, NULL);
    }
    if (!cfg.launchpad) { cfg.launchpad = 1; SysAdd(SYS_LAUNCH); SaveSettingsOnly(); }   /* first run of 1.4: put the launchpad on the dock (removable) */
    AddTray();
    if (GetAutostart()) SetAutostart(TRUE);
    UpdateGeometry();
    ApplySettings();
    ShowWindow(g_hwnd, SW_SHOWNA);
    UpdateGeometry();   /* register the app bar now that the window is visible (a hidden window is refused) */
    UpdateFrost();
    RenderFrame();
    Kick();
    SetTimer(g_hwnd, T_SCAN, 2000, NULL);
    SetTimer(g_hwnd, T_FS, 250, NULL);   /* fullscreen / game watch: 4x per second is plenty */
    SetTimer(g_hwnd, T_FROST, (UINT)(cfg.frostHold < 1 ? 1 : cfg.frostHold > 10 ? 10 : cfg.frostHold) * 1000, NULL);   /* user-adjustable backdrop refresh interval */
    SetTimer(g_hwnd, T_SB, 250, NULL);       /* poll the foreground IME state, including Shift conversion toggles */

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    if (g_hwnd) WTSUnRegisterSessionNotification(g_hwnd);
    for (int i = 0; i < nItems; i++) FreeItem(&items[i]);
    FreeSurface();
    GdiplusShutdown(g_gdipTok);
    OleUninitialize();
    if (mtx) CloseHandle(mtx);
    return 0;
}





























































































































