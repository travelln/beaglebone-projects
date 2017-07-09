
// main.c
// Purpose: This main module calls the child processes. 
//
// Author: Sunny Pritpal Singh (301244872)
// Class: CMPT 433 Embedded Systems @SFU
// Credit: User guides by Brian Fraser provided through CMPT 433 course website
//         also used his youtube videos and links to his demo code linked from the 
//         notes section on his website.


#define BUFFSIZE 1024
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "primeFinder.h"
#include "udpListener.h"
#include "A2DPot.h"
#include "14SegDisplay.h"

void reader (int *fds);

int main() 
{
	
	// Create a pipe. File descriptors for two ends of pipe are placed in fds
	int fds[2];
	
	//launch threads
	udpListener_launchThread();
	A2DPot_launchThread();
	sleep(1);//slepp to make sure all cape functionallity is open
	
	printf("Starting to find primes...\n");
	while(PrimeFinder_shouldCalculate()){
		pipe (fds);

		PrimeFinder_launchThread(fds[1]);

		reader(fds);
	}

	printf("Done!\n");
	
}

// Read strings from the stream as long as possible.
void reader (int *fds)
{
	
	FILE* stream;
	stream = fdopen (fds[0], "r");
	if (!feof (stream)){
		char buffer[64];
		fgets(buffer,64,stream);
		printf("Found prime: %s", buffer);

		fclose (stream);

	}	
	
}












