#include <stdlib.h>
#include "emul.h"
#include "cpu.h"

/*I choose to use function to access ROM/RAM/VRAM (but not register), 
because of the READ/WRITE caracteristic of the different part of the memory*/

void opcode_LD_X_Y(struct cpuGb * cpu, uint8_t a){ //1 byte / 0b01xxxyyy / 1 cycle
    uint8_t x = (a&0b00111000)>>3;
    uint8_t y = (a&0b00000111);
    cpu->reg[x] = cpu->reg[y];
}

void opcode_LD_X_N(struct cpuGb * cpu, uint8_t a){ //2 byte / 0b00xxx110 / 2 cycle
    uint8_t x = (0b00111000&a)>>3;
    cpu->reg[x] = readNext(cpu);
}

void opcode_LD_X_HL(struct cpuGb * cpu, uint8_t a){ //1 byte / 0b01xxx110 / 2 cycle
    uint8_t x = (0b00111000&a)>>3;
    cpu->reg[x] = readFromAdd(cpu, cpu->reg[HL]);
}

void opcode_LD_HL_X(struct cpuGb * cpu, uint8_t a){ //1 byte / 0b01110xxx / 2 cycle 
    uint8_t x = (0b00000111&a);
    writeToAdd(cpu, cpu->reg16[HL], cpu->reg[x]);
}

void opcode_LD_HL_N(struct cpuGb * cpu, uint8_t a){ //2 byte / 0b00110110 / 3 cycle
    writeToAdd(cpu, cpu->reg16[HL], readNext(cpu));
}

void opcode_LD_A_BC(struct cpuGb* cpu, uint8_t a){ //1 byte / 0b00001010 / 2 cycle 
    cpu->reg[A] = readFromAdd(cpu, cpu->reg16[BC]);
}

void opcode_LD_A_DE(struct cpuGb* cpu, uint8_t a){ //1 byte / 0b00011010 / 2 cycle 
    cpu->reg[A] = readFromAdd(cpu, cpu->reg16[DE]);
}

void opcode_LD_BC_A(struct cpuGb* cpu, uint8_t a){ //1 byte / 0b00000010 / 2 cycle
    writeToAdd(cpu, cpu->reg16[BC], cpu->reg[A]);
}

void opcode_LD_DE_A(struct cpuGb * cpu, uint8_t a){ // 1 byte / 0b00010010 / 2 cycle
    writeToAdd(cpu, cpu->reg16[DE], cpu->reg[A]);
}

void opcode_LD_A_NN(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b11111010 / 4 cycles
    uint8_t LL = readNext(cpu);
    uint8_t HH = readNext(cpu);

    uint16_t nn = combineByte(LL, HH);

    cpu->reg[A] = readFromAdd(cpu, nn);
}

void opcode_LD_nn_A(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b11101010 / 4 cycles
    uint8_t LL = readNext(cpu);
    uint8_t HH = readNext(cpu);

    uint16_t nn = combineByte(LL, HH);

    writeToAdd(cpu, nn, cpu->reg[A]);
}

void opcode_LDH_A_C(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b11110010 / 2 cycles
    cpu->reg[A] = readFromAdd(cpu,  combineByte(cpu->reg[C], 0xFF));
}

void opcode_LDH_C_A(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b11100010 / 2 cycle
    writeToAdd(cpu, combineByte(cpu->reg[C], 0xFF), cpu->reg[A]);
}

void opcode_LDH_A_n(struct cpuGb* cpu, uint8_t a){ // 2 byte / 0b11110000 / 3 cycle
    cpu->reg[A] = readFromAdd(cpu, combineByte(readNext(cpu), 0xFF));
}

void opcode_LDH_n_A(struct cpuGb* cpu, uint8_t a){ // 2 byte / 0b11100000 / 3 cycle
    writeToAdd(cpu, combineByte(readNext(cpu), 0xFF), cpu->reg[A]);
}

void opcode_LD_A_HLm(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b00111010 / 2 cycle
    cpu->reg[A] = readFromAdd(cpu, cpu->reg16[HL]--);
}

void opcode_LD_HLm_A(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b00110010 / 2 cycle
    writeToAdd(cpu, cpu->reg16[HL]--, cpu->reg[A]);
}

void opcode_LD_A_HLp(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b00101010 / 2 cycle
    cpu->reg[A] = readFromAdd(cpu, cpu->reg16[HL]++);
}

