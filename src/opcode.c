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
    cpu->reg[rnA] = readFromAdd(cpu, cpu->reg16[BC]);
}

void opcode_LD_A_DE(struct cpuGb* cpu, uint8_t a){ //1 byte / 0b00011010 / 2 cycle 
    cpu->reg[rnA] = readFromAdd(cpu, cpu->reg16[DE]);
}

void opcode_LD_BC_A(struct cpuGb* cpu, uint8_t a){ //1 byte / 0b00000010 / 2 cycle
    writeToAdd(cpu, cpu->reg16[BC], cpu->reg[rnA]);
}

void opcode_LD_DE_A(struct cpuGb * cpu, uint8_t a){ // 1 byte / 0b00010010 / 2 cycle
    writeToAdd(cpu, cpu->reg16[DE], cpu->reg[rnA]);
}

void opcode_LD_A_NN(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b11111010 / 4 cycles
    uint8_t LL = readNext(cpu);
    uint8_t HH = readNext(cpu);

    uint16_t nn = combineByte(LL, HH);

    cpu->reg[rnA] = readFromAdd(cpu, nn);
}

void opcode_LD_nn_A(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b11101010 / 4 cycles
    uint8_t LL = readNext(cpu);
    uint8_t HH = readNext(cpu);

    uint16_t nn = combineByte(LL, HH);

    writeToAdd(cpu, nn, cpu->reg[rnA]);
}

void opcode_LDH_A_C(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b11110010 / 2 cycles
    cpu->reg[rnA] = readFromAdd(cpu,  combineByte(cpu->reg[rnC], 0xFF));
}

void opcode_LDH_C_A(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b11100010 / 2 cycle
    writeToAdd(cpu, combineByte(cpu->reg[rnC], 0xFF), cpu->reg[rnA]);
}

void opcode_LDH_A_n(struct cpuGb* cpu, uint8_t a){ // 2 byte / 0b11110000 / 3 cycle
    cpu->reg[rnA] = readFromAdd(cpu, combineByte(readNext(cpu), 0xFF));
}

void opcode_LDH_n_A(struct cpuGb* cpu, uint8_t a){ // 2 byte / 0b11100000 / 3 cycle
    writeToAdd(cpu, combineByte(readNext(cpu), 0xFF), cpu->reg[rnA]);
}

void opcode_LD_A_HLm(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b00111010 / 2 cycle
    cpu->reg[rnA] = readFromAdd(cpu, cpu->reg16[HL]--);
}

void opcode_LD_HLm_A(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b00110010 / 2 cycle
    writeToAdd(cpu, cpu->reg16[HL]--, cpu->reg[rnA]);
}

void opcode_LD_A_HLp(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b00101010 / 2 cycle
    cpu->reg[rnA] = readFromAdd(cpu, cpu->reg16[HL]++);
}

void opcode_LD_HLp_A(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b00100010 / 2 cycle
    writeToAdd(cpu, cpu->reg16[HL]++, cpu->reg[rnA]);
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
    
    opcode_ADD8bit(cpu, cpu->reg[rnA], cpu->reg[x], &(cpu->reg[rnA]));
}

void opcode_ADD_HL(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10000110 / 2 cycle
    opcode_ADD8bit(cpu, cpu->reg[rnA], readFromAdd(cpu, cpu->reg16[HL]), &(cpu->reg[rnA]));
}

void opcode_ADD_n(struct cpuGb* cpu, uint8_t a){ // 1 (?2) byte / 0b11000110 / 2 cycle 
    uint8_t n = readNext(cpu);
    opcode_ADD8bit(cpu, cpu->reg[rnA], n, &(cpu->reg[rnA]));
}

void opcode_ADC_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10001xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    opcode_ADC8bit(cpu, cpu->reg[rnA], cpu->reg[x], &(cpu->reg[rnA]));
}

