#include "position.h"
#include "attacks.h"
#include "evaluate.h"
#include "uci.h"

using namespace std;
using namespace Xake;

int main()
{   
    Attacks::init();
    Position::init();
    Evaluate::init();
    
    UCI::run();

    return 0;
}