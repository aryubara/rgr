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

void generateRandomKey(uint8_t* key, int size) { // генерация случайного ключа
    srand(time(nullptr));
    for (int i = 0; i < size; i++) {
        key[i] = rand() % 256;
    }
}

void printKey(const uint8_t* key, int size, const string& name) { // вывод ключа в hex
    cout << name << ": ";
    for (int i = 0; i < size; i++) {
        printf("%02X", key[i]);
    }
    cout << endl;
}

void saveKeyToFile(const uint8_t* key, int size, const string& filename, const string& algo) { // сохранение ключа в файл
    vector<uint8_t> data;
    string header = algo + " KEY: ";
    for (char c : header) data.push_back(c);
    
    char hex[3];
    for (int i = 0; i < size; i++) {
        sprintf(hex, "%02X", key[i]);
        data.push_back(hex[0]);
        data.push_back(hex[1]);
    }
    data.push_back('\n');
    
    writeFile(filename, data);
}

bool loadKeyFromFile(uint8_t* key, int size, const string& filename) { // загрузка ключа из файла
    try {
        vector<uint8_t> data = readFile(filename);
        string content(data.begin(), data.end());
        
        size_t pos = content.find(": ");
        if (pos != string::npos) {
            string hexStr = content.substr(pos + 2);
            for (int i = 0; i < size && i * 2 < (int)hexStr.length(); i++) {
                sscanf(hexStr.c_str() + i*2, "%2hhx", &key[i]);
            }
            return true;
        }
    } catch (...) {
        return false;
    }
    return false;
}

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