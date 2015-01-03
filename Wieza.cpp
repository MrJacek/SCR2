#include "Utils.h"
#include "Wieza.h"

void wiezaRun(){
    sleep(1);
    char currentX = 0; //współrzędne w zakresie 0-7 (cyfrowo, nie analogowo)
    char currentY = 0;
    char nextX = 0;
    char nextY = 0;
    int doNothing = 0; //poselski licznik nic nierobienia

	//utworzenie polaczenia z srodowiskiem
    int df = createClient(TYPE_ENEMY);
	printf("Hello from ROOK");

	 //pobranie pozycji poczatkowej
    int* position = getInitialPosition(df);
    currentY = (char) *position + 48;
    currentX = (char) *(position + 1) + 48;

    //na sztywno wieza
    char pawn = PAWN_ROOK;

    while (rookDistance(currentX, currentY) > 0)
    {
        char** board = getBoard(df);

        //optymalizacyjnie poprawnie - drugie bez isMove się nie wykona
        bool isMove = rookCalculateMove(board, currentX, currentY, nextX, nextY);
        bool accepted = isMove && move(df, currentY+0x30, currentX+0x30, nextY+0x30, nextX+0x30);

        if (isMove && accepted) {
            board = getBoard(df);
            printf("Rook: %c%d -> %c%d\n", currentY + 'A', currentX+1, nextY + 'A', nextX+1);
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

bool rookCalculateMove(char **board, char currentX, char currentY, char &nextX, char &nextY) {
    char bestX = 0;
    char bestY = 0;
    char bestDistance = rookDistance(currentX, currentY);

	rookCalculateHorizontal(board, currentX, currentY, nextX, nextY, bestDistance, bestX, bestY);
	rookCalculateVertical(board, currentX, currentY, nextX, nextY, bestDistance, bestX, bestY);


    if ((bestX + bestY) != 0) {
        nextX = bestX;
        nextY = bestY;
        return true;
    }

    return false;
}

char rookDistance(char x, char y) {
    if (x < 0 || x > 7
        || y < 0 || y > 7
    ) {
        perror("Rook: Bad position\n");
        return 255;
    }

    return 14 - x - y; //7,7 - górny lewy róg
}

void rookCalculateHorizontal(char **board, char currentX, char currentY,
        char badX, char badY,
        char &bestDistance, char &bestX, char &bestY
) {
    for (char x=7; x>currentX; x--) {
        if (rookTestMove(board, x, currentY, badX, badY)) {
            char newDistance = rookDistance(x, currentY);
            if (newDistance < bestDistance) {
                bestDistance = newDistance;
                bestX = x;
                bestY = currentY;
            }

            break; //dystans już się nie polepszy
        }
    }
}

void rookCalculateVertical(char **board, char currentX, char currentY,
        char badX, char badY,
        char &bestDistance, char &bestX, char &bestY
) {
    for (char y=7; y>currentY; y--) {
        if (rookTestMove(board, currentX, y, badX, badY)) {
            char newDistance = rookDistance(currentX, y);
            if (newDistance < bestDistance) {
                bestDistance = newDistance;
                bestX = currentX;
                bestY = y;
            }

            break; //dystans już się nie polepszy
        }
    }
}

bool rookTestMove(char **board, char x, char y, char badX, char badY) {
    return  x >= 0 && x <= 7
            && y >= 0 && y <= 7
            && board[y][x] != '0'
            && x != badX
            && y != badY;
}
