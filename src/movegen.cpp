#include "movegen.h"
#include "position.h"

namespace Xake{

namespace MoveGen{

//TODO using templates
//constexpr Direction knights_directions[] = {NORTH_NORTH_WEST, NORTH_NORTH_EAST, NORTH_WEST_WEST, NORTH_EAST_EAST, SOUTH_WEST_WEST, SOUTH_EAST_EAST, SOUTH_SOUTH_WEST, SOUTH_SOUTH_EAST}; 
//Function Declarations
template<Color C, Direction D, MoveType T>
void loop_move(const Position &position, MoveList &moveList, Square120 from);

template<Color C, Direction D, MoveType T>
void non_loop_move(const Position &position, MoveList &moveList, Square120 from);

//Pawns
void white_pawn_moves(const Position &position, MoveList &moveList);
void white_pawn_quiet_moves(const Position &position, MoveList &moveList);
void white_pawn_capture_moves(const Position &position, MoveList &moveList);

void black_pawn_moves(const Position &position, MoveList &moveList);
void black_pawn_quiet_moves(const Position &position, MoveList &moveList);
void black_pawn_capture_moves(const Position &position, MoveList &moveList);

//Knights

//Bishops
template<Color C, MoveType T>
void bishop_moves(const Position &position, MoveList &moveList);

//Rooks
template<Color C, MoveType T>
void rook_moves(const Position &position, MoveList &moveList);

//Queens
template<Color C, MoveType T>
void queen_moves(const Position &position, MoveList &moveList);

//Kings
template<Color C, MoveType T>
void king_moves(const Position &position, MoveList &moveList);

//Function Definitions

template<Color C, Direction D, MoveType T>
void loop_move(const Position &position, MoveList &moveList, Square120 from){

    int toSquareIndex = from + D;
    Square120 toSquare = SQUARES_120[toSquareIndex];
    while(toSquare!= Square120::SQ120_OFFBOARD && position.get_mailbox_pieceType(Color::COLOR_NC, toSquare) == PieceType::NO_PIECE_TYPE){
        if constexpr(T == QUIET)
            moveList.set_move(make_quiet_move(from, toSquare, SpecialMove::NO_SPECIAL));
        toSquareIndex += D;
        toSquare = SQUARES_120[toSquareIndex];
    }

    PieceType toSquarePiece = position.get_mailbox_pieceType(~C, toSquare);
    if(toSquarePiece != PieceType::NO_PIECE_TYPE){
        PieceType fromSquarePiece = position.get_mailbox_pieceType(C, from);
        moveList.set_move(make_capture_move(from, toSquare, SpecialMove::NO_SPECIAL, make_piece(C,fromSquarePiece), make_piece(~C,toSquarePiece)));
    }
}

template<Color C, Direction D, MoveType T>
void non_loop_move(const Position &position, MoveList &moveList, Square120 from){

    int toSquareIndex = from + D;
    Square120 toSquare = SQUARES_120[toSquareIndex];
    if(toSquare!= Square120::SQ120_OFFBOARD){
        if(T == QUIET && position.get_mailbox_pieceType(Color::COLOR_NC, toSquare) == PieceType::NO_PIECE_TYPE){
            moveList.set_move(make_quiet_move(from, toSquare, SpecialMove::NO_SPECIAL));
        }
        PieceType toSquarePiece = position.get_mailbox_pieceType(~C, toSquare);     
        if(toSquarePiece != PieceType::NO_PIECE_TYPE){
            PieceType fromSquarePiece = position.get_mailbox_pieceType(C, from);
            moveList.set_move(make_capture_move(from, toSquare, SpecialMove::NO_SPECIAL, make_piece(C,fromSquarePiece), make_piece(~C,toSquarePiece)));
        }
    }  
}

//Pawns
void white_pawn_moves(const Position &position, MoveList &moveList){
    white_pawn_quiet_moves(position, moveList);
    white_pawn_capture_moves(position, moveList); 
}

void white_pawn_quiet_moves(const Position &position, MoveList &moveList){

    Square120 from = SQ120_NO_SQUARE;

    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(W_PAWN); ++pciCount){
        
        from = position.get_piece_list(W_PAWN)[pciCount];

        if(position.get_mailbox_pieceType(COLOR_NC, Square120(from+NORTH)) == NO_PIECE_TYPE)
        {
            if(square_rank(from) == RANK_2 && position.get_mailbox_pieceType(COLOR_NC, Square120(from+NORTH_NORTH))== NO_PIECE_TYPE){
                moveList.set_move(make_quiet_move(from, Square120(from+NORTH), SpecialMove::NO_SPECIAL));
                moveList.set_move(make_quiet_move(from, Square120(from+(NORTH_NORTH)), SpecialMove::PAWN_START));
            }else if(square_rank(from) == RANK_7){
                moveList.set_move(make_quiet_move(from, Square120(from+NORTH), PROMOTION_BISHOP));
                moveList.set_move(make_quiet_move(from, Square120(from+NORTH), PROMOTION_KNIGHT));
                moveList.set_move(make_quiet_move(from, Square120(from+NORTH), PROMOTION_QUEEN));
                moveList.set_move(make_quiet_move(from, Square120(from+NORTH), PROMOTION_ROOK));
            }else{
                moveList.set_move(make_quiet_move(from, Square120(from+NORTH), SpecialMove::NO_SPECIAL));
            }
        
        }
    }
}

void white_pawn_capture_moves(const Position &position, MoveList &moveList){

    Square120 from = SQ120_NO_SQUARE;

    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(W_PAWN); ++pciCount){
        
        from = position.get_piece_list(W_PAWN)[pciCount];

        //en passant captures
        Xake::Square120 enpassantSquare = position.get_enpassant_square();
        if(enpassantSquare != SQ120_NO_SQUARE){
            if(from+NORTH_WEST == enpassantSquare){
                // BUG enpassant capture must be NO_PIECE?
                moveList.set_move(make_enpassant_move(from, Square120(from+NORTH_WEST)));
            }        
            else if(from+NORTH_EAST == enpassantSquare){
                moveList.set_move(make_enpassant_move(from, Square120(from+NORTH_EAST)));
            }
        }
        PieceType northWestEnemyPiece = position.get_mailbox_pieceType(BLACK, Square120(from+NORTH_WEST));
        if(northWestEnemyPiece != NO_PIECE_TYPE && square_file(from) != FILE_A){
            if(square_rank(from) != RANK_7){
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_WEST), SpecialMove::NO_SPECIAL, Piece::W_PAWN, make_piece(BLACK, northWestEnemyPiece)));
            }
            else{
                Piece capturedPiece = make_piece(BLACK, northWestEnemyPiece);
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_WEST), PROMOTION_BISHOP,  Piece::W_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_WEST), PROMOTION_KNIGHT,  Piece::W_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_WEST), PROMOTION_QUEEN,   Piece::W_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_WEST), PROMOTION_ROOK,    Piece::W_PAWN, capturedPiece));
            }
        }
        PieceType northEastEnemyPiece = position.get_mailbox_pieceType(BLACK, Square120(from+NORTH_EAST));
        if(northEastEnemyPiece != NO_PIECE_TYPE && square_file(from) != FILE_H){
            if(square_rank(from) != RANK_7){
                Piece capturedPiece = make_piece(BLACK, northEastEnemyPiece);
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_EAST), SpecialMove::NO_SPECIAL, Piece::W_PAWN, capturedPiece));
            }
            else{
                Piece capturedPiece = make_piece(BLACK, northEastEnemyPiece);
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_EAST), PROMOTION_BISHOP,  Piece::W_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_EAST), PROMOTION_KNIGHT,  Piece::W_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_EAST), PROMOTION_QUEEN,   Piece::W_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+NORTH_EAST), PROMOTION_ROOK,    Piece::W_PAWN, capturedPiece));
            }
        }
    }
}

