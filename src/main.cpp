#include "position.h"
#include "attacks.h"
#include "evaluate.h"
#include "uci.h"
#include "engine_info.h"

#include <iostream>

using namespace std;
using namespace Akerbeltz;

int main()
{   
    std::cout << ENGINE_NAME << " " << ENGINE_VERSION
              << " by " << ENGINE_AUTHOR
              << " (build " << __DATE__ << " " << __TIME__ << ")"
              << std::endl;

    Attacks::init();
    Position::init();
    Evaluate::init();
    UCI::run();

    return 0;
}