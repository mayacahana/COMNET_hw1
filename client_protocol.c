/*
 * client_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 *
 */

#include "client_protocol.h"

/*
 * @param pointer to string and size_t variable n
 * removes last n bits from the string.
 */
void chopN(char* str, size_t n) {
	if (!str) {
		return;
	}
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
	printf("i am in defineUser\n");
	int status = 1;
	Message* m = (Message*) malloc(sizeof(Message));

	if (!m) {
		printf("malloc failed\n");
		fflush(NULL );
		status = 0;
	}

	//get username and password
	while (status) {
		printf("i am in while\n");
		status = 0;
		char* fullUsername = (char*) malloc(
				sizeof(char) * (MAX_USERNAME_SIZE + strlen("user: ")));
		char* fullPassword = (char*) malloc(
				sizeof(char) * (MAX_USERNAME_SIZE + strlen("password: ")));

		int userMaxLen = MAX_USERNAME_SIZE + strlen("User: ");
		int passMaxLen = MAX_USERNAME_SIZE + strlen("Password: ");

		fgets(fullUsername, userMaxLen, stdin);

		if (strcmp(fullUsername, "quit\n") != 0) {
			fgets(fullPassword, passMaxLen, stdin);
					if (strcmp(fullPassword, "quit\n") == 0) {
						m->header.type = QUIT;
					}
				} else
				m->header.type = QUIT;
		if (m->header.type == QUIT) {
			free(m);
			return 1;
		}
		if (fullUsername == NULL || fullPassword == NULL ) {
			return 1;
		}
		char userPrefix[7];
		char passwordPrefix[11];
		strcpy(userPrefix, fullUsername);
		strcpy(passwordPrefix, fullPassword);
		userPrefix[6] = '\0'; // "User: " if in correct format
		passwordPrefix[10] = '\0'; // "Password: " if in correct format

		//check if input is in correct format and create message
		if ((strcmp(userPrefix, "User: ") == 0)
				&& (strcmp(passwordPrefix, "Password: ") == 0)) {
			chopN(fullUsername, 6);
			chopN(fullPassword, 10);
			createMessageCommand(m, fullPassword, LOGIN_DETAILS, fullUsername);

			//		printf("message details: arg1: %s, arge=2: %s",	m->arg1, m->arg2);
			status = send_command(serverSocket, m); ///check status
			if (status != 0)
				printf("problem in send_command\n");
			//receive_command(serverSocket, m);
			if (strcmp(m->arg1, "WRONG") == 0) {
				printf("Wrong username or passoword. Please try again. \n");
			} else
				printf("%s", m->arg1);
		} else {
			printf("wrong format");	// fix this
			status = 1;
		}
		free(fullUsername);
		free(fullPassword);
	}	//end of while
	free(m);
	return status;
}

void createMessageCommand(Message* m, char* commandStr, MessageType type,
		char* prefix) {
	if (m == NULL )
		return;
	m->header.type = type;
	m->fromClient = 1;
	m->arg1 = prefix;
	m->header.arg1len = strlen(prefix);
	chopN(commandStr, strlen(prefix));
	m->arg2 = commandStr;
	m->header.arg2len = strlen(m->arg2);
	return;
}

int listOfFilesCommand(Message* m, char* commandStr, char* inputPrefix,
		int serverSocket, int mySocketfd) {
	printf("im in list_of_files\n");
	createMessageCommand(m, commandStr, LIST_OF_FILES, inputPrefix);
	int status = send_command(serverSocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(m);
		free(inputPrefix);
		return 0;
	}
	status = receive_command(serverSocket, m);
	if (status) {
		printf("error in receiving message\n");
	}
	printf("%s", m->arg1);
	free(inputPrefix);
	free(m);
	return 0;
}

int deleteFileCommand(Message* m, char* commandStr, char* inputPrefix,
		int serverSocket, int mySocket) {
	printf("im in deleteFiles\n");
	createMessageCommand(m, commandStr, DELETE_FILE, inputPrefix);
	int status = send_command(serverSocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(inputPrefix);
		free(m);
		return 0;
	}
	free(inputPrefix);
	status = receive_command(serverSocket, m);
	if (status) {
		printf("error in receiving message\n");
	}
	printf("%s", m->arg1);
	free(inputPrefix);
	free(m);
	return 0;
}

int addFileCommand(Message* m, char* commandStr, char* inputPrefix,
		int serverSocket, int mySocket) {
	printf("im in addFiles\n");
	createMessageCommand(m, commandStr, ADD_FILE, inputPrefix);
	char* buffer = addFileClientSide(m->arg1);
	if (buffer == NULL ) {
		free(inputPrefix);
		return 0;
	}
	strcpy(m->arg1, m->arg2);
	m->arg2 = buffer;
	int status = send_command(serverSocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(m);
		free(inputPrefix);
		return 0;
	}
	status = receive_command(serverSocket, m);
	if (status)
		printf("error in receiving message\n");
	printf("%s", m->arg1);
	free(inputPrefix);
	free(m);
	return 0;
}

