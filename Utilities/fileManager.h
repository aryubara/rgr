#pragma once

#include<vector>
#include<string>
#include<cstdint>

//ИНИЦИАЛИЗАЦИЯ ФУНКЦИЙ
std::vector<uint8_t> readFile(const std::string& path);
void writeFile(const std::string& path, const std::vector<uint8_t>& inputData);