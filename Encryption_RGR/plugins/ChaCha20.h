#ifndef CHACHA20_H
#define CHACHA20_H

#include <vector>
#include <cstdint>
#include <string>

using namespace std;

#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

void quarterRound(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d);

void columnRound(vector<uint32_t>& state);

void diagonalRound(vector<uint32_t>& state);

void chacha20block(vector<uint32_t>& state);

void initState(vector<uint32_t>& state, const uint32_t key_words[8], uint32_t counter, uint32_t nonce[3]);

void chacha20_encrypt(vector <uint8_t>& plaintext, const uint32_t key_words[8], uint32_t nonce[3], vector<uint8_t>& ciphertext);

void encryptFile(const string& inputFile, const string& outputFile, const uint32_t key_words[8], uint32_t nonce[3]);

void decryptFile(const string& inputFile, const string& outputFile, const uint32_t key_words[8], uint32_t nonce[3]);

void inputChaCha20();

#endif