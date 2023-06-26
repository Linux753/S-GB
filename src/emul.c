#include <stdlib.h>
#include <stdint.h>
#include "emul.h"

int initGB(struct GB* gb){
    int ret = EXIT_FAILURE;

    ret = EXIT_SUCCESS;

    initCPU(&gb->cpu);
    initControl(&gb->control, &gb->cpu);
    initPPU(&gb->ppu, &gb->cpu);
    ret |= initScreen(&gb->screen);

    return ret;
}

void emulate(struct GB* gb){
    while(!gb->control.quit){
        execute(&gb->cpu);

        updateControl(gb);

        analyseEvent(gb);

        printf("%x, %d\n", *gb->cpu.pc, gb->cpu.mem[BOOT_ROM_DISABLE]);
    }
}

    void quitGB(struct GB* gb){
    destroyScreen(&gb->screen);
}