void opcode_ADC_HL(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10001110 / 2 cycle
    opcode_ADC8bit(cpu, cpu->reg[rnA], readFromAdd(cpu, cpu->reg16[HL]), &(cpu->reg[rnA]));
}

void opcode_ADC_n(struct cpuGb* cpu, uint8_t a){ // 1 (?2) byte / 0b11001110 / 2 cycle 
    uint8_t n = readNext(cpu);
    opcode_ADC8bit(cpu, cpu->reg[rnA], n, &(cpu->reg[rnA]));
}

void opcode_SUB_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10010xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_SUB8bit(cpu, cpu->reg[rnA], cpu->reg[x], &(cpu->reg[rnA]));
}

void opcode_SUB_HL(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10010110 / 2 cycle
    opcode_SUB8bit(cpu, cpu->reg[rnA], readFromAdd(cpu, cpu->reg16[HL]), &(cpu->reg[rnA]));
}

void opcode_SUB_n(struct cpuGb* cpu, uint8_t a){ // 1 (?2) byte / 0b11010110 / 2 cycle 
    uint8_t n = readNext(cpu);
    opcode_SUB8bit(cpu, cpu->reg[rnA], n, &(cpu->reg[rnA]));
}

void opcode_SBC_X(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10011xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_SBC8bit(cpu, cpu->reg[rnA], cpu->reg[x], &(cpu->reg[rnA]));
}

void opcode_SBC_HL(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10011110 / 2 cycle
    opcode_SBC8bit(cpu, cpu->reg[rnA], readFromAdd(cpu, cpu->reg16[HL]), &(cpu->reg[rnA]));
}

void opcode_SBC_n(struct cpuGb* cpu, uint8_t a){ // 1 (?2) byte / 0b11011110 / 2 cycle 
    uint8_t n = readNext(cpu);
    opcode_SUB8bit(cpu, cpu->reg[rnA], n, &(cpu->reg[rnA]));
}

//Maybe one day change this to a proper fonction of compare in cpu.c
void opcode_CP_X(struct cpuGb* cpu, uint8_t a){// 1 byte / 0b10111xxx / 1 cycle
    uint8_t tempA = cpu->reg[rnA];
    opcode_SUB_X(cpu, a);
    cpu->reg[rnA] = tempA;
}

void opcode_CP_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10111110 / 2 cycle
    uint8_t tempA = cpu->reg[rnA];
    opcode_SUB_HL(cpu, a);
    cpu->reg[rnA] = tempA;
}

void opcode_CP_n(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11111110 / 2 cycle
    uint8_t tempA = cpu->reg[rnA];
    opcode_SUB_n(cpu, a);
    cpu->reg[rnA] = tempA; 
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

void opcode_DEC_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b00110101 / 3 cycle
    uint8_t add = cpu->reg16[HL];
    uint8_t res;

    opcode_DEC8bit(cpu, readFromAdd(cpu, add), &res);
    writeToAdd(cpu, add, res);
}

void opcode_AND_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10100xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_AND8bit(cpu, cpu->reg[rnA], cpu->reg[x], &(cpu->reg[rnA]));
}

void opcode_AND_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10100110 / 2 cycle
    uint8_t val = readFromAdd(cpu, cpu->reg16[HL]);

    opcode_AND8bit(cpu, cpu->reg[rnA], val, &(cpu->reg[rnA]));
}

void opcode_AND_n(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11100110 / 2 cycle
    uint8_t n = readNext(cpu);

    opcode_AND8bit(cpu, cpu->reg[rnA], n, &(cpu->reg[rnA]));
}

void opcode_OR_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10110xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_OR8bit(cpu, cpu->reg[rnA], cpu->reg[x], &(cpu->reg[rnA]));
}

void opcocde_OR_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10110110 / 2 cycle
    uint8_t val = readFromAdd(cpu, cpu->reg16[HL]);

    opcode_OR8bit(cpu, cpu->reg[rnA], val, &(cpu->reg[rnA]));
}

