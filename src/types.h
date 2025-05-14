/*General type definitios for Xake*/

#ifndef INCLUDE_TYPES_H
#define INCLUDE_TYPES_H

#include <cstdint>
#include <string>
#include <string_view>
#include <chrono>

namespace Xake{
//TODO quitar size_t pero cuando se pueda, los size que sean el ultimo valor de enum o sino meter todos aqui
//Enum sizes

using DepthSize = unsigned short int;
using NodesSize = unsigned long long int;
using MovesSize = unsigned short int;
using Key = unsigned long long;
using Time = long long;

constexpr std::size_t SQUARE_SIZE_64 = 64;
constexpr std::size_t SQUARE_SIZE_120 = 120;

//
constexpr std::size_t MAX_POSITION_MOVES_SIZE = 256;
constexpr std::size_t MAX_GAME_MOVES = 2048;
constexpr std::size_t MAX_SAME_PIECE = 10;
constexpr std::size_t MAX_DEPTH = 64;

constexpr unsigned short int CASTLING_POSIBILITIES = 4 * 4;

constexpr int CHECKMATE_SCORE = 30000;

constexpr Xake::Time NO_TIME = -1;

enum PieceType{
  NO_PIECE_TYPE,
  PAWN, 
  KNIGHT, 
  BISHOP, 
  ROOK, 
  QUEEN, 
  KING,
  PIECETYPE_SIZE
};

enum Piece : int{
  NO_PIECE,
  W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
  B_PAWN = 8 + W_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
  PIECE_SIZE = 16
};

const std::string_view PIECE_NAMES{" PNBRQK  pnbrqk "};

/*  NOTE 
    IMPROVE 
    inherits from int becouse if a member want to be a explicit number, like PIECE_SIZE=12, 
    this 12 is an int type, and enum number by default are unsigned int types, so must be casted.
    Inheriting from int, they all be int by default. This can be checked on cppinsishts
*/

enum Color : int{
  WHITE,
  BLACK,
  COLOR_NC,
  COLOR_SIZE
};

constexpr Piece make_piece(Color c, PieceType pt) { 
  return Piece((c << 3) + pt); 
}
//REVIEW para que sirve
constexpr Color piece_color(Piece piece) { 
  return Color(piece >> 3);
}

constexpr PieceType piece_type(Piece piece) {
  return PieceType(0x7 & piece);
}

constexpr Color operator~(Color color) {
  return Color(color ^ BLACK); 
}

enum File : int{
  FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H, FILE_SIZE
};

enum Rank : int{
  RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8, RANK_SIZE
};

enum Square120 : int{
	SQ120_A1 = 21, SQ120_B1, SQ120_C1, SQ120_D1, SQ120_E1, SQ120_F1, SQ120_G1, SQ120_H1,
	SQ120_A2 = 31, SQ120_B2, SQ120_C2, SQ120_D2, SQ120_E2, SQ120_F2, SQ120_G2, SQ120_H2,
	SQ120_A3 = 41, SQ120_B3, SQ120_C3, SQ120_D3, SQ120_E3, SQ120_F3, SQ120_G3, SQ120_H3,
	SQ120_A4 = 51, SQ120_B4, SQ120_C4, SQ120_D4, SQ120_E4, SQ120_F4, SQ120_G4, SQ120_H4,
	SQ120_A5 = 61, SQ120_B5, SQ120_C5, SQ120_D5, SQ120_E5, SQ120_F5, SQ120_G5, SQ120_H5,
	SQ120_A6 = 71, SQ120_B6, SQ120_C6, SQ120_D6, SQ120_E6, SQ120_F6, SQ120_G6, SQ120_H6,
	SQ120_A7 = 81, SQ120_B7, SQ120_C7, SQ120_D7, SQ120_E7, SQ120_F7, SQ120_G7, SQ120_H7,
	SQ120_A8 = 91, SQ120_B8, SQ120_C8, SQ120_D8, SQ120_E8, SQ120_F8, SQ120_G8, SQ120_H8,
  SQ120_NO_SQUARE, SQ120_OFFBOARD
};

enum Square64 : int{
	SQ64_A1, SQ64_B1, SQ64_C1, SQ64_D1, SQ64_E1, SQ64_F1, SQ64_G1, SQ64_H1,
	SQ64_A2, SQ64_B2, SQ64_C2, SQ64_D2, SQ64_E2, SQ64_F2, SQ64_G2, SQ64_H2,
	SQ64_A3, SQ64_B3, SQ64_C3, SQ64_D3, SQ64_E3, SQ64_F3, SQ64_G3, SQ64_H3,
	SQ64_A4, SQ64_B4, SQ64_C4, SQ64_D4, SQ64_E4, SQ64_F4, SQ64_G4, SQ64_H4,
	SQ64_A5, SQ64_B5, SQ64_C5, SQ64_D5, SQ64_E5, SQ64_F5, SQ64_G5, SQ64_H5,
	SQ64_A6, SQ64_B6, SQ64_C6, SQ64_D6, SQ64_E6, SQ64_F6, SQ64_G6, SQ64_H6,
	SQ64_A7, SQ64_B7, SQ64_C7, SQ64_D7, SQ64_E7, SQ64_F7, SQ64_G7, SQ64_H7,
	SQ64_A8, SQ64_B8, SQ64_C8, SQ64_D8, SQ64_E8, SQ64_F8, SQ64_G8, SQ64_H8,
  SQ64_NO_SQUARE, SQ64_OFFBOARD
};


// TODO revisar todos los 64to120 y 120to64
// set_FEN ((rank + 2) * 10) + (file +1)
constexpr Square64 SQUARE120_to_SQUARE64[SQUARE_SIZE_120] = {
  SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_A1,       SQ64_B1,       SQ64_C1,       SQ64_D1,       SQ64_E1,       SQ64_F1,       SQ64_G1,       SQ64_H1,       SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_A2,       SQ64_B2,       SQ64_C2,       SQ64_D2,       SQ64_E2,       SQ64_F2,       SQ64_G2,       SQ64_H2,       SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_A3,       SQ64_B3,       SQ64_C3,       SQ64_D3,       SQ64_E3,       SQ64_F3,       SQ64_G3,       SQ64_H3,       SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_A4,       SQ64_B4,       SQ64_C4,       SQ64_D4,       SQ64_E4,       SQ64_F4,       SQ64_G4,       SQ64_H4,       SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_A5,       SQ64_B5,       SQ64_C5,       SQ64_D5,       SQ64_E5,       SQ64_F5,       SQ64_G5,       SQ64_H5,       SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_A6,       SQ64_B6,       SQ64_C6,       SQ64_D6,       SQ64_E6,       SQ64_F6,       SQ64_G6,       SQ64_H6,       SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_A7,       SQ64_B7,       SQ64_C7,       SQ64_D7,       SQ64_E7,       SQ64_F7,       SQ64_G7,       SQ64_H7,       SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_A8,       SQ64_B8,       SQ64_C8,       SQ64_D8,       SQ64_E8,       SQ64_F8,       SQ64_G8,       SQ64_H8,       SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD,
  SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD, SQ64_OFFBOARD,
};

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

const std::string SQUARE_NAMES[SQUARE_SIZE_120] = {
  "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD","OFFBOARD", "OFFBOARD", "OFFBOARD",
  "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD","OFFBOARD", "OFFBOARD", "OFFBOARD",
  "OFFBOARD", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "OFFBOARD",
  "OFFBOARD", "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "OFFBOARD",
  "OFFBOARD", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "OFFBOARD",
  "OFFBOARD", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "OFFBOARD",
  "OFFBOARD", "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "OFFBOARD",
  "OFFBOARD", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "OFFBOARD",
  "OFFBOARD", "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "OFFBOARD",
  "OFFBOARD", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "OFFBOARD",
  "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD","OFFBOARD", "OFFBOARD", "OFFBOARD",
  "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD", "OFFBOARD","OFFBOARD", "OFFBOARD", "OFFBOARD"
};

constexpr Square120 square120(Rank rank, File file){
  return Square120(((rank + 2) * 10) + (file + 1));
} 

constexpr Square64 square120_to_square64(Square120 square){
  return SQUARE120_to_SQUARE64[square];
} 

constexpr File square_file(Square120 square){ 
  return File((square-1) % 10); 
}

constexpr Rank square_rank(Square120 square){
  return Rank((square/10) - 2);
}

enum CastlingRight: int{ 
  NO_RIGHT = 0,
  WKCA     = 1, 
  WQCA     = 2, 
  BKCA     = 4, 
  BQCA     = 8 
};

enum Direction: int{
    NORTH = 10,
    NORTH_NORTH = NORTH + NORTH,
    SOUTH = -NORTH,
    SOUTH_SOUTH = SOUTH + SOUTH,
    EAST  = 1,
    WEST  = -EAST,

