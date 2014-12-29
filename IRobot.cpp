#include "IRobot.h"
#include "Utils.h"

void irobotRun() {
    sleep(1);
    int df = createClient();
    
    //przydala by sie jakaś pętla?
    char** board = getBoard(df);
    printBoard(board, "IRobot");
    bool accepted = move(df, '1', '3');
    
    if (accepted) {
        //zrobić coś dalej
    } else {
       //zrobić coś innego
    }
    
    return;
}
