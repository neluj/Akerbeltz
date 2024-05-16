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

    return os;

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
    history[historySize-1].castlingRight = 0;
    history[historySize-1].fiftyMovesCounter = 0;
    history[historySize-1].movesCounter = 0;
    history[historySize-1].enpassantSquare = Square120::SQ120_NO_SQUARE;

    material_score[WHITE] = 0;
    material_score[BLACK] = 0;
    material_score[COLOR_NC] = 0;

}

void Position::clean_position(){

    clean_mailbox();
    clean_piece_list();
    clear_position_info();

}

//TODO FEN could be shorter 
// https://www.geeksforgeeks.org/processing-strings-using-stdistringstream/
// https://www.geeksforgeeks.org/stringstream-c-applications/
//https://www.geeksforgeeks.org/ios-manipulators-skipws-function-in-c/
//dividir en funciones
//constexpr std::string_view PieceToChar(" PNBRQK  pnbrqk"); en vez de switch case
void Position::set_FEN(std::string fenNotation){

    clean_position();

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
            pieceList[B_KING][pieceCounter[B_KING]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_KING];   
            ++file; 
            break;  

		case('P'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = PAWN;
            pieceList[W_PAWN][pieceCounter[W_PAWN]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_PAWN];   
            ++file; 
            break;

		case('R'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = ROOK;
            pieceList[W_ROOK][pieceCounter[W_ROOK]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_ROOK];   
            ++file; 
            break;

		case('N'):
             mailbox[WHITE][((rank + 2) * 10) + (file +1)] = KNIGHT;
            pieceList[W_KNIGHT][pieceCounter[W_KNIGHT]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_KNIGHT]; 
            ++file; 
            break;

		case('B'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = BISHOP;
            pieceList[W_BISHOP][pieceCounter[W_BISHOP]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_BISHOP]; 
            ++file; 
            break;

		case('Q'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = QUEEN;
            pieceList[W_QUEEN][pieceCounter[W_QUEEN]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_QUEEN];  
            ++file; 
            break;

		case('K'): 
            mailbox[WHITE][((rank + 2) * 10) + (file +1)] = KING;
            pieceList[W_KING][pieceCounter[W_KING]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[W_KING];   
            ++file; 
            break;  

		case('p'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = PAWN;
            pieceList[B_PAWN][pieceCounter[B_PAWN]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_PAWN];   
            ++file; 
            break;

		case('r'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = ROOK;
            pieceList[B_ROOK][pieceCounter[B_ROOK]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_ROOK];   
            ++file; 
            break;

		case('n'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = KNIGHT;
            pieceList[B_KNIGHT][pieceCounter[B_KNIGHT]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_KNIGHT]; 
            ++file; 
            break;

		case('b'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = BISHOP;
            pieceList[B_BISHOP][pieceCounter[B_BISHOP]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
            ++pieceCounter[B_BISHOP]; 
            ++file; 
            break;

		case('q'): 
            mailbox[BLACK][((rank + 2) * 10) + (file +1)] = QUEEN;
            pieceList[B_QUEEN][pieceCounter[B_QUEEN]] = static_cast<Square120>(((rank + 2) * 10) + (file +1));
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
        //TODO contar materiales y sumar y restar en cada captura, promotion o undo move
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
	    history[historySize-1].enpassantSquare = Square120(((enpassantRank+2) * 10) + (1 + enpassantFile));
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

    calc_material_score();

}

void Position::calc_material_score(){

    for(std::size_t sq = 0; sq < SQUARE_SIZE_120; ++sq){
        if(mailbox[WHITE][sq] != NO_PIECE_TYPE)
            material_score[WHITE] += Evaluate::calc_material_table(WHITE, mailbox[WHITE][sq], Square120(sq));
    }

    for(std::size_t sq = 0; sq < SQUARE_SIZE_120; ++sq){
        if(mailbox[BLACK][sq] != NO_PIECE_TYPE)
            material_score[BLACK] += Evaluate::calc_material_table(BLACK, mailbox[BLACK][sq], Square120(sq));
    }

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

    if(specialMove != SpecialMove::NO_SPECIAL){
        if(specialMove == SpecialMove::ENPASSANT){
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

    ++historySize;
    
    //Set in the new history

    //Castling rights
    history[historySize-1].castlingRight = history[historySize-2].castlingRight & CASTLE_PERSMISION_UPDATES[from];
    history[historySize-1].castlingRight &= CASTLE_PERSMISION_UPDATES[to];
    
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
        } else if(sideToMove==Color::BLACK && specialMove == SpecialMove::PAWN_START){
            history[historySize-1].enpassantSquare = Square120(from-10);
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
    history[historySize-1].castlingRight = 0;
    history[historySize-1].fiftyMovesCounter = 0;
    history[historySize-1].movesCounter = 0;
    history[historySize-1].enpassantSquare = Square120::SQ120_NO_SQUARE;

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
    material_score[pieceColor] -= Evaluate::calc_material_table(pieceColor, pieceType, from);
    material_score[pieceColor] += Evaluate::calc_material_table(pieceColor, pieceType, to);

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
    material_score[pieceColor] -= Evaluate::calc_material_table(pieceColor, pieceType, square);
}

void Position::add_piece(Square120 square, Piece piece){

    Color pieceColor = piece_color(piece);
    PieceType pieceType = piece_type(piece);
    mailbox[pieceColor][square] = pieceType;
    mailbox[Color::COLOR_NC][square] = pieceType;

    pieceList[piece][pieceCounter[piece]] = square;
    ++pieceCounter[piece];

    //Add piece value from material 
    material_score[pieceColor] += Evaluate::calc_material_table(pieceColor, pieceType, square);

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
