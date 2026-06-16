#pragma once

#include <vector>
#include <cstdint>

//ИНИЦИАЛИЗАЦИЯ КОНСТАНТ
const int m = 256; //мощность алфавита

//ИНИЦИАЛИЗАЦИЯ ФУНКЦИЙ
std::vector<uint8_t> affineEncrypt(const std::vector<uint8_t>& data, uint32_t a, uint32_t b);
std::vector<uint8_t> affineDecrypt(const std::vector<uint8_t>& data, uint32_t a, uint32_t b);

//ИНТЕРФЕЙС ДИНАМИЧЕСКОЙ БИБИЛИОТЕКИ
extern "C" {
const char* getName();
void encrypt(const std::vector<uint8_t>& inputData, std::vector<uint8_t>& outputData, uint32_t a, uint32_t b);
void decrypt(const std::vector<uint8_t>& inputData, std::vector<uint8_t>& outputData, uint32_t a, uint32_t b);
}