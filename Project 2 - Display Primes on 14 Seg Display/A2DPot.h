
// A2DPot.h
// Purpose: This header file contains the function headers for the functions 
//	declared in the A2DPot.c file. Please see the .c file for 
// 	detailed explanations of implementation.
//	     
//
// Author: Sunny Pritpal Singh (301244872)
// Class: CMPT 433 Embedded Systems @SFU
// Credit: User guides by Brian Fraser provided through CMPT 433 course website
//         also used his youtube videos and links to his demo code linked from the 
//         notes section on his website.

#ifndef _A2D_POT_H_
#define _A2D_POT_H_


/* *****************************PUBLIC FUNCTIONS******************************** */
// can be accessed by external modules and main processes


//Creates thread to acquire information from the POT
void A2DPot_launchThread();

//Acquire signal from POT and set values in primeFinder module
void *A2DPot_Initialize();

void turnOnDigit(int digit);

//stops execution of code for a desired interval
void delayCode(long seconds, long nanoseconds);

FILE* openFile(const char *name,const char *mode);

#endif
