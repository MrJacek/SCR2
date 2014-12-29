/* 
 * File:   Env.h
 * Author: jhojczak
 *
 * Created on 29 grudnia 2014, 01:18
 */

#ifndef ENV_H
#define	ENV_H

#include "Utils.h"
void environmentRun();
void clientIncomming(int sock,char** board);
int createServerSocket();
#endif	/* ENV_H */

