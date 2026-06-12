#ifndef I_CIPHER_API_H
#define I_CIPHER_API_H

extern "C" {
    bool validate_key(const char* key);
    void encrypt(const unsigned char* input, unsigned char* output, int len, const char* key);
    void decrypt(const unsigned char* input, unsigned char* output, int len, const char* key);
    void generate_key(char* out_key, int max_len);
}

#endif