void black_pawn_moves(const Position &position, MoveList &moveList){
    black_pawn_quiet_moves(position, moveList);
    black_pawn_capture_moves(position, moveList);
}

void black_pawn_quiet_moves(const Position &position, MoveList &moveList){

    Square120 from = SQ120_NO_SQUARE;

    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(B_PAWN); ++pciCount){

        from = position.get_piece_list(B_PAWN)[pciCount];

        if(position.get_mailbox_pieceType(COLOR_NC, Square120(from+SOUTH)) == NO_PIECE_TYPE)
        {
            if(square_rank(from) == RANK_7 && position.get_mailbox_pieceType(COLOR_NC, Square120(from+SOUTH_SOUTH))== NO_PIECE_TYPE){
                moveList.set_move(make_quiet_move(from, Square120(from+SOUTH), SpecialMove::NO_SPECIAL));
                moveList.set_move(make_quiet_move(from, Square120(from+(SOUTH_SOUTH)), SpecialMove::PAWN_START));
            }else if(square_rank(from) == RANK_2){
                moveList.set_move(make_quiet_move(from, Square120(from+SOUTH), PROMOTION_BISHOP));
                moveList.set_move(make_quiet_move(from, Square120(from+SOUTH), PROMOTION_KNIGHT));
                moveList.set_move(make_quiet_move(from, Square120(from+SOUTH), PROMOTION_QUEEN));
                moveList.set_move(make_quiet_move(from, Square120(from+SOUTH), PROMOTION_ROOK));
            }else{
                moveList.set_move(make_quiet_move(from, Square120(from+SOUTH), SpecialMove::NO_SPECIAL));
            }
        }
    }
}

