#ifndef LOADROM_H
#define LOADROM_H

#include <stdint.h>
#include "emul.h"

#define MIN_ROM_SIZE 0x8000

//These addresses definition is not exhaustive
#define ROM_HEADER_TITLE 0x134
#define ROM_HEADER_CGB_FLAG 0x143
#define ROM_HEADER_NEW_LICENSE_CODE 0x144
#define ROM_HEADER_CARTRIDGE_TYPE 0x147
#define ROM_HEADER_ROM_SIZE 0x148
#define ROM_HEADER_RAM_SIZE 0x149
#define ROM_HEADER_OLD_LICENSEE_CODE 0x14B

enum console{
    DMG, MGB, SGB, CGB, AGB
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

void printHeaderInfo(struct MetadataROM * data);


#endif