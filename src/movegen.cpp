#include "movegen_.h"
#include "position.h"
#include "attacks.h"

namespace Xake{

namespace MoveGen{

//DECLARATIONS
//PAWNS
void white_pawn_moves(const Position &pos, MoveList &moveList);
void white_pawn_capture_moves(const Position &pos, MoveList &moveList);
void white_pawn_quiet_moves(const Position &pos, MoveList &moveList);

void black_pawn_moves(const Position &pos, MoveList &moveList);
void black_pawn_capture_moves(const Position &pos, MoveList &moveList);

template<Direction D, SpecialMove SM>
void extract_quiet_moves(Bitboard toBitboard, MoveList &moveList);

template<Direction D, SpecialMove SM>
void extract_capture_moves(const Position &pos, Bitboard toBitboard, MoveList &moveList);

void generate_all_moves(const Position &pos, MoveList &moveList){
    if(pos.get_side_to_move() == WHITE){
        white_pawn_moves(pos, moveList);
    }
    else{
        black_pawn_moves(pos, moveList);
    }  
}

void generate_capture_moves(Position &pos, MoveList &moveList){
    if(pos.get_side_to_move() == WHITE){
        white_pawn_capture_moves(pos, moveList);
    }
    else{
        black_pawn_capture_moves(pos, moveList);
    }   
}

void white_pawn_moves(const Position &pos, MoveList &moveList){
    white_pawn_capture_moves(pos, moveList);
    white_pawn_quiet_moves(pos, moveList);
}

void black_pawn_moves(const Position &pos, MoveList &moveList){
    black_pawn_capture_moves(pos, moveList);
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

void white_pawn_quiet_moves(const Position &pos, MoveList &moveList){

    Bitboard quietSimpleMoves = (pos.get_pieceTypes_bitboard(WHITE, PAWN) << NORTH) & ~pos.get_occupied_bitboard(COLOR_NC);
    Bitboard notPromotionQuietMoves = quietSimpleMoves & ~Bitboards::RANK_8_MASK;
    extract_quiet_moves<NORTH, SpecialMove::NO_SPECIAL>(notPromotionQuietMoves, moveList);
    Bitboard startMoves = ((quietSimpleMoves & Bitboards::RANK_3_MASK) << NORTH) & ~pos.get_occupied_bitboard(COLOR_NC);
    extract_quiet_moves<NORTH_NORTH, SpecialMove::PAWN_START>(startMoves, moveList);

    Bitboard promotionQuietMoves = quietSimpleMoves & Bitboards::RANK_8_MASK;
    extract_quiet_moves<NORTH, PROMOTION_BISHOP>(promotionQuietMoves, moveList);
    extract_quiet_moves<NORTH, PROMOTION_KNIGHT>(promotionQuietMoves, moveList);
    extract_quiet_moves<NORTH, PROMOTION_QUEEN> (promotionQuietMoves, moveList);
    extract_quiet_moves<NORTH, PROMOTION_ROOK>  (promotionQuietMoves, moveList);

}


void black_pawn_capture_moves(const Position &pos, MoveList &moveList){

    Bitboard southEastMoves = (pos.get_pieceTypes_bitboard(BLACK, PAWN) >> 9) & ~Bitboards::FILE_A_MASK;
    Bitboard southEastCaptures = southEastMoves & pos.get_occupied_bitboard(WHITE) & ~Bitboards::RANK_1_MASK;
    extract_capture_moves<SOUTH_EAST, NO_SPECIAL>(pos, southEastCaptures, moveList);
    Bitboard southWestMoves = (pos.get_pieceTypes_bitboard(BLACK, PAWN) >> 7) & ~Bitboards::FILE_H_MASK;
    Bitboard southWestCaptures = southWestMoves & pos.get_occupied_bitboard(WHITE) & ~Bitboards::RANK_1_MASK;
    extract_capture_moves<SOUTH_WEST, NO_SPECIAL>(pos, southWestCaptures, moveList);
    Bitboard southEastCapturesPromotions = southEastMoves & pos.get_occupied_bitboard(WHITE) & Bitboards::RANK_1_MASK;
    extract_capture_moves<SOUTH_WEST, PROMOTION_BISHOP>(pos, southEastCapturesPromotions, moveList);
    extract_capture_moves<SOUTH_WEST, PROMOTION_KNIGHT>(pos, southEastCapturesPromotions, moveList);
    extract_capture_moves<SOUTH_WEST, PROMOTION_QUEEN> (pos, southEastCapturesPromotions, moveList);
    extract_capture_moves<SOUTH_WEST, PROMOTION_ROOK>  (pos, southEastCapturesPromotions, moveList);
    Bitboard southWestCapturesPromotions = southWestMoves & pos.get_occupied_bitboard(WHITE) & Bitboards::RANK_1_MASK;
    extract_capture_moves<SOUTH_WEST, PROMOTION_BISHOP>(pos, southWestCapturesPromotions, moveList);
    extract_capture_moves<SOUTH_WEST, PROMOTION_KNIGHT>(pos, southWestCapturesPromotions, moveList);
    extract_capture_moves<SOUTH_WEST, PROMOTION_QUEEN> (pos, southWestCapturesPromotions, moveList);
    extract_capture_moves<SOUTH_WEST, PROMOTION_ROOK>  (pos, southWestCapturesPromotions, moveList);

    //Enpassant
    if(pos.get_enpassant_square() != SQ64_NO_SQUARE){
        Square64 enpassantSquare = pos.get_enpassant_square();
        Bitboard enpassantMask = Bitboards::set_pieces(enpassantSquare);
        if(southEastMoves & enpassantMask){
            pos.get_enpassant_square();
            moveList.set_move(make_enpassant_move(enpassantSquare-SOUTH_EAST, enpassantSquare));
        }
        else if(southWestMoves & enpassantMask){
            moveList.set_move(make_enpassant_move(enpassantSquare-SOUTH_WEST, enpassantSquare));
        }
    }

}

template<Direction D, SpecialMove SM>
void extract_quiet_moves(Bitboard toBitboard, MoveList &moveList){

    while (toBitboard) {
        Square64 moveTo{__builtin_ctzll(toBitboard)};
        Square64 moveFrom{moveTo - D};
        moveList.set_move(make_quiet_move(moveFrom, moveTo, SM));
        toBitboard &= toBitboard - 1;
    }

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

}
} 

