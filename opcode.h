#ifndef OPCODE_H
#define OPCODE_H
#include "emul.h"

void opcode_LD_X_Y(struct cpuGb * cpu, uint8_t a);
void opcode_LD_R_N(struct cpuGb * cpu, uint8_t a);

#endif