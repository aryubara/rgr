#include "fileManager.h"
#include "LibraryManager.hpp"

using namespace std;

vector<uint8_t> readFile(const string& path) { // ЧТЕНИЕ ФАЙЛА
    //Блок обработки ошибок
    if (path.empty()) throw runtime_error("Путь к файлу не указан");
    if (!filesystem::exists(path)) throw runtime_error("Файл не существует");
    if (!filesystem::is_regular_file(path)) throw runtime_error("Указанный путь не является файлом");

    ifstream file(path, ios::binary); // Открытие файла в бинарном режиме
    if (!file.is_open()) throw runtime_error("Не удалось открыть файл");

    file.seekg(0, ios::end); // Перемещение указателя чтения в конец файла
    streamsize size = file.tellg(); // Получение размера файла в байтах
    if (size < 0) throw runtime_error("Не удалось определить размер файла");
    file.seekg(0, ios::beg); // Возврат указателя чтения в начало файла

    vector<uint8_t> inputData(size); // Вектор для хранения байтов файла
    if (size > 0) {
        file.read((char*)(inputData.data()), size); // Заполнение вектора байтами из файла
        if (!file) throw runtime_error("Ошибка чтения файла");
    }
    return inputData;
}
void writeFile(const string& path, const vector<uint8_t>& inputData) { // ЗАПИСЬ ФАЙЛА
    //Блок обработки ошибок
    if (path.empty()) throw runtime_error("Путь для сохранения не указан");

    ofstream file(path, ios::binary); // Создание (или открытие) файла
    if (!file.is_open()) throw runtime_error("Не удалось создать выходной файл");

    if (!inputData.empty()) {
        file.write(reinterpret_cast<const char*>(inputData.data()), inputData.size()); //Запись всех байтов в файл
        if (!file) throw runtime_error("Ошибка записи файла");
    }
}

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

bool inputColumns(uint32_t& cols) {
    cout << "Введите количество столбцов (2-100): ";
    cin >> cols;
    if (cin.fail() || cols < 2 || cols > 100) {
        clearInput();
        cout << "Ошибка: столбцов должно быть от 2 до 100!\n";
        return false;
    }
    clearInput();
    return true;
}

bool inputHex(uint32_t& val, const string& prompt) {
    string input;
    cout << prompt;
    cin >> input;
    
    if (input.empty()) {
        cout << "Ошибка: поле не может быть пустым!\n";
        return false;
    }
    for (char c : input) {
        if (!isxdigit(static_cast<unsigned char>(c))) {
            cout << "Ошибка: недопустимый hex-символ '" << c << ". Используйте 0-9, A-F\n";
            return false;
        }
    }
    if (input.length() > 8) {
        cout << "Ошибка: слишком длинное число! Максимум 8 hex-цифр\n";
        return false;
    }
    val = stoul(input, nullptr, 16);
    return true;
}

bool inputSerpentKey(uint32_t key[8]) {
    cout << "Введите 8 частей ключа (32 байта) в HEX:\n";
    for (int i = 0; i < 8; i++) {
        if (!inputHex(key[i], "key[" + to_string(i) + "] = ")) return false;
    }
    return true;
}

bool inputChaCha20Key(uint32_t key[8], uint32_t nonce[3]) {
    cout << "Введите 8 частей ключа (32 байта) в HEX:\n";
    for (int i = 0; i < 8; i++) {
        if (!inputHex(key[i], "key[" + to_string(i) + "] = ")) return false;
    }
    cout << "Введите 3 части nonce (12 байт) в HEX:\n";
    for (int i = 0; i < 3; i++) {
        if (!inputHex(nonce[i], "nonce[" + to_string(i) + "] = ")) return false;
    }
    return true;
}

bool fileExists(const string& path) {
    return filesystem::exists(path);
}

bool validateKey(const string& key) {
    if (key.empty()) {
        cout << "Ошибка: ключ не может быть пустым!\n";
        return false;
    }
    return true;
}