void opcode_LD_HLp_A(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b00100010 / 2 cycle
    writeToAdd(cpu, cpu->reg16[HL]++, cpu->reg[A]);
}

//Load 16 bit register
void opcode_LD_X_nn(struct cpuGb* cpu, uint8_t a){// 3 byte / 0b00xx0001 / 3 cycle
    uint8_t LL = readNext(cpu);
    uint8_t HH = readNext(cpu);
    uint8_t x = (0b00110000&a)>>4;

    cpu->reg16[x] = combineByte(LL, HH);
}

void opcode_LD_nn_SP(struct cpuGb* cpu, uint8_t a){//3 byte / 0b00001000 / 5 cycle
    uint8_t LL = readNext(cpu);
    uint8_t HH = readNext(cpu);
    
    uint16_t add = combineByte(LL, HH);

    writeToAdd(cpu, add, getLowByte(cpu->reg16[SP]));
    writeToAdd(cpu, add+1, getHighByte(cpu->reg16[SP]));
}

void opcode_LD_SP_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0xF9 /2 cycle
    cpu->reg16[SP] = cpu->reg16[HL];
}

void opcode_PUSH_X(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11xx0101 / 4 cycle
    uint8_t x = (0b00110000&a)>>4;
    
    writeToAdd(cpu, --cpu->reg16[SP], getHighByte(cpu->reg16[x]));
    writeToAdd(cpu, --cpu->reg16[SP], getLowByte(cpu->reg16[x]));
}

void opcode_POP_X(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11xx0001 / 4 cycle
    uint8_t x = (0b00110000&a)>>4;

    cpu->reg[x] = combineByte(readFromAdd(cpu, cpu->reg[SP]++),
                                readFromAdd(cpu, cpu->reg[SP]++));
}

void opcode_ADD_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10000xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_ADD8bit(cpu, cpu->reg[A], cpu->reg[x], &(cpu->reg[A]));
}

void opcode_ADD_HL(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10000110 / 2 cycle
    opcode_ADD8bit(cpu, cpu->reg[A], readFromAdd(cpu, cpu->reg16[HL]), &(cpu->reg[A]));
}

void opcode_ADD_n(struct cpuGb* cpu, uint8_t a){ // 1 (?2) byte / 0b11000110 / 2 cycle 
    uint8_t n = readNext(cpu);
    opcode_ADD8bit(cpu, cpu->reg[A], n, &(cpu->reg[A]));
}

void opcode_ADC_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10001xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    opcode_ADC8bit(cpu, cpu->reg[A], cpu->reg[x], &(cpu->reg[A]));
}

void opcode_ADC_HL(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10001110 / 2 cycle
    opcode_ADC8bit(cpu, cpu->reg[A], readFromAdd(cpu, cpu->reg16[HL]), &(cpu->reg[A]));
}

void opcode_ADC_n(struct cpuGb* cpu, uint8_t a){ // 1 (?2) byte / 0b11001110 / 2 cycle 
    uint8_t n = readNext(cpu);
    opcode_ADC8bit(cpu, cpu->reg[A], n, &(cpu->reg[A]));
}

void opcode_SUB_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10010xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_SUB8bit(cpu, cpu->reg[A], cpu->reg[x], &(cpu->reg[A]));
}

void opcode_SUB_HL(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10010110 / 2 cycle
    opcode_SUB8bit(cpu, cpu->reg[A], readFromAdd(cpu, cpu->reg16[HL]), &(cpu->reg[A]));
}

void opcode_SUB_n(struct cpuGb* cpu, uint8_t a){ // 1 (?2) byte / 0b11010110 / 2 cycle 
    uint8_t n = readNext(cpu);
    opcode_SUB8bit(cpu, cpu->reg[A], n, &(cpu->reg[A]));
}

void opcode_SBC_X(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10011xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_SBC8bit(cpu, cpu->reg[A], cpu->reg[x], &(cpu->reg[A]));
}

void opcode_SBC_HL(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10011110 / 2 cycle
    opcode_SBC8bit(cpu, cpu->reg[A], readFromAdd(cpu, cpu->reg16[HL]), &(cpu->reg[A]));
}

void opcode_SBC_n(struct cpuGb* cpu, uint8_t a){ // 1 (?2) byte / 0b11011110 / 2 cycle 
    uint8_t n = readNext(cpu);
    opcode_SUB8bit(cpu, cpu->reg[A], n, &(cpu->reg[A]));
}

