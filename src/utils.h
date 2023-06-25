#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdint.h>

struct Ext8bit{
    uint8_t mask;
    uint8_t dec;
};

struct Ext16bit{
    uint16_t mask;
    uint8_t dec;
};

#define combineByte(LL, HH) (((uint16_t) (LL)) | (((uint16_t) (HH))<<8))
#define getLowByte(NN) ((uint8_t) ((NN)&0x00FF))
#define getHighByte(NN) ((uint8_t) ((NN)>>8))

//TODO : Remake the extract bit function to take in account the non perfect mask
#define extractBits(p, extractor) (((*(p))&((extractor).mask))>>(extractor.dec)) 
#define writeBits(p, extractor, value) (*(p) = (*(p) & ~(((extractor.mask)>>extractor.dec)<<extractor.dec)) | ((value)<<(extractor.dec)))
//Extract the bit N of the pointer N, only work on 1 byte data
#define extractBitsN(p, n) ((*(p)) & (0xFF>>(7-(n))))>>(n)
#define setFlag0(p, flag) (*(p) = (*(p) & ~(flag)))
#define setFlag1(p, flag) (*(p) = (*(p) | flag))



#endif