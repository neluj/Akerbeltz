#include "position.h"

#include <iostream>
#include <random>

namespace Xake
{

namespace Zobrist{

    Key pieceSquare[PIECE_SIZE][SQUARE_SIZE_120];
    Key enpassantSquare[FILE_SIZE];
    Key castlingRight[CASTLING_POSIBILITIES];
    Key blackMoves;

}

const int CASTLE_PERSMISION_UPDATES[SQUARE_SIZE_120] = {
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 13, 15, 15, 15, 12, 15, 15, 14, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15,  7, 15, 15, 15,  3, 15, 15, 11, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15
};
    
std::ostream& operator<<(std::ostream& os, const Position& pos) {

    os << "+---+---+---+---+---+---+---+---+" << std::endl;
    for (int rank = RANK_8; rank >= RANK_1; --rank)
    {
        os << "| ";
        for (int file = FILE_A; file <= FILE_H; ++file)
        {
            Square120 square = Square120(((rank + 2) * 10) + (file +1));
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
    clean_piece_list();
    clear_position_info();

}

void Position::clean_mailbox(){
    //clean mailbox
    for(std::size_t c = 0; c < COLOR_SIZE; ++c){
        for(std::size_t i = 0; i < SQUARE_SIZE_120; ++i){
            mailbox[c][i] = NO_PIECE_TYPE;
        }
    }
}

void Position::clean_piece_list(){
    //clean piece list
    for(std::size_t i = 0; i < PIECE_SIZE; ++i){
        pieceCounter[i] = 0;
    }
    for(std::size_t pt = 0; pt < PIECE_SIZE; ++pt){
        for(std::size_t pn = 0; pn < MAX_SAME_PIECE; ++pn){
            pieceList[pt][pn] = SQ120_NO_SQUARE;
        }    
    }
}

void Position::clear_position_info(){

    sideToMove = COLOR_NC;

    historySize = 1;

    history[historySize-1].nextMove = 0;
    history[historySize-1].castlingRight = NO_RIGHT;
    history[historySize-1].fiftyMovesCounter = 0;
    history[historySize-1].movesCounter = 1;
    history[historySize-1].enpassantSquare = SQ120_NO_SQUARE;
    history[historySize-1].positionKey = 0;

    materialScore[WHITE] = 0;
    materialScore[BLACK] = 0;
    materialScore[COLOR_NC] = 0;

}

void Position::zobris_prng(){
    std::random_device rd;

    std::mt19937_64 e2(rd());

    std::uniform_int_distribution<Key> dist;

    for(int p = 0; p < PIECE_SIZE; ++p)
        for(int s = 0; s < SQUARE_SIZE_120; ++s)
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
            add_piece(square120(rank, file), Piece(PIECE_NAMES.find(token)));
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
		case 'K': history[historySize-1].castlingRight |= WKCA; break;
		case 'Q': history[historySize-1].castlingRight |= WQCA; break;
		case 'k': history[historySize-1].castlingRight |= BKCA; break;
		case 'q': history[historySize-1].castlingRight |= BQCA; break;
        }
    }

    //4.- Set enpassant square
    iss >> field;
    
    if(field != "-")
    {
        File enpassantFile = File(field[0] - 97);
        Rank enpassantRank = Rank(field[1]  - 49);
	    history[historySize-1].enpassantSquare = square120(enpassantRank, enpassantFile);
    }

    //5.- Set fifty moves counter
    iss >> history[historySize-1].fiftyMovesCounter;

    //6.- Set move counter
    iss >>  history[historySize-1].movesCounter;

    calc_key();

}

std::string Position::get_FEN() const{

    std::ostringstream oss;
    
    for (Rank rank = RANK_8; rank >= RANK_1; --rank)
    {
        int emptySquaresCounter;
        for (File file = FILE_A; file <= FILE_H; ++file)
        {
            Square120 square = square120(rank, file);
            PieceType pieceType{NO_PIECE_TYPE};
            for(emptySquaresCounter = 0; mailbox[COLOR_NC][square] == NO_PIECE_TYPE && file <= FILE_H;++file){
                ++emptySquaresCounter;
                square = square120(rank, file + 1);                    
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

    if(get_enpassant_square() == SQ120_NO_SQUARE)
        oss << " - ";
    else
         oss << " " << SQUARE_NAMES[get_enpassant_square()];

    oss << " " << get_fifty_moves_counter();
    
    oss << " " << get_moves_counter();

    return oss.str();
}

void Position::calc_material_score(){

    for(std::size_t sq = 0; sq < SQUARE_SIZE_120; ++sq){
        if(mailbox[WHITE][sq] != NO_PIECE_TYPE)
            materialScore[WHITE] += Evaluate::calc_material_table(WHITE, mailbox[WHITE][sq], Square120(sq));
    }

    for(std::size_t sq = 0; sq < SQUARE_SIZE_120; ++sq){
        if(mailbox[BLACK][sq] != NO_PIECE_TYPE)
            materialScore[BLACK] += Evaluate::calc_material_table(BLACK, mailbox[BLACK][sq], Square120(sq));
    }

}

void Position::calc_key(){
    
    history[historySize-1].positionKey ^= Zobrist::castlingRight[history[historySize-1].castlingRight];

    if(history[historySize-1].enpassantSquare != SQ120_NO_SQUARE)
        history[historySize-1].positionKey ^= Zobrist::enpassantSquare[square_file(history[historySize-1].enpassantSquare)];
     
    if(sideToMove == BLACK)
        history[historySize-1].positionKey ^= Zobrist::blackMoves;

}

bool Position::is_repetition() const {

    for(int i = historySize-1 - history[historySize-1].fiftyMovesCounter; i < historySize - 1; ++i){

        if(history[historySize-1].positionKey == history[i].positionKey){
            return true;
        }
    }
    return false;
}



const std::size_t DIRECTION_SIDES = 4;
const Direction sizes[DIRECTION_SIDES] = {EAST, WEST, NORTH, SOUTH};
const Direction diagonals[DIRECTION_SIDES] = {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST};


bool Position::square_is_attacked(Square120 square) const{

    //Pawns
    if(sideToMove==Color::WHITE){
        if(mailbox[Color::BLACK][Square120(square+NORTH_EAST)] == PieceType::PAWN || mailbox[Color::BLACK][Square120(square+NORTH_WEST)] == PieceType::PAWN){
            return true;
        }
    }
    if(sideToMove==Color::BLACK){
        if(mailbox[Color::WHITE][Square120(square+SOUTH_EAST)] == PieceType::PAWN || mailbox[Color::WHITE][Square120(square+SOUTH_WEST)] == PieceType::PAWN){
            return true;
        }
    }

    //Knights
    bool isKnightAttack =   (mailbox[~sideToMove][Square120(square+NORTH_NORTH_WEST)] == PieceType::KNIGHT || mailbox[~sideToMove][Square120(square+NORTH_NORTH_EAST)] == PieceType::KNIGHT ||
                            mailbox[~sideToMove][Square120(square+NORTH_WEST_WEST)] == PieceType::KNIGHT  || mailbox[~sideToMove][Square120(square+NORTH_EAST_EAST)] == PieceType::KNIGHT  ||
                            mailbox[~sideToMove][Square120(square+SOUTH_WEST_WEST)] == PieceType::KNIGHT  || mailbox[~sideToMove][Square120(square+SOUTH_EAST_EAST)] == PieceType::KNIGHT  ||
                            mailbox[~sideToMove][Square120(square+SOUTH_SOUTH_WEST)] == PieceType::KNIGHT || mailbox[~sideToMove][Square120(square+SOUTH_SOUTH_EAST)] == PieceType::KNIGHT) 
                            ? true : false;
    if(isKnightAttack){
        return true;
    }

    //bishops, queens
    for(Direction dir : diagonals){
        int toSquareIndex = square + dir;
        Square120 toSquare = SQUARES_120[toSquareIndex];
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

bool Position::do_move(Move move){

    Square120 from = move_from(move);
    Square120 to = move_to(move);
    SpecialMove specialMove = move_special(move);

    //Set move to the move history
    history[historySize-1].nextMove = move;

    ++historySize;
    history[historySize-1].positionKey = history[historySize-2].positionKey;
    history[historySize-1].positionKey ^= Zobrist::castlingRight[history[historySize-2].castlingRight];

    if(specialMove != SpecialMove::NO_SPECIAL){
        if(specialMove == SpecialMove::ENPASSANT){
            history[historySize-1].positionKey ^= Zobrist::enpassantSquare[square_file(history[historySize-2].enpassantSquare)];
            if(sideToMove==Color::WHITE){
                remove_piece(Square120(to+Direction::SOUTH));
            }else{
                remove_piece(Square120(to+Direction::NORTH));
            }
        }
        if(specialMove == SpecialMove::CASTLE){
            switch (to)
            {
            case Square120::SQ120_C1:
                move_piece(Square120::SQ120_A1, Square120::SQ120_D1);
                break;
            case Square120::SQ120_G1:
                move_piece(Square120::SQ120_H1, Square120::SQ120_F1);
                break;
            case Square120::SQ120_C8:
                move_piece(Square120::SQ120_A8, Square120::SQ120_D8);
                break;
            case Square120::SQ120_G8:
                move_piece(Square120::SQ120_H8, Square120::SQ120_F8);
                break;
            default:
                break;
            }
        }
    }
    
    //Set in the new history

    //Castling rights
    history[historySize-1].castlingRight = history[historySize-2].castlingRight & CASTLE_PERSMISION_UPDATES[from];
    history[historySize-1].castlingRight &= CASTLE_PERSMISION_UPDATES[to];
    history[historySize-1].positionKey ^= Zobrist::castlingRight[history[historySize-1].castlingRight];
    
    //Set next move to empty
    history[historySize-1].nextMove = 0;

    //Set fifty moves counter
    Piece capturedPiece = captured_piece(move);
    history[historySize-1].fiftyMovesCounter = history[historySize-2].fiftyMovesCounter+1;

    if(capturedPiece != Piece::NO_PIECE){
        remove_piece(to);
        history[historySize-1].fiftyMovesCounter = 0;
    }

    //If black move, add 1 to moves counter
    history[historySize-1].movesCounter = history[historySize-2].movesCounter;
    if(sideToMove==Color::BLACK)
        history[historySize-1].movesCounter = history[historySize-2].movesCounter+1;

    //Set enpassant square
    history[historySize-1].enpassantSquare = Square120::SQ120_NO_SQUARE;
    if(mailbox[Color::COLOR_NC][from] == PieceType::PAWN){
        history[historySize-1].fiftyMovesCounter = 0;
        
        if(sideToMove==Color::WHITE && specialMove == SpecialMove::PAWN_START){
            history[historySize-1].enpassantSquare = Square120(from+10);
            history[historySize-1].positionKey ^= Zobrist::enpassantSquare[square_file(history[historySize-1].enpassantSquare)];
        } else if(sideToMove==Color::BLACK && specialMove == SpecialMove::PAWN_START){
            history[historySize-1].enpassantSquare = Square120(from-10);
            history[historySize-1].positionKey ^= Zobrist::enpassantSquare[square_file(history[historySize-1].enpassantSquare)];
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
        history[historySize-1].positionKey ^= Zobrist::blackMoves;
        undo_move();
        return false;
    }

    sideToMove =~ sideToMove;
    history[historySize-1].positionKey ^= Zobrist::blackMoves;

    return true;
}

void Position::undo_move(){
    
    Move move = history[historySize-2].nextMove;
    Square120 from = move_from(move);
    Square120 to = move_to(move);
    SpecialMove specialMove = move_special(move);

    sideToMove =~ sideToMove;
    

    if(specialMove != SpecialMove::NO_SPECIAL){
        if(specialMove == SpecialMove::ENPASSANT){
            if(sideToMove==Color::WHITE){
                add_piece(Square120(to+Direction::SOUTH), Piece::B_PAWN);
            }else{
                add_piece(Square120(to+Direction::NORTH), Piece::W_PAWN);
            }
        }
        if(specialMove == SpecialMove::CASTLE){
            switch (to)
            {
            case Square120::SQ120_C1:
                move_piece(Square120::SQ120_D1, Square120::SQ120_A1);
                break;
            case Square120::SQ120_G1:
                move_piece(Square120::SQ120_F1, Square120::SQ120_H1);
                break;
            case Square120::SQ120_C8:
                move_piece(Square120::SQ120_D8, Square120::SQ120_A8);
                break;
            case Square120::SQ120_G8:
                move_piece(Square120::SQ120_F8, Square120::SQ120_H8);
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

    history[historySize-2].nextMove = 0;
    history[historySize-1].castlingRight = NO_RIGHT;
    history[historySize-1].fiftyMovesCounter = 0;
    history[historySize-1].movesCounter = 0;
    history[historySize-1].enpassantSquare = Square120::SQ120_NO_SQUARE;
    history[historySize-1].positionKey = 0;

    --historySize;

}

void Position::move_piece(Square120 from, Square120 to){

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

    for(std::size_t i = 0; i < pieceCounter[piece]; ++i){
        
        if(pieceList[piece][i] == from){
            pieceList[piece][i] = to;
            break;
        }
    }

    //Update piece value from material
    materialScore[pieceColor] -= Evaluate::calc_material_table(pieceColor, pieceType, from);
    materialScore[pieceColor] += Evaluate::calc_material_table(pieceColor, pieceType, to);

    //Update key
    history[historySize-1].positionKey ^= Zobrist::pieceSquare[piece][from];
    history[historySize-1].positionKey ^= Zobrist::pieceSquare[piece][to];

}

//TODO usar templates para evitar if else
void Position::remove_piece(Square120 square){

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

    //Find on concret piece list until find the piece that contains the square, for remove it 
    for(std::size_t i = 0; i < pieceCounter[piece]; ++i){
        if(pieceList[piece][i] == square){
            pieceList[piece][i] = pieceList[piece][pieceCounter[piece]-1];
            pieceList[piece][pieceCounter[piece]-1] = Square120::SQ120_NO_SQUARE;
            --pieceCounter[piece];
            break;
        }
    }

    //Remove piece value from material 
    materialScore[pieceColor] -= Evaluate::calc_material_table(pieceColor, pieceType, square);

    //Update key
    history[historySize-1].positionKey ^= Zobrist::pieceSquare[piece][square];
}

void Position::add_piece(Square120 square, Piece piece){

    Color pieceColor = piece_color(piece);
    PieceType pieceType = piece_type(piece);
    mailbox[pieceColor][square] = pieceType;
    mailbox[Color::COLOR_NC][square] = pieceType;

    pieceList[piece][pieceCounter[piece]] = square;
    ++pieceCounter[piece];

    //Add piece value from material 
    materialScore[pieceColor] += Evaluate::calc_material_table(pieceColor, pieceType, square);
    
    //Update key
    history[historySize-1].positionKey ^= Zobrist::pieceSquare[piece][square];

}

} // namespace Xake
