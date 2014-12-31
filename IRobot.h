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
bool testMove(char **board, char x, char y, char badX, char badY);

#endif	/* IROBOT_H */

