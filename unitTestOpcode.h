#ifndef UNITTESTOPCODE_H
#define UNITTESTOPCODE_H

int UT_opcode_ADD8bit(struct cpuGb* cpu);
int UT_opcode_ADC8bit(struct cpuGb* cpu);
int UT_opcode_SUB8bit(struct cpuGb* cpu);
int UT_opcode_INC8bit(struct cpuGb* cpu);
int UT_opcode_SBC8bit(struct cpuGb* cpu);
int UT_opcode_SCF(struct cpuGb* cpu);
int UT_writeBits(struct cpuGb* cpu);
int UT_opcode_ADDdd(struct cpuGb* cpu);
#endif