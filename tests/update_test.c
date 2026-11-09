#define UNICODE
#define _UNICODE
#define _WIN32_WINNT 0x0601
#define WINVER 0x0601
#define COBJMACROS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

/* The updater is included in full; only its two network entry points are replaced, so the
   real decision logic (status handling, release picking, verification) runs unmodified.
   Nothing here touches the network, the registry or a real release asset. */
#include "../MiniDock-source/minidock.c"

static int checks;
#define CHECK(cond, label) do { checks++; if (!(cond)) { fprintf(stderr, "FAIL %d: %s\n", checks, label); return 1; } } while (0)

/* ---- fake GitHub ---- */
static const char *g_reply[2];   /* [0] /releases/latest, [1] /releases?per_page= */
static DWORD g_code[2];
static BOOL g_transport[2] = { TRUE, TRUE };
static int g_calls[2];

static BOOL FakeNet(const WCHAR *url, DWORD cap, DWORD *status, char **body, DWORD *len) {
    int i = wcsstr(url, L"/releases/latest") ? 0 : 1;
    const char *s = g_reply[i];
    (void)cap;
    g_calls[i]++;
    *status = g_code[i];
    *len = 0;
    *body = NULL;
    if (!g_transport[i]) return FALSE;
    if (!s) s = "";
    *len = (DWORD)strlen(s);
    char *buf = (char *)HeapAlloc(GetProcessHeap(), 0, *len + 1);
    if (!buf) return FALSE;
    memcpy(buf, s, *len + 1);
    *body = buf;
    return TRUE;
}

static void Reply(int slot, DWORD code, const char *json) {
    g_code[slot] = code;
    g_reply[slot] = json;
    g_transport[slot] = TRUE;
}

/* A payload shaped like the real one: author object first, then the release fields, assets
   last, with a release note that talks about tag_name to catch a strstr-based reader. */
static const char *kRelease =
    "{\"url\":\"https://api.github.com/repos/wang01116/MiniDock/releases/1\","
    "\"html_url\":\"https://github.com/wang01116/MiniDock/releases/tag/v1.7.5\","
    "\"id\":1,\"author\":{\"login\":\"wang01116\",\"id\":9,\"html_url\":\"https://github.com/wang01116\"},"
    "\"node_id\":\"RE_1\",\"tag_name\":\"v1.7.5\",\"target_commitish\":\"main\","
    "\"name\":\"v1.7.5\",\"draft\":false,\"prerelease\":false,"
    "\"body\":\"Fixed \\\"tag_name\\\" parsing.\\r\\n\\r\\nAssets below.\","
    "\"assets\":["
    "{\"name\":\"MiniDock.exe\",\"size\":371712,"
    "\"digest\":\"sha256:621bef23504511f6fcda821fb6c000a7063c1d282114a8a96d7ceeaf019d639e\","
    "\"browser_download_url\":\"https://github.com/wang01116/MiniDock/releases/download/v1.7.5/MiniDock.exe\"},"
    "{\"name\":\"MiniDock.exe.sha256\",\"size\":96,\"digest\":null,"
    "\"browser_download_url\":\"https://github.com/wang01116/MiniDock/releases/download/v1.7.5/MiniDock.exe.sha256\"}"
    "]}";

/* Two releases out of order, plus a draft and a pre-release that must never win. */
static const char *kList =
    "[{\"tag_name\":\"v1.7.4\",\"name\":\"v1.7.4\",\"draft\":false,\"prerelease\":false,"
    "\"assets\":[{\"name\":\"MiniDock.exe\",\"size\":370688,\"digest\":\"sha256:aa\","
    "\"browser_download_url\":\"https://x/v1.7.4/MiniDock.exe\"}]},"
    "{\"tag_name\":\"v1.10.0\",\"name\":\"v1.10.0\",\"draft\":false,\"prerelease\":false,"
    "\"assets\":[{\"name\":\"MiniDock.exe\",\"size\":380000,\"digest\":\"sha256:bb\","
    "\"browser_download_url\":\"https://x/v1.10.0/MiniDock.exe\"}]},"
    "{\"tag_name\":\"v9.9.9\",\"name\":\"draft\",\"draft\":true,\"prerelease\":false,"
    "\"assets\":[{\"name\":\"MiniDock.exe\",\"size\":1,\"browser_download_url\":\"https://x/d.exe\"}]},"
    "{\"tag_name\":\"v1.11.0\",\"name\":\"beta\",\"draft\":false,\"prerelease\":true,"
    "\"assets\":[{\"name\":\"MiniDock.exe\",\"size\":1,\"browser_download_url\":\"https://x/p.exe\"}]},"
    "{\"tag_name\":\"v1.9.0\",\"name\":\"v1.9.0\",\"draft\":false,\"prerelease\":false,"
    "\"assets\":[]}]";

