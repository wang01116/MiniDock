/* Windows.Graphics.Capture window grabber for MiniDock (C mode, MinGW).
   Returns the window's SDR content without the HDR->SDR screen-grab whitening. */
#define INITGUID
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <dxgi1_6.h>
#include <roapi.h>
#include <hstring.h>
#include <winstring.h>
#include <windows.foundation.h>
#include <windows.graphics.h>
#include <windows.graphics.capture.h>
#include <windows.graphics.capture.interop.h>
#include <windows.graphics.directx.h>
#include <windows.graphics.directx.direct3d11.h>
#include <math.h>

typedef __x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureItem Item;
typedef __x_ABI_CWindows_CGraphics_CCapture_CIDirect3D11CaptureFramePoolStatics PoolStatics;
typedef __x_ABI_CWindows_CGraphics_CCapture_CIDirect3D11CaptureFramePool Pool;
typedef __x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureSession Session;
typedef __x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureSession3 Session3;
typedef __x_ABI_CWindows_CGraphics_CCapture_CIDirect3D11CaptureFrame Frame;
typedef __x_ABI_CWindows_CGraphics_CDirectX_CDirect3D11_CIDirect3DDevice RTDevice;
typedef __x_ABI_CWindows_CGraphics_CDirectX_CDirect3D11_CIDirect3DSurface RTSurface;
typedef __x_ABI_CWindows_CGraphics_CSizeInt32 SizeI32;

typedef struct IWgcDxgiAccess IWgcDxgiAccess;
typedef struct IWgcDxgiAccessVtbl {
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWgcDxgiAccess *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWgcDxgiAccess *);
    ULONG (STDMETHODCALLTYPE *Release)(IWgcDxgiAccess *);
    HRESULT (STDMETHODCALLTYPE *GetInterface)(IWgcDxgiAccess *, REFIID, void **);
} IWgcDxgiAccessVtbl;
struct IWgcDxgiAccess { const IWgcDxgiAccessVtbl *lpVtbl; };
static const GUID IID_WgcDxgiAccess = {0xA9B3D012,0x3DF2,0x4EE3,{0xB8,0xD1,0x86,0x95,0xF4,0x57,0xD3,0xC1}};

typedef HRESULT (WINAPI *PFN_CreateD3D11DeviceFromDXGIDevice)(IDXGIDevice *, IInspectable **);

static double WgcHalf(unsigned short v) {
    unsigned s = (v >> 15) & 1, e = (v >> 10) & 0x1f, m = v & 0x3ff;
    double d;
    if (e == 0) d = m ? ldexp((double)m, -24) : 0.0;
    else if (e == 31) d = 0.0;
    else d = ldexp((double)(m + 1024), (int)e - 25);
    return s ? -d : d;
}
static double WgcLin2Srgb(double l) {
    if (l <= 0.0031308) return 12.92 * l;
    if (l > 1) l = 1;
    return 1.055 * pow(l, 1.0 / 2.4) - 0.055;
}
static int WgcByte(double l) {
    double v = WgcLin2Srgb(l) * 255.0;
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    return (int)(v + 0.5);
}
/* scRGB value that corresponds to SDR white: DISPLAYCONFIG_SDR_WHITE_LEVEL / 1000. */
static double WgcSdrNorm(void) {
    double norm = 4.25;
    UINT32 np = 0, nm = 0;
    if (GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &np, &nm) != ERROR_SUCCESS) return norm;
    DISPLAYCONFIG_PATH_INFO *paths = (DISPLAYCONFIG_PATH_INFO *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)np * sizeof(DISPLAYCONFIG_PATH_INFO));
    DISPLAYCONFIG_MODE_INFO *modes = (DISPLAYCONFIG_MODE_INFO *)HeapAlloc(GetProcessHeap(), 0, (SIZE_T)nm * sizeof(DISPLAYCONFIG_MODE_INFO));
    if (paths && modes && QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &np, paths, &nm, modes, NULL) == ERROR_SUCCESS) {
        for (UINT32 i = 0; i < np; i++) {
            DISPLAYCONFIG_SDR_WHITE_LEVEL w;
            ZeroMemory(&w, sizeof w);
            w.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL;
            w.header.size = sizeof w;
            w.header.adapterId = paths[i].targetInfo.adapterId;
            w.header.id = paths[i].targetInfo.id;
            if (DisplayConfigGetDeviceInfo(&w.header) == ERROR_SUCCESS && w.SDRWhiteLevel > 0) {
                norm = (double)w.SDRWhiteLevel / 1000.0;
                break;
            }
        }
    }
    if (paths) HeapFree(GetProcessHeap(), 0, paths);
    if (modes) HeapFree(GetProcessHeap(), 0, modes);
    if (norm < 0.5) norm = 4.25;
    return norm;
}

