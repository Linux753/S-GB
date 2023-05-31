#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdint.h>
#include "emul.h"

#define BOOT_ROM_DISABLE 0xFF50

//To get register from the opcode for opcode from 0x00 to 0x3F
#define getReg8bit1(cpu, a) ((((a)&0XF0) == 0x30)? rnA : ((a)&0x08)? (((a)&0xF0)>>4)*2 + 3 : (((a)&0xF0)>>4)*2 + 2)
#define getReg16bit1(cpu, a) (((0xF0&(a))>>4) + 1)
#define getRegX_LD_X_Y(cpu, a) (((a)&0xF8)==0x78? rnA : ((((a)&0xF8)-0x40)/(0x08) + 2))
#define getRegY_LD_X_Y(cpu, a) (((a)&0x07)==0x07? rnA : ((a)&0x07) + 2)

void writeToAdd(struct cpuGb* cpu, uint16_t add, uint8_t value);
uint8_t readNext(struct cpuGb* cpu);
uint16_t readNext16U(struct cpuGb* cpu);
uint8_t readFromAdd(struct cpuGb* cpu, uint16_t add);
void initRegister(struct cpuGb* cpu);
void initCPU(struct cpuGb * cpu);
void writeFlag(struct cpuGb* cpu, int8_t z, int8_t n, int8_t h, int8_t c);

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
void opcode_ret(struct cpuGb* cpu, uint8_t a);

uint8_t rst_add(struct cpuGb* cpu, uint8_t a);
bool ISR(struct cpuGb* cpu);

bool execute(struct cpuGb * cpu);

#endif 
