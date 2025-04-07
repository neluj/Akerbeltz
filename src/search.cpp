#include "search.h"

#include <iostream>

#include "movegen.h"
#include "position.h"
#include "pvtable.h"

namespace Xake{

namespace Search{

using namespace Evaluate;

static NodesSize leafCounter;
PVTable::PVLine pvLine;

void perft(Position &position, DepthSize depth);

Score alpha_beta(Position &position, SearchInfo &searchInfo, Score alpha, Score beta, DepthSize depth);
Score quiescence_search(Position &position, SearchInfo &searchInfo, Score alpha, Score beta);
void clean_search_info(SearchInfo &searchInfo);
void check_time(SearchInfo &searchInfo);
//TODO isrepetition
//TODO PickNextMove

void search(Position &position, SearchInfo &searchInfo){

    int bestMoveScore = -CHECKMATE_SCORE;
    Move bestMove = 0;

    clean_search_info(searchInfo);

    for(DepthSize currentDepth = 1; currentDepth <= searchInfo.depth; ++currentDepth){
        
        bestMoveScore = alpha_beta(position, searchInfo, -CHECKMATE_SCORE, CHECKMATE_SCORE, currentDepth);
        
        if(searchInfo.timeOver){
            break;
        }
        
        Xake::Time timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count() - searchInfo.startTime;
        

        PVTable::load_pv_line(pvLine, MAX_DEPTH, position);
        bestMove = pvLine.moves[0];


        std::cout <<
        "info" << 
        " depth " << currentDepth << 
        " score cp " << bestMoveScore << 
        " move " << algebraic_move(bestMove) <<
        " nodes " << searchInfo.nodes <<
        " time " << timeMs;

        std::cout << " pv";

        for(DepthSize pvLineDepth = 0; pvLineDepth < pvLine.depth; ++pvLineDepth){
            std::cout << " " << algebraic_move(pvLine.moves[pvLineDepth]);
        }

        std::cout << std::endl;
        //std::cout << " score: " << bestMoveScore << " depth: " << currentDepth << "\n";
        //std::cout << "move:" << algebraic_move(veryBestMove) << " score: " << bestMoveScore << " depth: " << currentDepth << "\n";
        
    }

    std::cout << "bestmove " << algebraic_move(bestMove) << std::endl; 

}

Score alpha_beta(Position &position, SearchInfo &searchInfo, Score alpha, Score beta, DepthSize depth){

    if(depth==0){
        ++searchInfo.nodes;
        return quiescence_search(position, searchInfo, alpha, beta);
    }

    if((searchInfo.nodes & 2047) == 0){
        check_time(searchInfo);
    }

    ++searchInfo.nodes;

    if(position.is_repetition() || position.get_fifty_moves_counter() >= 100){
        return 0;
    }

    MoveGen::MoveList moveList;
    MoveGen::generate_all_moves(position, moveList);

    Score score = -CHECKMATE_SCORE;
    Score oldAlpha = alpha;
    Move bestMove = 0;

    for(std::size_t mIndx = 0; mIndx < moveList.size; ++mIndx){

        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }
        //TODO imprimir para ver si se ejecuta el movimiento

        score = -alpha_beta(position, searchInfo, -beta, -alpha, depth - 1);
        position.undo_move();

        if(searchInfo.timeOver){
            return 0;
        }
        
        if(score>alpha){
            if(alpha>=beta){
                return beta;
            }
            alpha = score;
            bestMove = move;
        }
        //if(score >= beta)
        //    return beta;
        //if(score>alpha)
        //    alpha = score;

    }

    if(alpha != oldAlpha){
        PVTable::insert_entry(position, bestMove);
    }

    return alpha;
    
}

Score quiescence_search(Position &position, SearchInfo &searchInfo, Score alpha, Score beta){

    if((searchInfo.nodes & 2047) == 0){
        check_time(searchInfo);
    }

    ++searchInfo.nodes;

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
    MoveGen::generate_capture_moves(position, moveList);

    score = -CHECKMATE_SCORE;
    Score oldAlpha = alpha;
    Move bestMove = 0;

    for(std::size_t mIndx = 0; mIndx < moveList.size; ++mIndx){

        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }

        score = -quiescence_search(position, searchInfo, -beta, -alpha);
        position.undo_move();

        if(searchInfo.timeOver){
            return 0;
        }
        
        if(score>alpha){
            if(alpha>=beta){
                return beta;
            }
            alpha = score;
            bestMove = move;
        }
    }

    if(alpha != oldAlpha){
        PVTable::insert_entry(position, bestMove);
    }

    return alpha;

}

void clean_search_info(SearchInfo &searchInfo){
    searchInfo.nodes = 0;
}

void check_time(SearchInfo &searchInfo){
    if(searchInfo.stopTime != NO_TIME){
        Xake::Time timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();

        if(timeMs >= searchInfo.stopTime){
            searchInfo.timeOver = true;
        }
    }
}


void perft(Position &position, DepthSize depth){

   if(depth == 0){
        leafCounter++;
        return;
    } 

    MoveGen::MoveList moveList;
    MoveGen::generate_all_moves(position, moveList);

    for(std::size_t mIndx = 0; mIndx < moveList.size; ++mIndx){
        
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

    leafCounter = 0;
    NodesSize allNodesCounter = 0;
    DepthSize actualDepth = searchInfo.depth-1;

    MoveGen::MoveList moveList;
    MoveGen::generate_all_moves(position, moveList);

    std::cout << "\n";
    
    for(std::size_t mIndx = 0; mIndx < moveList.size;++mIndx){

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

    std::cout << "\n" << "Total nodes size: " << allNodesCounter << "\n\n";
    return allNodesCounter;

}
}
}