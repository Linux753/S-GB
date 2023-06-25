#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdint.h>
#include "utils.h"

///////////////MACRO DEFINITION////////////////
///////CPU STATE AND TIMING : 
// no suffix : flag for the cpu.state variable
// _ADD address to trigger the state
// _LEN duration of the state, given in T-state, one T-State is a tick on the DMG-CPU clock///////
#define BOOT_ROM_DISABLE 0xFF50
#define DMAOAM_TRANSFER_ADD 0xFF46
#define PPU_OAMDMA_TRANSFER 0b1
#define PPU_OAMDMA_TRANSFER_LEN 160
#define PPU_MODE_0 0b10
#define PPU_MODE_0_LEN 146
#define PPU_MODE_1 0b100
#define PPU_MODE_1_LEN 4560
#define PPU_MODE_2_3 0b1000
#define PPU_MODE_2_3_LEN 310
#define PPU_INC_LY_LEN 456
#define CPU_DIV_LEN 256
#define CPU_TAC_TIMA_EN 0b00000100
#define CPU_TAC_CLK 0x03
//////Timer//////
#define CPU_DIV_ADD 0xFF04
#define CPU_TIMA_ADD 0xFF05
#define CPU_TMA_ADD 0xFF06
#define CPU_TAC_ADD 0xFF07
////////Interrupt////////
#define IE_ADD 0xFFFF
#define IF_ADD 0xFF0F
//////CPU 16 bit Register names//////
#define AF 0
#define BC 1
#define DE 2
#define HL 3
#define SP 4
#define PC 5
//////CPU 8 bit Register names///////
#define rnA 0
#define rnF 1
#define rnB 2
#define rnC 3
#define rnD 4
#define rnE 5
#define rnH 6
#define rnL 7
////////MISCELANOUS DEFINITION////////
#define MEMORY_SIZE 65536
#define CURSOR0 0 
#define OPCODE_NB 256 
#define REGISTER8_SIZE 8
#define REGISTER16_SIZE 6
#define BOOTROM_SIZE 0x0100 
#define STACK_INIT_ADD 0xFFFE //Source : Nintendo DMG-01 Gameboy console manual page 64
//To get register from the opcode for opcode from 0x00 to 0x3F
#define getReg8bit1(cpu, a) ((((a)&0XF0) == 0x30)? rnA : ((a)&0x08)? (((a)&0xF0)>>4)*2 + 3 : (((a)&0xF0)>>4)*2 + 2)
#define getReg16bit1(cpu, a) (((0xF0&(a))>>4) + 1)
#define getRegX_LD_X_Y(cpu, a) (((a)&0xF8)==0x78? rnA : ((((a)&0xF8)-0x40)/(0x08) + 2))
#define getRegY_LD_X_Y(cpu, a) (((a)&0x07)==0x07? rnA : ((a)&0x07) + 2)

/////////STRUCTURE DEFINITION////////////
struct State{
    uint64_t OAMDMATranfer;
    uint64_t ppuMode0;
    uint64_t ppuMode1;
    uint64_t ppuMode2;
    uint64_t ppuIncLY;
    uint64_t cpuDiv;
    uint64_t cpuTAC;
};

struct cpuGb{
    uint8_t bootROM[BOOTROM_SIZE];
    uint8_t mem[MEMORY_SIZE]; //Memory size to adjust !!!
    uint8_t * workingROM; //The actual ROM where reading is done : is bootROM if boot ROM is active, else it's mem

    uint8_t reg[REGISTER16_SIZE * 2]; 
    uint16_t * reg16;

    struct State stateTime;
    uint32_t state;
    uint32_t ticks;

    uint16_t * sp;
    uint16_t * pc;

    void (*opTble[OPCODE_NB])(struct cpuGb*, uint8_t);

    //Interrupt relative variable
    uint8_t IME;
    uint8_t * IE;
    uint8_t * IF;
    uint8_t isrJTable[5];

    uint8_t * flags;
    struct Ext8bit z; //Zero flag
    struct Ext8bit n; //Substraction flag
    struct Ext8bit h; //Half Carry Flag
    struct Ext8bit c; //Carry Flag
};


//////////FUNCTION DECLARATION//////////////
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

void triggerInterrupt(struct cpuGb* cpu);

bool execute(struct cpuGb * cpu);

#endif 
