#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "emul.h"
#include "cpu.h"
#include "opcode.h"
#include "unitTestOpcode.h"
#include "unitTest.h"

void resetRegister(struct cpuGb* cpu){
    for(int i=0; i<REGISTER16_SIZE; i++){
        cpu->reg16[i] = 0x0000;
    }
}

void setNextVals(struct cpuGb* cpu, uint8_t * vals, size_t nb){
    cpu->reg16[PC] = 0;
    for(int i = 0; i<nb; i++){
        cpu->mem[i] = vals[i];
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
        fprintf(stderr, "WriteBits to C success\n");
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

int UT_opcode_rlc(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;

    uint8_t a = 0b10001111;
    opcode_rlc(cpu, &a);
    if(a != 0b00011111 || extractBits(cpu->flags, cpu->c) != 1){
        fprintf(stderr, "Opcode rlc failed flag H\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode rlc success\n");
    }

    return ret;
}

int UT_opcode_rl(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;

    writeBits(cpu->flags, cpu->c, 0);
    uint8_t a = 0b10001111;
    opcode_rl(cpu, &a);
    if(a != 0b00011110 || *cpu->flags != 0b00010000){
        fprintf(stderr, "Opcode rl failed flag H\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode rl success\n");
    }

    return ret;
}

int UT_opcode_rrc(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;

    uint8_t a = 0b10001110;
    opcode_rrc(cpu, &a);
    if(a != 0b01000111 || *cpu->flags != 0b00000000){
        fprintf(stderr, "Opcode rrc failed\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode rrc success\n");
    }

    return ret;
}

int UT_opcode_rr(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;

    writeBits(cpu->flags, cpu->c, 1);
    uint8_t a = 0b10001110;
    opcode_rr(cpu, &a);
    if(a != 0b11000111 || *cpu->flags != 0b00000000){
        fprintf(stderr, "Opcode rr failed\n");
        ret = EXIT_FAILURE;
    }

    if(ret == EXIT_SUCCESS){
        fprintf(stderr, "Opcode rr success\n");
    }

    return ret;
}

BEGUT(UT_opcode_sla)
    uint8_t res = 0b10001110;
    UTOpcode(cpu, opcode_sla, "SLA", &res, 0b00011100, 0b00010000, &res)
ENDUT("SLA")

BEGUT(UT_opcode_swap)
    uint8_t res = 0b10001110;
    UTOpcode(cpu, opcode_swap, "SWAP", &res, 0b00001111, 255, &res)
ENDUT("SWAP")

BEGUT(UT_opcode_sra)
    uint8_t res = 0b10000000;
    UTOpcode(cpu, opcode_sra, "SRA", &res, 0b11000000, 0, &res)
ENDUT("SRA")

BEGUT(UT_opcode_srl)
    uint8_t res = 0b10000000;
    UTOpcode(cpu, opcode_srl, "SRL", &res, 0b01000000, 0, &res)
ENDUT("SRL")

BEGUT(UT_opcode_bit)
    uint8_t res = 0b00010000;
    uint8_t n = 4;
    UTOpcode(cpu, opcode_bit, "BIT", &res, res, 0b10100000, n, &res); 
ENDUT("BIT")

BEGUT(UT_opcode_set)
    uint8_t res = 0b00000000;
    uint8_t n = 4;
    UTOpcode(cpu, opcode_set, "SET", &res, 0b00010000, 255, n, &res); 
ENDUT("SET")

BEGUT(UT_opcode_res)
    uint8_t res = 0b00110000;
    uint8_t n = 5;
    UTOpcode(cpu, opcode_res, "RES", &res, 0b00010000, 255, n, &res); 
ENDUT("RES")

BEGUT(UT_opcode_CB_getPN)
    uint8_t *p;
    
    setNextVals(cpu, (uint8_t []){0x84}, 1);
    uint8_t regP = opcode_CB_getP(cpu, &p);
    uint8_t N = opcode_CB_getN(cpu, regP);
    assertEgal("CB Get P/N", p, &(cpu->reg[rnH]))
    assertEgal("CB Get P/N", N, 0)

    setNextVals(cpu,(uint8_t []) {0xDB}, 1);
    regP = opcode_CB_getP(cpu, &p);
    N = opcode_CB_getN(cpu, regP);
    assertEgal("CB Get P/N", p, &(cpu->reg[rnE]))
    assertEgal("CB Get P/N", N, 3)

    setNextVals(cpu,(uint8_t []) {0xFF}, 1);
    regP = opcode_CB_getP(cpu, &p);
    N = opcode_CB_getN(cpu, regP);
    assertEgal("CB Get P/N", p, &(cpu->reg[rnA]))
    assertEgal("CB Get P/N", N, 7)

    setNextVals(cpu,(uint8_t []) {0x16}, 1);
    regP = opcode_CB_getP(cpu, &p);
    assertEgal("CB Get P", p, &(cpu->mem[cpu->reg16[HL]]))
ENDUT("CB Get P/N")
