#ifndef INCLUDE_POSITION_H
#define INCLUDE_POSITION_H

#include "bitboards.h"
#include "move.h"
#include "evaluate.h"
#include <sstream>


namespace Xake{

    struct HistoryInfo{
        Move nextMove;
        CastlingRight castlingRight;
        unsigned short int fiftyMovesCounter;
        unsigned short int movesCounter;
        Square64 enpassantSquare;
        Key positionKey;
    };

class Position{

public:
    
    static void init();
    void set_FEN(std::string fenNotation);
    std::string get_FEN() const;
    Color get_side_to_move() const;
    int get_ply() const;
    int get_castling_right() const;
    Square64 get_enpassant_square() const;
    unsigned short get_fifty_moves_counter() const;
    unsigned short get_moves_counter() const;
    PieceType get_mailbox_pieceType(Color color, Square64 square) const;
    Bitboard get_pieceTypes_bitboard(Color color, PieceType pieceType) const;
    Bitboard get_occupied_bitboard(Color color) const;
    Key get_key() const;
    //bool square_is_attacked(Square64 square) const; 
    Evaluate::Score get_material_score(Color color) const;
    bool is_repetition() const;

    //Move related functions
    //bool do_move(Move move);
    void undo_move();
    void move_piece(Square64 from, Square64 to);
    void remove_piece(Square64 square);
    void add_piece(Square64 square, Piece piece);    

private:

    void clear_position();
    static void zobris_prng();

    void clear_position_info();
    void clear_pieceTypes_bitboards();
    void clear_occupied_bitboards();
    void clear_mailbox();

    void calc_material_score();
    void calc_key();
    
    Bitboard pieceTypesBitboards[COLOR_SIZE][PIECETYPE_SIZE];
    Bitboard occupiedBitboards[COLOR_SIZE];
    PieceType mailbox[COLOR_SIZE][SQUARE_SIZE_64];
    Color sideToMove{COLOR_NC};
    //TODO esto meterlo en HistoryInfo?
    int ply;
    HistoryInfo historyPly[MAX_GAME_MOVES];
    Evaluate::Score materialScore[COLOR_SIZE];

};

std::ostream& operator<<(std::ostream& os, const Position& pos);

inline Color Position::get_side_to_move() const{
    return sideToMove;
}
inline int Position::get_ply() const{
    return ply;
}
inline int Position::get_castling_right() const{
    return historyPly[ply-1].castlingRight;
}
inline Square64 Position::get_enpassant_square() const{
    return historyPly[ply-1].enpassantSquare;
}
inline unsigned short Position::get_fifty_moves_counter() const{
    return historyPly[ply-1].fiftyMovesCounter;
}
inline unsigned short Position::get_moves_counter() const{
    return historyPly[ply-1].movesCounter;
}
inline PieceType Position::get_mailbox_pieceType(Color color, Square64 square) const{
    return mailbox[color][square];
}
inline Bitboard Position::get_pieceTypes_bitboard(Color color, PieceType pieceType) const{
    return pieceTypesBitboards[color][pieceType];
}
inline Bitboard Position::get_occupied_bitboard(Color color) const{
    return occupiedBitboards[color];
}
inline Evaluate::Score Position::get_material_score(Color color) const{
    return materialScore[color];
}
inline Key Position::get_key() const{
    return historyPly[ply-1].positionKey;
}


} // namespace Xake

#endif // #ifndef INCLUDE_POSITION_H