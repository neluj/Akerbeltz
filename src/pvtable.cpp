#include "pvtable.h"

namespace Xake {

namespace PVTable {

    Move      pvTable[MAX_DEPTH][MAX_DEPTH];
    DepthSize pvLength[MAX_DEPTH];

    void clear() {
        for (DepthSize i = 0; i < MAX_DEPTH; ++i) {
            pvLength[i] = 0;
        }
    }

    void clear_ply(DepthSize ply) {
        if (ply < MAX_DEPTH) {
            pvLength[ply] = 0;
        }
    }

    void update_line(DepthSize ply, Move bestMove) {
        if (ply >= MAX_DEPTH) return;

        pvTable[ply][0] = raw_move(bestMove);

        DepthSize childPly = ply + 1;
        DepthSize childLen = (childPly < MAX_DEPTH) ? pvLength[childPly] : 0;

        for (DepthSize i = 0; i < childLen; ++i) {
            pvTable[ply][i + 1] = pvTable[childPly][i];
        }

        pvLength[ply] = childLen + 1;
    }

    void get_pv_line(PVLine &pvLine) {
        pvLine.depth = pvLength[0];
        for (DepthSize i = 0; i < pvLine.depth; ++i) {
            pvLine.moves[i] = pvTable[0][i];
        }
    }

} // namespace PVTable

} // namespace Xake