void opcode_OR_n(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11110110 / 2 cycle
    uint8_t n = readNext(cpu);

    opcode_OR8bit(cpu, cpu->reg[rnA], n, &(cpu->reg[rnA]));
}

void opcode_XOR_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10101xxx / 1 cycle
    uint8_t x = (0b00000111&a);
    
    opcode_XOR8bit(cpu, cpu->reg[rnA], cpu->reg[x], &(cpu->reg[rnA]));
}

void opcocde_XOR_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10101110 / 2 cycle
    uint8_t val = readFromAdd(cpu, cpu->reg16[HL]);

    opcode_XOR8bit(cpu, cpu->reg[rnA], val, &(cpu->reg[rnA]));
}

void opcode_XOR_n(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11101110 / 2 cycle
    uint8_t n = readNext(cpu);

    opcode_XOR8bit(cpu, cpu->reg[rnA], n, &(cpu->reg[rnA]));
}

void opcode_DAA(struct cpuGb* cpu, uint8_t a){
    if((cpu->reg[rnA]&0x0F) > 0x09 || extractBits(cpu->flags, cpu->h)) cpu->reg[rnA] += 0x06;
    if((cpu->reg[rnA]&0xF0) > 0x90 || extractBits(cpu->flags, cpu->c)) cpu->reg[rnA] += 0x60;

    writeFlag(cpu, cpu->reg[rnA] == 0, -1, 0, (extractBits(cpu->flags, cpu->n) == 0)&&(cpu->reg[rnA]>0x99)? 1:0);
}

void opcode_CPL(struct cpuGb* cpu, uint8_t a){// 1 byte / 0b00101111 / 1 cycle
    cpu->reg[rnA] = ~(cpu->reg[rnA]);

    writeBits(cpu->flags, cpu->n, 1);
    writeBits(cpu->flags, cpu->h, 1);
}

void opcode_ADD_HL_XX(struct cpuGb* cpu, uint8_t a){ //1 byte / 0bxxxx1001 / ? cycle
    uint8_t x = ((0xF0&a)>>4) + 1; //This is the way the 16 bit register is stored don't ask why

    opcode_ADD16bit(cpu, cpu->reg16[HL], cpu->reg16[x], &(cpu->reg16[HL]));
}

void opcode_INC_XX(struct cpuGb* cpu, uint8_t a){ // / 0xX3
    uint8_t x = ((0xF0&a)>>4) + 1;
    cpu->reg16[x] = cpu->reg16[x] + 1;
}

void opcode_DEC_XX(struct cpuGb* cpu, uint8_t a){// / 
    uint8_t x = ((0xF0&a)>>4) + 1;
    cpu->reg16[x] = cpu->reg16[x] - 1;
}

void opcode_add_SD_dd(struct cpuGb* cpu, uint8_t a){//2 byte / 0xE8 dd / 4 cycle
    int8_t dd = (int8_t) readNext(cpu);
    opcode_ADDdd(cpu, cpu->reg16[SP], dd, &(cpu->reg16[SP]));
}

void opcode_ld_HL_SPdd(struct cpuGb* cpu, uint8_t a){//2 byte / 0xF8 dd / 3 cycle
    int8_t dd = (int8_t) readNext(cpu);
    opcode_ADDdd(cpu, cpu->reg16[SP], dd, &(cpu->reg16[HL]));
}

void opcode_rlca(struct cpuGb* cpu, uint8_t a){//1 byte / 0x07 / 1 cycle
    opcode_rlc(cpu, &(cpu->reg[rnA]));
    writeBits(cpu->flags, cpu->z, 0); //For the rotate opcode specialized on the A register the Z flag is set to zero and not dependant
}

void opcode_rla(struct cpuGb* cpu, uint8_t a){
    opcode_rl(cpu, &(cpu->reg[rnA]));
    writeBits(cpu->flags, cpu->z, 0); 
}

