#include <stdlib.h>
#include <stdio.h>
#include "emul.h"
#include "ppu.h"
#include "cpu.h"

struct Ext8bit colorExt[4] = {{.mask = 0b00000011, .dec = 0},
                              {.mask = 0b00001100, .dec = 2},
                              {.mask = 0b00110000, .dec = 4},
                              {.mask = 0b11000000, .dec = 6}};

/////////////General PPU operation /////////////

void initPPU(struct PPU * ppu, struct cpuGb* cpu){
    ppu->LCDC = cpu->mem[0xFF40];
    
    ppu->OAM = &cpu->mem[PPU_OAM_ADD];
    ppu->sizeOAM = PPU_OAM_SIZE; 

    //ppu->vramTM = &cpu->mem[]
    ppu->LY = &cpu->mem[PPU_LY];
    *ppu->LY = 153;
    ppu->STAT = &cpu->mem[PPU_STAT];
    ppu->mode = (struct Ext8bit) {.dec = 0, .mask = 0b00000011};

    //TODO : initialize the palette
}

void DMAOAMTransfer(struct cpuGb* cpu){
    //It doesn't use readFrom add because it's not a cpu operation

    cpu->stateTime.OAMDMATranfer = cpu->ticks;
    setFlag1(&cpu->state, OAMDMA_TRANSFER);

    uint16_t srcAdd =  ((uint16_t) cpu->mem[DMAOAM_TRANSFER_ADD])<<8;
    uint16_t dstAdd = PPU_OAM_ADD;
    
    for(int i=0; i<PPU_OAM_SIZE; i++){
        cpu->mem[dstAdd+i] = cpu->mem[srcAdd+i];
    }
} 

void PPU_IncLY(struct cpuGb* cpu, struct PPU * ppu){
    cpu->stateTime.ppuIncLY = cpu->stateTime.ppuIncLY + PPU_INC_LY_LEN;
    *ppu->LY = ((*ppu->LY)+1)%154;
}
//////////General Tile managment ////////
uint8_t * getTileDataAdd(struct cpuGb* cpu, struct PPU* ppu, enum TileType type, uint8_t index){
    if(type == OBJ){
        return &(cpu->mem[PPU_OBJTD_ADD + 16*index]);
    }
    if(extractBitsN(ppu->LCDC, BG_W_tdadd)){
        return &(cpu->mem[PPU_BGWTD_ADD_1 + 16*index]);
    }
    int8_t sindex = (int8_t) index;
    uint16_t add = PPU_BGWTD_ADD_0 + 16*sindex; //Check if there is no overflow here
    return &(cpu->mem[add]);
}

void getTile(struct cpuGb* cpu, struct PPU* ppu, enum TileType type, uint8_t index, uint8_t (* tile)[8]){
    uint8_t * p = getTileDataAdd(cpu, ppu, type, index);
    for(size_t i=0; i<8; i++){
        uint8_t pxLow = p[i*2];
        uint8_t pxHigh = p[i*2 + 1];
        for(size_t j=0; j<8; j++){ 
            tile[i][j] = extractBitsN(&pxLow, j) | (extractBitsN(&pxHigh, j)<<1);
        }
    }   
}

//y the relative line to get on the sprite
void getTileLine(struct cpuGb* cpu, struct PPU* ppu, enum TileType type, uint8_t index, uint8_t line[8], uint8_t y){
    uint8_t * p = getTileDataAdd(cpu, ppu, type, index);
    uint8_t hSprite = (extractBitsN(ppu->LCDC, OBJ_size) == 0)? 8:16;
    
    if(y>hSprite){
        fprintf(stderr, "Error while rendering : asking %d° line when sprite height is %d", y, hSprite);
        return;
    }
    uint8_t pxLow = p[y*2];
    uint8_t pxHigh = p[y*2 + 1];
    for(size_t j=0; j<8; j++){
        line[j] = extractBitsN(&pxLow, j) | (extractBitsN(&pxHigh, j)<<1);
    }
}


///////Sprite rendering ////////////
//Fill the sprite array (need to be of size 40) of the relative address of the object in the scanline 
uint8_t getScanlineSprites(struct PPU* ppu, uint8_t y, uint8_t * sprite){
    uint8_t nbSprite = 0;
    uint8_t hSprite = (extractBitsN(ppu->LCDC, OBJ_size) == 0)? 8:16;
    for(uint8_t add = 0; add<PPU_OAM_SIZE; add+=4){
        if(!(ppu->OAM[add] > y+16 || ppu->OAM[add] + hSprite < y +16)){
            sprite[nbSprite++] = add;
        }
    }
}

