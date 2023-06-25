#ifndef EMUL_H
#define EMUL_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "cpu.h"
#include "ppu.h"
#include "screen.h"
#include "event.h"
#include "loadROM.h"
//For the moment only support DMG and MGB

struct GB{
    struct cpuGb cpu;
    struct Control control;

    struct MetadataROM metadataROM;

    //Rendering    
    struct PPU ppu; // Picture Processing Unit : struct managing registers relative to the rendering
    struct Screen screen; //struct managing the rendering of the emulator in itself
};

//////////FUNCTION TAKING GB'S POINTER AS PARAMETERS///////////
void updateControl(struct GB * gb);
int loadROM(struct GB * gb, char * path);

#endif

