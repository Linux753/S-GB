#ifndef EMUL_H
#define EMUL_H


#define BOOTROM_SIZE 0x0100 

#define MEMORY_SIZE 65536
#define CURSOR0 0 //TODO
#define OPCODE_NB 256 //TODO  
#define REGISTER8_SIZE 8
#define REGISTER16_SIZE 6

#define IE_ADD 0xFFFF
#define IF_ADD 0xFF0F
#define STACK_INIT_ADD 0xFFFE //Source : Nintendo DMG-01 Gameboy console manual page 64
//CPU 16 bit Register names
#define AF 0
#define BC 1
#define DE 2
#define HL 3
#define SP 4
#define PC 5
//CPU 8 bit Register names
#define rnA 0
#define rnF 1
#define rnB 2
#define rnC 3
#define rnD 4
#define rnE 5
#define rnH 6
#define rnL 7

#define combineByte(LL, HH) (((uint16_t) (LL)) | (((uint16_t) (HH))<<8))
#define getLowByte(NN) ((uint8_t) ((NN)&0x00FF))
#define getHighByte(NN) ((uint8_t) ((NN)>>8))

//TODO : Remake the extract bit function to take in account the non perfect mask
#define extractBits(p, extractor) (((*(p))&((extractor).mask))>>(extractor.dec)) 
#define writeBits(p, extractor, value) (*(p) = (*(p) & ~(((extractor.mask)>>extractor.dec)<<extractor.dec)) | ((value)<<(extractor.dec)))




#define CONTROL1_ADD 0xFFF0
#define CONTROL2_ADD 0xFFF2
#define INPUT_NB 16





////////////////Sound register address////////////////////////
#define SOUND_ON 0xFF26
#define SOUND_PANNING 0xFF25
#define SOUND_MASTER 0xFF24

#define SOUND_CH1WAVESWEEP 0xFF10
#define SOUND_CH1DUTYLENGTH 0xFF11
#define SOUND_CH1VOL 0xFF12
#define SOUND_CH1WAVELENGTH 0xFF13
#define SOUND_CH1WAVECTRL 0xFF14

#define SOUND_CH2DUTYLENGTH 0xFF16
#define SOUND_CH2VOL 0xFF17
#define SOUND_CH2WAVELENGTH 0xFF18
#define SOUND_CH2WAVECTRL 0xFF19

#define SOUND_CH3DACENBL 0xFF1A
#define SOUND_CH3LENGTH 0xFF1B
#define SOUND_CH3VOL 0xFF1C
#define SOUND_CH3WAVELENGTH 0xFF1D
#define SOUND_CH3WAVECTRL 0xFF1E
#define SOUND_CH3WAVERAM 0xFF30
#define SOUND_CH3WAVERAMLENGTH 16

#define SOUND_CH4LENGTH 0xFF20
#define SOUND_CH4VOL 0xFF21
#define SOUND_CH4FREQ 0xFF22
#define SOUND_CH4CTRL 0xFF23



#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <portaudio.h>


//For the moment only support DMG and MGB
enum console{
    DMG, MGB, SGB, CGB, AGB
};

struct Ext8bit{
    uint8_t mask;
    uint8_t dec;
};

struct Ext16bit{
    uint16_t mask;
    uint8_t dec;
};

struct cpuGb{
    uint8_t bootROM[BOOTROM_SIZE];
    uint8_t mem[MEMORY_SIZE]; //Memory size to adjust !!!
    uint8_t reg[REGISTER16_SIZE * 2]; 
    uint16_t * reg16;


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

struct Control{
    SDL_bool key[SDL_NUM_SCANCODES];
    SDL_bool quit;
    SDL_bool resizeWindow;

    SDL_Scancode keyTable[INPUT_NB];

    uint16_t * control1;
    uint16_t * control2;
};

struct DACChannel{ //Structure used by Pa to produce sound
    bool on;
    
    //Info on the wave boocling
    int16_t * wave;
    int phase;
    int phaseMax; //Size of the wave array

    //Info on the channel playing, used to simulate the length timer
    unsigned long int i; 
    unsigned long endI;
    

    //Channel Volume
    /*float volumeLeft;
    float volumeRight;*/
};

struct Channel4{
    struct DACChannel DAC;

    /////Register/////
    //Length timer
    uint8_t * length;
    struct Ext8bit lengthTime;

    //Channel volume & envelope
    uint8_t * vol;
    struct Ext8bit volInit;
    struct Ext8bit volEnvDir;
    struct Ext8bit volSweep;

