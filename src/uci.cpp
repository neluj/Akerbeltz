#include "uci.h"

#include <iostream>
#include <sstream>

#include "position.h"
#include "search.h"

namespace Xake{

namespace UCI{

const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void position(Position & pos, std::stringstream &is);
Move make_move(const Position &pos, std::string algebraic_move);
void go(Position & pos, Search::SearchInfo &info);
void uci_info();

// https://www.geeksforgeeks.org/processing-strings-using-stdistringstream/
// https://www.geeksforgeeks.org/stringstream-c-applications/
// https://www.geeksforgeeks.org/ios-manipulators-skipws-function-in-c/
void run(){

    Position pos;
    pos.set_FEN(START_FEN);

    Search::SearchInfo info;
    info.depth = 4;

    std::string inputStr, token;

    uci_info();

    //if(argc<2)
    //    token = "quit";

    do{
        getline(std::cin, inputStr);
        std::stringstream is(inputStr);

        is >> token;

        if (token == "go")
            go(pos, info);

        else if (token == "position")
            position(pos, is);
        //REVIEW
        else if (token == "ucinewgame"){
            is.str("startpos");
            position(pos, is);
        }
        
        else if(token == "d")
            std::cout << pos << std::endl;
        
        else if (token == "isready")
            std::cout << "readyok" << std::endl;

        else if (token == "uci")
            uci_info();
        
        // TODO info.stop = true
        else if(token == "quit"){
            break;
        }
    
        else 
            std::cout << "Unknown command " << token << "." << " Type 'quit' for quit program." << std::endl;

    }while(token != "quit");

}

void position(Position & pos, std::stringstream &is){

    std::string arg, fen;

    is >> arg;

    if(arg == "startpos")
        fen = START_FEN;
    else if(arg == "fen")
        while(is >> arg && arg != "moves")
            fen += arg + " ";
    else
        return;

    pos.set_FEN(fen);
    
    is >> arg;
    while(is >> arg)
        pos.do_move(make_move(pos, arg));

    std::cout << pos;
    
}

Move make_move(const Position &pos, std::string algebraic_move){
    
    //return error
    if( 4 > algebraic_move.size() || algebraic_move.size() > 5 )
        return 0;
    
	Square120 from = Square120((algebraic_move[1] - '1') * 10 + (algebraic_move[0] - 'a' + 1) + 20);
	Square120 to = Square120((algebraic_move[3] - '1') * 10 + (algebraic_move[2] - 'a' + 1) + 20);

    PieceType pieceTypeToMove = pos.get_mailbox_pieceType(COLOR_NC, from);
       

    SpecialMove specialMove{SpecialMove::NO_SPECIAL};

    if(algebraic_move.size() == 5){
        switch (algebraic_move[4])
        {
            case 'n':   specialMove = PROMOTION_KNIGHT; break;
            case 'b':   specialMove = PROMOTION_BISHOP; break;
            case 'r':   specialMove = PROMOTION_ROOK; break;
            case 'q':   specialMove = PROMOTION_QUEEN; break;
        }
    }else if(pieceTypeToMove == PAWN){ 
        if(abs(to - from) == NORTH_NORTH){
            specialMove = PAWN_START;
        }
        else if(to == pos.get_enpassant_square()){
            specialMove = ENPASSANT;
        }
    }else if(pieceTypeToMove == KING && abs(to - from) == 2){
        specialMove = CASTLE;
    }

    Piece capturedPiece = make_piece(WHITE, pos.get_mailbox_pieceType(WHITE, to));
    if(capturedPiece == NO_PIECE)
        capturedPiece = make_piece(BLACK, pos.get_mailbox_pieceType(WHITE, to));


    return make_move(from, to, specialMove, capturedPiece);
}

void go(Position & pos,  Search::SearchInfo &info){

    Search::search(pos, info);
    //TODO finish

}

void uci_info(){

    std::cout << "id name Xake 0.0.1" << std::endl;
    std::cout << "id author Julen Aristondo" << std::endl;
    std::cout << "uciok" << std::endl;

}

}
}