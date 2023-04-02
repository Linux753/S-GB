#include <stdlib.h>
#include "emul.h"

void initCPU(struct cpuGb * cpu){
    memset(cpu->mem, 0, sizeof(cpu->mem));
    memset(cpu->reg, 0, sizeof(cpu->reg));

    cpu->reg16 = (uint16_t *) cpu->reg;

    cpu->sp = &(cpu->mem[STACK_POINTER]);
    cpu->pc = &(cpu->mem[PROGRAM_COUNTER]);

    cpu->flags = &(cpu->mem[FLAGS]);
    cpu->z = (struct Ext8bit) {.mask=255, .dec=7};
    cpu->n = (struct Ext8bit) {.mask=0b01000000, .dec=6};
    cpu->h = (struct Ext8bit) {.mask=0b00100000, .dec=5};
    cpu->c = (struct Ext8bit) {.mask=0b00010000, .dec=4};
}

uint8_t readNext(struct cpuGb* cpu){
    return cpu->mem[(*(cpu->pc))++];
}

void writeToAdd(struct cpuGb* cpu, uint16_t add, uint8_t value){
    cpu->mem[add] = value;
}

uint8_t readFromAdd(struct cpuGb* cpu, uint16_t add){
    return cpu->mem[add];
}