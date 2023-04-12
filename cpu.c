#include <stdlib.h>
#include "emul.h"

void initCPU(struct cpuGb * cpu){
    memset(cpu->mem, 0, sizeof(cpu->mem));
    memset(cpu->reg, 0, sizeof(cpu->reg));

    cpu->reg16 = (uint16_t *) cpu->reg;

    cpu->sp = (uint16_t *) &(cpu->reg16[SP]);
    cpu->pc = (uint16_t *) &(cpu->reg16[PC]);

    cpu->flags = &(cpu->reg[rnF]);
    cpu->z = (struct Ext8bit) {.mask=0b10000000, .dec=7};
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

uint8_t add_lowOverflow8bit(struct cpuGb* cpu, uint8_t a, uint8_t b){
    return ((a&0x0F) + (b&0x0F) > 0x0F) ? 1:0;
}

void opcode_ADD8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    writeBits(cpu->flags, cpu->h, add_lowOverflow8bit(cpu, a, b));

    uint8_t c = __builtin_add_overflow(a, b, res)? 1:0;
    writeBits(cpu->flags, cpu->c, c);
    
    writeBits(cpu->flags, cpu->n, 0);
    
    writeBits(cpu->flags, cpu->z, *res == 0? 1: 0);
}

void opcode_ADC8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    uint8_t h = add_lowOverflow8bit(cpu, a, b);

    uint8_t c = __builtin_add_overflow(a, b, res)? 1:0;

    h |= add_lowOverflow8bit(cpu, *(res), extractBits(cpu->flags, cpu->c));

    c |= __builtin_add_overflow(*(res), b, res)? 1:0;

    writeBits(cpu->flags, cpu->h, h);

    writeBits(cpu->flags, cpu->c, c);
    
    writeBits(cpu->flags, cpu->n, 0);
    
    writeBits(cpu->flags, cpu->z, *res == 0? 1: 0);
}

//The INCs opcodes don't update the C flag for some reason (is it a typo in the doc?)
void opcode_INC8bit(struct cpuGb* cpu, uint8_t a, uint8_t * res){
    uint8_t h = add_lowOverflow8bit(cpu, a, 1);

    *res = a+1;

    writeBits(cpu->flags, cpu->h, h);
    
    writeBits(cpu->flags, cpu->n, 0);

    writeBits(cpu->flags, cpu->z, *res == 0? 1:0);
}

uint8_t sub_lowOverflow8bit(struct cpuGb* cpu, uint8_t a, uint8_t b){
    return ((uint8_t)((a&0x0F) - (b&0x0F) )>(uint8_t) 0x0F) ? 1:0;
    //Seem to works as intended but not entierly sure of the behavior of the conversion need to see rules of conversion and operation type to determine precisely that it's 100% accurate
}

void opcode_SUB8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    writeBits(cpu->flags, cpu->h, sub_lowOverflow8bit(cpu, a, b));    

    uint8_t c = __builtin_sub_overflow(a, b, res)? 1:0;
    writeBits(cpu->flags, cpu->c, c);

    writeBits(cpu->flags, cpu->n, 1);
    
    writeBits(cpu->flags, cpu->z, *res == 0? 1: 0);
}

void opcode_SBC8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t * res){    
    uint8_t h = sub_lowOverflow8bit(cpu, a, b);

    uint8_t c = __builtin_sub_overflow(a, b, res)? 1:0;
    
    h|= sub_lowOverflow8bit(cpu, *res, extractBits(cpu->flags, cpu->c));

    c |= __builtin_sub_overflow(*res, extractBits(cpu->flags, cpu->c), res)? 1:0;

    writeBits(cpu->flags, cpu->h, h);

    writeBits(cpu->flags, cpu->c, c);

    writeBits(cpu->flags, cpu->n, 1);
    
    writeBits(cpu->flags, cpu->z, *res == 0? 1: 0);
}

void opcode_DEC8bit(struct cpuGb* cpu, uint8_t a, uint8_t* res){
    uint8_t h = sub_lowOverflow8bit(cpu, a, 1);

    *res = a - 1;

    writeBits(cpu->flags, cpu->h, h);
    
    writeBits(cpu->flags, cpu->n, 0);

    writeBits(cpu->flags, cpu->z, *res == 0? 1:0);
}

void opcode_AND8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    *res = a&b;

    writeBits(cpu->flags, cpu->h, 1);
    
    writeBits(cpu->flags, cpu->n, 0);

    writeBits(cpu->flags, cpu->c, 0);

    writeBits(cpu->flags, cpu->z, *res == 0 ? 1:0);
}


void opcode_OR8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    *res = a|b;

    writeBits(cpu->flags, cpu->h, 0);
    
    writeBits(cpu->flags, cpu->n, 0);

    writeBits(cpu->flags, cpu->c, 0);

    writeBits(cpu->flags, cpu->z, *res == 0 ? 1:0);
}

void opcode_XOR8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    *res = a^b;

    writeBits(cpu->flags, cpu->h, 0);
    
    writeBits(cpu->flags, cpu->n, 0);

    writeBits(cpu->flags, cpu->c, 0);

    writeBits(cpu->flags, cpu->z, *res == 0 ? 1:0);
}

uint8_t add_lowOverflow16bit(struct cpuGb* cpu, uint16_t a, uint16_t b){
    return (a&0x000F + b&0x000F > 0x000F)? 1:0;
}

void opcode_ADD16bit(struct cpuGb* cpu, uint16_t a, uint16_t b, uint16_t *res){
    uint8_t h = add_lowOverflow16bit(cpu, a, b);

    uint8_t c = __builtin_add_overflow(a, b, res);

    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->h, h);
    writeBits(cpu->flags, cpu->n, 0);
}

void opcode_ADDdd(struct cpuGb* cpu, uint16_t a, int8_t dd, uint16_t *res){
    
}