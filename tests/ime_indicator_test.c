#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <imm.h>
#include <stdio.h>

/* Private hidden HWNDs and controlled IME replies: no physical input, layout changes,
   foreground changes or calls into a user's input context. */
static HWND root, focus, ime, foreground, lastTarget;
static DWORD rootThread, focusThread;
static HKL rootLayout, focusLayout;
static BOOL noFocus, noIme, failOpen, failConversion, openState = TRUE;
static BOOL localContext, localKnown = TRUE;
static DWORD conversionState = IME_CMODE_NATIVE;
static int queries, contextReads, releases, posted, keys;
static UINT queryFlags, queryTimeout;
static HKL requestedLayout;
static HWND TestForeground(void) { return foreground; }
static HWND TestShell(void) { return root; }
static DWORD TestThread(HWND h, LPDWORD process) {
    if (h == root || h == focus) { if (process) *process = 123; return h == focus ? focusThread : rootThread; }
    return GetWindowThreadProcessId(h, process);
}
static BOOL TestGui(DWORD thread, PGUITHREADINFO info) {
    (void)thread; info->hwndFocus = noFocus ? NULL : focus; return TRUE;
}
static HKL TestLayout(DWORD thread) { return thread == focusThread ? focusLayout : rootLayout; }
static HWND WINAPI TestIme(HWND h) { lastTarget = h; return noIme ? NULL : ime; }
static LRESULT WINAPI TestSend(HWND h, UINT message, WPARAM command, LPARAM data, UINT flags, UINT timeout, PDWORD_PTR result) {
    (void)data;
    if (h != ime || message != WM_IME_CONTROL) return 0;
    queries++; queryFlags = flags; queryTimeout = timeout;
    if (command == 0x0005) { if (failOpen) return 0; *result = openState; return 1; }
    if (command == 0x0001) { if (failConversion) return 0; *result = conversionState; return 1; }
    return 0;
}
static HIMC WINAPI TestContext(HWND h) { (void)h; contextReads++; return localContext ? (HIMC)(UINT_PTR)1 : NULL; }
static BOOL WINAPI TestConversion(HIMC h, LPDWORD conversion, LPDWORD sentence) {
    (void)h; *conversion = conversionState; *sentence = 0; return localKnown;
}
static BOOL WINAPI TestOpen(HIMC h) { (void)h; return openState; }
static BOOL WINAPI TestRelease(HWND h, HIMC context) { (void)h; (void)context; releases++; return TRUE; }
static int TestLayouts(int count, HKL *layouts) {
    if (count < 2) return 0;
    layouts[0] = (HKL)(UINT_PTR)0x0804; layouts[1] = (HKL)(UINT_PTR)0x0409; return 2;
}
static BOOL WINAPI TestPost(HWND h, UINT message, WPARAM w, LPARAM data) {
    (void)h; (void)w;
    if (message == WM_INPUTLANGCHANGEREQUEST) { posted++; requestedLayout = (HKL)data; return TRUE; }
    return FALSE;
}
static void WINAPI TestKey(BYTE key, BYTE scan, DWORD flags, ULONG_PTR extra) {
    (void)key; (void)scan; (void)flags; (void)extra; keys++;
}
#define GetForegroundWindow TestForeground
#define GetShellWindow TestShell
#define GetWindowThreadProcessId TestThread
#define GetGUIThreadInfo TestGui
#define GetKeyboardLayout TestLayout
#define ImmGetDefaultIMEWnd TestIme
#define SendMessageTimeoutW TestSend
#define ImmGetContext TestContext
#define ImmGetConversionStatus TestConversion
#define ImmGetOpenStatus TestOpen
#define ImmReleaseContext TestRelease
#define GetKeyboardLayoutList TestLayouts
#define PostMessageW TestPost
#define keybd_event TestKey
#include "../MiniDock-source/minidock.c"

