#include "bitboards.h"

namespace Xake{

void Bitboards::set_piece(Bitboard &bitboard, Square64 square64){
    bitboard |= (ONE << square64);
}

void Bitboards::clear_piece(Bitboard &bitboard, Square64 square64){
    bitboard ^= (ONE << square64);
}

void Bitboards::move_piece(Bitboard &bitboard, Square64 from, Square64 to){
    bitboard ^= (ONE << to) | (ONE << from);
}

}