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

void linear_transform(uint32_t& x0, uint32_t& x1, uint32_t& x2, uint32_t& x3) {
    x0 = (x0 << 13) | (x0 >> 19);
    x2 = (x2 << 3) | (x2 >> 29);
    x1 = x1 ^ x0 ^ x2;
    x3 = x3 ^ x2 ^ (x0 << 3);
    x1 = (x1 << 1) | (x1 >> 31);
    x3 = (x3 << 7) | (x3 >> 25);
    x0 = x0 ^ x1 ^ x3;
    x2 = x2 ^ x3 ^ (x1 << 7);
    x0 = (x0 << 5) | (x0 >> 27);
    x2 = (x2 << 22) | (x2 >> 10);
}

void inverse_linear_transform(uint32_t& x0, uint32_t& x1, uint32_t& x2, uint32_t& x3) {
    x2 = (x2 << 10) | (x2 >> 22);
    x0 = (x0 << 27) | (x0 >> 5);
    x2 = x2 ^ x3 ^ (x1 << 7);
    x0 = x0 ^ x1 ^ x3;
    x3 = (x3 << 25) | (x3 >> 7);
    x1 = (x1 << 31) | (x1 >> 1);
    x3 = x3 ^ x2 ^ (x0 << 3);
    x1 = x1 ^ x0 ^ x2;
    x2 = (x2 << 29) | (x2 >> 3);
    x0 = (x0 << 19) | (x0 >> 13);
}

void sbox(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d, int round) { // S-блочный слой
    int si = round % 8;
    for (int i = 0; i < 32; i += 4) {
        unsigned int aa = (a >> i) & 0xF;
        unsigned int bb = (b >> i) & 0xF;
        unsigned int cc = (c >> i) & 0xF;
        unsigned int dd = (d >> i) & 0xF;
        
        aa = S[si][aa];
        bb = S[si][bb];
        cc = S[si][cc];
        dd = S[si][dd];
        
        a = (a & ~(0xF << i)) | (aa << i);
        b = (b & ~(0xF << i)) | (bb << i);
        c = (c & ~(0xF << i)) | (cc << i);
        d = (d & ~(0xF << i)) | (dd << i);
    }
}

void inverse_sbox(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d, int round) {
    int si = round % 8;
    for (int i = 0; i < 32; i += 4) {
        unsigned int aa = (a >> i) & 0xF;
        unsigned int bb = (b >> i) & 0xF;
        unsigned int cc = (c >> i) & 0xF;
        unsigned int dd = (d >> i) & 0xF;
        
        aa = invS[si][aa];
        bb = invS[si][bb];
        cc = invS[si][cc];
        dd = invS[si][dd];
        
        a = (a & ~(0xF << i)) | (aa << i);
        b = (b & ~(0xF << i)) | (bb << i);
        c = (c & ~(0xF << i)) | (cc << i);
        d = (d & ~(0xF << i)) | (dd << i);
    }
}

void serpent_key_schedule(const uint32_t key[8], uint32_t subkeys[132]) { // расширение ключа
    uint32_t w[132];
    const uint32_t PHI = 0x9E3779B9;
    for (int i = 0; i < 8; i++) {
        w[i] = key[i];
    }
    for (int i = 8; i < 132; i++) {
        w[i] = (w[i-8] ^ w[i-5] ^ w[i-3] ^ w[i-1] ^ PHI ^ (i-8));
        w[i] = (w[i] << 11) | (w[i] >> 21);
    }
    for (int i = 0; i < 33; i++) {
        subkeys[4*i] = w[4*i];
        subkeys[4*i+1] = w[4*i+1];
        subkeys[4*i+2] = w[4*i+2];
        subkeys[4*i+3] = w[4*i+3];
        
        int si = (33 - 1 - i) % 8;
        for (int j = 0; j < 4; j++) {
            unsigned int a = (subkeys[4*i+j] >> 0) & 0xF;
            unsigned int b = (subkeys[4*i+j] >> 4) & 0xF;
            unsigned int c = (subkeys[4*i+j] >> 8) & 0xF;
            unsigned int d = (subkeys[4*i+j] >> 12) & 0xF;
            unsigned int e = (subkeys[4*i+j] >> 16) & 0xF;
            unsigned int f = (subkeys[4*i+j] >> 20) & 0xF;
            unsigned int g = (subkeys[4*i+j] >> 24) & 0xF;
            unsigned int h = (subkeys[4*i+j] >> 28) & 0xF;
            
            a = S[si][a];
            b = S[si][b];
            c = S[si][c];
            d = S[si][d];
            e = S[si][e];
            f = S[si][f];
            g = S[si][g];
            h = S[si][h];
            
            subkeys[4*i+j] = a | (b<<4) | (c<<8) | (d<<12) | (e<<16) | (f<<20) | (g<<24) | (h<<28);
        }
    }
}

