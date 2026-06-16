#include "scytale.h" 
#include <vector> 
#include <cstdint> 
 
using namespace std; 
 
// Реализация внутренней логики шифрования (намотка на цилиндр)
vector<uint8_t> scytaleEncrypt(const vector<uint8_t>& data, uint32_t columns) { 
    if (columns <= 1 || data.empty()) return data; 
     
    int rows = (data.size() + columns - 1) / columns; 
    vector<uint8_t> output(rows * columns, ' '); // Заполнение пробелами для текста 
     
    int idx = 0; 
    for (uint32_t col = 0; col < columns; ++col) { 
        for (int row = 0; row < rows; ++row) { 
            int srcIdx = row * columns + col; 
            if (srcIdx < static_cast<int>(data.size())) { 
                output[idx++] = data[srcIdx]; 
            } else { 
                output[idx++] = ' ';  
            } 
        } 
    } 
    return output; 
} 
 
// Реализация внутренней логики дешифрования
vector<uint8_t> scytaleDecrypt(const vector<uint8_t>& data, uint32_t columns) { 
    if (columns <= 1 || data.empty()) return data; 
     
    int rows = data.size() / columns; 
    vector<uint8_t> output(data.size(), ' '); 
     
    int idx = 0; 
    for (int row = 0; row < rows; ++row) { 
        for (uint32_t col = 0; col < columns; ++col) { 
            int srcIdx = col * rows + row; 
            if (srcIdx < static_cast<int>(data.size())) { 
                output[idx++] = data[srcIdx]; 
            } 
        } 
    } 
    return output; 
} 
 
// Реализация интерфейса плагина Скиталы
const char* getName() { 
    return "Scytale Cipher"; 
} 
 
void encrypt(const vector<uint8_t>& inputData, vector<uint8_t>& outputData, uint32_t columns) { 
    outputData = scytaleEncrypt(inputData, columns); 
} 
 
void decrypt(const vector<uint8_t>& inputData, vector<uint8_t>& outputData, uint32_t columns) { 
    outputData = scytaleDecrypt(inputData, columns); 
}