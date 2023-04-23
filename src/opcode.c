#include <stdlib.h>
#include "emul.h"
#include "cpu.h"

/*I choose to use function to access ROM/RAM/VRAM (but not register), 
because of the READ/WRITE caracteristic of the different part of the memory*/

void opcode_LD_X_Y(struct cpuGb * cpu, uint8_t a){ //1 byte / 0b01xxxyyy / 1 cycle
    uint8_t x = getRegX_LD_X_Y(cpu, a);
    uint8_t y = getRegY_LD_X_Y(cpu, a);
    cpu->reg[x] = cpu->reg[y];
}

void opcode_LD_X_N(struct cpuGb * cpu, uint8_t a){ //2 byte / 0b00xxx110 / 2 cycle
    uint8_t x = getReg8bit1(cpu, a);
    cpu->reg[x] = readNext(cpu);
}

void opcode_LD_X_HL(struct cpuGb * cpu, uint8_t a){ //1 byte / 0b01xxx110 / 2 cycle
    uint8_t x = getReg8bit1(cpu, (a-0x40));
    cpu->reg[x] = readFromAdd(cpu, cpu->reg[HL]);
}

void opcode_LD_HL_X(struct cpuGb * cpu, uint8_t a){ //1 byte / 0b01110xxx / 2 cycle 
    uint8_t x = (a==0x77)? rnA : a-0x68;
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
    uint8_t x = getReg16bit1(cpu, a);

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
    uint8_t x = getReg16bit1(cpu, a-0xC0);
    
    writeToAdd(cpu, --cpu->reg16[SP], getHighByte(cpu->reg16[x]));
    writeToAdd(cpu, --cpu->reg16[SP], getLowByte(cpu->reg16[x]));
}

void opcode_POP_X(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11xx0001 / 4 cycle
    uint8_t x = getReg16bit1(cpu, a-0xC0);

    cpu->reg[x] = combineByte(readFromAdd(cpu, cpu->reg[SP]++),
                                readFromAdd(cpu, cpu->reg[SP]++));
}

void opcode_ADD_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10000xxx / 1 cycle
    uint8_t x = getRegY_LD_X_Y(cpu, a);
    
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
    uint8_t x = getRegY_LD_X_Y(cpu, a);
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
    uint8_t x = getRegY_LD_X_Y(cpu, a);
    
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
    uint8_t x = getRegY_LD_X_Y(cpu, a);
    
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
    uint8_t x = getReg8bit1(cpu, a);

    opcode_INC8bit(cpu, cpu->reg[x], &(cpu->reg[x]));
}

void opcode_INC_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b00110100 / 3 cycle
    uint8_t add = cpu->reg16[HL];
    uint8_t res;

    opcode_INC8bit(cpu, readFromAdd(cpu, add), &res);
    writeToAdd(cpu, add, res);
}

void opcode_DEC_X(struct cpuGb* cpu, uint8_t a){// 1 byte / 0b00xxx101 / 1 cycle
    uint8_t x = getReg8bit1(cpu, a);

    opcode_DEC8bit(cpu, cpu->reg[x], &(cpu->reg[x]));
}

void opcode_DEC_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b00110101 / 3 cycle
    uint8_t add = cpu->reg16[HL];
    uint8_t res;

    opcode_DEC8bit(cpu, readFromAdd(cpu, add), &res);
    writeToAdd(cpu, add, res);
}

void opcode_AND_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10100xxx / 1 cycle
    uint8_t x = getRegY_LD_X_Y(cpu, a);
    
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
    uint8_t x = getRegY_LD_X_Y(cpu, a);
    
    opcode_OR8bit(cpu, cpu->reg[rnA], cpu->reg[x], &(cpu->reg[rnA]));
}

void opcode_OR_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10110110 / 2 cycle
    uint8_t val = readFromAdd(cpu, cpu->reg16[HL]);

    opcode_OR8bit(cpu, cpu->reg[rnA], val, &(cpu->reg[rnA]));
}

void opcode_OR_n(struct cpuGb* cpu, uint8_t a){//1 byte / 0b11110110 / 2 cycle
    uint8_t n = readNext(cpu);

    opcode_OR8bit(cpu, cpu->reg[rnA], n, &(cpu->reg[rnA]));
}

