/*
 * client_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 *
 */

#include "client_protocol.h"

char* getUserDetails() {
	char* userDetails = NULL;
	size_t* n = 0;
	int read = getline(&userDetails, &n, stdin);
	if (read != -1) {
		puts(userDetails);
		return userDetails;
	}
	printf("No Line Read");
	return NULL;
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
int defineUser(int serverSocket) {
	Message* m = (Message*) malloc(sizeof(Message));
	int status = 0;
	//get username and password
	while (status) {
		char* fullPassword;
		char* fullUsername = getUserDetails();
		if (strcmp(fullUsername, "quit") != 0) {
			fullPassword = getUserDetails();
			if (strcmp(fullPassword, "quit") == 0) {
				m->type = QUIT;
			}
		} else {
			m->type = QUIT;
		}
		if (m->type == QUIT) {
			free(m);
			return 1;
		}
		if (fullUsername == NULL || fullPassword == NULL ) {
			return 1;
		}
		char* userPrefix[7];
		char* passwordPrefix[11];
		strcpy(userPrefix, fullUsername);
		strcpy(passwordPrefix, fullPassword);
		userPrefix[7] = '\0'; // "User: " if in correct format
		passwordPrefix[11] = '\0'; // "Password: " if in correct format
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
			status = send_command(m, sockfd);
			receive_command(serverSocket, m);
			if (strcmp(m->arg1, "WRONG") == 0) {
				printf("Wrong username or passoword. Please try again. \n");
			} else
				printf("%s", m->arg1);
		} else {
			printf("wrong format");	// fix this
			status = 1;
		}
	}	//while
	char* fullUsername = getUserDetails();
	char* fullPassword = getUserDetails();
	if (fullUsername == NULL || fullPassword == NULL) {
		return 1;
	}
	char* userPrefix[7];
	char* passwordPrefix[11];
	strcpy(userPrefix, fullUsername);
	strcpy(passwordPrefix, fullPassword);
	userPrefix[7] = '\0'; // "User: " if in correct format
	passwordPrefix[11] = '\0'; // "Password: " if in correct format
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
		status = send_command(m, sockfd);

		receive_command(serverSocket, m);
		if (strcmp(m->arg1, "Wrong") == 0) {
			printf("Wrong username or passoword. Please try again. \n");
		} else
			printf("%s\n", m->arg1);

	} else {
		printf("wrong format");	// fix this
		status = 1;
	}
	free(m);
	return status;
}

Message* createMessage(char* commandStr, MessageType type, char* prefix) {
	Message* m = (Message*) malloc(sizeof(Message));
	if (m == NULL)
		return NULL;
	m->type = type;
	m->fromClient = 1;
	m->arg1 = prefix;
	chopN(commandStr, strlen(prefix));
	m->arg2 = commandStr;
	return m;
}