void black_pawn_capture_moves(const Position &position, MoveList &moveList){

    Square120 from = SQ120_NO_SQUARE;

    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(B_PAWN); ++pciCount){

        from = position.get_piece_list(B_PAWN)[pciCount];

        //en passant captures
        Xake::Square120 enpassantSquare = position.get_enpassant_square();
        if(enpassantSquare != SQ120_NO_SQUARE){
            if(from+SOUTH_WEST == enpassantSquare){
                moveList.set_move(make_enpassant_move(from, Square120(from+SOUTH_WEST)));
            }        
            else if(from+SOUTH_EAST == enpassantSquare){
                moveList.set_move(make_enpassant_move(from, Square120(from+SOUTH_EAST)));
            }
        }
        PieceType southWestEnemyPiece = position.get_mailbox_pieceType(WHITE, Square120(from+SOUTH_WEST));
        if(southWestEnemyPiece != NO_PIECE_TYPE && square_file(from) != FILE_A){
            if(square_rank(from) != RANK_2){
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_WEST), SpecialMove::NO_SPECIAL, Piece::B_PAWN, make_piece(WHITE, southWestEnemyPiece)));
            }
            else{
                Piece capturedPiece = make_piece(WHITE, southWestEnemyPiece);
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_WEST), PROMOTION_BISHOP, Piece::B_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_WEST), PROMOTION_KNIGHT, Piece::B_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_WEST), PROMOTION_QUEEN,  Piece::B_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_WEST), PROMOTION_ROOK,   Piece::B_PAWN, capturedPiece));
            }
        }
        PieceType southEastEnemyPiece = position.get_mailbox_pieceType(WHITE, Square120(from+SOUTH_EAST));
        if(southEastEnemyPiece != NO_PIECE_TYPE && square_file(from) != FILE_H){
            if(square_rank(from) != RANK_2){
                Piece capturedPiece = make_piece(WHITE, southEastEnemyPiece);
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_EAST), SpecialMove::NO_SPECIAL, Piece::B_PAWN, capturedPiece));
            }
            else{
                Piece capturedPiece = make_piece(WHITE, southEastEnemyPiece);
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_EAST), PROMOTION_BISHOP,  Piece::B_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_EAST), PROMOTION_KNIGHT,  Piece::B_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_EAST), PROMOTION_QUEEN,   Piece::B_PAWN, capturedPiece));
                moveList.set_move(make_capture_move(from, Square120(from+SOUTH_EAST), PROMOTION_ROOK,    Piece::B_PAWN, capturedPiece));
            }
        }
    }
}

//Knights
template<Color C, MoveType T>
void Knight_moves(const Position &position, MoveList &moveList){

    Piece c_Knight = make_piece(C, KNIGHT);
    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(c_Knight); ++pciCount){
        non_loop_move<C, NORTH_NORTH_WEST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, NORTH_NORTH_EAST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, NORTH_WEST_WEST,  T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, NORTH_EAST_EAST,  T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, SOUTH_WEST_WEST,  T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, SOUTH_EAST_EAST,  T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, SOUTH_SOUTH_WEST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, SOUTH_SOUTH_EAST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]); 
    }  
}

