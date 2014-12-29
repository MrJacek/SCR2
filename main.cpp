
#include "main.h"
#include "Env.h"
#include "IRobot.h"


int main() {
    std::thread env(environmentRun);
    std::thread robot(irobotRun);
    std::thread robot2(irobotRun);

    
    std::cout << "main, foo and bar now execute concurrently...\n";

    // synchronize threads:
    env.join();
    std::cout << "foo and bar completed.\n";
    return 0;
}