int sendClientCommand(char* commandStr, int serverSocket, int mySocketfd) {
	int status = 0;
	char* inputPrefix[15];
	strncpy(inputPrefix, commandStr, 14);
	inputPrefix[14] = "\0";
	if (strcmp(inputPrefix, "list_of_files") == 0) {
		Message* m = createMessage(commandStr, LIST_OF_FILES, inputPrefix);
		if (!m) {
			printf("error in creating message\n"); //DELETE THIS BEFORE HANDING IN
			return 1;
		if (m != NULL) {
			status = send_command(serverSocket, m);

			return status;
		}
		status = send_command(serverSocket, m);
		if (status != 0) {
			printf("error, re-send message\n")
			return 0;
		}
		status = receive_message(serverSocket, m);
		if (status) {
			printf("error in receiving message\n");
		}
		printf("%s", m->arg1);
		return 0;
	}
	inputPrefix[11] = "\0";
	if (strcmp(inputPrefix, "delete_file") == 0) {
		Message* m = createMessage(commandStr, DELETE_FILE, inputPrefix);
		if (!m) {
			printf("error in creating message\n"); //DELETE THIS BEFORE HANDING IN
			return 1;
		if (m != NULL) {
			status = send_command(serverSocket, m);
			return status;
		}
		status = send_command(serverSocket, m);
		if (status != 0) {
			printf("error, re-send message\n")
			return 0;
		}
		status = receive_message(serverSocket, m);
		if (status) {
			printf("error in receiving message\n");
		}
		printf("%s", m->arg1);
		return 0;
	}
	inputPrefix[8] = "\0";
	if (strcmp(inputPrefix, "add_file") == 0) {
		Message* m = createMessage(commandStr, ADD_FILE, inputPrefix);
		if (!m) {
			printf("error in creating message\n"); //DELETE THIS BEFORE HANDING IN
			return 1;
		}

		char* buffer = addFileClientSide(m->arg1) ;
		if (buffer == NULL)
			return 0;
		strcpy(m->arg1, m->arg2);
		m->arg2 = buffer;
		status = send_command(serverSocket, m);
		if (status != 0) {
			printf("error, re-send message\n")
			return 0;
		}
		status = receive_message(serverSocket, m);
		if (status) {
			printf("error in receiving message\n");
			return 0;
		}
		printf("%s", m->arg1);
		return 0;
	}
	if (strcmp(inputPrefix, "get_file") == 0) {
		Message* m = createMessage(commandStr, GET_FILE, inputPrefix);
		char* path_to_save = (char*)malloc(strlen(m->arg2));
		strcpy(path_to_save, m->arg2);
		if (!m) {
			printf("error in creating message\n"); //DELETE THIS BEFORE HANDING IN
			return 1;
		if (m != NULL) {
			status = send_command(serverSocket, m);
			return status;
		}
		status = send_command(serverSocket, m);
		if (status != 0) {
			printf("error, re-send message\n")
			return 0;
		}

		status = receive_message(serverSocket, m);
		if (status && (m->type != ERROR))
			getFileClientSide(path_to_save, m->arg1);
		else
			printf("error in receiving message\n");
		return 0;
	}
	inputPrefix[4] = "\0";
	if (strcmp(inputPrefix, "quit") == 0) {
		if (close(mySocketfd) == -1) {
			printf("close() failed\n");
			return 1;
		}
	}

	return 0;
}


char* addFileClientSide(char* filePath){
	FILE* fp = fopen(filePath, "r");
	char* buffer = (char*)malloc(MAX_FILE_SIZE);
	if (!buffer)
	{
		printf("malloc error\n");
		return NULL;
	}
	fread(buffer, MAX_FILE_SIZE, 1, fp);
	fclose(fp);
	return buffer;
}


void getFileClientSide(char* filePath, char* fileBuffer) {
	FILE *file = fopen(filePath, "w");
	Message* m;
	if (file == NULL ) {
		printf("File Not Opened\n");
		return;
	}
	fwrite(fileBuffer, sizeof(char), MAX_FILE_SIZE, file);
	fclose(file);
	printf("File Added Successfully");
	return;
}

int  client_start(char* hostname, int port) {
	if (hostname == NULL ) {
		int start_client(char* hostname, int port) {
	if (hostname == NULL) {
		hostname[10] = "localhost";
		port = 2235;
	} else if (port == 0)
		port = 2235;

	int status;
	int socketfd;
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);

	if (socketfd < 0) {
		printf("Could not create socket\n");
		return 1;
	}

	struct sockaddr_in my_addr, server_addr;

	socklen_t server_size = sizeof(server_addr);

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr = htonl(hostname);

	int connect = connect(socketfd, &server_addr, sizeof(struct server_addr));
	int clientSocket = connect(socketfd, &server_addr, sizeof(struct server_addr));
	if (clientSocket < 0) {
		close(socketfd);
		printf("connection failed");
		return 1;
	}
	status = defineUser();

	while (status == 0) {
		int maxLen = MAX_COMMAND_NAME + MAX_PATH_NAME + MAX_FILE_NAME;
		char* inputStr = NULL;
		size_t n = 0;
		int read = getline(&inputStr, n, stdin);
		if (read != -1) {
			status = sendClientCommand(inputStr, socketfd, clientSocket);
		} else
			status = 1;
	}

	if (close(socketfd) == -1) {
		printf("close() failed\n");
		return 1;
	}

	return status;
}

