/*
 * server_protocol.h
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 */

#ifndef SERVER_PROTOCOL_H_
#define SERVER_PROTOCOL_H_
#include "network.h"


typedef struct user_t {
	const char* user_name;
	const char* password;
	const char* dir_path;
} User;



#endif /* SERVER_PROTOCOL_H_ */
