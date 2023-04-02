#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "emul.h"


void soundSetOn(struct Sound* sound, bool on){
    if(on) *(sound->on) |= 0b10000000;
    else *(sound->on) &= 0b01111111;
}

void soundSetExtractor(struct cpuGb * cpu, struct Sound* sound){
    //Sound on/off
    sound->on = &(cpu->mem[SOUND_ON]);
    sound->onGlobal = (struct Ext8bit) {.mask = 255, .dec = 7};
    sound->onCh4 = (struct Ext8bit) {.mask = 0b00001000, .dec = 3};
    sound->onCh3 = (struct Ext8bit) {.mask = 0b00000100, .dec = 2};
    sound->onCh2 = (struct Ext8bit) {.mask = 0b00000010, .dec = 1};
    sound->onCh1 = (struct Ext8bit) {.mask = 0b00000001, .dec = 0};

    //Sound panning
    sound->panning = &(cpu->mem[SOUND_PANNING]);
    sound->ch4Left = (struct Ext8bit) {.mask = 0b10000000, .dec = 7};
    sound->ch3Left = (struct Ext8bit) {.mask = 0b01000000, .dec = 6};
    sound->ch2Left = (struct Ext8bit) {.mask = 0b00100000, .dec = 5};
    sound->ch1Left = (struct Ext8bit) {.mask = 0b00010000, .dec = 4};
    sound->ch4Right = (struct Ext8bit) {.mask = 0b00001000, .dec = 3};
    sound->ch3Right = (struct Ext8bit) {.mask = 0b00000100, .dec = 2};
    sound->ch2Right = (struct Ext8bit) {.mask = 0b00000010, .dec = 1};
    sound->ch1Right = (struct Ext8bit) {.mask = 0b00000001, .dec = 0};

    //Master Volume
    sound->masterVol = &(cpu->mem[SOUND_MASTER]);
    sound->VINLeft = (struct Ext8bit) {.mask = 255, .dec = 7};
    sound->volLeft = (struct Ext8bit) {.mask = 0b01110000, .dec = 4};
    sound->VINRight = (struct Ext8bit) {.mask = 0b00001000, .dec = 3};
    sound->volRight = (struct Ext8bit) {.mask = 0b00000111, .dec =0};
}

void soundCh1SetExtractor(struct cpuGb * cpu, struct Channel1* channel){
    //Wavelength sweep
    channel->waveSweep = &(cpu->mem[SOUND_CH1WAVESWEEP]);
    channel->waveSweepPace = (struct Ext8bit) {.mask = 255, .dec =4 };
    channel->waveSweepDec = (struct Ext8bit) {.mask = 0b00001000, .dec=3};
    channel->waveSweepSlope = (struct Ext8bit) {.mask = 0b00000111, .dec = 0};

    //Length time & duty cycle
    channel->dutyLength = &(cpu->mem[SOUND_CH1DUTYLENGTH]);
    channel->duty = (struct Ext8bit) {.mask = 255, .dec = 6};
    channel->lengthTimer = (struct Ext8bit) {.mask = 0b00111111, .dec = 0};

    //Volume & envelope
    channel->vol = &(cpu->mem[SOUND_CH1VOL]);
    channel->volInit = (struct Ext8bit) {.mask = 255, .dec = 4};
    channel->volEnvDir = (struct Ext8bit) {.mask = 0b00001000, .dec = 3};
    channel->volSweep = (struct Ext8bit) {.mask = 0b00000111, .dec = 0};

    //Wavelength
    channel->wavelength  = (uint16_t *) &(cpu->mem[SOUND_CH1WAVELENGTH]);
    channel->wavelengthTot = (struct Ext16bit) {.mask = 0b0000011111111111, .dec= 0};
    channel->waveCtrl = &(cpu->mem[SOUND_CH1WAVECTRL]);
    channel->waveTrig = (struct Ext8bit) {.mask = 255, .dec= 7};
    channel->waveEnbl = (struct Ext8bit) {.mask = 0b01111111, .dec= 6};
}