//Maybe one day change this to a proper fonction of compare in cpu.c
void opcode_CP_X(struct cpuGb* cpu, uint8_t a){// 1 byte / 0b10111xxx / 1 cycle
    uint8_t tempA = cpu->reg[A];
    opcode_SUB_X(cpu, a);
    cpu->reg[A] = tempA;
}

void opcode_CP_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10111110 / 2 cycle
    uint8_t tempA = cpu->reg[A];
    opcode_SUB_HL(cpu, a);
    cpu->reg[A] = tempA;
}

void opcode_CP_n(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11111110 / 2 cycle
    uint8_t tempA = cpu->reg[A];
    opcode_SUB_n(cpu, a);
    cpu->reg[A] = tempA; 
}

void opcode_INC_X(struct cpuGb* cpu, uint8_t a){//1 byte / 0b00xxx100 / 2 cycle
    uint8_t x = (0b00111000&a)>>3;

    opcode_INC8bit(cpu, cpu->reg[x], &(cpu->reg[x]));
}

void opcode_INC_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b00110100 / 3 cycle
    uint8_t add = cpu->reg16[HL];
    uint8_t res;

    opcode_INC8bit(cpu, readFromAdd(cpu, add), &res);
    writeToAdd(cpu, add, res);
}

void opcode_DEC_X(struct cpuGb* cpu, uint8_t a){// 1 byte / 0b00xxx101 / 1 cycle
    uint8_t x = (0b00111000&a)>>3;

    opcode_DEC8bit(cpu, cpu->reg[x], &(cpu->reg[x]));
}

void opcode_INC_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b00110101 / 3 cycle
    uint8_t add = cpu->reg16[HL];
    uint8_t res;

    opcode_DEC8bit(cpu, readFromAdd(cpu, add), &res);
    writeToAdd(cpu, add, res);
}

void opcode_AND_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10100xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_AND8bit(cpu, cpu->reg[A], cpu->reg[x], &(cpu->reg[A]));
}

void opcocde_AND_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10100110 / 2 cycle
    uint8_t val = readFromAdd(cpu, cpu->reg16[HL]);

    opcode_AND8bit(cpu, cpu->reg[A], val, &(cpu->reg[A]));
}

void opcode_AND_n(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11100110 / 2 cycle
    uint8_t n = readNext(cpu);

    opcode_AND8bit(cpu, cpu->reg[A], n, &(cpu->reg[A]));
}

void opcode_OR_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10110xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_OR8bit(cpu, cpu->reg[A], cpu->reg[x], &(cpu->reg[A]));
}

void opcocde_OR_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10110110 / 2 cycle
    uint8_t val = readFromAdd(cpu, cpu->reg16[HL]);

    opcode_OR8bit(cpu, cpu->reg[A], val, &(cpu->reg[A]));
}

void opcode_OR_n(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11110110 / 2 cycle
    uint8_t n = readNext(cpu);

    opcode_OR8bit(cpu, cpu->reg[A], n, &(cpu->reg[A]));
}

void opcode_XOR_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10101xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_XOR8bit(cpu, cpu->reg[A], cpu->reg[x], &(cpu->reg[A]));
}

void opcocde_XOR_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10101110 / 2 cycle
    uint8_t val = readFromAdd(cpu, cpu->reg16[HL]);

    opcode_XOR8bit(cpu, cpu->reg[A], val, &(cpu->reg[A]));
}

void opcode_XOR_n(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11101110 / 2 cycle
    uint8_t n = readNext(cpu);

    opcode_XOR8bit(cpu, cpu->reg[A], n, &(cpu->reg[A]));
}

void opcode_CCF(struct cpuGb* cpu, uint8_t a){// 1 byte / 0b00111111 / 1 cycle
    uint8_t c = extractBits(cpu->flags, cpu->c);
    writeBits(cpu->flags, cpu->c, ~c);

    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}

void opcode_SCF(struct cpuGb* cpu, uint8_t a){// 1 byte / 0b00110111 / 1 cycle
    writeBits(cpu->flags, cpu->c, 1);
    writeBits(cpu->flags, cpu->n, 0);
    writeBits(cpu->flags, cpu->h, 0);
}

void opcode_CPL(struct cpuGb* cpu, uint8_t a){// 1 byte / 0b00101111 / 1 cycle
    cpu->reg[A] = ~(cpu->reg[A]);

    writeBits(cpu->flags, cpu->n, 1);
    writeBits(cpu->flags, cpu->h, 1);
}


