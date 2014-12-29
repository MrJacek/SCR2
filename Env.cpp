#include "Env.h"

std::mutex board_state_mutex;
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
    int rc;
    char buf[3];
    char boardBuf[64];

    //obsluga żądania klienta
    std::cout << "Empty thread for: " << sock << "\n";
    while ((rc = read(sock, buf, sizeof (buf))) > 0) {
        printf("read %u bytes: %.*s\n", rc, rc, buf);

        //pytanie o stan szachownicy - odbrany tylko char B, dwa pozostale bez znaczenia
        if (buf[0] == ASK_BOARD[0]) {
            std::cout << "Get board" << "\n";

            //przepisanie stanu szachownicy na liniowy bufor
            writeBoardToBuffor(board, boardBuf);
            std::cout << boardBuf << "end" << "\n";

            //odpowiedź - stan szachownicy
            //<!-- CRITICAL SECTION
            board_state_mutex.lock();
            if (write(sock, boardBuf, sizeof (boardBuf)) != sizeof (boardBuf)) {
                if (64 > 0) fprintf(stderr, "partial write");
                else {
                    perror("write error");
                    exit(-1);
                }
            }
            std::cout << "Write board:" << sock << "\n";
            board_state_mutex.unlock();
            //-->
        }

        //prosba o ruch - odebrane M oraz nowe wspolrzedne, np: M13
        if (buf[0] == 'M') {
            std::cout << "Ask for move: [" << buf[1] << "," << buf[2] << "]\n";

            //Konwersja char na int i sanity check
            int x = ((int) buf[1]) - 48;
            int y = ((int) buf[2]) - 48;

            bool badCoord = false;
            if (x < 0 || x > 7) {
                perror("Wsp. X poza zakresem 0-8!\n");
                badCoord = true;
            }
            if (y < 0 || y > 7) {
                perror("Wsp. Y poza zakresem 0-8!\n");
                badCoord = true;
            }

            //odpowiedz = tak / nie
            char status = MOVE_DENIED;

            //<!-- CRITICAL SECTION
            board_state_mutex.lock();
            bool accepted = moveAccepted(x, y);
            board_state_mutex.unlock();
            //-->

            if (badCoord && accepted) {
                std::cout << "Move accepted\n";
                status = MOVE_ACCEPTED;
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
bool moveAccepted(int targetX, int targetY) {
    std::cout << "Checking move: [" << targetX << "," << targetY << "]\n";

    //Ruch możliwy jeżeli na danym polu nic nie stoi - wartość = 1
    if (board[targetY][targetX] == '1') {
        //to pole od teraz jest zajete
        board[targetY][targetX] = '0';
        return true;
    }

    return false;
}