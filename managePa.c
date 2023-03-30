#include <stdint.h>
#include <portaudio.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "managePa.h"
#include "emul.h"


int16_t randomInt16(){
    int rng= rand();
    if(rng % 0x10000 == 0) return rng%INT16_MAX;
    else return -rng%INT16_MAX;
}

void generateWaveTriangle(struct Sound * sound){
    float a = (((float) (INT16_MAX -INT16_MIN))/((float)sound->phaseMax)) * 2;
    for(int i=0; i<sound->phaseMax/2; i++){ //Ascending phase of the wave
       sound->wave[i] = a * i + INT16_MIN;
    }

    for(int i= sound->phaseMax/2; i<sound->phaseMax; i++){
        sound->wave[i] = -a *(i-(sound->phaseMax/2)) + INT16_MAX;
    }
}

void generateWaveSawtooth(struct Sound * sound){
    float a = (((float) (INT16_MAX -INT16_MIN))/((float)sound->phaseMax));
    for(int i=0; i<sound->phaseMax; i++){
        sound->wave[i] = a*i + INT16_MIN;
    }
}

void generateWaveSquare(struct Sound * sound){
    for(int i=0; i<sound->phaseMax/2; i++){
        sound->wave[i] = INT16_MAX;
    }
    for(int i=sound->phaseMax/2; i<sound->phaseMax; i++){
        sound->wave[i] = INT16_MIN;
    }
}

void generateWaveNoise(struct Sound * sound){
    for(int i=0; i<sound->phaseMax; i++){
        sound->wave[i] = randomInt16();
    }
}

int generateWave(struct Sound * sound, int frequency){
    int ret = EXIT_FAILURE;
    sound->phaseMax = sound->streamInfo->sampleRate/frequency;
    sound->phase = 0;

    if(sound->wave != NULL){ //Shouldn't be usefull
        free(sound->wave);
        sound->wave = NULL;
        fprintf(stderr, "Unusual freeing\n");
    }
    sound->wave = malloc(sizeof(*(sound->wave)) * sound->phaseMax);
    if(sound->wave == NULL) goto generateWaveEnd;

    switch(sound->type){
        case 0x00: //Triangle wave
            generateWaveTriangle(sound);
            break;
        case 0x01: //Sawtooth wave
            generateWaveSawtooth(sound);
            break;
        case 0x02: //Square wave
            generateWaveSquare(sound);
            break;
        case 0x03: //Random wave
            generateWaveNoise(sound);
            break;
    }


    ret = EXIT_SUCCESS;
generateWaveEnd:
    return ret;
}

void playTone(struct Sound *sound, int frequency, uint16_t ms ,bool advSound /* Set True for 0D opcode only*/){
    PaError paError = paNoError;
    if(Pa_IsStreamStopped(sound->stream)==0) stopSoundStream(sound); //We stop stream before making manipulation on it

    if(!advSound){
        sound->type = 0x00;
    }

    if(generateWave(sound, frequency)!= EXIT_SUCCESS) goto playToneError;
    //Finalyzing to set the time indices constant
    sound->endI= ms*0.001f * sound->streamInfo->sampleRate;
    sound->releaseBegI = sound->streamInfo->sampleRate * sound->releaseBegTime + sound->endI;

    //Resetting the playing indices
    sound->i =0;

    //Activating the avanced sound control:
    sound->advSound = advSound;


    //Start playing the new tone
    paError = Pa_StartStream(sound->stream);
    if(paError != paNoError) goto playToneError;

    return;
playToneError:
    fprintf(stderr, "Error while playing tone.\n");
    //Additionnal info 
    if(paError != paNoError) fprintf(stderr, "Port Audio error : %s", Pa_GetErrorText(paError));
}

