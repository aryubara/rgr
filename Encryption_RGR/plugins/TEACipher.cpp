#include "../include/ICipherAPI.h"
#include "tea.h" // Подключаем заголовочный файл Полины
#include <vector>
#include <cstring>
#include <algorithm>

using namespace std;

// ========== ОРИГИНАЛЬНАЯ МАТЕМАТИКА ПОЛИНЫ ==========

void encryptBlock(uint32_t& v0, uint32_t& v1, const uint32_t key[4]) {
uint32_t sum = 0;
for (uint32_t i = 0; i < ROUNDS; i++) {
sum += DELTA;
v0 += ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
v1 += ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
}
}

void decryptBlock(uint32_t& v0, uint32_t& v1, const uint32_t key[4]) {
uint32_t sum = DELTA * ROUNDS;
for (uint32_t i = 0; i < ROUNDS; i++) {
v1 -= ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]);
v0 -= ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]);
sum -= DELTA;
}
}

vector<uint8_t> addPadding(vector<uint8_t> inputData) {
uint8_t padding = BLOCK_SIZE - (inputData.size() % BLOCK_SIZE);
if (padding == 0) {
padding = BLOCK_SIZE;
}
for (uint8_t i = 0; i < padding; i++) {
inputData.push_back(padding);
}
return inputData;
}

void removePadding(vector<uint8_t>& inputData) {
if (inputData.empty()) return;
uint8_t padding = inputData.back();
if (padding == 0 || padding > BLOCK_SIZE) return;
for (uint8_t i = 0; i < padding; i++) {
if (inputData[inputData.size() - 1 - i] != padding) return;
}
inputData.resize(inputData.size() - padding);
}

vector<uint8_t> teaEncrypt(const vector<uint8_t>& inputData, const uint32_t key[4]) {
vector<uint8_t> result = addPadding(inputData);
for (size_t i = 0; i < result.size(); i += BLOCK_SIZE) {
uint32_t block[2] = {};
for (uint32_t part = 0; part < 2; part++) {
for (uint32_t byte = 0; byte < 4; byte++) {
block[part] |= static_cast<uint32_t>(result[i + part * 4 + byte] << (byte * 8));
}
}
encryptBlock(block[0], block[1], key);
for (uint32_t part = 0; part < 2; part++) {
for (uint32_t byte = 0; byte < 4; byte++) {
result[i + part * 4 + byte] = static_cast<uint8_t>(block[part] >> (byte * 8));
}
}
}
return result;
}

vector<uint8_t> teaDecrypt(const vector<uint8_t>& cipherData, const uint32_t key[4]) {
vector<uint8_t> result = cipherData;
for (size_t i = 0; i < result.size(); i += BLOCK_SIZE) {
uint32_t block[2] = {};
for (uint32_t part = 0; part < 2; part++) {
for (uint32_t byte = 0; byte < 4; byte++) {
block[part] |= static_cast<uint32_t>(result[i + part * 4 + byte]) << (byte * 8);
}
}
decryptBlock(block[0], block[1], key);
for (uint32_t part = 0; part < 2; part++) {
for (uint32_t byte = 0; byte < 4; byte++) {
result[i + part * 4 + byte] = static_cast<uint8_t>(block[part] >> (byte * 8));
}
}
}
removePadding(result);
return result;
}

// ========== ЕДИНЫЙ ИНТЕРФЕЙС ПЛАГИНОВ (EXTERN "C") ==========

extern "C" {

bool validate_key(const char* key) {
return key && strlen(key) > 0;
}

void encrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
if (len <= 0) return;

vector<uint8_t> plaintext(input, input + len);

// Преобразуем твой строковый ключ в массив из 4-х 32-битных чисел для Полины
uint32_t tea_key[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
size_t key_len = strlen(key);
if (key_len > 0) {
uint8_t key_bytes[16] = {0};
for (int i = 0; i < 16; ++i) {
key_bytes[i] = (unsigned char)key[i % key_len] ^ i;
}
memcpy(tea_key, key_bytes, 16);
}

// Вызываем оригинальную функцию шифрования Полины
vector<uint8_t> ciphertext = teaEncrypt(plaintext, tea_key);

memcpy(output, ciphertext.data(), ciphertext.size());
}

void decrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
if (len <= 0) return;

vector<uint8_t> ciphertext(input, input + len);

uint32_t tea_key[4] = {0x11111111, 0x22222222, 0x33333333, 0x44444444};
size_t key_len = strlen(key);
if (key_len > 0) {
uint8_t key_bytes[16] = {0};
for (int i = 0; i < 16; ++i) {
key_bytes[i] = (unsigned char)key[i % key_len] ^ i;
}
memcpy(tea_key, key_bytes, 16);
}

// Вызываем оригинальную функцию дешифрования Полины
vector<uint8_t> decryptedtext = teaDecrypt(ciphertext, tea_key);

memcpy(output, decryptedtext.data(), decryptedtext.size());
}

void generate_key(char* out_key, int max_len) {
strncpy(out_key, "TEA_128bit_Key", max_len);
}
}

