#include "xor.h" 
#include <vector> 
#include <cstdint> 
#include <string>

using namespace std; 

// Реализация внутренней логики алгоритма
vector<uint8_t> xorEncryptDecrypt(const vector<uint8_t>& data, const string& key) { 
    vector<uint8_t> result(data.size()); 
    if (key.empty()) return data; 
     
    for (size_t i = 0; i < data.size(); ++i) { 
        // Побайтовое наложение ключа по циклу с помощью операции ИСКЛЮЧАЮЩЕЕ ИЛИ
        result[i] = data[i] ^ static_cast<uint8_t>(key[i % key.size()]); 
    } 
    return result; 
} 

// Реализация интерфейса динамической библиотеки
const char* getName() { 
    return "XOR Cipher"; 
} 

void encrypt(const vector<uint8_t>& inputData, vector<uint8_t>& outputData, const char* key) { 
    string cipherKey = (key != nullptr) ? key : "";
    outputData = xorEncryptDecrypt(inputData, cipherKey); 
} 

void decrypt(const vector<uint8_t>& inputData, vector<uint8_t>& outputData, const char* key) { 
    // Операция XOR симметрична, поэтому дешифрование полностью идентично шифрованию
    string cipherKey = (key != nullptr) ? key : "";
    outputData = xorEncryptDecrypt(inputData, cipherKey); 
}