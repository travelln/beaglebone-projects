
// primeFinder.c
// Purpose: This module will create a thread that can be used to search for prime
//          numbers greater than a specified taget value (defined below these comments).
//	        A struct data structure is used internally to hold all primes found from the 
//          execution of this module and will be deallocated when the stop function is called.
//
// Author: Sunny Pritpal Singh (301244872)
// Class: CMPT 433 Embedded Systems @SFU
// Credit: User guides by Brian Fraser provided through CMPT 433 course website
//         also used his youtube videos and links to his demo code linked from the 
//         notes section on his website.

#define PRIMES_GREATER_THAN_OR_EQUAL_TO 5000000000
#define BUFFER_SIZE 16


#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include "primeFinder.h"

/* *****************************PRIVATE FUNCTIONS****************************** */
// not accessed outside of the module so they are all declared static to protect 
// them from malicious use 

// Determines which is next prime to compute and save the value
void* PrimeFinder_calcPrime(void* arg);

// Determines if a number is a prime or not
_Bool PrimeFinder_isPrime(unsigned long long primeValue); 

// Writes the prime number value to a dynamically allocated array
void PrimeFinder_writeToArray(unsigned long long primeValue);

// Writes the prime number value to a pipe
void PrimeFinder_writeToPipe(const char* message, FILE* stream);

// Used to delay execution of code
void PrimeFinder_delayCode(long seconds, long nanoseconds);
/* ***************************************************************************** */

struct primeStruct
{
	unsigned int howManyPrimes;
	unsigned long long *primeArray;
	bool isCalculating; //boolean flag: true if currently running, false otherwise
	bool shouldCalculate;
	bool delayMemDelete;
	long delay;
};

struct primeStruct primes = {.howManyPrimes = 0, .primeArray = NULL, .isCalculating = false, .shouldCalculate = 			true, .delayMemDelete = false, .delay = 0};


/*
 * Function:  PrimeFinder_launchThread
 * --------------------
 *  creates a new thread to check for prime numbers
 *
 *   pipeFileDesc: theside of the pipe that can be written to
 *
 *   returns: void
 */
void PrimeFinder_launchThread(int pipeFileDesc) {
		
	pthread_t primeThread;

	pthread_create(&primeThread, NULL, PrimeFinder_calcPrime, &pipeFileDesc);

	pthread_join(primeThread, NULL);
	//pthread_exit(0);

}

/*
 * Function:  PrimeFinder_calcPrime 
 * --------------------
 *  Launched by a call thread from pthread_create - this function will
 *   determine which number to start calculating primes from i.e if no
 *   previous prime has been found then it will start from the number defined
 *   in PRIMES_GREATER_THAN_OR_EQUAL_TO, otherwise it will check the primeArray
 *   to determine where to resume from.
 *
 *  arg*: this will be cast to an int ptr representing our pipe input file
 *   descriptor value passed in from pthread_create()
 *
 *  returns: void
 */
void* PrimeFinder_calcPrime(void* arg) {

	//This is our pipe which we will write to
	//allows the calling function to read the latest prime found
	int *fds = (int*) arg;

	//by default we will search for primes starting from this value
	unsigned long long startPrime = PRIMES_GREATER_THAN_OR_EQUAL_TO;

		primes.isCalculating = true;

		//Check to see if any primes have been previously found
		if (primes.primeArray != NULL) {
			startPrime = primes.primeArray[primes.howManyPrimes-1];
			startPrime++;
		}

		//loop until we find the next prime
		while(PrimeFinder_isPrime(startPrime) == false){
			startPrime++;
		}	

		//save our new prime to a dynamically allocated array
		PrimeFinder_writeToArray(startPrime);

		//convert our long long unsigned value to a string
		char buffer[BUFFER_SIZE];
		sprintf(buffer, "%llu", startPrime);
 		//write (fds[1], &startPrime,sizeof(unsigned long long ));
 		//write (fds[1], &buffer,64);
 		
 		
 		//printf(&startPrime);
		//save our new prime to the pipe
		FILE* stream = fdopen (*fds, "w");
		//PrimeFinder_writeToPipe(buffer,  stream);
		fprintf (stream, "%s\n", buffer);
	
		primes.isCalculating = false;
		
	fclose (stream);
	
	//set the delay time
	if (primes.delay > 500){
		PrimeFinder_delayCode(1,500*1000000);
	} else {
		PrimeFinder_delayCode(0,primes.delay*1000000);
	}
	
	return 0;
}

/*
 * Function:  PrimeFinder_isPrime 
 * --------------------
 *  determines if a given value is prime - I will just be using a naive 
 *   approach - nothing fancy.
 *
 *  primeValue: this number will be checked to see if it is prime
 *
 *  returns: true if the value is prime, false otherwise
 */
