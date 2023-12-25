#ifndef INCLUDE_SEARCH_H
#define INCLUDE_SEARCH_H

#include "types.h"

namespace Xake{

class Position;

namespace Search{

    static long leafCounter;
    void perft(int depth, Position &position);
    void perftTest(int depth, Position &position);

}

}

#endif