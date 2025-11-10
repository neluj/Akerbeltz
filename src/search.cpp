#include "search.h"

#include "movegen.h"
#include "position.h"
#include "pvtable.h"

#include <iostream>

namespace Xake{

namespace Search{

using namespace Evaluate;

static NodesSize leafCounter;
PVTable::PVLine pvLine;

//Killer heuristic
Move killerMoves[MAX_KILLERMOVES][MAX_DEPTH];

//History Heuristic
MoveScore searchHistory[PIECE_SIZE][SQ64_SIZE];

void perft(Position &position, DepthSize depth);

Score alpha_beta(Position &position, SearchInfo &searchInfo, Score alpha, Score beta, DepthSize depth);
Score quiescence_search(Position &position, SearchInfo &searchInfo, Score alpha, Score beta);
void clean_search_info(SearchInfo &searchInfo);
void pick_move(int moveIndx, MoveGen::MoveList &moveList);
bool is_draw(const Position &position, const SearchInfo &searchInfo);
static Move first_legal_move(Position& position);
void print_iter_info(DepthSize currentDepth, int bestMoveScore, Move bestMove, SearchInfo &searchInfo);


void search(Position &position, SearchInfo &searchInfo){

    int bestMoveScore = -CHECKMATE_SCORE;
    // fallback
    Move bestMove = first_legal_move(position);
    if (!bestMove) { std::cout << "bestmove 0000\n"; return; }

    clean_search_info(searchInfo);

    NodesSize prevTotalNodes = searchInfo.nodes;
    uint64_t  lastIterNodes  = 0;   

    for(DepthSize currentDepth = 1; currentDepth <= searchInfo.depth; ++currentDepth){

        const TimeManager::Ms iterStartMs = searchInfo.timeManager.elapsed_ms();

        bestMoveScore = alpha_beta(position, searchInfo, -CHECKMATE_SCORE, CHECKMATE_SCORE, currentDepth);

        //Necessary for avoid loading partially calculated pv line
        if (searchInfo.timeManager.out_of_time() || searchInfo.stop) {
            break;
        }

        PVTable::load_pv_line(pvLine, MAX_DEPTH, position);
        if (pvLine.depth > 0) bestMove = pvLine.moves[0];
        
        print_iter_info(currentDepth, bestMoveScore, bestMove, searchInfo);

        // Check iteration times
        const auto iterEndMs  = searchInfo.timeManager.elapsed_ms();
        const TimeManager::Ms iterMs  = iterEndMs - iterStartMs;
        const uint64_t iterNodes = searchInfo.nodes - prevTotalNodes;

        // Calc if new iteration is possible
        searchInfo.timeManager.on_iteration_finished(iterNodes, lastIterNodes);
        lastIterNodes   = iterNodes;
        prevTotalNodes  = searchInfo.nodes;

        if (!searchInfo.timeManager.enough_time_for_next_iteration(iterMs)) {
            break;
        }

    }

    std::cout << "bestmove " << algebraic_move(bestMove) << std::endl;
}


Score alpha_beta(Position &position, SearchInfo &searchInfo, Score alpha, Score beta, DepthSize depth){

    if (is_draw(position, searchInfo)) { return DRAW_SOCORE; }

    if(depth==0) { return quiescence_search(position, searchInfo, alpha, beta); }

    ++searchInfo.nodes;

    bool isCheck = position.square_is_attacked_bySide(Square64(Bitboards::ctz(position.get_pieceTypes_bitboard(position.get_side_to_move(), KING))), ~position.get_side_to_move());

    if(isCheck){
        depth++;
    }

    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);

    Move pvMove = PVTable::probe_move(position);

    //Set move scores
    for(int mIndx = 0; mIndx < moveList.size; ++mIndx){
        if(equal_move(moveList.moves[mIndx], pvMove)){
            moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], PV_SCORE);
        }else if(is_capture(moveList.moves[mIndx])){
            if(move_special(moveList.moves[mIndx]) == ENPASSANT){
                moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], MVVLVAScores[PAWN][PAWN]);
            }
            else{
                moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], MVVLVAScores[piece_type(attacker_piece(moveList.moves[mIndx]))][piece_type(captured_piece(moveList.moves[mIndx]))]);
            }
        }
        else if(equal_move(moveList.moves[mIndx], killerMoves[0][searchInfo.searchPly])){
            moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], KILLERMOVE_SOCORE_0);
        }else if(equal_move(moveList.moves[mIndx], killerMoves[1][searchInfo.searchPly])){
            moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], KILLERMOVE_SOCORE_1);
        }else{ 
            moveList.moves[mIndx] = set_heuristic_score(moveList.moves[mIndx], searchHistory[position.get_mailbox_piece(move_from(moveList.moves[mIndx]))][move_to(moveList.moves[mIndx])]);
        }
    }

    Score score = -CHECKMATE_SCORE;
    Score oldAlpha = alpha;
    Move bestMove = 0;
    int legalMoves = 0;

    for(int mIndx = 0; mIndx < moveList.size; ++mIndx){

        pick_move(mIndx, moveList);
        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }
        ++searchInfo.searchPly;
        ++legalMoves;
        
        score = -alpha_beta(position, searchInfo, -beta, -alpha, depth - 1);
        position.undo_move();
        --searchInfo.searchPly;

        if ((searchInfo.nodes & 2047) == 0) {
            if (searchInfo.timeManager.out_of_time() || searchInfo.stop) { break; }
        }

        if(score>alpha){
            if(score>=beta){

                if(!is_capture(move) && !equal_move(move, killerMoves[0][searchInfo.searchPly])){
                    killerMoves[1][searchInfo.searchPly] = killerMoves[0][searchInfo.searchPly];
                    killerMoves[0][searchInfo.searchPly] = raw_move(move);
                }

                return beta;
            }
            alpha = score;
            bestMove = move;

            if(!is_capture(bestMove)){
                searchHistory[position.get_mailbox_piece(move_from(bestMove))][move_to(bestMove)] += depth;
            }
        }
    }

    if(legalMoves == 0){
        if(isCheck)
            return -CHECKMATE_SCORE + searchInfo.searchPly;
        else
            return DRAW_SOCORE;
    }

    if(alpha != oldAlpha){
        PVTable::insert_entry(position, raw_move(bestMove));
    }

    return alpha;
    
}

