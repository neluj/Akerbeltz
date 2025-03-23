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
    std::size_t size{0};
};


void generate_all_moves(const Position &position, MoveList &moveList);
void generate_capture_moves(Position &position, MoveList &moveList);
// TODO void generate_quiet_moves(Position &position);


} // namespace Xake
}
#endif // #ifndef INCLUDE_MOVEGENERATOR_H