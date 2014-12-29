#include "Utils.h"

bool move(int fd, char axisY, char axisX) {
    char buf[3];
    char responseBuf[1];
    int rc;
    buf[0] = 'M';
    buf[1] = axisY;
    buf[2] = axisX;
    std::cout << "Prepare move request\n";
    if (write(fd, buf, 3) != 3) {
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

int createClient() {
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
    return fd;
}

char** getBoard(int fd) {
    int rc;
    char buf[100];
    char** board;
    board = new char*[8];
    for (int i = 0; i < 8; i++) {
        board[i] = new char[8];
    }

    std::cout << "Ask for board: " << fd << "\n";
    if (write(fd, ASK_BOARD, 1) != 1) {
        perror("write error");
        exit(-1);
    }
    std::cout << "Wait for board: " << fd << "\n";
    if ((rc = read(fd, buf, 64)) > 0) {
        std::cout << "Recive board.\n";
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

void writeBoardToBuffor(char** board, char* buffor) {
    int shift = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            buffor[x + shift] = board[y][x];
        }
        shift += 8;

    }
}

void printBoard(char** board, const char* name) {
    std::cout << "Thread: " << name << "\nBoard:\n";
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            std::cout << board[y][x];
        }
        std::cout << "\n";
    }
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
