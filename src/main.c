#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "emul.h"
#include "loadROM.h"
#include "cpu.h"

int main(int argc, char ** argv){
    srand(time(NULL));

    struct GB gb;

    initGB(&gb);

    loadROM(&gb, "testROM/tetris.gb");

    printHeaderInfo(&(gb.metadataROM));

    emulate(&gb);

    printf("\nNot enough implemented to show something, run test/ut to run unit test\n");

    return EXIT_SUCCESS;
}