#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "emul.h"
#include "cpu.h"
#include "opcode.h"

void resetRegister(struct cpuGb* cpu){
    for(int i=0; i<REGISTER16_SIZE; i++){
        cpu->reg16[i] = 0x0000;
    }
}

int UT_writeBits(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;
    resetRegister(cpu);
    writeBits(cpu->flags, cpu->c, 1);
    if(extractBits(cpu->flags, cpu->c) != 1){
        fprintf(stderr, "WriteBits to C failed\n");
        ret = EXIT_FAILURE;
    }
    else{
        fprintf(stderr, "WriteBits to C succeed\n");
    }
    return ret;
}

int UT_opcode_ADD8bit(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;
    resetRegister(cpu);

    uint8_t a = 0xF0;
    uint8_t b = 0x10;
    uint8_t res;
    opcode_ADD8bit(cpu, a, b, &res);
    if(extractBits(cpu->flags, cpu->c) != 1){
        fprintf(stderr, "Opcode ADD 8 bit failed flag C\n");
        ret = EXIT_FAILURE;
    }

    a = 0x0F;
    b = 0x01;
    res = 0;
    opcode_ADD8bit(cpu, a, b, &res);
    if(extractBits(cpu->flags, cpu->h) != 1){
        fprintf(stderr, "Opcode ADD 8 bit failed flag H\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode ADD 8 bit success\n");
    }

    return ret;
}

int UT_opcode_ADC8bit(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;
    resetRegister(cpu);
    
    writeBits(cpu->flags, cpu->c, 1);

    uint8_t a = 0xEE;
    uint8_t b = 0x10;
    uint8_t res = 0;
    opcode_ADC8bit(cpu, a, b, &res);

    if(extractBits(cpu->flags, cpu->c) != 1){
        fprintf(stderr, "Opcode ADC 8 bit failed flag C\n");
        ret = EXIT_FAILURE;
    }

    writeBits(cpu->flags, cpu->c, 1);
    a = 0x0E;
    b = 0x01;
    res = 0;
    opcode_ADC8bit(cpu, a, b, &res);
    if(extractBits(cpu->flags, cpu->h) != 1){
        fprintf(stderr, "Opcode ADC 8 bit failed flag H\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode ADC 8 bit success\n");
    }

    return ret;
}

int UT_opcode_INC8bit(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;
    resetRegister(cpu);

    uint8_t a = 0x0F;
    uint8_t res = 0;
    opcode_INC8bit(cpu, a, &res);
    if(extractBits(cpu->flags, cpu->h) != 1){
        fprintf(stderr, "Opcode INC 8 bit failed flag H\n");
        ret = EXIT_FAILURE;
    }

    return ret;
}

int UT_opcode_SUB8bit(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;
    resetRegister(cpu);

    uint8_t a = 0x00;
    uint8_t b = 0x05;
    uint8_t res = 0;
    opcode_SUB8bit(cpu, a, b, &res);

    if(extractBits(cpu->flags, cpu->c) != 1){
        fprintf(stderr, "Opcode SUB 8 bit failed flag C\n");
        ret = EXIT_FAILURE;
    }

    a = 0x10;
    b = 0x05;
    res = 0;
    opcode_SUB8bit(cpu, a, b, &res);
    if(extractBits(cpu->flags, cpu->h) != 1){
        fprintf(stderr, "Opcode SUB 8 bit failed flag H\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode SUB 8 bit success\n");
    }

    return ret;
}

int UT_opcode_SBC8bit(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;
    resetRegister(cpu);
    
    writeBits(cpu->flags, cpu->c, 1);

    uint8_t a = 0x05;
    uint8_t b = 0x05;
    uint8_t res = 0;
    opcode_SBC8bit(cpu, a, b, &res);

    if(extractBits(cpu->flags, cpu->c) != 1){
        fprintf(stderr, "Opcode SBC 8 bit failed flag C\n");
        ret = EXIT_FAILURE;
    }

    writeBits(cpu->flags, cpu->c, 1);
    a = 0x13;
    b = 0x03;
    res = 0;
    opcode_SBC8bit(cpu, a, b, &res);
    if(extractBits(cpu->flags, cpu->h) != 1){
        fprintf(stderr, "Opcode SBC 8 bit failed flag H\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode SBC 8 bit success\n");
    }

    return ret;
}

int UT_opcode_SCF(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;
    resetRegister(cpu);
    
    opcode_SCF(cpu, 0);
    if(extractBits(cpu->flags, cpu->c) != 1){
        fprintf(stderr, "Opcode SCF failed flag C\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode SCF 8 bit success\n");
    }

    return ret;
}

int UT_opcode_ADDdd(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;

    uint16_t a = 0xFFE3;
    int8_t dd = 0x05;
    uint16_t res;
    opcode_ADDdd(cpu, a, dd, &res);
    if(extractBits(cpu->flags, cpu->c) != 0 || extractBits(cpu->flags, cpu->h) != 0){
        fprintf(stderr, "Opcode ADDdd failed flag C or H\n");
        ret = EXIT_FAILURE;
    }

    a = 0xFFEF;
    dd = 0x20;
    opcode_ADDdd(cpu, a, dd, &res);
    if(extractBits(cpu->flags, cpu->c) != 1 && extractBits(cpu->flags, cpu->h) != 0){
        fprintf(stderr, "Opcode ADDdd failed flag C\n");
        ret = EXIT_FAILURE;
    }

    a = 0xFF01;
    dd = 0x75;
    opcode_ADDdd(cpu, a, dd, &res);
    if(extractBits(cpu->flags, cpu->h) != 1 && extractBits(cpu->flags, cpu->c) != 0){
        fprintf(stderr, "Opcode ADDdd failed flag H\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode Adddd 8 bit success\n");
    }
    return ret;
}
