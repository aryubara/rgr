#pragma once

#include <vector>
#include <cstdint>

//ИНИЦИАЛИЗАЦИЯ КОНСТАНТ
const int m = 256; //мощность алфавита

//ИНИЦИАЛИЗАЦИЯ ФУНКЦИЙ
std::vector<uint8_t> affineEncrypt(const std::vector<uint8_t>& data, uint32_t a, uint32_t b);
std::vector<uint8_t> affineDecrypt(const std::vector<uint8_t>& data, uint32_t a, uint32_t b);

