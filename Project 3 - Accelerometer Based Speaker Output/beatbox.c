#include "audioMixer.h"
#include "beatmaker.h"
#include "udpInterface.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

int main(int argc, char* argv[])
{
	AudioMixer_init();
	UdpInterface_init();
	Beatmaker_init();
}

