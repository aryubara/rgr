#include "../include/ICipherAPI.h"
#include "affine.h" // Подключаем заголовочный файл Полины
#include <vector>
#include <cstring>
#include <algorithm>
#include <cstdint>
#include <stdexcept>

using namespace std;

// ========== ВСПОМОГАТЕЛЬНАЯ МАТЕМАТИКА ПОЛИНЫ (из cryptoMath) ==========

// Алгоритм Евклида для поиска Наибольшего Общего Делителя
uint32_t gcd(uint32_t a, uint32_t b) {
while (b != 0) {
uint32_t temp = b;
b = a % b;
a = temp;
}
return a;
}

// Поиск модульного обратного элемента (нахождение a^-1 mod m)
uint32_t modInverse(uint32_t a, uint32_t m_val) {
a = a % m_val;
for (uint32_t x = 1; x < m_val; x++) {
if ((a * x) % m_val == 1)
return x;
}
return 1;
}

// ========== ОРИГИНАЛЬНАЯ МАТЕМАТИКА ПОЛИНЫ ==========

//ШИФРОВАНИЕ
vector<uint8_t> affineEncrypt(const vector<uint8_t>& data, uint32_t a, uint32_t b) {
if (gcd(a, m) != 1) { //a и m - взаимно простые
throw invalid_argument("Некорректный ключ a");
}

vector<uint8_t> result; //создание вектора
result.reserve(data.size()); //задание размера

for (uint8_t byte : data) { //шифрование байт
uint32_t encrypted = (a * byte + b) % m; // Y = (aX + b) mod m
result.push_back(static_cast<uint8_t>(encrypted));
}
return result;
}

//ДЕШИФРОВАНИЕ
vector<uint8_t> affineDecrypt(const vector<uint8_t>& data, uint32_t a, uint32_t b) {
if (gcd(a, m) != 1) { //a и m - взаимно простые
throw invalid_argument("Некорректный ключ a");
}

uint32_t aInverse = modInverse(a, m); //a^(-1) обратный к a по модулю m

vector<uint8_t> result; //создание вектора
result.reserve(data.size());//задание размера

for (uint8_t byte : data) { //дешифрование байтов
uint32_t decrypted = (aInverse * ((byte + m - (b % m)) % m)) % m; //(a^(-1))*(Y - b) mod m
//защита от отрицательных чисел

result.push_back(static_cast<uint8_t>(decrypted));
}
return result;
}

// ========== ЕДИНЫЙ ИНТЕРФЕЙС ПЛАГИНОВ (EXTERN "C") ==========

extern "C" {

bool validate_key(const char* key) {
// Ключ должен быть, и его первый символ ('a') должен быть взаимно простым с 256
if (!key || strlen(key) < 2) return false;
uint32_t a = static_cast<uint32_t>((unsigned char)key[0]);
return gcd(a, m) == 1;
}

void encrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
if (len <= 0) return;

vector<uint8_t> plaintext(input, input + len);

// По умолчанию берем хорошие параметры, если ключ пустой
uint32_t a = 5;
uint32_t b = 7;

// Если ключ передан, вытаскиваем параметры из первых двух байт строки
if (key && strlen(key) >= 2) {
a = static_cast<uint32_t>((unsigned char)key[0]);
b = static_cast<uint32_t>((unsigned char)key[1]);

// Если 'a' не валидно, скорректируем до ближайшего нечетного (взаимно простого с 256)
if (gcd(a, m) != 1) {
a |= 1;
}
}

try {
vector<uint8_t> ciphertext = affineEncrypt(plaintext, a, b);
memcpy(output, ciphertext.data(), len);
} catch (...) {
// Защита рантайма плагина от непредвиденных исключений
memcpy(output, input, len);
}
}

void decrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
if (len <= 0) return;

vector<uint8_t> ciphertext(input, input + len);

uint32_t a = 5;
uint32_t b = 7;

if (key && strlen(key) >= 2) {
a = static_cast<uint32_t>((unsigned char)key[0]);
b = static_cast<uint32_t>((unsigned char)key[1]);
if (gcd(a, m) != 1) {
a |= 1;
}
}

try {
vector<uint8_t> decryptedtext = affineDecrypt(ciphertext, a, b);
memcpy(output, decryptedtext.data(), len);
} catch (...) {
memcpy(output, input, len);
}
}

void generate_key(char* out_key, int max_len) {
// Генерируем дефолтную строку, где первый символ имеет код 5 (a=5), а второй код 7 (b=7)
if (max_len > 2) {
out_key[0] = 5;
out_key[1] = 7;
out_key[2] = '\0';
}
}
}

