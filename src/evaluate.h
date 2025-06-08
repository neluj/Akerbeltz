// TODO https://www.chessprogramming.org/Simplified_Evaluation_Function
#ifndef INCLUDE_EVALUATE_H
#define INCLUDE_EVALUATE_H

#include "types.h"


namespace Xake{

class Position;

namespace Evaluate{

    using Score = int;

    void init();
    
    Score calc_material_table(Piece piece, Square64 square);

    Score calc_score(const Position &position);
}
}

#endif