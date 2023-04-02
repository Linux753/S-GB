#include <stdlib.h>
#include "emul.h"
#include "cpu.h"

/*I choose to use function to access memory (but not register), 
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
