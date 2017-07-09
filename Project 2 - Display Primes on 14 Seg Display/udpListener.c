/* ****************************************************************************** */
// Assignment 2: udpListener.c
// Purpose: This module creates a non-blocking thread that listens for UDP packets
//	     sent from a client and will then respond to commands issued by the user.
//	     These commands will allow the user to control the running of the 
//           primeFinder process and display the value of primes that are 
//	     calculated by this process.
//
//	Usage:  "count"   -- show # primes found.
//		"get 10"  -- display prime # 10.
//	        "last 2"  -- display the last 2 primes found.
//	        "first 5" -- display the first 5 primes found.
//	        "stop"    -- cause the server program to end.
//
// Author: Sunny Pritpal Singh (301244872)
// Class: CMPT 433 Embedded Systems @SFU
// Credit: User guides by Brian Fraser provided through CMPT 433 course website
//         also used his youtube videos and links to his demo code linked from the 
//         notes section on his website.
/* ****************************************************************************** */

//mapping for command to integer values - this acts as a lookup table
#define HELP_COMMAND_SIZE 4
#define COUNT_COMMAND_SIZE 5 
#define GET_COMMAND_SIZE 3
#define LAST_COMMAND_SIZE 4
#define FIRST_COMMAND_SIZE 5
#define STOP_COMMAND_SIZE 4

#define HELP_COMMAND_STR "help"
#define COUNT_COMMAND_STR "count" 
#define GET_COMMAND_STR "get"
#define LAST_COMMAND_STR "last"
#define FIRST_COMMAND_STR "first"
#define STOP_COMMAND_STR "stop"

#define PORT 12345

#define MSG_MAX_LEN 1024


#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include "udpListener.h"
#include "primeFinder.h"

struct cmdStruct
{
	int cmdName[50];
	int cmdNumber[50];
	int totalCmds;
	int currentIndex;
};


struct cmdStruct commands = {.totalCmds = 0, .currentIndex = 0};

/*
 * Function:  udpListener_launchThread
 * --------------------
 *  creates a new thread to listen for incoming packets
 *
 *   returns: void
 */
void udpListener_launchThread(){

	pthread_t udpThread;

	pthread_create(&udpThread, NULL, &udpListener_openConn, NULL);

	//pthread_join(udpThread, NULL);
}

/*
 * Function:  udpListener_openConn
 * --------------------
 *  creates a new thread to check for prime numbers
 *
 *   pipeFileDesc: theside of the pipe that can be written to
 *
 *   returns: void
 */
void *udpListener_openConn(){

	//printf("CHECK ARRAY: %d\n", commands.cmdNumber[5]);
	
	char messageRec[MSG_MAX_LEN];
	char messageSend[MSG_MAX_LEN];
	
		// Address
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;                   // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
	sin.sin_port = htons(PORT);                 // Host to Network short
	
	// Create the socket for UDP
	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

	// Bind the socket to the port (PORT) that we specify
	bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
	
	while(1) { //continuously listen for incoming messages and respond
	
		unsigned int sin_len = sizeof(sin);
		
		//get the message from the client
		int bytesRx = recvfrom(socketDescriptor, messageRec, MSG_MAX_LEN, 0,
		(struct sockaddr *) &sin, &sin_len);

		messageRec[bytesRx] = 0;
		messageSend[bytesRx] = 0;
		//printf("Message received (%d bytes): \n\n%s\n", bytesRx, messageRec);
		
		//figure out what the message says
		udpListener_MessageParser(messageRec, messageSend);
		
		//send the reply message to the client
		sin_len = sizeof(sin);
		sendto( socketDescriptor,messageSend, strlen(messageSend),0,
				(struct sockaddr *) &sin, sin_len);

		}//end while	
	
	close(socketDescriptor); //close our socket connection -- All done!

	return 0;

}

/*
 * Function:  udpListener_MessageParser
 * --------------------
 *  Breaks up the message recieved from the client to check what
 *	what command it corresponds to.
 *
 *   messageRec: string recieved from user.
 *   messageSend: string that holds the response to be sent to the user.
 *
 *   returns: void
 */
