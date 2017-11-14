/*
 * client_protocol.c
 *
 *  Created on: Nov 10, 2017
 *      Author: mayacahana
 *
 */

#include client_protocol.h
#include server_protocol // remove this after bridge fixed


char* getUserDetails(){
	char* userDetails[26];
	int read = getline(&userDetails, 25, stdin);
	if (read != -1){
		puts(userDetails);
		return userDetails;
	}
	printf("No Line Read");
	return NULL;
}


/*
 * sends server username and password
 * returns 0 if succeeds, otherwise returns 1
 */
int createUser(){
	int status = 0;
	//get username
	char* userName = getUserDetails();
	char* userMessage[31] = "User: ";
	strcat(userMessage, userName);
	status = send_message(userMessage);//change this if needed
	if (status != 0){
		printf("Message Not Sent");
		return 1;
	}
	free(userMessage);

	//get password
	char* userPassword = getUserPassword();
	char* userMessagae[36] = "Password: ";
	strcat(userMessage, userPassword);
	status = send_message(userMessage); // change this if needed
	if (status != 0){
		printf("Message Not Sent");
		return 1;
	}
	return 0;
}

//??
int sendCommand(char* commandStr){
	int status = 0;
	while (status == 0){


		}
	}
	return 0;
}













}

