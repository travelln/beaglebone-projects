#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <string.h>
#include "beatmaker.h"
#include <sys/sysinfo.h>

#define PORT_NO 11235
#define BUFFER_SIZE 2048

void respondToCommand(char *command, char *outputBuffer)
{
	if (strncmp(command, "hihat", 5) == 0) {
		snprintf(outputBuffer, BUFFER_SIZE, "playedHiHat");
		Beatmaker_playSound(HIHAT);
	}
	else if (strncmp(command, "snare", 5) == 0) {
		snprintf(outputBuffer, BUFFER_SIZE, "playedSnare");
		Beatmaker_playSound(SNARE);
	}
	else if (strncmp(command, "bass", 4) == 0) {
		snprintf(outputBuffer, BUFFER_SIZE, "playedBass");
		Beatmaker_playSound(BASS);
	}
	else if (strncmp(command, "pollMode", 8) == 0) {
		int mode = Beatmaker_getMode();
		snprintf(outputBuffer, BUFFER_SIZE, "mode %d", mode);
	}
	else if (strncmp(command, "pollVolume", 10) == 0) {
		int volume = Beatmaker_getVolume();
		snprintf(outputBuffer, BUFFER_SIZE, "volume %d", volume);
	}
	else if (strncmp(command, "pollTempo", 9) == 0) {
		int tempo = Beatmaker_getTempo();
		snprintf(outputBuffer, BUFFER_SIZE, "tempo %d", tempo);
	}
	else if (strncmp(command, "mode", 4) == 0) {
		//Only works for single digit modes! 
		int mode = command[5] - '0';
		mode = Beatmaker_setMode(mode);
		snprintf(outputBuffer, BUFFER_SIZE, "mode %d", mode);
	}
	else if (strncmp(command, "volumeUp", 8) == 0) {
		int volume = Beatmaker_increaseVolume();
		snprintf(outputBuffer, BUFFER_SIZE, "volume %d", volume);
	}
	else if (strncmp(command, "volumeDown", 10) == 0) {
		int volume = Beatmaker_decreaseVolume();
		snprintf(outputBuffer, BUFFER_SIZE, "volume %d", volume);
	}
	else if (strncmp(command, "tempoUp", 7) == 0) {
		int tempo = Beatmaker_increaseTempo();
		snprintf(outputBuffer, BUFFER_SIZE, "tempo %d", tempo);
	}
	else if (strncmp(command, "tempoDown", 9) == 0) {
		int tempo = Beatmaker_decreaseTempo();
		snprintf(outputBuffer, BUFFER_SIZE, "tempo %d", tempo);
	}
	else if (strncmp(command, "pollUptime", 10) == 0) {
		struct sysinfo info;
		sysinfo(&info);
		snprintf(outputBuffer, BUFFER_SIZE, "uptime %ld", info.uptime);
	}
	else {
		snprintf(outputBuffer, BUFFER_SIZE, "huh");
	}
}

void* interfaceRunner(void* args)
{
	int socketDesc;
	struct sockaddr_in serverAddress, clientAddress;
	socklen_t addressLength = sizeof(serverAddress);

	char commandBuffer[BUFFER_SIZE];
	int numReceived;

	socketDesc = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketDesc < 0) {
		printf("Could not open socket");
	}

	serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT_NO);

    if (bind(socketDesc, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0){
    	printf("Could not bind socket");
    }

    while(true) {
    	numReceived = recvfrom(socketDesc, commandBuffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddress, &addressLength);
    	if (numReceived > 0 && numReceived < BUFFER_SIZE) {
   			//Rather than overwrite the entire buffer, just ensure the last character is made null
   			commandBuffer[numReceived] = 0;

   			//printf("Message length: %d: %s\n", numReceived, commandBuffer);
   			char responseBuffer[BUFFER_SIZE];
   			respondToCommand(commandBuffer, responseBuffer);

   			sendto(socketDesc, responseBuffer, strlen(responseBuffer), 0, (struct sockaddr *)&clientAddress, addressLength);
   		}
    }

    return NULL;
}

void UdpInterface_init()
{
	pthread_t threadID;
	pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
	pthread_create(&threadID, &threadAttr, interfaceRunner, NULL);
}