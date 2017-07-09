/* ****************************************************************************** */
// FourteenSegDisplay.c
// Purpose: This module will display to the 14 segment display, integer values
//		from 0-99.
//
// Author: Sunny Pritpal Singh (301244872)
// Class: CMPT 433 Embedded Systems @SFU
// Credit: User guides by Brian Fraser provided through CMPT 433 course website
//         also used his youtube videos and links to his demo code linked from the 
//         notes section on his website.
//	   This module uses code from Brian's I2C Guide with ammendments.
/* ****************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "primeFinder.h"
#include "A2DPot.h"
#include "14SegDisplay.h"

#define I2C_DEVICE_ADDRESS 0x20


#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14
#define REG_OUTB 0x15

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define EXPORT_FILE_LOCATION "/sys/devices/platform/bone_capemgr/slots"
#define EXPORT_VALUE "BB-I2C1"

#define EXPORT_FILE_GPIO "/sys/class/gpio/export"
#define EXPORT_FILE_61 "/sys/class/gpio/gpio61/value"
#define EXPORT_FILE_44 "/sys/class/gpio/gpio44/value"
#define EXPORT_FILE_44_DIR "/sys/class/gpio/gpio44/direction"
#define EXPORT_FILE_61_DIR "/sys/class/gpio/gpio61/direction"

#define EXPORT_VALUE_61 "61"
#define EXPORT_VALUE_44 "44"

#define EXPORT_VALUE_OUT "out"
#define EXPORT_VALUE_ONE "1"
#define EXPORT_VALUE_ZERO "0"

//lookup table for digits 0 -> 9
int digitTop[10] = {0x86,0x02,0x0e,0x06,0x8a,0x8c,0x88,0x06,0x8e,0x8e};
int digitBottom[10] = {0xa1,0x80,0x61,0xb0,0xd0,0xb0,0xb1,0x80,0xb1,0xb0};

/*
 * Function:  initI2cBus
 * --------------------
 *  Initialize the bus
 *
 *
 *   returns: integer value for error code if any
 */
static int initI2cBus(char* bus, int address)
{
	int i2cFileDesc = open(bus, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C: Unable to open bus for read/write (%s)\n", bus);
		perror("Error is:");
		exit(1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, address);

	if (result < 0) {
		perror("I2C: Unable to set I2C device to slave address.");
		exit(1);
	}

return i2cFileDesc;

}

/*
 * Function:  writeI2cReg
 * --------------------
 *  Writes a value to the I2C device register
 *
 *
 *   returns: void
 */
static void writeI2cReg(int i2cFileDesc, unsigned char regAddr,unsigned char value)
{
	unsigned char buff[2];
	
	buff[0] = regAddr;
	buff[1] = value;
	
	int res = write(i2cFileDesc, buff, 2);
	
	if (res != 2) {
		perror("I2C: Unable to write i2c register.");
		exit(1);
	}
}

/*
 * Function:  turnOnDigit
 * --------------------
 *  Turns on either the left, right or both digits
 *   of the two 14 digit segments.
 *
 *   returns: void
 */
void turnOnDigit(int digit){

	if (digit == 1){//turn on left digit 61
	
		
		FILE *export_file = openFile(EXPORT_FILE_61, "w");
		//This will make sure that the ADC cape is loaded
		fprintf(export_file, "%s", EXPORT_VALUE_ONE);
		fclose(export_file);
		
		export_file = openFile(EXPORT_FILE_44, "w");
		//This will make sure that the ADC cape is loaded
		fprintf(export_file, "%s", EXPORT_VALUE_ZERO);
		fclose(export_file);
	
	}
	else if (digit == 2){//turn on right digit 44

		FILE *export_file = openFile(EXPORT_FILE_61, "w");
		//This will make sure that the ADC cape is loaded
		fprintf(export_file, "%s", EXPORT_VALUE_ONE);
		fclose(export_file);
		
		export_file = openFile(EXPORT_FILE_44, "w");
		//This will make sure that the ADC cape is loaded
		fprintf(export_file, "%s", EXPORT_VALUE_ZERO);
		fclose(export_file);
			
	}
	else if (digit == 3){//turn on both digits

		//First we need to open up the export file
		FILE *export_file = openFile(EXPORT_FILE_GPIO, "w");
		//This will make sure that the ADC cape is loaded
		fprintf(export_file, "%s", EXPORT_VALUE_61);
		fclose(export_file);
		
		
		export_file = openFile(EXPORT_FILE_GPIO, "w");
		//This will make sure that the ADC cape is loaded
		fprintf(export_file, "%s", EXPORT_VALUE_44);
		fclose(export_file);
		
		//set the out direction
		export_file = openFile(EXPORT_FILE_61_DIR, "w");
		fprintf(export_file, "%s", EXPORT_VALUE_OUT);
		fclose(export_file);
		
		export_file = openFile(EXPORT_FILE_44_DIR, "w");
		//Set the out direction
		fprintf(export_file, "%s", EXPORT_VALUE_OUT);
		fclose(export_file);
		
		export_file = openFile(EXPORT_FILE_61, "w");
		//enable output on left digit
		fprintf(export_file, "%s", EXPORT_VALUE_ONE);
		fclose(export_file);
		
		export_file = openFile(EXPORT_FILE_44, "w");
		//enable output on right digit
		fprintf(export_file, "%s", EXPORT_VALUE_ONE);
		fclose(export_file);
	}

}

/*
 * Function:  writeOutToDisplay
 * --------------------
 *  Writes a value to the 14 segement display.
 *
 *   returns: void
 */
void writeOutToDisplay(int valueTop, int valueBottom){

	int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);
	
	writeI2cReg(i2cFileDesc, REG_OUTA, valueBottom); //left - bottom
	writeI2cReg(i2cFileDesc, REG_OUTB, valueTop);
	close(i2cFileDesc);
}

/*
 * Function:  outToDisplay
 * --------------------
 *  Recieves input from calling module containg the value to be
 *    written to the segment display.
 *
 *   toWrite - up to a two digit value that gets parsed for output.
 *   returns: void EXPORT_FILE_GPIO 
 */
void outToDisplay(int toWrite)
{

	//First we need to open up the export file
	FILE *export_file = openFile(EXPORT_FILE_GPIO, "w");
	//This will make sure that the ADC cape is loaded
	fprintf(export_file, "%s", EXPORT_VALUE_61);
	fclose(export_file);
	
	//First we need to open up the export file
	export_file = openFile(EXPORT_FILE_GPIO, "w");
	//This will make sure that the ADC cape is loaded
	fprintf(export_file, "%s", EXPORT_VALUE_44);
	fclose(export_file);
		


	toWrite = abs(toWrite);

	if(toWrite == 10)
		toWrite--;
	if(toWrite == 12)
		toWrite = toWrite -3;
		
	//if two digit value, then turn on both segments on display
	if (toWrite > 9){
	 toWrite = toWrite % 10;
	 turnOnDigit(3);

	}
	else {//if one digit value then turn on only left digit
		turnOnDigit(1);
	}

	int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS1, I2C_DEVICE_ADDRESS);

	writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
	writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
	writeI2cReg(i2cFileDesc, REG_OUTA, 0x00); //left - bottom
	writeI2cReg(i2cFileDesc, REG_OUTB, 0x00); //left - top
	
	writeOutToDisplay(digitTop[toWrite], digitBottom[toWrite]);

	close(i2cFileDesc);

return;
}







