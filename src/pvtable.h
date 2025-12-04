#ifndef INCLUDE_PVTABLE_H
#define INCLUDE_PVTABLE_H

#include "types.h"
#include "move.h"

namespace Xake {

namespace PVTable {

    struct PVLine {
        Move moves[MAX_DEPTH];
        DepthSize depth;
    };

    extern Move      pvTable[MAX_DEPTH][MAX_DEPTH];
    extern DepthSize pvLength[MAX_DEPTH];

    void clear();

    void clear_ply(DepthSize ply);

    void update_line(DepthSize ply, Move bestMove);

    void get_pv_line(PVLine &pvLine);

} // namespace PVTable

} // namespace Xake

#endif
