<div align="center">
  <img src="img/akerbeltz-logo.png" alt="Akerbeltz logo" width="280">
  <h3>Akerbeltz</h3>
  <p>
    <a href="LICENSE"><img alt="License" src="https://img.shields.io/github/license/neluj/Akerbeltz"></a>
    <a href="https://github.com/neluj/Akerbeltz/releases"><img alt="Version" src="https://img.shields.io/github/v/release/neluj/Akerbeltz?sort=semver"></a>
    <br>
    <a href="https://en.cppreference.com/w/cpp/23"><img alt="C++23" src="https://img.shields.io/badge/C%2B%2B-23-00599C?logo=c%2B%2B&logoColor=white"></a>
    <a href="https://github.com/google/googletest"><img alt="GoogleTest" src="https://img.shields.io/badge/GoogleTest-1.14.0-00A98F?logo=google&logoColor=white"></a>
    <a href="https://cmake.org/"><img alt="CMake" src="https://img.shields.io/badge/CMake-3.20%2B-064F8C?logo=cmake&logoColor=white"></a>
  </p>
</div>

## Table of contents
- [Overview](#overview)
- [Project layout](#project-layout)
- [Implemented techniques](#implemented-techniques)
- [Engine strength](#engine-strength)
- [Build and run](#build-and-run)
- [Using the engine](#using-the-engine)
- [Scripts](#scripts)
- [Sources](#sources)
- [Author](#author)

# Overview
Akerbeltz is a UCI chess engine written in C++23. This project started out of interest in putting key techniques into practice in a chess engine: performance, efficiency, and strategy. My goal has been to progress incrementally, implementing and documenting each improvement to measure its impact gradually, rather than building a “powerful” engine from day one.

## Project layout
Core layout and where to look for each subsystem:
- `src/` engine core (position, movegen, search, eval, TT, UCI, time manager).
- `test/` unit and integration tests (GoogleTest v1.14.0).
- `scripts/` utilities for perft, SPRT/smoke runs, and logs/results.
- `img/` logo and other static assets.
- `CMakeLists.txt` and `src/CMakeLists.txt` for the main build; `test/CMakeLists.txt` for tests.
- `README.md` project overview and usage.

## Implemented techniques

### Search
- [Iterative Deepening](https://www.chessprogramming.org/Iterative_Deepening) to refine the PV starting at depth 1.
- [Alpha-Beta](https://www.chessprogramming.org/Alpha-Beta) with full windows.
- [Quiescence Search](https://www.chessprogramming.org/Quiescence_Search) at leaf nodes to reduce tactical noise.
- [Check Extensions](https://www.chessprogramming.org/Check_Extensions) that extend depth when the side to move is in check.
- [Null Move Pruning](https://www.chessprogramming.org/Null_Move_Pruning) in non-endgames for aggressive cutoffs.
- [Transposition Table](https://www.chessprogramming.org/Transposition_Table) to cache scores and PV lines.

### Move ordering
- [Hash Move](https://www.chessprogramming.org/Hash_Move) from the TT.
- [MVV-LVA](https://www.chessprogramming.org/MVV-LVA) to prioritize captures.
- [Killer Move](https://www.chessprogramming.org/Killer_Heuristic) per ply to accelerate beta cutoffs.
- [History Heuristic](https://www.chessprogramming.org/History_Heuristic) for quiet moves.

### Evaluation
- [Piece-Square Tables](https://www.chessprogramming.org/Piece-Square_Tables) for MG/EG positional values.
- [Material](https://www.chessprogramming.org/Material) with MG/EG piece values and tempo bonus.
- [Tapered Eval](https://www.chessprogramming.org/Tapered_Eval) based on game phase.
- [Tempo](https://www.chessprogramming.org/Tempo) bonus for the side to move.

### Board Representation
- [Bitboards](https://www.chessprogramming.org/Bitboards) per color and piece type, plus occupancy for white/black/all.
- [Mailbox](https://www.chessprogramming.org/Mailbox) array for piece-on-square lookups and FEN I/O.
- [Zobrist Hashing](https://www.chessprogramming.org/Zobrist_Hashing) incremental key for TT/repetition (piece-square, castling, en passant, side).

### Move Generation
- [Move Generation](https://www.chessprogramming.org/Move_Generation) uses pseudo-legal generation per side; legality is verified by make/unmake in search.
- [Attack Tables](https://www.chessprogramming.org/Attacks) for pawn, knight, and king attacks.
- [Magic Bitboards](https://www.chessprogramming.org/Magic_Bitboards) for sliding piece attacks (rook/bishop/queen).

### Time management
- [Time Management](https://www.chessprogramming.org/Time_Management) with budgets for increment/movetime and iteration prediction.

### Protocol and rules
- [UCI](https://www.chessprogramming.org/UCI) with `Hash`, `ucinewgame`, `stop`, `quit`, and FEN support.
- [Perft](https://www.chessprogramming.org/Perft) via `go perft` to validate move generation.

## Engine strength
These results are from internal matches and are meant as a quick reference. Opponents are selected from the [CCRL](https://computerchess.org.uk/ccrl/404/) (Computer Chess Rating Lists). CCRL is a community rating list that benchmarks engines under standardized conditions and publishes multiple lists for different time controls (e.g., 40/4, 40/15, blitz). The table uses the 40/4 list as of 2025-12-20; 40/4 means 40 moves in 4 minutes. The tests were run on Ubuntu 24.04.3 LTS, Intel(R) Core(TM) i7-7700HQ CPU @ 2.80GHz, 16 GB RAM. Locally, the matches were run with the smoke test script (`scripts/smoke_test_ab.sh`) using cutechess time control `--tc 40/4` and the [UHO 2024 opening book](https://www.sp-cc.de/uho_2024.htm), specifically the 8‑move positions set in the 085–094 Elo band (EPD). The estimated Elo difference (Akerbeltz-1.0.0 minus opponent) and +/- values are taken from the match summary and should be read as rough indicators for this run.

<table>
  <thead>
    <tr>
      <th>Engine</th>
      <th>Version</th>
      <th>Elo</th>
      <th>Akerbeltz-1.0.0 vs (W-L-D)</th>
      <th>Score</th>
      <th>Elo diff (Akerbeltz-1.0.0 +/-)</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><a href="https://github.com/JasonCreighton/snowy">Snowy</a></td>
      <td>0.2 64-bit</td>
      <td>1871 (+20/-20)</td>
      <td>69-287-44</td>
      <td>0.228</td>
      <td>-212.4 +/- 37.5</td>
    </tr>
    <tr>
      <td><a href="https://github.com/stnevans/Apollo">Apollo</a></td>
      <td>1.2.1 64-bit</td>
      <td>1665 (+19/-19)</td>
      <td>119-234-47</td>
      <td>0.356</td>
      <td>-102.8 +/- 33.3</td>
    </tr>
    <tr>
      <td><a href="https://github.com/GunshipPenguin/shallow-blue">Shallow Blue</a></td>
      <td>2.0.0 64-bit</td>
      <td>1572 (+20/-20)</td>
      <td>178-174-48</td>
      <td>0.505</td>
      <td>3.5 +/- 32.0</td>
    </tr>
    <tr>
      <td><a href="https://github.com/En-En-Code/cookiecat">CookieCat</a></td>
      <td>2012.10.10 64-bit</td>
      <td>1564 (+20/-20)</td>
      <td>205-125-70</td>
      <td>0.600</td>
      <td>70.4 +/- 31.5</td>
    </tr>
    <tr>
      <td><a href="https://github.com/NicolasSegl/Athena">Athena</a></td>
      <td>1.0.1</td>
      <td>1529 (+19/-19)</td>
      <td>215-39-146</td>
      <td>0.720</td>
      <td>164.1 +/- 28.1</td>
    </tr>
    <tr>
      <td><a href="https://github.com/tm512/minnow">Minnow</a></td>
      <td>no version</td>
      <td>1446 (+19/-19)</td>
      <td>241-103-56</td>
      <td>0.672</td>
      <td>125.0 +/- 33.4</td>
    </tr>
  </tbody>
</table>

Based on these matches, **Akerbeltz-1.0.0 lands around ~1615 Elo for this specific pool and time control**, which is the key takeaway from the table.

## Build and run

Optional CPU tuning: by default builds are generic. You can pass `-DAKERBELTZ_ARCH=native` (or e.g. `-DAKERBELTZ_ARCH=x86-64-v3`) to add `-march=<value>`.
Note: other toolchains/compilers may work, but the ones listed below are the ones tested.

### Linux/macOS
- Prerequisites: CMake ≥ 3.20, a C++23 compiler (e.g., GCC 13+ or Clang 16+), and a build tool such as `make` or `ninja`.
- Configure and build (Release by default):
  ```bash
  cmake -S . -B build
  cmake --build build
  ```
- Run the engine (UCI). The binary is named `Akerbeltz-<version>` (default `Akerbeltz-1.0.0`, override with `-DAKERBELTZ_ENGINE_VERSION=...`):
  ```bash
  ./build/Akerbeltz-1.0.0
  ```
- Tests are OFF by default; enable them with `-DAKERBELTZ_BUILD_TESTS=ON` when configuring (GoogleTest is fetched automatically).
  ```bash
  cmake -S . -B build -DAKERBELTZ_BUILD_TESTS=ON
  cmake --build build
  ```
- Run tests:
  ```bash
  ctest --test-dir build --output-on-failure
  ```

### Windows (MSYS2 MINGW64)
- Prerequisites: MSYS2 MINGW64 with GCC (mingw-w64-x86_64-gcc), CMake >= 3.20, and Ninja.
- Configure and build (Release by default):
  ```bash
  cmake -S . -B build -G Ninja -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
  cmake --build build
  ```
- Run the engine (UCI). The binary is named `Akerbeltz-<version>.exe` (default `Akerbeltz-1.0.0.exe`, override with `-DAKERBELTZ_ENGINE_VERSION=...`):
  ```bash
  ./build/Akerbeltz-1.0.0.exe
  ```
- Tests are OFF by default; enable them with `-DAKERBELTZ_BUILD_TESTS=ON` when configuring:
  ```bash
  cmake -S . -B build -G Ninja -DAKERBELTZ_BUILD_TESTS=ON DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
  cmake --build build
  ```
- Run tests:
  ```bash
  ctest --test-dir build --output-on-failure
  ```

## Using the engine

### Console (UCI)
- [UCI](https://www.chessprogramming.org/UCI) (Universal Chess Interface) Protocol is a text protocol used to connect engines with GUIs and tournament tools.
- Implemented UCI commands:
  - `uci`: identifies the engine and supported options.
  - `isready`: synchronization point; replies `readyok`.
  - `setoption name Hash value <MB>`: sets TT size in MB.
  - `ucinewgame`: resets internal state for a new game.
  - `position`: sets the current position and optional move list.
    - `position startpos [moves ...]`: loads the start position and applies optional moves.
    - `position fen <FEN> [moves ...]`: loads a FEN and applies optional moves.
  - `go`: starts a search with the specified limits.
    - `go depth <N>`: searches to a fixed depth.
    - `go wtime <ms> btime <ms> winc <ms> binc <ms> movestogo <n>`: searches with time control.
    - `go movetime <ms>`: searches for a fixed time per move.
    - `go infinite`: searches until `stop`.
  - `stop`: stops the current search.
  - `quit`: exits the engine.
- Extra commands (non-UCI):
  - `go perft <N>`: runs perft and prints the node count at depth N.
  - `d`: prints the board state (debug helper).
- Examples (UCI):
  ```bash
  ./build/Akerbeltz-1.0.0
  uci
  isready
  position startpos
  go depth 10
  ```
  ```bash
  position fen r1bqkbnr/pppppppp/2n5/8/3P4/5N2/PPP1PPPP/RNBQKB1R w KQkq - 2 2 moves d4d5
  go wtime 300000 btime 300000 winc 2000 binc 2000 movestogo 40
  ```
- Example (perft):
  ```bash
  position startpos
  go perft 5
  ```

### GUI (UCI)
- The engine speaks UCI, so it can be used with any UCI-compatible GUI. The tools listed below are just examples.
- Arena (Windows/Linux):
  1. Open [Arena](https://www.playwitharena.de).
  2. Go to `Engines → Install New Engine...`.
  3. Select `Akerbeltz-<version>` (Linux) or `Akerbeltz-<version>.exe` (Windows) from your build folder.
  4. When prompted, choose UCI as the protocol.
  5. Configure options like `Hash` under `Engines → Manage...` if needed.
  6. Start a new game and select Akerbeltz as the engine.
- Cute Chess (Linux/Windows/macOS):
  1. Open Cute Chess and go to the engine manager.
  2. Add a new engine and point it to `Akerbeltz-<version>` (Linux/macOS) or `Akerbeltz-<version>.exe` (Windows).
  3. Select UCI as the protocol and save the engine.
  4. Start a game or tournament and pick Akerbeltz as one of the engines.
- Other UCI GUIs (e.g., Banksia, SCID vs. PC) are available on Linux/macOS/Windows:
  1. Add a new engine and point it to `Akerbeltz-<version>` (Linux/macOS) or `Akerbeltz-<version>.exe` (Windows).
  2. Select UCI as the protocol and adjust options (e.g., `Hash`) if available.

## Scripts
- The shell scripts (`smoke_test_ab.sh`, `sprt_ci.sh`, `confirm_ltc.sh`) are Bash-based. On Windows, run them via WSL2 or a similar environment.
- Those three scripts read engine definitions from `scripts/engines.tsv` and write logs to `scripts/logs/` and PGNs to `scripts/pgn/`.
- `scripts/engines.tsv` is semicolon-separated with columns: `name`, `cmd`, `proto` (optional: `uci` or `xboard`/`winboard`), `hash`. If `proto` is omitted/blank, it defaults to `uci`. If you want a hash but no proto, keep the empty field: `name;cmd;;hash`.
- Note: Scripts and utilities tested on Ubuntu 22.04.

### `scripts/smoke_test_ab.sh`
- Purpose: fast A/B smoke test with cutechess-cli using EPD openings and both colors per position.
- Requirements: `bash`, `cutechess-cli`, standard Unix tools (`awk`, `sed`, `grep`, `nproc`, `stdbuf`, `tee`), `scripts/engines.tsv`, and an EPD openings file.
- Tip: running without parameters prints the script usage/help.
- Usage example:
  ```bash
  ./scripts/smoke_test_ab.sh --engines scripts/engines.tsv \
    --openings scripts/utils/UHO_2024/UHO_2024_+090_+099/UHO_2024_8mvs_+090_+099.epd \
    --rounds 10 --tc 3+0.05
  ```

### `scripts/sprt_ci.sh`
- Purpose: A/B SPRT test with cutechess-cli (EPD or PGN openings).
- Requirements: `bash`, `cutechess-cli`, standard Unix tools (`awk`, `nproc`, `stdbuf`, `tee`), `scripts/engines.tsv`, and an EPD/PGN openings file.
- Tip: running without parameters prints the script usage/help.
- Usage example:
  ```bash
  ./scripts/sprt_ci.sh --engines scripts/engines.tsv \
    --openings scripts/utils/UHO_2024/UHO_2024_+090_+099/UHO_2024_8mvs_+090_+099.epd \
    --sprt-elo0 0 --sprt-elo1 5 --tc 10+0.1
  ```

### `scripts/confirm_ltc.sh`
- Purpose: long time control confirmation (pairwise or round-robin) with cutechess-cli.
- Requirements: `bash`, `cutechess-cli`, standard Unix tools (`awk`, `sed`, `grep`, `nproc`, `stdbuf`, `tee`), `scripts/engines.tsv`, and a PGN openings file.
- Tip: running without parameters prints the script usage/help.
- Usage example:
  ```bash
  ./scripts/confirm_ltc.sh --engines scripts/engines.tsv \
    --openings scripts/utils/UHO_2024/UHO_2024_+090_+099/UHO_2024_8mvs_+090_+099.pgn \
    --rounds 10 --tc 40/300+3
  ```

### `scripts/perft_suite.py`
- Purpose: run perft checks against an EPD suite and compare expected node counts.
- Requirements: `python3`, a built engine binary that supports `go perft`, and an EPD file (format includes FEN plus `D<depth> <nodes>` pairs).
- Note: you can use `scripts/utils/perftsuite.epd`. I did not create this file; I found it online and could not identify the original author, apologies.
- Output: prints per-depth results (`OK`/`ERR`), the FEN, a summary with totals, and the log path; writes a timestamped log in `results/`.
- Input format: EPD lines with a FEN followed by depth/node pairs (e.g., `... D1 20; D2 400; D3 8902;`).
- Example EPD lines:
  ```text
  rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1; D1 20; D2 400; D3 8902;
  r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1; D1 48; D2 2039; D3 97862;
  ```
- Usage example:
  ```bash
  python3 scripts/perft_suite.py --engine ./build/Akerbeltz-1.0.0 \
    --epd path/to/your_suite.epd --depths 1,2,3 --max-positions 20
  ```

## Sources
- [Chess Programming Wiki](https://www.chessprogramming.org/)
- [Stockfish](https://stockfishchess.org/)
- [Bluefever Chess Programming in C (YouTube playlist)](https://www.youtube.com/playlist?list=PLZ1QII7yudbc-Ky058TEaOstZHVbT-2hg)
- [Rustic Chess Engine (Marcel Vanthoor)](https://rustic-chess.org/)
- [Fast Chess Move Generation with Magic Bitboards (Marcel Vanthoor)](https://rhysre.net/fast-chess-move-generation-with-magic-bitboards.html)
- [PeSTO's Evaluation Function (Ronald Friederich)](https://www.chessprogramming.org/PeSTO%27s_Evaluation_Function)

## Author
- Name: Julen Aristondo
- Email: julen_aristondo@hotmail.com
- [GitHub](https://github.com/neluj)
- [LinkedIn](https://www.linkedin.com/in/julen-aristondo-5735a125b)
