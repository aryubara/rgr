#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include "LibraryManager.hpp"

using namespace std;

// Наш enum class для главного меню
enum class MenuOption {
SelectXor = 1,
SelectScytale,
Exit
};

// Наш enum class для выбора операции
enum class CipherMode {
Encrypt = 1,
Decrypt,
GenerateKey
};

const string SECRET_MARK = "NARANOVA";

void clearInputBuffer() {
cin.clear();
while (cin.get() != '\n');
}

vector<unsigned char> readBinaryFile(const string& path) {
ifstream inFile(path, ios::binary | ios::ate);
if (!inFile) {
cerr << "Ошибка: Не удалось открыть файл по указанному пути!\n";
return vector<unsigned char>();
}
streamsize size = inFile.tellg();
inFile.seekg(0, ios::beg);

vector<unsigned char> buffer(size);
if (!inFile.read((char*)buffer.data(), size)) {
cerr << "Ошибка чтения файла!\n";
return vector<unsigned char>();
}
inFile.close();
return buffer;
}

int main() {
LibraryManager manager;
string userStrChoice;

while (true) {
cout << "\n=== Encryption Algorithm RGR ===\n";
cout << "1. Подключить алгоритм XOR (.so)\n";
cout << "2. Подключить алгоритм Скитала (.so)\n";
cout << "3. Выход из программы\n";
cout << "Выберите команду: ";

cin >> userStrChoice;
if (userStrChoice != "1" && userStrChoice != "2" && userStrChoice != "3") {
cout << "Защита ввода: Пожалуйста, выберите пункт 1, 2 или 3!\n";
clearInputBuffer();
continue;
}

int userChoice = atoi(userStrChoice.c_str());
MenuOption option = static_cast<MenuOption>(userChoice);
string libPath = "";

// =================================================================
// ВОТ ОН! ТОТ САМЫЙ SWITCH-CASE ПО ENUM CLASS ДЛЯ ВЫБОРА АЛГОРИТМА
// =================================================================
switch (option) {
case MenuOption::SelectXor:
libPath = "./plugins/libxor.so";
break;

case MenuOption::SelectScytale:
libPath = "./plugins/libscytale.so";
break;

case MenuOption::Exit:
cout << "Завершение работы приложения.\n";
return 0;
}
// =================================================================

if (!manager.loadLibrary(libPath)) {
cout << "Критическая ошибка: библиотека шифра не найдена.\n";
continue;
}
cout << "Модуль криптографии успешно инициализирован!\n";

cout << "\n1. Зашифровать (текст или файл)\n2. Расшифровать (текст или файл)\n3. Сгенерировать ключ\nВыберите операцию: ";
string opStrChoice;
cin >> opStrChoice;
if (opStrChoice != "1" && opStrChoice != "2" && opStrChoice != "3") {
cout << "Неверная операция!\n";
clearInputBuffer();
continue;
}

int opChoice = atoi(opStrChoice.c_str());
CipherMode mode = static_cast<CipherMode>(opChoice);

if (mode == CipherMode::GenerateKey) {
char generatedKey[256] = {0};
manager.generateKey(generatedKey, 256);
cout << "Рекомендуемый ключ для данного шифра: " << generatedKey << endl;
continue;
}

cout << "\nОткуда взять данные?\n1. Ввести текст вручную в консоли\n2. Прочитать файл (картинка/видео/текст)\nВыберите вариант: ";
string srcStrChoice;
cin >> srcStrChoice;
clearInputBuffer();

if (srcStrChoice != "1" && srcStrChoice != "2") {
cout << "Неверный источник!\n";
continue;
}

vector<unsigned char> dataBuffer;

if (srcStrChoice == "1") {
cout << "Введите ваш текст: ";
string textInput;
getline(cin, textInput);
if (textInput.empty()) continue;
dataBuffer.assign(textInput.begin(), textInput.end());
} else {
string filePath;
cout << "Введите имя файла: ";
getline(cin, filePath);
dataBuffer = readBinaryFile(filePath);
if (dataBuffer.empty()) continue;
}

cout << "Введите ключ шифрования: ";
string cipherKey;
cin >> cipherKey;
clearInputBuffer();

if (!manager.validateKey(cipherKey.c_str())) {
cout << "Некорректный ключ для выбранного алгоритма!\n";
continue;
}

if (mode == CipherMode::Encrypt) {
vector<unsigned char> combinedInput;
combinedInput.insert(combinedInput.end(), SECRET_MARK.begin(), SECRET_MARK.end());
combinedInput.insert(combinedInput.end(), dataBuffer.begin(), dataBuffer.end());

vector<unsigned char> outBuffer(combinedInput.size() + 256, ' ');

manager.encrypt(combinedInput.data(), outBuffer.data(), combinedInput.size(), cipherKey.c_str());

int finalSize = combinedInput.size();
if (option == MenuOption::SelectScytale) {
int cols = atoi(cipherKey.c_str());
finalSize = ((combinedInput.size() + cols - 1) / cols) * cols;
}

if (srcStrChoice == "1") {
string ishod(dataBuffer.begin(), dataBuffer.end());
string result((const char*)outBuffer.data(), finalSize);
cout << "\n========================================\n";
cout << "ИСХОДНЫЙ ТЕКСТ: " << ishod << endl;
cout << "РЕЗУЛЬТАТ РАБОТЫ (ЗАШИФРОВАН): " << result << endl;
cout << "========================================\n";
} else {
cout << "\nФайл успешно зашифрован в памяти!\n";
}

cout << "Сохранить полученный результат в файл? (y/n): ";
char saveChoice;
cin >> saveChoice;
clearInputBuffer();

if (saveChoice == 'y' || saveChoice == 'Y') {
string fileName;
cout << "Введите имя файла для сохранения: ";
getline(cin, fileName);

ofstream outFile(fileName, ios::binary);
if (outFile) {
outFile.write((const char*)outBuffer.data(), finalSize);
outFile.close();
cout << "Успешно! Результат сохранен в файл: " << fileName << endl;
}
}

} else {
vector<unsigned char> decryptedBuffer(dataBuffer.size() + 256, ' ');

manager.decrypt(dataBuffer.data(), decryptedBuffer.data(), dataBuffer.size(), cipherKey.c_str());

string checkMark((const char*)decryptedBuffer.data(), SECRET_MARK.size());

if (checkMark != SECRET_MARK) {
cout << "\n------------------------------------------------------------\n";
cout << " ОШИБКА: Введен неверный ключ шифрования!\n";
cout << "Расшифрование прервано.\n";
cout << "------------------------------------------------------------\n";
continue;
}

vector<unsigned char> cleanData(decryptedBuffer.begin() + SECRET_MARK.size(), decryptedBuffer.begin() + dataBuffer.size());

if (srcStrChoice == "1") {
string result((const char*)cleanData.data(), cleanData.size());
cout << "\n========================================\n";
cout << "КЛЮЧ НАЙДЕН! ДАННЫЕ УСПЕШНО ДЕКОДИРОВАНЫ.\n";
cout << "РЕЗУЛЬТАТ РАБОТЫ: " << result << endl;
cout << "========================================\n";
} else {
cout << "\nКЛЮЧ НАЙДЕН! Файл успешно расшифрован в памяти.\n";
}

cout << "Сохранить расшифрованный результат в файл? (y/n): ";
char saveChoice;
cin >> saveChoice;
clearInputBuffer();

if (saveChoice == 'y' || saveChoice == 'Y') {
string fileName;
cout << "Введите имя файла для сохранения: ";
getline(cin, fileName);

ofstream outFile(fileName, ios::binary);
if (outFile) {
outFile.write((const char*)cleanData.data(), cleanData.size());
outFile.close();
cout << "Успешно! Результат сохранен в файл: " << fileName << endl;
}
}
}
}
return 0;
}

