#include "search.h"
#include <iostream>
#include "movegen.h"
#include "position.h"
#include <iostream>

namespace Xake{

void Search::perft(int depth, Position &position){

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
        perft(depth-1, position);
        position.undo_move();
    }
     //std::cout << "\n";
}


void Search::perftTest(int depth, Position &position){

    leafCounter = 0;
    long allNodesCounter = 0;

    MoveList moveList;
    generate_all_moves(position, moveList);

    std::cout << "\n";
    
    for(std::size_t mIndx = 0; mIndx < moveList.size;++mIndx){

        Move move = moveList.moves[mIndx];
        if(!position.do_move(move)){
            continue;
        }

        long cumnodes = leafCounter;
        perft(depth-1, position);

        position.undo_move();

        long oldNodes = leafCounter - cumnodes;
        allNodesCounter += oldNodes;

        std::cout << algebraic_move(move) << ": " << oldNodes << "\n";

    }

    std::cout << "\n" << "Total nodes size: " << allNodesCounter << "\n\n";

}


}