#include <stdlib.h>
#include <stdio.h>
#include "emul.h"
#include "screen.h"

void initKeyTable(struct Control * control){
    SDL_Scancode * table = control->keyTable;
    table[0] = SDL_SCANCODE_UP; //RIGHT
    table[1] = SDL_SCANCODE_DOWN; //LEFT
    table[2] = SDL_SCANCODE_LEFT; //UP
    table[3] = SDL_SCANCODE_RIGHT; //DOWN
    table[4] = SDL_SCANCODE_Q; //B
    table[5] = SDL_SCANCODE_S; //A 
    table[6] = SDL_SCANCODE_D; //SELECT 
    table[7] = SDL_SCANCODE_F; //START
}

void initControl(struct Control * control, struct cpuGb * cpu){
    memset(control, SDL_FALSE, sizeof(control->key));
    control->quit = SDL_FALSE;
    control->resizeWindow = SDL_FALSE;

    initKeyTable(control);

    control->controlP = (uint8_t *) &(cpu->mem[CONTROL_JYP_ADD]);
}

//Update the key table and other relevent event variable
//Return if a key was pressed or not
bool updateEvent(struct Control * control){ 
    bool ret = false;
    SDL_Event event;

    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_KEYUP:
                control->key[event.key.keysym.scancode] = SDL_FALSE;
                break;
            case SDL_KEYDOWN:
                control->key[event.key.keysym.scancode] = SDL_TRUE;
                ret = true;
                break;
            case SDL_QUIT:
                control->quit = SDL_TRUE;
                break;
            case SDL_WINDOWEVENT:
                if(event.window.event == SDL_WINDOWEVENT_RESIZED){
                    control->resizeWindow = SDL_TRUE;
                }
                break;
        }
    }
    return ret;
}


void updateControl(struct GB * gb){
    struct Control * control = &(gb->control);
    //Updating the SDL Event 
    bool keyPressed = updateEvent(control);

    *control->controlP &= ~((*control->controlP)&0x0F); //Resetting the controller state bits

    if((*control->controlP)&CONTROL_SELECT_ACTION){
        for(int i=0; i<4; i++){
            if(control->key[control->keyTable[i]]) 
                *control->controlP |= (0x01<<i);
        }
    }
    if((*control->controlP)&CONTROL_SELECT_DIR){
        for(int i=0; i<4; i++){
            if(control->key[control->keyTable[i+4]])
                *control->controlP |= (0x01<<i);
        }
    }

    analyseEvent(gb);

    if(keyPressed){
        interruptRequest(&gb->cpu);
    }
}

void resizeWindow(struct Screen * screen){
    int w, h;
    SDL_GetWindowSize(screen->window, &w, &h);
    screen->pixelWidth = w/PIXEL_BY_WIDTH;
    screen->pixelHeight = h/PIXEL_BY_HEIGHT;
    SDL_SetWindowSize(screen->window, screen->pixelWidth * PIXEL_BY_WIDTH, screen->pixelHeight * PIXEL_BY_HEIGHT);
}

void analyseEvent(struct GB * gb){ //Analyze event relevent to the emulator itself
    if(gb->control.resizeWindow == SDL_TRUE){
        resizeWindow(&(gb->screen));
    }
    if(gb->control.key[SDL_SCANCODE_O] == SDL_TRUE){
        renderScreen(&(gb->screen));
    }
}