typedef struct {
    BOOL tried, ok, roInitialized;
    ID3D11Device *dev;
    ID3D11DeviceContext *ctx;
    RTDevice *rtDev;
    IGraphicsCaptureItemInterop *interop;
} WgcState;
static WgcState g_wgc;

static BOOL WgcInit(void) {
    if (g_wgc.tried) return g_wgc.ok;
    g_wgc.tried = TRUE;
    HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);
    if (FAILED(hr)) hr = RoInitialize(RO_INIT_SINGLETHREADED);
    g_wgc.roInitialized = SUCCEEDED(hr);
    if (FAILED(hr)) return FALSE;
    D3D_FEATURE_LEVEL fl;
    hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                           NULL, 0, D3D11_SDK_VERSION, &g_wgc.dev, &fl, &g_wgc.ctx);
    if (FAILED(hr) || !g_wgc.dev) return FALSE;
    IDXGIDevice *dxgiDev = NULL;
    g_wgc.dev->lpVtbl->QueryInterface(g_wgc.dev, &IID_IDXGIDevice, (void **)&dxgiDev);
    HMODULE h = GetModuleHandleW(L"d3d11.dll");
    PFN_CreateD3D11DeviceFromDXGIDevice createDevice = h ? (PFN_CreateD3D11DeviceFromDXGIDevice)GetProcAddress(h, "CreateDirect3D11DeviceFromDXGIDevice") : NULL;
    IInspectable *insp = NULL;
    hr = createDevice && dxgiDev ? createDevice(dxgiDev, &insp) : E_NOINTERFACE;
    if (dxgiDev) dxgiDev->lpVtbl->Release(dxgiDev);
    if (FAILED(hr) || !insp) return FALSE;
    insp->lpVtbl->QueryInterface(insp, &IID___x_ABI_CWindows_CGraphics_CDirectX_CDirect3D11_CIDirect3DDevice, (void **)&g_wgc.rtDev);
    insp->lpVtbl->Release(insp);
    if (!g_wgc.rtDev) return FALSE;
    HSTRING hs = NULL;
    const WCHAR *cls = L"Windows.Graphics.Capture.GraphicsCaptureItem";
    WindowsCreateString(cls, (UINT32)wcslen(cls), &hs);
    hr = RoGetActivationFactory(hs, &IID_IGraphicsCaptureItemInterop, (void **)&g_wgc.interop);
    WindowsDeleteString(hs);
    g_wgc.ok = SUCCEEDED(hr) && g_wgc.interop != NULL;
    return g_wgc.ok;
}

