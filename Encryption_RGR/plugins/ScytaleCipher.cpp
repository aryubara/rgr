#include "../include/ICipherAPI.h"
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <string>

using namespace std;

extern "C" {
bool validate_key(const char* key) {
if (!key || strlen(key) == 0) return false;
for (size_t i = 0; i < strlen(key); ++i) {
if (!isdigit(key[i])) return false;
}
int val = atoi(key);
return val > 1;
}

void encrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
int columns = atoi(key);
if (columns <= 1) return;
int rows = (len + columns - 1) / columns;
int idx = 0;

for (int c = 0; c < columns; ++c) {
for (int r = 0; r < rows; ++r) {
int input_idx = r * columns + c;
if (input_idx < len) {
output[idx++] = input[input_idx];
} else {
output[idx++] = ' '; // Возвращаем обычный пробел вместо 0x00
}
}
}
}

void decrypt(const unsigned char* input, unsigned char* output, int len, const char* key) {
int columns = atoi(key);
if (columns <= 1) return;
int rows = (len + columns - 1) / columns;
int idx = 0;

for (int r = 0; r < rows; ++r) {
for (int c = 0; c < columns; ++c) {
int input_idx = c * rows + r;
if (input_idx < len) {
output[idx++] = input[input_idx];
}
}
}
}

void generate_key(char* out_key, int max_len) {
srand(time(nullptr));
int columns = rand() % 8 + 2;
string s = to_string(columns);
strncpy(out_key, s.c_str(), max_len);
}
}

