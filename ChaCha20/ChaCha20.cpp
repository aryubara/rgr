#include "ChaCha20.h"
#include "Utilities/crypto_utils.h"
#include <vector>
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <fstream>

void printState(const vector<uint32_t>& state, const string& label) {
    cout << "\n" << label << endl;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            cout << hex << setw(8) << setfill('0') << state[i*4 + j] << " ";
        }
        cout << endl;
    }
    cout << dec << setfill(' ');
}

void printBytes(const uint8_t* data, size_t size, const string& label) {
    cout << "\n" << label << endl;
    for (size_t i = 0; i < size; ++i) {
        cout << hex << setw(2) << setfill('0') << (int)data[i] << " ";
        if ((i + 1) % 16 == 0) cout << endl;
    }
    cout << dec << setfill(' ') << endl;
}

void quarterRound(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) {
    cout << "  Выполнение quarterRound:\n";
    cout << "    До: a=" << hex << a << " b=" << b << " c=" << c << " d=" << d << dec << "\n";
    
    a += b; d ^= a; d = ROTL(d, 16);
    cout << "    После 1: a=" << hex << a << " d=" << d << dec << "\n";
    
    c += d; b ^= c; b = ROTL(b, 12);
    cout << "    После 2: c=" << hex << c << " b=" << b << dec << "\n";
    
    a += b; d ^= a; d = ROTL(d, 8);
    cout << "    После 3: a=" << hex << a << " d=" << d << dec << "\n";
    
    c += d; b ^= c; b = ROTL(b, 7);
    cout << "    После 4: b=" << hex << b << " c=" << c << dec << "\n";
}

void columnRound(vector<uint32_t>& state) {
    cout << "\n Перемешивание по колонкам\n";
    printState(state, "Состояние до columnRound");
    
    for (int i = 0; i < 4; ++i) {
        cout << "\nКолонка " << i << ":\n";
        quarterRound(
            state[i],
            state[i + 4],
            state[i + 8],
            state[i + 12]
        );
    }
    
    printState(state, "Состояние после columnRound");
}

void diagonalRound(vector<uint32_t>& state) {
    cout << "\n Перемешивание по диагонали\n";
    printState(state, "Состояние до diagonalRound");
    
    cout << "\nДиагональ 0:\n";
    quarterRound(state[0], state[5], state[10], state[15]);
    
    cout << "\nДиагональ 1:\n";
    quarterRound(state[1], state[6], state[11], state[12]);
    
    cout << "\nДиагональ 2:\n";
    quarterRound(state[2], state[7], state[8], state[13]);
    
    cout << "\nДиагональ 3:\n";
    quarterRound(state[3], state[4], state[9], state[14]);
    
    printState(state, "Состояние после diagonalRound");
}

void chacha20block(vector<uint32_t>& state) {
    cout << "\nГенерация блока\n";
    printState(state, "Начальное состояние");
    
    vector<uint32_t> copy_state = state;
    
    for (int i = 0; i < 10; ++i) {
        cout << "\n Раунд" << (i+1) << endl;
        columnRound(copy_state);
        diagonalRound(copy_state);
    }
    printState(copy_state, "Перемешанное состояние");
    
    for (size_t i = 0; i < state.size(); ++i) {
        state[i] += copy_state[i];
    }
    printState(state, "Итоговое состояние блока");
}

void initState(vector<uint32_t>& state, const uint32_t key_words[8], uint32_t counter, uint32_t nonce[3]) {
    cout << "\n Инициализация состояние\n";
    cout << "Счетчик: " << counter << "\n";
    cout << "Nonce: ";
    for (int i = 0; i < 3; ++i) cout << hex << nonce[i] << " ";
    cout << dec << "\n";
    
    state[0] = 0x61707865; state[1] = 0x3320646e; 
    state[2] = 0x79622d32; state[3] = 0x6b206574;
    
    cout << "Константы:\n";
    for (int i = 0; i < 4; ++i) {
        cout << "  state[" << i << "] = " << hex << state[i] << dec << "\n";
    }
    
    for (int i = 0; i < 8; ++i) {
        state[4 + i] = key_words[i];
    }
    
    cout << "Ключ:\n";
    for (int i = 0; i < 8; ++i) {
        cout << "  state[" << (4+i) << "] = " << hex << state[4+i] << dec << "\n";
    }
    
    state[12] = counter;
    cout << "Счетчик: state[12] = " << counter << "\n";
    
    for (int i = 0; i < 3; ++i) {
        state[i + 13] = nonce[i];
        cout << "Nonce[" << i << "]: state[" << (13+i) << "] = " << hex << nonce[i] << dec << "\n";
    }
}

