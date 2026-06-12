#pragma once

#include <vector>
#include <cstdint>

//ИНИЦИАЛИЗАЦИЯ КОНСТАНТ 
constexpr uint32_t DELTA = 0x9E3779B9; //Дельта
constexpr uint32_t ROUNDS = 32; //Количество раундов
const uint32_t BLOCK_SIZE = 8; //Размер блока


//ИНИЦИАЛИЗЦИЯ ФУНКЦИЙ
std::vector<uint8_t> teaEncrypt(const std::vector<uint8_t>& data, const uint32_t key[4]);
std::vector<uint8_t> teaDecrypt(const std::vector<uint8_t>& data, const uint32_t key[4]);


//ИНТЕРФЕЙС ДИНАМИЧЕСКОЙ БИБЛИОТЕКИ
extern "C" {

const char* getName();

void encrypt(const std::vector<uint8_t>& inputData, std::vector<uint8_t>& outputData, const uint32_t key[4]);
void decrypt(const std::vector<uint8_t>& inputData, std::vector<uint8_t>& outputData, const uint32_t key[4]);
}