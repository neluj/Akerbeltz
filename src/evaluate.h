#ifndef INCLUDE_EVALUATE_H
#define INCLUDE_EVALUATE_H

#include "types.h"


namespace Xake{

class Position;

namespace Evaluate{

    using Score = int32_t;
    
    constexpr Score CHECKMATE_SCORE = 900000;
    constexpr Score DRAW_SOCORE = 0;

    using GamePhaseWeight = int;

    inline constexpr GamePhaseWeight MAX_PHASE_PIECE_WEIGHT = 24;
    inline constexpr GamePhaseWeight ENDGAME_PHASE_THRESHOLD = 6;
    inline constexpr GamePhaseWeight PHASE_PIECE_WEIGHT[Piece::PIECE_SIZE] = {
        /*NO*/ 0,
        /*W_P*/ 0, /*W_N*/1, /*W_B*/1, /*W_R*/2, /*W_Q*/4, /*W_K*/0,
        /*gap*/ 0, 0,
        /*B_P*/ 0, /*B_N*/1, /*B_B*/1, /*B_R*/2, /*B_Q*/4, /*B_K*/0,
        /*gap*/ 0
    };

    void init();

    Score calc_score(const Position &position);

    bool material_draw(const Position& pos);
}
}

#endif