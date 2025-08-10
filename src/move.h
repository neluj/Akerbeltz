#ifndef INCLUDE_MOVE_H
#define INCLUDE_MOVE_H

#include "types.h"

namespace Xake{
// VIDEO
/*Move:
0000 0000 0000 0000 0000 0000 0000 0011 1111 -> From 0x7F
0000 0000 0000 0000 0000 0000 1111 1100 0000 -> To 0x7F
0000 0000 0000 0000 0000 0011 0000 0000 0000 -> Special move flag
0000 0000 0000 0000 0001 1100 0000 0000 0000 -> Promotion Piece
0000 0000 0000 0001 1110 0000 0000 0000 0000 -> Captured piece
1111 1111 1111 1110 0000 0000 0000 0000 0000 -> Score
*/

using Move = unsigned long int;
using MoveScore = unsigned short int;

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

constexpr MoveScore MVVLVASCORE = 900;

constexpr MoveScore MVVLVAScores[PIECETYPE_SIZE][PIECETYPE_SIZE] = {
    {0,  0,  0,  0,  0,  0, 0},
    {0, 15, 25, 35, 45, 55, 0},
    {0, 14, 24, 34, 44, 54, 0},
    {0, 13, 23, 33, 43, 53, 0},
    {0, 12, 22, 32, 42, 52, 0},
    {0, 11, 21, 31, 41, 51, 0},
    {0, 10, 20, 30, 40, 50, 0} 
};

inline Move make_quiet_move(Square64 from, Square64 to, SpecialMove SpecialMove) {
    return Move((SpecialMove << 12) | (to << 6) | from);
}

inline Move make_capture_move(Square64 from, Square64 to, SpecialMove SpecialMove, Piece attackerPiece , Piece capturedPiece) {
    
    PieceType attackerType = piece_type(attackerPiece);
    PieceType capturedType = piece_type(capturedPiece);
    MoveScore mScore = MVVLVASCORE + MVVLVAScores[attackerType][capturedType];

    return Move((mScore << 21) | (capturedPiece << 17) | (SpecialMove << 12) | (to << 6) | from);
}

inline Move make_enpassant_move(Square64 from, Square64 to) {
    return Move(((MVVLVASCORE + MVVLVAScores[PAWN][PAWN]) << 21) | (ENPASSANT << 12) | (to << 6) | from);
}

inline Square64 move_from(Move move) {
	return Square64(move & 0x3f);
}

inline Square64 move_to(Move move) {
	return Square64((move >> 6) & 0x3f);
}

inline SpecialMove move_special(Move move) {
    return SpecialMove((move >> 12) & 0x1F);
}

inline PieceType promoted_piece(Move move){
    return PieceType((move >> 14) & 0x07);
} 

inline Piece captured_piece(Move move){
    return Piece((move >> 17) & 0x0f);
} 

inline MoveScore move_score(Move move){
    return move >> 21;
}

inline Move set_score(Move move, MoveScore moveScore){
    return move | (moveScore << 21);
}

inline std::string algebraic_move(Move move) {
    std::string algebraic_move;
    Square64 from   = move_from(move);
    Square64 to     = move_to(move);
    algebraic_move = SQUARE_NAMES[from] + SQUARE_NAMES[to];

    PieceType promoted = promoted_piece(move);

    switch (promoted)
    {
        case KNIGHT:    algebraic_move += 'n'; break;
        case BISHOP:    algebraic_move += 'b'; break;
        case ROOK:      algebraic_move += 'r'; break;
        case QUEEN:     algebraic_move += 'q'; break;
        default:                               break;
    }

    return algebraic_move;
}


} // namespace Xake

#endif // #ifndef INCLUDE_MOVE_H



