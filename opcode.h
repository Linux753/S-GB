#ifndef OPCODE_H
#define OPCODE_H
#include "emul.h"

void opcode_LD_X_Y(struct cpuGb * cpu, uint8_t a);
void opcode_LD_X_N(struct cpuGb * cpu, uint8_t a);
void opcode_LD_X_HL(struct cpuGb * cpu, uint8_t a);
void opcode_LD_HL_X(struct cpuGb * cpu, uint8_t a); 
void opcode_LD_HL_N(struct cpuGb * cpu, uint8_t a);
void opcode_LD_A_BC(struct cpuGb* cpu, uint8_t a);
void opcode_LD_A_DE(struct cpuGb* cpu, uint8_t a); 
void opcode_LD_BC_A(struct cpuGb* cpu, uint8_t a);
void opcode_LD_DE_A(struct cpuGb * cpu, uint8_t a);
void opcode_LD_A_NN(struct cpuGb* cpu, uint8_t a);
void opcode_LD_nn_A(struct cpuGb* cpu, uint8_t a);
void opcode_LDH_A_C(struct cpuGb* cpu, uint8_t a);
void opcode_LDH_C_A(struct cpuGb* cpu, uint8_t a);
void opcode_LDH_A_n(struct cpuGb* cpu, uint8_t a);
void opcode_LDH_n_A(struct cpuGb* cpu, uint8_t a);
void opcode_LD_A_HLm(struct cpuGb* cpu, uint8_t a);
void opcode_LD_HLm_A(struct cpuGb* cpu, uint8_t a);
void opcode_LD_A_HLp(struct cpuGb* cpu, uint8_t a);
void opcode_LD_HLp_A(struct cpuGb* cpu, uint8_t a);
void opcode_LD_X_nn(struct cpuGb* cpu, uint8_t a);
void opcode_LD_nn_SP(struct cpuGb* cpu, uint8_t a);
void opcode_LD_SP_HL(struct cpuGb* cpu, uint8_t a);
void opcode_PUSH_X(struct cpuGb* cpu, uint8_t a);
void opcode_POP_X(struct cpuGb* cpu, uint8_t a);

#endif