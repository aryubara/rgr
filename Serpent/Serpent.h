#ifndef SERPENT_H
#define SERPENT_H

#include <vector>
#include <cstdint>
#include <string>

using namespace std;

#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

const int SERPENT_BLOCK_SIZE = 16;  // 128 бит
const int SERPENT_KEY_SIZE = 32;     // 256 бит
const int SERPENT_ROUNDS = 32;
const int SERPENT_SUBKEYS = 132;     // 33 раунда * 4 ключа

void serpent_key_schedule(const uint32_t key[8], uint32_t subkeys[132]);

// функции шифрования/дешифрования блоков
void serpent_encrypt_block(const uint8_t plaintext[16], uint8_t ciphertext[16], const uint32_t subkeys[132]);
void serpent_decrypt_block(const uint8_t ciphertext[16], uint8_t plaintext[16], const uint32_t subkeys[132]);

// функции для работы с данными (padding)
void addPadding(vector<uint8_t>& data);
void removePadding(vector<uint8_t>& data);

#endif
