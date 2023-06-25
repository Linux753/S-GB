#ifndef PPU_H
#define PPU_H

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "screen.h"
#include "emul.h"

/////////////Screen constant and register address///////////////////
//#define SPF 16 //Ms by frame
//#define FREQUENCY 1000 //Expressed in ms^-1
#define PPU_OAM_ADD 0xFE00
#define PPU_OAM_SIZE 0x9F
#define PPU_OAM_MAX_SPRITE 40
#define PPU_SPRITETD 0x8000
#define PPU_SPRITETDSIZE 0x1000

#define PPU_BGP_ADD 0xFF47
#define PPU_OBP0_ADD 0xFF48
#define PPU_OBP1_ADD 0xFF49

#define PPU_BGWTD_ADD_1 0x8000
#define PPU_BGWTD_ADD_0 0x9000
#define PPU_BGWTM_ADD_0 0x9800
#define PPU_BGWTM_ADD_1 0x9C00
#define PPU_OBJTD_ADD 0x8000
#define PPU_OBJTD_ADD_MAX 0x8FFF

#define PPU_LY 0xFF44
#define PPU_LYC 0xFF45
#define PPU_STAT 0xFF41
#define STAT_LYC_Int 0b01000000
#define STAT_MODE2_Int 0b00100000
#define STAT_MODE1_Int 0b00010000
#define STAT_MODE0_Int 0b00001000
#define STAT_LYC 0b00000100
#define STAT_MODE 0b00000011

enum LCDCBit{
    BG_on=0, //Activate the window
    OBJ_on, //Activate the objects
    OBJ_size, //0 for 8*8 , 1 for 8*16 object
    BG_tmadd, //
    BG_W_tdadd, //Select the address to get the tile data
    W_on, //Activate the window
    W_tmzone, //
    LCD_on //
};

enum SpriteAttribute{
    PalNb=0, //CGB only
    TileVRAMBank=3,//CGB only
    PaletteNb = 4, //Non-CGB only
    Xflip = 5,
    Yflip = 6,
    BGover = 7
};

enum TileType{
    BG,WINDOW,OBJ
};

struct SpritePixel{
    uint8_t value;
    bool BGOver;
};

struct PPU{
    //Bg & Window 
    uint8_t * vramTM; //VRAM Tiles Map
    size_t sizeVramTM;

    //Sprite:
    uint8_t * OAM; //Object Attributes Memory
    size_t sizeOAM;

    uint8_t * LCDC;

    uint8_t hSprite;

    struct SpritePixel spriteRender[PIXEL_BY_HEIGHT][PIXEL_BY_WIDTH];

    ///Register
    uint8_t * LY;
    uint8_t * STAT;
    struct Ext8bit mode;
};



void PPURenderScreen(struct Screen * screen);
void DMAOAMTransfer(struct cpuGb* cpu);
void PPUMode0(struct cpuGb* cpu, struct PPU* ppu);
void PPUMode1(struct cpuGb* cpu, struct PPU* ppu, struct Screen * screen);
void PPUMode2_3(struct cpuGb* cpu, struct PPU* ppu, struct Screen * screen);
void PPU_IncLY(struct cpuGb* cpu, struct PPU * ppu);
#endif