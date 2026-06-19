#include "Serpent.h"
#include "crypto_utils.h"
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

const unsigned char S[8][16] = { // S-БЛОКИ serpent
    { 3, 8,15, 1,10, 6, 5,11,14,13, 4, 2, 7, 0, 9,12 },
    {15,12, 2, 7, 9, 0, 5,10, 1,11,14, 8, 6,13, 3, 4 },
    { 8, 6, 7, 9, 3,12,10,15,13, 1,14, 4, 0,11, 5, 2 },
    { 0,15,11, 8,12, 9, 6, 3,13, 1, 2, 4,10, 7, 5,14 },
    { 1,15, 8, 3,12, 0,11, 6, 2, 5, 4,10, 9,14, 7,13 },
    {15, 5, 2,11, 4,10, 9,12, 0, 3,14, 8,13, 6, 7, 1 },
    { 7, 2,12, 5, 8, 4, 6,11,14, 9, 1,15,13, 3,10, 0 },
    { 1,13,15, 0,14, 8, 2,11, 7, 4,12,10, 9, 3, 5, 6 }
};

const unsigned char invS[8][16] = {
    {13, 3,11, 0,10, 6, 5,12, 1,14, 4, 7,15, 9, 8, 2},
    { 5, 8, 2,14,15, 6,12, 3,11, 4, 7, 9, 1,13,10, 0},
    {12, 9,15, 4,11,14, 1, 2, 0, 3, 6,13, 5, 8,10, 7},
    { 0, 9,10, 7,11,14, 6,13, 3, 5,12, 2, 4, 8,15, 1},
    { 5, 0, 8, 3,10, 9, 7,14, 2,12,11, 6, 4,15,13, 1},
    { 8,15, 2, 9, 4, 1,13,14,11, 6, 5, 3, 7,12,10, 0},
    {15,10, 1,13, 5, 3, 6, 0, 4, 9,14, 7, 2,12, 8,11},
    { 3, 0, 6,13, 9,14,15, 8, 5,12,11, 7,10, 1, 4, 2}
};

void linear_transform(uint32_t& word0, uint32_t& word1, uint32_t& word2, uint32_t& word3) {
    word0 = ROTL(word0, 13); word2 = ROTL(word2, 3);
    word1 ^= word0 ^ word2; word3 ^= word2 ^ (word0 << 3);
    word1 = ROTL(word1, 1); word3 = ROTL(word3, 7);
    word0 ^= word1 ^ word3; word2 ^= word3 ^ (word1 << 7);
    word0 = ROTL(word0, 5); word2 = ROTL(word2, 22);
}

void inverse_linear_transform(uint32_t& word0, uint32_t& word1, uint32_t& word2, uint32_t& word3) {
    word2 = ROTL(word2, 10); word0 = ROTL(word0, 27);
    word2 ^= word3 ^ (word1 << 7); word0 ^= word1 ^ word3;
    word3 = ROTL(word3, 25); word1 = ROTL(word1, 31);
    word3 ^= word2 ^ (word0 << 3); word1 ^= word0 ^ word2;
    word2 = ROTL(word2, 29); word0 = ROTL(word0, 19);
}

void sbox(uint32_t& word0, uint32_t& word1, uint32_t& word2, uint32_t& word3, int round) { // S-блочный слой
    int sbox_index = round % 8;
    for (int bit_pos = 0; bit_pos < 32; bit_pos += 4) {
        unsigned int nibble0 = (word0 >> bit_pos) & 0xF; unsigned int nibble1 = (word1 >> bit_pos) & 0xF;
        unsigned int nibble2 = (word2 >> bit_pos) & 0xF; unsigned int nibble3 = (word3 >> bit_pos) & 0xF;
        
        nibble0 = S[sbox_index][nibble0]; nibble1 = S[sbox_index][nibble1];
        nibble2 = S[sbox_index][nibble2]; nibble3 = S[sbox_index][nibble3];
        
        word0 = (word0 & ~(0xF << bit_pos)) | (nibble0 << bit_pos);
        word1 = (word1 & ~(0xF << bit_pos)) | (nibble1 << bit_pos);
        word2 = (word2 & ~(0xF << bit_pos)) | (nibble2 << bit_pos);
        word3 = (word3 & ~(0xF << bit_pos)) | (nibble3 << bit_pos);
    }
}

