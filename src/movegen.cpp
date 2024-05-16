#include "movegen.h"
#include "position.h"

namespace Xake{

namespace MoveGen{

enum MoveType{
    STANDARD,
    CAPTURE
};

//TODO using templates
//constexpr Direction knights_directions[] = {NORTH_NORTH_WEST, NORTH_NORTH_EAST, NORTH_WEST_WEST, NORTH_EAST_EAST, SOUTH_WEST_WEST, SOUTH_EAST_EAST, SOUTH_SOUTH_WEST, SOUTH_SOUTH_EAST}; 
//Function Declarations
template<Color C, Direction D, MoveType T>
void loop_move(const Position &position, MoveList &moveList, Square120 from);

template<Color C, Direction D, MoveType T>
void non_loop_move(const Position &position, MoveList &moveList, Square120 from);

//Pawns
void white_pawn_moves(const Position &position, MoveList &moveList);
void white_pawn_quiet_moves(const Position &position, MoveList &moveList, Square120 from);
void white_pawn_capture_moves(const Position &position, MoveList &moveList, Square120 from);

void black_pawn_moves(const Position &position, MoveList &moveList);
void black_pawn_quiet_moves(const Position &position, MoveList &moveList, Square120 from);
void black_pawn_capture_moves(const Position &position, MoveList &moveList, Square120 from);

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
        if(T == STANDARD)
            moveList.set_move(make_move(from, toSquare, SpecialMove::NO_SPECIAL, Piece::NO_PIECE));
        toSquareIndex += D;
        toSquare = SQUARES_120[toSquareIndex];
    }
    PieceType toSquarePiece = position.get_mailbox_pieceType(~C, toSquare);
    if(toSquarePiece != PieceType::NO_PIECE_TYPE){
        moveList.set_move(make_move(from, toSquare, SpecialMove::NO_SPECIAL, make_piece(~C,toSquarePiece)));
    }
}

template<Color C, Direction D, MoveType T>
void non_loop_move(const Position &position, MoveList &moveList, Square120 from){

    int toSquareIndex = from + D;
    Square120 toSquare = SQUARES_120[toSquareIndex];
    if(toSquare!= Square120::SQ120_OFFBOARD){
        if(T == STANDARD && position.get_mailbox_pieceType(Color::COLOR_NC, toSquare) == PieceType::NO_PIECE_TYPE){
            moveList.set_move(make_move(from, toSquare, SpecialMove::NO_SPECIAL, Piece::NO_PIECE));
        }
        PieceType toSquarePiece = position.get_mailbox_pieceType(~C, toSquare);     
        if(toSquarePiece != PieceType::NO_PIECE_TYPE){
            moveList.set_move(make_move(from, toSquare, SpecialMove::NO_SPECIAL, make_piece(~C,toSquarePiece)));
        }
    }  
}


//Pawns
void white_pawn_moves(const Position &position, MoveList &moveList){
    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(Xake::W_PAWN); ++pciCount){
        white_pawn_quiet_moves(position, moveList, position.get_piece_list(Xake::W_PAWN)[pciCount]);
        white_pawn_capture_moves(position, moveList, position.get_piece_list(Xake::W_PAWN)[pciCount]);
    }
}

void white_pawn_quiet_moves(const Position &position, MoveList &moveList, Square120 from){

    if(position.get_mailbox_pieceType(Xake::COLOR_NC, Square120(from+NORTH)) == Xake::NO_PIECE_TYPE)
    {
        if(square_rank(from) == RANK_2 && position.get_mailbox_pieceType(Xake::COLOR_NC, Square120(from+NORTH_NORTH))== Xake::NO_PIECE_TYPE){
            moveList.set_move(make_move(from, Square120(from+NORTH), SpecialMove::NO_SPECIAL, Xake::NO_PIECE));
            moveList.set_move(make_move(from, Square120(from+(NORTH_NORTH)), SpecialMove::PAWN_START, Xake::NO_PIECE));
        }else if(square_rank(from) == RANK_7){
            moveList.set_move(make_move(from, Square120(from+NORTH), Xake::PROMOTION_BISHOP, Xake::NO_PIECE));
            moveList.set_move(make_move(from, Square120(from+NORTH), Xake::PROMOTION_KNIGHT, Xake::NO_PIECE));
            moveList.set_move(make_move(from, Square120(from+NORTH), Xake::PROMOTION_QUEEN, Xake::NO_PIECE));
            moveList.set_move(make_move(from, Square120(from+NORTH), Xake::PROMOTION_ROOK, Xake::NO_PIECE));
        }else{
            moveList.set_move(make_move(from, Square120(from+NORTH), SpecialMove::NO_SPECIAL, Xake::NO_PIECE));
        }
       
    }
}

