#include "uci.h"
#include "position.h"
#include "search.h"
#include "timemanager.h"
#include "ttable.h"

#include <exception>
#include <iostream>
#include <sstream>
#include <string>
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
void setoption(std::istringstream &is);

void run(){

    Position pos;
    pos.set_FEN(START_FEN);

    Search::SearchInfo searchInfo;
    std::thread searchThread;

    std::string inputStr, token;

    uci_info();

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
        
        else if (token == "setoption")
            setoption(is);

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

void go_info(const Position & pos, std::istringstream &is, Search::SearchInfo &searchInfo) {

    std::string arg;

    searchInfo.depth     = MAX_DEPTH;
    searchInfo.stop      = false;
    searchInfo.searchPly = 0;

    using TM = TimeManager;
    TM::BudgetParams bP;
    TM &tm = searchInfo.timeManager;
    tm.mark_start();

    auto read_ms = [&](std::optional<TM::Ms>& dst) {
        long long v;
        if (is >> v) dst = TM::Ms{v};
    };

    auto read_int = [&](std::optional<int>& dst) {
        int v;
        if (is >> v) dst = v;
    };  

    // UCI parse
    while (is >> arg) {
        if      (arg == "depth")    { is >> searchInfo.depth; }
        else if (arg == "wtime" && pos.get_side_to_move() == WHITE) { read_ms(bP.colorTimeMs); bP.ply = pos.get_ply(); }
        else if (arg == "btime" && pos.get_side_to_move() == BLACK) { read_ms(bP.colorTimeMs); bP.ply = pos.get_ply(); }
        else if (arg == "winc"  && pos.get_side_to_move() == WHITE) { read_ms(bP.incMs); }
        else if (arg == "binc"  && pos.get_side_to_move() == BLACK) { read_ms(bP.incMs); }
        else if (arg == "movestogo") { read_int(bP.movesToGo); }
        else if (arg == "movetime")  { read_ms(bP.moveTimeMs); }
        else if (arg == "infinite")  { bP.moveTimeMs.reset(); bP.colorTimeMs.reset(); }
    }

    tm.allocate_budget(bP);

    const auto rem = tm.remaining_ms();
    std::cout << "info string search depth " << searchInfo.depth
              << " remaining " << (rem ? std::to_string(rem->count()) + "ms" : "infinite")
              << std::endl;
}

void uci_info(){

    std::cout << "id name Xake 0.0.1" << "\n";
    std::cout << "id author Julen Aristondo" << "\n";
    std::cout << "option name Hash type spin default " << TT::DEFAULT_TT_MB
              << " min " << TT::MIN_TT_MB
              << " max " << TT::MAX_TT_MB << "\n";
    std::cout << "uciok" << "\n";

}

void setoption(std::istringstream &is) {

    std::string token;
    std::string name;
    std::string value;

    while (is >> token) {
        if (token == "name") {
            while (is >> token && token != "value") {
                if (!name.empty()) name += " ";
                name += token;
            }
            if (token == "value") {
                is >> value;
            }
            break;
        }
    }

    if (name == "Hash" && !value.empty()) {

        const std::size_t hashMB = std::stoull(value);
        TT::resize(hashMB);
        std::cout << "info string Hash set to " << TT::current_size_mb() << " MB" << std::endl;

    }
}

}
}
