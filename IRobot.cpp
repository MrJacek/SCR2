#include "IRobot.h"
#include "Utils.h"

void irobotRun() {
    sleep(1);
    int df = createClient();
    char** board = getBoard(df);
    printBoard(board, "IRobot");
    move(df, '1', '3');
    return;
}
