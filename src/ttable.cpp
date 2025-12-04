#include "ttable.h"
#include <cstring>
#include <algorithm>
#include <limits>

namespace Xake {

namespace TT {

    Entry table[TTEntries];

    void clear() {
        std::memset(table, 0, sizeof(table));
    }

    bool probe(Key key, Entry &outEntry) {
        std::size_t index = key % TTEntries;
        Entry &e = table[index];

        if (e.key == 0 || e.key != key)
            return false;

        outEntry = e;
        return true;
    }

    void store(Key key, DepthSize depth, Score score, Flag flag, Move bestMove) {
        std::size_t index = key % TTEntries;
        Entry &e = table[index];

        // Simple replacement policy: keep the entry with greater depth
        if (e.key != 0 && e.key != key && e.depth > depth)
            return;

        e.key   = key;
        e.depth = depth;
        e.flag  = flag;
        e.move  = raw_move(bestMove);
        e.score = score;
    }

} // namespace TT

} // namespace Xake
