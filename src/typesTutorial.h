/*General type definitios for Xake*/

#ifndef INCLUDE_TYPESTUTORIAL_H
#define INCLUDE_TYPESTUTORIAL_H

#include <cstdint>
#include <string>

namespace Xake{

//Enum sizes
constexpr std::size_t PIECE_SIZE = 12;
constexpr std::size_t FILE_SIZE = 8;
constexpr std::size_t RANK_SIZE = 8;
constexpr std::size_t SQUARE_SIZE = 64;

//
constexpr std::size_t MAX_POSITION_MOVES_SIZE = 256;
constexpr std::size_t MAX_GAME_MOVES = 2048;
constexpr std::size_t MAX_SAME_PIECE = 10;

enum PieceType{
  NO_PIECE,
  PAWN, 
  KNIGHT, 
  BISHOP, 
  ROOK, 
  QUEEN, 
  KING
};

/*  NOTE 
    IMPROVE 
    inherits from int becouse if a member want to be a explicit number, like PIECE_SIZE=12, 
    this 12 is an int type, and enum number by default are unsigned int types, so must be casted.
    Inheriting from int, they all be int by default. This can be checked on cppinsishts
*/
// IMPROVE crear array con color/piecetype
enum Piece : int{
  W_PAWN, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
  B_PAWN, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
  EMPTY
};


enum Color : int{
  WHITE,
  BLACK,
  COLOR_NC
};

// IMPROVE
constexpr Color piece_color(Piece piece){
  if(piece<6){
    return WHITE;
  }else if(piece<12){
    return BLACK;
  }
  return COLOR_NC;
}

constexpr Color operator~(Color color) {
  return Color(color ^ 1); 
}

enum File : int{
  FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

enum Rank : int{
  RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
};

// BITBOARD
enum Square : int{
	A1 = 21, B1, C1, D1, E1, F1, G1, H1,
	A2 = 31, B2, C2, D2, E2, F2, G2, H2,
	A3 = 41, B3, C3, D3, E3, F3, G3, H3,
	A4 = 51, B4, C4, D4, E4, F4, G4, H4,
	A5 = 61, B5, C5, D5, E5, F5, G5, H5,
	A6 = 71, B6, C6, D6, E6, F6, G6, H6,
	A7 = 81, B7, C7, D7, E7, F7, G7, H7,
	A8 = 91, B8, C8, D8, E8, F8, G8, H8,
  NO_SQUARE, OFFBOARD
};

constexpr Rank square_rank(Square square){
  return Rank(square >> 3);
}

constexpr File square_file(Square square){ 
  return File(square & 7); 
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
    SOUTH = -NORTH,
    EAST  = 1,
    WEST  = -EAST,

    NORTH_EAST = NORTH + EAST,
    NORTH_WEST = NORTH + WEST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST
};


} // namespace Xake

#endif // #ifndef INCLUDE_TYPESTUTORIAL_H