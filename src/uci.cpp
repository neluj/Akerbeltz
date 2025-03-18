#include "uci.h"

#include <iostream>
#include <sstream>

#include "position.h"
#include "search.h"

namespace Xake{

namespace UCI{

const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void position(Position & pos, std::istringstream &is);
Move make_move(const Position &pos, std::string algebraic_move);
void go(Position & pos, std::istringstream &is, Search::SearchInfo &searchInfo);
void go_info(const Position & pos, std::istringstream &is, Search::SearchInfo &searchInfo);
void uci_info();

void run(){

    Position pos;
    pos.set_FEN(START_FEN);

    Search::SearchInfo searchInfo;

    std::string inputStr, token;

    uci_info();

    //if(argc<2)
    //    token = "quit";

    do{
        getline(std::cin, inputStr);
        std::istringstream is(inputStr);

        is >> token;

        if (token == "go")
            go(pos, is, searchInfo);

        else if (token == "position")
            position(pos, is);
        //REVIEW
        else if (token == "ucinewgame"){
            is.str("startpos");
            position(pos, is);
        }
        
        else if(token == "d")
            std::cout << pos;
        
        else if (token == "isready")
            std::cout << "readyok" << std::endl;

        else if (token == "uci")
            uci_info();
        
        // TODO searchInfo.stop = true
        else if(token == "quit"){
            break;
        }
    
        else 
            std::cout << "Unknown command " << token << "." << " Type 'quit' for quit program." << std::endl;

    }while(token != "quit");

}

void position(Position & pos, std::istringstream &is){

    std::string arg, fen;

    is >> arg;

    if(arg == "startpos"){
        fen = START_FEN;
        is >> arg;
    }
    else if(arg == "fen")
        while(is >> arg && arg != "moves")
            fen += arg + " ";
    else
        return;

    pos.set_FEN(fen);
    //VIDEO uno de los bugs que he tenido al ponerlo en marcha con el protocolo ha sido que a veces is >> arg; se salta y otras no. Solo
    //se queda el salto en startpos, a que tiene que saltar dos palabras, no solo "moves", sino "startpos" tambien.
    //is >> arg;
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
        capturedPiece = make_piece(BLACK, pos.get_mailbox_pieceType(BLACK, to));

    return make_move(from, to, specialMove, capturedPiece);
}

void go(Position & pos, std::istringstream &is, Search::SearchInfo &searchInfo){

    std::string arg;
    std::streampos isParamPos = is.tellg(); 

    is >> arg;

    if(arg == "perft"){
        go_info(pos, is, searchInfo);
        Search::perftTest(pos, searchInfo); 
    }
    else{
        is.seekg(isParamPos);
        go_info(pos, is, searchInfo);
        Search::search(pos, searchInfo);
    }

}

void go_info(const Position & pos, std::istringstream &is, Search::SearchInfo &searchInfo){

    std::string arg;
    searchInfo.depth = MAX_DEPTH;
    searchInfo.stopTime = Xake::NO_TIME;
    searchInfo.moveTime = Xake::NO_TIME;
    searchInfo.startTime = Xake::NO_TIME;
    searchInfo.timeOver = false;
    Xake::Time colorTime = Xake::NO_TIME;    
    Xake::Time moveTime = Xake::NO_TIME; 
    Xake::Time colorInc = 0;   
    Xake::MovesSize movesToGo = 30;

    while (is >> arg)
    { 
        if(arg == "depth")
            is >> searchInfo.depth;
        else if(arg == "wtime" && pos.get_side_to_move() == WHITE)
            is >> colorTime;
        else if(arg == "btime" && pos.get_side_to_move() == BLACK)
            is >> colorTime;
        else if(arg == "winc" && pos.get_side_to_move() == WHITE)
            is >> colorInc;
        else if(arg == "binc" && pos.get_side_to_move() == BLACK)
            is >> colorInc;
        else if(arg == "movestogo")
            is >> movesToGo;
        else if(arg == "movetime")
            is >> moveTime;
        else if(arg == "infinite")
            moveTime = Xake::NO_TIME;
    }

    searchInfo.startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    if(moveTime != Xake::NO_TIME){
        searchInfo.moveTime = moveTime;
        searchInfo.stopTime = searchInfo.startTime + searchInfo.moveTime;    
    }else if (colorTime != Xake::NO_TIME){
        colorTime /= movesToGo;
        colorTime -= 50;
        searchInfo.moveTime = colorTime + colorInc;
        searchInfo.stopTime = searchInfo.startTime + searchInfo.moveTime; 
    }

    std::cout <<
    "time:" << searchInfo.moveTime << 
    " start:" << searchInfo.startTime << 
    " stop:" << searchInfo.stopTime <<
    " depth:" << searchInfo.depth;

}

void uci_info(){

    std::cout << "id name Xake 0.0.1" << std::endl;
    std::cout << "id author Julen Aristondo" << std::endl;
    std::cout << "uciok" << std::endl;

}

}
}