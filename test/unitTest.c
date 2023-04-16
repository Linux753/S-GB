#include <stdlib.h>
#include "unitTestOpcode.h"
#include "emul.h"

int main(void){
    int ret = EXIT_SUCCESS;

    struct cpuGb cpu;
    initCPU(&cpu);

    UT_writeBits(&cpu);
    UT_opcode_ADD8bit(&cpu);
    UT_opcode_ADC8bit(&cpu);
    UT_opcode_SUB8bit(&cpu);
    UT_opcode_INC8bit(&cpu);
    UT_opcode_SBC8bit(&cpu);
    UT_opcode_SCF(&cpu);
    UT_opcode_ADDdd(&cpu);
    UT_opcode_rlc(&cpu);
    UT_opcode_rl(&cpu);
    UT_opcode_rrc(&cpu);
    UT_opcode_rr(&cpu);
    UT_opcode_sla(&cpu);
    UT_opcode_swap(&cpu);
    UT_opcode_sra(&cpu);
    UT_opcode_srl(&cpu);

    return ret;
}