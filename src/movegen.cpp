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
void black_pawn_quiet_moves(const Position &pos, MoveList &moveList);

template<Color C, PieceType PT, MoveType MT>
void no_special_moves(const Position &pos, MoveList &moveList);

template<Direction D, SpecialMove SM>
void extract_quiet_moves(Bitboard toBitboard, MoveList &moveList);

template<SpecialMove SM>
void extract_quiet_moves(Square64 from, Bitboard toBitboard, MoveList &moveList);

template<Direction D, SpecialMove SM>
void extract_capture_moves(const Position &pos, Bitboard toBitboard, MoveList &moveList);

template<SpecialMove SM>
void extract_capture_moves(const Position &pos, Square64 from, Bitboard toBitboard, MoveList &moveList);

template<PieceType PT>
constexpr const Bitboard* non_sliding_attack_table();
//TODO cómo hacer para evitar repeticiones de código
void generate_all_moves(const Position &pos, MoveList &moveList){
    if(pos.get_side_to_move() == WHITE){
        white_pawn_moves(pos, moveList);
        no_special_moves<WHITE, KNIGHT, CAPTURE>(pos, moveList);
        no_special_moves<WHITE, KNIGHT, QUIET>(pos, moveList);
        no_special_moves<WHITE, KING, CAPTURE>(pos, moveList);
        no_special_moves<WHITE, KING, QUIET>(pos, moveList);
    }
    else{
        black_pawn_moves(pos, moveList);
        no_special_moves<BLACK, KNIGHT, CAPTURE>(pos, moveList);
        no_special_moves<BLACK, KNIGHT, QUIET>(pos, moveList);
        no_special_moves<BLACK, KING, CAPTURE>(pos, moveList);
        no_special_moves<BLACK, KING, QUIET>(pos, moveList);
    }  
}

void white_pawn_moves(const Position &pos, MoveList &moveList){
    white_pawn_capture_moves(pos, moveList);
    white_pawn_quiet_moves(pos, moveList);
}

void black_pawn_moves(const Position &pos, MoveList &moveList){
    black_pawn_capture_moves(pos, moveList);
    black_pawn_quiet_moves(pos, moveList);
}

