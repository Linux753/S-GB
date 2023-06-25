#include <stdlib.h>
#include "emul.h"
#include "cpu.h"
#include "opcode.h"
#include "ppu.h"

struct Ext8bit hiBit = {.mask = 0b10000000, .dec=7};
struct Ext8bit lowBit ={.mask = 0b00000001, .dec = 0};

void initRegister(struct cpuGb* cpu){
    memset(cpu->reg, 0, sizeof(cpu->reg));
    cpu->reg16[PC] = STACK_INIT_ADD;
}

void initState(struct State * state){
    state->OAMDMATranfer = 0;
    state->ppuMode0 = 0;
    state->ppuMode1 = 0;
    state->ppuMode2 = 0;
    state->ppuIncLY = PPU_INC_LY_LEN;
    state->cpuDiv = CPU_DIV_LEN;
    state->cpuTAC = 1024;
}

void initCPU(struct cpuGb * cpu){
    memset(cpu->mem, 0, sizeof(cpu->mem));
    memset(cpu->reg, 0, sizeof(cpu->reg));
    memset(cpu->bootROM, 0, sizeof(BOOTROM_SIZE));
    cpu->workingROM = cpu->bootROM;

    cpu->reg16 = (uint16_t *) cpu->reg;

    initState(&cpu->stateTime);
    cpu->ticks = 0;

    cpu->sp = (uint16_t *) &(cpu->reg16[SP]);
    cpu->pc = (uint16_t *) &(cpu->reg16[PC]);

    init_opcodeTable(cpu);

    cpu->IME = 0;
    cpu->IE = &(cpu->mem[IE_ADD]);
    cpu->IF = &(cpu->mem[IF_ADD]);
    memcpy(cpu->isrJTable, (uint8_t []) {0x40, 0x48, 0x50, 0x58, 0x60}, sizeof(cpu->isrJTable));

    cpu->flags = &(cpu->reg[rnF]);
    cpu->z = (struct Ext8bit) {.mask=0b10000000, .dec=7};
    cpu->n = (struct Ext8bit) {.mask=0b01000000, .dec=6};
    cpu->h = (struct Ext8bit) {.mask=0b00100000, .dec=5};
    cpu->c = (struct Ext8bit) {.mask=0b00010000, .dec=4};
}

uint8_t readNext(struct cpuGb* cpu){
    return readFromAdd(cpu, (*(cpu->pc))++);
}

uint16_t readNext16U(struct cpuGb* cpu){
    uint16_t add = readNext(cpu);
    add |= ((uint16_t) readNext(cpu))<<8;
    return add;
}

void writeToAdd(struct cpuGb* cpu, uint16_t add, uint8_t value){
    
    cpu->workingROM[add] = value;

    //Checking if it trigger special event
    switch(add){
        case DMAOAM_TRANSFER_ADD:
            DMAOAMTransfer(cpu);
            break;
        case CPU_DIV_ADD:
            cpu->mem[CPU_DIV_ADD] = 0;
            break;

    }
}

uint8_t readFromAdd(struct cpuGb* cpu, uint16_t add){
    struct State * state = &(cpu->stateTime);
    
    /*//Checking the cpu states
    if(state->OAMDMATranferBegin != 0 //To avoid trigger of the state at the begining
    && cpu->ticks-state->OAMDMATranferBegin<state->OAMDMATransferLen){
        if(!(add>0xFF80 && add<0xFFFE)){ //Can only access HRAM
            return 0xFF;
        }
    }*/
    
    return cpu->workingROM[add];
}

uint8_t add_lowOverflow8bit(struct cpuGb* cpu, uint8_t a, uint8_t b){
    return ((a&0x0F) + (b&0x0F) > 0x0F) ? 1:0;
}

//Write the flags to theflag register, -1 for unmodified flag 
void writeFlag(struct cpuGb* cpu, int8_t z, int8_t n, int8_t h, int8_t c){ 
    if(h>=0) writeBits(cpu->flags, cpu->h, h);

    if(c>=0) writeBits(cpu->flags, cpu->c, c);
    
    if(n>=0) writeBits(cpu->flags, cpu->n, n);
    
    if(z>=0) writeBits(cpu->flags, cpu->z, z);
}

/*void getRegLD_X_Y(struct cpuGb* cpu, uint8_t a, uint8_t * x, uint8_t * y){
    switch(a&0xF8){
        case 0x40:
            *x = rnB;
            break;
        case 0x48:
            *x = rnC;
            break;
        case 0x50:
            *x = rnD;
            break;
        case 0x58:
    }
}*/

