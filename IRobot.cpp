#include "IRobot.h"
#include "Utils.h"

//tutaj ta tablica bo w utills.c miałem błąd z duplikacją, pewnie chodzi o dołączanie plików, mniejsza o to
const char* PAWNS[]={"King", "Queen", "Rook", "Bishop", "Knight", "Pawn"};

void irobotRun() {
    sleep(1);
    char currentX = 0; //współrzędne w zakresie 0-7 (cyfrowo, nie analogowo)
    char currentY = 0;
    char nextX = 0;
    char nextY = 0;
    int doNothing = 0; //poselski licznik nic nierobienia
    
    //utworzenie polaczenia z srodowiskiem
    int df = createClient(TYPE_HERO);
    
//    //pobranie pozycji poczatkowej
//    if (type == TYPE_ENEMY) {
//        int* position = getInitialPosition(df);
//        currentY = (char) *position;
//        currentX = (char) *(position + 1);
//    }



    while (distance(currentX, currentY) > 0)
    {
        char** board = getBoard(df);
        char pawn = getPawn(df);        

        //optymalizacyjnie poprawnie - drugie bez isMove się nie wykona
        bool isMove = calculateMove(board, pawn, currentX, currentY, nextX, nextY);
        bool accepted = isMove && move(df, currentY+0x30, currentX+0x30, nextY+0x30, nextX+0x30);

        if (isMove && accepted) {
            printf("iRobot: Like %s: %c%d -> %c%d\n", PAWNS[pawn], currentY + 'A', currentX, nextY + 'A', nextX);
            printBoard(board, "IRobot");

            //zaktualizować obecną pozycję
            currentX = nextX;
            currentY = nextY;
            doNothing = 0;
            nextX = 0;
            nextY = 0;
        } else {
            doNothing++;

            if (doNothing % 10 == 0) {
                printf("iRobot: do nothing: %d\n", doNothing);
            }
        }
    }

    std::cout << "iRobot: mission complete!\n";
  
    return;
}

/**
 * Obliczenie dystansu do punktu docelowego. Im mniej tym lepiej.
 * 
 * współrzędne w zakresie 0-7 (cyfrowo, nie analogowo)
 *  
 */
char distance(char x, char y) {
    if (x < 0 || x > 7
        || y < 0 || y > 7
    ) {
        perror("iRobot: Bad position\n");
        return 255;
    }

    return 14 - x - y; //7,7 - górny lewy róg
}

/**
 * Szukanie najlepszego możliwego ruchu dla danego pionka
 * Naiwnie - tam gdzie mniejsza odleglość. Pomijamy punkt nextX, nextY,
 * ponieważ różny od zera oznacza poprzednio nieudany ruch. A jeśli równy 0,0 to nie interesuje,
 * więc też nieporządany. Nowy ruch trafi do nextX, nextY.
 * Zwraca true jeśli jest nowy ruch
 *
 * współrzędne w zakresie 0-7 (cyfrowo, nie analogowo)
 *
 */
bool calculateMove(char **board, char pawn, char currentX, char currentY, char &nextX, char &nextY) {
    char bestX = 0;
    char bestY = 0;
    char bestDistance = distance(currentX, currentY);

    switch (pawn) {
        case PAWN_KING:
            //tylko trzy ruchy króla mogą polepszyć sprawę
            if (testMove(board, currentX+1, currentY+1, nextX, nextY)) {
                bestX = currentX + 1;
                bestY = currentY + 1;
            } else if (testMove(board, currentX, currentY+1, nextX, nextY)) {
                bestX = currentX;
                bestY = currentY + 1;
            } else if (testMove(board, currentX+1, currentY, nextX, nextY)) {
                bestX = currentX + 1;
                bestY = currentY;
            }
            break;
        case PAWN_QUEEN://todo
        case PAWN_ROOK:
        case PAWN_BISHOP:
        case PAWN_KNIGHT:
            break;
        case PAWN_PAWN:
            if (testMove(board, currentX, currentY+1, nextX, nextY)) {
                bestX = currentX;//w górę na pewno dystans się polepszy
                bestY = currentY + 1;
            }
            break;
        default:
            perror("iRobot: this pawn is not member of Staunton chess set");
    }

    if ((bestX + bestY) != 0) {
        nextX = bestX;
        nextY = bestY;
        return true;
    }

    return false;
}

/**
 * Sprawdzenie czy dany ruch jest możliwy na plaszny, w tym sprawdzenie poprawności współrzędnych
 * Pomijamy punkt badX, badY, ponieważ różny od zera oznacza poprzednio nieudany ruch. A jeśli równy 0,0 to nie interesuje,
 * więc też nieporządany.
 *
 * współrzędne w zakresie 0-7 (cyfrowo, nie analogowo)
 *
 */
bool testMove(char **board, char x, char y, char badX, char badY) {
    return  x >= 0 && x <= 7
            && y >= 0 && y <= 7
            && board[y][x] != '0'
            && x != badX
            && y != badY;
}