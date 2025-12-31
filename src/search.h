#ifndef INCLUDE_SEARCH_H
#define INCLUDE_SEARCH_H

#include "types.h"
#include "move.h"
#include "timemanager.h"

#include <atomic>

namespace Akerbeltz{

class Position;

namespace Search{
    struct SearchInfo{
        DepthSize depth;
        NodesSize nodes;
        DepthSize searchPly;
        Akerbeltz::TimeManager timeManager;
        std::atomic_bool stop;
    };

    NodesSize perftTest(Position &position, SearchInfo &searchInfo);
    void search(Position &position, SearchInfo &searchInfo);

}

}

#endif
