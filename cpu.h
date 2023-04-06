#ifndef CPU_H
#define CPU_H

#include "emul.h"

void writeToAdd(struct cpuGb* cpu, uint16_t add, uint8_t value);
uint8_t readNext(struct cpuGb* cpu);
uint8_t readFromAdd(struct cpuGb* cpu, uint16_t add);

void opcode_ADC8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_ADD8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_SUB8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_SBC8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t * res);
void opcode_INC8bit(struct cpuGb* cpu, uint8_t a, uint8_t * res);
void opcode_DEC8bit(struct cpuGb* cpu, uint8_t a, uint8_t* res);
void opcode_AND8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_OR8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_XOR8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
#endif 