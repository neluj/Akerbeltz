#ifndef INCLUDE_TTABLE_H
#define INCLUDE_TTABLE_H

#include "types.h"
#include "move.h"
#include "evaluate.h"

#include <cstddef>
#include <cstdint>

namespace Akerbeltz {

class Position;

namespace TT {

    using Evaluate::Score;

    constexpr std::size_t DEFAULT_TT_MB = 128;
    constexpr std::size_t MIN_TT_MB     = 4;
    constexpr std::size_t MAX_TT_MB     = 4096;

    enum Flag : uint8_t {
        FLAG_NONE,
        FLAG_EXACT,
        FLAG_LOWERBOUND,
        FLAG_UPPERBOUND
    };

    struct Entry {
        Key        key;    // Zobrist key
        Score      score;  // Position Score 
        DepthSize  depth;  // Search depth
        Flag       flag;   // TT::Flag
        Move       move;   // Best move from position
    };

    struct PVLine {
        Move moves[MAX_DEPTH];
        DepthSize depth;
    };

    void resize(std::size_t sizeMB);

    std::size_t current_size_mb();

    void clear();

    // Returns true if key exist
    bool probe(Key key, Entry &outEntry);

    void store(Key key, DepthSize depth, Score score, Flag flag, Move bestMove);

    void load_pv_line(Position& pos, PVLine& line, DepthSize depth = MAX_DEPTH);

} // namespace TT

} // namespace Akerbeltz

#endif