//Bishops
template<Color C, MoveType T>
void bishop_moves(const Position &position, MoveList &moveList){
    Piece c_bishop = make_piece(C, BISHOP);
    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(c_bishop); ++pciCount){
        loop_move<C,NORTH_EAST,T>(position,moveList,position.get_piece_list(c_bishop)[pciCount]);
        loop_move<C,NORTH_WEST,T>(position,moveList,position.get_piece_list(c_bishop)[pciCount]);
        loop_move<C,SOUTH_EAST,T>(position,moveList,position.get_piece_list(c_bishop)[pciCount]);
        loop_move<C,SOUTH_WEST,T>(position,moveList,position.get_piece_list(c_bishop)[pciCount]);
    }  
}

//Rooks
template<Color C, MoveType T>
void rook_moves(const Position &position, MoveList &moveList){
    Piece c_rook = make_piece(C, ROOK);
    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(c_rook); ++pciCount){
        loop_move<C,EAST, T>(position,moveList,position.get_piece_list(c_rook)[pciCount]);
        loop_move<C,WEST, T>(position,moveList,position.get_piece_list(c_rook)[pciCount]);
        loop_move<C,NORTH,T>(position,moveList,position.get_piece_list(c_rook)[pciCount]);
        loop_move<C,SOUTH,T>(position,moveList,position.get_piece_list(c_rook)[pciCount]);
    }  
}

//Queens
template<Color C, MoveType T>
void queen_moves(const Position &position, MoveList &moveList){
    Piece c_queen = make_piece(C, QUEEN);
    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(c_queen); ++pciCount){
        loop_move<C, EAST, T>(position,moveList,position.get_piece_list(c_queen)[pciCount]);
        loop_move<C, WEST, T>(position,moveList,position.get_piece_list(c_queen)[pciCount]);
        loop_move<C, NORTH,T>(position,moveList,position.get_piece_list(c_queen)[pciCount]);
        loop_move<C, SOUTH,T>(position,moveList,position.get_piece_list(c_queen)[pciCount]);
        loop_move<C, NORTH_EAST,T>(position,moveList,position.get_piece_list(c_queen)[pciCount]);
        loop_move<C, NORTH_WEST,T>(position,moveList,position.get_piece_list(c_queen)[pciCount]);
        loop_move<C, SOUTH_EAST,T>(position,moveList,position.get_piece_list(c_queen)[pciCount]);
        loop_move<C, SOUTH_WEST,T>(position,moveList,position.get_piece_list(c_queen)[pciCount]);
    }     
}

