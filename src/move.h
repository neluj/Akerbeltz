#ifndef INCLUDE_MOVE_H
#define INCLUDE_MOVE_H

#include "types.h"

namespace Xake{


// VIDEO
/*Move:
.... 0000 0000 0000 0000 0000 0000 0000 0011 1111 -> From
.... 0000 0000 0000 0000 0000 0000 1111 1100 0000 -> To
.... 0000 0000 0000 0000 0000 0011 0000 0000 0000 -> Special move flag
.... 0000 0000 0000 0000 0001 1100 0000 0000 0000 -> Promotion Piece
.... 0000 0000 0000 0001 1110 0000 0000 0000 0000 -> Captured piece
.... 1111 1111 1111 1110 0000 0000 0000 0000 0000 -> Score
*/

/*Move Scores:
1000 0000 0000 0000 ... -> PVMove  0x4000
0111 1100 0000 0000 ... -> MVVLVA  0x3F80
0000 0010 0000 0000 ... -> Killer1 0x40
0000 0001 0000 0000 ... -> Killer2 0x20
.... 1111 1111 1111 111 -> History Heuristic
*/

using Move = uint64_t;
constexpr Move NOMOVE = 0;

using MoveScore = uint64_t;
constexpr MoveScore MS_ONE = 1;


constexpr int MAX_KILLERMOVES = 2;

constexpr MoveScore PV_SCORE = (MS_ONE << 42);
constexpr MoveScore KILLERMOVE_SOCORE_0 = (MS_ONE << 36);
constexpr MoveScore KILLERMOVE_SOCORE_1 = (MS_ONE << 35);

constexpr MoveScore MVVLVAScores[PIECETYPE_SIZE][PIECETYPE_SIZE] = {
    {0,  0,  0,  0,  0,  0, 0},
    {0, (MoveScore{6} << 37), (MoveScore{12} << 37), (MoveScore{18} << 37), (MoveScore{24} << 37), (MoveScore{30} << 37), 0},
    {0, (MoveScore{5} << 37), (MoveScore{11} << 37), (MoveScore{17} << 37), (MoveScore{23} << 37), (MoveScore{29} << 37), 0},
    {0, (MoveScore{4} << 37), (MoveScore{10} << 37), (MoveScore{16} << 37), (MoveScore{22} << 37), (MoveScore{28} << 37), 0},
    {0, (MoveScore{3} << 37), (MoveScore{9} <<  37), (MoveScore{15} << 37), (MoveScore{21} << 37), (MoveScore{27} << 37), 0},
    {0, (MoveScore{2} << 37), (MoveScore{8} <<  37), (MoveScore{14} << 37), (MoveScore{20} << 37), (MoveScore{26} << 37), 0},
    {0, (MoveScore{1} << 37), (MoveScore{7} <<  37), (MoveScore{13} << 37), (MoveScore{19} << 37), (MoveScore{25} << 37), 0} 
};

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

inline Move make_quiet_move(Square64 from, Square64 to, SpecialMove SpecialMove) {
    return Move((SpecialMove << 12) | (to << 6) | from);
}

inline Move make_capture_move(Square64 from, Square64 to, SpecialMove SpecialMove, Piece attackerPiece , Piece capturedPiece) {
    
    PieceType attackerType = piece_type(attackerPiece);
    PieceType capturedType = piece_type(capturedPiece);

    return Move((MVVLVAScores[attackerType][capturedType] << 21) | (capturedPiece << 17) | (SpecialMove << 12) | (to << 6) | from);
}

inline Move make_enpassant_move(Square64 from, Square64 to) {
    return Move(((MVVLVAScores[PAWN][PAWN]) << 21) | (ENPASSANT << 12) | (to << 6) | from);
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

inline Move set_heuristic_score(Move move, MoveScore moveScore){
    return move | (moveScore << 21);
}

inline Move raw_move(Move move){
    return move & 0x1FFFFFULL;
}

inline bool is_capture(Move m) {
    return captured_piece(m) != NO_PIECE || move_special(m) == ENPASSANT;
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



