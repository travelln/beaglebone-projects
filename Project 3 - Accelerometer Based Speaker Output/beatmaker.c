#include "audioMixer.h"
#include "accelerometer.h"
#include "joystick.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "beatmaker.h"

#define NUM_MODES 4
#define MAX_VOLUME 100
#define MAX_TEMPO 300
#define MIN_TEMPO 40
#define MAX_BARS 20
#define SMALLEST_BEAT 16
#define NUM_INSTRUMENTS 3

int tempo = 120;
int volume = 80;
int mode = 3;
wavedata_t hihat;
wavedata_t snare;
wavedata_t snareh;
wavedata_t bass;
wavedata_t bassh;

//Highest level is a bar,
//each bar has an array per instrument.
//One bool per sixteenth beat
//True is play that instrument
//False is silence
//I totally didn't go overboard at all. Nope.
const _Bool sunday[8][NUM_INSTRUMENTS][SMALLEST_BEAT] = 
{
	{
		{false, false, true, true, false, true, true, true, true, true, true, true, true, true, false, false}, //Hi Hat
		{true, true, false, false, true, false, false, false, false, false, false, false, false, false, true, true}, //Snare
		{true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false} //Bass
	},
	{
		{true, true, true, true, true, true, false, false, true, true, true, true, true, true, false, false}, //Hi Hat
		{false, false, false, false, false, false, true, true, false, false, false, false, false, false, true, true}, //Snare
		{true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false} //Bass
	},
	{
		{false, false, true, true, false, true, true, true, true, true, true, true, true, true, false, false}, //Hi Hat
		{true, true, false, false, true, false, false, false, false, false, false, false, false, false, true, true}, //Snare
		{true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false} //Bass
	},
	{
		{true, true, true, true, true, true, false, false, true, true, true, true, true, true, false, true}, //Hi Hat
		{false, false, false, false, false, false, true, true, false, false, false, false, false, false, true, false}, //Snare
		{true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false} //Bass
	},
	{
		{true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, false}, //Hi Hat
		{false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true}, //Snare
		{true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false} //Bass
	},
	{
		{true, true, true, true, true, true, true, true, true, true, true, true, true, true, false, true}, //Hi Hat
		{false, false, false, false, false, false, false, false, false, false, false, false, false, false, true, false}, //Snare
		{true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false} //Bass
	},
	{
		{true, true, true, true, true, true, false, false, true, true, true, true, true, true, true, true}, //Hi Hat
		{false, false, false, false, false, false, true, true, false, false, false, false, false, false, false, false}, //Snare
		{true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false} //Bass
	},
	{
		{true, true, false, true, true, true, true, true, true, true, true, true, true, true, false, false}, //Hi Hat
		{false, false, true, false, false, false, false, false, false, false, false, false, false, false, true, true}, //Snare
		{true, false, false, false, true, false, false, false, true, false, false, false, true, false, false, false} //Bass
	}
};

_Bool levee[1][NUM_INSTRUMENTS][SMALLEST_BEAT] = {
	{
		{true, false, true, false, true, false, true, false, true, false, true, false, true, false, true, false}, //Hi Hat
		{false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false}, //Snare
		{true, false, false, false, false, false, false, true, false, false, true, true, false, false, false, false} //Bass
	}
};

void standardBeat(int beat)
{
	if (beat % 4 == 0) {
		AudioMixer_queueSound(&bass);
	}
	if (beat % 4 == 2) {
		AudioMixer_queueSound(&snare);
	}
	AudioMixer_queueSound(&hihat);
}

void nonStandardBeat(int beat, int bar, const _Bool track[][NUM_INSTRUMENTS][SMALLEST_BEAT])
{
	if(track[bar][0][beat]) {
		AudioMixer_queueSound(&hihat);
	}
	if(track[bar][1][beat]) {
		AudioMixer_queueSound(&snareh);
	}
	if(track[bar][2][beat]) {
		AudioMixer_queueSound(&bass);
	}
}

void Beatmaker_init() 
{
	AudioMixer_readWaveFileIntoMemory("beatbox-wav-files/100053__menegass__gui-drum-cc.wav", &hihat);
	AudioMixer_readWaveFileIntoMemory("beatbox-wav-files/100059__menegass__gui-drum-snare-soft.wav", &snare);
	AudioMixer_readWaveFileIntoMemory("beatbox-wav-files/100058__menegass__gui-drum-snare-hard.wav", &snareh);
	AudioMixer_readWaveFileIntoMemory("beatbox-wav-files/100052__menegass__gui-drum-bd-soft.wav", &bass);
	AudioMixer_readWaveFileIntoMemory("beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav", &bassh);

	Accelerometer_init();
	Joystick_init();

	struct timespec currentDelay = {0,500000000};
	int sixteenthBeat = 0;
	int bar = 0;

	while(true) {
		//One sixteenth beat
		currentDelay.tv_nsec = (long)((60.0/tempo/4.0) * 1000000000);
		switch(mode) {
			case 0:
				break;
			case 1:
				standardBeat(sixteenthBeat);
				break;
			case 2:
				nonStandardBeat(sixteenthBeat, bar % 8, sunday);
				break;
			case 3:
				nonStandardBeat(sixteenthBeat, 0, levee);
				break;
			default:
				break;
		}
		if (sixteenthBeat == SMALLEST_BEAT - 1) {
			bar = (bar + 1) % MAX_BARS;
		}
		sixteenthBeat = (sixteenthBeat + 1) % SMALLEST_BEAT;
		nanosleep(&currentDelay, (struct timespec *) NULL);
	}
}

int Beatmaker_increaseTempo() 
{
	tempo += 5;
	if (tempo > MAX_TEMPO) {
		tempo = MAX_TEMPO;	
	}
	return tempo;
}


int Beatmaker_decreaseTempo() 
{
	tempo -= 5;
	if (tempo < MIN_TEMPO) {
		tempo = MIN_TEMPO;	
	}
	return tempo;
}

int Beatmaker_increaseVolume()
{
	volume += 5;
	if (volume > MAX_VOLUME) {
		volume = MAX_VOLUME;
	}
	AudioMixer_setVolume(volume);
	return volume;
}

int Beatmaker_decreaseVolume()
{
	volume -= 5;
	if (volume < 0) {
		volume = 0;
	}
	AudioMixer_setVolume(volume);
	return volume;
}

void Beatmaker_changeMode()
{
	mode = (mode + 1) % NUM_MODES;
}

int Beatmaker_setMode(int inputMode)
{
	if(inputMode >= 0 && inputMode <= NUM_MODES) {
		mode = inputMode;
	}
	return mode;
}

void Beatmaker_playSound(int sound)
{
	if(sound == HIHAT) {
		AudioMixer_queueSound(&hihat);
	}
	else if(sound == SNARE) {
		AudioMixer_queueSound(&snare);
	}

	else if(sound == BASS) {
		AudioMixer_queueSound(&bassh);
	}
}

int Beatmaker_getMode()
{
	return mode;
}

int Beatmaker_getVolume()
{
	return volume;
}

int Beatmaker_getTempo()
{
	return tempo;
}