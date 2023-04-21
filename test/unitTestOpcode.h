#ifndef UNITTESTOPCODE_H
#define UNITTESTOPCODE_H

#include "emul.h"

#define BEGUT(utName) int utName(struct cpuGb* cpu){\
    int ret = EXIT_SUCCESS;\
    initRegister(cpu);

/*Make a unit test for the function f, 
must have ret variable declared in the scope of call.
Set NULL for res if res are not tested.
Set flagsVal to 255 to not test flags.
The flag is reset before executing the function so untouched value are 0
Repeat res for the argument of the function if necessary*/
#define UTOpcode(cpu, f, fname, res, resVal, flagsVal, ...) *cpu->flags = 0x00;\
    f((cpu), __VA_ARGS__);\
    if((*(cpu)->flags != (flagsVal) && (flagsVal) != 255) || ((res)!=NULL && *(res) != resVal)){ fprintf(stderr, "%s failed\n", fname);\
    ret = EXIT_FAILURE;}

#define ENDUT(fname) if(ret == EXIT_SUCCESS) fprintf(stderr, "Opcode %s success\n", fname); return ret;}

int UT_opcode_ADD8bit(struct cpuGb* cpu);
int UT_opcode_ADC8bit(struct cpuGb* cpu);
int UT_opcode_SUB8bit(struct cpuGb* cpu);
int UT_opcode_INC8bit(struct cpuGb* cpu);
int UT_opcode_SBC8bit(struct cpuGb* cpu);
int UT_writeBits(struct cpuGb* cpu);
int UT_opcode_ADDdd(struct cpuGb* cpu);
int UT_opcode_rlc(struct cpuGb* cpu);
int UT_opcode_rl(struct cpuGb* cpu);
int UT_opcode_rrc(struct cpuGb* cpu);
int UT_opcode_rr(struct cpuGb* cpu);
int UT_opcode_sla(struct cpuGb* cpu);
int UT_opcode_swap(struct cpuGb* cpu);
int UT_opcode_sra(struct cpuGb* cpu);
int UT_opcode_srl(struct cpuGb* cpu);
int UT_opcode_bit(struct cpuGb* cpu);
int UT_opcode_res(struct cpuGb* cpu);
int UT_opcode_set(struct cpuGb* cpu);
int UT_opcode_CB_getPN(struct cpuGb* cpu);
int UT_opcode_jp(struct cpuGb* cpu);
int UT_opcode_call(struct cpuGb* cpu);
int UT_opcode_ret(struct cpuGb* cpu);
int UT_opcode_ccf(struct cpuGb* cpu);
int UT_opcode_DAA(struct cpuGb* cpu);
#endif