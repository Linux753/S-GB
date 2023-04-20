#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdint.h>
#include "emul.h"

void writeToAdd(struct cpuGb* cpu, uint16_t add, uint8_t value);
uint8_t readNext(struct cpuGb* cpu);
uint16_t readNext16U(struct cpuGb* cpu);
uint8_t readFromAdd(struct cpuGb* cpu, uint16_t add);
void initRegister(struct cpuGb* cpu);
void initCPU(struct cpuGb * cpu);

void opcode_ADC8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_ADD8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_SUB8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_SBC8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t * res);
void opcode_INC8bit(struct cpuGb* cpu, uint8_t a, uint8_t * res);
void opcode_DEC8bit(struct cpuGb* cpu, uint8_t a, uint8_t* res);
void opcode_AND8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_OR8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_XOR8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res);
void opcode_ADD16bit(struct cpuGb* cpu, uint16_t a, uint16_t b, uint16_t *res);
void opcode_ADDdd(struct cpuGb* cpu, uint16_t a, int8_t dd, uint16_t *res);


uint8_t opcode_CB_getP(struct cpuGb* cpu, uint8_t ** p);
uint8_t opcode_CB_getN(struct cpuGb* cpu, uint8_t regP);
void opcode_rlc(struct cpuGb* cpu, uint8_t * p);
void opcode_rl(struct cpuGb* cpu, uint8_t * p);
void opcode_rrc(struct cpuGb* cpu, uint8_t * p);
void opcode_rr(struct cpuGb* cpu, uint8_t * p);
void opcode_sla(struct cpuGb* cpu, uint8_t *p);
void opcode_swap(struct cpuGb* cpu, uint8_t *p);
void opcode_sra(struct cpuGb* cpu, uint8_t *p);
void opcode_srl(struct cpuGb* cpu, uint8_t *p);
void opcode_bit(struct cpuGb* cpu, uint8_t n, uint8_t *p);
void opcode_set(struct cpuGb* cpu, uint8_t n, uint8_t *p);
void opcode_res(struct cpuGb* cpu, uint8_t n, uint8_t *p);

void opcode_jp(struct cpuGb* cpu, uint16_t add);
void opcode_call(struct cpuGb* cpu, uint16_t add);
void opcode_ret(struct cpuGb* cpu);
#endif 
