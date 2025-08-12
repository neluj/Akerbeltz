#ifndef INCLUDE_PVTABLE_H
#define INCLUDE_PVTABLE_H

#include "types.h"
#include "move.h"

namespace Xake {

class Position;

namespace PVTable {

    struct PVLine {
        Move moves[MAX_DEPTH];
        DepthSize depth;
    };

    struct Entry {
        Key  key;
        Move move;
    };

    constexpr std::size_t PVTableSize   = 0x100000 * 128; 
    constexpr std::size_t PVTableEntries = PVTableSize / sizeof(Entry);

    extern Entry table[PVTableEntries];

    void init();
    void insert_entry(const Position& position, Move move);
    Move probe_move(const Position& position);
    void load_pv_line(PVLine& pvLine, DepthSize depth, Position& position);

}

}

#endif
