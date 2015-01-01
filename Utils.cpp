#include "Utils.h"

std::random_device rdU;

bool move(int fd, char oldY, char oldX, char axisY, char axisX) {
    char buf[5];
    char responseBuf[1];
    int rc;
    buf[0] = 'M';
    buf[1] = oldY;
    buf[2] = oldX;
    buf[3] = axisY;
    buf[4] = axisX;
    std::cout << "Prepare move request\n";
    if (write(fd, buf, 5) != 5) {
        perror("write error");
        exit(-1);
    }
    std::cout << "Send move request\n";
    if ((rc = read(fd, responseBuf, 1)) > 0) {
        std::cout << "Receive move request response\n";
        return responseBuf[0] == '1';
    }
    
    return false;
}

/**
 * Utworzenie klienta.
 * @param type typ robota, H - Hero, E - enemy.
 * @return deskrpytor socketu.
 */
int createClient(char type) {
    struct sockaddr_un addr;
    char buf[100];
    int fd, rc;
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(-1);
    }
    memset(&addr, 0, sizeof (addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "socket", sizeof (addr.sun_path) - 1);
    if (connect(fd, (struct sockaddr*) &addr, sizeof (addr)) == -1) {
        perror("connect error");
        exit(-1);
    }
    
    //zarejestorwanie z typem
    if (write(fd, &type, 1) != 1) {
        perror("write error");
        exit(-1);
    }
    
    return fd;
}

/**
 * Funkcja pobiera polozenie poczatkowe figury.
 * @param fd deskryptor socketu
 * @return wskaznik na pierwszy element dwuelementowej tablicy wspolrzednych [Y,X]
 */
int* getInitialPosition(int fd) {
    int* responseBuf = new int[2];
    int rc;
    
    if ((rc = read(fd, responseBuf, 2*sizeof(int))) > 0) {
        std::cout << "Client: " << fd << ": " << "Received initial position\n";
        return &responseBuf[0];
    }
}

char** getBoard(int fd) {
    int rc;
    char buf[100];
    char** board;
    board = new char*[8];
    for (int i = 0; i < 8; i++) {
        board[i] = new char[8];
    }

    std::cout << "Client: " << fd << ": " << "Ask for board: " << fd << "\n";
    if (write(fd, ASK_BOARD, 1) != 1) {
        perror("write error");
        exit(-1);
    }
    std::cout << "Client: " << fd << ": " << "Wait for board: " << fd << "\n";
    if ((rc = read(fd, buf, 64)) > 0) {
        std::cout << "Client: " << fd << ": " << "Recive board.\n";
        int shift = 0;
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                board[y][x] = buf[shift + x];
            }
            shift += 8;
        }
    }

    if (rc == -1) {
        perror("read");
        exit(-1);
    } else if (rc == 0) {
        printf("EOF\n");
        close(fd);
    }
    return board;
}

char getPawn(int) {
    return rdU() % 6;
}

void writeBoardToBuffor(char** board, char* buffor) {
    int shift = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            buffor[x + shift] = board[y][x];
        }
        shift += 8;
    }
}

void printBoard(char** board, const char* name, char prevX, char prevY, char nextX, char nextY) {
    std::cout << "Thread: " << name << "\nBoard:\n"
        << " |12345678| \n"
        << "-+--------+-\n";

    for (int y = 7; y >= 0; y--) {
        std::cout << (char)((char)y+'A') << '|';

        for (int x = 0; x < 8; x++) {
            std::string prefix = "";
            std::string sufix = "";

            if (x == prevX && y == prevY) {
                prefix = "\e[1;31m"; //czerwony znak
                sufix = "\e[0m";
            } else if (x == nextX && y == nextY) {
                prefix = "\e[1;32m"; //zielony znak
                sufix = "\e[0m";
            }

            std::cout << prefix << board[y][x] << sufix;        
        }

        std::cout << "|" << (char)((char)y+'A') << "\n";
    }

    std::cout
        << "-+--------+-\n"
        << " |12345678| \n";
}

void refresh2() {
//    std::string s;
//
//    initscr();
//    while (1) {
//        for (int y = 0; y < 8; y++) {
//            for (int x = 0; x < 8; x++) {
//                s.append("0");
//            }
//            s.append("\n");
//        }
//        s.append("END");
//        printw(s.data()); //2
//        getch(); //3
//        //        sleep(1);
//    }
//
//    endwin(); //4 
}
