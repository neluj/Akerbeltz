#ifndef INCLUDE_SEARCH_H
#define INCLUDE_SEARCH_H

#include "types.h"
#include "move.h"

#include <atomic>

namespace Xake{

class Position;

namespace Search{
    struct SearchInfo{
        DepthSize depth;
        NodesSize nodes;
        DepthSize searchPly;
        Xake::Time startTime;
        Xake::Time stopTime;
        Xake::Time moveTime;
        Xake::Time realTime;
        bool timeOver;
        std::atomic_bool stop;
    };

    NodesSize perftTest(Position &position, SearchInfo &searchInfo);
    void search(Position &position, SearchInfo &searchInfo);

}

}

#endif