void opcode_XOR_X(struct cpuGb* cpu, uint8_t a){ // 1 byte / 0b10101xxx / 1 cycle
    uint8_t x = getRegY_LD_X_Y(cpu, a);
    
    opcode_XOR8bit(cpu, cpu->reg[rnA], cpu->reg[x], &(cpu->reg[rnA]));
}

void opcode_XOR_HL(struct cpuGb* cpu, uint8_t a){//1 byte / 0b10101110 / 2 cycle
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
    uint8_t x = getReg16bit1(cpu, a);

    opcode_ADD16bit(cpu, cpu->reg16[HL], cpu->reg16[x], &(cpu->reg16[HL]));
}

void opcode_INC_XX(struct cpuGb* cpu, uint8_t a){ // / 0xX3
    uint8_t x = getReg16bit1(cpu, a);
    cpu->reg16[x] = cpu->reg16[x] + 1;
}

void opcode_DEC_XX(struct cpuGb* cpu, uint8_t a){// / 
    uint8_t x = getReg16bit1(cpu, a);
    cpu->reg16[x] = cpu->reg16[x] - 1;
}

void opcode_add_SP_dd(struct cpuGb* cpu, uint8_t a){//2 byte / 0xE8 dd / 4 cycle
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

void init_opcodeTable(struct cpuGb* cpu){
    cpu->opTble[0x00] = opcode_nop;
    cpu->opTble[0x01] = opcode_LD_X_nn;
    cpu->opTble[0x02] = opcode_LD_BC_A;
    cpu->opTble[0x03] = opcode_INC_XX;
    cpu->opTble[0x04] = opcode_INC_X;
    cpu->opTble[0x05] = opcode_DEC_X;
    cpu->opTble[0x06] = opcode_LD_X_N;
    cpu->opTble[0x07] = opcode_rlca;
    cpu->opTble[0x08] = opcode_LD_nn_SP;
    cpu->opTble[0x09] = opcode_ADD_HL_XX;
    cpu->opTble[0x0A] = opcode_LD_A_BC;
    cpu->opTble[0x0B] = opcode_DEC_XX;
    cpu->opTble[0x0C] = opcode_INC_X;
    cpu->opTble[0x0D] = opcode_DEC_X;
    cpu->opTble[0x0E] = opcode_LD_X_N;
    cpu->opTble[0x0F] = opcode_rrca;
    cpu->opTble[0x10] = opcode_stop;
    cpu->opTble[0x11] = opcode_LD_X_nn;
    cpu->opTble[0x12] = opcode_LD_DE_A;
    cpu->opTble[0x13] = opcode_INC_XX;
    cpu->opTble[0x14] = opcode_INC_X;
    cpu->opTble[0x15] = opcode_DEC_X;
    cpu->opTble[0x16] = opcode_LD_X_N;
    cpu->opTble[0x17] = opcode_rla;
    cpu->opTble[0x18] = opcode_jr;
    cpu->opTble[0x19] = opcode_ADD_HL_XX;
    cpu->opTble[0x1A] = opcode_LD_A_DE;
    cpu->opTble[0x1B] = opcode_DEC_XX;
    cpu->opTble[0x1C] = opcode_INC_X;
    cpu->opTble[0x1D] = opcode_DEC_X;
    cpu->opTble[0x1E] = opcode_LD_X_N;
    cpu->opTble[0x1F] = opcode_rra;
    cpu->opTble[0x20] = opcode_jr_NZ;
    cpu->opTble[0x21] = opcode_LD_X_nn;
    cpu->opTble[0x22] = opcode_LD_HLp_A;
    cpu->opTble[0x23] = opcode_INC_XX;
    cpu->opTble[0x24] = opcode_INC_X;
    cpu->opTble[0x25] = opcode_DEC_X;
    cpu->opTble[0x26] = opcode_LD_X_N;
    cpu->opTble[0x27] = opcode_DAA;
    cpu->opTble[0x28] = opcode_jr_Z;
    cpu->opTble[0x29] = opcode_ADD_HL_XX;
    cpu->opTble[0x2A] = opcode_LD_A_HLp;
    cpu->opTble[0x2B] = opcode_DEC_XX;
    cpu->opTble[0x2C] = opcode_INC_X;
    cpu->opTble[0x2D] = opcode_DEC_X;
    cpu->opTble[0x2E] = opcode_LD_X_N;
    cpu->opTble[0x2F] = opcode_CPL;
    cpu->opTble[0x30] = opcode_jr_NC;
    cpu->opTble[0x31] = opcode_LD_X_nn;
    cpu->opTble[0x32] = opcode_LD_HLm_A;
    cpu->opTble[0x33] = opcode_INC_XX;
    cpu->opTble[0x34] = opcode_INC_HL;
    cpu->opTble[0x35] = opcode_DEC_HL;
    cpu->opTble[0x36] = opcode_LD_HL_N;
    cpu->opTble[0x37] = opcode_scf;
    cpu->opTble[0x38] = opcode_jr_C;
    cpu->opTble[0x39] = opcode_ADD_HL_XX;
    cpu->opTble[0x3A] = opcode_LD_A_HLm;
    cpu->opTble[0x3B] = opcode_DEC_XX;
    cpu->opTble[0x3C] = opcode_INC_X;
    cpu->opTble[0x3D] = opcode_DEC_X;
    cpu->opTble[0x3E] = opcode_LD_X_N;
    cpu->opTble[0x3F] = opcode_ccf;
    for(int i = 0x40; i<0x6F; i++){
        if(i&0x07 == 0x07) cpu->opTble[i] = opcode_LD_X_Y; 
        else cpu->opTble[i] = opcode_LD_X_HL;
    }
    cpu->opTble[0x70] = opcode_LD_HL_X;
    cpu->opTble[0x71] = opcode_LD_HL_X;
    cpu->opTble[0x72] = opcode_LD_HL_X;
    cpu->opTble[0x73] = opcode_LD_HL_X;
    cpu->opTble[0x74] = opcode_LD_HL_X;
    cpu->opTble[0x75] = opcode_LD_HL_X;
    cpu->opTble[0x76] = opcode_halt;
    cpu->opTble[0x77] = opcode_LD_HL_X;
    cpu->opTble[0x78] = opcode_LD_X_Y;
    cpu->opTble[0x79] = opcode_LD_X_Y;
    cpu->opTble[0x7A] = opcode_LD_X_Y;
    cpu->opTble[0x7B] = opcode_LD_X_Y;
    cpu->opTble[0x7C] = opcode_LD_X_Y;
    cpu->opTble[0x7D] = opcode_LD_X_Y;
    cpu->opTble[0x7E] = opcode_LD_X_HL;
    cpu->opTble[0x7F] = opcode_LD_X_Y;
    for(int i = 0x80; i<0x88; i++){
        if(i&0x07 == 0x07) cpu->opTble[i] = opcode_ADD_X; 
        else cpu->opTble[i] = opcode_ADD_HL;
    }
    for(int i = 0x88; i<0x90; i++){
        if(i&0x07 == 0x07) cpu->opTble[i] = opcode_ADC_X; 
        else cpu->opTble[i] = opcode_ADC_HL;
    }
    for(int i = 0x90; i<0x98; i++){
        if(i&0x07 == 0x07) cpu->opTble[i] = opcode_SUB_X; 
        else cpu->opTble[i] = opcode_SUB_HL;
    }
    for(int i = 0x98; i<0xA0; i++){
        if(i&0x07 == 0x07) cpu->opTble[i] = opcode_SBC_X; 
        else cpu->opTble[i] = opcode_SBC_HL;
    }
    for(int i = 0xA0; i<0xA8; i++){
        if(i&0x07 == 0x07) cpu->opTble[i] = opcode_AND_X; 
        else cpu->opTble[i] = opcode_AND_HL;
    }
    for(int i = 0xA8; i<0xB0; i++){
        if(i&0x07 == 0x07) cpu->opTble[i] = opcode_XOR_X; 
        else cpu->opTble[i] = opcode_XOR_HL;
    }
    for(int i = 0xB0; i<0xB8; i++){
        if(i&0x07 == 0x07) cpu->opTble[i] = opcode_OR_X; 
        else cpu->opTble[i] = opcode_OR_HL;
    }
    for(int i = 0xB8; i<0xC0; i++){
        if(i&0x07 == 0x07) cpu->opTble[i] = opcode_CP_X; 
        else cpu->opTble[i] = opcode_CP_HL;
    }
    cpu->opTble[0xC0] = opcode_ret_NZ;
    cpu->opTble[0xC1] = opcode_POP_X;
    cpu->opTble[0xC2] = opcode_jp_NZ_NNNN;
    cpu->opTble[0xC3] = opcode_jp;
    cpu->opTble[0xC4] = opcode_call_NZ;
    cpu->opTble[0xC5] = opcode_PUSH_X;
    cpu->opTble[0xC6] = opcode_ADD_n;
    cpu->opTble[0xC7] = opcode_rst;
    cpu->opTble[0xC8] = opcode_ret_Z;
    cpu->opTble[0xC9] = opcode_ret;
    cpu->opTble[0xCA] = opcode_jp_Z_NNNN;
    cpu->opTble[0xCB] = opcode_CBprefix;
    cpu->opTble[0xCC] = opcode_call_Z;
    cpu->opTble[0xCD] = opcode_call;
    cpu->opTble[0xCE] = opcode_ADC_n;
    cpu->opTble[0xCF] = opcode_rst;
    cpu->opTble[0xD0] = opcode_ret_NC;
    cpu->opTble[0xD1] = opcode_POP_X;
    cpu->opTble[0xD2] = opcode_jp_NC_NNNN;
    cpu->opTble[0xD3] = opcode_nop; //undefined opcode
    cpu->opTble[0xD4] = opcode_call_NC;
    cpu->opTble[0xD5] = opcode_PUSH_X;
    cpu->opTble[0xD6] = opcode_SUB_n;
    cpu->opTble[0xD7] = opcode_rst;
    cpu->opTble[0xD8] = opcode_ret_C;
    cpu->opTble[0xD9] = opcode_reti;
    cpu->opTble[0xDA] = opcode_jp_C_NNNN;
    cpu->opTble[0xDB] = opcode_nop; //undefined opcode
    cpu->opTble[0xDC] = opcode_call_C;
    cpu->opTble[0xDD] = opcode_nop; //undefined opcode
    cpu->opTble[0xDE] = opcode_SBC_n;
    cpu->opTble[0xDF] = opcode_rst;
    cpu->opTble[0xE0] = opcode_LDH_n_A;
    cpu->opTble[0xE1] = opcode_POP_X;
    cpu->opTble[0xE2] = opcode_LDH_C_A;
    cpu->opTble[0xE3] = opcode_nop; //undefined opcode
    cpu->opTble[0xE4] = opcode_nop;
    cpu->opTble[0xE5] = opcode_PUSH_X;
    cpu->opTble[0xE6] = opcode_AND_n;
    cpu->opTble[0xE7] = opcode_rst;
    cpu->opTble[0xE8] = opcode_add_SP_dd;
    cpu->opTble[0xE9] = opcode_jp_HL;
    cpu->opTble[0xEA] = opcode_LD_nn_A;
    cpu->opTble[0xEB] = opcode_nop; //undefined opcode
    cpu->opTble[0xEC] = opcode_nop; //undefined opcode
    cpu->opTble[0xED] = opcode_nop; //undefined opcode
    cpu->opTble[0xEE] = opcode_XOR_n;
    cpu->opTble[0xEF] = opcode_rst;
    cpu->opTble[0xF0] = opcode_LDH_A_n;
    cpu->opTble[0xF1] = opcode_POP_X;
    cpu->opTble[0xF2] = opcode_LDH_A_C;
    cpu->opTble[0xF3] = opcode_di;
    cpu->opTble[0xF4] = opcode_nop; //undefined opcode
    cpu->opTble[0xF5] = opcode_PUSH_X;
    cpu->opTble[0xF6] = opcode_OR_n;
    cpu->opTble[0xF7] = opcode_rst;
    cpu->opTble[0xF8] = opcode_ld_HL_SPdd;
    cpu->opTble[0xF9] = opcode_LD_SP_HL;
    cpu->opTble[0xFA] = opcode_LD_A_NN;
    cpu->opTble[0xFB] = opcode_ei;
    cpu->opTble[0xFC] = opcode_nop; //undefined opcode
    cpu->opTble[0xFD] = opcode_nop; //undefined opcode
    cpu->opTble[0xFE] = opcode_CP_n;
    cpu->opTble[0xFF] = opcode_rst;
}
/*TODO UT 
LD_X_HL
LD_HL_X
POP_X
*/
