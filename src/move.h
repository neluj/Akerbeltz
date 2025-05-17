#ifndef INCLUDE_MOVE_H
#define INCLUDE_MOVE_H

#include "types.h"

namespace Xake{

/*Move:
0000 0000 0000 0000 0000 0000 0000 0111 1111 -> From 0x7F
0000 0000 0000 0000 0000 0111 1111 1000 0000 -> To 0x7F
0000 0000 0000 0000 0000 1000 0000 0000 0000 -> Special move flag
0000 0000 0000 0000 0111 0000 0000 0000 0000 -> Promotion Piece
0000 0000 0000 0111 1000 0000 0000 0000 0000 -> Captured piece
1111 1111 1111 1000 0000 0000 0000 0000 0000 -> Score
*/

typedef unsigned long int Move;

enum MoveType{
    QUIET,
    CAPTURE
};

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

inline Move make_quiet_move(Square120 from, Square120 to, SpecialMove SpecialMove) {
    return Move((SpecialMove << 14) + (to << 7) + from);
}

inline Move make_capture_move(Square120 from, Square120 to, SpecialMove SpecialMove, Piece attackerPiece , Piece capturedPiece) {
    return Move((capturedPiece << 19) | (SpecialMove << 14) + (to << 7) + from);
}

inline Move make_enpassant_move(Square120 from, Square120 to) {
    return Move((ENPASSANT << 14) + (to << 7) + from);
}

inline Square120 move_from(Move move) {
	return Square120(move & 0x7f);
}

inline Square120 move_to(Move move) {
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
    Square120 from   = move_from(move);
    Square120 to     = move_to(move);
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



