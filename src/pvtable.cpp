#include "pvtable.h"
#include "position.h"

namespace Xake {

namespace PVTable {

    Entry table[PVTableEntries];

    void init() {
        for (std::size_t i = 0; i < PVTableEntries; ++i) {
            table[i].key  = 0;
            table[i].move = NOMOVE;
        }
    }

    void insert_entry(const Position& position, Move move) {
        std::size_t index = position.get_key() % PVTableEntries;
        table[index].key  = position.get_key();
        table[index].move = move;
    }

    Move probe_move(const Position& position) {
        std::size_t index = position.get_key() % PVTableEntries;
        if (table[index].key == position.get_key())
            return table[index].move;
        return NOMOVE; 
    }

    void load_pv_line(PVLine& pvLine, DepthSize depth, Position& position) {
        pvLine.depth = 0;
        Move move = probe_move(position);

        while (pvLine.depth < depth && move != NOMOVE) {
            
            position.do_move(move);
            pvLine.moves[pvLine.depth++] = move;
            move = probe_move(position);

        }

        DepthSize counter = pvLine.depth;

        while (counter > 0) {
            position.undo_move();
            --counter;
        }
    }
}

}
