
// Assignment 2: udpListener.h
// Purpose: This header file contains the function headers for the functions 
//	declared in the udpListener.c file. Please see the .c file for 
// 	detailed explanations of implementation.
//	     
//
// Author: Sunny Pritpal Singh (301244872)
// Class: CMPT 433 Embedded Systems @SFU
// Credit: User guides by Brian Fraser provided through CMPT 433 course website
//         also used his youtube videos and links to his demo code linked from the 
//         notes section on his website.

#ifndef _UDP_LISTENER_H_
#define _UDP_LISTENER_H_


/* *****************************PUBLIC FUNCTIONS******************************** */
// can be accessed by external modules and main processes

//launches the udp thread to listen for packets
void udpListener_launchThread();
//continuously listens and accepts incoming UDP packets and sends a response
void *udpListener_openConn();
//takes packet data and deciphers the command sent bby the user
void udpListener_MessageParser(char *messageRec, char* messageSend);
//outputs the help message via UDP to net cat connections
void udpListener_DisplayHelp(char* messageSend);
//displays a prime number by index
void udpListener_DisplayGet(char* messageSend, char* cmdNUM);
//displays the last prime found or the last x number of primes specified
//by the user
void udpListener_DisplayLast(char* messageSend, char* cmdNUM);
//displays the first prime found or the first x number of primes specified
//by the user
void udpListener_DisplayFirst(char* messageSend, char* cmdNUM);
//checks that the user has sent a valid integer for a command
_Bool udpListener_CheckValidNum(char* cmdNUM);





















#endif
