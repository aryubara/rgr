#include "ChaCha20.h"
#include "Utilities/crypto_utils.h"
#include <vector>
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <fstream>

void quarterRound(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) { // получаем случайные байты 
    a += b; d ^= a; d = ROTL(d, 16);
    c += d; b ^= c; b = ROTL(b, 12);
    a += b; d ^= a; d = ROTL(d, 8);
    c += d; b ^= c; b = ROTL(b, 7);
}

void columnRound(vector<uint32_t>& state) { // перемешивание по колонкам
    for (int i = 0; i < 4; ++i) { // по 4 колонки
        quarterRound(
            state[i], // a
            state[i + 4], // b
            state[i + 8], // c 
            state[i + 12] // d
        );
    }
}

void diagonalRound(vector<uint32_t>& state) { // перемешивание по диагонали
    quarterRound(state[0], state[5], state[10], state[15]);
    quarterRound(state[1], state[6], state[11], state[12]);
    quarterRound(state[2], state[7], state[8], state[13]);
    quarterRound(state[3], state[4], state[9], state[14]);
}

void chacha20block(vector<uint32_t>& state) {
    vector<uint32_t> copy_state = state;
    for (int i = 0; i < 10; ++ i) {
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

void chacha20_encrypt(vector <uint8_t>& plaintext, const uint32_t key_words[8], uint32_t nonce[3], 
                        vector<uint8_t>& ciphertext) {

    ciphertext.resize(plaintext.size());
    uint32_t counter = 0;
    vector<uint32_t> state(16); // используем безнаковые 32бит числа и линейную матрицу

    for (size_t offset = 0; offset < plaintext.size(); offset += 64) {
        initState(state, key_words, counter, nonce);
        chacha20block(state);

        uint8_t keystream_bytes[64];
        for (int i = 0; i < 16; ++i) { // преобразование 16 чисел в 64 байта
            keystream_bytes[4*i + 0] = (state[i] >> 0) & 0xFF; keystream_bytes[4*i + 1] = (state[i] >> 8) & 0xFF;
            keystream_bytes[4*i + 2] = (state[i] >> 16) & 0xFF; keystream_bytes[4*i + 3] = (state[i] >> 24) & 0xFF;
        }
        size_t block_size = min<size_t>(64, plaintext.size() - offset);
        for (size_t i = 0; i < block_size; ++i) {
            ciphertext[offset + i] = plaintext[offset + i] ^ keystream_bytes[i];
        }
        counter++;
    }
}

extern "C" {

const char* getName() {
    return "ChaCha20";
}

void encrypt(const vector<uint8_t>& input, vector<uint8_t>& output, const uint8_t* key, const uint8_t* nonce) {
    uint32_t key_words[8];
    uint32_t nonce_words[3];
    
    bytesToWordsLittleEndian(key, key_words, 8);
    bytesToWordsLittleEndian(nonce, nonce_words, 3);
    
    vector<uint8_t> data = input;
    chacha20_encrypt(data, key_words, nonce_words, output);
}

void decrypt(const vector<uint8_t>& input, vector<uint8_t>& output, const uint8_t* key, const uint8_t* nonce) {
             encrypt(input, output, key, nonce);
}
}