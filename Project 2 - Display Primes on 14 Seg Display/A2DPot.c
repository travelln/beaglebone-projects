/* ****************************************************************************** */
// A2DPot.c
// Purpose: This module creates a non-blocking thread that increases or decreases the amount of 
//           primes to be found -> speed is determined by turning the dial on the beaglebone cape.
//
//
//
//
// Author: Sunny Pritpal Singh (301244872)
// Class: CMPT 433 Embedded Systems @SFU
// Credit: User guides by Brian Fraser provided through CMPT 433 course website
//         also used his youtube videos and links to his demo code linked from the 
//         notes section on his website.
/* ****************************************************************************** */

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define EXPORT_FILE_LOCATION "/sys/devices/platform/bone_capemgr/slots"
#define EXPORT_VALUE "BB-ADC"
#define A2D_MAX_READING 4095

#define A2D_READING_0 0
#define A2D_READING_500 500
#define A2D_READING_1000 1000
#define A2D_READING_1500 1500
#define A2D_READING_2000 2000
#define A2D_READING_2500 2500
#define A2D_READING_3000 3000
#define A2D_READING_3500 3500
#define A2D_READING_4000 4000
#define A2D_READING_4100 4100

#define A2D_DELAY_0 0
#define A2D_DELAY_2 2
#define A2D_DELAY_6 6
#define A2D_DELAY_12 12
#define A2D_DELAY_25 25
#define A2D_DELAY_30 30
#define A2D_DELAY_50 50
#define A2D_DELAY_100 100
#define A2D_DELAY_500 500
#define A2D_DELAY_1500 1500

#define A2D_MILLI_TO_NANO 1000000
#define TRIGGER_DELAY_100ms 100000000

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
#include "A2DPot.h"
#include "14SegDisplay.h"

/* Function Declerations */
//prints data tp a file
void printToFile(FILE* toPrint, const char *value);
//gets voltage reading
int getVoltage0Reading(); 


/*
 * Function:  A2DPot_launchThread
 * --------------------
 *  creates a new thread to check POT values
 *
 *
 *   returns: void
 */
void A2DPot_launchThread() {


	pthread_t potThread;

	pthread_create(&potThread, NULL, &A2DPot_Initialize, NULL);

}

/*
 * Function:  A2DPot_Initialize
 * --------------------
 *  sets up the cape and reads values from the POT
 *
 *
 *   returns: void
 */
void *A2DPot_Initialize() {

    /*SETUP THE ADC CAPE FOR READING*/	
	 //First we need to open up the export file
	FILE *export_file = openFile(EXPORT_FILE_LOCATION, "w");
	//This will make sure that the ADC cape is loaded
	fprintf(export_file, "%s", EXPORT_VALUE);
	fclose(export_file);
	sleep(1);
		int delay = 0; //holds the raw voltage output from POT

    /*Convert POT value to time value*/		
	while (true) {
		int reading = getVoltage0Reading();
		
		if (reading == A2D_READING_0 ) {
		
			delay = A2D_DELAY_0;
		} else if (reading < A2D_READING_500) {
		
			delay = A2D_DELAY_2;
		} else if (reading < A2D_READING_1000) {
		
			delay = A2D_DELAY_6;
		} else if (reading < A2D_READING_1500) {
		
			delay = A2D_DELAY_12;
		} else if (reading < A2D_READING_2000) {
		
			delay = A2D_DELAY_25;
		} else if (reading < A2D_READING_2500) {
		
			delay = A2D_DELAY_30;
		} else if (reading < A2D_READING_3000) {
		
			delay = A2D_DELAY_50;
		} else if (reading < A2D_READING_3500) {
		
			delay = A2D_DELAY_100;
		} else if (reading < A2D_READING_4000) {
		
			delay = A2D_DELAY_500;
		} else if (reading < A2D_READING_4100) {
		
			delay = A2D_DELAY_1500;
		}
	
			//converts from milliseconds to nanoseconds
	long delayInNano = delay * A2D_MILLI_TO_NANO;
	
			//tell our primes thread to sleep
	PrimeFinder_setDelayBetweenPrimes(delayInNano);
	
	printf("Delay value: %d ms   \n", delay);	
	
	//calculate the number of primes found in the last second
	int primeCount = PrimeFinder_getNumPrimesFound();
	sleep(1); //delay for one second before acquiring
		        //new delay value from POT
	primeCount = primeCount - PrimeFinder_getNumPrimesFound();
	//printf("Primes found: %d\n", primeCount);
	outToDisplay(primeCount);
	}//end while

return 0;

}

/*
 * Function:  openFile 
 * --------------------
 * open a file, if it cannot be opened then output an error message and exit:
 *
 *  name: specifies the desired file to open
 *  mode: specifies the file control i.e r = read, w = write
 *
 *  returns: the handle to the file if successful
 */

FILE* openFile(const char *name,const char *mode) {

	FILE *temp = fopen(name, mode);
	if (temp == NULL) {
		printf("ERROR: Unable to open export file %s - EXITING PROGRAM.\n",name);
		exit(1);
	}

	return temp;
}

/*
 * Function:  printToFile 
 * --------------------
 * prints a value to a file, will output error if
 *   the specified path is not available and exit:
 *
 *  toPrint: file handle to print too
 *  value: string to print to a file
 *
 *  returns: void
 */
void printToFile(FILE* toPrint, const char *value) {

	int charWritten = fprintf(toPrint,"%s", value);

	if (charWritten <= 0) { 
		printf("ERROR: Unable to write to file - Exiting Program.\n");
	}

	fclose(toPrint);
}

/*
 * Function:  getVoltage0Reading
 * --------------------
 * Acquires the POT signal value from the cape
 *
 *  returns: void
 */
int getVoltage0Reading() {
	// Open file
	FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
	
	if (!f) {
		printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
		printf("try:echo BB-ADC > /sys/devices/platform/bone_capemgr/slots\n");
		exit(-1);
	}

	// Get reading
	int a2dReading = 0;
	int itemsRead = fscanf(f, "%d", &a2dReading);

	if (itemsRead <= 0) {
		printf("ERROR: Unable to read values from voltage input file.\n");
	exit(-1);
	}
	// Close file
	fclose(f);

	return a2dReading;
}

/*
 * Function:  delayCode 
 * --------------------
 * delays the execution of code using:
 *
 *  seconds: number of seconds to delay
 *  nanoseconds: number of nanoseconds to delay 
 *               (note: 100000000ns = 100ms)
 *  returns: void
 */
void delayCode(long seconds, long nanoseconds) {

	struct timespec reqDelay = {seconds, nanoseconds};
	nanosleep(&reqDelay, (struct timespec *) NULL);
}



