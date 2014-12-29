/* 
 * File:   Env.h
 * Author: jhojczak
 *
 * Created on 29 grudnia 2014, 01:18
 */

#ifndef ENV_H
#define	ENV_H

#include "Utils.h"
#include <mutex>
#include <random>

void environmentRun();
void clientIncomming(int sock);
int createServerSocket();
bool moveAccepted(int oldX, int oldY, int targetX, int targetY);
void randomInitialPosition(int array[]);
#endif	/* ENV_H */