void udpListener_MessageParser(char *messageRec, char* messageSend){

	char *cmdSTR;
	char *cmdNUM;
	cmdSTR = strtok (messageRec," ");
	cmdNUM = strtok (NULL," ");
	
	//printf("HERE IS THE NUM: %s\n", cmdNUM);
	
	if(strncmp(HELP_COMMAND_STR, cmdSTR, HELP_COMMAND_SIZE) == 0) {
		udpListener_DisplayHelp(messageSend);
	}
	else if(strncmp(COUNT_COMMAND_STR, cmdSTR, COUNT_COMMAND_SIZE) == 0) {
		unsigned int count = PrimeFinder_getNumPrimesFound();
		sprintf(messageSend, " Number of primes found = %u\n", count);
	}	
	else if(strncmp(GET_COMMAND_STR, cmdSTR, GET_COMMAND_SIZE) == 0) {

		udpListener_DisplayGet(messageSend, cmdNUM);	
	
	} 
	else if(strncmp(LAST_COMMAND_STR, cmdSTR, LAST_COMMAND_SIZE) == 0) {

		udpListener_DisplayLast(messageSend, cmdNUM);	
	
	} 
	else if(strncmp(FIRST_COMMAND_STR, cmdSTR, FIRST_COMMAND_SIZE) == 0) {

		udpListener_DisplayFirst(messageSend, cmdNUM);	
	
	} 
	else if(strncmp(STOP_COMMAND_STR, cmdSTR, STOP_COMMAND_SIZE) == 0) {

		PrimeFinder_statusChange();
		sprintf(messageSend," Program terminating.\n");
	
	} 
	else { 
		sprintf(messageSend," Unknown command. Type 'help' for command list.\n");
	}
	
	
	
//printf("PARSED Message received %s: \n",cmd);
//printf("%s\n",messageSend);
}


/*
 * Function:  udpListener_DisplayHelp
 * --------------------
 *  Generates the return packet data containing a listing
  *     if commands the user can enter into the console.
 *
 *   pipeFileDesc: theside of the pipe that can be written to
 *
 *   returns: void
 */
void udpListener_DisplayHelp(char* messageSend){

	sprintf(messageSend," Accepted command examples:\n"
		" count   -- show # primes found.\n"
		" get 10  -- display prime # 10.\n"
	        " last 2  -- display the last 2 primes found.\n"
	        " first 5 -- display the first 5 primes found.\n"
	        " stop    -- cause the server program to end\n");
	
}

/*
 * Function:  udpListener_DisplayLast
 * --------------------
 *  Generates the return packet data containing the last x
 *  number of primes as requested by the user.
 *  Error checks the value to make sure it is within bounds of the
 *        number of computed primes and will display max 50 primes.
 *
 *   messageSend: string that will hold the formatted reply message
 *                contains any data requested by user or an
 *                appropriate error message.
 *   cmdNUM: string input from user representing numerical argument.
 *
 *   returns: void
 */
void udpListener_DisplayLast(char* messageSend, char *cmdNUM){

			//if only "last" was entered by user
	if (cmdNUM == NULL){ 
		unsigned int count = PrimeFinder_getNumPrimesFound();
			
		sprintf(messageSend, " Last 1 primes = \n %llu \n", PrimeFinder_getPrimeByIndex(count-1));
		return;
	}
			//if the user entered an invalid number - print error
	if(udpListener_CheckValidNum(cmdNUM) == false) { 
		sprintf(messageSend," Unknown command. Type 'help' for command list.\n");
		return;
	}
		
	unsigned int primesFound = PrimeFinder_getNumPrimesFound();
		
	int count = 0; //convert our input from user to an int value
	sscanf(cmdNUM, "%d", &count);
		
	if((count < 1) || (count > primesFound)) {

		sprintf(messageSend," Invalid argument. Must be between 1 and %d.\n", primesFound);
		return;		
	}
		//display a maximum of 50 primes
	if(count > 50)
	  count = 50;
		
	int i = 0;
	int newLineCount = 1;
	sprintf(messageSend," Last %d primes = \n", count);
	
			//output the requested number of primes
	for(i=(primesFound - count); i < primesFound; i++) {
		char temp[100];
		sprintf(temp, " %llu", PrimeFinder_getPrimeByIndex(i));
			
		strcat(messageSend, temp);
		
		if (newLineCount == 4){
			strcat(messageSend, ",\n");
			newLineCount = 1;
		} else if (i < primesFound-1) {
		   strcat(messageSend, ", ");
		   newLineCount++;
		} else {
			strcat(messageSend, "\n");
		}
	
	}//end for loop
}