void opcode_rrca(struct cpuGb* cpu, uint8_t a){
    opcode_rrc(cpu, &(cpu->reg[rnA]));
    writeBits(cpu->flags, cpu->z, 0); 
}

void opcode_rra(struct cpuGb* cpu, uint8_t a){
    opcode_rr(cpu, &(cpu->reg[rnA]));
    writeBits(cpu->flags, cpu->z, 0); 
}

void opcode_CBprefix(struct cpuGb* cpu, uint8_t a){
    uint8_t * p;
    uint8_t regP = opcode_CB_getP(cpu, &p);
    uint8_t n = opcode_CB_getN(cpu, regP);
    switch(regP&0xF8){
        case 0x00:
            opcode_rlc(cpu, p);
            break;
        case 0x08:
            opcode_rrc(cpu,p);
            break;
        case 0x10:
            opcode_rl(cpu, p);
            break;
        case 0x18:
            opcode_rr(cpu, p);
            break;
        case 0x20:
            opcode_sla(cpu, p);
            break;
        case 0x28:
            opcode_sra(cpu, p);
            break;
        case 0x30:
            opcode_swap(cpu, p);
            break;
        case 0x38:
            opcode_srl(cpu, p);
            break;
        default:
            switch(regP/0x40){
                case 1:
                    opcode_bit(cpu, n, p);
                    break;
                case 2:
                    opcode_res(cpu, n, p);
                    break;
                case 3:
                    opcode_set(cpu, n, p);
                    break;
            }
            break;
    }
}

void opcode_jp_NNNN(struct cpuGb* cpu, uint8_t a){//3 byte / C3 nn nn / 4
    uint16_t add = readNext16U(cpu);
    opcode_jp(cpu, add);
}

void opcode_jp_HL(struct cpuGb* cpu, uint8_t a){// 1 byte / E9 / 1 cycle
    opcode_jp(cpu, cpu->reg16[HL]);
}

void opcode_jp_C_NNNN(struct cpuGb* cpu, uint8_t a){
    uint16_t add = readNext16U(cpu);
    if(extractBits(cpu->flags, cpu->c)) opcode_jp(cpu, add);
}

void opcode_jp_NC_NNNN(struct cpuGb* cpu, uint8_t a){
    uint16_t add = readNext16U(cpu);
    if(!extractBits(cpu->flags, cpu->c)) opcode_jp(cpu, add);
}

void opcode_jp_Z_NNNN(struct cpuGb* cpu, uint8_t a){
    uint16_t add = readNext16U(cpu);
    if(extractBits(cpu->flags, cpu->z)) opcode_jp(cpu, add);
}

void opcode_jp_NZ_NNNN(struct cpuGb* cpu, uint8_t a){
    uint16_t add = readNext16U(cpu);
    if(!extractBits(cpu->flags, cpu->z)) opcode_jp(cpu, add);
}

void opcode_jr(struct cpuGb* cpu, uint8_t a){
    int8_t dd = (int8_t) readNext(cpu);
    *cpu->pc = *cpu->pc + dd;
}

void opcode_jr_C(struct cpuGb* cpu, uint8_t a){
    int8_t dd = (int8_t) readNext(cpu);
    if(extractBits(cpu->flags, cpu->c)) *cpu->pc = *cpu->pc + dd;
}

void opcode_jr_NC(struct cpuGb* cpu, uint8_t a){
    int8_t dd = (int8_t) readNext(cpu);
    if(!extractBits(cpu->flags, cpu->c)) *cpu->pc = *cpu->pc + dd;
}

void opcode_jr_Z(struct cpuGb* cpu, uint8_t a){
    int8_t dd = (int8_t) readNext(cpu);
    if(extractBits(cpu->flags, cpu->z)) *cpu->pc = *cpu->pc + dd;
}

