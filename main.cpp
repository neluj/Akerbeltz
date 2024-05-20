#include "position.h"
#include "evaluate.h"
#include "uci.h"

using namespace std;
using namespace Xake;

int main()
{   
    Position::init();
    Evaluate::init();
    
    UCI::run();

    return 0;
}