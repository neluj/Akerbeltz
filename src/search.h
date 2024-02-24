#ifndef INCLUDE_SEARCH_H
#define INCLUDE_SEARCH_H

#include "types.h"

namespace Xake{

class Position;

namespace Search{

    static long leafCounter;
    void perft(Position &position, int depth);
    long perftTest(Position &position, int depth);

}

}

#endif