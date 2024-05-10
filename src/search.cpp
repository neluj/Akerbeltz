#include "search.h"

#include <iostream>

#include "movegen.h"
#include "position.h"

namespace Xake{

namespace Search{

using namespace Evaluate;

static NodesSize leafCounter;
void perft(Position &position, DepthSize depth);

Score alpha_beta(Position &position, SearchInfo &searchInfo, Score alpha, Score beta, DepthSize depth);

void search(Position &position, SearchInfo &searchInfo){

    int bestMoveScore = -CHECKMATE_SCORE;

    for(DepthSize currentDepth = 1; currentDepth <= searchInfo.depth; ++currentDepth){
        
        bestMoveScore = alpha_beta(position, searchInfo, -CHECKMATE_SCORE, CHECKMATE_SCORE, bestMoveScore);
        std::cout << "best score: " << bestMoveScore << " depth: " << currentDepth;
        
    }

}

Score alpha_beta(Position &position, SearchInfo &searchInfo, Score alpha, Score beta, DepthSize depth){

    if(depth==0){
        return Evaluate::calc_score(position);
    }

    MoveGen::MoveList moveList;
    MoveGen::generate_all_moves(position, moveList);

    Score score = -CHECKMATE_SCORE;

    for(std::size_t mIndx = 0; mIndx < moveList.size; ++mIndx){

        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }

        score = -alpha_beta(position, searchInfo, -beta, -alpha, depth - 1);
        position.undo_move();
        
        if(score>alpha){
            if(alpha>=beta){
                return beta;
            }
            alpha = score;
        }

    }

    return alpha;
    
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