//Kings
template<Color C, MoveType T>
void king_moves(const Position &position, MoveList &moveList){
    Piece c_king = make_piece(C, KING);
    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(c_king); ++pciCount){
        non_loop_move<C, EAST, T>(position,moveList,position.get_piece_list(c_king)[pciCount]);
        non_loop_move<C, WEST, T>(position,moveList,position.get_piece_list(c_king)[pciCount]);
        non_loop_move<C, NORTH,T>(position,moveList,position.get_piece_list(c_king)[pciCount]);
        non_loop_move<C, SOUTH,T>(position,moveList,position.get_piece_list(c_king)[pciCount]);
        non_loop_move<C, NORTH_EAST,T>(position,moveList,position.get_piece_list(c_king)[pciCount]);
        non_loop_move<C, NORTH_WEST,T>(position,moveList,position.get_piece_list(c_king)[pciCount]);
        non_loop_move<C, SOUTH_EAST,T>(position,moveList,position.get_piece_list(c_king)[pciCount]);
        non_loop_move<C, SOUTH_WEST,T>(position,moveList,position.get_piece_list(c_king)[pciCount]);
    }

    //Castling
    int castlingRights = position.get_castling_right();
    if(C == Color::WHITE){
        if(castlingRights & CastlingRight::WKCA){
            if(position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_F1) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_G1 ) == PieceType::NO_PIECE_TYPE){
                if(!position.square_is_attacked(Square120::SQ120_F1) && !position.square_is_attacked(Square120::SQ120_E1)){
                    moveList.set_move(make_quiet_move(Square120::SQ120_E1, Square120::SQ120_G1, SpecialMove::CASTLE));
                }
            }
        }
        if(castlingRights & CastlingRight::WQCA){
            if(position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_D1) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_C1) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_B1) == PieceType::NO_PIECE_TYPE){
                if(!position.square_is_attacked(Square120::SQ120_E1) && !position.square_is_attacked(Square120::SQ120_D1)){
                    moveList.set_move(make_quiet_move(Square120::SQ120_E1, Square120::SQ120_C1, SpecialMove::CASTLE));
                }
            }
        }
    }
    else if(C == Color::BLACK){
        if(castlingRights & CastlingRight::BKCA){
            if(position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_F8) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_G8) == PieceType::NO_PIECE_TYPE){
                if(!position.square_is_attacked(Square120::SQ120_E8) && !position.square_is_attacked(Square120::SQ120_F8)){
                    moveList.set_move(make_quiet_move(Square120::SQ120_E8, Square120::SQ120_G8, SpecialMove::CASTLE));
                }
            }
        }
        if(castlingRights & CastlingRight::BQCA){
            if(position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_D8) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_C8) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_B8) == PieceType::NO_PIECE_TYPE){
                if(!position.square_is_attacked(Square120::SQ120_E8) && !position.square_is_attacked(Square120::SQ120_D8)){
                    moveList.set_move(make_quiet_move(Square120::SQ120_E8, Square120::SQ120_C8, SpecialMove::CASTLE));
                }
            }
        }
    }  
}
// TODO usar templates para movimientos de peones 
// TODO meter asserts para comprobar que los squares existen siempre
//Global
void generate_all_moves(const Position &position, MoveList &moveList){
    //TODO cambiar a pasar c como parametro
    //Color c= position.get_side_to_move();
    if(position.get_side_to_move() == WHITE){
        white_pawn_moves(position, moveList);
        Knight_moves<Color::WHITE, MoveType::QUIET>(position, moveList);
        bishop_moves<Color::WHITE, MoveType::QUIET>(position, moveList);
        rook_moves<Color::WHITE, MoveType::QUIET>(position, moveList);
        queen_moves<Color::WHITE, MoveType::QUIET>(position, moveList);
        king_moves<Color::WHITE, MoveType::QUIET>(position, moveList);
    }else{
        black_pawn_moves(position, moveList);
        Knight_moves<Color::BLACK, MoveType::QUIET>(position, moveList);
        bishop_moves<Color::BLACK, MoveType::QUIET>(position, moveList);
        rook_moves<Color::BLACK, MoveType::QUIET>(position, moveList);
        queen_moves<Color::BLACK, MoveType::QUIET>(position, moveList);
        king_moves<Color::BLACK, MoveType::QUIET>(position, moveList);
    }
}

void generate_capture_moves(Position &position, MoveList &moveList){
    //TODO cambiar a pasar c como parametro
    //Color c= position.get_side_to_move();
    if(position.get_side_to_move() == WHITE){
        white_pawn_capture_moves(position, moveList);
        Knight_moves<Color::WHITE, MoveType::CAPTURE>(position, moveList);
        bishop_moves<Color::WHITE, MoveType::CAPTURE>(position, moveList);
        rook_moves<Color::WHITE, MoveType::CAPTURE>(position, moveList);
        queen_moves<Color::WHITE, MoveType::CAPTURE>(position, moveList);
        king_moves<Color::WHITE, MoveType::CAPTURE>(position, moveList);
    }else{
        black_pawn_capture_moves(position, moveList);
        Knight_moves<Color::BLACK, MoveType::CAPTURE>(position, moveList);
        bishop_moves<Color::BLACK, MoveType::CAPTURE>(position, moveList);
        rook_moves<Color::BLACK, MoveType::CAPTURE>(position, moveList);
        queen_moves<Color::BLACK, MoveType::CAPTURE>(position, moveList);
        king_moves<Color::BLACK, MoveType::CAPTURE>(position, moveList);
    }
}

} 
}              