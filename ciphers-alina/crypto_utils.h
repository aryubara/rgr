#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include "fileManager.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdio>
#include <filesystem>
#include <string>

using namespace std;

void bytesToWordsLittleEndian(const uint8_t* bytes, uint32_t* words, int count) { // преобразование байтов для ChaCha20 (little-endian)
    for (int i = 0; i < count; i++) {
        words[i] = bytes[i*4] | (bytes[i*4+1] << 8) | 
                   (bytes[i*4+2] << 16) | (bytes[i*4+3] << 24);
    }
}

void wordsToBytesLittleEndian(const uint32_t* words, uint8_t* bytes, int count) {
    for (int i = 0; i < count; i++) {
        bytes[i*4] = words[i] & 0xFF;
        bytes[i*4+1] = (words[i] >> 8) & 0xFF;
        bytes[i*4+2] = (words[i] >> 16) & 0xFF;
        bytes[i*4+3] = (words[i] >> 24) & 0xFF;
    }
}

void bytesToWordsBigEndian(const uint8_t* bytes, uint32_t* words, int count) { // преобразование байтов для Serpent (big-endian)
    for (int i = 0; i < count; i++) {
        words[i] = (bytes[i*4] << 24) | (bytes[i*4+1] << 16) | 
                   (bytes[i*4+2] << 8) | bytes[i*4+3];
    }
}

void wordsToBytesBigEndian(const uint32_t* words, uint8_t* bytes, int count) {
    for (int i = 0; i < count; i++) {
        bytes[i*4] = words[i] >> 24;
        bytes[i*4+1] = words[i] >> 16;
        bytes[i*4+2] = words[i] >> 8;
        bytes[i*4+3] = words[i];
    }
}

#endif