    //Frequency & randomness
    uint8_t * freq;
    struct Ext8bit freqClkShift;
    struct Ext8bit freqLFSRW;
    struct Ext8bit freqClkDiv;

    //Channel control
    uint8_t * ctrl;
    struct Ext8bit trig;
    struct Ext8bit lengthEnbl;
};

struct Channel3{
    struct DACChannel DAC;

    /////Register/////
    //DAC Enable
    uint8_t * dacPtr;
    struct Ext8bit dacEnbl;

    //Length timer
    uint8_t* length;
    struct Ext8bit lengthTime;

    //Output volume
    uint8_t * vol;
    struct Ext8bit volLevel; //Mute to max (in weird order see doc)

    //Channel wavelength : speed at wich the wave is played
    uint16_t * wavelength;
    struct Ext16bit wavelengthTot;
    uint8_t * waveCtrl;
    struct Ext8bit waveTrig;
    struct Ext8bit waveEnbl;

    //Channel waveRAM
    uint8_t * waveRAM;
    uint8_t waveRAMLength; //In byte
    struct Ext8bit lowSample;
    struct Ext8bit highSample;    
};

struct Channel2{
    struct DACChannel DAC;

    /////Register/////
    //Channel length & duty cycle
    uint8_t * dutyLength;
    struct Ext8bit duty;
    struct Ext8bit lengthTimer;

    //Channel volume & envelope
    uint8_t * vol;
    struct Ext8bit volInit;
    struct Ext8bit volEnvDir;
    struct Ext8bit volSweep;

    //Channel wavelength
    uint16_t * wavelength;
    struct Ext16bit wavelengthTot;
    uint8_t * waveCtrl;
    struct Ext8bit waveTrig;
    struct Ext8bit waveEnbl;
};

struct Channel1{
    struct DACChannel DAC;

    /////Register/////
    //Channel wave sweep
    uint8_t * waveSweep;
    struct Ext8bit waveSweepPace;
    struct Ext8bit waveSweepDec;
    struct Ext8bit waveSweepSlope; 

    //Channel length & duty cycle
    uint8_t * dutyLength;
    struct Ext8bit duty;
    struct Ext8bit lengthTimer;

    //Channel volume & envelope
    uint8_t * vol;
    struct Ext8bit volInit;
    struct Ext8bit volEnvDir;
    struct Ext8bit volSweep;

    //Channel wavelength
    uint16_t * wavelength;
    struct Ext16bit wavelengthTot;
    uint8_t * waveCtrl;
    struct Ext8bit waveTrig;
    struct Ext8bit waveEnbl;
};

struct Sound{
    PaStream * stream;
    const PaStreamInfo * streamInfo;

    //Sound channel
    struct Channel1 ch1;
    struct Channel2 ch2;
    struct Channel3 ch3;
    struct Channel4 ch4;

    ///////General sound register /////////
    //On/Off
    uint8_t * on;
    struct Ext8bit onGlobal;
    struct Ext8bit onCh4;
    struct Ext8bit onCh3;
    struct Ext8bit onCh2;
    struct Ext8bit onCh1;

    //Sound panning
    uint8_t * panning;
    struct Ext8bit ch4Left;
    struct Ext8bit ch3Left;
    struct Ext8bit ch2Left;
    struct Ext8bit ch1Left;
    struct Ext8bit ch4Right;
    struct Ext8bit ch3Right;
    struct Ext8bit ch2Right;
    struct Ext8bit ch1Right;

    //Master volume & VIN panning
    uint8_t * masterVol;
    struct Ext8bit VINLeft;
    struct Ext8bit volLeft;
    struct Ext8bit VINRight;
    struct Ext8bit volRight;
};

enum MBC{
    ROM,MBC1,MBC2,MBC3,MBC5,MBC6,MBC7,MMM01,M161,HuC1,HuC3,unknown
};

struct MetadataROM{
    char title[17]; //Max title size : 16 
    unsigned int ROMBank; //16 KiB / ROM Bank
    unsigned int RAMBank; // 8 KiB / RAM Bank
    enum MBC mbc;

    uint16_t licenseeCode;

};

struct Chip16{
    struct cpuGb cpu;
    struct Control control;

    struct MetadataROM metadataROM;

    //Rendering    
    struct PPU ppu; //Picture Processing Unit : struct managing registers relative to the rendering
    struct Screen screen; //struct managing the rendering of the emulator in itself
};


#endif

