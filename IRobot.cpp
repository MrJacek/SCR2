#include "IRobot.h"
#include "Utils.h"

void irobotRun() {
    sleep(1);
    char currentX = '0';
    char currentY = '0';
    
    //utworzenie polaczenia z srodowiskiem
    int df = createClient(TYPE_ENEMY); //testowo ENEMY, IRobot powinien mieć HERO
    
    //pobranie pozycji poczatkowej
    int* position = getInitialPosition(df);
    currentY = (char) *position + 48;
    currentX = (char) *(position + 1) + 48;
    
    //pewnie przydala by sie jakaś pętla...
    char** board = getBoard(df);
    printBoard(board, "IRobot");
    bool accepted = move(df, currentY, currentX, '1', '3');

    if (accepted) {
        //zaktualizować obecną pozycję        
    } else {
       //zrobić coś innego
    }
  
    return;
}