void chacha20_encrypt(vector <uint8_t>& plaintext, const uint32_t key_words[8], uint32_t nonce[3], 
                        vector<uint8_t>& ciphertext) {
    cout << "\n Начало шифрования\n";
    cout << "Размер открытого текста: " << plaintext.size() << " байт\n";
    
    ciphertext.resize(plaintext.size());
    uint32_t counter = 0;
    vector<uint32_t> state(16);

    for (size_t offset = 0; offset < plaintext.size(); offset += 64) {
        cout << "\n БЛОК " << (offset/64 + 1) << endl;

        initState(state, key_words, counter, nonce);
        chacha20block(state);

        uint8_t keystream_bytes[64];
        for (int i = 0; i < 16; ++i) {
            keystream_bytes[4*i + 0] = (state[i] >> 0) & 0xFF;
            keystream_bytes[4*i + 1] = (state[i] >> 8) & 0xFF;
            keystream_bytes[4*i + 2] = (state[i] >> 16) & 0xFF;
            keystream_bytes[4*i + 3] = (state[i] >> 24) & 0xFF;
        }
        
        printBytes(keystream_bytes, 64, "Сгенерированный ключевой поток");
        
        size_t block_size = min<size_t>(64, plaintext.size() - offset);
        cout << "\nРазмер блока для шифрования: " << block_size << " байт\n";
        
        cout << "Открытый текст (блок):\n";
        for (size_t i = 0; i < block_size; ++i) {
            cout << hex << setw(2) << setfill('0') << (int)plaintext[offset + i] << " ";
            if ((i + 1) % 16 == 0) cout << "\n";
        }
        cout << dec << setfill(' ') << "\n";
        
        for (size_t i = 0; i < block_size; ++i) {
            ciphertext[offset + i] = plaintext[offset + i] ^ keystream_bytes[i];
        }
        
        cout << "Зашифрованный текст (блок):\n";
        for (size_t i = 0; i < block_size; ++i) {
            cout << hex << setw(2) << setfill('0') << (int)ciphertext[offset + i] << " ";
            if ((i + 1) % 16 == 0) cout << "\n";
        }
        cout << dec << setfill(' ') << "\n";
        
        counter++;
        cout << "Счетчик увеличен до: " << counter << "\n";
    }
    
}

extern "C" {

const char* getName() {
    return "ChaCha20";
}

void encrypt(const vector<uint8_t>& input, vector<uint8_t>& output, const uint8_t* key, const uint8_t* nonce) {
    cout << "Размер входных данных: " << input.size() << " байт\n";
    
    uint32_t key_words[8];
    uint32_t nonce_words[3];
    
    cout << "Преобразование ключа (little-endian):\n";
    bytesToWordsLittleEndian(key, key_words, 8);
    cout << "Ключевые слова: ";
    for (int i = 0; i < 8; ++i) cout << hex << key_words[i] << " ";
    cout << dec << "\n";
    
    cout << "Преобразование nonce (little-endian):\n";
    bytesToWordsLittleEndian(nonce, nonce_words, 3);
    cout << "Nonce слова: ";
    for (int i = 0; i < 3; ++i) cout << hex << nonce_words[i] << " ";
    cout << dec << "\n";
    
    vector<uint8_t> data = input;
    chacha20_encrypt(data, key_words, nonce_words, output);
}

void decrypt(const vector<uint8_t>& input, vector<uint8_t>& output, const uint8_t* key, const uint8_t* nonce) {
    encrypt(input, output, key, nonce);
}
}