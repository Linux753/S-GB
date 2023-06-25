#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>
#include <portaudio.h>
#include "utils.h"

//////////MACRO DEFINITION//////////
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

//////////STRUCTURE DEFINITION/////////
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


//////////FUNCTION DECLARATION/////////

#endif