void white_pawn_capture_moves(const Position &position, MoveList &moveList, Square120 from){
    
    //en passant captures
    if(position.get_enpassant_square() != SQ120_NO_SQUARE){
        if(from+NORTH_WEST == position.get_enpassant_square()){
            moveList.set_move(make_move(from, Square120(from+NORTH_WEST), Xake::ENPASSANT, Piece::NO_PIECE));
        }        
        if(from+NORTH_EAST == position.get_enpassant_square()){
            moveList.set_move(make_move(from, Square120(from+NORTH_EAST), Xake::ENPASSANT, Piece::NO_PIECE));
        }
    }
    PieceType northWestEnemyPiece = position.get_mailbox_pieceType(Xake::BLACK, Square120(from+NORTH_WEST));
    if(northWestEnemyPiece != NO_PIECE_TYPE && square_file(from) != FILE_A){
        if(square_rank(from) != RANK_7){
            moveList.set_move(make_move(from, Square120(from+NORTH_WEST), SpecialMove::NO_SPECIAL, make_piece(Xake::BLACK, northWestEnemyPiece)));
        }
        else{
            Piece capturedPiece = make_piece(Xake::BLACK, northWestEnemyPiece);
            moveList.set_move(make_move(from, Square120(from+NORTH_WEST), Xake::PROMOTION_BISHOP,  capturedPiece));
            moveList.set_move(make_move(from, Square120(from+NORTH_WEST), Xake::PROMOTION_KNIGHT,  capturedPiece));
            moveList.set_move(make_move(from, Square120(from+NORTH_WEST), Xake::PROMOTION_QUEEN,   capturedPiece));
            moveList.set_move(make_move(from, Square120(from+NORTH_WEST), Xake::PROMOTION_ROOK,    capturedPiece));
        }
    }
    PieceType northEastEnemyPiece = position.get_mailbox_pieceType(Xake::BLACK, Square120(from+NORTH_EAST));
    if(northEastEnemyPiece != NO_PIECE_TYPE && square_file(from) != FILE_H){
        if(square_rank(from) != RANK_7){
            moveList.set_move(make_move(from, Square120(from+NORTH_EAST), SpecialMove::NO_SPECIAL, make_piece(Xake::BLACK, northEastEnemyPiece)));
        }
        else{
            Piece capturedPiece = make_piece(Xake::BLACK, northEastEnemyPiece);
            moveList.set_move(make_move(from, Square120(from+NORTH_EAST), Xake::PROMOTION_BISHOP,  capturedPiece));
            moveList.set_move(make_move(from, Square120(from+NORTH_EAST), Xake::PROMOTION_KNIGHT,  capturedPiece));
            moveList.set_move(make_move(from, Square120(from+NORTH_EAST), Xake::PROMOTION_QUEEN,   capturedPiece));
            moveList.set_move(make_move(from, Square120(from+NORTH_EAST), Xake::PROMOTION_ROOK,    capturedPiece));
        }
    }
}

void black_pawn_moves(const Position &position, MoveList &moveList){
    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(Xake::B_PAWN); ++pciCount){
        black_pawn_quiet_moves(position, moveList, position.get_piece_list(Xake::B_PAWN)[pciCount]);
        black_pawn_capture_moves(position, moveList, position.get_piece_list(Xake::B_PAWN)[pciCount]);
    }
}