int getFileCommand(Message* m, char* commandStr, char* inputPrefix,
		int serverSocket, int mySocket) {
	printf("im in get_file\n");
	createMessageCommand(m, commandStr, GET_FILE, inputPrefix);
	char* path_to_save = (char*) malloc(strlen(m->arg2));
	strcpy(path_to_save, m->arg2);
	int status = send_command(serverSocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(m);
		free(inputPrefix);
		return 0;
	}
	status = receive_command(serverSocket, m);
	if (status && (m->header.type != ERROR))
		getFileClientSide(path_to_save, m->arg1);
	else {
		printf("error in receiving message\n");
		free(inputPrefix);
		free(m);
		return 0;
	}
	return 0;
}

int sendClientCommand(char* commandStr, int serverSocket, int mySocketfd) {
	printf("in sendClientCommand\n");
	int status = 0;
	char* inputPrefix = (char*) malloc(
			sizeof(char) * (strlen("list_of_files") + 2));
	strcpy(inputPrefix, commandStr);
	inputPrefix[strlen("list_of_files")] = '\0';
	Message* m = (Message*) malloc(sizeof(Message));

	printf("inputPrefix: %s\n", inputPrefix);//DELETE THIS

	if (strcmp(inputPrefix, "list_of_files") == 0) {
		printf("salami1\n");
		return listOfFilesCommand(m, commandStr, inputPrefix, serverSocket,
				mySocketfd);
	}
	printf("salami deleteFile\n");
	inputPrefix[11] = '\0';
	if (strcmp(inputPrefix, "delete_file\n") == 0) {
		printf("salami2\n");
		return deleteFileCommand(m, commandStr, inputPrefix, serverSocket,
				mySocketfd);
	}
	printf("salami addfile\n");
	inputPrefix[8] = '\0';
	if (strcmp(inputPrefix, "add_file\n") == 0) {
		return addFileCommand(m, commandStr, inputPrefix, serverSocket,
				mySocketfd);
	}

	printf("salami getFile\n");
	if (strcmp(inputPrefix, "get_file\n") == 0) {
		return getFileCommand(m, commandStr, inputPrefix, serverSocket,
				mySocketfd);
	}
printf("salami quit\n");
	inputPrefix[4] = '\0';
	if (strcmp(inputPrefix, "quit") == 0) {
		printf("we got quit\n");
		if (close(mySocketfd) == -1) {
			printf("close() failed\n");
			free(inputPrefix);
		}

		free(m);
		free(inputPrefix);
		return 1;
	}
	printf("end\n");
	free(m);
	free(inputPrefix);
	return 0;
}

char* addFileClientSide(char* filePath) {
	FILE* fp = fopen(filePath, "r");
	char* buffer = (char*) malloc(MAX_FILE_SIZE);
	if (!buffer) {
		printf("malloc error\n");
		return NULL ;
	}
	fread(buffer, MAX_FILE_SIZE, 1, fp);
	fclose(fp);
	return buffer;
}

void getFileClientSide(char* filePath, char* fileBuffer) {
	FILE *file = fopen(filePath, "w");
	if (file == NULL ) {
		printf("File Not Opened\n");
		return;
	}
	fwrite(fileBuffer, sizeof(char), MAX_FILE_SIZE, file);
	fclose(file);
	printf("File Added Successfully");
	return;
}

int client_start(char* hostname, int port) {
	printf("i am in clientStart\n");
	if (hostname == NULL ) {
		char* hostname = (char*) malloc(sizeof(char) * 11);
		strcpy(hostname, "localhost");
//port = 1337;
		port = 2235;
	} else if (port == 0)
//port = 1337;
		port = 2235;

	int status, serverSocket;
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);

	if (socketfd < 0) {
		printf("Could not create socket\n");
		return 1;
	}
	struct sockaddr_in server_addr;

	memset(&server_addr, '0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_aton(hostname, &server_addr.sin_addr);
	serverSocket = connect(socketfd, (struct sockaddr*) &server_addr,
			sizeof(server_addr));
	if (serverSocket < 0) {
		close(socketfd);
		printf("Connection failed \n");
		return 1;
	}
	fflush(NULL );
	status = defineUser(socketfd);
	printf("status: %d\n", status);
	fflush(NULL );
	char* inputStr ;
	while (status == 0) {
		inputStr = (char*) malloc(sizeof(char) * (MAX_COMMAND_NAME + 1));
		fgets(inputStr, MAX_COMMAND_NAME + 1, stdin);
		status = sendClientCommand(inputStr, socketfd, serverSocket);
		free(inputStr);
	}

	if (close(socketfd) == -1) {
		printf("close() failed\n");
		free(inputStr);
		return 1;

	}

	return status;
}