int paStreamCallback(const void *input, void *output, unsigned long frameCount, 
                    const PaStreamCallbackTimeInfo *timeInfo, 
                    PaStreamCallbackFlags statusFlags, void *userData){
    int16_t * out = (int16_t *) output;
    struct Sound *sound = (struct Sound *) userData;
    
    unsigned long i;

    if(!sound->advSound){
        for(i=0; i<frameCount; i++,sound->i++){
            if(sound->i>=sound->endI) goto paStreamCallbackEndTone;
            *(out++) = sound->wave[sound->phase];
            ++sound->phase;
            sound->phase = sound->phase%sound->phaseMax;
        }
    }
    else{
        for(i=0; i<frameCount; i++, sound->i++){
            if(sound->i>=sound->endI){ //End of tone
                goto paStreamCallbackEndTone;
            }
            else if(sound->i>=sound->releaseBegI){ //Release phase : decrease to 0
                *(out++) = sound->wave[sound->phase] * (sound->sustain + sound->slopeRelease*(sound->i - sound->releaseBegI));
            }
            else if(sound->i>=sound->decayEndI){ //Sustain phase
                *(out++) = sound->wave[sound->phase] * sound->sustain;
            }
            else if(sound->i>=sound->attackEndI){//Decay phase
                *(out++) = sound->wave[sound->phase] * (sound->volume + sound->slopeDecay * (sound->i - sound->attackEndI));
            }
            else{ //Attack phase
                *(out++) = sound->wave[sound->phase] * (sound->slopeAttack * (sound->i));
            }

            ++sound->phase;
            if(sound->phase>=sound->phaseMax) sound->phase = 0;
        }
    }
    return paContinue;
paStreamCallbackEndTone:
    for(; i<frameCount; i++){
        *(out++) = 0;
    }
    return paComplete;
}

void abortSoundStream(struct Sound * sound){
    //fprintf(stderr, "Aborting stream\n");
    //fprintf(stderr, "%p\n", sound->wave);
    Pa_AbortStream(sound->stream);
    if(sound->wave != NULL){
        //fprintf(stderr, "Classical free\n");
        free(sound->wave);
        //fprintf(stderr, "Free was okay tho\n");
        sound->wave = NULL;
    }
    //fprintf(stderr, "Now we stop everything %p\n", sound->stream);
    //fprintf(stderr, "Finished aborting\n");
}

void stopSoundStream(void *userData){
    struct Sound * sound = (struct Sound *) userData;
    //fprintf(stderr, "Stopping stream\n");
    Pa_StopStream(sound->stream);
    if(sound->wave != NULL){
        free(sound->wave);
        sound->wave = NULL;
    }
}

int initPa(struct Sound * sound, int channelCount){
    PaError error;
    int ret = EXIT_SUCCESS;
    error = Pa_Initialize();
    if(error != paNoError) goto initPaError;

    PaDeviceIndex deviceId = Pa_GetDefaultOutputDevice();
    const PaDeviceInfo * deviceInfo = Pa_GetDeviceInfo(deviceId);
    PaStreamParameters paramOut = {.device = deviceId,
    .channelCount = channelCount,
    .sampleFormat = paInt16,
    .suggestedLatency = deviceInfo->defaultLowOutputLatency,
    .hostApiSpecificStreamInfo = NULL
    };

    sound->stream = NULL;
    error = Pa_OpenStream(&(sound->stream), NULL, &paramOut, deviceInfo->defaultSampleRate, FRAMES_PER_BUFFER, paNoFlag, paStreamCallback, sound);
    if(error != paNoError) goto initPaError;

    sound->streamInfo = Pa_GetStreamInfo(sound->stream);
    //Start stream if not done in main loop

    //Pa_SetStreamFinishedCallback(sound->stream,stopSoundStream); //Does produce error log but not problematic error

initPaEnd:
    return ret;

initPaError:
    fprintf(stderr, "Error Port_Audio : %s\n", Pa_GetErrorText(error));
    ret = EXIT_FAILURE;
    goto initPaEnd;
}

void terminatePa(struct Sound * sound){
    if(sound->wave != NULL) free(sound->wave);
    if(sound->stream != NULL) Pa_CloseStream(sound->stream);
    Pa_Terminate();
}
