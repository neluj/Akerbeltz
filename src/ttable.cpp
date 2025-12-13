#include "ttable.h"
#include "position.h"

#include <algorithm>
#include <vector>

namespace Xake {

namespace TT {

    std::size_t clamp_mb(std::size_t sizeMB);
    std::size_t entry_count(std::size_t sizeMB);

    std::size_t ttSizeMB = TT::DEFAULT_TT_MB;
    std::vector<Entry> table(entry_count(ttSizeMB));

    constexpr std::size_t mb_to_bytes(std::size_t mb) { return mb * 1024ULL * 1024ULL; }

    std::size_t clamp_mb(std::size_t sizeMB) {
        sizeMB = std::max(TT::MIN_TT_MB, sizeMB);
        sizeMB = std::min(TT::MAX_TT_MB, sizeMB);
        return sizeMB;
    }

    std::size_t entry_count(std::size_t sizeMB) {
        const auto bytes = mb_to_bytes(sizeMB);
        return std::max<std::size_t>(1, bytes / sizeof(Entry));
    }

    void resize(std::size_t sizeMB) {
        ttSizeMB = clamp_mb(sizeMB);
        table.assign(entry_count(ttSizeMB), Entry{});
    }

    std::size_t current_size_mb() { return ttSizeMB; }

    void clear() {
        std::fill(table.begin(), table.end(), Entry{});
    }

    bool probe(Key key, Entry &outEntry) {
        if (table.empty()) return false;

        std::size_t index = key % table.size();
        Entry &e = table[index];

        if (e.key == 0 || e.key != key)
            return false;

        outEntry = e;
        return true;
    }

    void store(Key key, DepthSize depth, Score score, Flag flag, Move bestMove) {
        if (table.empty()) return;

        std::size_t index = key % table.size();
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
