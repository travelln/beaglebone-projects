
//Author: Sunny Pritpal Singh
//CMPT 433
//Purpose: main function that links to all other modules
//  This program is a baremetal application that uses the watchdog timer,
//    led, joystick and user input to flash leds and display options to the user.
// Credit: Course material proveided by Brian Fraser.




#include "consoleUtils.h"
#include "soc_AM335x.h"
#include "beaglebone.h"
#include "error.h"
#include "hw_types.h"

#include "serial.h"
#include "timers.h"
#include "fakeTyper.h"
#include "leds.h"

#include "joystick.h"




#define PRM_DEV             0x44E00F00   // base
#define PRM_RSTST_OFFSET    0x08    // reg offset


int main(void)
{
	// Initialization
	Serial_init();
	Timers_timerInit();
	Timers_watchdogInit();
	FakeTyper_init();

		//get the reset value
	unsigned int resetValue = HWREG(PRM_DEV + PRM_RSTST_OFFSET);
	// Display startup messages to console:
	
	ConsoleUtilsPrintf("\nWelcome to Light Bouncer\n");
	ConsoleUtilsPrintf("  by Sunny Singh\n");
	//ConsoleUtilsPrintf("Reset register value before: %d\n", resetValue);
	ConsoleUtilsPrintf("-----------------\n");
	if (resetValue == 1){
	
		ConsoleUtilsPrintf("Reset Source (0x1) = cold reset\n");	
	}
	
	if ((resetValue == 30) || (resetValue == 10) || (resetValue == 48) || (resetValue == 16)){
	
		ConsoleUtilsPrintf("Reset Source (0x10) = watchdog reset\n");	
	}	
	
	if ((resetValue == 20)|| (resetValue == 32)){
	
		ConsoleUtilsPrintf("Reset Source (0x20) = warm reset\n"); //pressed the reset button	
	}
	//ConsoleUtilsPrintf("   Type 'j' to queue a joke.\n");

	ConsoleUtilsPrintf("\nCommands:\n");	
	ConsoleUtilsPrintf("?    : Display this help message.\n");
	ConsoleUtilsPrintf("0-9  : Set speed 0 (slow) to 9 (fast)\n");
	ConsoleUtilsPrintf("a    : Select pattern A (bounce).\n");
	ConsoleUtilsPrintf("b    : Select patter B (bar).\n");
	ConsoleUtilsPrintf("x    : Stop hitting the watchdog.\n");


		//drive 1's to the reset register
	HWREG(PRM_DEV + PRM_RSTST_OFFSET) = 110011;
	
	// Main loop:
	while(1)
	{
		// Handle background processing
		Serial_doBackgroundWork();
		FakeTyper_doBackgroundWork();

		_Bool isButtonPressed = readButtonWithBitTwiddling();
		if (isButtonPressed ==  true)
		{
			initializeLeds();

			driveLedsWithSWFunction();
		
			//ConsoleUtilsPrintf("\nJoystick is pressed up:\n");
		}
		//	ConsoleUtilsPrintf("\nButton state: %d\n" ,isButtonPressed);
		// Timer ISR signals intermittent background activity.
		if(Timers_isIsrFlagSet())
		{
			Timers_clearIsrFlag();
			Timers_hitWatchdog();
		}
	}
}
