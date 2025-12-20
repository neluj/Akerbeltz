#!/usr/bin/env python3
import argparse
import re
import subprocess
import sys
import time
from dataclasses import dataclass
from pathlib import Path
from typing import Callable, Dict, List, Optional, Tuple

# --- Regex patterns matching the user's engine output & EPD ---
EPD_LINE_RE = re.compile(r"^\s*(?P<fen>(?:\S+\s+){5}\S+)\s*(?P<ops>.*)$")
DEPTH_KV_RE = re.compile(r"\bD(?P<depth>\d+)\s+(?P<nodes>\d+)\b")
TOTAL_NODES_RE = re.compile(r"total nodes size:\s*(\d+)", re.IGNORECASE)
UCI_OK_RE = re.compile(r"\buciok\b", re.IGNORECASE)
READY_OK_RE = re.compile(r"\breadyok\b", re.IGNORECASE)

ANSI_GREEN = "\033[32m"
ANSI_RED = "\033[31m"
ANSI_RESET = "\033[0m"

def colorize(text: str, ok: bool, enable: bool) -> str:
    if not enable:
        return text
    return f"{ANSI_GREEN}{text}{ANSI_RESET}" if ok else f"{ANSI_RED}{text}{ANSI_RESET}"

@dataclass
class PerftCase:
    fen: str
    expected: Dict[int, int]  # depth -> nodes
    id: Optional[str] = None

@dataclass
class PerftResult:
    fen: str
    depth: int
    expected: int
    got: Optional[int]  # None if timeout or parse error
    ok: bool
    elapsed_ms: int
    error: Optional[str] = None

@dataclass
class Summary:
    total_checks: int = 0
    passed: int = 0
    failed: int = 0
    errors: int = 0
    duration_ms: int = 0


