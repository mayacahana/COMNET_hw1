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
int defineUser(int scket) {
	int status = 1;
	Message* user_msg = (Message*) malloc(sizeof(Message));
	if (!user_msg) {
		printf("malloc failed\n");
		fflush(NULL);
		status = 0;
	}

	//get username and password
	while (status && user_msg->header.type != QUIT) {
		int userMaxLen = MAX_USERNAME_SIZE + strlen("User: ");
		int passMaxLen = MAX_PASSWORD_SIZE + strlen("Password: ");
		char* fullUsername = (char*) malloc(sizeof(char) * (userMaxLen));
		fgets(fullUsername, userMaxLen, stdin);
		char userPrefix[7];
		strncpy(userPrefix, fullUsername, 6);
		userPrefix[6] = '\0';
		int userFlag = strcmp(userPrefix, "User: ");
		chopN(fullUsername, strlen("User: "));
		fullUsername[strlen(fullUsername) - 1] = '\0';

		if (strcmp(fullUsername, "quit\n") != 0) {
			createMessageCommand(user_msg, LOGIN_DETAILS, fullUsername);
			char* fullPassword = (char*) malloc(sizeof(char) * (passMaxLen));
			fgets(fullPassword, userMaxLen, stdin);
			char passwordPrefix[11];
			strncpy(passwordPrefix, fullPassword, 10);
			passwordPrefix[10] = '\0';
			int passFlag = strcmp(passwordPrefix, "Password: ");
			printf("%s\n%s\n", userPrefix, passwordPrefix); //DELETE THIS
			if (strcmp(fullPassword, "quit\n") == 0) {
				createQuitCommand(user_msg, scket);
			} else if (passFlag || userFlag) {
				printf("Wrong prefix of 'User:' or 'Password:'");
				printf("u = %d, p = %d", userFlag, passFlag); // DELETE THIS
				printf("user = %s\n", fullUsername); //DELETE THIS
			} else {
				status = send_command(scket, user_msg);
				if (!status) {
					Message* pass_msg = (Message*) malloc(sizeof(Message));
					chopN(fullPassword, strlen("Password: "));
					fullPassword[strlen(fullPassword) - 1] = '\0';
					createMessageCommand(pass_msg, LOGIN_DETAILS, fullPassword);
					status = send_command(scket, pass_msg);
					receive_command(scket, pass_msg);
					if (pass_msg->header.type == INVALID_LINE) {
						status = 1;
					} else {
						status = 0;
						printMessageArg(pass_msg);
					}
					free(pass_msg);
				} else {
					printf("Error in sending user name\n");
				}

			}
		} else {
			status = createQuitCommand(user_msg, scket);
		}
	}
	free(user_msg);
	return status;
}

/*
 * @param Message* m, MessageType type, char* string
 * receives the message type and string, and assigns them to the
 * relevant field in the message struct.
 */
void createMessageCommand(Message* m, MessageType type, char* string) {
	if (m == NULL)
		return;
	m->header.type = type;
	if (string == NULL) {
		strcpy(m->arg1, "");
		return;
	}
	strcpy(m->arg1, string);
	m->header.arg1len = strlen(string) + 1;
	return;
}

/*
 * @param Message *m
 * creates Message struct via createMessageCommand;
 */
int createQuitCommand(Message* m, int mySocketfd) {
	createMessageCommand(m, QUIT, "00");
	int status = send_command(mySocketfd, m);
	if (status != 0) {
		printf("Error. re-send message %s\n", strerror(errno));
		return 1;
	}
	return 1;
}
/*
 * @param Message* m, char* commandStr, int mySocketfd
 * creates Message struct vie createMessageCommand;
 * sends the Message we created to server. If succeeds reveives message from server
 * if send did not succeed, print message to user and exits.
 */
int listOfFilesCommand(Message* m, char* commandStr, int mySocketfd) {
	printf("im in list_of_files\n"); //delete this
	createMessageCommand(m, LIST_OF_FILES, "list");
	int status = send_command(mySocketfd, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(m);
		return 0;
	}
	status = receive_command(mySocketfd, m);
	if (status) {
		printf("error in receiving message\n");
	}
	printMessageArg(m);
	free(m);
	return status;
}

int deleteFileCommand(Message* m, char* file_name, int mySocket) {
	printf("im in deleteFiles\n");
	createMessageCommand(m, DELETE_FILE, file_name);
	int status = send_command(mySocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(m);
		return 1;
	}
	status = receive_command(mySocket, m);
	if (!status) {
		printf("error in receiving message\n");
	}
	printMessageArg(m);
	free(m);
	return 0;
}