void generate_capture_moves(Position &pos, MoveList &moveList){
    if(pos.get_side_to_move() == WHITE){
        white_pawn_capture_moves(pos, moveList);
        no_special_moves<WHITE, KNIGHT, CAPTURE>(pos, moveList);
        no_special_moves<WHITE, KING, CAPTURE>(pos, moveList);
    }
    else{
        black_pawn_capture_moves(pos, moveList);
        no_special_moves<BLACK, KNIGHT, CAPTURE>(pos, moveList);
        no_special_moves<BLACK, KING, CAPTURE>(pos, moveList);
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

    Bitboard southEastMoves = (pos.get_pieceTypes_bitboard(BLACK, PAWN) >> NORTH_WEST) & ~Bitboards::FILE_A_MASK;
    Bitboard southEastCaptures = southEastMoves & pos.get_occupied_bitboard(WHITE) & ~Bitboards::RANK_1_MASK;
    extract_capture_moves<SOUTH_EAST, NO_SPECIAL>(pos, southEastCaptures, moveList);
    Bitboard southWestMoves = (pos.get_pieceTypes_bitboard(BLACK, PAWN) >> NORTH_EAST) & ~Bitboards::FILE_H_MASK;
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

void black_pawn_quiet_moves(const Position &pos, MoveList &moveList){

    Bitboard quietSimpleMoves = (pos.get_pieceTypes_bitboard(BLACK, PAWN) >> NORTH) & ~pos.get_occupied_bitboard(COLOR_NC);
    Bitboard notPromotionQuietMoves = quietSimpleMoves & ~Bitboards::RANK_1_MASK;
    extract_quiet_moves<SOUTH, SpecialMove::NO_SPECIAL>(notPromotionQuietMoves, moveList);
    Bitboard startMoves = ((quietSimpleMoves & Bitboards::RANK_6_MASK) >> NORTH) & ~pos.get_occupied_bitboard(COLOR_NC);
    extract_quiet_moves<SOUTH_SOUTH, SpecialMove::PAWN_START>(startMoves, moveList);

    Bitboard promotionQuietMoves = quietSimpleMoves & Bitboards::RANK_1_MASK;
    extract_quiet_moves<SOUTH, PROMOTION_BISHOP>(promotionQuietMoves, moveList);
    extract_quiet_moves<SOUTH, PROMOTION_KNIGHT>(promotionQuietMoves, moveList);
    extract_quiet_moves<SOUTH, PROMOTION_QUEEN> (promotionQuietMoves, moveList);
    extract_quiet_moves<SOUTH, PROMOTION_ROOK>  (promotionQuietMoves, moveList);

}

template<Color C, PieceType PT, MoveType MT>
void no_special_moves(const Position &pos, MoveList &moveList){

    Bitboard pieceBitboards = pos.get_pieceTypes_bitboard(C, PT);
    Bitboard attackMoves{ZERO};
    while (pieceBitboards) {
        Square64 fromSquare{__builtin_ctzll(pieceBitboards)}; 

        if constexpr(MT == QUIET){
            attackMoves = non_sliding_attack_table<PT>()[fromSquare] & ~pos.get_occupied_bitboard(COLOR_NC);
            extract_quiet_moves<NO_SPECIAL>(fromSquare, attackMoves, moveList);

        }
        if constexpr(MT == CAPTURE){
            attackMoves = non_sliding_attack_table<PT>()[fromSquare] & pos.get_occupied_bitboard(~C);
            extract_capture_moves<NO_SPECIAL>(pos, fromSquare, attackMoves, moveList);
        }
        pieceBitboards &= pieceBitboards - 1;
    }


}

template<PieceType PT>
constexpr const Bitboard* non_sliding_attack_table();
 
template<>
constexpr const Bitboard* non_sliding_attack_table<KNIGHT>() {
    return Attacks::knightAttacks;
}

template<>
constexpr const Bitboard* non_sliding_attack_table<KING>() {
    return Attacks::kingAttacks;
}

template<Direction D, SpecialMove SM>
void extract_quiet_moves(Bitboard toBitboard, MoveList &moveList){

    while (toBitboard) {
        Square64 to{__builtin_ctzll(toBitboard)};
        Square64 moveFrom{to - D};
        moveList.set_move(make_quiet_move(moveFrom, to, SM));
        toBitboard &= toBitboard - 1;
    }

}

template<SpecialMove SM>
void extract_quiet_moves(Square64 from, Bitboard toBitboard, MoveList &moveList){

    while (toBitboard) {
        Square64 to{__builtin_ctzll(toBitboard)};
        moveList.set_move(make_quiet_move(from, to, SM));
        toBitboard &= toBitboard - 1;
    }
}


template<Direction D, SpecialMove SM>
void extract_capture_moves(const Position &pos, Bitboard toBitboard, MoveList &moveList){

    while (toBitboard) {
        Square64 to{__builtin_ctzll(toBitboard)};
        Square64 from{to - D};
        moveList.set_move(make_capture_move(from, to, SM, pos.get_mailbox_piece(from), pos.get_mailbox_piece(to)));
        toBitboard &= toBitboard - 1;
    }

}

template<SpecialMove SM>
void extract_capture_moves(const Position &pos, Square64 from, Bitboard toBitboard, MoveList &moveList){

    while (toBitboard) {
        Square64 to{__builtin_ctzll(toBitboard)};
        moveList.set_move(make_capture_move(from, to, SM, pos.get_mailbox_piece(from), pos.get_mailbox_piece(to)));
        toBitboard &= toBitboard - 1;
    }

}

}
} 

