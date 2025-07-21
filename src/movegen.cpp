#include "movegen_.h"
#include "position.h"
#include "attacks.h"

namespace Xake{

namespace MoveGen{

template<Direction D, SpecialMove SM>
void extract_capture_moves(const Position &pos, Bitboard toBitboard, MoveList &moveList);

void white_pawn_capture_moves(const Position &pos, MoveList &moveList);

void generate_all_moves(const Position &pos, MoveList &moveList){

}

void generate_capture_moves(Position &pos, MoveList &moveList){
    white_pawn_capture_moves(pos, moveList);
}

template<Direction D, SpecialMove SM>
void extract_capture_moves(const Position &pos, Bitboard toBitboard, MoveList &moveList){

    while (toBitboard) {
        Square64 moveTo{__builtin_ctzll(toBitboard)};
        Square64 moveFrom{moveTo - D};
        moveList.set_move(make_capture_move(moveFrom, moveTo, SM, pos.get_mailbox_piece(moveFrom), pos.get_mailbox_piece(moveTo)));
        toBitboard &= toBitboard - 1;
    }

}

void white_pawn_capture_moves(const Position &pos, MoveList &moveList){

    Bitboard northEastMoves = (pos.get_pieceTypes_bitboard(WHITE, PAWN) << NORTH_EAST) & ~Bitboards::FILE_A_MASK;
    Bitboard northEastCaptures = northEastMoves & pos.get_occupied_bitboard(BLACK) & ~Bitboards::RANK_8_MASK;
    extract_capture_moves<NORTH_EAST, NO_SPECIAL>(pos, northEastCaptures, moveList);
    Bitboard northWestMoves = (pos.get_pieceTypes_bitboard(WHITE, PAWN) << NORTH_WEST) & ~Bitboards::FILE_H_MASK;
    Bitboard northWestCaptures = northWestMoves & pos.get_occupied_bitboard(BLACK) & ~Bitboards::RANK_8_MASK;
    extract_capture_moves<NORTH_WEST, NO_SPECIAL>(pos, northWestCaptures, moveList);
    Bitboard northEastCapturesPromotions = northEastMoves & pos.get_occupied_bitboard(BLACK) & Bitboards::RANK_8_MASK;
    extract_capture_moves<NORTH_WEST, PROMOTION_BISHOP>(pos, northEastCapturesPromotions, moveList);
    extract_capture_moves<NORTH_WEST, PROMOTION_KNIGHT>(pos, northEastCapturesPromotions, moveList);
    extract_capture_moves<NORTH_WEST, PROMOTION_QUEEN> (pos, northEastCapturesPromotions, moveList);
    extract_capture_moves<NORTH_WEST, PROMOTION_ROOK>  (pos, northEastCapturesPromotions, moveList);
    Bitboard northWestCapturesPromotions = northWestMoves & pos.get_occupied_bitboard(BLACK) & Bitboards::RANK_8_MASK;
    extract_capture_moves<NORTH_WEST, PROMOTION_BISHOP>(pos, northWestCapturesPromotions, moveList);
    extract_capture_moves<NORTH_WEST, PROMOTION_KNIGHT>(pos, northWestCapturesPromotions, moveList);
    extract_capture_moves<NORTH_WEST, PROMOTION_QUEEN> (pos, northWestCapturesPromotions, moveList);
    extract_capture_moves<NORTH_WEST, PROMOTION_ROOK>  (pos, northWestCapturesPromotions, moveList);

    //Enpassant
    if(pos.get_enpassant_square() != SQ64_NO_SQUARE){
        Square64 enpassantSquare = pos.get_enpassant_square();
        Bitboard enpassantMask = Bitboards::set_pieces(enpassantSquare);
        if(northEastMoves & enpassantMask){
            pos.get_enpassant_square();
            moveList.set_move(make_enpassant_move(enpassantSquare-NORTH_EAST, enpassantSquare));
        }
        else if(northWestMoves & enpassantMask){
            moveList.set_move(make_enpassant_move(enpassantSquare-NORTH_WEST, enpassantSquare));
        }
    }

}

}
} 

