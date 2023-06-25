#ifndef EVENT_H
#define EVENT_H

#include <stdlib.h>
#include <SDL2/SDL.h>
#include "emul.h"

/////////MACRO DEFINITION///////
#define CONTROL_NB 8
#define CONTROL_JYP_ADD 0xFF00
#define CONTROL_SELECT_ACTION 0b00100000
#define CONTROL_SELECT_DIR 0b00010000
#define CONTROL_DOWN_START 0b00001000
#define CONTROL_UP_SELECT 0b00000100
#define CONTROL_LEFT_B 0b00000010
#define CONTROL_RIGHT_A 0b00000001

////////STRUCTURE DEFINITION//////
struct Control{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    SDL_bool resizeWindow;

    SDL_Scancode keyTable[CONTROL_NB];

    uint8_t * controlP;
};

void initControl(struct Control * control, struct cpuGb * cpu);

#endif