#include <stdio.h>
#include <stdlib.h>
#include "emul.h"
#include "loadROM.h"

int loadBytes(char* path, uint8_t * buf, size_t size){
    FILE * file = fopen(path, "rb");
    int ret = EXIT_FAILURE;
    if(file == NULL){
        fprintf(stderr, "Error loading bootROM");
        goto endLoadBytes;
    }
    /*int i = 0;
    while(!feof(file) && i<size){ //TODO : suppress the verification, not really useful
        if(fread(&(buf[i]), sizeof(uint8_t), 1, file) != 1){
            fprintf(stderr, "Error reading rom, %s\n", path);
            goto endLoadBytes;
        }
        i++;
    }*/

    if(fread(buf, sizeof(uint8_t), size, file) != size){
        fprintf(stderr, "Error reading rom, %s : not enough byte\n", path);
        goto endLoadBytes;
    }

    ret = EXIT_SUCCESS;
endLoadBytes:
    if(file!=NULL){
        fclose(file);
    }
    return ret;
}

int loadBootROM(struct cpuGb* cpu, enum console console){
    char * path;
    int ret = EXIT_SUCCESS;
    switch(console){
        case DMG:
        case MGB:
            path = "bootROMs/dmg_rom.bin";
            break;
        default:
            path = "Unsupported";
            break;
    }
    return loadBytes(path, cpu->bootROM, BOOTROM_SIZE);
}

enum MBC getMBC(uint8_t * rom){
    switch(rom[ROM_HEADER_CARTRIDGE_TYPE]){
        case 0x00:
        case 0x08:
        case 0x09:
            return ROM;
            break;
        case 0x01:
        case 0x02:
        case 0x03:
            return MBC1;
            break;
        case 0x05:
        case 0x06:
            return MBC2;
            break;
        case 0x0B:
        case 0x0C:
        case 0x0D:
            return MMM01;
            break;
        case 0x0F:
        case 0x10:
        case 0x11:
        case 0x12:
        case 0x13:
            return MBC3;
            break;
        case 0x19:
        case 0x1A:
        case 0x1B:
        case 0x1C:
        case 0x1D:
        case 0x1E:
            return MBC5;
            break;
        case 0x20:
            return MBC6;
            break;
        case 0x22:
            return MBC7;
            break;
        case 0xFE:
            return HuC3;
            break;
        case 0xFF:
            return HuC1;
            break;
        default:
            return unknown;
            break;
    }

}

//Don't take in account the odd ROM size specified by Pan Docs
unsigned int getNbROMBank(uint8_t * rom){
    return 1<<(rom[ROM_HEADER_ROM_SIZE] + 1);
}

unsigned int getNbRAMBank(uint8_t * rom){
    switch(rom[ROM_HEADER_RAM_SIZE]){
        case 0x02:
            return 1;
            break;
        case 0x03:
            return 4;
            break;
        case 0x04:
            return 16;
            break;
        case 0x05:
            return 8;
            break;
        default:
            return 0;
            break;
    }
}

void analyseHeader(struct GB * gb){
    struct MetadataROM * metadata = &(gb->metadataROM);
    
    memcpy(metadata->title, &(gb->cpu.mem[ROM_HEADER_TITLE]), 16);
    metadata->title[16] = '\0';

    metadata->mbc = getMBC(gb->cpu.mem);
    metadata->ROMBank = getNbROMBank(gb->cpu.mem);
    metadata->RAMBank = getNbRAMBank(gb->cpu.mem);

}

void printHeaderInfo(struct MetadataROM * data){
    printf("Title :");
    printf("%s\n", data->title);
    printf("MBC type : %d\n", data->mbc);
    printf("ROM Bank : %d , RAM Bank : %d\n", data->ROMBank, data->RAMBank);

}

int loadROM(struct GB * gb, char * path){
    int ret = EXIT_FAILURE;
    if(loadBytes(path, gb->cpu.mem, MIN_ROM_SIZE) != EXIT_SUCCESS){
        goto endLoadROM;
    }

    analyseHeader(gb);
    
    if(loadBootROM(&gb->cpu, DMG) != EXIT_SUCCESS){
        goto endLoadROM;
    }
    
    ret = EXIT_SUCCESS;
endLoadROM:
    return ret;
}
