#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include "LibraryManager.hpp"

using namespace std;

// Наш расширенный enum class для главного меню
enum class MenuOption {
SelectXor = 1,
SelectScytale,
SelectTea, // Добавили TEA
SelectAffine, // Добавили Аффинный
SelectChaCha, // Добавили ChaCha20
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
cout << "3. Подключить алгоритм TEA (Полина) (.so)\n";
cout << "4. Подключить алгоритм Аффинный (Полина) (.so)\n";
cout << "5. Подключить алгоритм ChaCha20 (Алина) (.so)\n";
cout << "6. Выход из программы\n";
cout << "Выберите команду: ";

cin >> userStrChoice;
if (userStrChoice != "1" && userStrChoice != "2" && userStrChoice != "3" &&
userStrChoice != "4" && userStrChoice != "5" && userStrChoice != "6") {
cout << "Защита ввода: Пожалуйста, выберите пункт от 1 до 6!\n";
clearInputBuffer();
continue;
}

int userChoice = atoi(userStrChoice.c_str());

// Мапим ввод пользователя на enum (пункт 6 теперь Exit)
MenuOption option;
if (userChoice == 6) {
option = MenuOption::Exit;
} else {
option = static_cast<MenuOption>(userChoice);
}

string libPath = "";

// =================================================================
// ОБНОВЛЕННЫЙ SWITCH-CASE С УЧЕТОМ ВСЕХ ШИФРОВ КОМАНДЫ
// =================================================================
switch (option) {
case MenuOption::SelectXor:
libPath = "./plugins/libxor.so";
break;

case MenuOption::SelectScytale:
libPath = "./plugins/libscytale.so";
break;

case MenuOption::SelectTea:
libPath = "./plugins/libtea.so";
break;

case MenuOption::SelectAffine:
libPath = "./plugins/libaffine.so";
break;

case MenuOption::SelectChaCha:
libPath = "./plugins/libchacha.so";
break;

case MenuOption::Exit:
cout << "Завершение работы приложения.\n";
return 0;
}
// =================================================================

if (!manager.loadLibrary(libPath)) {
cout << "Критическая ошибка: библиотека шифра не найдена по пути " << libPath << ".\n";
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

// Красивый вывод числового ключа для Аффинного шифра Полины
if (option == MenuOption::SelectAffine) {
cout << "Рекомендуемый ключ для Аффинного шифра: a = "
<< (int)((unsigned char)generatedKey[0]) << ", b = "
<< (int)((unsigned char)generatedKey[1]) << endl;
} else {
cout << "Рекомендуемый ключ для данного шифра: " << generatedKey << endl;
}
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

// Для Аффинного шифра ключ может состоять из бинарных/служебных символов,
// но getline или cin для РГР считывает строку.
cin >> cipherKey;
clearInputBuffer();

if (!manager.validateKey(cipherKey.c_str())) {
cout << "Некорректный ключ для выбранного алгоритма!\n";
if (option == MenuOption::SelectAffine) {
cout << "(Подсказка: Для Аффинного шифра код первого символа должен быть взаимно простым с m=256)\n";
}
continue;
}

if (mode == CipherMode::Encrypt) {
vector<unsigned char> combinedInput;
combinedInput.insert(combinedInput.end(), SECRET_MARK.begin(), SECRET_MARK.end());
combinedInput.insert(combinedInput.end(), dataBuffer.begin(), dataBuffer.end());

// Выделяем заведомо достаточный буфер (с запасом под Padding у блочного TEA)
vector<unsigned char> outBuffer(combinedInput.size() + 256, ' ');

manager.encrypt(combinedInput.data(), outBuffer.data(), combinedInput.size(), cipherKey.c_str());

// Вычисляем итоговый точный размер зашифрованных данных
int finalSize = combinedInput.size();

if (option == MenuOption::SelectScytale) {
int cols = atoi(cipherKey.c_str());
if (cols > 0) {
finalSize = ((combinedInput.size() + cols - 1) / cols) * cols;
}
} else if (option == MenuOption::SelectTea) {
// Блочный шифр TEA от Полины округляет размер блока вверх до кратности 8 байт
finalSize = ((combinedInput.size() + 7) / 8) * 8;
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
// Режим расшифрования
vector<unsigned char> decryptedBuffer(dataBuffer.size() + 256, ' ');

manager.decrypt(dataBuffer.data(), decryptedBuffer.data(), dataBuffer.size(), cipherKey.c_str());

// Проверяем твою метку безопасности "NARANOVA"
string checkMark((const char*)decryptedBuffer.data(), SECRET_MARK.size());

if (checkMark != SECRET_MARK) {
cout << "\n------------------------------------------------------------\n";
cout << " ОШИБКА: Введен неверный ключ шифрования!\n";
cout << "Расшифрование прервано.\n";
cout << "------------------------------------------------------------\n";
continue;
}

// Вычисляем чистый размер расшифрованных данных с учетом возможного Padding'а в TEA
int cleanSize = dataBuffer.size() - SECRET_MARK.size();

if (option == MenuOption::SelectTea) {
// Если работал TEA, в самом конце буфера (до удаления маркерной строки)
// Полина сохранила размер байт Padding'а. Нам нужно его вычесть.
// Так как decryptedBuffer содержит [SECRET_MARK][ДАННЫЕ][PADDING],
// полный размер блока после дешифрования равен dataBuffer.size().
unsigned char paddingSize = decryptedBuffer[dataBuffer.size() - 1];
if (paddingSize > 0 && paddingSize <= 8) {
cleanSize -= paddingSize;
}
}

vector<unsigned char> cleanData(decryptedBuffer.begin() + SECRET_MARK.size(),
decryptedBuffer.begin() + SECRET_MARK.size() + cleanSize);

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

