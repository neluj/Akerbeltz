#ifndef INCLUDE_SEARCH_H
#define INCLUDE_SEARCH_H

#include "types.h"
#include "move.h"

namespace Xake{

class Position;

namespace Search{
    struct SearchInfo{
        unsigned short int depth;
    };

    NodesSize perftTest(Position &position, DepthSize depth);
    void search(Position &position,  SearchInfo &searchInfo);

}

}

#endif