void soundCh2SetExtractor(struct cpuGb * cpu, struct Channel2* channel){
    //Length time & duty cycle
    channel->dutyLength = &(cpu->mem[SOUND_CH2DUTYLENGTH]);
    channel->duty = (struct Ext8bit) {.mask = 255, .dec = 6};
    channel->lengthTimer = (struct Ext8bit) {.mask = 0b00111111, .dec = 0};

    //Volume & envelope
    channel->vol = &(cpu->mem[SOUND_CH2VOL]);
    channel->volInit = (struct Ext8bit) {.mask = 255, .dec = 4};
    channel->volEnvDir = (struct Ext8bit) {.mask = 0b00001000, .dec = 3};
    channel->volSweep = (struct Ext8bit) {.mask = 0b00000111, .dec = 0};

    //Wavelength
    channel->wavelength  = (uint16_t *) &(cpu->mem[SOUND_CH2WAVELENGTH]);
    channel->wavelengthTot = (struct Ext16bit) {.mask = 0b0000011111111111, .dec= 0};
    channel->waveCtrl = &(cpu->mem[SOUND_CH2WAVECTRL]);
    channel->waveTrig = (struct Ext8bit) {.mask = 255, .dec= 7};
    channel->waveEnbl = (struct Ext8bit) {.mask = 0b01111111, .dec= 6};
}

void soundCh3SetExtractor(struct cpuGb * cpu, struct Channel3* channel){
    //DAC Enable
    channel->dacPtr= &(cpu->mem[SOUND_CH3DACENBL]);
    channel->dacEnbl = (struct Ext8bit) {.mask=255, .dec=7};

    //Length timer
    channel->length = &(cpu->mem[SOUND_CH3LENGTH]);
    channel->lengthTime = (struct Ext8bit) {.mask = 255, .dec = 0};

    //Output level
    channel->vol = &(cpu->mem[SOUND_CH3VOL]);
    channel->volLevel = (struct Ext8bit) {.mask = 0b01100000, .dec = 5};

    //Wavelength 
    channel->wavelength  = (uint16_t *) &(cpu->mem[SOUND_CH3WAVELENGTH]);
    channel->wavelengthTot = (struct Ext16bit) {.mask = 0b0000011111111111, .dec= 0};
    channel->waveCtrl = &(cpu->mem[SOUND_CH3WAVECTRL]);
    channel->waveTrig = (struct Ext8bit) {.mask = 255, .dec= 7};
    channel->waveEnbl = (struct Ext8bit) {.mask = 0b01111111, .dec= 6};

    //Wave RAM
    channel->waveRAM = &(cpu->mem[SOUND_CH3WAVERAM]);
    channel->waveRAMLength = SOUND_CH3WAVERAMLENGTH;
    channel->lowSample = (struct Ext8bit) {.mask = 0x0F, .dec= 0};
    channel->highSample = (struct Ext8bit) {.mask = 0xFF, .dec = 4};
}

void soundCh4SetExtractor(struct cpuGb * cpu, struct Channel4 * channel){
    //Length timer
    channel->length = &(cpu->mem[SOUND_CH4LENGTH]);
    channel->lengthTime = (struct Ext8bit) {.mask = 0b00011111, .dec= 0};
    
    //Channel volume & envelope
    channel->vol = &(cpu->mem[SOUND_CH4VOL]);
    channel->volInit = (struct Ext8bit) {.mask = 255, .dec= 4};
    channel->volEnvDir = (struct Ext8bit) {.mask = 0b00001000, .dec= 3};
    channel->volSweep = (struct Ext8bit) {.mask = 0b00000111, .dec= 0};

    //Frequency & randomness
    channel->freq = &(cpu->mem[SOUND_CH4FREQ]);
    channel->freqClkShift = (struct Ext8bit) {.mask = 255, .dec = 4};
    channel->freqLFSRW = (struct Ext8bit) {.mask = 0b00001000, .dec= 3};
    channel->freqClkDiv = (struct Ext8bit) {.mask = 0b00000111, .dec= 0};

    //Control
    channel->ctrl = &(cpu->mem[SOUND_CH4CTRL]);
    channel->trig = (struct Ext8bit) {.mask = 0xFF, .dec= 7};
    channel->lengthEnbl = (struct Ext8bit) {.mask = 0b01000000, .dec = 6};
}