uint8_t getSpritePixelColor(uint8_t paletteId, uint8_t nPalette){

}

//add the relative address in the OAM table
void renderLineSprite(struct cpuGb* cpu, struct PPU* ppu, uint8_t yScan, uint8_t add){
    uint8_t * attrib = &ppu->OAM[add+3];
    uint8_t xSprite = ppu->OAM[add+1];
    uint8_t ySprite = ppu->OAM[add];
    uint8_t row = (extractBitsN(attrib, Yflip))? ppu->hSprite-(yScan+16 - ySprite):yScan+16 - ySprite;

    uint8_t index;
    if(extractBitsN(ppu->LCDC, OBJ_size) == 0){
        index = ppu->OAM[add+2] & 0xFE;
    }
    else{
        index = ppu->OAM[add+2] | 0x01;
    }

    static uint8_t line[8];

    getTileLine(cpu, ppu, OBJ, index, line, row);

    for(int i = 0; i<8; i++){
        if(i+xSprite-8>0 && i+xSprite-8<PIXEL_BY_WIDTH){
            uint8_t color = extractBits(&cpu->mem[PPU_OBP0_ADD+extractBitsN(attrib, PaletteNb)], colorExt[0x04&line[(extractBitsN(attrib, Xflip)==0)?i: 8-i]]);
            ppu->spriteRender[yScan][(i+xSprite)-8].value = color;
            ppu->spriteRender[yScan][(i+xSprite)-8].BGOver = extractBitsN(attrib, BGover);
        }
    }
}

void renderScanlineSprites(struct cpuGb* cpu, struct PPU* ppu, uint8_t y){
    uint8_t spriteAdd[PPU_OAM_MAX_SPRITE] = {0};
    uint8_t nbSprite = getScanlineSprites(ppu, y, spriteAdd);
    if(nbSprite>10){
        nbSprite = 10;
    }

    for(int i = nbSprite-1; i>=0; i--){
        renderLineSprite(cpu, ppu, y, spriteAdd[i]);
    }
}

/////////Background and windows rendering//////////

void renderBackground(struct cpuGb* cpu, struct PPU * ppu){
    
}

//////////Global rendering (PPU Mode) /////////////
void PPURenderScanline(struct cpuGb* cpu, struct PPU * ppu){
    ppu->hSprite = (extractBitsN(ppu->LCDC, OBJ_size) == 0)? 8:16;

}

void PPUMode1(struct cpuGb* cpu, struct PPU* ppu, struct Screen * screen){
    setFlag1(&cpu->state, PPU_MODE_1);
    cpu->stateTime.ppuMode1 = cpu->stateTime.ppuMode0 + PPU_MODE_0_LEN;
    writeBits(ppu->STAT, ppu->mode, 1);
    if((*ppu->STAT) & STAT_MODE1_Int){
        triggerInterrupt(cpu); //Trigger STAT interrupt
    }
}

//Need to update the cpu timing outside the function depending on when it's been called
void PPUMode2_3(struct cpuGb* cpu, struct PPU* ppu, struct Screen * screen){
    if(*ppu->LY>=143) 
        PPUMode1(cpu, ppu, screen);
    
    setFlag1(&cpu->state, PPU_MODE_2_3);

    if(cpu->mem[PPU_LYC] - *ppu->LY == 0){
        setFlag1(ppu->STAT, STAT_LYC);
        if((*ppu->STAT) & STAT_LYC_Int){
            triggerInterrupt(cpu); //Trigger STAT interrupt
        }
    }
    else{
        setFlag0(ppu->STAT, STAT_LYC);
    }

    writeBits(ppu->STAT, ppu->mode, 2);
    if((*ppu->STAT) & STAT_MODE2_Int){
        triggerInterrupt(cpu); //Trigger STAT interrupt
    }
    PPURenderScanline(cpu, ppu);
}

void PPUMode0(struct cpuGb* cpu, struct PPU* ppu){
    setFlag1(&cpu->state, PPU_MODE_0);
    cpu->stateTime.ppuMode0 = cpu->stateTime.ppuMode2 + PPU_MODE_2_3_LEN;
    writeBits(ppu->STAT, ppu->mode, 0);
    if((*ppu->STAT) & STAT_MODE0_Int){
        triggerInterrupt(cpu); //Trigger STAT interrupt
    }
}