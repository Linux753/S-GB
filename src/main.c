#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "emul.h"
#include "loadROM.h"
#include "cpu.h"

int main(int argc, char ** argv){
    srand(time(NULL));

    struct Chip16 chip16;

    initCPU(&(chip16.cpu));

    loadROM(&chip16, "testROM/tetris.gb");

    printHeaderInfo(&(chip16.metadataROM));

    printf("\nNot enough implemented to show something, run test/ut to run unit test\n");

    return EXIT_SUCCESS;
}