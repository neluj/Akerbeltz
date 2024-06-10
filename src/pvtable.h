#ifndef INCLUDE_PVTABLE_H
#define INCLUDE_PVTABLE_H

#include <unordered_map>

#include "types.h"
#include "move.h"

namespace Xake{

class Position;

namespace PVTable{

    struct PVLine{
        Move moves[MAX_DEPTH];
        DepthSize depth;
    };

    void init();

    void insert_entry(const Position &position, Move move);

    Move prove_move(const Position &position);

    void load_pv_line(PVLine &pvLine, DepthSize depth, Position &position);

}
}



#endif