#ifndef CPU_H
#define CPU_H

#include "emul.h"

void writeToAdd(struct cpuGb* cpu, uint16_t add, uint8_t value);
uint8_t readNext(struct cpuGb* cpu);
uint8_t readFromAdd(struct cpuGb* cpu, uint16_t add);

#endif 