static int checks;
#define CHECK(condition, label) do { checks++; if (!(condition)) { fprintf(stderr, "FAIL %d: %s\n", checks, label); return 1; } } while (0)
#define LABEL(text) (!lstrcmpW(ImeLabel(), L##text))
int main(void) {
    g_inst = GetModuleHandleW(NULL); DefaultCfg();
    WNDCLASSW wc = {0}; wc.hInstance = g_inst; wc.lpfnWndProc = DefWindowProcW; wc.lpszClassName = L"MiniDockPrivateImeTest";
    CHECK(RegisterClassW(&wc), "register private window class");
    root = CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName, L"", WS_POPUP, 0, 0, 10, 10, NULL, NULL, g_inst, NULL);
    focus = CreateWindowExW(0, L"EDIT", L"", WS_CHILD, 0, 0, 10, 10, root, NULL, g_inst, NULL);
    ime = CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1, NULL, NULL, g_inst, NULL);
    CHECK(root && focus && ime, "create hidden private root, edit and IME windows");
    foreground = root; rootThread = GetCurrentThreadId() + 100; focusThread = rootThread + 1;
    rootLayout = (HKL)(UINT_PTR)0x0409; focusLayout = (HKL)(UINT_PTR)0x0804;
    CHECK(LABEL("中"), "remote Chinese input displays Chinese even when cross-thread HIMC access would fail");
    CHECK(lastTarget == focus && contextReads == 0, "query targets the focused child and does not read its remote input context");
    CHECK(queryTimeout == 20 && (queryFlags & SMTO_ABORTIFHUNG) && (queryFlags & SMTO_ERRORONEXIT), "remote queries have a short timeout and abort policy");
    conversionState = 0;
    CHECK(LABEL("EN"), "Chinese-layout English conversion is English without a layout switch");
    conversionState = IME_CMODE_NATIVE;
    CHECK(LABEL("中"), "a Shift-style conversion change is read again without stale caching");
    openState = FALSE;
    CHECK(LABEL("EN"), "a closed IME is English even if its native conversion bit remains set");
    openState = TRUE; conversionState = IME_CMODE_FULLSHAPE;
    CHECK(LABEL("EN"), "full-width alphanumeric mode is not misreported as native Chinese");
    conversionState = IME_CMODE_NATIVE; focusLayout = (HKL)(UINT_PTR)0x1004;
    CHECK(LABEL("中"), "all Chinese sublanguages use the Chinese-mode check");
    focusLayout = (HKL)(UINT_PTR)0x0409; int before = queries;
    CHECK(LABEL("EN") && queries == before, "English layout bypasses IME IPC entirely");
    focusLayout = (HKL)(UINT_PTR)0x0804; failOpen = TRUE;
    CHECK(LABEL("IME") && contextReads == 0, "timeout or access denial is not falsely labelled English or read via remote HIMC");
    failOpen = FALSE; failConversion = TRUE;
    CHECK(LABEL("IME"), "an unavailable conversion query uses a neutral indicator");
    failConversion = FALSE; noIme = TRUE;
    CHECK(LABEL("IME"), "a missing default IME window uses a neutral indicator");
    focusThread = GetCurrentThreadId(); localContext = TRUE;
    CHECK(LABEL("中") && releases == 1, "the local UI thread can use a balanced IMM fallback");
    openState = FALSE;
    CHECK(LABEL("EN") && releases == 2, "local fallback also handles a closed IME");
    openState = TRUE; localKnown = FALSE;
    CHECK(LABEL("IME") && releases == 3, "failed local conversion still releases its context");
    localKnown = TRUE; localContext = noIme = FALSE; focusThread = rootThread + 1;
    noFocus = TRUE; rootLayout = (HKL)(UINT_PTR)0x0804;
    CHECK(LABEL("中") && lastTarget == root, "missing child focus falls back to the foreground thread IME");
    noFocus = FALSE; rootLayout = focusLayout = (HKL)(UINT_PTR)0x0804;
    CHECK(LABEL("中"), "begin click test in Chinese mode");
    ToggleIme();
    CHECK(posted == 1 && requestedLayout == (HKL)(UINT_PTR)0x0409 && keys == 0, "existing click behavior requests a layout switch without sending test keystrokes");
    CHECK(LABEL("中"), "the label is not optimistically flipped before the application processes its request");
    rootLayout = focusLayout = requestedLayout;
    CHECK(LABEL("EN"), "the label follows the completed asynchronous click switch");
    foreground = g_hwnd = ime; rootLayout = focusLayout = (HKL)(UINT_PTR)0x0804;
    CHECK(LABEL("中") && lastTarget == focus, "Dock foreground fallback resolves a target without reading a Dock context");
    foreground = root; g_hwnd = NULL;
    DestroyWindow(ime); DestroyWindow(root);
    printf("PASS %d IME indicator checks\n", checks); return 0;
}