void black_pawn_quiet_moves(const Position &position, MoveList &moveList, Square120 from){

    if(position.get_mailbox_pieceType(Xake::COLOR_NC, Square120(from+SOUTH)) == Xake::NO_PIECE_TYPE)
    {
        if(square_rank(from) == RANK_7 && position.get_mailbox_pieceType(Xake::COLOR_NC, Square120(from+SOUTH_SOUTH))== Xake::NO_PIECE_TYPE){
            moveList.set_move(make_move(from, Square120(from+SOUTH), SpecialMove::NO_SPECIAL, Xake::NO_PIECE));
            moveList.set_move(make_move(from, Square120(from+(SOUTH_SOUTH)), SpecialMove::PAWN_START, Xake::NO_PIECE));
        }else if(square_rank(from) == RANK_2){
            moveList.set_move(make_move(from, Square120(from+SOUTH), Xake::PROMOTION_BISHOP, Xake::NO_PIECE));
            moveList.set_move(make_move(from, Square120(from+SOUTH), Xake::PROMOTION_KNIGHT, Xake::NO_PIECE));
            moveList.set_move(make_move(from, Square120(from+SOUTH), Xake::PROMOTION_QUEEN, Xake::NO_PIECE));
            moveList.set_move(make_move(from, Square120(from+SOUTH), Xake::PROMOTION_ROOK, Xake::NO_PIECE));
        }else{
            moveList.set_move(make_move(from, Square120(from+SOUTH), SpecialMove::NO_SPECIAL, Xake::NO_PIECE));
        }
    }
}

void black_pawn_capture_moves(const Position &position, MoveList &moveList, Square120 from){
    
    //en passant captures
    if(position.get_enpassant_square() != SQ120_NO_SQUARE){
        if(from+SOUTH_WEST == position.get_enpassant_square()){
            moveList.set_move(make_move(from, Square120(from+SOUTH_WEST), Xake::ENPASSANT, Piece::NO_PIECE));
        }        
        if(from+SOUTH_EAST == position.get_enpassant_square()){
            moveList.set_move(make_move(from, Square120(from+SOUTH_EAST), Xake::ENPASSANT, Piece::NO_PIECE));
        }
    }
    PieceType southWestEnemyPiece = position.get_mailbox_pieceType(Xake::WHITE, Square120(from+SOUTH_WEST));
    if(southWestEnemyPiece != NO_PIECE_TYPE && square_file(from) != FILE_A){
        if(square_rank(from) != RANK_2){
            moveList.set_move(make_move(from, Square120(from+SOUTH_WEST), SpecialMove::NO_SPECIAL, make_piece(Xake::WHITE, southWestEnemyPiece)));
        }
        else{
            Piece capturedPiece = make_piece(Xake::WHITE, southWestEnemyPiece);
            moveList.set_move(make_move(from, Square120(from+SOUTH_WEST), Xake::PROMOTION_BISHOP,  capturedPiece));
            moveList.set_move(make_move(from, Square120(from+SOUTH_WEST), Xake::PROMOTION_KNIGHT,  capturedPiece));
            moveList.set_move(make_move(from, Square120(from+SOUTH_WEST), Xake::PROMOTION_QUEEN,   capturedPiece));
            moveList.set_move(make_move(from, Square120(from+SOUTH_WEST), Xake::PROMOTION_ROOK,    capturedPiece));
        }
    }
    PieceType southEastEnemyPiece = position.get_mailbox_pieceType(Xake::WHITE, Square120(from+SOUTH_EAST));
    if(southEastEnemyPiece != NO_PIECE_TYPE && square_file(from) != FILE_H){
        if(square_rank(from) != RANK_2){
            moveList.set_move(make_move(from, Square120(from+SOUTH_EAST), SpecialMove::NO_SPECIAL, make_piece(Xake::WHITE, southEastEnemyPiece)));
        }
        else{
            Piece capturedPiece = make_piece(Xake::WHITE, southEastEnemyPiece);
            moveList.set_move(make_move(from, Square120(from+SOUTH_EAST), Xake::PROMOTION_BISHOP,  capturedPiece));
            moveList.set_move(make_move(from, Square120(from+SOUTH_EAST), Xake::PROMOTION_KNIGHT,  capturedPiece));
            moveList.set_move(make_move(from, Square120(from+SOUTH_EAST), Xake::PROMOTION_QUEEN,   capturedPiece));
            moveList.set_move(make_move(from, Square120(from+SOUTH_EAST), Xake::PROMOTION_ROOK,    capturedPiece));
        }
    }
}

