#ifndef BEAT_MAKER_H
#define BEAT_MAKER_H

#define HIHAT 1
#define SNARE 2
#define BASS 3

int Beatmaker_setMode(int mode);

int Beatmaker_increaseTempo();

int Beatmaker_decreaseTempo();

void Beatmaker_init();

int Beatmaker_increaseVolume();

int Beatmaker_decreaseVolume();

void Beatmaker_changeMode();

void Beatmaker_playSound(int sound);

int Beatmaker_getMode();

int Beatmaker_getVolume();

int Beatmaker_getTempo();

#endif
