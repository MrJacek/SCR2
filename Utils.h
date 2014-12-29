/* 
 * File:   Utils.h
 * Author: jhojczak
 *
 * Created on 29 grudnia 2014, 01:15
 */

#ifndef UTILS_H
#define	UTILS_H
#include <iostream>
#include <thread>
#include <unistd.h>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <sys/un.h>

const char ASK_BOARD[]={'B'};
const char ASK_MOVE[]={'M'};
const char MOVE_ACCEPTED = '1';
const char MOVE_DENIED = '0';
const char TYPE_HERO = 'H';
const char TYPE_ENEMY = 'E';

char** getBoard(int);
bool move(int fd, char oldY, char oldX, char axisY, char axisX);
void refresh2();
int createClient(char type);
void writeBoardToBuffor(char** board, char* buffor);
void printBoard(char** board,const char* name);
int* getInitialPosition(int fd);

#endif	/* UTILS_H */

