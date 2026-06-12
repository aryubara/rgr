#include "affine.h"
#include "Utilities/cryptoMath.h"

#include <vector>
#include <cstdint>
#include <stdexcept>

using namespace std;


//ШИФРОВАНИЕ
vector<uint8_t> affineEncrypt(const vector<uint8_t>& data, uint32_t a, uint32_t b) {
    if (gcd(a, m) != 1) { //a и m - взаимно простые 
        throw invalid_argument("Некорректный ключ a");
    }

    vector<uint8_t> result; //создание вектора
    result.reserve(data.size()); //задание размера

    for (uint8_t byte : data) { //шифрование байт
        uint32_t encrypted = (a * byte + b) % m; // Y = (aX + b) mod m
        result.push_back(static_cast<uint8_t>(encrypted));
    }
    return result;
}


//ДЕШИФРОВАНИЕ
vector<uint8_t> affineDecrypt(const vector<uint8_t>& data, uint32_t a, uint32_t b) {
    if (gcd(a, m) != 1) { //a и m - взаимно простые 
        throw invalid_argument("Некорректный ключ a");
    }

    uint32_t aInverse = modInverse(a, m); //a^(-1) обратный к a по модулю m

    vector<uint8_t> result; //создание вектора
    result.reserve(data.size());//задание размера

    for (uint8_t byte : data) { //дешифрование байтов
        uint32_t decrypted = (aInverse * ((byte + m - (b % m)) % m)) % m; //(a^(-1))*(Y - b) mod m
                            //защита от отрицательных чисел

        result.push_back(static_cast<uint8_t>(decrypted));
    }
    return result;
}