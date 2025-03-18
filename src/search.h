#ifndef INCLUDE_SEARCH_H
#define INCLUDE_SEARCH_H

#include "types.h"
#include "move.h"

namespace Xake{

class Position;

namespace Search{
    struct SearchInfo{
        DepthSize depth;
        NodesSize nodes;
        Xake::Time startTime;
        Xake::Time stopTime;
        Xake::Time moveTime;
        bool timeOver;
    };

    NodesSize perftTest(Position &position, SearchInfo &searchInfo);
    void search(Position &position, SearchInfo &searchInfo);

}

}

#endif