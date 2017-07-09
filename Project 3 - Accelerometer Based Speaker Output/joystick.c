#include <stdio.h>
#include <stdbool.h>
#include "beatmaker.h"
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define GPIO_EXPORT_FILE "/sys/class/gpio/export"

#define PRESSED 48

#define JOYSTICK_UP "26"
#define JOYSTICK_UP_OUTPUT "/sys/class/gpio/gpio26/direction"
#define JOYSTICK_UP_VALUE "/sys/class/gpio/gpio26/value"

#define JOYSTICK_DOWN "46"
#define JOYSTICK_DOWN_OUTPUT "/sys/class/gpio/gpio46/direction"
#define JOYSTICK_DOWN_VALUE "/sys/class/gpio/gpio46/value"

#define JOYSTICK_LEFT "65"
#define JOYSTICK_LEFT_OUTPUT "/sys/class/gpio/gpio65/direction"
#define JOYSTICK_LEFT_VALUE "/sys/class/gpio/gpio65/value"

#define JOYSTICK_RIGHT "47"
#define JOYSTICK_RIGHT_OUTPUT "/sys/class/gpio/gpio47/direction"
#define JOYSTICK_RIGHT_VALUE "/sys/class/gpio/gpio47/value"

#define JOYSTICK_IN "27"
#define JOYSTICK_IN_OUTPUT "/sys/class/gpio/gpio27/direction"
#define JOYSTICK_IN_VALUE "/sys/class/gpio/gpio27/value"

void checkFilePointer(FILE *pFilePointer, const char* pin)
{
	if (pFilePointer == NULL) {
		printf("%s could not be opened: %s\n", pin, strerror(errno));
		exit(1);
	}
}

void checkFileWrite(int writeStatus)
{
	if (writeStatus <= 0) {
		printf("A GPIO File could not be written to\n");
	}
}

void setupGPIO(const char* pin, const char* pinFile)
{
	FILE *pGPIOExportFile = fopen(GPIO_EXPORT_FILE, "w");
	checkFilePointer(pGPIOExportFile, GPIO_EXPORT_FILE);
	checkFileWrite(fprintf(pGPIOExportFile, "%s", pin));
	fclose(pGPIOExportFile);

	FILE *pOutputFile = fopen(pinFile, "w");
	checkFilePointer(pOutputFile, pinFile);
	checkFileWrite(fprintf(pOutputFile, "in"));
	fclose(pOutputFile);
}

_Bool checkPinState(const char* pin)
{
	FILE *pJoyStick = fopen(pin, "r");
	checkFilePointer(pJoyStick, pin);
	int state = fgetc(pJoyStick);
	if (state == PRESSED) {
		fclose(pJoyStick);
		return true;
	}
	else{
		fclose(pJoyStick);
		return false;
	}

}

void* joystickRunner(void* arg)
{
	struct timespec pollingDelay = {0, 10000000};
	struct timespec debounceDelay = {0, 180000000};
	while(true) {
		if (checkPinState(JOYSTICK_LEFT_VALUE)) {
			Beatmaker_increaseTempo();
			nanosleep(&debounceDelay, (struct timespec *) NULL);
		}
		if (checkPinState(JOYSTICK_RIGHT_VALUE)) {
			Beatmaker_decreaseTempo();
			nanosleep(&debounceDelay, (struct timespec *) NULL);
		}
		if (checkPinState(JOYSTICK_UP_VALUE)) {
			Beatmaker_increaseVolume();
			nanosleep(&debounceDelay, (struct timespec *) NULL);
		}
		if (checkPinState(JOYSTICK_DOWN_VALUE)) {
			Beatmaker_decreaseVolume();
			nanosleep(&debounceDelay, (struct timespec *) NULL);
		}
		if (checkPinState(JOYSTICK_IN_VALUE)) {
			Beatmaker_changeMode();
			nanosleep(&debounceDelay, (struct timespec *) NULL);
		}
		nanosleep(&pollingDelay, (struct timespec *) NULL);
	}
	return NULL;
}

void Joystick_init() 
{
	setupGPIO(JOYSTICK_UP, JOYSTICK_UP_OUTPUT);
	setupGPIO(JOYSTICK_DOWN, JOYSTICK_DOWN_OUTPUT);
	setupGPIO(JOYSTICK_LEFT, JOYSTICK_LEFT_OUTPUT);
	setupGPIO(JOYSTICK_RIGHT, JOYSTICK_RIGHT_OUTPUT);
	setupGPIO(JOYSTICK_IN, JOYSTICK_IN_OUTPUT);
	//Create the thread here
	pthread_t threadID;
	pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
	pthread_create(&threadID, &threadAttr, joystickRunner, NULL);
}