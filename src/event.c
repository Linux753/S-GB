#include <stdlib.h>
#include <stdio.h>
#include "emul.h"

void initKeyTable(struct Control * control){
    SDL_Scancode * table = control->keyTable;
    table[0] = SDL_SCANCODE_UP; //UP
    table[1] = SDL_SCANCODE_DOWN; //DOWN
    table[2] = SDL_SCANCODE_LEFT; //LEFT
    table[3] = SDL_SCANCODE_RIGHT; //RIGHT
    table[4] = SDL_SCANCODE_KP_0; //SELECT
    table[5] = SDL_SCANCODE_KP_ENTER; //START
    table[6] = SDL_SCANCODE_KP_1; //A
    table[7] = SDL_SCANCODE_KP_2; //B
    table[8] = SDL_SCANCODE_Z;
    table[9] = SDL_SCANCODE_S;
    table[10] = SDL_SCANCODE_Q;
    table[11] = SDL_SCANCODE_D;
    table[12] = SDL_SCANCODE_A;
    table[13] = SDL_SCANCODE_E;
    table[14] = SDL_SCANCODE_F;
    table[15] = SDL_SCANCODE_G;
}

void initControl(struct Control * control, struct cpuGb * cpu){
    memset(control, SDL_FALSE, sizeof(control->key));
    control->quit = SDL_FALSE;
    control->resizeWindow = SDL_FALSE;

    initKeyTable(control);

    control->controlP = (uint8_t *) &(cpu->mem[CONTROL_JYP_ADD]);
}


void updateControl(struct Chip16 * chip){
    struct Control * control = &(chip->control);

    //Updating the SDL Event 
    updateEvent(control); 
    
    
}

void resizeWindow(struct Screen * screen){
    int w, h;
    SDL_GetWindowSize(screen->window, &w, &h);
    screen->pixelWidth = w/PIXEL_BY_WIDTH;
    screen->pixelHeight = h/PIXEL_BY_HEIGHT;
    SDL_SetWindowSize(screen->window, screen->pixelWidth * PIXEL_BY_WIDTH, screen->pixelHeight * PIXEL_BY_HEIGHT);
}

void analyseEvent(struct Chip16 * chip16){ //Analyze event relevent to the emulator itself
    if(chip16->control.resizeWindow == SDL_TRUE){
        resizeWindow(&(chip16->screen));
    }
    if(chip16->control.key[SDL_SCANCODE_O] == SDL_TRUE){
        renderScreen(&(chip16->screen));
    }
}