/* Grab one frame of hwnd into bits: 32bpp BGRA (top-down), alpha 255. */
BOOL WgcGrabWindow(HWND hwnd, void *bits, int w, int h) {
    if (!hwnd || !bits || w <= 0 || h <= 0) return FALSE;
    if (!WgcInit()) return FALSE;
    Item *item = NULL;
    HRESULT hr = g_wgc.interop->lpVtbl->CreateForWindow(g_wgc.interop, hwnd,
                 &IID___x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureItem, (void **)&item);
    if (FAILED(hr) || !item) {  return FALSE; }

    SizeI32 size;
    item->lpVtbl->get_Size(item, &size);
    HSTRING hs = NULL;
    const WCHAR *cls = L"Windows.Graphics.Capture.Direct3D11CaptureFramePool";
    WindowsCreateString(cls, (UINT32)wcslen(cls), &hs);
    PoolStatics *statics = NULL;
    hr = RoGetActivationFactory(hs, &IID___x_ABI_CWindows_CGraphics_CCapture_CIDirect3D11CaptureFramePoolStatics, (void **)&statics);
    WindowsDeleteString(hs);
    if (FAILED(hr) || !statics) { item->lpVtbl->Release(item); return FALSE; }
    Pool *pool = NULL;
    int wantFp16 = 1;
    hr = statics->lpVtbl->Create(statics, g_wgc.rtDev,
             (enum __x_ABI_CWindows_CGraphics_CDirectX_CDirectXPixelFormat)10 /* R16G16B16A16Float */,
             2, size, &pool);
    if (FAILED(hr) || !pool) {
        wantFp16 = 0;
        hr = statics->lpVtbl->Create(statics, g_wgc.rtDev,
                 (enum __x_ABI_CWindows_CGraphics_CDirectX_CDirectXPixelFormat)87 /* B8G8R8A8UIntNormalized */,
                 2, size, &pool);
    }
    statics->lpVtbl->Release(statics);
    if (FAILED(hr) || !pool) { item->lpVtbl->Release(item); return FALSE; }
    Session *session = NULL;
    hr = pool->lpVtbl->CreateCaptureSession(pool, item, &session);
    if (FAILED(hr) || !session) { pool->lpVtbl->Release(pool); item->lpVtbl->Release(item); return FALSE; }
    Session3 *s3 = NULL;
    if (SUCCEEDED(session->lpVtbl->QueryInterface(session, &IID___x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureSession3, (void **)&s3)) && s3) {
        s3->lpVtbl->put_IsBorderRequired(s3, 0);
        s3->lpVtbl->Release(s3);
    }
    hr = session->lpVtbl->StartCapture(session);
    BOOL ok = FALSE;
    if (SUCCEEDED(hr)) {
        /* the preview window can be perfectly static: poke a repaint so the session yields a frame */
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        Frame *frame = NULL;
        for (int i = 0; i < 150; i++) {
            hr = pool->lpVtbl->TryGetNextFrame(pool, &frame);
            if (SUCCEEDED(hr) && frame) break;
            Sleep(3);
        }
        if (frame) {
            RTSurface *surface = NULL;
            hr = frame->lpVtbl->get_Surface(frame, &surface);
            if (SUCCEEDED(hr) && surface) {
                IWgcDxgiAccess *access = NULL;
                hr = surface->lpVtbl->QueryInterface(surface, &IID_WgcDxgiAccess, (void **)&access);
                if (SUCCEEDED(hr) && access) {
                    ID3D11Texture2D *tex = NULL;
                    hr = access->lpVtbl->GetInterface(access, &IID_ID3D11Texture2D, (void **)&tex);
                    if (SUCCEEDED(hr) && tex) {
                        D3D11_TEXTURE2D_DESC desc;
                        tex->lpVtbl->GetDesc(tex, &desc);
                        D3D11_TEXTURE2D_DESC sd = desc;
                        sd.Usage = D3D11_USAGE_STAGING; sd.BindFlags = 0; sd.CPUAccessFlags = D3D11_CPU_ACCESS_READ; sd.MiscFlags = 0;
                        ID3D11Texture2D *staging = NULL;
                        if (SUCCEEDED(g_wgc.dev->lpVtbl->CreateTexture2D(g_wgc.dev, &sd, NULL, &staging))) {
                            g_wgc.ctx->lpVtbl->CopyResource(g_wgc.ctx, (ID3D11Resource *)staging, (ID3D11Resource *)tex);
                            D3D11_MAPPED_SUBRESOURCE m;
                            if (SUCCEEDED(g_wgc.ctx->lpVtbl->Map(g_wgc.ctx, (ID3D11Resource *)staging, 0, D3D11_MAP_READ, 0, &m))) {
                                int sw = (int)desc.Width, sh = (int)desc.Height;
                                int fp16 = (desc.Format == DXGI_FORMAT_R16G16B16A16_FLOAT);
                                double norm = fp16 ? WgcSdrNorm() : 1.0;
                                unsigned char *dst = (unsigned char *)bits;
                                for (int y = 0; y < h; y++) {
                                    int sy = sh == h ? y : (int)((long long)y * sh / h);
                                    if (sy >= sh) sy = sh - 1;
                                    unsigned char *row = (unsigned char *)m.pData + (SIZE_T)sy * m.RowPitch;
                                    for (int x = 0; x < w; x++) {
                                        int sx = sw == w ? x : (int)((long long)x * sw / w);
                                        if (sx >= sw) sx = sw - 1;
                                        unsigned char *o = dst + ((SIZE_T)y * w + x) * 4;
                                        if (fp16) {
                                            unsigned short *q = (unsigned short *)(row + (SIZE_T)sx * 8);
                                            double r = WgcHalf(q[0]) / norm, g = WgcHalf(q[1]) / norm, b = WgcHalf(q[2]) / norm;
                                            o[2] = (unsigned char)WgcByte(r);
                                            o[1] = (unsigned char)WgcByte(g);
                                            o[0] = (unsigned char)WgcByte(b);
                                        } else {
                                            unsigned char *p = row + (SIZE_T)sx * 4;
                                            o[0] = p[0]; o[1] = p[1]; o[2] = p[2];
                                        }
                                        o[3] = 255;
                                    }
                                }
                                g_wgc.ctx->lpVtbl->Unmap(g_wgc.ctx, (ID3D11Resource *)staging, 0);
                                ok = TRUE;
                            }
                            staging->lpVtbl->Release(staging);
                        }
                        tex->lpVtbl->Release(tex);
                    }
                    access->lpVtbl->Release(access);
                }
                surface->lpVtbl->Release(surface);
            }
            frame->lpVtbl->Release(frame);
        }
    }
    session->lpVtbl->Release(session);
    pool->lpVtbl->Release(pool);
    item->lpVtbl->Release(item);
    return ok;
}

/* Capture one FP16 (scRGB linear) frame of hwnd into a malloc'd buffer (pitch = w*8).
   Caller frees with free(). Used by the DirectComposition HDR snapshot. */