void inverse_sbox(uint32_t& word0, uint32_t& word1, uint32_t& word2, uint32_t& word3, int round) {
    int sbox_index = round % 8;
    for (int bit_pos = 0; bit_pos < 32; bit_pos += 4) {
        unsigned int nibble0 = (word0 >> bit_pos) & 0xF; unsigned int nibble1 = (word1 >> bit_pos) & 0xF;
        unsigned int nibble2 = (word2 >> bit_pos) & 0xF; unsigned int nibble3 = (word3 >> bit_pos) & 0xF;
        
        nibble0 = invS[sbox_index][nibble0]; nibble1 = invS[sbox_index][nibble1];
        nibble2 = invS[sbox_index][nibble2]; nibble3 = invS[sbox_index][nibble3];
        
        word0 = (word0 & ~(0xF << bit_pos)) | (nibble0 << bit_pos);
        word1 = (word1 & ~(0xF << bit_pos)) | (nibble1 << bit_pos);
        word2 = (word2 & ~(0xF << bit_pos)) | (nibble2 << bit_pos);
        word3 = (word3 & ~(0xF << bit_pos)) | (nibble3 << bit_pos);
    }
}

void serpent_key_schedule(const uint32_t key[8], uint32_t subkeys[132]) { // расширение ключа
    uint32_t key_copy[132];
    const uint32_t PHI = 0x9E3779B9;
    for (int key_ind = 0; key_ind < 8; ++key_ind) key_copy[key_ind] = key[key_ind];
    for (int key_ind = 8; key_ind < 132; ++key_ind) {
        key_copy[key_ind] = (key_copy[key_ind-8] ^ key_copy[key_ind-5] ^ key_copy[key_ind-3] ^ key_copy[key_ind-1] ^ PHI ^ (key_ind-8));
        key_copy[key_ind] = (key_copy[key_ind] << 11) | (key_copy[key_ind] >> 21);
    }
    for (int key_ind = 0; key_ind < 33; ++key_ind) {
        subkeys[4*key_ind] = key_copy[4*key_ind]; subkeys[4*key_ind+1] = key_copy[4*key_ind+1];
        subkeys[4*key_ind+2] = key_copy[4*key_ind+2]; subkeys[4*key_ind+3] = key_copy[4*key_ind+3];
        
        int sbox_index = (32 - key_ind) % 8;
        for (int word_ind = 0; word_ind < 4; ++word_ind) {
            unsigned int nibble0 = (subkeys[4*key_ind+word_ind] >> 0) & 0xF; unsigned int nibble1 = (subkeys[4*key_ind+word_ind] >> 4) & 0xF;
            unsigned int nibble2 = (subkeys[4*key_ind+word_ind] >> 8) & 0xF; unsigned int nibble3 = (subkeys[4*key_ind+word_ind] >> 12) & 0xF;
            unsigned int nibble4 = (subkeys[4*key_ind+word_ind] >> 16) & 0xF; unsigned int nibble5 = (subkeys[4*key_ind+word_ind] >> 20) & 0xF;
            unsigned int nibble6 = (subkeys[4*key_ind+word_ind] >> 24) & 0xF; unsigned int nibble7 = (subkeys[4*key_ind+word_ind] >> 28) & 0xF;
            
            nibble0 = S[sbox_index][nibble0]; nibble1 = S[sbox_index][nibble1]; nibble2 = S[sbox_index][nibble2]; nibble3 = S[sbox_index][nibble3];
            nibble4 = S[sbox_index][nibble4]; nibble5 = S[sbox_index][nibble5]; nibble6 = S[sbox_index][nibble6]; nibble7 = S[sbox_index][nibble7];
            
            subkeys[4*key_ind+word_ind] = nibble0 | (nibble1<<4) | (nibble2<<8) | (nibble3<<12) |
             (nibble4<<16) | (nibble5<<20) | (nibble6<<24) | (nibble7<<28);
        }
    }
}

void serpent_encrypt_block(const uint8_t plaintext[16], uint8_t ciphertext[16], const uint32_t subkeys[132]) {
    uint32_t state0, state1, state2, state3;
    state0 = (plaintext[0] << 24) | (plaintext[1] << 16) | (plaintext[2] << 8) | plaintext[3];
    state1 = (plaintext[4] << 24) | (plaintext[5] << 16) | (plaintext[6] << 8) | plaintext[7];
    state2 = (plaintext[8] << 24) | (plaintext[9] << 16) | (plaintext[10] << 8) | plaintext[11];
    state3 = (plaintext[12] << 24) | (plaintext[13] << 16) | (plaintext[14] << 8) | plaintext[15];
    
    for (int round = 0; round < 32; round++) {
        state0 ^= subkeys[4*round];
        state1 ^= subkeys[4*round+1];
        state2 ^= subkeys[4*round+2];
        state3 ^= subkeys[4*round+3];
        
        sbox(state0, state1, state2, state3, round);
        if (round < 31) {
            linear_transform(state0, state1, state2, state3);
        }
    }
    state0 ^= subkeys[128];
    state1 ^= subkeys[129];
    state2 ^= subkeys[130];
    state3 ^= subkeys[131];
    
    ciphertext[0] = state0 >> 24; ciphertext[1] = state0 >> 16; ciphertext[2] = state0 >> 8; ciphertext[3] = state0;
    ciphertext[4] = state1 >> 24; ciphertext[5] = state1 >> 16; ciphertext[6] = state1 >> 8; ciphertext[7] = state1;
    ciphertext[8] = state2 >> 24; ciphertext[9] = state2 >> 16; ciphertext[10] = state2 >> 8; ciphertext[11] = state2;
    ciphertext[12] = state3 >> 24; ciphertext[13] = state3 >> 16; ciphertext[14] = state3 >> 8; ciphertext[15] = state3;
}

