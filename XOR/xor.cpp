#include "xor.h" 
#include <vector> 
#include <cstdint> 
#include <string>
#include <iostream>  
#include <bitset>    

using namespace std; 

// вспомогательная функция для красивого пошагового вывода
void printStep(size_t index, char dataChar, uint8_t dataByte, char keyChar, uint8_t keyByte, uint8_t resultByte) {
    cout << index + 1 << ") " << dataChar << " (ключ " << keyChar << ")\n";
    cout << "  " << dataChar << " -> ASCII-код: " << static_cast<int>(dataByte) 
         << " -> в двоичной системе: " << bitset<8>(dataByte) << "\n";
    cout << "  " << keyChar << " -> ASCII-код: " << static_cast<int>(keyByte) 
         << " -> в двоичной системе: " << bitset<8>(keyByte) << "\n";
    cout << "  Вычисление XOR: " << bitset<8>(resultByte) 
         << " (" << static_cast<int>(resultByte) << ")\n\n";
}

// реализация внутренней логики алгоритма
vector<uint8_t> xorEncryptDecrypt(const vector<uint8_t>& data, const string& key) { 
    vector<uint8_t> result(data.size()); 
    if (key.empty()) return data; 
     
    cout << "КЛЮЧ: " << key << "\n\n";

    for (size_t i = 0; i < data.size(); ++i) { 
        uint8_t dataByte = data[i];
        uint8_t keyByte = static_cast<uint8_t>(key[i % key.size()]);
        
        result[i] = dataByte ^ keyByte; 

        char dataChar = static_cast<char>(dataByte);
        char keyChar = static_cast<char>(keyByte);
        
        printStep(i, dataChar, dataByte, keyChar, keyByte, result[i]);
    } 
    return result; 
} 

// реализация интерфейса динамической библиотеки
const char* getName() { 
    return "XOR Cipher"; 
} 

void encrypt(const vector<uint8_t>& inputData, vector<uint8_t>& outputData, const char* key) { 
    string cipherKey = (key != nullptr) ? key : "";
    
    cout << "       ШИФРОВАНИЕ      \n";

    
    outputData = xorEncryptDecrypt(inputData, cipherKey); 
    
}

void decrypt(const vector<uint8_t>& inputData, vector<uint8_t>& outputData, const char* key) { 
    string cipherKey = (key != nullptr) ? key : "";
    
    
   
    cout << "      ДЕШИФРОВАНИЕ     \n";
   
    outputData = xorEncryptDecrypt(inputData, cipherKey); 
    
    
}
