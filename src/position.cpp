#include "position.h"
#include <ctype.h>
#include <iostream>

namespace Xake
{

const int CASTLE_PERSMISION_UPDATES[Xake::SQUARE_SIZE_120] = {
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
    
Position::Position(){
    init();
}

void Position::init(){

    //init mailbox
    for(std::size_t c = 0; c < COLOR_SIZE; ++c){
        for(std::size_t i = 0; i < SQUARE_SIZE_120; ++i){
            mailbox[c][i] = NO_PIECE_TYPE;
        }
    }
    for(std::size_t i = 0; i < PIECE_SIZE; ++i){
        pieceCounter[i] = 0;
    }
    for(std::size_t pt = 0; pt < PIECE_SIZE; ++pt){
        for(std::size_t pn = 0; pn < MAX_SAME_PIECE; ++pn){
            pieceList[pt][pn] = NO_SQUARE;
        }    
    }

    historySize = 1;

    history[historySize-1].nextMove = 0;
    history[historySize-1].castlingRight = 0;
    history[historySize-1].fiftyMovesCounter = 0;
    history[historySize-1].movesCounter = 0;
    history[historySize-1].enpassantSquare = Square::NO_SQUARE;
}

//TODO FEN could be shorter 
void Position::set_FEN(const std::string & fenNotation){

    init();

    int     rank    = RANK_8;
    int     file    = FILE_A;
        
    std::string::size_type charIndex = 0;
    char c = fenNotation[charIndex];
    while(!isspace(c))
    {   
        switch (c)
        {
		case('k'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = KING;
            pieceList[B_KING][pieceCounter[B_KING]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_KING];   
            ++file; 
            break;  

		case('P'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = PAWN;
            pieceList[W_PAWN][pieceCounter[W_PAWN]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_PAWN];   
            ++file; 
            break;

		case('R'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = ROOK;
            pieceList[W_ROOK][pieceCounter[W_ROOK]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_ROOK];   
            ++file; 
            break;

		case('N'):
             mailbox[WHITE][((rank + 2) * 10) + (file +1)] = KNIGHT;
            pieceList[W_KNIGHT][pieceCounter[W_KNIGHT]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_KNIGHT]; 
            ++file; 
            break;

		case('B'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = BISHOP;
            pieceList[W_BISHOP][pieceCounter[W_BISHOP]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_BISHOP]; 
            ++file; 
            break;

		case('Q'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = QUEEN;
            pieceList[W_QUEEN][pieceCounter[W_QUEEN]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_QUEEN];  
            ++file; 
            break;

		case('K'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = KING;
            pieceList[W_KING][pieceCounter[W_KING]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_KING];   
            ++file; 
            break;  

		case('p'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = PAWN;
            pieceList[B_PAWN][pieceCounter[B_PAWN]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_PAWN];   
            ++file; 
            break;

		case('r'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = ROOK;
            pieceList[B_ROOK][pieceCounter[B_ROOK]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_ROOK];   
            ++file; 
            break;

		case('n'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = KNIGHT;
            pieceList[B_KNIGHT][pieceCounter[B_KNIGHT]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_KNIGHT]; 
            ++file; 
            break;

		case('b'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = BISHOP;
            pieceList[B_BISHOP][pieceCounter[B_BISHOP]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_BISHOP]; 
            ++file; 
            break;

		case('q'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = QUEEN;
            pieceList[B_QUEEN][pieceCounter[B_QUEEN]] = static_cast<Square>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_QUEEN];  
            ++file; 
            break;

        case '/':
            file = FILE_A;
            --rank;
            break;
        
        default:
            if(isdigit(c))
                if((c - 48)>0 && (c - 48)<9)
                    file += c - 48;
            break;
        }
        
        c = fenNotation[++charIndex];
    }

    
    sideToMove = fenNotation[++charIndex] == 'w' ? WHITE : BLACK;

    c = fenNotation[++++charIndex];
    while(!isspace(c))
    {
        switch (c)
        {
		case 'K': history[historySize-1].castlingRight |= WKCA; break;
		case 'Q': history[historySize-1].castlingRight |= WQCA; break;
		case 'k': history[historySize-1].castlingRight |= BKCA; break;
		case 'q': history[historySize-1].castlingRight |= BQCA; break;
        }
        c = fenNotation[++charIndex];   
    }

    c = fenNotation[++charIndex];
    if(c != '-')
    {
        int enpassantFile = c - 97;
        c = fenNotation[++charIndex];
        int enpassantRank = c - 49;
	    history[historySize-1].enpassantSquare = Square(((enpassantRank+2) * 10) + (1 + enpassantFile));
    }

    c = fenNotation[++++charIndex];
    std::string fiftyMovesString{""};

    while(!isspace(c))
    {
        fiftyMovesString += c;
        c = fenNotation[++charIndex];   
    }

    history[historySize-1].fiftyMovesCounter = std::stoi(fiftyMovesString);


    c = fenNotation[++charIndex];
    std::string moveCounterString{""};

    while(!isspace(c) && c != '\0')
    {
        moveCounterString += c;
        c = fenNotation[++charIndex];   
    }

    history[historySize-1].movesCounter= std::stoi(moveCounterString);

    for(std::size_t c = 0; c < COLOR_SIZE-1; ++c){
        for(std::size_t i = 0; i < SQUARE_SIZE_120; ++i){
            if(mailbox[c][i] != NO_PIECE_TYPE)
                mailbox[COLOR_NC][i] = mailbox[c][i];
        }
    }

}


const std::size_t DIRECTION_SIDES = 4;
const Direction sizes[DIRECTION_SIDES] = {EAST, WEST, NORTH, SOUTH};
const Direction diagonals[DIRECTION_SIDES] = {NORTH_EAST, NORTH_WEST, SOUTH_EAST, SOUTH_WEST};


bool Position::square_is_attacked(const Square &square) const{

    //Pawns
    if(sideToMove==Color::WHITE){
        if(mailbox[Color::BLACK][Square(square+NORTH_EAST)] == PieceType::PAWN || mailbox[Color::BLACK][Square(square+NORTH_WEST)] == PieceType::PAWN){
            return true;
        }
    }
    if(sideToMove==Color::BLACK){
        if(mailbox[Color::WHITE][Square(square+SOUTH_EAST)] == PieceType::PAWN || mailbox[Color::WHITE][Square(square+SOUTH_WEST)] == PieceType::PAWN){
            return true;
        }
    }

    //Knights
    bool isKnightAttack =   (mailbox[~sideToMove][Square(square+NORTH_NORTH_WEST)] == PieceType::KNIGHT || mailbox[~sideToMove][Square(square+NORTH_NORTH_EAST)] == PieceType::KNIGHT ||
                            mailbox[~sideToMove][Square(square+NORTH_WEST_WEST)] == PieceType::KNIGHT  || mailbox[~sideToMove][Square(square+NORTH_EAST_EAST)] == PieceType::KNIGHT  ||
                            mailbox[~sideToMove][Square(square+SOUTH_WEST_WEST)] == PieceType::KNIGHT  || mailbox[~sideToMove][Square(square+SOUTH_EAST_EAST)] == PieceType::KNIGHT  ||
                            mailbox[~sideToMove][Square(square+SOUTH_SOUTH_WEST)] == PieceType::KNIGHT || mailbox[~sideToMove][Square(square+SOUTH_SOUTH_EAST)] == PieceType::KNIGHT) 
                            ? true : false;
    if(isKnightAttack){
        return true;
    }

    //bishops, queens
    for(Direction dir : diagonals){
        int toSquareIndex = square + dir;
        Square toSquare = SQUARES_120[toSquareIndex];
        while(toSquare != Square::OFFBOARD && mailbox[Color::COLOR_NC][toSquare] == NO_PIECE_TYPE) {
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
        Square toSquare = SQUARES_120[toSquareIndex];
        while(toSquare != Square::OFFBOARD && mailbox[Color::COLOR_NC][toSquare] == NO_PIECE_TYPE) {
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
        Square toSquare = SQUARES_120[toSquareIndex];
        if(mailbox[~sideToMove][toSquare] == PieceType::KING){
            return true;
        }
    }
    for(Direction dir : sizes){
        int toSquareIndex = square + dir;
        Square toSquare = SQUARES_120[toSquareIndex];
        if(mailbox[~sideToMove][toSquare] == PieceType::KING){
            return true;
        }
    }

    return false;
}

bool Position::do_move(const Move &move){

    Square from = move_from(move);
    Square to = move_to(move);
    SpecialMove specialMove = move_special(move);

    //Set move to the move history
    history[historySize-1].nextMove = move;

    if(specialMove != SpecialMove::NO_SPECIAL){
        if(specialMove == SpecialMove::ENPASSANT){
            if(sideToMove==Color::WHITE){
                remove_piece(Square(to+Direction::SOUTH));
            }else{
                remove_piece(Square(to+Direction::NORTH));
            }
        }
        if(specialMove == SpecialMove::CASTLE){
            switch (to)
            {
            case Square::C1:
                move_piece(Square::A1, Square::D1);
                break;
            case Square::G1:
                move_piece(Square::H1, Square::F1);
                break;
            case Square::C8:
                move_piece(Square::A8, Square::D8);
                break;
            case Square::G8:
                move_piece(Square::H8, Square::F8);
                break;
            default:
                break;
            }
        }
    }

    ++historySize;
    
    //Set in the new history

    //Castling rights
    history[historySize-1].castlingRight = history[historySize-2].castlingRight & CASTLE_PERSMISION_UPDATES[from];
    history[historySize-1].castlingRight = history[historySize-2].castlingRight & CASTLE_PERSMISION_UPDATES[to];
    
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
    history[historySize-1].enpassantSquare = Square::NO_SQUARE;
    if(mailbox[Color::COLOR_NC][from] == PieceType::PAWN){
        history[historySize-1].fiftyMovesCounter = 0;
        
        if(sideToMove==Color::WHITE && specialMove == SpecialMove::PAWN_START){
            history[historySize-1].enpassantSquare = Square(from+10);
        } else if(sideToMove==Color::BLACK && specialMove == SpecialMove::PAWN_START){
            history[historySize-1].enpassantSquare = Square(from-10);
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
        undo_move();
        return false;
    }
    sideToMove =~ sideToMove;
    return true;
}

void Position::undo_move(){
    
    Move move = history[historySize-2].nextMove;
    Square from = move_from(move);
    Square to = move_to(move);
    SpecialMove specialMove = move_special(move);

    sideToMove =~ sideToMove; 

    if(specialMove != SpecialMove::NO_SPECIAL){
        if(specialMove == SpecialMove::ENPASSANT){
            if(sideToMove==Color::WHITE){
                add_piece(Square(to+Direction::SOUTH), Piece::B_PAWN);
            }else{
                add_piece(Square(to+Direction::NORTH), Piece::W_PAWN);
            }
        }
        if(specialMove == SpecialMove::CASTLE){
            switch (to)
            {
            case Square::C1:
                move_piece(Square::D1, Square::A1);
                break;
            case Square::G1:
                move_piece(Square::F1, Square::H1);
                break;
            case Square::C8:
                move_piece(Square::D8, Square::A8);
                break;
            case Square::G8:
                move_piece(Square::F8, Square::H8);
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
    history[historySize-1].castlingRight = 0;
    history[historySize-1].fiftyMovesCounter = 0;
    history[historySize-1].movesCounter = 0;
    history[historySize-1].enpassantSquare = Square::NO_SQUARE;

    --historySize;

}

void Position::move_piece(const Square &from, const Square &to){

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

}

//TODO usar templates para evitar if else
void Position::remove_piece(const Square &square){

    Color pieceColor{Color::COLOR_NC};
    PieceType pieceType{PieceType::NO_PIECE_TYPE};

    // Check witch piece is located on the square
    if(mailbox[Color::WHITE][square] != PieceType::NO_PIECE_TYPE){
        pieceType = mailbox[Color::WHITE][square];
        mailbox[Color::WHITE][square] = PieceType::NO_PIECE_TYPE;
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

    Piece piece = make_piece(pieceColor, pieceType);

    for(std::size_t i = 0; i < pieceCounter[piece]; ++i){
        
        if(pieceList[piece][i] == square){
            pieceList[piece][i] = pieceList[piece][pieceCounter[piece]-1];
            pieceList[piece][pieceCounter[piece]-1] = Square::NO_SQUARE;
            --pieceCounter[piece];
            break;
        }
    }
}

void Position::add_piece(const Square &square, const Piece &piece){

    Color pieceColor = piece_color(piece);
    PieceType pieceType = piece_type(piece);
    mailbox[pieceColor][square] = pieceType;
    mailbox[Color::COLOR_NC][square] = pieceType;

    pieceList[piece][pieceCounter[piece]] = square;
    ++pieceCounter[piece];

}

void Position::print_board(){

    for(std::size_t rank = 90; rank > 10; rank -= 10){
        for(std::size_t file = 1; file < 9; ++file) {
            char csquare;
            std::size_t index = rank + file;
            if(mailbox[Color::WHITE][index] != PieceType::NO_PIECE_TYPE){
                PieceType wpt = mailbox[Color::WHITE][index];
                switch (wpt)
                {
                    case PieceType::BISHOP: csquare = 'B'; break;
                    case PieceType::KING: csquare = 'K'; break;
                    case PieceType::KNIGHT:   csquare = 'N'; break;
                    case PieceType::PAWN:  csquare = 'P'; break;
                    case PieceType::QUEEN:  csquare = 'Q'; break;
                    case PieceType::ROOK:  csquare = 'R'; break;
                }
            }
            else if(mailbox[Color::BLACK][index] != PieceType::NO_PIECE_TYPE){
                PieceType wpt = mailbox[Color::BLACK][index];
                switch (wpt)
                {
                    case PieceType::BISHOP: csquare = 'b'; break;
                    case PieceType::KING: csquare = 'k'; break;
                    case PieceType::KNIGHT:   csquare = 'n'; break;
                    case PieceType::PAWN:  csquare = 'p'; break;
                    case PieceType::QUEEN:  csquare = 'q'; break;
                    case PieceType::ROOK:  csquare = 'r'; break;
                }
            }
            else{
                csquare = '-';
            }
            std::cout << " " << csquare << " ";
        }
        std::cout << "\n";
    }
}

} // namespace Xake