void serpent_decrypt(const uint8_t ciphertext[16], uint8_t plaintext[16], // дешифрование блока
                     const uint32_t subkeys[132]) {
    uint32_t state0, state1, state2, state3;
    
    state0 = (ciphertext[0] << 24) | (ciphertext[1] << 16) | (ciphertext[2] << 8) | ciphertext[3];
    state1 = (ciphertext[4] << 24) | (ciphertext[5] << 16) | (ciphertext[6] << 8) | ciphertext[7];
    state2 = (ciphertext[8] << 24) | (ciphertext[9] << 16) | (ciphertext[10] << 8) | ciphertext[11];
    state3 = (ciphertext[12] << 24) | (ciphertext[13] << 16) | (ciphertext[14] << 8) | ciphertext[15];
    
    state0 ^= subkeys[128];
    state1 ^= subkeys[129];
    state2 ^= subkeys[130];
    state3 ^= subkeys[131];
    
    for (int round = 31; round >= 0; round--) {
        if (round < 31) {
            inverse_linear_transform(state0, state1, state2, state3);
        }
        inverse_sbox(state0, state1, state2, state3, round);
        
        state0 ^= subkeys[4*round];
        state1 ^= subkeys[4*round+1];
        state2 ^= subkeys[4*round+2];
        state3 ^= subkeys[4*round+3];
    }
    plaintext[0] = state0 >> 24; plaintext[1] = state0 >> 16; plaintext[2] = state0 >> 8; plaintext[3] = state0;
    plaintext[4] = state1 >> 24; plaintext[5] = state1 >> 16; plaintext[6] = state1 >> 8; plaintext[7] = state1;
    plaintext[8] = state2 >> 24; plaintext[9] = state2 >> 16; plaintext[10] = state2 >> 8; plaintext[11] = state2;
    plaintext[12] = state3 >> 24; plaintext[13] = state3 >> 16; plaintext[14] = state3 >> 8; plaintext[15] = state3;
}

void addPadding(vector<uint8_t>& data) {
    uint8_t padding = 16 - (data.size() % 16);
    if (padding == 0) padding = 16;
    for (uint8_t i = 0; i < padding; ++i) {
        data.push_back(padding);
    }
}

void removePadding(vector<uint8_t>& data) {
    if (data.empty()) return;
    uint8_t padding = data.back();
    if (padding > 16) return;
    data.resize(data.size() - padding);
}

vector<uint8_t> encryptData(const vector<uint8_t>& input, const uint32_t subkeys[132]) {
    vector<uint8_t> data = input;
    addPadding(data);
    
    vector<uint8_t> encrypted;
    uint8_t block[16];
    uint8_t out[16];
    
    for (size_t i = 0; i < data.size(); i += 16) {
        memcpy(block, &data[i], 16);
        serpent_encrypt_block(block, out, subkeys);
        encrypted.insert(encrypted.end(), out, out + 16);
    }
    return encrypted;
}

vector<uint8_t> decryptData(const vector<uint8_t>& encrypted, const uint32_t subkeys[132]) {
    vector<uint8_t> decrypted;
    uint8_t block[16], out[16];
    
    for (size_t i = 0; i < encrypted.size(); i += 16) {
        memcpy(block, &encrypted[i], 16);
        serpent_decrypt(block, out, subkeys);
        decrypted.insert(decrypted.end(), out, out + 16);
    }
    removePadding(decrypted);
    return decrypted;
}

extern "C" {

const char* getName() { return "Serpent"; }

void encrypt(const vector<uint8_t>& input, vector<uint8_t>& output, const uint8_t* key) {
    uint32_t key_words[8];
    bytesToWordsBigEndian(key, key_words, 8);
    
    uint32_t subkeys[132];
    serpent_key_schedule(key_words, subkeys);
    
    output = encryptData(input, subkeys);
}

void decrypt(const vector<uint8_t>& input, vector<uint8_t>& output, const uint8_t* key) {
    uint32_t key_words[8];
    bytesToWordsBigEndian(key, key_words, 8);
    
    uint32_t subkeys[132];
    serpent_key_schedule(key_words, subkeys);
    
    output = decryptData(input, subkeys);
}
}
