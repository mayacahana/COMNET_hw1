/*
 * client_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 *
 */

#include "client_protocol.h"

char* getUserDetails() {
	char* userDetails[MAX_USERNAME_SIZE];
	int read = getline(&userDetails, 40, stdin);
	if (read != -1) {
		puts(userDetails);
		return userDetails;
	}
	printf("No Line Read");
	return NULL ;
}
/*
 * @param pointer to string and size_t variable n
 * removes last n bits from the string.
 */
void chopN(char *str, size_t n) {
	assert(n != 0 && str != 0);
	size_t len = strlen(str);
	if (n > len)
		return;
	memmove(str, str + n, len - n + 1);
}

/*
 * sends server username and password
 * returns 0 if succeeds, otherwise returns 1
 */
int defineUser() {
	Message* m = (Message*) malloc(sizeof(Message));
	int status = 0;
	//get username and password
	char* fullUsername = getUserDetails();
	char* fullPassword = getUserDetails();
	if (fullUsername == NULL || fullPassword == NULL ) {
		return 1;
	}
	char* userPrefix[7];
	char* passwordPrefix[11];
	strncpy(userPrefix, fullUsername);
	strncpy(passwordPrefix, fullPassword);
	userPrefix[7] = "\0"; // "User: " if in correct format
	passwordPrefix[11] = "\0"; // "Password: " if in correct format
	//check if input is in correct format and create message
	if ((strcmp(userPrefix, "User: ") == 0)
			&& (strcmp(passwordPrefix, "Password: ") == 0)) {
		char* username[26], password[26];
		username = chopN(fullUsername, 6);
		password = chopN(fullPassword, 10);
		m->type = LOGIN_DETAILS;
		m->arg1 = username;
		m->arg2 = password;
		m->fromClient = 1;
		status = send_command(,m);
	} else {
		printf("wrong format");	// fix this
		status = 1;
	}
	free(m);
	return status;
}

Message* createMessage(char* commandStr, MessageType type, char* prefix) {
	Message* m = (Message*) malloc(sizeof(Message));
	if (m == NULL )
		return NULL ;
	m->type = type;
	m->fromClient = 1;
	m->arg1 = prefix;
	chopN(commandStr, strlen(prefix));
	m->arg2 = commandStr;
	return m;
}

int sendClientCommand(char* commandStr, int serverSocket, int mySocketfd) {
	int status = 0;
	char* inputPrefix[14];
	strncpy(inputPrefix, commandStr);
	inputPrefix[14] = "\0";
	if (strcmp(inputPrefix, "list_of_files") == 0) {
		Message* m = createMessage(commandStr, LIST_OF_FILES, inputPrefix);
		if (m != NULL ) {
			status = send_command(serverSocket, m);
			return status;
		}
		printf("message not created");
		return 1;
	}
	inputPrefix[11] = "\0";
	if (strcmp(inputPrefix, "delete_file") == 0) {
		Message* m = createMessage(commandStr, DELETE_FILE, inputPrefix);
		if (m != NULL ) {
			status = send_command(serverSocket, m);
			return status;
		}
		printf("message not created");
		return 1;
	}
	inputPrefix[8] = "\0";
	if (strcmp(inputPrefix, "add_file") == 0) {
		Message* m = createMessage(commandStr, ADD_FILE, inputPrefix);
		if (m != NULL ) {
			status = send_command(serverSocket, m);
			return status;
		}
		printf("message not created");
		return 1;
	}
	if (strcmp(inputPrefix, "get_file") == 0) {
		Message* m = createMessage(commandStr, GET_FILE, inputPrefix);
		if (m != NULL ) {
			status = send_command(serverSocket, m);
			return status;
		}
		printf("message not created");
		return 1;
	}
	inputPrefix[4] = "\0";
	if (strcmp(inputPrefix, "quit") == 0) {
		if (close(mySocketfd) == -1)
		{
			printf("close() failed: %s\n", strerror(errno));
			return 1;
		}
	}

	return 1;
}

void startClient(char* hostname, int port) {
	if (hostname == NULL ) {
		hostname[10] = "localhost";
		port = 2235;
	} else if (port == 0)
		port = 2235;

	int status;
	int socketfd;
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);

	if (socketfd < 0) {
		printf("Could not create socket\n");
		return;
	}

	struct sockaddr_in my_addr, server_addr;

	socklen_t server_size = sizeof(server_addr);

	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(port);
	dest_addr.sin_addr = INADDR_LOOPBACK; // to do??

	int connect;
	connect = connect(socketfd, (struct sockaddr*)&server_addr, sizeof(struct server_addr));
	if (connect < 0){
		close(socketfd);
		printf("connection failed");
		return;
	}
	status = defineUser();

	while (status == 0) {
		int maxLen = MAX_COMMAND_NAME + MAX_PATH_NAME + MAX_FILE_NAME;
		char* inputStr[maxLen];
		int read = getline(&inputStr, maxLen, stdin);
		if (read != -1) {
			puts(inputStr);
			status = sendClientCommand(inputStr, dest_addr);
		} else
			status = 1;
	}

	if (close(sockfd) == -1)
	{
		printf("close() failed: %s\n", strerror(errno));
		return 1;
	}


	return status;
}