//Knights
template<Color C, MoveType T>
void Knight_moves(const Position &position, MoveList &moveList){

    Piece c_Knight = make_piece(C, Xake::KNIGHT);
    for(std::size_t pciCount = 0; pciCount < position.get_piece_size(c_Knight); ++pciCount){
        non_loop_move<C, NORTH_NORTH_WEST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, NORTH_NORTH_EAST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, NORTH_WEST_WEST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, NORTH_EAST_EAST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, SOUTH_WEST_WEST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, SOUTH_EAST_EAST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, SOUTH_SOUTH_WEST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]);
        non_loop_move<C, SOUTH_SOUTH_EAST, T>(position,moveList,position.get_piece_list(c_Knight)[pciCount]); 
    }  
}

//Bishops
template<Color C, MoveType T>
void bishop_moves(const Position &position, MoveList &moveList){
    Piece c_bishop = make_piece(C, Xake::BISHOP);
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
    Piece c_rook = make_piece(C, Xake::ROOK);
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
    Piece c_queen = make_piece(C, Xake::QUEEN);
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
    Piece c_king = make_piece(C, Xake::KING);
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
                    moveList.set_move(make_move(Square120::SQ120_E1, Square120::SQ120_G1, SpecialMove::CASTLE, Piece::NO_PIECE));
                }
            }
        }
        if(castlingRights & CastlingRight::WQCA){
            if(position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_D1) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_C1) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_B1) == PieceType::NO_PIECE_TYPE){
                if(!position.square_is_attacked(Square120::SQ120_E1) && !position.square_is_attacked(Square120::SQ120_D1)){
                    moveList.set_move(make_move(Square120::SQ120_E1, Square120::SQ120_C1, SpecialMove::CASTLE, Piece::NO_PIECE));
                }
            }
        }
    }
    else if(C == Color::BLACK){
        if(castlingRights & CastlingRight::BKCA){
            if(position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_F8) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_G8) == PieceType::NO_PIECE_TYPE){
                if(!position.square_is_attacked(Square120::SQ120_E8) && !position.square_is_attacked(Square120::SQ120_F8)){
                    moveList.set_move(make_move(Square120::SQ120_E8, Square120::SQ120_G8, SpecialMove::CASTLE, Piece::NO_PIECE));
                }
            }
        }
        if(castlingRights & CastlingRight::BQCA){
            if(position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_D8) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_C8) == PieceType::NO_PIECE_TYPE && 
               position.get_mailbox_pieceType(Color::COLOR_NC, Square120::SQ120_B8) == PieceType::NO_PIECE_TYPE){
                if(!position.square_is_attacked(Square120::SQ120_E8) && !position.square_is_attacked(Square120::SQ120_D8)){
                    moveList.set_move(make_move(Square120::SQ120_E8, Square120::SQ120_C8, SpecialMove::CASTLE, Piece::NO_PIECE));
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
    Xake::Color c= position.get_side_to_move();
    if(position.get_side_to_move() == WHITE){
        white_pawn_moves(position, moveList);
        Knight_moves<Color::WHITE, MoveType::STANDARD>(position, moveList);
        bishop_moves<Color::WHITE, MoveType::STANDARD>(position, moveList);
        rook_moves<Color::WHITE, MoveType::STANDARD>(position, moveList);
        queen_moves<Color::WHITE, MoveType::STANDARD>(position, moveList);
        king_moves<Color::WHITE, MoveType::STANDARD>(position, moveList);
    }else{
        black_pawn_moves(position, moveList);
        Knight_moves<Color::BLACK, MoveType::STANDARD>(position, moveList);
        bishop_moves<Color::BLACK, MoveType::STANDARD>(position, moveList);
        rook_moves<Color::BLACK, MoveType::STANDARD>(position, moveList);
        queen_moves<Color::BLACK, MoveType::STANDARD>(position, moveList);
        king_moves<Color::BLACK, MoveType::STANDARD>(position, moveList);
    }
}
}

}               