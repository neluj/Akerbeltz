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
        Square enpassantSquare;
    };

    const Square SQUARES_120[SQUARE_SIZE_120] = {
      OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD,
      OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD,
      OFFBOARD, A1, B1, C1, D1, E1, F1, G1, H1, OFFBOARD,
      OFFBOARD, A2, B2, C2, D2, E2, F2, G2, H2, OFFBOARD,
      OFFBOARD, A3, B3, C3, D3, E3, F3, G3, H3, OFFBOARD,
      OFFBOARD, A4, B4, C4, D4, E4, F4, G4, H4, OFFBOARD,
      OFFBOARD, A5, B5, C5, D5, E5, F5, G5, H5, OFFBOARD,
      OFFBOARD, A6, B6, C6, D6, E6, F6, G6, H6, OFFBOARD,
      OFFBOARD, A7, B7, C7, D7, E7, F7, G7, H7, OFFBOARD,
      OFFBOARD, A8, B8, C8, D8, E8, F8, G8, H8, OFFBOARD,
      OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD,
      OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD, OFFBOARD,
    };

class Position{

public:
    
    Position(); 
    void set_FEN(const std::string & fenNotation);
    Color get_side_to_move() const;
    int get_castling_right() const;
    Square get_enpassant_square() const;
    unsigned short get_fifty_moves_counter() const;
    unsigned short get_moves_counter() const;
    PieceType get_mailbox_piece(const Color &color, const Square &square) const;
    std::size_t get_piece_size(const Piece &piece) const;
    const Square* get_piece_list(const Piece &piece) const;
    bool square_is_attacked(const Square &square) const; 

    //Move related functions
    bool do_move(const Move &move);
    void undo_move();
    void move_piece(const Square &from, const Square &to);
    void remove_piece(const Square &square);
    void add_piece(const Square &square, const Piece &piece);

    void print_board();
    Evaluate::Score get_material_score(const Color &color);

private:

    void init();
    void calc_material_score();

    PieceType mailbox[COLOR_SIZE][SQUARE_SIZE_120];
    std::size_t pieceCounter[PIECE_SIZE];
    Square pieceList[PIECE_SIZE][MAX_SAME_PIECE];
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

/*NOTE https://stackoverflow.com/questions/9734175/why-are-class-member-functions-inlined
*/
inline Color Position::get_side_to_move() const{
    return sideToMove;
}
inline int Position::get_castling_right() const{
    return history[historySize-1].castlingRight;
}
inline Square Position::get_enpassant_square() const{
    return history[historySize-1].enpassantSquare;
}
inline unsigned short Position::get_fifty_moves_counter() const{
    return history[historySize-1].fiftyMovesCounter;
}
inline unsigned short Position::get_moves_counter() const{
    return history[historySize-1].movesCounter;
}
inline PieceType Position::get_mailbox_piece(const Color &color, const Square &square) const{
    return mailbox[color][square];
}
inline std::size_t Position::get_piece_size(const Piece &piece) const{
    return pieceCounter[piece];
}
inline const Square* Position::get_piece_list(const Piece &piece) const{
    return pieceList[piece];
}
inline Evaluate::Score Position::get_material_score(const Color &color){
    return material_score[color];
}


} // namespace Xake

#endif // #ifndef INCLUDE_POSITION_H