BOOL WgcCaptureHdrCpu(HWND hwnd, void **outPixels, int *outW, int *outH) {
    if (!hwnd || !outPixels || !outW || !outH) return FALSE;
    *outPixels = NULL; *outW = 0; *outH = 0;
    if (!WgcInit()) return FALSE;
    Item *item = NULL;
    HRESULT hr = g_wgc.interop->lpVtbl->CreateForWindow(g_wgc.interop, hwnd,
                 &IID___x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureItem, (void **)&item);
    if (FAILED(hr) || !item) return FALSE;
    SizeI32 size;
    item->lpVtbl->get_Size(item, &size);
    HSTRING hs = NULL;
    const WCHAR *cls = L"Windows.Graphics.Capture.Direct3D11CaptureFramePool";
    WindowsCreateString(cls, (UINT32)wcslen(cls), &hs);
    PoolStatics *statics = NULL;
    hr = RoGetActivationFactory(hs, &IID___x_ABI_CWindows_CGraphics_CCapture_CIDirect3D11CaptureFramePoolStatics, (void **)&statics);
    WindowsDeleteString(hs);
    if (FAILED(hr) || !statics) { item->lpVtbl->Release(item); return FALSE; }
    Pool *pool = NULL;
    hr = statics->lpVtbl->Create(statics, g_wgc.rtDev,
             (enum __x_ABI_CWindows_CGraphics_CDirectX_CDirectXPixelFormat)10 /* R16G16B16A16Float */,
             2, size, &pool);
    statics->lpVtbl->Release(statics);
    if (FAILED(hr) || !pool) { item->lpVtbl->Release(item); return FALSE; }
    Session *session = NULL;
    hr = pool->lpVtbl->CreateCaptureSession(pool, item, &session);
    if (FAILED(hr) || !session) { pool->lpVtbl->Release(pool); item->lpVtbl->Release(item); return FALSE; }
    Session3 *s3 = NULL;
    if (SUCCEEDED(session->lpVtbl->QueryInterface(session, &IID___x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureSession3, (void **)&s3)) && s3) {
        s3->lpVtbl->put_IsBorderRequired(s3, 0);
        s3->lpVtbl->Release(s3);
    }
    hr = session->lpVtbl->StartCapture(session);
    BOOL ok = FALSE;
    if (SUCCEEDED(hr)) {
        /* the preview window can be perfectly static: poke a repaint so the session yields a frame */
        RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        Frame *frame = NULL;
        for (int i = 0; i < 150; i++) {
            hr = pool->lpVtbl->TryGetNextFrame(pool, &frame);
            if (SUCCEEDED(hr) && frame) break;
            Sleep(3);
        }
        if (frame) {
            RTSurface *surface = NULL;
            hr = frame->lpVtbl->get_Surface(frame, &surface);
            if (SUCCEEDED(hr) && surface) {
                IWgcDxgiAccess *access = NULL;
                hr = surface->lpVtbl->QueryInterface(surface, &IID_WgcDxgiAccess, (void **)&access);
                if (SUCCEEDED(hr) && access) {
                    ID3D11Texture2D *tex = NULL;
                    hr = access->lpVtbl->GetInterface(access, &IID_ID3D11Texture2D, (void **)&tex);
                    if (SUCCEEDED(hr) && tex) {
                        D3D11_TEXTURE2D_DESC desc;
                        tex->lpVtbl->GetDesc(tex, &desc);
                        D3D11_TEXTURE2D_DESC sd = desc;
                        sd.Usage = D3D11_USAGE_STAGING; sd.BindFlags = 0; sd.CPUAccessFlags = D3D11_CPU_ACCESS_READ; sd.MiscFlags = 0;
                        ID3D11Texture2D *staging = NULL;
                        if (SUCCEEDED(g_wgc.dev->lpVtbl->CreateTexture2D(g_wgc.dev, &sd, NULL, &staging))) {
                            g_wgc.ctx->lpVtbl->CopyResource(g_wgc.ctx, (ID3D11Resource *)staging, (ID3D11Resource *)tex);
                            D3D11_MAPPED_SUBRESOURCE m;
                            if (SUCCEEDED(g_wgc.ctx->lpVtbl->Map(g_wgc.ctx, (ID3D11Resource *)staging, 0, D3D11_MAP_READ, 0, &m))) {
                                int W = (int)desc.Width, H = (int)desc.Height;
                                unsigned short *pix = (unsigned short *)malloc((size_t)W * H * 8);
                                if (pix) {
                                    for (int y = 0; y < H; y++) {
                                        memcpy(pix + (size_t)y * W * 4,
                                               (unsigned char *)m.pData + (SIZE_T)y * m.RowPitch,
                                               (size_t)W * 8);
                                    }
                                    *outPixels = pix; *outW = W; *outH = H;
                                    ok = TRUE;
                                }
                                g_wgc.ctx->lpVtbl->Unmap(g_wgc.ctx, (ID3D11Resource *)staging, 0);
                            }
                            staging->lpVtbl->Release(staging);
                        }
                        tex->lpVtbl->Release(tex);
                    }
                    access->lpVtbl->Release(access);
                }
                surface->lpVtbl->Release(surface);
            }
            frame->lpVtbl->Release(frame);
        }
    }
    session->lpVtbl->Release(session);
    pool->lpVtbl->Release(pool);
    item->lpVtbl->Release(item);
    return ok;
}

