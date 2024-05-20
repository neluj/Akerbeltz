#ifndef INCLUDE_POSITION_H
#define INCLUDE_POSITION_H

#include "move.h"
#include "evaluate.h"
#include <sstream>


namespace Xake{

    struct HistoryInfo{
        Move nextMove;
        int castlingRight;
        unsigned short int fiftyMovesCounter;
        unsigned short int movesCounter;
        Square120 enpassantSquare;
        Key positionKey;
    };

    //TODO review if is repeted in types.h
    const Square120 SQUARES_120[SQUARE_SIZE_120] = {
      SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_A1,       SQ120_B1,       SQ120_C1,       SQ120_D1,       SQ120_E1,       SQ120_F1,       SQ120_G1,       SQ120_H1,       SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_A2,       SQ120_B2,       SQ120_C2,       SQ120_D2,       SQ120_E2,       SQ120_F2,       SQ120_G2,       SQ120_H2,       SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_A3,       SQ120_B3,       SQ120_C3,       SQ120_D3,       SQ120_E3,       SQ120_F3,       SQ120_G3,       SQ120_H3,       SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_A4,       SQ120_B4,       SQ120_C4,       SQ120_D4,       SQ120_E4,       SQ120_F4,       SQ120_G4,       SQ120_H4,       SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_A5,       SQ120_B5,       SQ120_C5,       SQ120_D5,       SQ120_E5,       SQ120_F5,       SQ120_G5,       SQ120_H5,       SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_A6,       SQ120_B6,       SQ120_C6,       SQ120_D6,       SQ120_E6,       SQ120_F6,       SQ120_G6,       SQ120_H6,       SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_A7,       SQ120_B7,       SQ120_C7,       SQ120_D7,       SQ120_E7,       SQ120_F7,       SQ120_G7,       SQ120_H7,       SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_A8,       SQ120_B8,       SQ120_C8,       SQ120_D8,       SQ120_E8,       SQ120_F8,       SQ120_G8,       SQ120_H8,       SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD,
      SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD, SQ120_OFFBOARD,
    };

class Position{

public:
    
    static void init();
    void set_FEN(std::string fenNotation);
    Color get_side_to_move() const;
    int get_castling_right() const;
    Square120 get_enpassant_square() const;
    unsigned short get_fifty_moves_counter() const;
    unsigned short get_moves_counter() const;
    PieceType get_mailbox_pieceType(Color color, Square120 square) const;
    std::size_t get_piece_size(Piece piece) const;
    const Square120* get_piece_list(Piece piece) const;
    Key get_key() const;
    bool square_is_attacked(Square120 square) const; 

    //Move related functions
    bool do_move(Move move);
    void undo_move();
    void move_piece(Square120 from, Square120 to);
    void remove_piece(Square120 square);
    void add_piece(Square120 square, Piece piece);

    void print_board();
    Evaluate::Score get_material_score(Color color) const;

private:

    void clean_position();
    static void zobris_prng();

    void clean_mailbox();
    void clean_piece_list();
    void clear_position_info();

    void calc_material_score();
    void calc_key();
    
    // NOTE revisar si merece así o guardando piezas directamente?
    PieceType mailbox[COLOR_SIZE][SQUARE_SIZE_120];
    std::size_t pieceCounter[PIECE_SIZE];
    Square120 pieceList[PIECE_SIZE][MAX_SAME_PIECE];
    Color sideToMove{COLOR_NC};
    //TODO improve this to CastligRigth type, and divid by color (stockfish  CastlingRights castling_rights(Color c) const;)
    //int castlingRight{0};
    //Square enpassantSquare{NO_SQUARE};
    //unsigned short int fiftyMovesCounter{0};
    //unsigned short int movesCounter{0};
    //Move moveHistory[MAX_GAME_MOVES];
    int historySize;
    HistoryInfo history[MAX_GAME_MOVES];
    Evaluate::Score material_score[COLOR_SIZE];

};

std::ostream& operator<<(std::ostream& os, const Position& pos);

/*NOTE https://stackoverflow.com/questions/9734175/why-are-class-member-functions-inlined
*/
inline Color Position::get_side_to_move() const{
    return sideToMove;
}
inline int Position::get_castling_right() const{
    return history[historySize-1].castlingRight;
}
inline Square120 Position::get_enpassant_square() const{
    return history[historySize-1].enpassantSquare;
}
inline unsigned short Position::get_fifty_moves_counter() const{
    return history[historySize-1].fiftyMovesCounter;
}
inline unsigned short Position::get_moves_counter() const{
    return history[historySize-1].movesCounter;
}
inline PieceType Position::get_mailbox_pieceType(Color color, Square120 square) const{
    return mailbox[color][square];
}
inline std::size_t Position::get_piece_size(Piece piece) const{
    return pieceCounter[piece];
}
inline const Square120* Position::get_piece_list(Piece piece) const{
    return pieceList[piece];
}
inline Evaluate::Score Position::get_material_score(Color color) const{
    return material_score[color];
}
inline Key Position::get_key() const{
    return history[historySize-1].positionKey;
}


} // namespace Xake

#endif // #ifndef INCLUDE_POSITION_H