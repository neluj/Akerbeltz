<div align="center">
  <picture>
    <source media="(prefers-color-scheme: dark)" srcset="img/akerbeltz-white.svg">
    <source media="(prefers-color-scheme: light)" srcset="img/akerbeltz-black.svg">
    <img src="img/akerbeltz-black.svg" alt="Akerbeltz logo" width="280">
  </picture>
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
- `src/` engine core.
- `test/` unit and integration tests (GoogleTest v1.14.0).
- `scripts/` utilities for perft.
- `img/` logos.
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

<table>
  <thead>
    <tr>
      <th>Version</th>
      <th>List</th>
      <th>Rating</th>
      <th>Rank</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>1.0.0</td>
      <td>Local</td>
      <td>1615</td>
      <td>-</td>
    </tr>
    <tr>
      <td>1.1.0</td>
      <td>Local</td>
      <td>2254</td>
      <td>-</td>
    </tr>
  </tbody>
</table>

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
- Run the engine (UCI). The binary is named `Akerbeltz-<version>`:
  ```bash
  ./build/Akerbeltz-1.1.0
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

### Windows (MSYS2 UCRT64)
- Prerequisites: use the `MSYS2 UCRT64` shell with GCC, CMake ≥ 3.20, and Ninja:
  ```bash
  pacman -S --needed mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-ninja
  ```
- Configure and build a generic static binary (recommended for widest compatibility across Windows PCs):
  ```bash
  cmake -S . -B build-win -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DAKERBELTZ_BUILD_TESTS=OFF \
    -DCMAKE_EXE_LINKER_FLAGS="-static -static-libgcc -static-libstdc++"
  cmake --build build-win -j
  strip build-win/Akerbeltz-1.1.0.exe
  ```
- Run the engine (UCI):
  ```bash
  ./build-win/Akerbeltz-1.1.0.exe
  ```
- Tests are OFF by default; enable them in a separate build directory if needed:
  ```bash
  cmake -S . -B build-win-tests -G Ninja -DCMAKE_BUILD_TYPE=Release -DAKERBELTZ_BUILD_TESTS=ON
  cmake --build build-win-tests -j
  ctest --test-dir build-win-tests --output-on-failure
  ```
- Static linking here is meant to avoid MinGW runtime DLL dependencies such as `libstdc++-6.dll`, `libgcc_s_seh-1.dll`, and `libwinpthread-1.dll`.

## Using the engine

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
  ./build/Akerbeltz-1.1.0
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

## Scripts

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
  python3 scripts/perft_suite.py --engine ./build/Akerbeltz-1.1.0 \
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
