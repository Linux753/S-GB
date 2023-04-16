#ifndef MANAGEPA_H
#define MANAGEPA_H

#define FRAMES_PER_BUFFER 64

#include <stdint.h>
#include <stdbool.h>
#include "emul.h"


int initPa(struct Sound * sound, int channelCount);
void playTone(struct Sound *sound, int frequency, uint16_t ms ,bool advSound /* Set True for 0D opcode only*/);
void abortSoundStream(struct Sound * sound);
void stopSoundStream(void *userData);
#endif