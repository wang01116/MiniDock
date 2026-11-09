"""Compare isolated drawing/scanning workloads using a private copy of a profile.

Requires Windows, Python, git and MinGW-w64 gcc in PATH. Never starts MiniDock's
WinMain, hooks, guardian, AppBars or startup integration. Does not publish profiles.
"""
import argparse
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--profile", type=Path, required=True,
                        help="directory containing MiniDock.ini and optional icons/")
    parser.add_argument("--baseline", default="origin/main")
    parser.add_argument("--out", type=Path, required=True)
    args = parser.parse_args()
    repo = Path(__file__).resolve().parent.parent
    profile = args.profile.resolve()
    if not (profile / "MiniDock.ini").is_file():
        parser.error("profile must contain MiniDock.ini")
    compiler = shutil.which("gcc")
    if not compiler:
        parser.error("gcc is not in PATH")
    libraries = "gdiplus ole32 oleaut32 shell32 shlwapi user32 gdi32 comctl32 comdlg32 uuid urlmon version dwmapi advapi32 psapi uxtheme iphlpapi imm32 wtsapi32 powrprof msimg32 winmm".split()
    baseline = subprocess.check_output(
        ["git", "show", f"{args.baseline}:MiniDock-source/minidock.c"], cwd=repo)
    baseline_oid = subprocess.check_output(
        ["git", "rev-parse", args.baseline], cwd=repo, text=True).strip()
    temp_root = Path(tempfile.gettempdir()).resolve()
    work = Path(tempfile.mkdtemp(prefix="MiniDock-resource-")).resolve()
    try:
        private = work / "profile"
        private.mkdir()
        shutil.copy2(profile / "MiniDock.ini", private / "MiniDock.ini")
        if (profile / "icons").is_dir():
            shutil.copytree(profile / "icons", private / "icons")
        (work / "upstream.c").write_bytes(baseline)
        sources = {"upstream": work / "upstream.c", "candidate": repo / "MiniDock-source/minidock.c"}
        for name, source in sources.items():
            command = [compiler, "-Os", "-ffunction-sections", "-Wl,--gc-sections",
                       "-o", str(work / f"{name}.exe"), str(repo / "tests/resource_comparison.c"),
                       f'-DMINIDOCK_BENCH_SOURCE="{source.as_posix()}"']
            if name == "candidate":
                command += ["-DBENCH_GROUP_RUNNING", "-DBENCH_STATUS_CURRENT"]
                command.append(str(repo / "tests/wgc_stub.c"))
            subprocess.run(command + ["-l" + lib for lib in libraries], check=True, cwd=work)
        rows = []
        cases = [(name, round_number, False) for round_number, order in
                 [(1, ["upstream", "candidate"]), (2, ["candidate", "upstream"])] for name in order]
        cases.append(("candidate", 3, True))
        args.out.parent.mkdir(parents=True, exist_ok=True)
        for name, round_number, group_off in cases:
            result = subprocess.run([str(work / f"{name}.exe"), str(private)] +
                                    (["--group-off"] if group_off else []),
                                    capture_output=True, text=True, encoding="utf-8",
                                    errors="replace", timeout=45, check=True)
            row = json.loads(result.stdout.strip().splitlines()[-1])
            row.update(version=name, round=round_number, group_running_disabled=group_off,
                       baseline_commit=baseline_oid, logical_processors=os.cpu_count())
            rows.append(row)
            args.out.write_text(json.dumps(rows, indent=2) + "\n", encoding="utf-8", newline="\n")
            print(json.dumps(row), flush=True)
    finally:
        # Verify the resolved cleanup target is exactly our temporary workspace.
        if work.parent != temp_root or not work.name.startswith("MiniDock-resource-"):
            raise RuntimeError("refusing cleanup outside the benchmark temporary workspace")
        shutil.rmtree(work)


if __name__ == "__main__":
    main()
