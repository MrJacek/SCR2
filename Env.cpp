#include "Env.h"

void environmentRun() {
    createServerSocket();
}

int createServerSocket() {
    struct sockaddr_un addr;
    char** board;
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
        std::thread* t = new std::thread(clientIncomming, cl, board);
    }
}

/**
 * Funkcja obsługi klienta wykonowyna w oddzielnym wątku dla każdego socketa
 * @param sock deskryptor socketa
 * @param board wskaznik na stan szachownicy
 */
void clientIncomming(int sock, char** board) {
    int rc;
    char buf[3];
    char boardBuf[64];

    //przepisanie stanu szachownicy na liniowy bufor
    writeBoardToBuffor(board, boardBuf);
    std::cout << boardBuf << "end" << "\n";

    //obsluga żądania klienta
    std::cout << "Empty thread for: " << sock << "\n";    
    while ((rc = read(sock, buf, sizeof (buf))) > 0) {
        printf("read %u bytes: %.*s\n", rc, rc, buf);
        //pytanie o stan szachownicy - odbrany tylko char B, dwa pozostale bez znaczenia
        if (buf[0] == ASK_BOARD[0]) {
            std::cout << "Get board" << "\n";
            if (write(sock, boardBuf, sizeof (boardBuf)) != sizeof (boardBuf)) {
                if (64 > 0) fprintf(stderr, "partial write");
                else {
                    perror("write error");
                    exit(-1);
                }
            }
            std::cout << "Write board:" << sock << "\n";
        }
        //prosba o ruch - odebrane M oraz nowe wspolrzedne, np: M13
        if (buf[0] == 'M') {
            std::cout << "Ask for move: [" << buf[1] << "," << buf[2] << "]\n";

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