#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <time.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "beatmaker.h"

#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x1C
#define CONTROL_REGISTER 0x2A
#define NUM_DATA_BYTES 7
#define THRESHOLD 16000

int openI2cBus()
{
	int i2cFileDesc = open(I2CDRV_LINUX_BUS1, O_RDWR);
	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write\n");
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, I2C_DEVICE_ADDRESS);
	if (result < 0) {
		printf("Unable to set I2C device to slave address.");
	}
	return i2cFileDesc;
}

void readI2cReg(int i2cFileDesc, unsigned char regAddr, char* data)
{
	int result = write(i2cFileDesc, &regAddr, sizeof(regAddr));
	if (result != sizeof(regAddr)) {
		printf("Unable to write to I2C register");
	}

	
	result = read(i2cFileDesc, data, sizeof(char) * NUM_DATA_BYTES);
}

void writeI2cReg(int i2cFileDesc, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		printf("Unable to write i2c register");
	}
}

void* accelerometerRunner(void* args) 
{
	int openBus = openI2cBus();
	//Change the device to active mode
	writeI2cReg(openBus, CONTROL_REGISTER, 0x01);


	struct timespec pollingDelay = {0, 10000000};
	struct timespec debounceDelay = {0, 125000000};

	char data[NUM_DATA_BYTES];

	while(true) {
		readI2cReg(openBus, 0x00, data);

		int16_t x = (data[1] << 8) + (data[2]);
		int16_t y = (data[3] << 8) + (data[4]);
		int16_t z = (data[5] << 8) + (data[6]);

		if(x > THRESHOLD) {
			//printf("X\n");
			Beatmaker_playSound(1);
			nanosleep(&debounceDelay, (struct timespec *) NULL);
		}
		else if(y > THRESHOLD) {
			//printf("Y\n");
			Beatmaker_playSound(2);
			nanosleep(&debounceDelay, (struct timespec *) NULL);
		}
		else if(z > 2 * THRESHOLD) {
			//printf("Z\n");
			Beatmaker_playSound(3);
			nanosleep(&debounceDelay, (struct timespec *) NULL);
		}
		//printf("x=%d y= %d z = %d \n", x, y, z);
		nanosleep(&pollingDelay, (struct timespec *) NULL);
	}


	return NULL;
}

void Accelerometer_init()
{
	pthread_t threadID;
	pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
	pthread_create(&threadID, &threadAttr, accelerometerRunner, NULL);
}