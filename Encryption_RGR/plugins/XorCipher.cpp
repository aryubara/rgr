#include "../include/ICipherAPI.h"
#include <cstring>
#include <cstdlib>
#include <ctime>

using namespace std;

extern "C" {
    bool validate_key(const char* key) {
        return key != nullptr && strlen(key) > 0;
    }

    void encrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
        int key_len = strlen(key);
        for (int i = 0; i < len; ++i) {
            output[i] = input[i] ^ key[i % key_len];
        }
    }

    void decrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
        encrypt(input, output, len, key); 
    }

    void generate_key(char* out_key, int max_len) {
        const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        srand(time(nullptr));
        int len = rand() % (max_len - 4) + 4; 
        for (int i = 0; i < len; ++i) {
            out_key[i] = charset[rand() % (sizeof(charset) - 1)];
        }
        out_key[len] = '\0';
    }
}