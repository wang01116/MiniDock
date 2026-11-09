#include <windows.h>
/* Test stubs for the WGC/DirectComposition snapshot module (not linked in unit tests). */
BOOL WgcGrabWindow(HWND hwnd, void *bits, int w, int h) { (void)hwnd; (void)bits; (void)w; (void)h; return FALSE; }
BOOL WgcPrepareCapture(HWND hwnd) { (void)hwnd; return FALSE; }
void WgcCancelCapture(void) {}
BOOL WgcPrimeHdrCapture(HWND hwnd, int w, int h) { (void)hwnd; (void)w; (void)h; return FALSE; }
void WgcDrainHdrCapture(void) {}
BOOL HdrSnapShow(HWND src, int x, int y, int w, int h) { (void)src; (void)x; (void)y; (void)w; (void)h; return FALSE; }
void HdrSnapDestroy(void) {}
HWND HdrSnapWindow(void) { return NULL; }
void HdrSnapProbeInfo(int *px, int *py, int *ref, int *wx, int *wy) {
    if (px) *px = 0;
    if (py) *py = 0;
    if (ref) *ref = 0;
    if (wx) *wx = 0;
    if (wy) *wy = 0;
}
void WgcPrewarm(void) {}
void WgcShutdown(void) {}