void opcode_jr_NZ(struct cpuGb* cpu, uint8_t a){
    int8_t dd = (int8_t) readNext(cpu);
    if(!extractBits(cpu->flags, cpu->z)) *cpu->pc = *cpu->pc + dd;
}

void opcode_call_NNNN(struct cpuGb* cpu, uint8_t a){
    uint16_t add = readNext16U(cpu);
    opcode_call(cpu, add);
}

void opcode_call_C(struct cpuGb* cpu, uint8_t a){
    uint16_t add = readNext16U(cpu);
    if(extractBits(cpu->flags, cpu->c)) opcode_call(cpu, add);
}

void opcode_call_NC(struct cpuGb* cpu, uint8_t a){
    uint16_t add = readNext16U(cpu);
    if(!extractBits(cpu->flags, cpu->c)) opcode_call(cpu, add);
}

void opcode_call_Z(struct cpuGb* cpu, uint8_t a){
    uint16_t add = readNext16U(cpu);
    if(extractBits(cpu->flags, cpu->z)) opcode_call(cpu, add);
}

void opcode_call_NZ(struct cpuGb* cpu, uint8_t a){
    uint16_t add = readNext16U(cpu);
    if(!extractBits(cpu->flags, cpu->z)) opcode_call(cpu, add);
}

void opcode_ret_C(struct cpuGb* cpu, uint8_t a){
    if(extractBits(cpu->flags, cpu->c)) opcode_ret(cpu);
}

void opcode_ret_NC(struct cpuGb* cpu, uint8_t a){
    if(!extractBits(cpu->flags, cpu->c)) opcode_ret(cpu);
}

void opcode_ret_Z(struct cpuGb* cpu, uint8_t a){
    if(extractBits(cpu->flags, cpu->z)) opcode_ret(cpu);
}

void opcode_ret_NZ(struct cpuGb* cpu, uint8_t a){
    if(!extractBits(cpu->flags, cpu->z)) opcode_ret(cpu);
}

void opcode_di(struct cpuGb* cpu, uint8_t a){ // 1 byte / F3 / 1 cycle
    cpu->IME = 0;
}

void opcode_ei(struct cpuGb* cpu, uint8_t a){// 1 byte / FB / 1 cycle
    cpu->IME = 1;
}

void opcode_ccf(struct cpuGb* cpu, uint8_t a){// 1 byte / 3F / 1 cycle
    uint8_t c = (extractBits(cpu->flags, cpu->c)^1);
    writeFlag(cpu, -1, 0, 0, c);
}

void opcode_scf(struct cpuGb* cpu, uint8_t a){ // 1 byte/ 3F / 1 cycle
    writeFlag(cpu, -1, 0, 0, 1);
}

void opcode_nop(struct cpuGb* cpu, uint8_t a){ // 1 byte / 00 / 1 cycle

}

void opcode_reti(struct cpuGb* cpu, uint8_t a){ // 1 byte / D9 / 1 cycle
    cpu->IME = 1;
    opcode_ret(cpu);
}

void opcode_rst(struct cpuGb* cpu, uint8_t a){ // 1 byte / xx / 4 cycle
    opcode_call(cpu, rst_add(cpu, a));
}

//For the moment not implementing the "halt bug"
void opcode_halt(struct cpuGb* cpu, uint8_t a){// 1 byte
    if(!ISR(cpu)) (*cpu->pc)--;
}

void opcode_stop(struct cpuGb* cpu, uint8_t a){// 2 byte // 10 XX (the XX part is ignored)
    fprintf(stderr, "STOP instructioàn used, not commmon\n");
    struct Ext8bit ext = {.dec = 4, .mask = 0b00010000};
    uint8_t hInter = ((*cpu->IE)&0x1F)&((*cpu->IF)&0x1F);
    if(!extractBits(&hInter, ext)) (*cpu->pc)--;
    else (*cpu->pc)++;
}
