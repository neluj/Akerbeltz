#ifndef INCLUDE_TTABLE_H
#define INCLUDE_TTABLE_H

#include "types.h"
#include "move.h"
#include "evaluate.h"

#include <cstdint>

namespace Xake {

class Position;

namespace TT {

    using Evaluate::Score;

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

    constexpr std::size_t TTSizeMB      = 128; // 128 MB
    constexpr std::size_t TTBytes       = TTSizeMB * 1024ULL * 1024ULL;
    constexpr std::size_t TTEntries     = TTBytes / sizeof(Entry);

    extern Entry table[TTEntries];

    struct PVLine {
        Move moves[MAX_DEPTH];
        DepthSize depth;
    };

    void clear();

    // Returns true if key exist
    bool probe(Key key, Entry &outEntry);

    void store(Key key, DepthSize depth, Score score, Flag flag, Move bestMove);

    void load_pv_line(Position& pos, PVLine& line, DepthSize depth = MAX_DEPTH);

} // namespace TT

} // namespace Xake

#endif
