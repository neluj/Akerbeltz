#include "evaluate.h"
#include "uci.h"

using namespace std;
using namespace Xake;

int main()
{   
    Evaluate::init();
    
    UCI::run();

    return 0;
}