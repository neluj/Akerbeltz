// TODO https://www.chessprogramming.org/Simplified_Evaluation_Function
#ifndef INCLUDE_EVALUATE_H
#define INCLUDE_EVALUATE_H

#include "types.h"

namespace Xake{

namespace Evaluate{

    using Score = int;

    Score evaluate_material(PieceType pieceType);

}
}

#endif