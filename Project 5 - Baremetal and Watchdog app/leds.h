

#ifndef _LEDS_H_
#define _LEDS_H_
#include <stdbool.h>




/*****************************************************************************
 **                INTERNAL FUNCTION PROTOTYPES
 *****************************************************************************/
void initializeLeds(void);
void driveLedsWithSWFunction(void);
void driveLedsWithSetAndClear(void);
void driveLedsWithBitTwiddling(void);
void busyWait(unsigned int count);


#endif
