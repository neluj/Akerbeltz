#include "ttable.h"
#include "position.h"
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

        if (e.depth > depth && (e.key == key || e.key != 0)) return;

        e.key   = key;
        e.depth = depth;
        e.flag  = flag;
        e.move  = raw_move(bestMove);
        e.score = score;
    }

    void load_pv_line(Position& pos, PVLine& pvLine, DepthSize depth) {
  
        pvLine.depth = 0;

        Entry entry;
        while (pvLine.depth < depth && TT::probe(pos.get_key(), entry )) {
            Move m = entry.move;
            if (m == NOMOVE) break;

            if (!pos.do_move(m)) break;
            pvLine.moves[pvLine.depth++] = raw_move(m);
        }

        DepthSize counter = pvLine.depth;
        while (counter > 0){ pos.undo_move(); --counter;}
    }
  
}// namespace TT

} // namespace Xake
