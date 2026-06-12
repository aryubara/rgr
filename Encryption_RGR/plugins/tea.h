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