void opcode_ADD8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    uint8_t h = add_lowOverflow8bit(cpu, a, b);

    uint8_t c = __builtin_add_overflow(a, b, res)? 1:0;

    writeFlag(cpu, *res == 0, 0, h, c);
}

void opcode_ADC8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    uint8_t h = add_lowOverflow8bit(cpu, a, b);

    uint8_t c = __builtin_add_overflow(a, b, res)? 1:0;

    h |= add_lowOverflow8bit(cpu, *(res), extractBits(cpu->flags, cpu->c));

    c |= __builtin_add_overflow(*(res), b, res)? 1:0;

    writeFlag(cpu, *res == 0, 0, h, c);
}

//The INCs opcodes don't update the C flag for some reason
void opcode_INC8bit(struct cpuGb* cpu, uint8_t a, uint8_t * res){
    uint8_t h = add_lowOverflow8bit(cpu, a, 1);

    *res = a+1;

    writeFlag(cpu, *res == 0, 0, h, -1);
}

uint8_t sub_lowOverflow8bit(struct cpuGb* cpu, uint8_t a, uint8_t b){
    return ((uint8_t)((a&0x0F) - (b&0x0F) )>(uint8_t) 0x0F) ? 1:0;
}

void opcode_SUB8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    uint8_t h = sub_lowOverflow8bit(cpu, a, b);

    uint8_t c = __builtin_sub_overflow(a, b, res)? 1:0;

    writeFlag(cpu, *res == 0, 1, h, c);
}

void opcode_SBC8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t * res){    
    uint8_t h = sub_lowOverflow8bit(cpu, a, b);

    uint8_t c = __builtin_sub_overflow(a, b, res)? 1:0;
    
    h|= sub_lowOverflow8bit(cpu, *res, extractBits(cpu->flags, cpu->c));

    c |= __builtin_sub_overflow(*res, extractBits(cpu->flags, cpu->c), res)? 1:0;

    writeFlag(cpu, *res == 0, 1, h, c);
}

void opcode_DEC8bit(struct cpuGb* cpu, uint8_t a, uint8_t* res){
    uint8_t h = sub_lowOverflow8bit(cpu, a, 1);

    *res = a - 1;

    writeFlag(cpu, *res==0, 1, h, -1);
}

void opcode_AND8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    *res = a&b;

    writeFlag(cpu, *res == 0, 0, 1, 0);
}


void opcode_OR8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    *res = a|b;

    writeFlag(cpu, *res == 0, 0,0,0);
}

void opcode_XOR8bit(struct cpuGb* cpu, uint8_t a, uint8_t b, uint8_t *res){
    *res = a^b;

    writeFlag(cpu, *res == 0, 0, 0, 0);
}

uint8_t add_lowOverflow16bit(struct cpuGb* cpu, uint16_t a, uint16_t b){
    return ((a&0x000F) + (b&0x000F) > 0x000F)? 1:0;
}

void opcode_ADD16bit(struct cpuGb* cpu, uint16_t a, uint16_t b, uint16_t *res){
    uint8_t h = add_lowOverflow16bit(cpu, a, b);

    uint8_t c = __builtin_add_overflow(a, b, res);

    writeFlag(cpu, -1, 0, h, c);
}

void opcode_ADDdd(struct cpuGb* cpu, uint16_t a, int8_t dd, uint16_t *res){
    uint8_t h = (((a&0x000F) + (dd&0x0F)) != ((a+dd)&0x000F))? 1:0;

    uint8_t c = __builtin_add_overflow(a, dd, res);

    writeFlag(cpu, 0, 0, h, c);
}

uint8_t opcode_CB_getP(struct cpuGb* cpu, uint8_t ** p){
    uint8_t regP = readNext(cpu);
    switch(regP&0x0F){
        case 0x06:
        case 0x0E:
            *p = &(cpu->workingROM[cpu->reg16[HL]]); //TODO : Check if this is ok to use workingROM and no readFromAddress()
            break;
        case 0x07:
        case 0x0F:
            *p = &(cpu->reg[rnA]);
            break;
        default:
            *p = &(cpu->reg[((regP&0x0F)<0x07)? (regP&0x0F)+2: (regP&0x0F)-6]);
            break;
    }

    return regP;
}

uint8_t opcode_CB_getN(struct cpuGb* cpu, uint8_t regP){
    regP %= 0x40;
    regP /= 8;
}

