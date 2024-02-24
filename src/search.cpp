#include "search.h"
#include <iostream>
#include "movegen.h"
#include "position.h"
#include <iostream>

namespace Xake{

void Search::perft(Position &position, int depth){

   if(depth == 0){
        leafCounter++;
        return;
    } 

    MoveList moveList;
    generate_all_moves(position, moveList);

    for(std::size_t mIndx = 0; mIndx < moveList.size;++mIndx){
        
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


long Search::perftTest(Position &position, int depth){

    leafCounter = 0;
    long allNodesCounter = 0;

    MoveList moveList;
    generate_all_moves(position, moveList);

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