ID3D11Device *WgcDevice(void) { if (!WgcInit()) return NULL; return g_wgc.dev; }
ID3D11DeviceContext *WgcContext(void) { if (!WgcInit()) return NULL; return g_wgc.ctx; }

/* One preview capture. The worker opens the session; the UI thread reads and copies frames. */
typedef struct {
    Item *item;
    Pool *pool;
    Session *session;
    Frame *frame;
    HWND window;
    int width, height;
} HdrCapture;
typedef struct {
    LONG references;
    LONG ready;
    HWND window;
    int width, height;
    IGraphicsCaptureItemInterop *interop;
    RTDevice *device;
    HdrCapture capture;
} HdrCaptureWork;
static HdrCaptureWork *g_hdrWork;
static void CloseHdrCapture(HdrCapture *capture) {
    if (capture->frame) capture->frame->lpVtbl->Release(capture->frame);
    if (capture->session) capture->session->lpVtbl->Release(capture->session);
    if (capture->pool) capture->pool->lpVtbl->Release(capture->pool);
    if (capture->item) capture->item->lpVtbl->Release(capture->item);
    ZeroMemory(capture, sizeof *capture);
}
static BOOL OpenHdrCapture(HdrCapture *capture, IGraphicsCaptureItemInterop *interop,
                           RTDevice *device, HWND hwnd, int w, int h) {
    Item *item = NULL;
    HRESULT hr = interop->lpVtbl->CreateForWindow(interop, hwnd,
                 &IID___x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureItem, (void **)&item);
    if (FAILED(hr) || !item) return FALSE;
    BOOL preSized = w > 0 && h > 0;
    SizeI32 size = {w, h};
    if (!preSized && (FAILED(item->lpVtbl->get_Size(item, &size)) || size.Width <= 0 || size.Height <= 0)) {
        item->lpVtbl->Release(item);
        return FALSE;
    }
    HSTRING hs = NULL;
    const WCHAR *cls = L"Windows.Graphics.Capture.Direct3D11CaptureFramePool";
    WindowsCreateString(cls, (UINT32)wcslen(cls), &hs);
    PoolStatics *statics = NULL;
    hr = RoGetActivationFactory(hs, &IID___x_ABI_CWindows_CGraphics_CCapture_CIDirect3D11CaptureFramePoolStatics, (void **)&statics);
    WindowsDeleteString(hs);
    if (FAILED(hr) || !statics) { item->lpVtbl->Release(item); return FALSE; }
    Pool *pool = NULL;
    if (preSized) {
        __x_ABI_CWindows_CGraphics_CCapture_CIDirect3D11CaptureFramePoolStatics2 *freeThreaded = NULL;
        hr = statics->lpVtbl->QueryInterface(statics,
            &IID___x_ABI_CWindows_CGraphics_CCapture_CIDirect3D11CaptureFramePoolStatics2, (void **)&freeThreaded);
        if (SUCCEEDED(hr) && freeThreaded) {
            hr = freeThreaded->lpVtbl->CreateFreeThreaded(freeThreaded, device,
                (enum __x_ABI_CWindows_CGraphics_CDirectX_CDirectXPixelFormat)10, 2, size, &pool);
            freeThreaded->lpVtbl->Release(freeThreaded);
        }
    } else {
        hr = statics->lpVtbl->Create(statics, device,
            (enum __x_ABI_CWindows_CGraphics_CDirectX_CDirectXPixelFormat)10, 2, size, &pool);
    }
    statics->lpVtbl->Release(statics);
    if (FAILED(hr) || !pool) { item->lpVtbl->Release(item); return FALSE; }
    Session *session = NULL;
    hr = pool->lpVtbl->CreateCaptureSession(pool, item, &session);
    if (FAILED(hr) || !session) { pool->lpVtbl->Release(pool); item->lpVtbl->Release(item); return FALSE; }
    Session3 *s3 = NULL;
    if (SUCCEEDED(session->lpVtbl->QueryInterface(session, &IID___x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureSession3, (void **)&s3)) && s3) {
        s3->lpVtbl->put_IsBorderRequired(s3, 0);
        s3->lpVtbl->Release(s3);
    }
    hr = session->lpVtbl->StartCapture(session);
    if (FAILED(hr)) {
        session->lpVtbl->Release(session); pool->lpVtbl->Release(pool); item->lpVtbl->Release(item);
        return FALSE;
    }
    *capture = (HdrCapture){item, pool, session, NULL, hwnd, size.Width, size.Height};
    return TRUE;
}
static BOOL CopyHdrFrame(Frame *frame, void **outTex, int *outW, int *outH) {
    BOOL ok = FALSE;
    HRESULT hr;
    RTSurface *surface = NULL;
    hr = frame->lpVtbl->get_Surface(frame, &surface);
    if (SUCCEEDED(hr) && surface) {
        IWgcDxgiAccess *access = NULL;
        hr = surface->lpVtbl->QueryInterface(surface, &IID_WgcDxgiAccess, (void **)&access);
        if (SUCCEEDED(hr) && access) {
            ID3D11Texture2D *tex = NULL;
            hr = access->lpVtbl->GetInterface(access, &IID_ID3D11Texture2D, (void **)&tex);
            if (SUCCEEDED(hr) && tex) {
                D3D11_TEXTURE2D_DESC desc;
                tex->lpVtbl->GetDesc(tex, &desc);
                D3D11_TEXTURE2D_DESC cd = desc;
                cd.Usage = D3D11_USAGE_DEFAULT; cd.BindFlags = 0; cd.CPUAccessFlags = 0; cd.MiscFlags = 0;
                ID3D11Texture2D *copy = NULL;
                if (SUCCEEDED(g_wgc.dev->lpVtbl->CreateTexture2D(g_wgc.dev, &cd, NULL, &copy))) {
                    g_wgc.ctx->lpVtbl->CopyResource(g_wgc.ctx, (ID3D11Resource *)copy, (ID3D11Resource *)tex);
                    *outTex = (void *)copy; *outW = (int)desc.Width; *outH = (int)desc.Height;
                    ok = TRUE;
                }
                tex->lpVtbl->Release(tex);
            }
            access->lpVtbl->Release(access);
        }
        surface->lpVtbl->Release(surface);
    }
    return ok;
}
/* The UI and queued worker each hold a reference. Closing a preview never waits
   for a slow session to open; whoever releases the last reference cleans it up. */
