#include "../include/ICipherAPI.h"
#include "ChaCha20.h" // Подключаем заголовочный файл Алины
#include <vector>
#include <cstring>
#include <algorithm>
#include <cstdint>

using namespace std;

// ========== ОРИГИНАЛЬНАЯ МАТЕМАТИКА АЛИНИНОГО АЛГОРИТМА ==========

void quarterRound(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
a += b; d ^= a; d = ROTL(d, 16);
c += d; b ^= c; b = ROTL(b, 12);
a += b; d ^= a; d = ROTL(d, 8);
c += d; b ^= c; b = ROTL(b, 7);
}

void columnRound(vector<uint32_t>& state) {
for (int i = 0; i < 4; ++i) {
quarterRound(state[i], state[i + 4], state[i + 8], state[i + 12]);
}
}

void diagonalRound(vector<uint32_t>& state) {
quarterRound(state[0], state[5], state[10], state[15]);
quarterRound(state[1], state[6], state[11], state[12]);
quarterRound(state[2], state[7], state[8], state[13]);
quarterRound(state[3], state[4], state[9], state[14]);
}

void chacha20block(vector<uint32_t>& state) {
vector<uint32_t> copy_state = state;
for (int i = 0; i < 10; ++i) {
columnRound(copy_state);
diagonalRound(copy_state);
}
for (size_t i = 0; i < state.size(); ++i) {
state[i] += copy_state[i];
}
}

void initState(vector<uint32_t>& state, const uint32_t key_words[8], uint32_t counter, uint32_t nonce[3]) {
state[0] = 0x61707865; state[1] = 0x3320646e;
state[2] = 0x79622d32; state[3] = 0x6b206574;
for (int i = 0; i < 8; ++i) state[4 + i] = key_words[i];
state[12] = counter;
for (int i = 0; i < 3; ++i) state[i + 13] = nonce[i];
}

void chacha20_encrypt(vector <uint8_t>& plaintext, const uint32_t key_words[8], uint32_t nonce[3], vector<uint8_t>& ciphertext) {
ciphertext.resize(plaintext.size());
uint32_t counter = 0;
vector<uint32_t> state(16);

for (size_t offset = 0; offset < plaintext.size(); offset += 64) {
initState(state, key_words, counter, nonce);
chacha20block(state);

uint8_t keystream_bytes[64];
for (int i = 0; i < 16; ++i) {
keystream_bytes[4*i + 0] = (state[i] >> 0) & 0xFF;
keystream_bytes[4*i + 1] = (state[i] >> 8) & 0xFF;
keystream_bytes[4*i + 2] = (state[i] >> 16) & 0xFF;
keystream_bytes[4*i + 3] = (state[i] >> 24) & 0xFF;
}
size_t block_size = min<size_t>(64, plaintext.size() - offset);
for (size_t i = 0; i < block_size; ++i) {
ciphertext[offset + i] = plaintext[offset + i] ^ keystream_bytes[i];
}
counter++;
}
}

// ========== ЕДИНЫЙ ИНТЕРФЕЙС ПЛАГИНОВ (EXTERN "C") ==========

extern "C" {

bool validate_key(const char* key) {
return key && strlen(key) > 0;
}

void encrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
if (len <= 0) return;

vector<uint8_t> plaintext(input, input + len);
vector<uint8_t> ciphertext;

// Воспроизводим 256-битный ключ из пользовательской строки
uint32_t key_words[8] = {0};
uint8_t key_bytes[32] = {0};
size_t key_len = strlen(key);
for (int i = 0; i < 32; ++i) {
key_bytes[i] = (unsigned char)key[i % key_len] ^ (i * 13);
}
for (int i = 0; i < 8; ++i) {
key_words[i] = (key_bytes[i*4] << 24) | (key_bytes[i*4+1] << 16) |
(key_bytes[i*4+2] << 8) | key_bytes[i*4+3];
}

// Фиксированный технический Nonce для плагина
uint32_t nonce[3] = { 0x00112233, 0x44556677, 0x8899AABB };

// Запускаем родное шифрование Алины
chacha20_encrypt(plaintext, key_words, nonce, ciphertext);

memcpy(output, ciphertext.data(), len);
}

void decrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
// У ChaCha20 шифрование и расшифрование делают одно и то же (XOR)
encrypt(input, output, len, key);
}

void generate_key(char* out_key, int max_len) {
strncpy(out_key, "ChaCha20_Secure_Key_256", max_len);
}
}

