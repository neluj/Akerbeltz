#ifndef INCLUDE_MOVE_H
#define INCLUDE_MOVE_H

#include "types.h"

namespace Xake{

/*Move:
0000 0000 0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0000 0000 0011 1111 1000 0000 -> To 0x7F
0000 0000 0000 0000 0000 1100 0000 0000 0000 -> Special move flag
0000 0000 0000 0000 0111 0000 0000 0000 0000 -> Promotion Piece
0000 0000 0000 0111 1000 0000 0000 0000 0000 -> Captured piece type
1111 1111 1111 1000 0000 0000 0000 0000 0000 -> Score
*/

typedef unsigned long int Move;

/* 
    NORMAL: Quiet and Capture moves
*/
// VIDEO
enum SpecialMove: int{
    NO_SPECIAL          = 0,
    ENPASSANT           = 1,
    PAWN_START          = 2,
    CASTLE              = 3,
    PROMOTION_KNIGHT    = KNIGHT    << 2, 
    PROMOTION_BISHOP    = BISHOP    << 2, 
    PROMOTION_ROOK      = ROOK      << 2, 
    PROMOTION_QUEEN     = QUEEN     << 2
};

const std::string SQUARE_NAMES[Xake::SQUARE_SIZE_120] = {
    "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD","OFFBOARD", "OFFBOARD", "OFFBOARD",
    "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD","OFFBOARD", "OFFBOARD", "OFFBOARD",
    "OFFBOARD", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "OFFBOARD",
    "OFFBOARD", "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "OFFBOARD",
    "OFFBOARD", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "OFFBOARD",
    "OFFBOARD", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "OFFBOARD",
    "OFFBOARD", "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "OFFBOARD",
    "OFFBOARD", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "OFFBOARD",
    "OFFBOARD", "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "OFFBOARD",
    "OFFBOARD", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "OFFBOARD",
    "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD","OFFBOARD", "OFFBOARD", "OFFBOARD",
    "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD","OFFBOARD", "OFFBOARD", "OFFBOARD"

};


//IMPROVE inline or constxpr? same for other functionsfff
inline Move make_move(Square120 from, Square120 to, SpecialMove SpecialMove, Piece capturedPiece) {
    return Move((capturedPiece << 19) | (SpecialMove << 14) + (to << 7) + from);
}

inline Square120 move_from(Move move) {
	return Square120(move & 0x7f);
}

inline Xake::Square120 move_to(Move move) {
	return Square120((move >> 7) & 0x7f);
}

inline SpecialMove move_special(Move move) {
    return SpecialMove((move >> 14) & 0x1F);
}

inline PieceType promoted_piece(Move move){
    return PieceType((move >> 16) & 0x07);
} 

inline Piece captured_piece(Move move){
    return Piece((move >> 19) & 0x0f);
} 

inline std::string algebraic_move(Move move) {
    std::string algebraic_move;
    Xake::Square120 from   = move_from(move);
    Xake::Square120 to     = move_to(move);
    algebraic_move = SQUARE_NAMES[from] + SQUARE_NAMES[to];

    PieceType promoted = promoted_piece(move);

    switch (promoted)
    {
        case KNIGHT:    algebraic_move += 'n'; break;
        case BISHOP:    algebraic_move += 'b'; break;
        case ROOK:      algebraic_move += 'r'; break;
        case QUEEN:     algebraic_move += 'q'; break;
    }

    return algebraic_move;
}


} // namespace Xake

#endif // #ifndef INCLUDE_MOVE_H



