#include <stdlib.h>
#include <stdio.h>
#include "emul.h"
#include "ppu.h"

void initPPU(struct PPU * ppu, struct cpuGb* cpu){
    ppu->LCDC = cpu->mem[0xFF40];
    
}

uint8_t * getTileDataAdd(struct cpuGb* cpu, struct PPU* ppu, enum TileType type, uint8_t index){
    if(type == OBJ){
        return &(cpu->mem[PPU_OBJTD_ADD + index]);
    }
    if(extractBitsN(ppu->LCDC, BG_W_tdadd)){
        return &(cpu->mem[PPU_BGTD_ADD_1 + index]);
    }
    int8_t sindex = (int8_t) index;
    uint16_t add = PPU_BGTD_ADD_0 + sindex;
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

void PPURenderScreen(struct Screen * screen){
    
}

void PPURenderScanline(struct PPU * ppu, struct Screen * screen){
    

}

