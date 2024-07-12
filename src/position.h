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
    Evaluate::Score get_material_score(Color color) const;
    bool is_repetition() const;

    //Move related functions
    bool do_move(Move move);
    void undo_move();
    void move_piece(Square120 from, Square120 to);
    void remove_piece(Square120 square);
    void add_piece(Square120 square, Piece piece);    

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
    //TODO esto meterlo en HistoryInfo?
    int historySize;
    HistoryInfo history[MAX_GAME_MOVES];
    Evaluate::Score materialScore[COLOR_SIZE];

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
    return materialScore[color];
}
inline Key Position::get_key() const{
    return history[historySize-1].positionKey;
}


} // namespace Xake

#endif // #ifndef INCLUDE_POSITION_H