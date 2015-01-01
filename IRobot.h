/* 
 * File:   IRobot.h
 * Author: jhojczak
 *
 * Created on 28 grudnia 2014, 22:47
 */

#ifndef IROBOT_H
#define	IROBOT_H


void irobotRun();
char distance(char x, char y);
bool calculateMove(char **board, char pawn, char currentX, char currentY, char &nextX, char &nextY);
void calculateDiagonal(char **board, char currentX, char currentY,
        char badX, char badY,
        char &bestDistance, char &bestX, char &bestY);
void calculateHorizontal(char **board, char currentX, char currentY,
        char badX, char badY,
        char &bestDistance, char &bestX, char &bestY);
void calculateVertical(char **board, char currentX, char currentY,
        char badX, char badY,
        char &bestDistance, char &bestX, char &bestY);
bool testMove(char **board, char x, char y, char badX, char badY);

#endif	/* IROBOT_H */