static void ReleaseHdrWork(HdrCaptureWork *work) {
    if (work && InterlockedDecrement(&work->references) == 0) {
        CloseHdrCapture(&work->capture);
        work->device->lpVtbl->Release(work->device);
        work->interop->lpVtbl->Release(work->interop);
        HeapFree(GetProcessHeap(), 0, work);
    }
}
static void CancelHdrWork(void) {
    HdrCaptureWork *work = g_hdrWork;
    g_hdrWork = NULL;
    ReleaseHdrWork(work);
}
static DWORD WINAPI PrimeHdrOnWorker(void *arg) {
    HdrCaptureWork *work = arg;
    HRESULT hr = RoInitialize(RO_INIT_MULTITHREADED);
    DWORD pid = 0;
    GetWindowThreadProcessId(work->window, &pid);
    if (SUCCEEDED(hr) && pid == GetCurrentProcessId() &&
        InterlockedCompareExchange(&work->references, 0, 0) == 2) {
        OpenHdrCapture(&work->capture, work->interop, work->device, work->window, work->width, work->height);
    }
    InterlockedExchange(&work->ready, 1);
    ReleaseHdrWork(work);
    if (SUCCEEDED(hr)) RoUninitialize();
    return 0;
}
static BOOL IsAgile(IUnknown *object) {
    IUnknown *agile = NULL;
    if (FAILED(object->lpVtbl->QueryInterface(object, &IID_IAgileObject, (void **)&agile)) || !agile) return FALSE;
    agile->lpVtbl->Release(agile);
    return TRUE;
}
BOOL WgcPrimeHdrCapture(HWND hwnd, int w, int h) {
    CancelHdrWork();
    /* The UI owns device initialization, including HDR enabled after startup. */
    if (w < 32 || h < 32 || !WgcInit() || !IsAgile((IUnknown *)g_wgc.interop) ||
        !IsAgile((IUnknown *)g_wgc.rtDev)) return FALSE;
    HdrCaptureWork *work = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof *work);
    if (!work) return FALSE;
    work->references = 2;
    work->window = hwnd; work->width = w; work->height = h;
    work->interop = g_wgc.interop;
    work->device = g_wgc.rtDev;
    work->interop->lpVtbl->AddRef(work->interop);
    work->device->lpVtbl->AddRef(work->device);
    if (!QueueUserWorkItem(PrimeHdrOnWorker, work, WT_EXECUTEDEFAULT)) {
        work->references = 1;
        ReleaseHdrWork(work);
        return FALSE;
    }
    g_hdrWork = work;
    return TRUE;
}
static HdrCapture *ReadyHdrCapture(void) {
    if (!g_hdrWork || !InterlockedCompareExchange(&g_hdrWork->ready, 0, 0)) return NULL;
    return &g_hdrWork->capture;
}
void WgcDrainHdrCapture(void) {
    HdrCapture *primed = ReadyHdrCapture();
    Pool *pool = primed ? primed->pool : NULL;
    if (!pool) return;
    for (int i = 0; i < 2; ++i) {
        Frame *frame = NULL;
        if (FAILED(pool->lpVtbl->TryGetNextFrame(pool, &frame)) || !frame) break;
        SizeI32 size;
        if (SUCCEEDED(frame->lpVtbl->get_ContentSize(frame, &size)) &&
            size.Width == primed->width && size.Height == primed->height) {
            if (primed->frame) primed->frame->lpVtbl->Release(primed->frame);
            primed->frame = frame;
        } else frame->lpVtbl->Release(frame);
    }
}
BOOL WgcCaptureHdrTexture(HWND hwnd, void **outTex, int *outW, int *outH) {
    if (!hwnd || !outTex || !outW || !outH) return FALSE;
    *outTex = NULL; *outW = 0; *outH = 0;
    HdrCapture *primed = ReadyHdrCapture();
    if (primed && primed->window == hwnd) {
        DWORD began = GetTickCount();
        do {
            WgcDrainHdrCapture();
            if (primed->frame) break;
            Sleep(1);
        } while (GetTickCount() - began < 24);
        BOOL ok = primed->frame && CopyHdrFrame(primed->frame, outTex, outW, outH);
        if (ok) { CancelHdrWork(); return TRUE; }
    }
    CancelHdrWork();
    HdrCapture capture = {0};
    if (!WgcInit() || !OpenHdrCapture(&capture, g_wgc.interop, g_wgc.rtDev, hwnd, 0, 0)) return FALSE;
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    for (int i = 0; i < 150; ++i) {
        if (SUCCEEDED(capture.pool->lpVtbl->TryGetNextFrame(capture.pool, &capture.frame)) && capture.frame) break;
        Sleep(3);
    }
    BOOL ok = capture.frame && CopyHdrFrame(capture.frame, outTex, outW, outH);
    CloseHdrCapture(&capture);
    return ok;
}

