/*
 * client_protocol.h
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 */

#ifndef CLIENT_PROTOCOL_H_
#define CLIENT_PROTOCOL_H_
#include "network.h"
#include "server_protocol.h" // remove this after bridge fixed

#include <stdio.h>

void chopN(char* str, size_t n);
int defineUser(int serverSocket);
Message* createMessage(char* commandStr, MessageType type, char* prefix);
int sendClientCommand(char* commandStr, int serverSocket, int mySocketfd);
char* addFileClientSide(char* filePath);
void getFileClientSide(char* filePath, char* fileBuffer);
int  client_start(char* hostname, int port);




#endif /* CLIENT_PROTOCOL_H_ */
