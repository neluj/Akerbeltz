// TODO https://www.chessprogramming.org/Simplified_Evaluation_Function
#ifndef INCLUDE_EVALUATE_H
#define INCLUDE_EVALUATE_H

#include "types.h"


namespace Xake{

class Position;

namespace Evaluate{

    using Score = int;

    void init();
    //Score evaluate_material(PieceType pieceType);
    
    Score calc_material_table(Color color, PieceType pieceType, Square120 sq);

    Score calc_score(const Position &position);
}
}

#endif