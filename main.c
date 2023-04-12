#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "emul.h"
#include "manageSDL.h"
#include "unitTestOpcode.h"
#include "cpu.h"

int main(int argc, char ** argv){
    srand(time(NULL));

    struct cpuGb cpu;
    initCPU(&cpu);

    UT_writeBits(&cpu);
    UT_opcode_ADD8bit(&cpu);
    UT_opcode_ADC8bit(&cpu);
    UT_opcode_SUB8bit(&cpu);
    UT_opcode_INC8bit(&cpu);
    UT_opcode_SBC8bit(&cpu);
    UT_opcode_SCF(&cpu);

    return EXIT_SUCCESS;
}