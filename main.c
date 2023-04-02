#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "emul.h"
#include "manageEmul.h"
#include "manageSDL.h"

int main(int argc, char ** argv){
    struct Chip16 chip16;

    srand(time(NULL));

    if(initChip16(&chip16) != EXIT_SUCCESS){
        fprintf(stderr, "Error while initializing the emulator");
    }

    if(argc<2){
        loadROM(&(chip16.cpu), "ROMPack/ROMs/Demos/Intro.c16");
    }
    else{
        loadROM(&(chip16.cpu), argv[1]);
    }
    emulateChip16(&chip16);

    return EXIT_SUCCESS;
}