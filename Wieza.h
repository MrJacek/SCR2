/*
 * File:   Wieza.h
 * Author: jhojczak
 *
 * Created on 28 grudnia 2014, 22:45
 */

#ifndef WIEZA_H
#define	WIEZA_H

void wiezaRun();

char rookDistance(char x, char y);

bool rookCalculateMove(char **board, char currentX, char currentY, char &nextX, char &nextY);

void rookCalculateHorizontal(char **board, char currentX, char currentY,
                            char badX, char badY,
                            char &bestDistance, char &bestX, char &bestY);

void rookCalculateVertical(char **board, char currentX, char currentY,
                            char badX, char badY,
                            char &bestDistance, char &bestX, char &bestY);

bool rookTestMove(char **board, char x, char y, char badX, char badY);
#endif	/* WIEZA_H */

