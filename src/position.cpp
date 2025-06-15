#include "position.h"

#include <iostream>
#include <random>

namespace Xake
{

namespace Zobrist{

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
            PieceType pieceType{NO_PIECE_TYPE};
            Color color{COLOR_NC};
            if(pos.get_mailbox_pieceType(WHITE, square) != NO_PIECE_TYPE){
                pieceType = pos.get_mailbox_pieceType(WHITE, square);
                color = WHITE;
            }else if(pos.get_mailbox_pieceType(BLACK, square) != NO_PIECE_TYPE){
                pieceType = pos.get_mailbox_pieceType(BLACK, square);
                color = BLACK;
            }

            os << PIECE_NAMES[make_piece(color, pieceType)] << " | ";

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

void Position::clean_position(){

    clean_mailbox();
    clear_position_info();

}

void Position::clean_mailbox(){
    //clean mailbox
    for(std::size_t c = 0; c < COLOR_SIZE; ++c){
        for(std::size_t i = 0; i < SQUARE_SIZE_64; ++i){
            mailbox[c][i] = NO_PIECE_TYPE;
        }
    }
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

void Position::zobris_prng(){
    std::random_device rd;

    std::mt19937_64 e2(rd());

    std::uniform_int_distribution<Key> dist;

    for(int p = 0; p < PIECE_SIZE; ++p)
        for(int s = 0; s < SQUARE_SIZE_64; ++s)
            Zobrist::pieceSquare[p][s] = dist(e2);

    for(int f = 0; f < FILE_SIZE; ++f)
        Zobrist::enpassantSquare[f] = dist(e2);

    for(int c = 0; c < CASTLING_POSIBILITIES; ++c)
        Zobrist::castlingRight[c] = dist(e2);
        
    Zobrist::blackMoves = dist(e2);
}

void Position::set_FEN(std::string fenNotation){

    std::string field;

    clean_position();

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
            PieceType pieceType{NO_PIECE_TYPE};
            for(emptySquaresCounter = 0; mailbox[COLOR_NC][square] == NO_PIECE_TYPE && file <= FILE_H;++file){
                ++emptySquaresCounter;
                square = make_square64(rank, file + 1);                    
            }

            if(emptySquaresCounter)
                oss << emptySquaresCounter;
            
            if(mailbox[WHITE][square] != NO_PIECE_TYPE){
                pieceType = mailbox[WHITE][square];
                oss << PIECE_NAMES[make_piece(WHITE, pieceType)];

            }else if(mailbox[BLACK][square] != NO_PIECE_TYPE){
                pieceType = mailbox[BLACK][square];
                oss << PIECE_NAMES[make_piece(BLACK, pieceType)];
            }
        }

        if(rank > RANK_1)
            oss << "/";
    }

    oss << (sideToMove == WHITE ? " w " : " b ");

    int castlingRights = get_castling_right();

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

    for(std::size_t sq = 0; sq < SQUARE_SIZE_64; ++sq){
        if(mailbox[WHITE][sq] != NO_PIECE_TYPE)
            materialScore[WHITE] += Evaluate::calc_material_table(make_piece(WHITE, mailbox[WHITE][sq]), Square64(sq));
    }