void serpent_encrypt_block(const uint8_t plaintext[16], uint8_t ciphertext[16], const uint32_t subkeys[132]) {
    uint32_t x0, x1, x2, x3;
    x0 = (plaintext[0] << 24) | (plaintext[1] << 16) | (plaintext[2] << 8) | plaintext[3];
    x1 = (plaintext[4] << 24) | (plaintext[5] << 16) | (plaintext[6] << 8) | plaintext[7];
    x2 = (plaintext[8] << 24) | (plaintext[9] << 16) | (plaintext[10] << 8) | plaintext[11];
    x3 = (plaintext[12] << 24) | (plaintext[13] << 16) | (plaintext[14] << 8) | plaintext[15];
    
    for (int round = 0; round < 32; round++) {
        x0 ^= subkeys[4*round];
        x1 ^= subkeys[4*round+1];
        x2 ^= subkeys[4*round+2];
        x3 ^= subkeys[4*round+3];
        
        sbox(x0, x1, x2, x3, round);
        if (round < 31) {
            linear_transform(x0, x1, x2, x3);
        }
    }
    x0 ^= subkeys[128];
    x1 ^= subkeys[129];
    x2 ^= subkeys[130];
    x3 ^= subkeys[131];
    
    ciphertext[0] = x0 >> 24; ciphertext[1] = x0 >> 16; ciphertext[2] = x0 >> 8; ciphertext[3] = x0;
    ciphertext[4] = x1 >> 24; ciphertext[5] = x1 >> 16; ciphertext[6] = x1 >> 8; ciphertext[7] = x1;
    ciphertext[8] = x2 >> 24; ciphertext[9] = x2 >> 16; ciphertext[10] = x2 >> 8; ciphertext[11] = x2;
    ciphertext[12] = x3 >> 24; ciphertext[13] = x3 >> 16; ciphertext[14] = x3 >> 8; ciphertext[15] = x3;
}

void serpent_decrypt(const uint8_t ciphertext[16], uint8_t plaintext[16], // дешифрование блока
                     const uint32_t subkeys[132]) {
    uint32_t x0, x1, x2, x3;
    
    x0 = (ciphertext[0] << 24) | (ciphertext[1] << 16) | (ciphertext[2] << 8) | ciphertext[3];
    x1 = (ciphertext[4] << 24) | (ciphertext[5] << 16) | (ciphertext[6] << 8) | ciphertext[7];
    x2 = (ciphertext[8] << 24) | (ciphertext[9] << 16) | (ciphertext[10] << 8) | ciphertext[11];
    x3 = (ciphertext[12] << 24) | (ciphertext[13] << 16) | (ciphertext[14] << 8) | ciphertext[15];
    
    x0 ^= subkeys[128];
    x1 ^= subkeys[129];
    x2 ^= subkeys[130];
    x3 ^= subkeys[131];
    
    for (int round = 31; round >= 0; round--) {
        if (round < 31) {
            inverse_linear_transform(x0, x1, x2, x3);
        }
        inverse_sbox(x0, x1, x2, x3, round);
        
        x0 ^= subkeys[4*round];
        x1 ^= subkeys[4*round+1];
        x2 ^= subkeys[4*round+2];
        x3 ^= subkeys[4*round+3];
    }
    plaintext[0] = x0 >> 24; plaintext[1] = x0 >> 16; plaintext[2] = x0 >> 8; plaintext[3] = x0;
    plaintext[4] = x1 >> 24; plaintext[5] = x1 >> 16; plaintext[6] = x1 >> 8; plaintext[7] = x1;
    plaintext[8] = x2 >> 24; plaintext[9] = x2 >> 16; plaintext[10] = x2 >> 8; plaintext[11] = x2;
    plaintext[12] = x3 >> 24; plaintext[13] = x3 >> 16; plaintext[14] = x3 >> 8; plaintext[15] = x3;
}

void addPadding(vector<uint8_t>& data) {
    uint8_t padding = 16 - (data.size() % 16);
    if (padding == 0) padding = 16;
    for (uint8_t i = 0; i < padding; i++) {
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
    uint8_t block[16];
    uint8_t out[16];
    
    for (size_t i = 0; i < encrypted.size(); i += 16) {
        memcpy(block, &encrypted[i], 16);
        serpent_decrypt(block, out, subkeys);
        decrypted.insert(decrypted.end(), out, out + 16);
    }
    removePadding(decrypted);
    return decrypted;
}

extern "C" {

const char* getName() {
    return "Serpent";
}

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