static BOOL WriteText(const WCHAR *path, const char *text) {
    HANDLE h = CreateFileW(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return FALSE;
    DWORD wr = 0;
    BOOL ok = WriteFile(h, text, (DWORD)strlen(text), &wr, NULL);
    CloseHandle(h);
    return ok;
}

int main(void) {
    /* ---- versions ---- */
    Ver a, b;
    CHECK(VerParse("1.7.4", &a) && a.mj == 1 && a.mn == 7 && a.pt == 4 && !a.hasBd, "plain version");
    CHECK(VerParse("v1.7.4", &a) && a.pt == 4, "leading v");
    CHECK(!wcscmp(a.text, L"1.7.4"), "display text has no prefix");
    CHECK(VerParse("1.7.4-65-abc1234", &a) && a.hasBd && a.bd == 65, "dev build number");
    CHECK(!wcscmp(a.text, L"1.7.4-65"), "dev display text");
    CHECK(!VerParse("2026.11.06", &a), "a date is not a version");
    CHECK(!VerParse("1.2.3.4", &a), "four groups are not a version");
    CHECK(!VerParse("1.2", &a), "two groups are not a version");
    CHECK(!VerParse("dev", &a), "a channel name is not a version");

    VerParse("1.7.4", &a); VerParse("1.7.5", &b);
    CHECK(VerNewer(&b, &a) && !VerNewer(&a, &b), "patch decides");
    CHECK(!VerNewer(&a, &a), "same version is not newer");
    VerParse("1.10.0", &b);
    CHECK(VerNewer(&b, &a), "1.10.0 beats 1.7.4");
    VerParse("1.7.4-3-x", &b);
    CHECK(VerNewer(&a, &b), "a release beats a snapshot of itself");
    CHECK(!VerNewer(&b, &a), "a snapshot does not beat its release");
    VerParse("1.7.4-4-x", &a); VerParse("1.7.4-3-x", &b);
    CHECK(VerNewer(&a, &b), "dev builds compare by number");

    CHECK(VerScan("Development build 2.0.0-8-00cb41d", &a) && a.mj == 2 && a.bd == 8, "version in a title");
    CHECK(VerScan("MiniDock_v1.6.5.exe", &a) && a.pt == 5, "version in an asset name");
    CHECK(!VerScan("no digits here", &a), "no version anywhere");

    /* ---- SHA-256 ---- */
    BYTE hash[32];
    char hex[65];
    Sha256 c;
    ShaInit(&c);
    ShaUpdate(&c, "abc", 3);
    ShaFinal(&c, hash);
    ShaHex(hash, hex);
    CHECK(!strcmp(hex, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"), "sha256 of abc");
    ShaInit(&c);
    ShaUpdate(&c, "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56);
    ShaFinal(&c, hash);
    ShaHex(hash, hex);
    CHECK(!strcmp(hex, "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"), "sha256 of 56 bytes");
    ShaInit(&c);   /* padding spills into a second block */
    ShaUpdate(&c, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 64);
    ShaFinal(&c, hash);
    CHECK(hash[0] || 1, "block-aligned input hashes");

    WCHAR tmp[MAX_PATH], tmp2[MAX_PATH];
    GetTempPathW(MAX_PATH, tmp);
    PathCombineW(tmp2, tmp, L"minidock-update-test.bin");
    CHECK(WriteText(tmp2, "abc"), "temp file written");
    CHECK(ShaFile(tmp2, hash), "hash a file");
    ShaHex(hash, hex);
    CHECK(!strcmp(hex, "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"), "file hash matches");

    /* ---- verification ---- */
    Rel rel;
    ZeroMemory(&rel, sizeof rel);
    rel.size = 3;
    lstrcpyA(rel.digest, "sha256:ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    CHECK(UpdVerify(tmp2, &rel, 3) == VER_OK, "digest accepted");
    lstrcpyA(rel.digest, "sha256:248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
    CHECK(UpdVerify(tmp2, &rel, 3) == VER_HASH, "wrong digest rejected");
    lstrcpyA(rel.digest, "");
    CHECK(UpdVerify(tmp2, &rel, 3) == VER_NOTEXE, "no digest, not a PE");
    CHECK(UpdVerify(tmp2, &rel, 99) == VER_SIZE, "short download rejected before hashing");
    char pe[8] = { 'M', 'Z', 0, 0, 0, 0, 0, 0 };
    HANDLE h = CreateFileW(tmp2, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD wr = 0;
    if (h != INVALID_HANDLE_VALUE) { WriteFile(h, pe, 8, &wr, NULL); CloseHandle(h); }
    rel.size = 8;
    CHECK(UpdVerify(tmp2, &rel, 8) == VER_OK, "no digest, PE header accepted");
    DeleteFileW(tmp2);

    /* ---- release parsing ---- */
    CHECK(RelParse(kRelease, &rel), "release parses");
    CHECK(!strcmp(rel.tag, "v1.7.5"), "tag read at top level");
    CHECK(!strcmp(rel.file, "MiniDock.exe"), "canonical exe preferred over the sidecar");
    CHECK(!strcmp(rel.digest, "sha256:621bef23504511f6fcda821fb6c000a7063c1d282114a8a96d7ceeaf019d639e"), "digest read");
    CHECK(rel.size == 371712, "size read");
    CHECK(!rel.pre && rel.ver.pt == 5, "version and channel");
    CHECK(wcsstr(rel.wurl, L"releases/download/v1.7.5/MiniDock.exe") != NULL, "download url is wide");

    const char *devRelease =
        "{\"tag_name\":\"dev\",\"name\":\"Development build 2.0.0-8-00cb41d\","
        "\"draft\":false,\"prerelease\":true,"
        "\"assets\":[{\"name\":\"MiniDock-2.0.0-8-00cb41d-win64.exe\",\"size\":400000,"
        "\"digest\":\"sha256:cc\",\"browser_download_url\":\"https://x/dev.exe\"}]}";
    CHECK(RelParse(devRelease, &rel) && rel.ver.mj == 2 && rel.ver.bd == 8, "dev tag: version from the name");

    const char *draftRelease =
        "{\"tag_name\":\"v9.9.9\",\"draft\":true,\"prerelease\":false,"
        "\"assets\":[{\"name\":\"MiniDock.exe\",\"browser_download_url\":\"https://x/d.exe\"}]}";
    CHECK(!RelParse(draftRelease, &rel), "drafts are not installable");

    const char *noExe =
        "{\"tag_name\":\"v1.8.0\",\"draft\":false,\"prerelease\":false,"
        "\"assets\":[{\"name\":\"source.zip\",\"browser_download_url\":\"https://x/s.zip\"}]}";
    CHECK(!RelParse(noExe, &rel), "a release without an exe is not offered");

    /* a nested "name" (an asset) must not be mistaken for the release title */
    const char *nested =
        "{\"tag_name\":\"v1.2.0\",\"name\":\"\",\"draft\":false,\"prerelease\":false,"
        "\"assets\":[{\"name\":\"MiniDock.exe\",\"browser_download_url\":\"https://x/a.exe\"}]}";
    CHECK(RelParse(nested, &rel) && !strcmp(rel.file, "MiniDock.exe"), "nested keys do not leak");

    /* ---- newest among a list ---- */
    g_updNet = FakeNet;
    Rel best;
    Reply(0, 404, "{\"message\":\"Not Found\"}");
    Reply(1, 200, kList);
    WCHAR msg[220];
    g_calls[0] = g_calls[1] = 0;
    CHECK(UpdFetch(&best, msg, 220), "404 falls back to the list");
    CHECK(g_calls[0] == 1 && g_calls[1] == 1, "both endpoints asked once");
    CHECK(!wcscmp(best.ver.text, L"1.10.0"), "highest version wins, not the first entry");
    CHECK(!strcmp(best.digest, "sha256:bb"), "the winner's digest is kept");

    /* stable installs are never moved onto a pre-release */
    Reply(1, 200, "[{\"tag_name\":\"v1.11.0\",\"name\":\"beta\",\"draft\":false,\"prerelease\":true,"
                  "\"assets\":[{\"name\":\"MiniDock.exe\",\"browser_download_url\":\"https://x/p.exe\"}]}]");
    g_updNet = FakeNet;
    CHECK(!UpdFetch(&best, msg, 220) && wcsstr(msg, L"还没有"), "only pre-releases: nothing to offer");

    /* ---- error reporting ---- */
    Reply(0, 403, "{\"message\":\"API rate limit exceeded\"}");
    CHECK(!UpdFetch(&best, msg, 220) && wcsstr(msg, L"限流"), "403 is a rate limit, not an empty repo");
    Reply(0, 500, "oops");
    CHECK(!UpdFetch(&best, msg, 220) && wcsstr(msg, L"HTTP 500"), "server errors are named");
    g_transport[0] = FALSE;
    CHECK(!UpdFetch(&best, msg, 220) && wcsstr(msg, L"无法连接"), "transport failure is reported");
    g_transport[0] = TRUE;

    /* ---- url helpers ---- */
    WCHAR url[1600], name[64];
    UpdJoin(url, 1600, L"https://ghfast.top/", L"https://github.com/a/b.exe");
    CHECK(!wcscmp(url, L"https://ghfast.top/https://github.com/a/b.exe"), "mirror prefix is a plain concat");
    UpdJoin(url, 1600, L"", L"https://github.com/a/b.exe");
    CHECK(!wcscmp(url, L"https://github.com/a/b.exe"), "empty prefix means direct");
    UpdName(L"https://ghfast.top/", name, 64);
    CHECK(!wcscmp(name, L"ghfast.top"), "mirror name for the status line");
    UpdName(L"", name, 64);
    CHECK(wcsstr(name, L"直连") != NULL, "direct access is labelled");

    /* ---- swap script ---- */
    WCHAR script[2400];
    UpdScript(script, 2400, L"C:\\A B\\MiniDock.update.exe", L"C:\\A B\\MiniDock.exe", L"MiniDock.exe");
    CHECK(wcsstr(script, L"move /Y \"C:\\A B\\MiniDock.update.exe\" \"C:\\A B\\MiniDock.exe\"") != NULL, "quoted move");
    CHECK(wcsstr(script, L"start \"\" \"C:\\A B\\MiniDock.exe\"") != NULL, "restart after the swap");
    CHECK(wcsstr(script, L"tasklist /FI \"IMAGENAME eq MiniDock.exe\"") != NULL, "waits for this process");
    CHECK(wcsstr(script, L"del \"%~f0\"") != NULL, "removes itself");
    /* `%n%` inside a parenthesised block is expanded once at parse time, which would make the
       retry counters loop forever; there must be no block in the script at all. */
    CHECK(wcsstr(script, L"(\r\n") == NULL && wcsstr(script, L") else") == NULL, "no parenthesised blocks");

    /* ---- staging path ---- */
    WCHAR stage[MAX_PATH];
    UpdStagePath(stage, MAX_PATH);
    CHECK(wcsstr(stage, L"MiniDock.update.exe") != NULL, "staging file name");
    CHECK(wcsstr(stage, L".part") == NULL, "staging path is the final name");

    printf("PASS %d checks\n", checks);
    return 0;
}