    NORTH_EAST = NORTH + EAST,
    NORTH_WEST = NORTH + WEST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,

    NORTH_NORTH_WEST = NORTH + NORTH_WEST,
    NORTH_NORTH_EAST = NORTH + NORTH_EAST,
    NORTH_WEST_WEST  = NORTH_WEST + WEST,
    NORTH_EAST_EAST  = NORTH_EAST + EAST,
    SOUTH_WEST_WEST  = SOUTH_WEST + WEST,
    SOUTH_EAST_EAST  = SOUTH_EAST + EAST,
    SOUTH_SOUTH_WEST = SOUTH + SOUTH_WEST,
    SOUTH_SOUTH_EAST = SOUTH + SOUTH_EAST
};

constexpr Square120 operator+(Square120& sq1, int sq2) { return Square120 (int(sq1) + sq2); }

inline File& operator++(File& f) { return f = File(int(f) + 1); }
constexpr File operator+(File f1, int f2) { return File (int(f1) + f2); }
inline File&   operator+=(File& f1, int f2) { return f1 = f1 + f2; }

inline Rank& operator--(Rank& r) { return r = Rank(int(r) - 1); }

inline CastlingRight&   operator|=(CastlingRight& cr1, int cr2){ return cr1 = CastlingRight(cr1 | cr2); }
constexpr CastlingRight   operator&(CastlingRight cr1, int cr2){ return CastlingRight(int(cr1) & cr2); }
inline CastlingRight&   operator&=(CastlingRight& cr1, int cr2){ return cr1 = CastlingRight(cr1 & cr2); }

} // namespace Xake

#endif // #ifndef INCLUDE_TYPESTUTORIAL_H