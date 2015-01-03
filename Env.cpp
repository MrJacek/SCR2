#include "Env.h"

std::mutex board_state_mutex;
std::random_device rd;
std::minstd_rand0 gen(rd());
std::uniform_int_distribution<> dis(0, 7);
char** board;

void environmentRun() {
    createServerSocket();
}

int createServerSocket() {
    struct sockaddr_un addr;

    //utworzymy stan srodowiska w chwili 0
    board = new char*[8];
    for (int i = 0; i < 8; i++) {
        board[i] = new char[8];
        for (int j = 0; j < 8; j++) {
            board[i][j] = '1';
        }
    }

    int fd, cl, rc;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }
    memset(&addr, 0, sizeof (addr));
    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, "socket", sizeof (addr.sun_path) - 1);
    unlink("socket");
    if (bind(fd, (struct sockaddr*) &addr, sizeof (addr)) == -1) {
        perror("bind error");
        exit(-1);
    }
    if (listen(fd, 5) == -1) {
        perror("listen error");
        exit(-1);
    }
    while (1) {
        if ((cl = accept(fd, NULL, NULL)) == -1) {
            perror("accept error");
            continue;
        }
        std::cout << "Start thread for client: " << cl << "\n";
        std::thread* t = new std::thread(clientIncomming, cl);
    }
}

/**
 * Funkcja obsługi klienta wykonowyna w oddzielnym wątku dla każdego socketa
 * @param sock deskryptor socketa
 */
void clientIncomming(int sock) {
    char type;
    int rc;
    char buf[5];
    char boardBuf[64];

    std::cout << "Env: " << sock << ": " << "New service thread created!\n";
    //jaki typ bota się zarejestrował
    if (read(sock, &type, 1) > 0) {
        std::cout << "Env: " << sock << ": " << "Client type = [" << type << "] registered!\n";
    }

    if (type == 'E') {
        //typ enemy
        //losowa pozycja poczatkowa (z wyjatkiem [0,0])
        int* initialPos = new int[2];
        randomInitialPosition(initialPos);
        std::cout << "Env: " << sock << ": " << "Generated initial position = [" << initialPos[0] << ", " << initialPos[1] << "]!\n";

        if (write(sock, &(*initialPos), 2 * sizeof (int)) != 2 * sizeof (int)) {
            perror("Initial position socket write error");
            exit(-1);
        }
    } else {
        //typ hero
        //<!-- CRITICAL SECTION
            board_state_mutex.lock();
            board[0][0] = '0';
            board_state_mutex.unlock();
        //-->
    }

    while ((rc = read(sock, buf, sizeof (buf))) > 0) {
       // printf("read %u bytes: %.*s\n", rc, rc, buf);

        //pytanie o stan szachownicy - odbrany tylko char B, dwa pozostale bez znaczenia
        if (buf[0] == ASK_BOARD[0]) {
        //    std::cout << "Env: " << sock << ": " << "Get board\n";

            //odpowiedź - stan szachownicy
            //<!-- CRITICAL SECTION
            board_state_mutex.lock();

            //przepisanie stanu szachownicy na liniowy bufor
            writeBoardToBuffor(board, boardBuf);
        //    std::cout << "Env: " << sock << ": " << boardBuf << "end" << "\n";

            if (write(sock, boardBuf, sizeof (boardBuf)) != sizeof (boardBuf)) {
                if (64 > 0) fprintf(stderr, "partial write");
                else {
                    perror("write error");
                    exit(-1);
                }
            }
         //   std::cout << "Env: " << sock << ": " << "Write board!\n";

            board_state_mutex.unlock();
            //-->
        }

        //prosba o ruch - kod operacji M oraz stare i nowe wspolrzedne, np: M1213
        if (buf[0] == 'M') {
          //  std::cout << "Env: " << sock << ": " << "Ask for move: [" << buf[1] << "," << buf[2] << "] -> [" << buf[3] << "," << buf[4] << "]\n";

            //Konwersja char na int i sanity check
            int oldY = ((int) buf[1]) - 48;
            int oldX = ((int) buf[2]) - 48;
            int newY = ((int) buf[3]) - 48;
            int newX = ((int) buf[4]) - 48;

            bool badCoord = false;
            if (oldX < 0 || oldX > 7) {
                perror("Wsp. X poza zakresem 0-8!\n");
                badCoord = true;
            }
            if (oldY < 0 || oldY > 7) {
                perror("Wsp. Y poza zakresem 0-8!\n");
                badCoord = true;
            }
            if (newX < 0 || newX > 7) {
                perror("Wsp. X poza zakresem 0-8!\n");
                badCoord = true;
            }
            if (newY < 0 || newX > 7) {
                perror("Wsp. Y poza zakresem 0-8!\n");
                badCoord = true;
            }

            //odpowiedz = tak / nie
            char status = MOVE_DENIED;

            //<!-- CRITICAL SECTION
            board_state_mutex.lock();
            bool accepted = moveAccepted(oldX, oldY, newX, newY);
            board_state_mutex.unlock();
            //-->

            if (!badCoord && accepted) {
         //       std::cout << "Env: " << sock << ": " << "Move accepted\n";
                status = MOVE_ACCEPTED;
            } else {
       //         std::cout << "Env: " << sock << ": " << "Move denied\n";
            }

            //zwrocenie odpowiedzi
            if (write(sock, &status, 1) != 1) {
                perror("Move status socket write error");
                exit(-1);
            }
        }

        bzero(buf, 5);
        bzero(boardBuf, 64);
    }

    if (rc == -1) {
        perror("read");
        exit(-1);
    } else if (rc == 0) {
        printf("EOF\n");
        close(sock);
    }
}

/**
 * Sprawdza czy ruch o który prosi wątek jest możliwy.
 * @return true jeżeli jest, false w p.p.
 */
bool moveAccepted(int sourceX, int sourceY, int targetX, int targetY) {
 //   std::cout << "Checking move: [" << sourceY << "," << sourceX << "] -> [" << targetY << "," << targetX << "]\n";
    printBoard(board, "Env", sourceX, sourceY, targetX, targetY);
    if (board[sourceY][sourceX] != '0') {
        perror("Na zrodlowym polu brak figury!\n");
        return false;
    }

    //Ruch możliwy jeżeli na danym polu nic nie stoi - wartość = 1
    if (board[targetY][targetX] == '1') {
        //to pole od teraz jest zajete
        board[targetY][targetX] = '0';
        //a stare zostanie zwolnione
        board[sourceY][sourceX] = '1';

        return true;
    }

    return false;
}

/**
 * Wygeneruje pozycje startową dla klienta.
 * Pozycja nie może być (0,0) - tam startuje robot.
 * Generowane pozycje naniesiona na mape szachownicy.
 *
 * @param array tablica do wypelnienia
 */
void randomInitialPosition(int array[]) {
    int startX = 0;
    int startY = 0;

    //<!-- CRITICAL SECTION
    board_state_mutex.lock();
    while (startX == 0 || board[startY][startX] == '0') {
        startX = dis(gen);
        startY = dis(gen);
    }

    board[startY][startX] = '0';
    board_state_mutex.unlock();
    //-->

    array[0] = startY;
    array[1] = startX;
}
