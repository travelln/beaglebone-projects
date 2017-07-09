
// primeFinder.h
// Purpose: This header file contains the function headers for the functions 
//	declared in the primeFinder.c file. Please see the .c file for 
// 	detailed explanations of implementation.
//	     
//
// Author: Sunny Pritpal Singh (301244872)
// Class: CMPT 433 Embedded Systems @SFU
// Credit: User guides by Brian Fraser provided through CMPT 433 course website
//         also used his youtube videos and links to his demo code linked from the 
//         notes section on his website.

#ifndef _PRIME_FINDER_H_
#define _PRIME_FINDER_H_


/* *****************************PUBLIC FUNCTIONS******************************** */
// can be accessed by external modules and main processes


// Begin computing primes on a separate thread. Given the pipe
// handle into which it should push any primes found.
void PrimeFinder_launchThread(int pipeFileDesc);

// Return how many primes have been found.
unsigned int PrimeFinder_getNumPrimesFound(void);

// Get a prime by its index. Returns 0 if index is out of range.
unsigned long long PrimeFinder_getPrimeByIndex(int index);

// Return true if currently computing primes.
_Bool PrimeFinder_isCalculatingPrimes(void);

// Stop calculating primes and free all memory.
void PrimeFinder_stopCalculating(void);

// Setup delay between primes:
// (The use of this is described in the A2D assignment section)
void PrimeFinder_setDelayBetweenPrimes(long delayInNs);

_Bool PrimeFinder_shouldCalculate(void);

void PrimeFinder_statusChange(void);

long PrimeFinder_getDelay();

#endif