def parse_epd(epd_path: Path, limit: Optional[int] = None) -> List[PerftCase]:
    cases: List[PerftCase] = []
    with epd_path.open("r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            m = EPD_LINE_RE.match(line)
            if not m:
                continue
            fen = m.group("fen")
            ops = m.group("ops")
            expected: Dict[int, int] = {}
            for dm in DEPTH_KV_RE.finditer(ops):
                d = int(dm.group("depth"))
                n = int(dm.group("nodes"))
                expected[d] = n
            if expected:
                cases.append(PerftCase(fen=fen, expected=expected))
                if limit is not None and len(cases) >= limit:
                    break
    return cases


class Engine:
    def __init__(self, path: Path, timeout_sec: float = 60.0):
        self.path = path
        self.timeout_sec = timeout_sec
        self.proc: Optional[subprocess.Popen] = None

    def start(self):
        self.proc = subprocess.Popen(
            [str(self.path)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            universal_newlines=True,
            bufsize=1,
        )
        # Handshake UCI
        self._writeln("uci")
        self._read_until_regex(UCI_OK_RE, self.timeout_sec)
        self._writeln("isready")
        self._read_until_regex(READY_OK_RE, self.timeout_sec)

    def stop(self):
        if self.proc and self.proc.poll() is None:
            try:
                self._writeln("quit")
                self.proc.wait(timeout=2)
            except Exception:
                self.proc.kill()
        self.proc = None

    def perft(self, fen: str, depth: int, timeout_sec: Optional[float] = None) -> Tuple[Optional[int], str, int]:
        """
        Returns (nodes, raw_output, elapsed_ms). nodes=None on timeout/parse error.
        """
        if not self.proc or self.proc.poll() is not None:
            raise RuntimeError("Engine process is not running. Call start().")

        t0 = time.time()
        self._writeln(f"position fen {fen}")
        self._writeln(f"go perft {depth}")
        # Read until "total nodes size:" line arrives or timeout
        try:
            nodes, raw = self._read_total_nodes(timeout_sec or self.timeout_sec)
        except TimeoutError as e:
            return None, str(e), int((time.time() - t0) * 1000)
        return nodes, raw, int((time.time() - t0) * 1000)

    # --- helpers ---
    def _writeln(self, s: str):
        assert self.proc and self.proc.stdin
        self.proc.stdin.write(s + "\n")
        self.proc.stdin.flush()

    def _read_total_nodes(self, timeout_sec: float) -> Tuple[Optional[int], str]:
        assert self.proc and self.proc.stdout
        deadline = time.time() + timeout_sec
        buf_lines: List[str] = []
        nodes: Optional[int] = None
        while time.time() < deadline:
            line = self.proc.stdout.readline()
            if not line:
                time.sleep(0.01)
                continue
            buf_lines.append(line.rstrip("\n"))
            m = TOTAL_NODES_RE.search(line)
            if m:
                try:
                    nodes = int(m.group(1))
                except ValueError:
                    nodes = None
                break
        if nodes is None:
            raise TimeoutError(f"Timed out ({timeout_sec}s) waiting for perft result")
        return nodes, "\n".join(buf_lines)

    def _read_until_regex(self, pattern: re.Pattern, timeout_sec: float):
        assert self.proc and self.proc.stdout
        deadline = time.time() + timeout_sec
        while time.time() < deadline:
            line = self.proc.stdout.readline()
            if not line:
                time.sleep(0.01)
                continue
            if pattern.search(line):
                return
        raise TimeoutError(f"Timeout waiting for pattern: {pattern.pattern}")


def run_suite(engine_path: Path, epd_path: Path, depths_filter: Optional[List[int]], max_positions: Optional[int],
              stop_on_first_mismatch: bool, timeout_sec: float,
              on_result: Optional[Callable[[PerftResult], None]] = None) -> Summary:
    """
    Execute perft for each case/depth and call on_result as soon as each finishes.
    Returns only the final summary.
    """
    cases = parse_epd(epd_path, limit=max_positions)
    summary = Summary()
    t_global0 = time.time()

    eng = Engine(engine_path, timeout_sec=timeout_sec)
    try:
        eng.start()
        for case_idx, case in enumerate(cases, start=1):
            depths = sorted(case.expected.keys())
            if depths_filter:
                depths = [d for d in depths if d in depths_filter]
                if not depths:
                    continue
            for d in depths:
                exp = case.expected[d]
                nodes, raw, elapsed_ms = eng.perft(case.fen, d, timeout_sec=timeout_sec)
                summary.total_checks += 1
                if nodes is None:
                    result = PerftResult(fen=case.fen, depth=d, expected=exp, got=None, ok=False,
                                         elapsed_ms=elapsed_ms, error="timeout/parse-error")
                    summary.errors += 1
                    if on_result: on_result(result)
                    if stop_on_first_mismatch:
                        raise RuntimeError("Stopping on first error")
                else:
                    ok = (nodes == exp)
                    if ok:
                        summary.passed += 1
                    else:
                        summary.failed += 1
                    result = PerftResult(fen=case.fen, depth=d, expected=exp, got=nodes, ok=ok,
                                         elapsed_ms=elapsed_ms, error=None if ok else f"diff={nodes-exp:+d}")
                    if on_result: on_result(result)
                    if (not ok) and stop_on_first_mismatch:
                        raise RuntimeError("Stopping on first mismatch")
    finally:
        eng.stop()
        summary.duration_ms = int((time.time() - t_global0) * 1000)

    return summary


def main():
    p = argparse.ArgumentParser(description="Run perft tests from an EPD file against a UCI engine and compare.")
    p.add_argument("--engine", required=True, help="Path to engine binary (UCI-compliant).")
    p.add_argument("--epd", default="perftsuite.epd", help="Path to EPD file (default: ./perftsuite.epd)")
    p.add_argument("--depths", default=None, help="Comma-separated list of depths to check (e.g., 1,2,3). Default: all depths present per line.")
    p.add_argument("--max-positions", type=int, default=None, help="Limit number of positions read from EPD.")
    p.add_argument("--timeout-sec", type=float, default=60.0, help="Timeout per perft (seconds).")
    p.add_argument("--stop-on-first-mismatch", action="store_true", help="Stop immediately when a mismatch is found.")
    p.add_argument("--outdir", default=None, help="Optional output directory for logs (default: ./results in CWD).")
    p.add_argument("--quiet", action="store_true", help="Only show summary and log path in console.")
    args = p.parse_args()

    engine_path = Path(args.engine)
    epd_path = Path(args.epd)

    if not engine_path.exists():
        print(f"ERROR: Engine binary not found: {engine_path}", file=sys.stderr)
        return 2
    if not epd_path.exists():
        print(f"ERROR: EPD file not found: {epd_path}", file=sys.stderr)
        return 2

    # Parse depth filter
    depths_filter = None
    if args.depths:
        try:
            depths_filter = [int(x.strip()) for x in args.depths.split(",") if x.strip()]
        except ValueError:
            print("ERROR: --depths must be a comma-separated list of integers", file=sys.stderr)
            return 2

    # Create output directory and open log file immediately
    from datetime import datetime
    outdir = Path(args.outdir) if args.outdir else (Path.cwd() / "results")
    outdir.mkdir(parents=True, exist_ok=True)
    timestamp = datetime.now().strftime("%Y%m%d-%H%M%S")
    engine_tag = engine_path.name
    log_path = outdir / f"perft_{engine_tag}_{timestamp}.log"

    enable_color = sys.stdout.isatty()

    def cprint(*a, **kw):
        print(*a, **kw)
        sys.stdout.flush()

    # Startup message
    cprint("Starting perft tests... this might take a while.")

    header_sep = "=" * 80
    # Write header immediately to file
    with log_path.open("w", encoding="utf-8") as lf:

        # Run suite with per-result callback that prints & logs in real time
        def on_result(res: PerftResult):
            status_plain = "OK " if res.ok else "ERR"
            status_col = colorize(status_plain, res.ok, enable_color and not args.quiet)
            got_str = "TIMEOUT" if res.got is None else str(res.got)
            line_plain = f"[{status_plain}] depth D{res.depth}  expected={res.expected}  got={got_str}  Δ={'' if res.got is None else res.got - res.expected:+d}  ({res.elapsed_ms} ms)"
            line_col = f"[{status_col}] depth D{res.depth}  expected={res.expected}  got={got_str}  Δ={'' if res.got is None else res.got - res.expected:+d}  ({res.elapsed_ms} ms)"

            # Console (unless quiet)
            if not args.quiet:
                cprint(line_col)
                cprint(f"FEN: {res.fen}")
                cprint("-" * 80)

            # File (plain)
            lf.write(line_plain + "\n")
            lf.write(f"FEN: {res.fen}\n")
            lf.write("-" * 80 + "\n")
            lf.flush()

        t0 = time.time()
        summary = run_suite(
            engine_path=engine_path,
            epd_path=epd_path,
            depths_filter=depths_filter,
            max_positions=args.max_positions,
            stop_on_first_mismatch=args.stop_on_first_mismatch,
            timeout_sec=args.timeout_sec,
            on_result=on_result
        )
        # After completion, write summary to file and optionally to console
        lf.write(header_sep + "\n")
        lf.write(f"Checked cases: {summary.total_checks} | Passed: {summary.passed} | Failed: {summary.failed} | Errors: {summary.errors}\n")
        lf.write(f"Total duration: {summary.duration_ms} ms\n")
        lf.write(header_sep + "\n")
        lf.flush()

    # Print summary to console (even in quiet mode we print summary & log path)
    cprint(header_sep)
    cprint(f"Checked cases: {summary.total_checks} | Passed: {summary.passed} | Failed: {summary.failed} | Errors: {summary.errors}")
    cprint(f"Total duration: {summary.duration_ms} ms")
    cprint(header_sep)

    cprint(f"Log saved to: {log_path}")

    # Exit code: 0 if all passed, 1 if any failed, 2 for usage errors above
    return 0 if (summary.failed == 0 and summary.errors == 0) else 1


if __name__ == "__main__":
    sys.exit(main())
