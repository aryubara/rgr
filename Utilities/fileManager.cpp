#include "fileManager.h"

#include<fstream>
#include<filesystem>
#include<stdexcept>

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
