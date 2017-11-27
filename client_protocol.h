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
void createMessageCommand(Message* m,  MessageType type, char* prefix);
void createQuitCommand(Message* m);
int sendClientCommand(char* commandStr, int mySocketfd);
char* addFileClientSide(char* filePath);
void getFileClientSide(char* filePath, char* fileBuffer);
int  client_start(char* hostname, int port);
int listOfFilesCommand(Message* m, char* commandStr,  int mySocketfd);
int deleteFileCommand(Message* m, char* commandStr, char* inputPrefix,
		 int mySocket);
int addFileCommand(Message* m, char* commandStr, char* inputPrefix,
		int mySocket);
int getFileCommand(Message* m, char* commandStr, char* inputPrefix,
		int mySocket) ;






#endif /* CLIENT_PROTOCOL_H_ */
