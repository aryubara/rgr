#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <cctype>
#include <limits>


using namespace std;


//ИНИЦИАЛИЗАЦИЯ ФУНКЦИЙ
vector<uint8_t> readFile(const string& path);
void writeFile(const string& path, const vector<uint8_t>& inputData);

void clearInput();
bool inputNumber(int& val, const string& prompt, int minVal, int maxVal);
bool inputColumns(uint32_t& cols);
bool inputHex(uint32_t& val, const string& prompt);
bool inputSerpentKey(uint32_t key[8]);
bool inputChaCha20Key(uint32_t key[8], uint32_t nonce[3]);
bool fileExists(const string& path);
bool validateKey(const string& key);