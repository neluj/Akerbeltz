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

const MoveScore MVVLVAScores[PIECETYPE_SIZE][PIECETYPE_SIZE] = {
    {0,  0,  0,  0,  0,  0, 0},
    {0, 15, 25, 35, 45, 55, 0},
    {0, 14, 24, 34, 44, 54, 0},
    {0, 13, 23, 33, 43, 53, 0},
    {0, 12, 22, 32, 42, 52, 0},
    {0, 11, 21, 31, 41, 51, 0},
    {0, 10, 20, 30, 40, 50, 0} 
};

inline Move make_quiet_move(Square120 from, Square120 to, SpecialMove SpecialMove) {
    return Move((SpecialMove << 14) | (to << 7) | from);
}

inline Move make_capture_move(Square120 from, Square120 to, SpecialMove SpecialMove, Piece attackerPiece , Piece capturedPiece) {

    PieceType attackerType = piece_type(attackerPiece);
    PieceType capturedType = piece_type(capturedPiece);

    MoveScore mScore = MVVLVASCORE + MVVLVAScores[attackerType][capturedType];

    return Move((mScore << 23) | (capturedPiece << 19) | (SpecialMove << 14) | (to << 7) | from);
}

inline Move make_enpassant_move(Square120 from, Square120 to) {
    return Move((ENPASSANT << 14) | (to << 7) | from);
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

inline MoveScore move_score(Move move){
    return move >> 23;
}

inline Move set_score(Move move, MoveScore moveScore){
    return move | (moveScore << 23);
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



