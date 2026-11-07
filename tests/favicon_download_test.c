#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <urlmon.h>
#include <stdio.h>
#include <wchar.h>

/* Keep the production worker, while replacing network, file and image I/O. */
static int downloadCalls, deleteCalls, moveCalls, imageCalls, badPaths;
static BOOL downloadOk, validImage;
static DWORD downloadedSize;
static UINT widths[4];
static WCHAR movedFrom[MAX_PATH + 8];
static const WCHAR *cacheFile = L"minidock-favicon-regression.cache";
static BOOL KnownPath(LPCWSTR path) {
    WCHAR expected[MAX_PATH + 8];
    for (int i = 0; i < 4; i++) {
        wsprintfW(expected, L"%s.%d", cacheFile, i);
        if (!wcscmp(path, expected)) return TRUE;
    }
    wsprintfW(expected, L"%s.tmp", cacheFile);
    return !wcscmp(path, expected);
}
static HRESULT WINAPI TestDownload(LPUNKNOWN caller, LPCWSTR url, LPCWSTR file, DWORD reserved, LPBINDSTATUSCALLBACK callback) {
    (void)caller; (void)reserved; (void)callback;
    downloadCalls++;
    if (!KnownPath(file) || !url[0]) badPaths++;
    return downloadOk ? S_OK : E_FAIL;
}
static BOOL WINAPI TestDeleteFile(LPCWSTR file) {
    deleteCalls++;
    if (!KnownPath(file)) badPaths++;
    return TRUE;
}
static BOOL WINAPI TestMoveFile(LPCWSTR from, LPCWSTR to, DWORD flags) {
    (void)flags;
    moveCalls++;
    if (!KnownPath(from) || wcscmp(to, cacheFile)) badPaths++;
    lstrcpynW(movedFrom, from, MAX_PATH + 8);
    return TRUE;
}
static BOOL WINAPI TestFileAttributes(LPCWSTR file, GET_FILEEX_INFO_LEVELS level, LPVOID info) {
    (void)level;
    if (!KnownPath(file)) badPaths++;
    ZeroMemory(info, sizeof(WIN32_FILE_ATTRIBUTE_DATA));
    ((WIN32_FILE_ATTRIBUTE_DATA *)info)->nFileSizeLow = downloadedSize;
    return TRUE;
}
#define URLDownloadToFileW TestDownload
#define DeleteFileW TestDeleteFile
#define MoveFileExW TestMoveFile
#define GetFileAttributesExW TestFileAttributes
#define GdipCreateBitmapFromFile TestCreateBitmap
#define GdipGetImageWidth TestImageWidth
#define GdipDisposeImage TestDisposeImage
#include "../MiniDock-source/minidock.c"

GpStatus GPAPI TestCreateBitmap(const WCHAR *file, GpBitmap **bitmap) {
    if (!KnownPath(file)) badPaths++;
    imageCalls++;
    *bitmap = validImage ? (GpBitmap *)(UINT_PTR)imageCalls : NULL;
    return validImage ? 0 : 1;
}
GpStatus GPAPI TestImageWidth(GpImage *image, UINT *width) {
    UINT_PTR index = (UINT_PTR)image - 1;
    if (index >= 4) { badPaths++; *width = 0; }
    else *width = widths[index];
    return 0;
}
GpStatus GPAPI TestDisposeImage(GpImage *image) { (void)image; return 0; }

static int checks;
#define CHECK(condition, label) do { checks++; if (!(condition)) { fprintf(stderr, "FAIL %d: %s\n", checks, label); return 1; } } while (0)
static void RunWorker(int mode, BOOL downloaded, BOOL image, DWORD size, UINT a, UINT b, UINT c, UINT d) {
    downloadCalls = deleteCalls = moveCalls = imageCalls = badPaths = 0;
    movedFrom[0] = 0;
    downloadOk = downloaded; validImage = image; downloadedSize = size;
    widths[0] = a; widths[1] = b; widths[2] = c; widths[3] = d;
    DlJob *job = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof *job);
    if (!job) ExitProcess(2);
    job->mode = mode;
    lstrcpyW(job->host, L"fixture.invalid");
    lstrcpyW(job->url, L"https://fixture.invalid/icon.png");
    lstrcpyW(job->file, cacheFile);
    DlThread(job);
}
int main(void) {
    RunWorker(0, FALSE, FALSE, 0, 0, 0, 0, 0);
    CHECK(downloadCalls == 4 && !moveCalls && !imageCalls, "all downloads fail without exceeding four candidates");
    CHECK(deleteCalls == 12 && !badPaths, "failed downloads clean up only valid initialized paths");
    RunWorker(0, TRUE, FALSE, 128, 0, 0, 0, 0);
    CHECK(downloadCalls == 4 && imageCalls == 4 && !moveCalls, "invalid images exhaust the four candidates safely");
    CHECK(deleteCalls == 8 && !badPaths, "invalid images clean up all candidate paths");
    RunWorker(0, TRUE, TRUE, 128, 16, 32, 64, 48);
    CHECK(downloadCalls == 4 && imageCalls == 4 && moveCalls == 1, "small images exhaust the candidates and retain a result");
    CHECK(!wcscmp(movedFrom, L"minidock-favicon-regression.cache.2") && !badPaths, "the widest small image is selected");
    RunWorker(0, TRUE, TRUE, 128, 128, 0, 0, 0);
    CHECK(downloadCalls == 1 && imageCalls == 1 && moveCalls == 1, "large first image stops further requests");
    CHECK(deleteCalls == 2 && !badPaths, "early success never cleans up uninitialized candidate paths");
    RunWorker(0, TRUE, TRUE, 128, 16, 32, 256, 0);
    CHECK(downloadCalls == 3 && imageCalls == 3 && moveCalls == 1, "large third image stops before the fourth candidate");
    CHECK(deleteCalls == 6 && !badPaths && !wcscmp(movedFrom, L"minidock-favicon-regression.cache.2"), "partial cleanup and selected result remain correct");
    RunWorker(0, TRUE, TRUE, 20, 128, 128, 128, 128);
    CHECK(downloadCalls == 4 && !imageCalls && !moveCalls, "too-small files are rejected without image decoding");
    CHECK(deleteCalls == 12 && !badPaths, "too-small files clean up all valid paths");
    RunWorker(1, TRUE, TRUE, 128, 128, 0, 0, 0);
    CHECK(downloadCalls == 1 && moveCalls == 1 && !imageCalls, "custom icon URL keeps its single-download behavior");
    CHECK(!wcscmp(movedFrom, L"minidock-favicon-regression.cache.tmp") && !badPaths, "custom icon uses the expected temporary path");
    RunWorker(1, FALSE, FALSE, 0, 0, 0, 0, 0);
    CHECK(downloadCalls == 1 && !moveCalls && !imageCalls, "custom icon failure does not move a result");
    CHECK(deleteCalls == 2 && !badPaths, "custom icon failure cleans up its temporary path");
    printf("PASS %d favicon checks\n", checks);
    return 0;
}