/* ---- prepared capture: start the WGC session while the flight animation runs, so the
   frame is already available when the flight ends (saves ~150-200 ms). ---- */
static Item *g_preItem = NULL;
static Pool *g_prePool = NULL;
static Session *g_preSession = NULL;
static BOOL g_preActive = FALSE;

void WgcCancelCapture(void) {
    CancelHdrWork();
    if (g_preSession) { g_preSession->lpVtbl->Release(g_preSession); g_preSession = NULL; }
    if (g_prePool) { g_prePool->lpVtbl->Release(g_prePool); g_prePool = NULL; }
    if (g_preItem) { g_preItem->lpVtbl->Release(g_preItem); g_preItem = NULL; }
    g_preActive = FALSE;
}

static int g_preMonX, g_preMonY;

BOOL WgcPrepareCapture(HWND hwnd) {
    WgcCancelCapture();

    if (!hwnd || !WgcInit()) {  return FALSE; }
    HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi;
    ZeroMemory(&mi, sizeof mi);
    mi.cbSize = sizeof mi;
    if (!mon || !GetMonitorInfoW(mon, &mi)) {  return FALSE; }
    g_preMonX = mi.rcMonitor.left;
    g_preMonY = mi.rcMonitor.top;
    /* capture the whole monitor: the item keeps a fixed size while the preview window
       grows, and the final frame is the settled desktop (the preview window on top). */
    Item *item = NULL;
    HRESULT hr = g_wgc.interop->lpVtbl->CreateForMonitor(g_wgc.interop, mon,
                 &IID___x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureItem, (void **)&item);
    if (FAILED(hr) || !item) return FALSE;
    SizeI32 size;
    item->lpVtbl->get_Size(item, &size);
    HSTRING hs = NULL;
    const WCHAR *cls = L"Windows.Graphics.Capture.Direct3D11CaptureFramePool";
    WindowsCreateString(cls, (UINT32)wcslen(cls), &hs);
    PoolStatics *statics = NULL;
    hr = RoGetActivationFactory(hs, &IID___x_ABI_CWindows_CGraphics_CCapture_CIDirect3D11CaptureFramePoolStatics, (void **)&statics);
    WindowsDeleteString(hs);
    if (FAILED(hr) || !statics) { item->lpVtbl->Release(item); return FALSE; }
    Pool *pool = NULL;
    hr = statics->lpVtbl->Create(statics, g_wgc.rtDev,
             (enum __x_ABI_CWindows_CGraphics_CDirectX_CDirectXPixelFormat)10, 2, size, &pool);
    statics->lpVtbl->Release(statics);
    if (FAILED(hr) || !pool) { item->lpVtbl->Release(item); return FALSE; }
    Session *session = NULL;
    hr = pool->lpVtbl->CreateCaptureSession(pool, item, &session);
    if (FAILED(hr) || !session) { pool->lpVtbl->Release(pool); item->lpVtbl->Release(item); return FALSE; }
    Session3 *s3 = NULL;
    if (SUCCEEDED(session->lpVtbl->QueryInterface(session, &IID___x_ABI_CWindows_CGraphics_CCapture_CIGraphicsCaptureSession3, (void **)&s3)) && s3) {
        s3->lpVtbl->put_IsBorderRequired(s3, 0);
        s3->lpVtbl->Release(s3);
    }
    if (FAILED(session->lpVtbl->StartCapture(session))) {
        session->lpVtbl->Release(session); pool->lpVtbl->Release(pool); item->lpVtbl->Release(item);
        return FALSE;
    }
    g_preItem = item; g_prePool = pool; g_preSession = session; g_preActive = TRUE;

    return TRUE;
}

