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

#define PPU_BGTD_ADD_1 0x8000
#define PPU_BGTD_ADD_0 0x9000
#define PPU_OBJTD_ADD 0x8000
#define PPU_OBJTD_ADD_MAX 0x8FFF
enum LCDCBit{
    BG_on=0 , OBJ_on, OBJ_size, BG_tmadd, BG_W_tdadd,
    W_on, W_tmzone, LCD_on
};

enum TileType{
    BG,WINDOW,OBJ
};

struct PPU{
    //Bg & Window 
    uint8_t * vramTM; //VRAM Tiles Map
    size_t sizeVramTM;

    //Sprite:
    uint8_t * OAM; //Object Attributes Memory
    size_t sizeOAM;

    uint8_t * LCDC;
};



void PPURenderScreen(struct Screen * screen);
#endif