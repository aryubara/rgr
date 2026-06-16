#pragma once 

#include <vector> 
#include <cstdint> 
#include <string>

//ИНИЦИАЛИЗАЦИЯ КОНСТАНТ 
const int m_xor = 256; // мощность алфавита (побайтовая обработка)

//ИНИЦИАЛИЗАЦИЯ ФУНКЦИЙ 
std::vector<uint8_t> xorEncryptDecrypt(const std::vector<uint8_t>& data, const std::string& key); 

//ИНТЕРФЕЙС ДИНАМИЧЕСКОЙ БИБЛИОТЕКИ 
extern "C" { 
    const char* getName(); 
    void encrypt(const std::vector<uint8_t>& inputData, std::vector<uint8_t>& outputData, const char* key); 
    void decrypt(const std::vector<uint8_t>& inputData, std::vector<uint8_t>& outputData, const char* key); 
}