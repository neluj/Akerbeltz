#include "uci.h"
#include "position.h"
#include "search.h"

#include <iostream>
#include <sstream>
#include <thread>
#include <functional>

namespace Xake{

namespace UCI{

const std::string START_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

void position(Position & pos, std::istringstream &is);
Move make_move(const Position &pos, std::string algebraic_move);
void go(Position & pos, std::istringstream &is, Search::SearchInfo &searchInfo, std::thread &searchThread);
void go_info(const Position & pos, std::istringstream &is, Search::SearchInfo &searchInfo);
void uci_info();

void run(){

    Position pos;
    pos.set_FEN(START_FEN);

    Search::SearchInfo searchInfo;
    std::thread searchThread;

    std::string inputStr, token;

    uci_info();

    //if(argc<2)
    //    token = "quit";

    do{
        getline(std::cin, inputStr);
        std::istringstream is(inputStr);

        is >> token;

        if (token == "go")
            go(pos, is, searchInfo, searchThread);

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
        
        else if (token == "stop"){
            searchInfo.stop = true;
            if(searchThread.joinable())
                searchThread.join();
        }
        
        else if(token == "quit"){
            searchInfo.stop = true;
            if(searchThread.joinable())
                searchThread.join();
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

    //std::cout << pos;
    
}

Move make_move(const Position &pos, std::string algebraic_move){
    
    //return error
    if( algebraic_move.size() < 4 || algebraic_move.size() > 5 ){
        return 0;
    }
    
    if(algebraic_move[0] - 'a' > 8 || algebraic_move[0] - 'a' < 0){
        return 0;
    }

    if(algebraic_move[2] - 'a' > 8 || algebraic_move[2] - 'a' < 0){
        return 0;
    }
    
    if(algebraic_move[1] - '1' > 8 || algebraic_move[1] - '1' < 0){
        return 0;
    }

    if(algebraic_move[3] - '1' > 8 || algebraic_move[3] - '1' < 0){
        return 0;
    }

	Square64 from = Square64(((algebraic_move[1] - '1') * 8 ) + (algebraic_move[0] - 'a'));
	Square64 to   = Square64(((algebraic_move[3] - '1') * 8 ) + (algebraic_move[2] - 'a')); 
          

    Piece piece = pos.get_mailbox_piece(from);
    SpecialMove specialMove{SpecialMove::NO_SPECIAL};

    if(algebraic_move.size() == 5){
        switch (algebraic_move[4])
        {
            case 'n':   specialMove = PROMOTION_KNIGHT; break;
            case 'b':   specialMove = PROMOTION_BISHOP; break;
            case 'r':   specialMove = PROMOTION_ROOK; break;
            case 'q':   specialMove = PROMOTION_QUEEN; break;
            default:
                return 0;
        }
    }else if(piece_type(piece) == PAWN){ 
        if(abs(to - from) == NORTH_NORTH){
            specialMove = PAWN_START;
        }
        else if(to == pos.get_enpassant_square()){
            return make_enpassant_move(from, to);
        }
    }else if(piece_type(piece) == KING && abs(to - from) == 2){
        specialMove = CASTLE;
    }

    Piece capturedPiece = pos.get_mailbox_piece(to);

    if(capturedPiece == NO_PIECE)
        return make_quiet_move(from, to, specialMove);
        
    return make_capture_move(from, to, specialMove, piece, capturedPiece);
}

void go(Position & pos, std::istringstream &is, Search::SearchInfo &searchInfo, std::thread &searchThread){

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
        if (searchThread.joinable())
            searchThread.join();
        searchThread = std::thread(Search::search, std::ref(pos), std::ref(searchInfo));
    }

}

void go_info(const Position & pos, std::istringstream &is, Search::SearchInfo &searchInfo){

    std::string arg;
    searchInfo.depth = MAX_DEPTH;
    searchInfo.stopTime = Xake::NO_TIME;
    searchInfo.startTime = Xake::NO_TIME;
    searchInfo.moveTime = Xake::NO_TIME;
    searchInfo.realTime = Xake::NO_TIME;
    searchInfo.timeOver = false;
    searchInfo.stop = false;
    searchInfo.searchPly = 0;
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
    "max time ms: " << searchInfo.moveTime << 
    " depth: " << searchInfo.depth << 
    "\n";

}

void uci_info(){

    std::cout << "id name Xake 0.0.1" << "\n";
    std::cout << "id author Julen Aristondo" << "\n";
    std::cout << "uciok" << "\n";

}

}
}