    for(std::size_t sq = 0; sq < SQUARE_SIZE_64; ++sq){
        if(mailbox[BLACK][sq] != NO_PIECE_TYPE)
            materialScore[BLACK] += Evaluate::calc_material_table(make_piece(BLACK, mailbox[BLACK][sq]), Square64(sq));
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



const std::size_t DIRECTION_SIDES = 4;
const Direction sizes[DIRECTION_SIDES] = {EAST, WEST, NORTH, SOUTH};
const Direction diagonals[DIRECTION_SIDES] = {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST};

/*
bool Position::square_is_attacked(Square64 square) const{

    //Pawns
    if(sideToMove==Color::WHITE){
        if(mailbox[Color::BLACK][Square64(square+NORTH_EAST)] == PieceType::PAWN || mailbox[Color::BLACK][Square64(square+NORTH_WEST)] == PieceType::PAWN){
            return true;
        }
    }
    if(sideToMove==Color::BLACK){
        if(mailbox[Color::WHITE][Square64(square+SOUTH_EAST)] == PieceType::PAWN || mailbox[Color::WHITE][Square64(square+SOUTH_WEST)] == PieceType::PAWN){
            return true;
        }
    }

    //Knights
    bool isKnightAttack =   (mailbox[~sideToMove][Square64(square + NORTH_NORTH_WEST )] == PieceType::KNIGHT || mailbox[~sideToMove][Square64(square+NORTH_NORTH_EAST )] == PieceType::KNIGHT ||
                             mailbox[~sideToMove][Square64(square + NORTH_WEST_WEST  )] == PieceType::KNIGHT || mailbox[~sideToMove][Square64(square+NORTH_EAST_EAST  )] == PieceType::KNIGHT ||
                             mailbox[~sideToMove][Square64(square + SOUTH_WEST_WEST  )] == PieceType::KNIGHT || mailbox[~sideToMove][Square64(square+SOUTH_EAST_EAST  )] == PieceType::KNIGHT ||
                             mailbox[~sideToMove][Square64(square + SOUTH_SOUTH_WEST )] == PieceType::KNIGHT || mailbox[~sideToMove][Square64(square+SOUTH_SOUTH_EAST )] == PieceType::KNIGHT) 
                            ? true : false;
    if(isKnightAttack){
        return true;
    }

    //bishops, queens
    for(Direction dir : diagonals){
        int toSquareIndex = square + dir;
        Square64 toSquare = SQUARES_120[toSquareIndex];
        while(toSquare != Square120::SQ120_OFFBOARD && mailbox[Color::COLOR_NC][toSquare] == NO_PIECE_TYPE) {
            toSquareIndex += dir;
            toSquare = SQUARES_120[toSquareIndex];
        }
        if(mailbox[~sideToMove][toSquare] == PieceType::BISHOP || mailbox[~sideToMove][toSquare] == PieceType::QUEEN){
            return true;
        }
    }

    //rooks, queens
    for(Direction dir : sizes){
        int toSquareIndex = square + dir;
        Square120 toSquare = SQUARES_120[toSquareIndex];
        while(toSquare != Square120::SQ120_OFFBOARD && mailbox[Color::COLOR_NC][toSquare] == NO_PIECE_TYPE) {
            toSquareIndex += dir;
            toSquare = SQUARES_120[toSquareIndex];
        }
        if(mailbox[~sideToMove][toSquare] == PieceType::ROOK || mailbox[~sideToMove][toSquare] == PieceType::QUEEN){
            return true;
        }
    }

    //king
    for(Direction dir : diagonals){
        int toSquareIndex = square + dir;
        Square120 toSquare = SQUARES_120[toSquareIndex];
        if(mailbox[~sideToMove][toSquare] == PieceType::KING){
            return true;
        }
    }
    for(Direction dir : sizes){
        int toSquareIndex = square + dir;
        Square120 toSquare = SQUARES_120[toSquareIndex];
        if(mailbox[~sideToMove][toSquare] == PieceType::KING){
            return true;
        }
    }

    return false;
} 
*/
/*
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
        if(specialMove == SpecialMove::CASTLE){
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
    if(mailbox[Color::COLOR_NC][from] == PieceType::PAWN){
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

    Piece kingS = make_piece(sideToMove, PieceType::KING);
    if(square_is_attacked(pieceList[kingS][0])){
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
*/
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

    Color pieceColor{Color::COLOR_NC};
    PieceType pieceType{PieceType::NO_PIECE_TYPE};

    if(mailbox[Color::WHITE][from] != PieceType::NO_PIECE_TYPE){
        pieceType = mailbox[Color::WHITE][from];
        mailbox[Color::WHITE][from] = PieceType::NO_PIECE_TYPE;
        mailbox[Color::WHITE][to] = pieceType;
        pieceColor = Color::WHITE;
    }else if(mailbox[Color::BLACK][from] != PieceType::NO_PIECE_TYPE){
        pieceType = mailbox[Color::BLACK][from];
        mailbox[Color::BLACK][from] = PieceType::NO_PIECE_TYPE;
        mailbox[Color::BLACK][to] = pieceType;
        pieceColor = Color::BLACK;
    }else{
        pieceColor = Color::COLOR_NC;
    }

    mailbox[Color::COLOR_NC][from] = PieceType::NO_PIECE_TYPE;
    mailbox[Color::COLOR_NC][to] = pieceType;

    Piece piece = make_piece(pieceColor, pieceType);

    //Update piece value from material
    materialScore[pieceColor] -= Evaluate::calc_material_table(piece, from);
    materialScore[pieceColor] += Evaluate::calc_material_table(piece, to);

    //Update key
    historyPly[ply-1].positionKey ^= Zobrist::pieceSquare[piece][from];
    historyPly[ply-1].positionKey ^= Zobrist::pieceSquare[piece][to];

}

//TODO usar templates para evitar if else
void Position::remove_piece(Square64 square){

    Color pieceColor{Color::COLOR_NC};
    PieceType pieceType{PieceType::NO_PIECE_TYPE};

    // Check witch piece is located on the square of each color mailbox
    if(mailbox[Color::WHITE][square] != PieceType::NO_PIECE_TYPE){
        //Save piece type
        pieceType = mailbox[Color::WHITE][square];
        //Remove piece from corresponding mailbox
        mailbox[Color::WHITE][square] = PieceType::NO_PIECE_TYPE;
        //Save piece color
        pieceColor = Color::WHITE;
    }else if(mailbox[Color::BLACK][square] != PieceType::NO_PIECE_TYPE){
        pieceType = mailbox[Color::BLACK][square];
        mailbox[Color::BLACK][square] = PieceType::NO_PIECE_TYPE;
        pieceColor = Color::BLACK;
    }else{
        pieceColor = Color::COLOR_NC;
    }

    //Update both color mailbox
    mailbox[Color::COLOR_NC][square] = PieceType::NO_PIECE_TYPE;

    //Knowing the piece color and piece type, make piece
    Piece piece = make_piece(pieceColor, pieceType);

    //Remove piece value from material 
    materialScore[pieceColor] -= Evaluate::calc_material_table(piece, square);

    //Update key
    historyPly[ply-1].positionKey ^= Zobrist::pieceSquare[piece][square];
}

void Position::add_piece(Square64 square, Piece piece){

    Color pieceColor = piece_color(piece);
    PieceType pieceType = piece_type(piece);
    mailbox[pieceColor][square] = pieceType;
    mailbox[Color::COLOR_NC][square] = pieceType;

    //Add piece value from material 
    materialScore[pieceColor] += Evaluate::calc_material_table(piece, square);
    
    //Update key
    historyPly[ply-1].positionKey ^= Zobrist::pieceSquare[piece][square];

}

} // namespace Xake
