#pragma once 
 
#include <vector> 
#include <cstdint> 
 
//ИНИЦИАЛИЗАЦИЯ КОНСТАНТ 
const int m_scytale = 256; // Мощность алфавита (размер байта)
 
//ИНИЦИАЛИЗАЦИЯ ФУНКЦИЙ 
std::vector<uint8_t> scytaleEncrypt(const std::vector<uint8_t>& data, uint32_t columns); 
std::vector<uint8_t> scytaleDecrypt(const std::vector<uint8_t>& data, uint32_t columns); 
 
//ИНТЕРФЕЙС ДИНАМИЧЕСКОЙ БИБЛИОТЕКИ 
extern "C" { 
    const char* getName(); 
    void encrypt(const std::vector<uint8_t>& inputData, std::vector<uint8_t>& outputData, uint32_t columns); 
    void decrypt(const std::vector<uint8_t>& inputData, std::vector<uint8_t>& outputData, uint32_t columns); 
}