/* Grab the latest frame of the prepared session as a stable texture and end the session. */
/* Return the newest frame of the prepared monitor session as a texture, plus the monitor origin.
   Keeps consuming frames until they stop arriving so the settled flight frame is used. */
BOOL WgcGetPreparedTexture(void **outTex, int *outW, int *outH, int *monX, int *monY) {
    if (!g_preActive || !g_prePool) {  return FALSE; }

    if (!outTex || !outW || !outH) { WgcCancelCapture(); return FALSE; }
    *outTex = NULL; *outW = 0; *outH = 0;
    if (monX) *monX = g_preMonX;
    if (monY) *monY = g_preMonY;
    ID3D11Texture2D *copy = NULL;
    int cw = 0, ch = 0;
    Frame *last = NULL;
    int idle = 0;
    for (int i = 0; i < 10; i++) {
        Frame *frame = NULL;
        if (SUCCEEDED(g_prePool->lpVtbl->TryGetNextFrame(g_prePool, &frame)) && frame) {
            idle = 0;
            if (last) last->lpVtbl->Release(last);
            last = frame;   /* keep the newest frame (its texture stays valid while referenced) */
        } else {
            idle++;
            if (idle > 3) break;
            Sleep(2);
        }
    }
    if (last) {
        RTSurface *surface = NULL;
        if (SUCCEEDED(last->lpVtbl->get_Surface(last, &surface)) && surface) {
            IWgcDxgiAccess *access = NULL;
            if (SUCCEEDED(surface->lpVtbl->QueryInterface(surface, &IID_WgcDxgiAccess, (void **)&access)) && access) {
                ID3D11Texture2D *tex = NULL;
                if (SUCCEEDED(access->lpVtbl->GetInterface(access, &IID_ID3D11Texture2D, (void **)&tex)) && tex) {
                    D3D11_TEXTURE2D_DESC desc;
                    tex->lpVtbl->GetDesc(tex, &desc);
                    D3D11_TEXTURE2D_DESC cd = desc;
                    cd.Usage = D3D11_USAGE_DEFAULT; cd.BindFlags = 0; cd.CPUAccessFlags = 0; cd.MiscFlags = 0;
                    if (SUCCEEDED(g_wgc.dev->lpVtbl->CreateTexture2D(g_wgc.dev, &cd, NULL, &copy))) {
                        g_wgc.ctx->lpVtbl->CopyResource(g_wgc.ctx, (ID3D11Resource *)copy, (ID3D11Resource *)tex);
                        cw = (int)desc.Width; ch = (int)desc.Height;
                    }
                    tex->lpVtbl->Release(tex);
                }
                access->lpVtbl->Release(access);
            }
            surface->lpVtbl->Release(surface);
        }
        last->lpVtbl->Release(last);
    }
    if (!copy) { WgcCancelCapture();  return FALSE; }

    *outTex = (void *)copy; *outW = cw; *outH = ch;
    return TRUE;
}

/* create the shared D3D11/WGC state up front so the first restore is not slower */
void WgcPrewarm(void) {
    WgcInit();
}

/* Stop captures and drop the UI device when switching back to a classic style.
   A worker still opening a capture owns its references until it finishes. */
void WgcShutdown(void) {
    WgcCancelCapture();
    if (g_wgc.interop) g_wgc.interop->lpVtbl->Release(g_wgc.interop);
    if (g_wgc.rtDev) g_wgc.rtDev->lpVtbl->Release(g_wgc.rtDev);
    if (g_wgc.ctx) g_wgc.ctx->lpVtbl->Release(g_wgc.ctx);
    if (g_wgc.dev) g_wgc.dev->lpVtbl->Release(g_wgc.dev);
    if (g_wgc.roInitialized) RoUninitialize();
    ZeroMemory(&g_wgc, sizeof g_wgc);
}