void opcode_rlc(struct cpuGb* cpu, uint8_t * p){
    uint8_t c = extractBits(p, hiBit);
    
    *p = (*p)<<1;
    
    writeBits(p, lowBit, c);
    writeFlag(cpu, *p==0, 0, 0, c);
}

void opcode_rl(struct cpuGb* cpu, uint8_t * p){
    uint8_t oldC = extractBits(cpu->flags, cpu->c);
    uint8_t c = extractBits(p, hiBit); 
    
    *p = (*p)<<1;

    writeBits(p, lowBit, oldC);
    writeFlag(cpu, *p==0, 0, 0, c);
}

void opcode_rrc(struct cpuGb* cpu, uint8_t * p){
    uint8_t c = extractBits(p, lowBit);
    
    *p = (*p)>>1;
    
    writeBits(p, hiBit, c);
    writeFlag(cpu, *p == 0, 0, 0, c);
}

void opcode_rr(struct cpuGb* cpu, uint8_t * p){
    uint8_t oldC = extractBits(cpu->flags, cpu->c);
    uint8_t c = extractBits(p, lowBit); 
    
    *p = (*p)>>1;

    writeBits(p, hiBit, oldC);
    writeFlag(cpu, *p == 0, 0, 0, c);
}

void opcode_sla(struct cpuGb* cpu, uint8_t *p){
    uint8_t c = extractBits(p, hiBit);

    *p = (*p)<<1;

    writeFlag(cpu, *p==0, 0, 0, c);
}

void opcode_swap(struct cpuGb* cpu, uint8_t *p){

    uint8_t hiN = extractBits(p, hiBit);
    uint8_t lowN = extractBits(p, lowBit);
    writeBits(p, hiBit, lowN);
    writeBits(p, lowBit, hiN);

    writeFlag(cpu, *p==0, 0, 0, 0);
}

void opcode_sra(struct cpuGb* cpu, uint8_t *p){
    uint8_t b7 = extractBits(p, hiBit);
    uint8_t c = extractBits(p, lowBit);
    
    *p = (*p)>>1;
    writeBits(p, hiBit, b7);

    writeFlag(cpu, *p==0, 0, 0, c);
}

void opcode_srl(struct cpuGb* cpu, uint8_t *p){
    uint8_t c = extractBits(p, lowBit);
    
    *p = (*p)>>1;
    writeBits(p, hiBit, 0);

    writeFlag(cpu, *p==0, 0, 0, c);
}

void opcode_bit(struct cpuGb* cpu, uint8_t n, uint8_t *p){
    struct Ext8bit ext = (struct Ext8bit) {.mask = 0x01<<n, .dec = n};
    writeFlag(cpu, extractBits(p, ext), 0, 1, -1);
}

void opcode_set(struct cpuGb* cpu, uint8_t n, uint8_t *p){
    struct Ext8bit ext = (struct Ext8bit) {.mask = 0x01<<n, .dec = n};
    writeBits(p, ext, 1);
}

void opcode_res(struct cpuGb* cpu, uint8_t n, uint8_t *p){
    struct Ext8bit ext = (struct Ext8bit) {.mask = 0x01<<n, .dec = n};
    writeBits(p, ext, 0);
}

void opcode_jp(struct cpuGb* cpu, uint16_t add){
    *cpu->pc = add;
}

//According to doc sp decrease before storing addresses so sp can be 1 location past end of available RAM
void opcode_call(struct cpuGb* cpu, uint16_t add){
    (*cpu->sp)--;
    writeToAdd(cpu, *cpu->sp, ((*cpu->pc)&0xFF00)>>8);
    (*cpu->sp)--;
    writeToAdd(cpu, *cpu->sp, (*cpu->pc)&0x00FF);
    *cpu->pc = add;
}

//The argument a is unused, just here for usage in the opcode table
void opcode_ret(struct cpuGb* cpu, uint8_t a){
    *cpu->pc = ((uint16_t) readFromAdd(cpu, *cpu->sp));
    (*cpu->sp)++;
    *cpu->pc |= ((uint16_t) readFromAdd(cpu, *cpu->sp))<<8;
    (*cpu->sp)++;
}

uint8_t rst_add(struct cpuGb* cpu, uint8_t a){
    switch(a){
        case 0xC7:
            return 0x00;
            break;
        case 0xD7:
            return 0x10;
            break;
        case 0xE7:
            return 0x20;
            break;
        case 0xF7:
            return 0x30;
            break;
        case 0xCF:
            return 0x08;
            break;
        case 0xDF:
            return 0x18;
            break;
        case 0xEF:
            return 0x28;
            break;
        case 0xFF:
            return 0x38;
            break;
        default:
            fprintf(stderr, "Error: bad RST opcode\n");
            break;
    }
}

