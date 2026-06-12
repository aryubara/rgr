#include "tea.h"

using namespace std;

//ШИФРОВАНИЕ БЛОКА
void encryptBlock(uint32_t& v0, uint32_t& v1, const uint32_t key[4]) {

    uint32_t sum = 0; //начальное значени суммы раундов при шифровании

    for (uint32_t i = 0; i < ROUNDS; i++) {
        sum += DELTA; //sum = sum + delta

        //шифрование половин блока
        v0 += ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]); 
        v1 += ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]); 
    }
}

//ДЕШИФРОВАНИЕ БЛОКА
void decryptBlock(uint32_t& v0, uint32_t& v1, const uint32_t key[4]) {

    uint32_t sum = DELTA * ROUNDS; //начальное значение суммы раундов при дешифровании 

    for (uint32_t i = 0; i < ROUNDS; i++) {

        //дешифрование половин блока    
        v1 -= ((v0 << 4) + key[2]) ^ (v0 + sum) ^ ((v0 >> 5) + key[3]); //
        v0 -= ((v1 << 4) + key[0]) ^ (v1 + sum) ^ ((v1 >> 5) + key[1]); //

        sum -= DELTA; //sum = sum - delta
    }
}

//ДОБАВЛЕНИЕ СЛУЖЕБНЫХ БАЙТОВ ДО КРАТНОСТИ
vector<uint8_t> addPadding(vector<uint8_t> inputData) {

    uint8_t padding = BLOCK_SIZE - (inputData.size() % BLOCK_SIZE); //определение недостающих байтов

    if (padding == 0) {
        padding = BLOCK_SIZE;
    }

    for (uint8_t i = 0; i < padding; i++) { //добавление байтов
        inputData.push_back(padding);
    }
    return inputData;
}

//УДАЛЕНИЕ СЛУЖЕБНЫХ БАЙТОВ
void removePadding(vector<uint8_t>& inputData) {

    if (inputData.empty()) {
        return;
    }

    uint8_t padding = inputData.back(); //получение последнего байта, содержащего количество добавленных байтов
    if (padding == 0 || padding > BLOCK_SIZE) {
        return;
    }

    for (uint8_t i = 0; i < padding; i++) { //проверка, что элемент

        if (inputData[inputData.size() - 1 - i] != padding) { //добавлен с помощью padding
            return;
        }
    }

    inputData.resize(inputData.size() - padding); //удаление добавленных служебных байтов 
}


//ШИФРОВАНИЕ
vector<uint8_t> teaEncrypt(const vector<uint8_t>& inputData, const uint32_t key[4]) {

    vector<uint8_t> result = addPadding(inputData); //добавление до кратности

    for (size_t i = 0; i < result.size(); i += BLOCK_SIZE) { //обработка блоков

        uint32_t block[2] = {}; //блок делится на v0 и v1 

        for (uint32_t part = 0; part < 2; part++) { //обработка v0 и v1

            for (uint32_t byte = 0; byte < 4; byte++) { 

                //Получение 4 байт в одно 32-битное число, сдвигая каждый байт на своё место
                block[part] |= static_cast<uint32_t>(result[i + part * 4 + byte] << (byte * 8)); 
            }
        }

        encryptBlock(block[0], block[1], key); //шифрование блока

        for (uint32_t part = 0; part < 2; part++) { //преобразование 32-битных чисел в байты

            for (uint32_t byte = 0; byte < 4; byte++) {

                //Извлечение зашифрованных uint32_t обратно на байты и запись в результат
                result[i + part * 4 + byte] = static_cast<uint8_t>(block[part] >> (byte * 8));
            }
        }
    }
    return result;
}


//ДЕШИФРОВАНИЕ
vector<uint8_t> teaDecrypt(const vector<uint8_t>& cipherData, const uint32_t key[4]) {

    vector<uint8_t> result = cipherData;

    for (size_t i = 0; i < result.size(); i += BLOCK_SIZE) { //обработка зашифрованных блоков

        uint32_t block[2] = {};

        for (uint32_t part = 0; part < 2; part++) { //разделение на блоки

            for (uint32_t byte = 0; byte < 4; byte++) { 

                block[part] |= static_cast<uint32_t>(result[i + part * 4 + byte]) << (byte * 8);
            }
        }

        decryptBlock(block[0], block[1], key); //дешифрование блока

        for (uint32_t part = 0; part < 2; part++) { //преобразование блоков

            for (uint32_t byte = 0; byte < 4; byte++) {

                //Извлечение расшифрованных uint32_t обратно на байты и запись в результат
                result[i + part * 4 + byte] = static_cast<uint8_t>(block[part] >> (byte * 8));
            }
        }
    }
    removePadding(result); //удаление служебных байтов

    return result;
}