Score quiescence_search(Position &position, SearchInfo &searchInfo, Score alpha, Score beta){

    ++searchInfo.nodes;

    if (is_draw(position, searchInfo)){
        return DRAW_SOCORE;
    }

    if(searchInfo.depth > MAX_DEPTH - 1){
        return Evaluate::calc_score(position);
    }

    Score score = Evaluate::calc_score(position);

    if(score >= beta){
        return beta;
    }

    if(score > alpha){
        alpha = score;
    }

    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_captures(position, moveList);

    score = -CHECKMATE_SCORE;
    Score oldAlpha = alpha;
    Move bestMove = 0;

    for(int mIndx = 0; mIndx < moveList.size; ++mIndx){

        pick_move(mIndx, moveList);

        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }
        ++searchInfo.searchPly;

        score = -quiescence_search(position, searchInfo, -beta, -alpha);
        position.undo_move();
        --searchInfo.searchPly;

        if ((searchInfo.nodes & 2047) == 0 && (searchInfo.timeManager.out_of_time() || searchInfo.stop)) { break; }
        
        if(score>alpha){
            if(score>=beta){
                return beta;
            }
            alpha = score;
            bestMove = move;
        }
    }

    if(alpha != oldAlpha){
        PVTable::insert_entry(position, raw_move(bestMove));
    }

    return alpha;

}

void clean_search_info(SearchInfo &searchInfo){
    searchInfo.nodes = 0;
    //searchInfo.timeOver = false;
    
    for(int i = 0; i < MAX_KILLERMOVES; ++i){
        for(int x = 0; x < MAX_DEPTH; ++x){
            killerMoves[i][x] = 0;
        }
    }
    for(int i = 0; i < PIECE_SIZE; ++i){
        for(int x = 0; x < SQ64_SIZE; ++x){
            searchHistory[i][x] = 0;
        }
    }
}

bool is_draw(const Position &position, const SearchInfo &searchInfo) {
    if ((position.is_repetition() || position.get_fifty_moves_counter() >= 100) && searchInfo.searchPly) {
        return true;
    }
    return false;
}

static Move first_legal_move(Position& position) {
    MoveGen::MoveList ml;
    MoveGen::generate_pseudo_moves(position, ml);
    for (int i = 0; i < ml.size; ++i) {
        Move m = ml.moves[i];
        if (position.do_move(m)) { position.undo_move(); return m; }
    }
    return 0;
}

void pick_move(int moveIndx, MoveGen::MoveList &moveList){

    MoveScore moveScr{0};
    MoveScore bestScr{0};
    int bestIndx = moveIndx;

    for(int i = moveIndx; i < moveList.size; ++i){
        moveScr = move_score(moveList.moves[i]);
        if(moveScr > bestScr){
            bestScr = moveScr;
            bestIndx = i;
        }
    }

    Move moveTemp = moveList.moves[moveIndx];
    moveList.moves[moveIndx] = moveList.moves[bestIndx];
    moveList.moves[bestIndx] = moveTemp;
}

void print_iter_info(DepthSize currentDepth, int bestMoveScore, Move bestMove, SearchInfo &searchInfo){

        std::cout <<
        "info" << 
        " depth " << currentDepth << 
        " score cp " << bestMoveScore << 
        " move " << algebraic_move(bestMove) <<
        " nodes " << searchInfo.nodes <<
        " time " << searchInfo.timeManager.elapsed_ms().count();

        std::cout << " pv";

        for(DepthSize pvLineDepth = 0; pvLineDepth < pvLine.depth; ++pvLineDepth){
            std::cout << " " << algebraic_move(pvLine.moves[pvLineDepth]);
        }

        std::cout << std::endl;
}


void perft(Position &position, DepthSize depth){

   if(depth == 0){
        leafCounter++;
        return;
    } 

    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);

    for(int mIndx = 0; mIndx < moveList.size; ++mIndx){
        
        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }
        //std::cout << "\n" << "Sub-move:"<< algebraic_move(move);
        perft(position, depth-1);
        position.undo_move();
    }
    //std::cout << "\n";
}


NodesSize perftTest(Position &position, SearchInfo &searchInfo){

    clean_search_info(searchInfo);
    leafCounter = 0;
    NodesSize allNodesCounter = 0;
    DepthSize actualDepth = searchInfo.depth-1;

    MoveGen::MoveList moveList;
    MoveGen::generate_pseudo_moves(position, moveList);

    std::cout << "\n";
    
    for(int mIndx = 0; mIndx < moveList.size;++mIndx){

        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }

        long cumnodes = leafCounter;
        perft(position, actualDepth);

        position.undo_move();

        long oldNodes = leafCounter - cumnodes;
        allNodesCounter += oldNodes;

        std::cout << algebraic_move(move) << ": " << oldNodes << "\n";

    }

    std::cout << "\n" << 
    "total nodes size: " << allNodesCounter <<
    " time ms: " << searchInfo.timeManager.elapsed_ms().count() <<
    "\n\n";

    return allNodesCounter;

}
}
}