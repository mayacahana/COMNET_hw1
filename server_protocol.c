/*
 * server_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 */

#include "server_protocol.h"


/*
 * checks user is not NULL, count the numbers of files in the users directory and prints
 * a greeting to the users, specifies the username and number of files.
 */
void getNameAndFiles(User* user){
	if (!user){
		return;
	}
	int numOfFiles = 0;
    DIR *d;
    struct dirent *dir;
    d = opendir(user->dir_path);
    if (d){
        while ((dir = readdir(d)) != NULL){
        	numOfFiles++;
        }
        closedir(d);
    }
    printf("Hi %s, you have %d files stored", user->user_name, numOfFiles);
    return;
}


int client_serving(int clientsocket, User *users, int numOfUsers){
	printf("Welcome! Please log in\n");
	User* user = NULL;
	int flag = 1;
	char* username = (char*) malloc (sizeof(char)*(strlen("User: ")+ MAX_USERNAME_SIZE));
	char* password = (char*) malloc (sizeof(char)*(strlen("Password: ") + MAX_PASSWORD_SIZE));
	while (user == NULL || flag){
		scanf("%s", &username);
		scanf("%s", &password);
		if (strcmp(username, "quit")==0){
			flag = 0;
		}
		else{
			username = username + strlen("User: ");
			password = password + strlen("Password: ");
			for (int i = 0 ; i< numOfUsers; i++){
				if (strcmp(users[i]->user_name, username)==0){
					if (strcmp(users[i]->password,password)==0){
						user = users[i];
						getNameAndFiles(user);
					}
				}
			}
			if (user == NULL){
				printf("Wrong user name or password, please try again or quit\n");
			}
		}
		username = username - 5;
		password = password - 9;
	}
	free(username);
	free(password);
	char* comStr = (char*) malloc(sizeof(char*)*(MAX_COMMAND_NAME+MAX_FILE_NAME+MAX_PATH_NAME+3));
	while(flag){
		scanf("%s", &comStr);
		flag = parseAndDoCommand(comStr);
	}
	free(comStr);
	return 0;
}



void start_listen(User *usersArray, int numOfUsers, int port) {
	int status, newsocketfd;
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in my_addr, client_addr;
	socklen_t client_size = sizeof(client_addr);
	if (socketfd < 0) {
		printf("Could not create socket\n");
		return;
	}


	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	//my_addr.sin_addr =
	status = bind(socketfd, &my_addr, sizeof(my_addr));

	if (status < 0) {
		printf("Could not bind ip to socket");
		return;
	}

	if(listen(socketfd, 1) < 0 ){
		printf("Could not listen...");
		return;
	}
	while (1) {
		newsocketfd = accept(socketfd, &client_addr, &client_size);
		if (newsocketfd < 0){
			printf("accept() not successful...");
			return;
		}
		client_serving(newsocketfd);
		close(newsocketfd);
	}
}


void start_server(char* users_file, const char* dir_path, int port) {
	//creating the main folder
	//reading input file
	FILE* usersFile = fopen(users_file, "r");

	User* usersArray = (User*) malloc(15 * sizeof(User));
	int numOfUsers = 0;
	/*struct stat dirctry;

	 stat(dir_path, &dirctry);*/

	if (usersFile != NULL) {
		char* user_buffer = (char*) malloc(sizeof(char*) * 26);
		char* pass_buffer = (char*) malloc(sizeof(char*) * 26);

		//	const char* fileDirPath = (char*) malloc(sizeof(dir_path) + 27);

		while (sscanf(usersFile, "%s", user_buffer) > 0) {
			const char* fileDirPath = (char*) malloc(sizeof(dir_path) + 27);

			strcpy(fileDirPath, dir_path);
			strcat(fileDirPath, "/");
			strcat(fileDirPath, user_buffer);

			if (!mkdir(fileDirPath)) {
				User* newUser = (User*) malloc(sizeof(User));
				strcpy(newUser->user_name, user_buffer);
				sscanf(usersFile, "%s", pass_buffer);
				strcpy(newUser->password, pass_buffer);
				strcpy(newUser->dir_path, fileDirPath);
				usersArray[numOfUsers] = newUser;
				numOfUsers++;
			} else {
				printf("cannot create user directory for :%s", user_buffer);
			}
			free(user_buffer);
			free(pass_buffer);
			free(fileDirPath);

			user_buffer = (char*) malloc(sizeof(char*) * 26);
			pass_buffer = (char*) malloc(sizeof(char*) * 26);
		}
		strat_listen(usersArray);
	}
}