//For the moment don't take on account the precise timing of interrut handling
bool ISR(struct cpuGb* cpu){
    uint8_t hInter = ((*cpu->IE)&0x1F)&((*cpu->IF)&0x1F);
    if(hInter){
        if(cpu->IME){
            struct Ext8bit ext = {.dec = 0, .mask=1};
            int i =0;
            //Taking account of the interrupt priorities
            while(!extractBits(&hInter, ext)){ 
                i++;
                ext.dec++;
                ext.mask<<1;
            }

            writeBits(cpu->IF, ext, 0);
            cpu->IME = 0;
            opcode_call(cpu, cpu->isrJTable[i]);
        }
        return true;
    }
    else{
        return false;
    }
}

uint64_t CPU_getTACLen(struct cpuGb* cpu){
    switch(cpu->mem[CPU_TAC_ADD]&CPU_TAC_CLK){
        case 0:
            return 1024;
            break;
        case 1:
            return 16;
            break;
        case 2:
            return 64;
            break;
        case 3:
            return 256;
            break;
    }
}

void CPU_incTIMA(struct cpuGb* cpu){
    cpu->stateTime.cpuTAC += CPU_getTACLen(cpu);

    if(cpu->mem[CPU_TIMA_ADD] == 0xFF){
        cpu->mem[CPU_TIMA_ADD] = cpu->mem[CPU_TMA_ADD];
        interruptRequest(cpu);
        return;
    }
    cpu->mem[CPU_TIMA_ADD] += 1;
}

void CPU_incDIV(struct cpuGb* cpu){
    cpu->mem[CPU_DIV_ADD] +=1;
    cpu->stateTime.cpuDiv += CPU_DIV_LEN;
}

//Untrigger event based on timing (untriggering some state can trigger other)
//Make the appropriated function call
//Update the cpu's state and stateTime variable
void manageTiming(struct GB * gb){
    struct cpuGb* cpu = &gb->cpu;
    struct PPU * ppu = &gb->ppu;
    struct State * stateTime = &cpu->stateTime;
    struct Screen * screen = &gb->screen;

    if(stateTime->OAMDMATranfer-cpu->ticks<=0
    && cpu->state&PPU_OAMDMA_TRANSFER){
        setFlag0(&cpu->state, PPU_OAMDMA_TRANSFER);
    }

    if(stateTime->ppuIncLY-cpu->ticks<=0){
        PPU_IncLY(cpu, ppu);
    }

    if(stateTime->ppuMode2 == 0 
    || (stateTime->ppuMode2-cpu->ticks<=0
    && cpu->state&PPU_MODE_2_3)){
        setFlag0(&cpu->state, PPU_MODE_2_3);
        PPUMode0(cpu, ppu);
    }

    if(stateTime->ppuMode1-cpu->ticks<=0
    && cpu->state&PPU_MODE_1){
        setFlag0(&cpu->state, PPU_MODE_1);
        PPUMode2_3(cpu, ppu, screen);
        stateTime->ppuMode2 = stateTime->ppuMode1+PPU_MODE_1_LEN;
    }

    if(stateTime->ppuMode0-cpu->ticks<=0
    && cpu->state&PPU_MODE_0){
        setFlag0(&cpu->state, PPU_MODE_0);
        PPUMode2_3(cpu, ppu, screen);
        stateTime->ppuMode2 = stateTime->ppuMode0+PPU_MODE_0_LEN;  
    }

    //CPU Timer register:
    if(stateTime->cpuDiv-cpu->ticks<=0){
        CPU_incDIV(cpu);
    }

    if(cpu->mem[CPU_TAC_ADD]&CPU_TAC_TIMA_EN
    &&stateTime->cpuTAC-cpu->ticks<=0){
        CPU_incTIMA(cpu);
    }
    //TODO : Check timing is it tick-stateTime or the opposite?
}

bool execute(struct cpuGb * cpu){
    cpu->workingROM = (cpu->mem[BOOT_ROM_DISABLE]!=0)? cpu->mem: cpu->bootROM;
    
    if(cpu->mem[BOOT_ROM_DISABLE] != 0) return true;

    static uint8_t opcode;
    opcode = cpu->workingROM[(*cpu->pc)++];

    cpu->opTble[opcode](cpu, opcode);
    return false;
}
//TODO The workingROM is only for the ROM so the 32 first KiB of the mem variable further it's always on the mem variable 