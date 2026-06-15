#pragma once

#include <vector>
#include <string>
#include <cstdint>

std::vector<uint8_t> readFile(const std::string& path);
void writeFile(const std::string& path, const std::vector<uint8_t>& inputData);
void clearInput();
bool inputNumber(int& val, const std::string& prompt, int minVal, int maxVal);
bool inputMode(int& modeChoice);
bool inputKeyMode(int& keyMode, const std::string& manualText, const std::string& generateText);
bool inputTextData(std::vector<uint8_t>& inputData);
bool inputFilePath(std::string& path);
bool inputXorKey(std::string& key);
bool inputColumns(uint32_t& cols);
bool inputHex(uint32_t& val, const std::string& prompt);
bool inputAffineKey(uint32_t& a, uint32_t& b);
bool inputTeaKey(uint32_t key[4]);
bool inputSerpentKey(uint32_t key[8]);
bool inputChaCha20Key(uint32_t key[8], uint32_t nonce[3]);
bool fileExists(const std::string& path);
bool validateKey(const std::string& key);