int addFileCommand(Message* m, char* path_to_file ,char* file_name, int mySocket) {
	// CHANGE ALL OF THIS
	printf("im in addFiles\n"); //DELETE THIS
	createMessageCommand(m, ADD_FILE, path_to_file);
	int status = send_command(mySocket, m);
	if (status == 0) {
		char* buffer = (char*) malloc(MAX_FILE_SIZE);
		if (!buffer) {
			printf("malloc error\n");
			free(m);
			return 0;
		}
		char* filePath;
		const char* s = " ";
		/* get the first word of commandStr */
		filePath = strtok(path_to_file, s);
		addFileClientSide(buffer, filePath); // buffer now has whole content of file
		if (buffer == NULL) {
			free(path_to_file);
			free(m);
			return 0;
		}
		Message* fileContent = (Message*) malloc(sizeof(Message*));
		createMessageCommand(fileContent, FILE_CONTENT, buffer);
		status = send_command(mySocket, m);
		free(buffer);
		if (status != 0) {
			printf("error, re-send message\n");
			free(m);
			return 0;
		}
		status = receive_command(mySocket, m);
		if (status)
			printf("error in receiving message\n");
		else
			printf("%s", m->arg1);
	} else
		printf("error, re-send message\n");
	free(m);
	return 0;
}

int getFileCommand(Message* m, char* file_name, char* path_to_save, int mySocket) {
	//CHANGE ALL OF THIS
	printf("im in get_file\n");
	createMessageCommand(m, GET_FILE, file_name);
	//char* path_to_save = (char*) malloc(strlen(m->arg2));
	//strcpy(path_to_save, m->arg2);
	int status = send_command(mySocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(m);
		return 0;
	}
	status = receive_command(mySocket, m);
	if (status == 0 && (m->header.type != ERROR)) {
		char* filePath;
		const char* s = " ";
		/* get the second word of commandStr */
		strtok(file_name, s);
		filePath = strtok(file_name, s);
		getFileClientSide(filePath, m->arg1);
	} else
		printf("error in receiving message\n");
	free(m);
	return 0;
}
int sendClientCommand(char* commandStr, int mySocketfd) {
	printf("in sendClientCommand\n");
	int status = 0;
	Message* m = (Message*) malloc(sizeof(Message));
	char* c = malloc(strlen(commandStr) + 5);
	char delimit[] = " \t\r\n\v\f";
	strcpy(c, commandStr);
	c[strlen(commandStr) + 1] = '\0';
	char *str1 = strtok(c, delimit);
	char *str2 = strtok(NULL, delimit);
	if (str2 != NULL) {
		char *str3 = strtok(NULL, delimit);
		if (str3 != NULL) {
			if (strcmp(str1, "add_file") == 0) {
				if(addFileCommand(m, str2, str3, mySocketfd) == 1){
					printf("Error in ad");
				}
			} else if (strcmp(str1, "get_file") == 0) {
				return getFileCommand(m, str2, str3, mySocketfd);
			}
		}
		if (strcmp(str1, "delete_file") == 0) {
			printf("salami2\n");
			return deleteFileCommand(m, str2, mySocketfd);
		}
	}
	if (strcmp(str1, "list_of_files") == 0) {
		return listOfFilesCommand(m, commandStr, mySocketfd);
	} else if (strcmp(str1, "quit") == 0) {
		printf("we got quit\n");
		if (close(mySocketfd) == -1) {
			printf("close() failed\n");
			return 1;
		}
	}
	printf("end\n");
	free(m);
	return 0;
}

void addFileClientSide(char* buffer, char* filePath) {
	FILE* fp = fopen(filePath, "r");
	if (!fp) {
		printf("File cannot be openes \n");
		return;
	}
	fread(buffer, MAX_FILE_SIZE, 1, fp);
	fclose(fp);
	return;
}

void getFileClientSide(char* filePath, char* fileBuffer) {
	if (filePath == NULL) {
		printf("no filePath received\n");
		return;
	}
	FILE *file = fopen(filePath, "w");
	if (!file) {
		printf("File Not Opened\n");
		return;
	}
	fwrite(fileBuffer, sizeof(char), MAX_FILE_SIZE, file);
	fclose(file);
	printf("File Added Successfully");
	return;
}

int client_start(char* hostname, int port) {
	if (hostname == NULL) {
		char* hostname = (char*) malloc(sizeof(char) * 11);
		strcpy(hostname, "localhost");
//port = 1337;
		port = 5566;
	} else if (port == 0)
//port = 1337;
		port = 5566;

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
	Message* msg = (Message*) calloc(sizeof(Message), 1);
	status = receive_command(socketfd, msg);
	if (status) {
		printf("problem in greeting\n");
	} else {
		printMessageArg(msg);
		status = defineUser(socketfd);
		printf("status: %d\n", status);
		fflush(NULL);
	}
	//char* inputStr;
	char* inputStr;
	while (status == 0) {
		inputStr = (char*) malloc(sizeof(char) * (MAX_COMMAND_NAME + 2 + 2 * MAX_ARG_LEN));
		fgets(inputStr, MAX_COMMAND_NAME + 2 + 2 * MAX_ARG_LEN, stdin);
		status = sendClientCommand(inputStr, socketfd);
		free(inputStr);
	}
	if (close(socketfd) == -1) {
		printf("close() failed\n");
		free(inputStr);
		return 1;

	}

	return status;
}

