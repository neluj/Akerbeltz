#include "position.h"
#include "attacks.h"

#include <random>

namespace Xake
{

namespace Zobrist{

    constexpr uint64_t ZOBRIST_SEED = 0x9E3779B97F4A7C15ULL;

    Key pieceSquare[PIECE_SIZE][SQUARE_SIZE_64];
    Key enpassantSquare[FILE_SIZE];
    Key castlingRight[CASTLING_POSIBILITIES];
    Key blackMoves;

}

const int CASTLE_PERSMISION_UPDATES[SQUARE_SIZE_64] = {
    13, 15, 15, 15, 12, 15, 15, 14, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
    15, 15, 15, 15, 15, 15, 15, 15, 
     7, 15, 15, 15,  3, 15, 15, 11,
};
    
std::ostream& operator<<(std::ostream& os, const Position& pos) {

    os << "+---+---+---+---+---+---+---+---+" << std::endl;
    for (Rank rank = RANK_8; rank >= RANK_1; --rank)
    {
        os << "| ";
        for (File file = FILE_A; file <= FILE_H; ++file)
        {
            Square64 square = make_square64(rank, file);
            os << PIECE_NAMES[pos.get_mailbox_piece(square)] << " | ";
        }

        os << rank + 1 << std::endl << "+---+---+---+---+---+---+---+---+" << std::endl; 
    }

    os << "  a   b   c   d   e   f   g   h" << std::endl;

    os << "Fen: " << pos.get_FEN() << std::endl;

    os << "Key: " << pos.get_key() << std::endl;

    return os; 

}

void Position::init(){
    zobris_prng();
}

void Position::clear_position(){
    clear_position_info();
    clear_pieceTypes_bitboards();
    clear_occupied_bitboards();
    clear_mailbox();
}

void Position::clear_position_info(){

    sideToMove = COLOR_NC;

    ply = 1;

    historyPly[ply-1].nextMove = 0;
    historyPly[ply-1].castlingRight = NO_RIGHT;
    historyPly[ply-1].fiftyMovesCounter = 0;
    historyPly[ply-1].movesCounter = 1;
    historyPly[ply-1].enpassantSquare = SQ64_NO_SQUARE;
    historyPly[ply-1].positionKey = 0;

    materialScore[WHITE] = 0;
    materialScore[BLACK] = 0;
    materialScore[COLOR_NC] = 0;

}

void Position::clear_pieceTypes_bitboards(){
    //clear pieceTypes bitboards
    for(int c = 0; c < COLOR_SIZE; ++c){
        for(int p = 0; p < PIECETYPE_SIZE; ++p){
            pieceTypesBitboards[c][p] = ZERO;
        }
    }
}

void Position::clear_occupied_bitboards(){
    //clear occupied bitboards
    for(int c = 0; c < COLOR_SIZE; ++c){
        occupiedBitboards[c] = ZERO;
        
    }
}

void Position::clear_mailbox(){
    //clear mailbox
    for(int i = 0; i < SQUARE_SIZE_64; ++i){
        mailbox[i] = NO_PIECE;
    }
}

void Position::zobris_prng(){

   	std::mt19937_64 e2(Zobrist::ZOBRIST_SEED);
    std::uniform_int_distribution<Key> dist(
    		std::llround(std::pow(2,61)),
    		std::llround(std::pow(2,62)));

    // Initializes a random key for each piece on each square
	for (int piece_type = 0; piece_type < PIECE_SIZE; piece_type++) {
		for (int square = 0; square < SQUARE_SIZE_64; square++)
			Zobrist::pieceSquare[piece_type][square] = dist(e2);
	}

    // Initializes a random key for an enpassant square on each file
	for (int file = FILE_A; file <= FILE_H; file++) {
		Zobrist::enpassantSquare[file] = dist(e2);
	}

	for (int i = 0; i < 16; i++) {
		Zobrist::castlingRight[i] = dist(e2);
	}

	Zobrist::blackMoves = dist(e2);
}

void Position::set_FEN(std::string fenNotation){

    std::string field;

    clear_position();

    Rank rank = RANK_8;
    File file = FILE_A;
    

    std::istringstream iss(fenNotation);

    //1.- Set pieces on board
    iss >> field;

    for(char token : field){
        
        if(isdigit(token)){
            file += (token - '0');
        }
        else if(token == '/'){
            file = FILE_A;
            --rank;
        }
        else{
            add_piece(make_square64(rank, file), Piece(PIECE_NAMES.find(token)));
            ++file;
        }
    }

    //2.- Set side to move
    iss >> field;

    sideToMove = field == "w" ? WHITE : BLACK;


    //3.- Set castling rights
    iss >> field;
    
    for(char token : field)
    {
        switch (token)
        {
		case 'K': historyPly[ply-1].castlingRight |= WKCA; break;
		case 'Q': historyPly[ply-1].castlingRight |= WQCA; break;
		case 'k': historyPly[ply-1].castlingRight |= BKCA; break;
		case 'q': historyPly[ply-1].castlingRight |= BQCA; break;
        }
    }

    //4.- Set enpassant square
    iss >> field;
    
    if(field != "-")
    {
        File enpassantFile = File(field[0] - 97);
        Rank enpassantRank = Rank(field[1]  - 49);
	    historyPly[ply-1].enpassantSquare = make_square64(enpassantRank, enpassantFile);
    }

    //5.- Set fifty moves counter
    iss >> historyPly[ply-1].fiftyMovesCounter;

    //6.- Set move counter
    iss >>  historyPly[ply-1].movesCounter;

    calc_key();

}

std::string Position::get_FEN() const{

    std::ostringstream oss;
    
    for (Rank rank = RANK_8; rank >= RANK_1; --rank)
    {
        int emptySquaresCounter;
        for (File file = FILE_A; file <= FILE_H; ++file)
        {
            Square64 square = make_square64(rank, file);
            for(emptySquaresCounter = 0; mailbox[square] == NO_PIECE && file <= FILE_H; ++file){
                ++emptySquaresCounter;
                square = make_square64(rank, file + 1);                    
            }

            if(emptySquaresCounter)
                oss << emptySquaresCounter;
            
            oss << PIECE_NAMES[mailbox[square]];
        }

        if(rank > RANK_1)
            oss << "/";
    }

    oss << (sideToMove == WHITE ? " w " : " b ");

    CastlingRight castlingRights = get_castling_right();

    if(castlingRights & CastlingRight::WKCA)
        oss << 'K';

    if(castlingRights & CastlingRight::WQCA)
        oss << 'Q';

    if(castlingRights & CastlingRight::BKCA)
        oss << 'k';
        
    if(castlingRights & CastlingRight::BQCA)
        oss << 'q';
    
    if(castlingRights == CastlingRight::NO_RIGHT)
        oss << '-';

    if(get_enpassant_square() == SQ64_NO_SQUARE)
        oss << " - ";
    else
         oss << " " << SQUARE_NAMES[get_enpassant_square()];

    oss << " " << get_fifty_moves_counter();
    
    oss << " " << get_moves_counter();

    return oss.str();
}

void Position::calc_material_score(){

    for(Square64 sq64 = SQ64_A1; sq64 < SQUARE_SIZE_64; ++sq64){
        if(mailbox[sq64] != NO_PIECE){
            if( piece_color(mailbox[sq64]) ==  WHITE)
                materialScore[WHITE] += Evaluate::calc_material_table(mailbox[sq64], sq64);
            else    
                materialScore[BLACK] += Evaluate::calc_material_table(mailbox[sq64], sq64);
        }
    }

}

void Position::calc_key(){
    
    historyPly[ply-1].positionKey ^= Zobrist::castlingRight[historyPly[ply-1].castlingRight];

    if(historyPly[ply-1].enpassantSquare != SQ64_NO_SQUARE)
        historyPly[ply-1].positionKey ^= Zobrist::enpassantSquare[square_file(historyPly[ply-1].enpassantSquare)];
     
    if(sideToMove == BLACK)
        historyPly[ply-1].positionKey ^= Zobrist::blackMoves;

}

bool Position::is_repetition() const {

    for(int i = ply-1 - historyPly[ply-1].fiftyMovesCounter; i < ply - 1; ++i){

        if(historyPly[ply-1].positionKey == historyPly[i].positionKey){
            return true;
        }
    }
    return false;
}



const int DIRECTION_SIDES = 4;
const Direction sizes[DIRECTION_SIDES] = {EAST, WEST, NORTH, SOUTH};
const Direction diagonals[DIRECTION_SIDES] = {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST};


bool Position::square_is_attacked(Square64 sq64) const{

    return   (Attacks::pawnAttacks[sideToMove][sq64] & pieceTypesBitboards[~sideToMove][PAWN])
           | (Attacks::knightAttacks[sq64] & pieceTypesBitboards[~sideToMove][KNIGHT])
           | (Attacks::kingAttacks[sq64] &  pieceTypesBitboards[~sideToMove][KING])
           | (Attacks::sliding_diagonal_attacks(occupiedBitboards[COLOR_NC], sq64) & (pieceTypesBitboards[~sideToMove][BISHOP] | pieceTypesBitboards[~sideToMove][QUEEN]))
           | (Attacks::sliding_side_attacks(occupiedBitboards[COLOR_NC], sq64) & (pieceTypesBitboards[~sideToMove][ROOK] | pieceTypesBitboards[~sideToMove][QUEEN]));
}


bool Position::do_move(Move move){

    Square64 from = move_from(move);
    Square64 to = move_to(move);
    SpecialMove specialMove = move_special(move);

    //Set move to the move history
    historyPly[ply-1].nextMove = move;

    ++ply;
    historyPly[ply-1].positionKey = historyPly[ply-2].positionKey;
    historyPly[ply-1].positionKey ^= Zobrist::castlingRight[historyPly[ply-2].castlingRight];

    if(specialMove != SpecialMove::NO_SPECIAL){
        if(specialMove == SpecialMove::ENPASSANT){
            historyPly[ply-1].positionKey ^= Zobrist::enpassantSquare[square_file(historyPly[ply-2].enpassantSquare)];
            if(sideToMove==Color::WHITE){
                remove_piece(Square64(to+Direction::SOUTH));
            }else{
                remove_piece(Square64(to+Direction::NORTH));
            }
        }
        else if(specialMove == SpecialMove::CASTLE){
            switch (to)
            {
            case Square64::SQ64_C1:
                move_piece(Square64::SQ64_A1, Square64::SQ64_D1);
                break;
            case Square64::SQ64_G1:
                move_piece(Square64::SQ64_H1, Square64::SQ64_F1);
                break;
            case Square64::SQ64_C8:
                move_piece(Square64::SQ64_A8, Square64::SQ64_D8);
                break;
            case Square64::SQ64_G8:
                move_piece(Square64::SQ64_H8, Square64::SQ64_F8);
                break;
            default:
                break;
            }
        }
    }
    
    //Set in the new history

    //Castling rights
    historyPly[ply-1].castlingRight = historyPly[ply-2].castlingRight & CASTLE_PERSMISION_UPDATES[from];
    historyPly[ply-1].castlingRight &= CASTLE_PERSMISION_UPDATES[to];
    historyPly[ply-1].positionKey ^= Zobrist::castlingRight[historyPly[ply-1].castlingRight];
    
    //Set next move to empty
    historyPly[ply-1].nextMove = 0;

    //Set fifty moves counter
    Piece capturedPiece = captured_piece(move);
    historyPly[ply-1].fiftyMovesCounter = historyPly[ply-2].fiftyMovesCounter+1;

    if(capturedPiece != Piece::NO_PIECE){
        remove_piece(to);
        historyPly[ply-1].fiftyMovesCounter = 0;
    }

    //If black move, add 1 to moves counter
    historyPly[ply-1].movesCounter = historyPly[ply-2].movesCounter;
    if(sideToMove==Color::BLACK)
        historyPly[ply-1].movesCounter = historyPly[ply-2].movesCounter+1;

    //Set enpassant square
    historyPly[ply-1].enpassantSquare = Square64::SQ64_NO_SQUARE;
    if(piece_type(mailbox[from]) == PieceType::PAWN){
        historyPly[ply-1].fiftyMovesCounter = 0;
        
        if(sideToMove==Color::WHITE && specialMove == SpecialMove::PAWN_START){
            historyPly[ply-1].enpassantSquare = Square64(from + Direction::NORTH);
            historyPly[ply-1].positionKey ^= Zobrist::enpassantSquare[square_file(historyPly[ply-1].enpassantSquare)];
        } else if(sideToMove==Color::BLACK && specialMove == SpecialMove::PAWN_START){
            historyPly[ply-1].enpassantSquare = Square64(from + Direction::SOUTH);
            historyPly[ply-1].positionKey ^= Zobrist::enpassantSquare[square_file(historyPly[ply-1].enpassantSquare)];
        }
    }

    move_piece(from, to);
    

    PieceType promPieceType = promoted_piece(move);

    if(promPieceType != PieceType::NO_PIECE_TYPE){
        Piece promPiece = make_piece(sideToMove, promPieceType);
        remove_piece(to);
        add_piece(to, promPiece);
    }

    Bitboard kingBitboard = pieceTypesBitboards[sideToMove][KING];
    Square64 kingsq64{__builtin_ctzll(kingBitboard)};

    if(square_is_attacked(kingsq64)){
        //VIDEO
        sideToMove =~ sideToMove;
        historyPly[ply-1].positionKey ^= Zobrist::blackMoves;
        undo_move();
        return false;
    }

    sideToMove =~ sideToMove;
    historyPly[ply-1].positionKey ^= Zobrist::blackMoves;

    return true;
}

void Position::undo_move(){
    
    Move move = historyPly[ply-2].nextMove;
    Square64 from = move_from(move);
    Square64 to = move_to(move);
    SpecialMove specialMove = move_special(move);

    sideToMove =~ sideToMove;
    

    if(specialMove != SpecialMove::NO_SPECIAL){
        if(specialMove == SpecialMove::ENPASSANT){
            if(sideToMove==Color::WHITE){
                add_piece(Square64(to + Direction::SOUTH), Piece::B_PAWN);
            }else{
                add_piece(Square64(to + Direction::NORTH), Piece::W_PAWN);
            }
        }
        if(specialMove == SpecialMove::CASTLE){
            switch (to)
            {
            case Square64::SQ64_C1:
                move_piece(Square64::SQ64_D1, Square64::SQ64_A1);
                break;
            case Square64::SQ64_G1:
                move_piece(Square64::SQ64_F1, Square64::SQ64_H1);
                break;
            case Square64::SQ64_C8:
                move_piece(Square64::SQ64_D8, Square64::SQ64_A8);
                break;
            case Square64::SQ64_G8:
                move_piece(Square64::SQ64_F8, Square64::SQ64_H8);
                break;
            default:
                break;
            }
        }
    }

    move_piece(to, from);

    Piece capturedPiece = captured_piece(move);
    if(capturedPiece != Piece::NO_PIECE){
        add_piece(to, capturedPiece);
    }

    PieceType promPieceType = promoted_piece(move);

    if(promPieceType != PieceType::NO_PIECE_TYPE){
        remove_piece(from);
        add_piece(from, sideToMove == Color::WHITE ? Piece::W_PAWN : Piece::B_PAWN);
    }

    historyPly[ply-2].nextMove = 0;
    historyPly[ply-1].castlingRight = NO_RIGHT;
    historyPly[ply-1].fiftyMovesCounter = 0;
    historyPly[ply-1].movesCounter = 0;
    historyPly[ply-1].enpassantSquare = Square64::SQ64_NO_SQUARE;
    historyPly[ply-1].positionKey = 0;

    --ply;

}

void Position::move_piece(Square64 from, Square64 to){

    Piece piece = mailbox[from];
    mailbox[from] =  NO_PIECE;
    mailbox[to] =  piece;
    PieceType pieceType = piece_type(piece);
    Color pieceColor = piece_color(piece);

    pieceTypesBitboards[pieceColor][pieceType] = Bitboards::clear_pieces(pieceTypesBitboards[pieceColor][pieceType], from);
    occupiedBitboards[pieceColor] = Bitboards::clear_pieces(occupiedBitboards[pieceColor], from);
    occupiedBitboards[Color::COLOR_NC] = Bitboards::clear_pieces(occupiedBitboards[Color::COLOR_NC],from);

    pieceTypesBitboards[pieceColor][pieceType] = Bitboards::set_pieces(pieceTypesBitboards[pieceColor][pieceType], to);
    occupiedBitboards[pieceColor] = Bitboards::set_pieces(occupiedBitboards[pieceColor], to);
    occupiedBitboards[Color::COLOR_NC] = Bitboards::set_pieces(occupiedBitboards[Color::COLOR_NC],to);

    //Update piece value from material
    materialScore[pieceColor] -= Evaluate::calc_material_table(piece, from);
    materialScore[pieceColor] += Evaluate::calc_material_table(piece, to);

    //Update key
    historyPly[ply-1].positionKey ^= Zobrist::pieceSquare[piece][from];
    historyPly[ply-1].positionKey ^= Zobrist::pieceSquare[piece][to];

}

void Position::remove_piece(Square64 square){

    Piece piece = mailbox[square];
    mailbox[square] = NO_PIECE;
    Color pieceColor = piece_color(piece);
    PieceType pieceType = piece_type(piece);

    pieceTypesBitboards[pieceColor][pieceType] = Bitboards::clear_pieces(pieceTypesBitboards[pieceColor][pieceType], square);
    occupiedBitboards[pieceColor] = Bitboards::clear_pieces(occupiedBitboards[pieceColor], square);
    occupiedBitboards[Color::COLOR_NC] = Bitboards::clear_pieces(occupiedBitboards[Color::COLOR_NC], square);

    //Remove piece value from material 
    materialScore[pieceColor] -= Evaluate::calc_material_table(piece, square);

    //Update key
    historyPly[ply-1].positionKey ^= Zobrist::pieceSquare[piece][square];
}

void Position::add_piece(Square64 square, Piece piece){

    Color pieceColor = piece_color(piece);
    PieceType pieceType = piece_type(piece);

    mailbox[square] = piece;
    pieceTypesBitboards[pieceColor][pieceType] = Bitboards::set_pieces(pieceTypesBitboards[pieceColor][pieceType], square);
    occupiedBitboards[pieceColor] = Bitboards::set_pieces(occupiedBitboards[pieceColor], square);
    occupiedBitboards[Color::COLOR_NC] = Bitboards::set_pieces(occupiedBitboards[Color::COLOR_NC], square);

    //Add piece value from material 
    materialScore[pieceColor] += Evaluate::calc_material_table(piece, square);
    
    //Update key
    historyPly[ply-1].positionKey ^= Zobrist::pieceSquare[piece][square];

}

} // namespace Xake
