// DirectComposition HDR snapshot: capture the preview window as an FP16 texture (GPU side),
// show it as real scRGB HDR content so DWM does not re-map an SDR snapshot and whiten it.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dxgi1_4.h>
#include <dcomp.h>
#include <math.h>
#include <stdlib.h>

extern "C" {
BOOL WgcCaptureHdrTexture(HWND hwnd, void **outTex, int *outW, int *outH);
ID3D11Device *WgcDevice(void);
ID3D11DeviceContext *WgcContext(void);
}

static double HHalf(unsigned short v) {
    unsigned s = (v >> 15) & 1, e = (v >> 10) & 0x1f, m = v & 0x3ff;
    double d;
    if (e == 0) d = m ? ldexp((double)m, -24) : 0.0;
    else if (e == 31) d = 0.0;
    else d = ldexp((double)(m + 1024), (int)e - 25);
    return s ? -d : d;
}
static double HSrgb(double l) {
    if (l <= 0.0031308) return 12.92 * l;
    if (l > 1) l = 1;
    return 1.055 * pow(l, 1.0 / 2.4) - 0.055;
}

static LRESULT CALLBACK HdrSnapProc(HWND h, UINT m, WPARAM w, LPARAM l) {
    if (m == WM_NCHITTEST) return HTTRANSPARENT;   /* clicks go to the restored window below */
    if (m == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    return DefWindowProcW(h, m, w, l);
}

static HWND g_wnd = NULL;
static IDXGISwapChain1 *g_swap = NULL;
static IDCompositionDevice *g_dcomp = NULL;
static IDCompositionTarget *g_target = NULL;
static IDCompositionVisual *g_visual = NULL;
static RECT g_rect = {0,0,0,0};
static int g_probeX = 0, g_probeY = 0, g_probeRef = 0;
static BOOL g_ok = FALSE;

extern "C" void HdrSnapDestroy(void) {
    if (g_visual) { g_visual->Release(); g_visual = NULL; }
    if (g_target) { g_target->Release(); g_target = NULL; }
    if (g_dcomp) { g_dcomp->Release(); g_dcomp = NULL; }
    if (g_swap) { g_swap->Release(); g_swap = NULL; }
    if (g_wnd) { DestroyWindow(g_wnd); g_wnd = NULL; }
    g_ok = FALSE;
}
extern "C" HWND HdrSnapWindow(void) { return g_wnd; }
extern "C" void HdrSnapProbeInfo(int *px, int *py, int *ref, int *wx, int *wy) {
    if (px) *px = g_probeX;
    if (py) *py = g_probeY;
    if (ref) *ref = g_probeRef;
    if (wx) *wx = (int)g_rect.left;
    if (wy) *wy = (int)g_rect.top;
}
extern "C" BOOL HdrSnapPainted(void) {
    if (!g_ok || !g_wnd) return TRUE;
    HDC screen = GetDC(NULL);
    if (!screen) return TRUE;
    int sum = 0, n = 0;
    for (int yy = 0; yy < 4; yy += 2)
        for (int xx = 0; xx < 4; xx += 2) {
            COLORREF c = GetPixel(screen, (int)g_rect.left + g_probeX + xx, (int)g_rect.top + g_probeY + yy);
            if (c != CLR_INVALID) {
                sum += (GetRValue(c) * 299 + GetGValue(c) * 587 + GetBValue(c) * 114) / 1000;
                n++;
            }
        }
    ReleaseDC(NULL, screen);
    if (!n) return TRUE;
    int need = g_probeRef / 3;
    if (need < 18) need = 18;
    return (sum / n) >= need;
}

extern "C" BOOL HdrSnapShow(HWND src, int x, int y, int w, int h) {
    if (!src || w < 32 || h < 32) return FALSE;
    HdrSnapDestroy();
    void *texv = NULL;
    int cw = 0, ch = 0;
    if (!WgcCaptureHdrTexture(src, &texv, &cw, &ch) || !texv) return FALSE;

    ID3D11Texture2D *tex = (ID3D11Texture2D *)texv;
    ID3D11Device *dev = WgcDevice();
    ID3D11DeviceContext *ctx = WgcContext();
    if (!dev || !ctx) {  tex->Release(); return FALSE; }

    /* probe hole: read a 512x512 centre patch back and pick the brightest 16x16 cell */
    int rw = cw < 512 ? cw : 512, rh = ch < 512 ? ch : 512;
    int rx0 = (cw - rw) / 2, ry0 = (ch - rh) / 2;
    int bx = -1, by = -1, best = 0;
    D3D11_TEXTURE2D_DESC sdesc;
    ZeroMemory(&sdesc, sizeof sdesc);
    sdesc.Width = rw; sdesc.Height = rh; sdesc.MipLevels = 1; sdesc.ArraySize = 1;
    sdesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; sdesc.SampleDesc.Count = 1;
    sdesc.Usage = D3D11_USAGE_STAGING; sdesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    ID3D11Texture2D *stg = NULL;
    if (SUCCEEDED(dev->CreateTexture2D(&sdesc, NULL, &stg))) {
        D3D11_BOX box = { (UINT)rx0, (UINT)ry0, 0, (UINT)(rx0 + rw), (UINT)(ry0 + rh), 1 };
        ctx->CopySubresourceRegion(stg, 0, 0, 0, 0, tex, 0, &box);
        D3D11_MAPPED_SUBRESOURCE m;
        if (SUCCEEDED(ctx->Map(stg, 0, D3D11_MAP_READ, 0, &m))) {
            for (int yy = 8; yy + 16 < rh; yy += 16)
                for (int xx = 8; xx + 16 < rw; xx += 16) {
                    int sum = 0;
                    for (int k = 0; k < 16; k += 4)
                        for (int j = 0; j < 16; j += 4) {
                            unsigned short *q = (unsigned short *)((BYTE *)m.pData + (SIZE_T)(yy + k) * m.RowPitch) + (SIZE_T)(xx + j) * 4;
                            double lin = HHalf(q[0]) / 4.25;
                            sum += (int)(HSrgb(lin) * 255.0);
                        }
                    if (sum > best) { best = sum; bx = xx + rx0; by = yy + ry0; }
                }
            ctx->Unmap(stg, 0);
        }
        stg->Release();
    }
    if (bx < 0 || best < 16 * 4 * 40) {  tex->Release(); return FALSE; }
    int ref = 0;
    /* ref is re-derived from the mapped patch above only approximately; read the chosen 4x4 again */
    ref = (int)(best / 16.0 * 4.0 / 4.0);   /* best has 16x4 samples; average of 4x4 */

    static BOOL clsReg = FALSE;
    if (!clsReg) {
        WNDCLASSW wc;
        ZeroMemory(&wc, sizeof wc);
        wc.hInstance = GetModuleHandleW(NULL);
        wc.lpfnWndProc = HdrSnapProc;
        wc.lpszClassName = L"MiniDockHdrSnap";
        RegisterClassW(&wc);
        clsReg = TRUE;
    }
    g_wnd = CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
                            L"MiniDockHdrSnap", L"", WS_POPUP, x, y, w, h, NULL, NULL, GetModuleHandleW(NULL), NULL);
    if (!g_wnd) {  tex->Release(); HdrSnapDestroy(); return FALSE; }

    IDXGIDevice *dxgiDev = NULL;
    dev->QueryInterface(__uuidof(IDXGIDevice), (void **)&dxgiDev);
    IDXGIAdapter *adapter = NULL;
    if (dxgiDev) dxgiDev->GetAdapter(&adapter);
    IDXGIFactory2 *factory = NULL;
    if (adapter) {
        IDXGIFactory1 *f1 = NULL;
        if (SUCCEEDED(adapter->GetParent(__uuidof(IDXGIFactory1), (void **)&f1)) && f1) {
            f1->QueryInterface(__uuidof(IDXGIFactory2), (void **)&factory);
            f1->Release();
        }
    }
    if (!factory) {  tex->Release(); if (dxgiDev) dxgiDev->Release(); if (adapter) adapter->Release(); HdrSnapDestroy(); return FALSE; }
    DXGI_SWAP_CHAIN_DESC1 sd;
    ZeroMemory(&sd, sizeof sd);
    sd.Width = w; sd.Height = h;
    sd.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    sd.Stereo = FALSE; sd.SampleDesc.Count = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2; sd.Scaling = DXGI_SCALING_STRETCH;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    sd.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
    HRESULT hr = factory->CreateSwapChainForComposition(dev, &sd, NULL, &g_swap);
    factory->Release();
    if (FAILED(hr) || !g_swap) {  tex->Release(); if (dxgiDev) dxgiDev->Release(); if (adapter) adapter->Release(); HdrSnapDestroy(); return FALSE; }
    IDXGISwapChain3 *swap3 = NULL;
    if (SUCCEEDED(g_swap->QueryInterface(__uuidof(IDXGISwapChain3), (void **)&swap3)) && swap3) {
        swap3->SetColorSpace1(DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709);
        swap3->Release();
    }

    ID3D11Texture2D *back = NULL;
    if (SUCCEEDED(g_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&back)) && back) {
        if (cw == w && ch == h) {
            ctx->CopyResource(back, tex);      /* GPU to GPU, no CPU round trip */
        } else {
            /* rare size mismatch: scale on the CPU through a staging copy */
            D3D11_TEXTURE2D_DESC rd = sdesc;
            rd.Width = cw; rd.Height = ch;
            ID3D11Texture2D *full = NULL;
            if (SUCCEEDED(dev->CreateTexture2D(&rd, NULL, &full))) {
                ctx->CopyResource(full, tex);
                D3D11_MAPPED_SUBRESOURCE m;
                if (SUCCEEDED(ctx->Map(full, 0, D3D11_MAP_READ, 0, &m))) {
                    D3D11_TEXTURE2D_DESC wd = rd;
                    wd.Width = w; wd.Height = h;
                    wd.Usage = D3D11_USAGE_STAGING; wd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                    ID3D11Texture2D *dst = NULL;
                    if (SUCCEEDED(dev->CreateTexture2D(&wd, NULL, &dst))) {
                        D3D11_MAPPED_SUBRESOURCE d;
                        if (SUCCEEDED(ctx->Map(dst, 0, D3D11_MAP_WRITE, 0, &d))) {
                            for (int yy = 0; yy < h; yy++) {
                                unsigned short *drow = (unsigned short *)((BYTE *)d.pData + (SIZE_T)yy * d.RowPitch);
                                int sy = (int)((long long)yy * ch / h);
                                unsigned short *srow = (unsigned short *)((BYTE *)m.pData + (SIZE_T)sy * m.RowPitch);
                                for (int xx = 0; xx < w; xx++) {
                                    int sx = (int)((long long)xx * cw / w);
                                    unsigned short *s = srow + (SIZE_T)sx * 4;
                                    drow[xx * 4 + 0] = s[0]; drow[xx * 4 + 1] = s[1]; drow[xx * 4 + 2] = s[2]; drow[xx * 4 + 3] = 0x3C00;
                                }
                            }
                            ctx->Unmap(dst, 0);
                            ctx->CopyResource(back, dst);
                        }
                        dst->Release();
                    }
                    ctx->Unmap(full, 0);
                }
                full->Release();
            }
        }
        back->Release();
    }
    g_swap->Present(0, 0);

    if (FAILED(DCompositionCreateDevice(dxgiDev, __uuidof(IDCompositionDevice), (void **)&g_dcomp)) || !g_dcomp) {
         tex->Release(); if (dxgiDev) dxgiDev->Release(); if (adapter) adapter->Release(); HdrSnapDestroy(); return FALSE;
    }
    if (FAILED(g_dcomp->CreateTargetForHwnd(g_wnd, TRUE, &g_target)) || !g_target) {
         tex->Release(); if (dxgiDev) dxgiDev->Release(); if (adapter) adapter->Release(); HdrSnapDestroy(); return FALSE;
    }
    if (FAILED(g_dcomp->CreateVisual(&g_visual)) || !g_visual) {
         tex->Release(); if (dxgiDev) dxgiDev->Release(); if (adapter) adapter->Release(); HdrSnapDestroy(); return FALSE;
    }
    g_visual->SetContent(g_swap);
    g_target->SetRoot(g_visual);
    g_dcomp->Commit();
    ShowWindow(g_wnd, SW_SHOWNOACTIVATE);

    HRGN full = CreateRectRgn(0, 0, w, h);
    HRGN hole = CreateRectRgn(bx, by, bx + 4, by + 4);
    if (full && hole) {
        CombineRgn(full, full, hole, RGN_DIFF);
        SetWindowRgn(g_wnd, full, TRUE);
    } else if (full) {
        SetWindowRgn(g_wnd, full, TRUE);
    }
    if (hole) DeleteObject(hole);

    g_probeX = bx + 1; g_probeY = by + 1;
    g_probeRef = ref < 18 ? 18 : ref;
    g_rect.left = x; g_rect.top = y; g_rect.right = x + w; g_rect.bottom = y + h;
    g_ok = TRUE;
    tex->Release();
    if (dxgiDev) dxgiDev->Release();
    if (adapter) adapter->Release();
    return TRUE;
}
