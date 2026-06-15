#include "fileManager.h"
#include "cryptoMath.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;


//ЧТЕНИЕ ФАЙЛА
vector<uint8_t> readFile(const string& path) {

    //Блок обработки ошибок
    if (path.empty()) {
        throw runtime_error("Путь к файлу не указан");
    }

    if (!filesystem::exists(path)) {
        throw runtime_error("Файл не существует");
    }

    if (!filesystem::is_regular_file(path)) {
        throw runtime_error("Указанный путь не является файлом");
    }

    ifstream file(path, ios::binary); //Открытие файла в бинарном режиме

    if (!file.is_open()) {
        throw runtime_error("Не удалось открыть файл");
    }

    file.seekg(0, ios::end); //Перемещение указателя чтения в конец файла

    streamsize size = file.tellg(); //Получение размера файла в байтах

    if (size < 0) {
        throw runtime_error("Не удалось определить размер файла");
    }

    file.seekg(0, ios::beg); //Возврат указателя чтения в начало файла

    vector<uint8_t> inputData(size); //Вектор для хранения байтов файла

    if (size > 0) {
        file.read((char*)(inputData.data()), size); //Заполнение вектора байтами из файла

        if (!file) {
            throw runtime_error("Ошибка чтения файла");
        }
    }
    return inputData;
}


//ЗАПИСЬ ФАЙЛА
void writeFile(const string& path, const vector<uint8_t>& inputData) {

    //Блок обработки ошибок
    if (path.empty()) {
        throw runtime_error("Путь для сохранения не указан");
    }

    ofstream file(path, ios::binary); //Создание (или открытие) файла

    if (!file.is_open()) {
        throw runtime_error("Не удалось создать выходной файл");
    }

    if (!inputData.empty()) {
        file.write(reinterpret_cast<const char*>(inputData.data()), inputData.size()); //Запись всех байтов в файл

        if (!file) {
            throw runtime_error("Ошибка записи файла");
        }
    }
}


//РАБОТА С ВВОДОМ

void clearInput() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

bool inputNumber(int& val, const string& prompt, int minVal, int maxVal) {
    cout << prompt;
    cin >> val;

    if (cin.fail() || val < minVal || val > maxVal) {
        clearInput();
        cout << "Ошибка: введите число от " << minVal << " до " << maxVal << "!\n";
        return false;
    }
    clearInput();
    return true;
}

bool inputMode(int& modeChoice) {
    return inputNumber(modeChoice, "\n1 - Ввод текста\n2 - Работа с файлом\nВыберите режим шифрования: ", 1, 2);
}

bool inputKeyMode(int& keyMode, const string& manualText, const string& generateText) {
    return inputNumber(keyMode, "\n1 - " + manualText + "\n2 - " + generateText + "\nВыберите режим: ", 1, 2);
}

bool inputTextData(vector<uint8_t>& inputData) {
    string text;
    cout << "Введите текст: ";
    getline(cin, text);
    inputData.assign(text.begin(), text.end());
    return true;
}

bool inputFilePath(string& path) {
    cout << "Введите путь к файлу: ";
    getline(cin, path);

    if (path.empty()) {
        cout << "Ошибка: путь не может быть пустым!\n";
        return false;
    }
    if (!fileExists(path)) {
        cout << "Ошибка: файл не найден!\n";
        return false;
    }
    return true;
}

bool validateKey(const string& key) {
    if (key.empty()) {
        cout << "Ошибка: ключ не может быть пустым!\n";
        return false;
    }
    return true;
}

bool inputXorKey(string& key) {
    cout << "Введите ключ: ";
    getline(cin, key);
    return validateKey(key);
}

bool inputColumns(uint32_t& cols) {
    int value;
    if (!inputNumber(value, "Введите количество столбцов (2-100): ", 2, 100)) {
        return false;
    }
    cols = value;
    return true;
}

bool inputHex(uint32_t& val, const string& prompt) {
    string input;
    cout << prompt;
    cin >> input;

    if (cin.fail() || input.empty()) {
        clearInput();
        cout << "Ошибка: поле не может быть пустым!\n";
        return false;
    }
    clearInput();

    for (char c : input) {
        if (!isxdigit(static_cast<unsigned char>(c))) {
            cout << "Ошибка: недопустимый hex-символ '" << c << "'!\n";
            return false;
        }
    }
    if (input.length() > 8) {
        cout << "Ошибка: максимум 8 hex-символов!\n";
        return false;
    }
    val = stoul(input, nullptr, 16);
    return true;
}

bool inputAffineKey(uint32_t& a, uint32_t& b) {
    int value;
    if (!inputNumber(value, "Введите ключ a: ", 1, 255)) {
        return false;
    }
    a = value;
    if (gcd(a, 256) != 1) {
        cout << "Ошибка: a должно быть взаимно простым с 256!\n";
        return false;
    }
    if (!inputNumber(value, "Введите ключ b: ", 0, 255)) {
        return false;
    }
    b = value;
    return true;
}

bool inputTeaKey(uint32_t key[4]) {
    cout << "Введите 4 части ключа:\n";
    for (int i = 0; i < 4; i++) {
        if (!inputHex(key[i], "key[" + to_string(i) + "] = ")) {
            return false;
        }
    }
    return true;
}

bool inputIdeaKey(uint16_t key[8]) {
    cout << "Введите 8 частей ключа:\n";
    for (int i = 0; i < 8; i++) {
        int value;
        if (!inputNumber(value, "key[" + to_string(i) + "] = ", 0, 65535)) {
            return false;
        }
        key[i] = value;
    }
    return true;
}

bool inputChaCha20Key(uint32_t key[8], uint32_t nonce[3]) {
    cout << "Введите 8 частей ключа (HEX):\n";
    for (int i = 0; i < 8; i++) {
        if (!inputHex(key[i], "key[" + to_string(i) + "] = ")) {
            return false;
        }
    }
    cout << "Введите 3 части nonce (HEX):\n";
    for (int i = 0; i < 3; i++) {
        if (!inputHex(nonce[i], "nonce[" + to_string(i) + "] = ")) {
            return false;
        }
    }
    return true;
}

bool fileExists(const string& path) {
    return filesystem::exists(path);
}