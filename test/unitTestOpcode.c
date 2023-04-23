#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "emul.h"
#include "cpu.h"
#include "opcode.h"
#include "unitTestOpcode.h"
#include "unitTest.h"


void setNextVals(struct cpuGb* cpu, uint8_t * vals, size_t nb){
    *cpu->pc = 0;
    for(int i = 0; i<nb; i++){
        cpu->mem[i] = vals[i];
    }
}

int UT_writeBits(struct cpuGb* cpu){
    int ret = EXIT_SUCCESS;
    initRegister(cpu);
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
    initRegister(cpu);

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
    initRegister(cpu);
    
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
    initRegister(cpu);

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
    initRegister(cpu);

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
    initRegister(cpu);
    
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
    assertEgal("CB Get P", p, &(cpu->reg[rnH]))
    assertEgal("CB Get N", N, 0)

    setNextVals(cpu,(uint8_t []) {0xDB}, 1);
    regP = opcode_CB_getP(cpu, &p);
    N = opcode_CB_getN(cpu, regP);
    assertEgal("CB Get P", p, &(cpu->reg[rnE]))
    assertEgal("CB Get N", N, 3)

    setNextVals(cpu,(uint8_t []) {0xFF}, 1);
    regP = opcode_CB_getP(cpu, &p);
    N = opcode_CB_getN(cpu, regP);
    assertEgal("CB Get P", p, &(cpu->reg[rnA]))
    assertEgal("CB Get N", N, 7)

    setNextVals(cpu,(uint8_t []) {0x16}, 1);
    regP = opcode_CB_getP(cpu, &p);
    assertEgal("CB Get P", p, &(cpu->mem[cpu->reg16[HL]]))
ENDUT("CB Get P/N")

BEGUT(UT_opcode_jp)
    uint16_t add = 0x0454;
    opcode_jp(cpu, add);
    assertEgal("JP", *cpu->pc, add)
ENDUT("JP")

BEGUT(UT_opcode_call)
    uint16_t add = 0x0500;
    *cpu->pc = 0x0400;
    opcode_call(cpu, add);
    assertEgal("CALL PC", *cpu->pc, 0x0500)
    assertEgal("CALL SP", cpu->mem[*cpu->sp] | ((uint16_t)cpu->mem[(*cpu->sp) +1])<<8, 0x400)
ENDUT("CALL")

BEGUT(UT_opcode_ret)
    uint16_t add = 0x0500;
    *cpu->pc = 0x0400;
    opcode_call(cpu, add);
    opcode_ret(cpu);
    assertEgal("RET", *cpu->pc, 0x400)
ENDUT("RET")

BEGUT(UT_opcode_ccf)
    opcode_ccf(cpu, 0);
    assertEgal("CCF", *cpu->flags, 0b00010000)
    *cpu->flags = 0b00010000;
    opcode_ccf(cpu, 0);
    assertEgal("CCF", *cpu->flags, 0b00000000)
ENDUT("CCF")

BEGUT(UT_opcode_DAA)
    cpu->reg[rnA] = 0x3C;
    opcode_DAA(cpu, 0);
    assertEgal("DAA", cpu->reg[rnA], 0x42)
ENDUT("DAA")

BEGUT(UT_opcode_LD_X_nn)
    setNextVals(cpu, (uint8_t []) {0x77, 0x44}, 2);
    opcode_LD_X_nn(cpu, 0x01);
    assertEgal("LD X nn", cpu->reg16[BC], 0x4477)
    setNextVals(cpu, (uint8_t []) {0x77, 0x44}, 2);
    opcode_LD_X_nn(cpu, 0x11);
    assertEgal("LD X nn", cpu->reg16[DE], 0x4477)
    setNextVals(cpu, (uint8_t []) {0x77, 0x44}, 2);
    opcode_LD_X_nn(cpu, 0x21);
    assertEgal("LD X nn", cpu->reg16[HL], 0x4477)
    setNextVals(cpu, (uint8_t []) {0x67, 0x43}, 2);
    opcode_LD_X_nn(cpu, 0x21);
    assertEgal("LD X nn", cpu->reg16[SP], 0x4367)
ENDUT("LD X nn")

BEGUT(UT_opcode_INC_X)
    opcode_INC_X(cpu, 0x14);
    assertEgal("INC X", cpu->reg[rnD], 0x01);
ENDUT("INC X")
