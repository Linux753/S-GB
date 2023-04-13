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
    return ((a&0x000F) + (b&0x000F) > 0x000F)? 1:0;
}

void opcode_ADD16bit(struct cpuGb* cpu, uint16_t a, uint16_t b, uint16_t *res){
    uint8_t h = add_lowOverflow16bit(cpu, a, b);

    uint8_t c = __builtin_add_overflow(a, b, res);

    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->h, h);
    writeBits(cpu->flags, cpu->n, 0);
}

void opcode_ADDdd(struct cpuGb* cpu, uint16_t a, int8_t dd, uint16_t *res){
    uint8_t h = (((a&0x000F) + (dd&0x0F)) != ((a+dd)&0x000F))? 1:0;

    uint8_t c = __builtin_add_overflow(a, dd, res);

    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->h, h);
    writeBits(cpu->flags, cpu->z, 0);
    writeBits(cpu->flags, cpu->n, 0);
}

uint8_t opcode_CB_getP(struct cpuGb* cpu, uint8_t ** p){
    uint8_t regP = readNext(cpu);
    switch(regP&0x0F){
        case 0x06:
        case 0x0E:
            *p = &(cpu->mem[cpu->reg16[HL]]);
            break;
        case 0x07:
        case 0x0F:
            *p = &(cpu->reg[rnA]);
            break;
        default:
            *p = &(cpu->reg[(regP<0x07)? regP+2: regP-6]);
            break;
    }

    return regP;
}

void opcode_rlc(struct cpuGb* cpu, uint8_t * p){
    struct Ext8bit hiBit = {.mask = 0b10000000, .dec=7};
    struct Ext8bit lowBit ={.mask = 0b00000001, .dec = 0};
    uint8_t c = extractBits(p, hiBit);
    
    *p = (*p)<<1;
    
    writeBits(p, lowBit, c);
    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->z, (*p == 0 ? 1:0));
    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}

void opcode_rl(struct cpuGb* cpu, uint8_t * p){
    struct Ext8bit hiBit = {.mask = 0b10000000, .dec=7};
    struct Ext8bit lowBit ={.mask = 0b00000001, .dec = 0};
    uint8_t oldC = extractBits(cpu->flags, cpu->c);
    uint8_t c = extractBits(p, hiBit); 
    
    *p = (*p)<<1;

    writeBits(p, lowBit, oldC);
    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->z, (*p == 0 ? 1:0));
    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}

void opcode_rrc(struct cpuGb* cpu, uint8_t * p){
    struct Ext8bit hiBit = {.mask = 0b10000000, .dec=7};
    struct Ext8bit lowBit ={.mask = 0b00000001, .dec = 0};
    uint8_t c = extractBits(p, lowBit);
    
    *p = (*p)>>1;
    
    writeBits(p, hiBit, c);
    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->z, (*p == 0 ? 1:0));
    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}

void opcode_rr(struct cpuGb* cpu, uint8_t * p){
    struct Ext8bit hiBit = {.mask = 0b10000000, .dec=7};
    struct Ext8bit lowBit ={.mask = 0b00000001, .dec = 0};
    uint8_t oldC = extractBits(cpu->flags, cpu->c);
    uint8_t c = extractBits(p, lowBit); 
    
    *p = (*p)>>1;

    writeBits(p, hiBit, oldC);
    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->z, (*p == 0 ? 1:0));
    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}

void opcode_sla(struct cpuGb* cpu, uint8_t *p){
    struct Ext8bit hiBit = {.mask = 0b10000000, .dec=7};
    uint8_t c = extractBits(p, hiBit);

    *p = (*p)<<1;

    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->z, (*p == 0 ? 1:0));
    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}

void opcode_swap(struct cpuGb* cpu, uint8_t *p){
    struct Ext8bit hiBit = {.mask = 0b10000000, .dec=7};
    struct Ext8bit lowBit ={.mask = 0b00000001, .dec = 0};

    uint8_t hiN = extractBits(p, hiBit);
    uint8_t lowN = extractBits(p, lowBit);
    writeBits(p, hiBit, lowN);
    writeBits(p, lowBit, hiN);

    writeBits(cpu->flags, cpu->c, 0);
    writeBits(cpu->flags, cpu->z, (*p == 0 ? 1:0));
    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}

void opcode_sra(struct cpuGb* cpu, uint8_t *p){
    struct Ext8bit hiBit = {.mask = 0b10000000, .dec=7};
    struct Ext8bit lowBit ={.mask = 0b00000001, .dec = 0};
    uint8_t b7 = extractBits(p, hiBit);
    uint8_t c = extractBits(p, lowBit);
    
    *p = (*p)>>1;
    writeBits(p, hiBit, b7);

    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->z, (*p == 0 ? 1:0));
    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}

void opcode_srl(struct cpuGb* cpu, uint8_t *p){
    struct Ext8bit hiBit = {.mask = 0b10000000, .dec=7};
    struct Ext8bit lowBit ={.mask = 0b00000001, .dec = 0};
    uint8_t c = extractBits(p, lowBit);
    
    *p = (*p)>>1;
    writeBits(p, hiBit, 0);

    writeBits(cpu->flags, cpu->c, c);
    writeBits(cpu->flags, cpu->z, (*p == 0 ? 1:0));
    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}
