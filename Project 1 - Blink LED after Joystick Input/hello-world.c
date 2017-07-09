
//Assignment 1: hello-world.c
//Purpose: Access GPIO and LED lights on the BeagleBone.
//         If the joysitck is in the neutral position the program
//         will flash the boards LED light once, if the joystick
//         is pressed up then the LED  will blink 3 times.
//         The program will display a message after it checks the 
//         state of the joystick -- if the joystick is pressed up 10 
//         times then the program will exit. A dealy is processed
//         so that the LED lights blinks for a longer duration.
//Author: Sunny Pritpal Singh (301244872)
//Credit: User guides by Brian Fraser 
//provided through CMPT 433 course website

#define TRIGGER_FILE_NAME "/sys/class/leds/beaglebone:green:usr0/trigger"
#define LED_CONTROL_BRIGHTNESS "/sys/class/leds/beaglebone:green:usr0/brightness"
#define EXPORT_FILE_LOCATION "/sys/class/gpio/export"
#define JSUP_VALUE_LOCATION "/sys/class/gpio/gpio26/value"
#define JOYSTICK_UP_GPIO_VALUE 26
#define TRIGGER_FILE "none"
#define TRIGGER_MODE "brightness"

#define TRIGGER_DELAY_100ms 100000000
#define TRIGGER_DELAY_700ms 700000000
#define LED_OFF "0"
#define LED_ON "1"
#define BRIGHTNESS_DELAY_1 1
#define BRIGHTNESS_DELAY_3 3
#define JOYSTICK_MAX_PRESSED 9

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

/* Function Declerations */
FILE* openFile(const char *name,const char *mode);
void printToFile(FILE* toPrint, const char *value);
void delayCode(long sec, long nsec);
void flashLED(int mode);
void turnLEDOn();
void turnLEDOff();
void changeTriggerMode();
_Bool checkIfPressed();



int main() 
{
	int up_press_count = 0; //counter to record joystick presses
	int led_flash_mode = 0; //how many times to flash the LED light

	
	printf("Hello embedded world, from Sunny Singh!\n");

    /*MAIN LOOP BODY FOR USER INTERACTION*/
	while(1) {

    /*READ VALUES FROM BOARD*/	
		//Read the value file associated with JSUP GPIO 26	
	        _Bool joystick_pressed_up = checkIfPressed();	
	
		//increment joystick counter if pressed and determine how many times
		//to flash the LED accordingly
		if (joystick_pressed_up == true) {

			up_press_count++;
	        	led_flash_mode = BRIGHTNESS_DELAY_3;	
		}
		else
			led_flash_mode = BRIGHTNESS_DELAY_1;

    /*PRINT OUTPUT MESSAGE*/
	        printf("Flashing %d time(s):  Joystick = %d & counter = %d\n", led_flash_mode, joystick_pressed_up, up_press_count);

    /*FLASH THE LED ON THE BEAGLE BOARD*/
		flashLED(led_flash_mode);

		//if joystick has been held up 10 times then 
		//print goodbye message and exit program
		if (up_press_count == JOYSTICK_MAX_PRESSED) {
			printf("Thank you for the blinks!\n");
			exit(1);
		}

	}//end WHILE

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
 * Function:  turnLEDOn
 * --------------------
 * Turns the led on specified by a defined path in LED_CONTROL_BRIGHTNESS:
 *
 *  returns: void
 */
void turnLEDOn() {

	//open the brightness control
	FILE *pLedBrightnessFile = fopen(LED_CONTROL_BRIGHTNESS, "w");	
        fprintf(pLedBrightnessFile, LED_ON);
	//printToFile(pLedBrightnessFile, "1");

	fclose(pLedBrightnessFile);
}

/*
 * Function:  turnLEDOff
 * --------------------
 * Turns the led off specified by a defined path in LED_CONTROL_BRIGHTNESS:
 *
 *  returns: void
 */
void turnLEDOff() {

	FILE *pLedBrightnessFile = fopen(LED_CONTROL_BRIGHTNESS, "w");

	//make sure the LED is off before we start
	fprintf(pLedBrightnessFile, LED_OFF);
	fclose(pLedBrightnessFile);
}

/*
 * Function:  changeTriggerMode 
 * --------------------
 * changes the trigger file mode specified by TRIGGER_MODE:
 *
 *  returns: void
 */
void changeTriggerMode() {

	//First we need to open the trigger file
	FILE *pLedTriggerFile = fopen(TRIGGER_FILE_NAME, "w");

	//change the current value of the trigger to "none"
	//this allows us to manually control it
	printToFile(pLedTriggerFile, TRIGGER_FILE);
	
	fclose(pLedTriggerFile);
}

/*
 * Function:  checkIfPressed 
 * --------------------
 * checks if the joystick is pressed up:
 *
 *  returns: boolean true if the joystick is pressed up
 *           boolean false if the joystick is not pressed
 */
_Bool checkIfPressed() {

	 //First we need to open up the export file
	FILE *export_file = openFile(EXPORT_FILE_LOCATION, "w");	
	//Write the pin number to the export file of the JSUP signal on the Zen cape
	fprintf(export_file, "%d", JOYSTICK_UP_GPIO_VALUE);
	
	fclose(export_file);
	//Open the value file corresponding with the signal so that we can read it
	FILE *jsup_value_file = openFile(JSUP_VALUE_LOCATION, "r");

	int value_read = 0;

	//read the value stored for the joystick up file
	fscanf(jsup_value_file, "%d", &value_read);

	//TESTING STATEMENT - outputs the value of jsup file
	//printf("\n VALUE = %d  \n", value_read);
	
	//invert the value read so that 1 means pressed and 0 means no
	if (value_read == 1) {
		//fclose(export_file);
		fclose(jsup_value_file);
		return false;
	}
	else{
		//fclose(export_file);
                fclose(jsup_value_file);
		return true;
	}
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

/*
 * Function:  flashLED 
 * --------------------
 * flashes the beagleboard led 1 with a delay using:
 *
 *  mode: number of times to flash
 *
 *  returns: void
 */
void flashLED(int mode) {

	//if mode is one than flash the LED once
	if (mode == BRIGHTNESS_DELAY_1) {

	turnLEDOn();  		                     //ON for 100ms
	delayCode(0,TRIGGER_DELAY_100ms);	     //delay
	turnLEDOff(); 		                     //OFF for 700ms
	delayCode(0,TRIGGER_DELAY_700ms);
	return;
	}

	//if mode is 3 then flash the LED 3 times
	if (mode == BRIGHTNESS_DELAY_3) {

	turnLEDOn();  	                            //ON for 100ms (1)
	delayCode(0,TRIGGER_DELAY_100ms);           //dealy
	turnLEDOff();                               //OFF for 100ms 
	delayCode(0,TRIGGER_DELAY_100ms);	    //delay
	turnLEDOn();                                //ON for 100ms (2)
	delayCode(0,TRIGGER_DELAY_100ms);           //delay
	turnLEDOff();                               //OFF for 100ms
	delayCode(0,TRIGGER_DELAY_100ms);	    //delay
	turnLEDOn();                                //ON for 100ms (3)
	delayCode(0,TRIGGER_DELAY_100ms);	    //delay
	turnLEDOff();                               //OFF for 700ms
	delayCode(0,TRIGGER_DELAY_700ms);

	}	
	return;
}