/*
 * Function:  udpListener_DisplayGet
 * --------------------
 * Generates the return packet data containing the prime 
 *     number requested by index value from the user.
 *
 *   messageSend: string that will hold the formatted reply message
 *                contains any data requested by user or an
 *                appropriate error message.
 *   cmdNUM: string input from user representing numerical argument.
 *
 *   returns: void
 */
void udpListener_DisplayGet(char* messageSend, char* cmdNUM) {

		//if only "last" was entered by user
	if (cmdNUM == NULL){ 
		unsigned int count = PrimeFinder_getNumPrimesFound();
			
		sprintf(messageSend, " Last 1 primes = \n %llu \n", PrimeFinder_getPrimeByIndex(count-1));
		return;
	}
					//if the user entered an invalid number - print error
	if(udpListener_CheckValidNum(cmdNUM) == false) { 
		sprintf(messageSend," Unknown command. Type 'help' for command list.\n");
		return;
	}
	
	unsigned int primesFound = PrimeFinder_getNumPrimesFound();
		
	int count = 0; //convert our input from user to an int value
	sscanf(cmdNUM, "%d", &count);
		
	if((count < 1) || (count > primesFound)) {

		sprintf(messageSend," Invalid argument. Must be between 1 and %d.\n", primesFound);
		return;		
	}		


	sprintf(messageSend, " Prime %d = %llu \n", count, PrimeFinder_getPrimeByIndex(count));
}


/*
 * Function:  udpListener_DisplayFirst
 * --------------------
 *  Generates the return packet data containing first x number 
 *     of primes as requested by the user.
 *  Error checks the value to make sure it is within bounds of the
 *        number of computed primes and will display max 50 primes.
 *
 *   messageSend: string that will hold the formatted reply message
 *                contains any data requested by user or an
 *                appropriate error message.
 *   cmdNUM: string input from user representing numerical argument.
 *
 *   returns: void
 */
void udpListener_DisplayFirst(char* messageSend, char *cmdNUM){

			//if only "last" was entered by user
	if (cmdNUM == NULL){ 
		//unsigned int count = PrimeFinder_getNumPrimesFound();
			
		sprintf(messageSend, " First 1 primes = \n %llu \n", PrimeFinder_getPrimeByIndex(1));
		return;
	}
			//if the user entered an invalid number - print error
	if(udpListener_CheckValidNum(cmdNUM) == false) { 
		sprintf(messageSend," Unknown command. Type 'help' for command list.\n");
		return;
	}
		
	unsigned int primesFound = PrimeFinder_getNumPrimesFound();
		
	int count = 0; //convert our input from user to an int value
	sscanf(cmdNUM, "%d", &count);
		
	if((count < 1) || (count > primesFound)) {

		sprintf(messageSend," Invalid argument. Must be between 1 and %d.\n", primesFound);
		return;		
	}
		//display a maximum of 50 primes
	if(count > 50)
	  count = 50;
		
	int i = 0;
	int newLineCount = 1;
	sprintf(messageSend," First %d primes = \n", count);
	
			//output the requested number of primes
	for(i = 1; i <= count; i++) {
		char temp[100];
		sprintf(temp, " %llu", PrimeFinder_getPrimeByIndex(i));
			
		strcat(messageSend, temp);
		
		if (newLineCount == 4){
			strcat(messageSend, ",\n");
			newLineCount = 1;
		} else if (i < count) {
		   strcat(messageSend, ", ");
		   newLineCount++;
		} else {
			strcat(messageSend, "\n");
		}
	
	}//end for loop
}

/*
 * Function:  udpListener_CheckValidNum
 * --------------------
 *  Checks if the sting entered by the user is a valid integer
 *
 *   cmdNUM: possible integer 
 *
 *   returns: true - if the entire string is all numerals.
 *            false - otherwise.
 */
_Bool udpListener_CheckValidNum(char *cmdNUM) {

	int i = 0;
	for(i = 0; i < strlen(cmdNUM)-1; i++) {
			
		if (isdigit(cmdNUM[i]) == 0) {
			
			return false;
		} 
	}//end for loop
	
	return true;
}



















