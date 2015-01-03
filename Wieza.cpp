#include "Wieza.h"

void wiezaRun(){
    sleep(1);
    char currentX = 0; //współrzędne w zakresie 0-7 (cyfrowo, nie analogowo)
    char currentY = 0;
    char nextX = 0;
    char nextY = 0;
    int doNothing = 0; //poselski licznik nic nierobienia
    
	//utworzenie polaczenia z srodowiskiem
    int df = createClient(PAWN_ROOK);
	printf("Hello from ROOK");

    while (distance(currentX, currentY) > 0)
    {
        char** board = getBoard(df);
        char pawn = getPawn(df);
		
        //optymalizacyjnie poprawnie - drugie bez isMove się nie wykona
        bool isMove = calculateMove(board, pawn, currentX, currentY, nextX, nextY);
        bool accepted = isMove && move(df, currentY+0x30, currentX+0x30, nextY+0x30, nextX+0x30);

        if (isMove && accepted) {
            board = getBoard(df);
            printf("Pawn: Like %s: %c%d -> %c%d\n", PAWNS[pawn], currentY + 'A', currentX+1, nextY + 'A', nextX+1);
            printBoard(board, "Rook", currentX, currentY, nextX, nextY);

            //zaktualizować obecną pozycję
            currentX = nextX;
            currentY = nextY;
            doNothing = 0;
            nextX = 0;
            nextY = 0;
        } else {
            doNothing++;

            if (doNothing % 10 == 0) {
                printf("Rook: do nothing: %d\n", doNothing);
            }
        }
    }

    std::cout << "Rook: mission complete!\n";
  
    return;
}



bool calculateMove(char **board, char currentX, char currentY, char &nextX, char &nextY) {
    char bestX = 0;
    char bestY = 0;
    char bestDistance = distance(currentX, currentY);
	
	calculateHorizontal(board, currentX, currentY, nextX, nextY, bestDistance, bestX, bestY);
	calculateVertical(board, currentX, currentY, nextX, nextY, bestDistance, bestX, bestY);
    
	
    if ((bestX + bestY) != 0) {
        nextX = bestX;
        nextY = bestY;
        return true;
    }

    return false;
}

char distance(char x, char y) {
    if (x < 0 || x > 7
        || y < 0 || y > 7
    ) {
        perror("Rook: Bad position\n");
        return 255;
    }

    return 14 - x - y; //7,7 - górny lewy róg
}

void calculateHorizontal(char **board, char currentX, char currentY,
        char badX, char badY,
        char &bestDistance, char &bestX, char &bestY
) {
    for (char x=7; x>currentX; x--) {
        if (testMove(board, x, currentY, badX, badY)) {
            char newDistance = distance(x, currentY);
            if (newDistance < bestDistance) {
                bestDistance = newDistance;
                bestX = x;
                bestY = currentY;
            }

            break; //dystans już się nie polepszy
        }
    }
}

void calculateVertical(char **board, char currentX, char currentY,
        char badX, char badY,
        char &bestDistance, char &bestX, char &bestY
) {
    for (char y=7; y>currentY; y--) {
        if (testMove(board, currentX, y, badX, badY)) {
            char newDistance = distance(currentX, y);
            if (newDistance < bestDistance) {
                bestDistance = newDistance;
                bestX = currentX;
                bestY = y;
            }

            break; //dystans już się nie polepszy
        }
    }
}

bool testMove(char **board, char x, char y, char badX, char badY) {
    return  x >= 0 && x <= 7
            && y >= 0 && y <= 7
            && board[y][x] != '0'
            && x != badX
            && y != badY;
}
