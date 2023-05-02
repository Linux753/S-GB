#ifndef MANAGESDL_H
#define MANAGESDL_H

#include <SDL2/SDL.h>
#include "ppu.h"

#define INIT_PIXEL_DIM 5
#define PIXEL_BY_WIDTH 160
#define PIXEL_BY_HEIGHT 144
#define INIT_SCREEN_WIDTH PIXEL_BY_WIDTH*INIT_PIXEL_DIM
#define INIT_SCREEN_HEIGHT PIXEL_BY_HEIGHT*INIT_PIXEL_DIM
#define PALETTE_SIZE 4

struct Screen{
    SDL_Window * window;
    SDL_Renderer * renderer;
    

    uint32_t pixelHeight;
    uint32_t pixelWidth;

    uint32_t nextRefresh;
    
    uint8_t screen[PIXEL_BY_WIDTH][PIXEL_BY_HEIGHT];

    //Palette of color used by the GB's palettes (in BGP, OBP0 and OBP1 register)
    SDL_Color palette[PALETTE_SIZE];

    struct PPU ppu;
};

int initScren(struct Screen * screen);
void destroyScreen(struct Screen * screen);
void renderScreen(struct Screen * screen);

#endif