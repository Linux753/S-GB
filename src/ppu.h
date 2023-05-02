#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

/////////////Screen constant and register address///////////////////
//#define SPF 16 //Ms by frame
//#define FREQUENCY 1000 //Expressed in ms^-1
#define RENDER_OAM 0xFE00
#define RENDER_OAMSIZE 0xA0
#define RENDER_SPRITETD 0x8000
#define RENDER_SPRITETDSIZE 0x1000

struct PPU{
    //Bg & Window 
    uint8_t * vramTD; //VRAM Tiles Data
    size_t sizeVramTD;
    uint8_t * vramTM; //VRAM Tiles Map
    size_t sizeVramTM;

    //Sprite:
    uint8_t * spriteTD; //Sprites Tiles Data
    size_t sizeSpriteTD;
    uint8_t * OAM; //Object Attributes Memory
    size_t sizeOAM;
};

void PPURenderScreen(struct Screen * screen);
#endif