#ifndef INCLUDE_SEARCH_H
#define INCLUDE_SEARCH_H

#include "types.h"
#include "move.h"

namespace Xake{

class Position;

namespace Search{

    using DepthSize = unsigned short int;
    using NodesSize = unsigned long long int;

    struct SearchInfo{
        unsigned short int depth;
        //DELETEME 
        Move bestMove;
    };

    NodesSize perftTest(Position &position, DepthSize depth);
    void search(Position &position,  SearchInfo &searchInfo);

}

}

#endif