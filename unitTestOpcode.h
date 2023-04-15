#ifndef UNITTESTOPCODE_H
#define UNITTESTOPCODE_H
/*Make a unit test for the function f, 
must have ret variable declared in the scope of call.
Set NULL for res if res are not tested.
Set flagsVal to 255 to not test flags.
Repeat res for the argument of the function if necessary*/
#define BEGUT(fname) int fname(struct cpuGb* cpu){\
    int ret = EXIT_SUCCESS;
#define UTOpcode(cpu, f, fname, res, resVal, flagsVal, ...) f((cpu), __VA_ARGS__);\
    if((*(cpu)->flags != (flagsVal) && (flagsVal) != 255) || (*(res) != resVal && (res)!=NULL)){ fprintf(stderr, "%s failed\n", fname);\
    ret = EXIT_FAILURE;}

#define ENDUT(fname) if(ret == EXIT_SUCCESS) fprintf(stderr, "Opcode %s success\n", fname); return ret;}

int UT_opcode_ADD8bit(struct cpuGb* cpu);
int UT_opcode_ADC8bit(struct cpuGb* cpu);
int UT_opcode_SUB8bit(struct cpuGb* cpu);
int UT_opcode_INC8bit(struct cpuGb* cpu);
int UT_opcode_SBC8bit(struct cpuGb* cpu);
int UT_opcode_SCF(struct cpuGb* cpu);
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

#endif