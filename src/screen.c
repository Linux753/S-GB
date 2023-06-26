#include <stdlib.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "screen.h"
#include "ppu.h"

//Init the SDL part of the struct Screen
int initSDL(struct Screen * screen){ //Done
    if(0!= SDL_Init(SDL_INIT_TIMER|SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS)){
        fprintf(stderr, "Error SDL_init: %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    screen->window = SDL_CreateWindow("SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              INIT_SCREEN_WIDTH, INIT_SCREEN_HEIGHT, SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE);
    if(screen->window == NULL){
        return EXIT_FAILURE;
    }

    screen->renderer = SDL_CreateRenderer(screen->window, -1, SDL_RENDERER_ACCELERATED);
    if(screen->renderer == NULL){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int initScreen(struct Screen * screen){
    screen->pixelHeight = INIT_PIXEL_DIM;
    screen->pixelWidth = INIT_PIXEL_DIM;

    screen->nextRefresh = SDL_GetTicks();

    screen->palette[0] = (SDL_Color) {.a = 255, .r = 255, .g = 255, .b = 255};
    screen->palette[1] = (SDL_Color) {.a = 255, .r = 175, .g = 175, .b = 175};
    screen->palette[2] = (SDL_Color) {.a = 255, .r = 100, .g = 100, .b = 100};
    screen->palette[3] = (SDL_Color) {.a = 255, .r = 0, .g = 0, .b = 0};

    memset(screen->screen, 0, sizeof(screen));

    return initSDL(screen);
}

int SetRenderPaletteColor(SDL_Renderer * renderer, struct Screen * screen, uint8_t color){ //Done
    return SDL_SetRenderDrawColor(renderer, screen->palette[color].r, screen->palette[color].g, screen->palette[color].b, screen->palette[color].a);
}

void renderScreen(struct Screen * screen){
    
    //Prepare screen array for render using GB's memory data
    PPURenderScreen(screen);


    if(0!=SetRenderPaletteColor(screen->renderer, screen, 0)){
        fprintf(stderr, "Error while clearing the renderer : SDL_RenderClear : %s", SDL_GetError());

    }
    if(0 != SDL_RenderClear(screen->renderer)){
        fprintf(stderr, "Error while clearing the renderer : SDL_RenderClear : %s", SDL_GetError());
    }
    SDL_Rect pixel = {0, 0, 0, 0};
    for(int i=0; i<PIXEL_BY_WIDTH; i++){
        for(int j=0; j<PIXEL_BY_HEIGHT; j++){
            if(0!=SetRenderPaletteColor(screen->renderer, screen, screen->screen[i][j])){
                fprintf(stderr, "Error while clearing the renderer : SDL_RenderClear : %s", SDL_GetError());
            }
            pixel.x = i*screen->pixelWidth;
            pixel.y = j*screen->pixelHeight;
            pixel.h = screen->pixelHeight;
            pixel.w = screen->pixelWidth;
            if(0!= SDL_RenderFillRect(screen->renderer, &pixel)){
                fprintf(stderr, "Error while drawing pixel SDL_RenderFillRect() : %s", SDL_GetError());
            }  
        }
    }
    
    SDL_RenderPresent(screen->renderer);
}

void destroyScreen(struct Screen * screen){ //Done
    if(screen->window != NULL){
        SDL_DestroyWindow(screen->window);
    }
    if(screen->renderer != NULL){
        SDL_DestroyRenderer(screen->renderer);
    }
    SDL_Quit();
}