_Bool PrimeFinder_isPrime(unsigned long long primeValue) {

	int i =0;
	
	for(i = 2; i <= sqrt(primeValue); i++){

		if ((primeValue % i) == 0)
			return false; //not a prime
	}
	//found a prime
	return true;
}

/*
 * Function:  PrimeFinder_writeToArray 
 * --------------------
 *  Writes the prime number value to a new dynamically allocated array
 *   and copies over any old values.
 *
 *  primeValue: new prime number to be added to memory
 *
 *  returns: void
 */
void PrimeFinder_writeToArray(unsigned long long primeValue) {
	//increment our count of primes found
	primes.howManyPrimes++;
	int i = 0;
	
	//alloc new memory for array to hold old prime(s) and new prime found
	unsigned long long *answer = malloc((sizeof(unsigned long long)*primes.howManyPrimes));

	//copies over any old prime values previously found
	if (primes.howManyPrimes > 1) {
	
		for(i = 0; i < primes.howManyPrimes; i++) {

		answer[i] = primes.primeArray[i];
	
		}
	}

	answer[primes.howManyPrimes-1] = primeValue; //sets the new prime found into our new array
	free(primes.primeArray); //release our old memory
	primes.primeArray = answer;
}

/*
 * Function:  PrimeFinder_getNumPrimesFound
 * --------------------
 *  Gets the total number of primes found so far.
 *  
 *  returns: int - count of prime numbers
 */

unsigned int PrimeFinder_getNumPrimesFound(void) {

	return primes.howManyPrimes;
}

/*
 * Function:  PrimeFinder_writeToPi_Bool PrimeFinder_isCalculatingPrimes(void);pe
 * --------------------
 *  Writes a prime number to the pipe.
 *
 *  message: new prime number to be added to pipe - converted to string form
 *  ouputStream: the opened File object  that is set for writing
 *  
 *  returns: void
 */
void PrimeFinder_writeToPipe(const char* message, FILE* stream) {
		// Write the message to the stream, and send it off immediately.
		fprintf (stream, "%s\n", message);
		fflush (stream);
}

/*
 * Function:  PrimeFinder_getPrimeByIndex
 * --------------------
 *  Gets a prime number value by index number from the dynamic array
 *   which holds all the primes.
 *  
 *  returns: unsigned long long - prime number retrieved from array
 *   if the index value is out of range it will return 0
 */
unsigned long long PrimeFinder_getPrimeByIndex(int index) {

	if ((index <= 0) || (index > primes.howManyPrimes))
		return 0;

	return primes.primeArray[index -1];
}

/*
 * Function:  PrimeFinder_isCalculatingPrimes
 * --------------------
 *  Determines if the primes PrimeFinder_calcPrime() is currently
 *   running.
 *  
 *  returns: bool - true if currently calculating a prime number
 *   false otherwise
 */
_Bool PrimeFinder_isCalculatingPrimes(void) {

	return primes.isCalculating;
}

/*
 * Function:  PrimeFinder_shouldCalculate
 * --------------------
 *  Determines if the primes PrimeFinder_calcPrime() should be
 *   running.
 *  
 *  returns: bool - true if currently calculating a prime number
 *   false otherwise
 */
_Bool PrimeFinder_shouldCalculate(void) {

	return primes.shouldCalculate;
}
/*
 * Function:  PrimeFinder_stopCalculating
 * --------------------
 *  Exits the thread after freeing all dynamically allocated memory.
 *  
 *  returns: void
 */
void PrimeFinder_stopCalculating(void) {

	primes.shouldCalculate = false;

	if(primes.primeArray != NULL) 
		free(primes.primeArray);

	//pthread_exit(0);
}

/*
 * Function:  PrimeFinder_statusChange
 * --------------------
 *  Prepares to shutdown by signaling primes to stop.
 *  
 *  returns: void
 */
void PrimeFinder_statusChange(void) {

primes.shouldCalculate = false;

}


/*
 * Function:  PrimeFinder_setDelayBetweenPrimes
 * --------------------
 *  Determines if the primes PrimeFinder_calcPrime() is currently
 *   running.
 *  
 *  delayInMs - integer value in milleseconds that will be used
 *              by this delay function to pause code execution.
 *  
 *  returns: void
 */
void PrimeFinder_setDelayBetweenPrimes(long delayInNs) {

	primes.delay = delayInNs;
}


/*
 * Function:  PrimeFinder_delayCode 
 * --------------------
 * delays the execution of code using:
 *
 *  seconds: number of seconds to delay
 *  nanoseconds: number of nanoseconds to delay 
 *               (note: 100000000ns = 100ms)
 *  returns: void
 */
void PrimeFinder_delayCode(long seconds, long nanoseconds) {

	struct timespec reqDelay = {seconds, nanoseconds};
	nanosleep(&reqDelay, (struct timespec *) NULL);
	
}
















