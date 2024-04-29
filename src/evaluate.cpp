#include "evaluate.h"
#include <unordered_map>

namespace Xake{

namespace Evaluate{

std::unordered_map<PieceType, Score> material_evaluation = {
    {NO_PIECE_TYPE, 0},
    {PAWN, 100},
    {KNIGHT, 320},
    {BISHOP, 330},
    {ROOK, 500},
    {QUEEN, 900},
    {KING, 50000}
};

Score evaluate_material(PieceType pieceType){
    return material_evaluation.at(pieceType);
}


}
}