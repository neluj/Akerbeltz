#ifndef INCLUDE_MOVEGENERATOR_H
#define INCLUDE_MOVEGENERATOR_H

#include <vector>
#include "move.h"

namespace Xake{

class Position;

namespace MoveGen{

struct MoveList{
    
    void set_move(Move move){
        moves[size] = move;
        ++size;
    }
    Move moves[MAX_POSITION_MOVES_SIZE];
    int size{0};
};


void generate_all_moves(const Position &pos, MoveList &moveList);
void generate_capture_moves(Position &pos, MoveList &moveList);


} // namespace Xake
}
#endif // #ifndef INCLUDE_MOVEGENERATOR_H