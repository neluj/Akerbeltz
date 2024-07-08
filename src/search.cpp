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
void clean_search_info(SearchInfo &searchInfo);

void search(Position &position, SearchInfo &searchInfo){

    int bestMoveScore = -CHECKMATE_SCORE;
    Move bestMove = 0;

    clean_search_info(searchInfo);

    for(DepthSize currentDepth = 1; currentDepth <= searchInfo.depth; ++currentDepth){
        
        bestMoveScore = alpha_beta(position, searchInfo, -CHECKMATE_SCORE, CHECKMATE_SCORE, currentDepth);

        PVTable::load_pv_line(pvLine, MAX_DEPTH, position);
        bestMove = pvLine.moves[0];

        std::cout << 
        "depth:" << currentDepth << 
        " score:" << bestMoveScore << 
        " move:" << algebraic_move(bestMove) <<
        " nodes:" << searchInfo.nodes;

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
        return Evaluate::calc_score(position);
    }

    ++searchInfo.nodes;

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
        
        if(score>alpha){
            if(alpha>=beta){
                return beta;
            }
            alpha = score;
            bestMove = move;
            // DELETEME
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

void clean_search_info(SearchInfo &searchInfo){
    searchInfo.nodes = 0;
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


NodesSize perftTest(Position &position, DepthSize depth){

    leafCounter = 0;
    NodesSize allNodesCounter = 0;

    MoveGen::MoveList moveList;
    MoveGen::generate_all_moves(position, moveList);

    //std::cout << "\n";
    
    for(std::size_t mIndx = 0; mIndx < moveList.size;++mIndx){

        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }

        long cumnodes = leafCounter;
        perft(position, depth-1);

        position.undo_move();

        long oldNodes = leafCounter - cumnodes;
        allNodesCounter += oldNodes;

        //std::cout << algebraic_move(move) << ": " << oldNodes << "\n";

    }

    std::cout << "\n" << "Total nodes size: " << allNodesCounter << "\n\n";
    return allNodesCounter;

}

}

}