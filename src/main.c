#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "emul.h"
#include "loadROM.h"
#include "cpu.h"

int main(int argc, char ** argv){
    srand(time(NULL));

    struct GB gb;

    initCPU(&(gb.cpu));

    loadROM(&gb, "testROM/tetris.gb");

    printHeaderInfo(&(gb.metadataROM));

    int i=0;
    while(!execute(&(gb.cpu))){
        printf("%x, %d\n", *gb.cpu.pc, gb.cpu.mem[BOOT_ROM_DISABLE]);
        if(*gb.cpu.pc==0x28){
            printf("Here");
        }
    }

    printf("\nNot enough implemented to show something, run test/ut to run unit test\n");

    return EXIT_SUCCESS;
}