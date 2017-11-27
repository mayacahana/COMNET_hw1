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
		fflush(NULL );
		status = 0;
	}

	//get username and password
	while (status && user_msg->header.type != QUIT) {
		int userMaxLen = MAX_USERNAME_SIZE + strlen("User: ");
		int passMaxLen = MAX_PASSWORD_SIZE + strlen("Password: ");
		char* fullUsername = (char*) malloc(sizeof(char) * (userMaxLen));
		fgets(fullUsername, userMaxLen, stdin);
		char userPrefix[6];
		strncpy(userPrefix, fullUsername, 6);
		userPrefix[6] = '\0';
		int userFlag = strcmp(userPrefix, "User: ");
		chopN(fullUsername, strlen("User: "));
		fullUsername[strlen(fullUsername) - 1] = '\0';

		if (strcmp(fullUsername, "quit\n") != 0) {
			createMessageCommand(user_msg, LOGIN_DETAILS, fullUsername);
			char* fullPassword = (char*) malloc(sizeof(char) * (passMaxLen));
			fgets(fullPassword, userMaxLen, stdin);
			char passwordPrefix[10];
			strncpy(passwordPrefix, fullPassword, 10);
			passwordPrefix[10] = '\0';
			int passFlag = strcmp(passwordPrefix, "Password: ");
			printf("%s\n%s\n", userPrefix, passwordPrefix); //DELETE THIS
			if (strcmp(fullPassword, "quit\n") == 0) {
				createQuitCommand(user_msg);
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
			createQuitCommand(user_msg);
			send_command(scket, user_msg);
			status = 1;
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
	if (m == NULL )
		return;
	m->header.type = type;
	if (string == NULL ) {
		m->arg1 = NULL;
		return;
	}
	strcpy(m->arg1, string);
	m->header.arg1len = strlen(string) + 1;
	//chopN(commandStr, strlen(prefix));
	return;
}

/*
 * @param Message *m
 * creates Message struct via createMessageCommand;
 */
int createQuitCommand(Message* m, int mySocketfd) {
	createMessageCommand(m, QUIT, "00");
	int status = send_command(mySocketfd, m);
	if (status != 0){
		printf("Error. re-send message %s\n", strerror(errno));
		free(m);
		return 1;
	}
	status = receive_command(mySocketfd,m);
	if (status != 0){
		printf("Error in receiving message %s\n", strerror(errno));
	}
	printMessageArg(m);
	free(m);
	return status;
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

int deleteFileCommand(Message* m, char* commandStr, int mySocket) {
	printf("im in deleteFiles\n"); //DELETE THIS
	createMessageCommand(m, DELETE_FILE, commandStr);
	int status = send_command(mySocket, m);
	if (status != 0) {
		printf("error, re-send message\n");
		free(commandStr);
		free(m);
		return 0;
	}
	status = receive_command(mySocket, m);
	if (status)
		printf("error in receiving message\n");
	else
		printf("%s", m->arg1);
	free(commandStr);
	free(m);
	return 0;
}

int addFileCommand(Message* m, char* commandStr, int mySocket) {
	printf("im in addFiles\n"); //DELETE THIS
	createMessageCommand(m, ADD_FILE, commandStr);
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
		filePath = strtok(commandStr, s);
		buffer = addFileClientSide(filePath); // buffer now has whole content of file
		if (buffer == NULL ) {
			free(commandStr);
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

int getFileCommand(Message* m, char* commandStr, int mySocket) {
	printf("im in get_file\n");
	createMessageCommand(m, GET_FILE, inputPrefix);
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
		strtok(commandStr, s);
		filePath = strtok(commandStr, s);
		getFileClientSide(filePath, m->arg1);
	} else
		printf("error in receiving message\n");
	free(m);
	return 0;
}
int sendClientCommand(char* commandStr, int sckt) {
	char op[MAX_COMMAND_NAME], arg1[MAX_ARG_LEN], arg2[MAX_ARG_LEN];
	while (1) {
		Message* m = (Message*) malloc(sizeof(Message));
		memset(op, 0, MAX_COMMAND_NAME);
		memset(arg1, 0, MAX_ARG_LEN);
		memset(arg2, 0, MAX_ARG_LEN);
		//parsing
		strcpy(op, strtok(commandStr, " "));
		strcpy(arg1, strtok(NULL," "));
		strcpy(arg2, strtok(NULL," "));
		// checking op commands:
		if (strcmp(op, "list_of_files") == 0) {
			if (listOfFilesCommand(m, commandStr, sckt) == 1) {
				printf("Error in getting list_of_files \n");
				if (close(sckt) == -1)
					printf("%s\n", strerror(errno));
				return 1;
			}
		} else if (strcmp(op, "delete_file") == 0) {
			if (arg1 == NULL) {
				printf("Error in delete file. name of file not provided \n");
			} else if (deleteFileCommand(m, commandStr, sckt) == 1) {
				printf("Error in delete_file \n");
				if (close(sckt) == -1)
					printf("%s\n", strerror(errno));
				return 1;
			}
		} else if (strcmp(op, "add_file") == 0) {
			if (arg1 == NULL || arg2 == NULL) {
				printf("Path to file and/or new file name not provided \n");
			} else if (addFileCommand(m, commandStr, sckt) == 1) {
				printf("Error in add_file \n");
				if (close(sckt) == -1)
					printf("%s \n", strerror(errno));
				return 1;
			}
		} else if (strcmp(op, "get_file") == 0) {
			if (arg1 == NULL || arg2 == NULL) {
				printf("Path for save and/or file name not provided");
			} else if (getFileCommand(m, commandStr, sckt) == 1) {
				printf("Error in get_file\n");
				if (close(sckt) == -1)
					printf("%s\n",strerror(errno));
				return 1;
			}
		} else if (strcmp(op, "quit") == 0) {
			if (createQuitCommand(m, sckt) == 1 ){
				printf("Error in quitting \n");
				if (close(sckt) == -1)
					printf("%s\n",strerror(errno));
				return 1;
			}
			break;
		}
		printf("Invalid opcode \n");
		free(m);
		break;
	}
	//close socket
	printf("end\n");
	if(close(sckt) == -1){
		printf("%s\n", strerror(errno));
		return 1;
	}
	return 0;
}

void addFileClientSide(char* buffer, char* filePath) {
	FILE* fp = fopen(filePath, "r");
	fread(buffer, MAX_FILE_SIZE, 1, fp);
	fclose(fp);
	return;
}

void getFileClientSide(char* filePath, char* fileBuffer) {
	if (filePath == NULL){
		printf("no filePath received\n");
		return;
	}
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
	if (hostname == NULL ) {
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
	}
	char* inputStr;
	while (status == 0) {
		inputStr = (char*) malloc(sizeof(char) * (MAX_COMMAND_NAME + 1));
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

