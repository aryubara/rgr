#include "LibraryManager.hpp"
#include "fileManager.h"
//#include "Utilities/cryptoMath.h"
#include "crypto_utils.h"
#include <iostream>
#include <random>

using namespace std;

enum class Menu {
    EXIT = 0,
    XOR = 1,
    SCYTALE = 2,
    AFFINE = 3,
    TEA = 4, 
    SERPENT = 5,
    CHACHA20 = 6
};

enum class Mode {
    TEXT = 1,
    FILE = 2
};

int main() {
    LibraryManager manager;
    int choice;
    do {
        cout << "\n1 - XOR\n2 - Шифр Скиттла\n3 - Аффинный шифр\n4 - TEA\n";
        cout << "5 - Serpent\n6 - ChaCha20\n0 - Выход\nВыберите алгоритм: ";

        cin >> choice;
        switch (static_cast<Menu>(choice)) {
            case Menu::XOR: {
                if (!manager.loadLibrary("./libXOR.so")) {
                    cout << "Алгоритм недоступен\n";
                    break;
                }
                int modeChoice;
                if (!inputNumber(modeChoice, "\n1 - Ввод текста\n2 - Работа с файлом\nВыберите режим: ", 1, 2)) { 
                    manager.unloadLibrary(); break; 
                }
                string key;
                cout << "Введите ключ: ";
                getline(cin, key);
                if (key.empty()) {
                    cout << "Ошибка: ключ не может быть пустым!\n";
                    manager.unloadLibrary();
                    break;
                }
                switch (static_cast<Mode>(modeChoice)) {
                    case Mode::TEXT: {
                        string text;
                        cout << "Введите текст: ";
                        getline(cin, text);
                        
                        if (text.empty()) {
                            cout << "Ошибка: текст не может быть пустым!\n";
                            manager.unloadLibrary();
                            break;
                        }   
                        vector<uint8_t> inputData(text.begin(), text.end());

                        try {
                            vector<uint8_t> encryptedData, decryptedData;

                            manager.xorEncrypt(inputData,encryptedData,key.c_str());
                            manager.xorDecrypt(encryptedData, decryptedData, key.c_str());
                
                            cout << "\nЗашифрованный текст:\n";
                            for (uint8_t byte : encryptedData) {
                                cout << static_cast<int>(byte) << ' ';
                            }
                            cout << "\n\nРасшифрованный текст:\n";
                            for (uint8_t byte : decryptedData) {
                                cout << static_cast<char>(byte);
                            }
                            cout << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    case Mode::FILE: {
                        string inputPath;
                        cout << "Введите путь к файлу: ";
                        cin >> inputPath;
                
                        try {
                            vector<uint8_t> inputData = readFile(inputPath);
                            vector<uint8_t> encryptedData, decryptedData;

                            manager.xorEncrypt(inputData, encryptedData, key.c_str());
                            filesystem::path path(inputPath);
                            string encryptedPath = "encrypted_" + path.filename().string();
                            writeFile(encryptedPath, encryptedData);
                            
                            manager.xorDecrypt(encryptedData, decryptedData,key.c_str());
                            string decryptedPath = "decrypted_" + path.filename().string();
                            writeFile( decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    default:
                        cout << "Некорректный режим\n";
                }
                manager.unloadLibrary();
                break;
            }
            case Menu::SCYTALE: {
                if (!manager.loadLibrary("./libScytale.so")) {
                    cout << "Алгоритм недоступен\n";
                    break;
                }
                int modeChoice;
                if (!inputNumber(modeChoice, "\n1 - Ввод текста\n2 - Работа с файлом\nВыберите режим: ", 1, 2)) {
                    manager.unloadLibrary();
                    break;
                }
                uint32_t columns;
                if (!inputColumns(columns)) {
                    manager.unloadLibrary();
                    break;
                }
                switch (static_cast<Mode>(modeChoice)) {
                    case Mode::TEXT: {
                        cin.ignore();
                        string text;
                        cout << "Введите текст: ";
                        getline(cin, text);
                        if (text.empty()) {
                            cout << "Ошибка: текст не может быть пустым!\n";
                            manager.unloadLibrary();
                            break;
                        }
                        vector<uint8_t> inputData(text.begin(), text.end());
                        try {
                            vector<uint8_t> encryptedData, decryptedData;
                            manager.scytaleEncrypt(inputData, encryptedData, columns);
                            manager.scytaleDecrypt(encryptedData, decryptedData, columns);
                            
                            cout << "\nЗашифрованный текст:\n";
                            for (uint8_t byte : encryptedData) {
                                cout << static_cast<int>(byte) << ' ';
                            }
                            cout << "\n\nРасшифрованный текст:\n";
                            for (uint8_t byte : decryptedData) {
                                cout << static_cast<char>(byte);
                            }
                            cout << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    case Mode::FILE: {
                        string inputPath;
                        cout << "Введите путь к файлу: ";
                        cin >> inputPath;

                        try {
                            vector<uint8_t> inputData = readFile(inputPath);
                            vector<uint8_t> encryptedData, decryptedData;

                            manager.scytaleEncrypt(inputData, encryptedData, columns);
                            filesystem::path path(inputPath);
                            string encryptedPath = "encrypted_" + path.filename().string();
                            writeFile(encryptedPath, encryptedData);

                            manager.scytaleDecrypt(encryptedData, decryptedData, columns);
                            string decryptedPath = "decrypted_" + path.filename().string();
                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    default:
                        cout << "Некорректный режим\n";
                }
                manager.unloadLibrary();
                break;
            }
            case Menu::AFFINE: {
                if (!manager.loadLibrary("./libAffine.so")) {
                    cout << "Алгоритм недоступен\n";
                    break;
                }
                int modeChoice;
                if (!inputNumber(modeChoice, "\n1 - Ввод текста\n2 - Работа с файлом\nВыберите режим: ", 1, 2)) {
                    manager.unloadLibrary();
                    break;
                }
                uint32_t a, b;
                int keyMode;

                cout << "\n1 - Ввести ключ вручную\n2 - Сгенерировать ключ\nВыберите режим: ";
                cin >> keyMode;

                if (keyMode == 1) {
                    cout << "Введите ключ a: ";
                    cin >> a;
                    cout << "Введите ключ b: ";
                    cin >> b;
                    clearInput();
                }
                else if (keyMode == 2) {
                    random_device rd;
                    mt19937 generator(rd());
                    uniform_int_distribution<uint32_t> keyA(1, 255);
                    uniform_int_distribution<uint32_t> keyB(0, 255);

                    do {
                        a = keyA(generator);
                    } while (gcd(a, 256) != 1);
                    b = keyB(generator);

                    cout << "\nСгенерированный ключ:\n";
                    cout << "a = " << a << '\n';
                    cout << "b = " << b << '\n';
                }
                else {
                    cout << "Некорректный режим\n";
                    manager.unloadLibrary();
                    break;
                }
                switch (static_cast<Mode>(modeChoice)) {
                    case Mode::TEXT: {
                        cin.ignore();
                        string text;
                        cout << "Введите текст: ";
                        getline(cin, text);
                        if (text.empty()) {
                            cout << "Ошибка: текст не может быть пустым!\n";
                            manager.unloadLibrary();
                            break;
                        }
                        vector<uint8_t> inputData(text.begin(), text.end());

                        try {
                            vector<uint8_t> encryptedData, decryptedData;

                            manager.affineEncrypt(inputData,encryptedData, a, b);
                            manager.affineDecrypt(encryptedData, decryptedData, a, b);

                            cout << "\nЗашифрованный текст:\n";
                            for (uint8_t byte : encryptedData) {
                                cout << static_cast<int>(byte) << ' ';
                            }
                            cout << "\n\nРасшифрованный текст:\n";
                            for (uint8_t byte : decryptedData) {
                                cout << static_cast<char>(byte);
                            }
                            cout << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    case Mode::FILE: {
                        string inputPath;
                        cout << "Введите путь к файлу: ";
                        cin >> inputPath;

                        try {
                            vector<uint8_t> inputData = readFile(inputPath);
                            vector<uint8_t> encryptedData, decryptedData;

                            manager.affineEncrypt(inputData, encryptedData, a, b);
                            filesystem::path path(inputPath);
                            string encryptedPath = "encrypted_" + path.filename().string();
                            writeFile(encryptedPath, encryptedData);

                            manager.affineDecrypt(encryptedData, decryptedData, a, b);
                            string decryptedPath = "decrypted_" + path.filename().string();
                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    default:
                        cout << "Некорректный режим\n";
                }
                manager.unloadLibrary();
                break;
            }
            case Menu::TEA: {
                if (!manager.loadLibrary("./libTEA.so")) {
                    cout << "Алгоритм недоступен\n";
                    break;
                }
                int modeChoice;
                if (!inputNumber(modeChoice, "\n1 - Ввод текста\n2 - Работа с файлом\nВыберите режим: ", 1, 2)) {
                    manager.unloadLibrary();
                    break;
                }
                uint32_t key[4];
                int keyMode;

                cout << "\n1 - Ввести ключ вручную\n2 - Сгенерировать ключ\nВыберите режим: ";
                cin >> keyMode;

                if (keyMode == 1) {
                    cout << "Введите 4 части ключа:\n";
                    for (uint32_t i = 0; i < 4; i++) {
                        cout << "key[" << i << "] = ";
                        cin >> key[i];
                    }
                    clearInput();
                }
                else if (keyMode == 2) {
                    random_device rd;
                    mt19937 generator(rd());
                    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

                    for (uint32_t i = 0; i < 4; i++) {
                        key[i] = dist(generator);
                    }
                    cout << "\nСгенерированный ключ:\n";
                    for (uint32_t i = 0; i < 4; i++) {
                        cout << "key[" << i << "] = " << key[i] << '\n';
                    }
                }
                else {
                    cout << "Некорректный режим\n";
                    manager.unloadLibrary();
                    break;
                }
                switch (static_cast<Mode>(modeChoice)) {
                    case Mode::TEXT: {
                        cin.ignore();
                        string text;
                        cout << "Введите текст: ";
                        getline(cin, text);
                        if (text.empty()) {
                            cout << "Ошибка: текст не может быть пустым!\n";
                            manager.unloadLibrary();
                            break;
                        }
                        vector<uint8_t> inputData(text.begin(), text.end());

                        try {
                            vector<uint8_t> encryptedData, decryptedData;

                            manager.teaEncrypt(inputData, encryptedData, key);
                            manager.teaDecrypt(encryptedData, decryptedData, key);

                            cout << "\nЗашифрованный текст:\n";
                            for (uint8_t byte : encryptedData) {
                                cout << static_cast<int>(byte) << ' ';
                            }
                            cout << "\n\nРасшифрованный текст:\n";
                            for (uint8_t byte : decryptedData) {
                                cout << static_cast<char>(byte);
                            }
                            cout << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    case Mode::FILE: {
                        string inputPath;
                        cout << "Введите путь к файлу: ";
                        cin >> inputPath;

                        try {
                            vector<uint8_t> inputData = readFile(inputPath);
                            vector<uint8_t> encryptedData, decryptedData;

                            manager.teaEncrypt(inputData, encryptedData, key);
                            filesystem::path path(inputPath);
                            string encryptedPath = "encrypted_" + path.filename().string();
                            writeFile(encryptedPath, encryptedData);

                            manager.teaDecrypt(encryptedData, decryptedData, key);
                            string decryptedPath = "decrypted_" + path.filename().string();
                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath<< '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    default:
                        cout << "Некорректный режим\n";
                }
                manager.unloadLibrary();
                break;
            }
            case Menu::SERPENT: {
                if (!manager.loadLibrary("./libSerpent.so")) {
                    cout << "Алгоритм недоступен\n";
                    break;
                }
                int modeChoice;
                if (!inputNumber(modeChoice, "\n1 - Ввод текста\n2 - Работа с файлом\nВыберите режим: ", 1, 2)) {
                    manager.unloadLibrary();
                    break;
                }
                uint32_t key[8];
                int keyMode;

                cout << "\n1 - Ввести ключ вручную\n2 - Сгенерировать ключ\nВыберите режим: ";
                cin >> keyMode;

                if (keyMode == 1) {
                    if (!inputSerpentKey(key)) {
                        cout << "Ошибка ввода ключа!\n";
                        manager.unloadLibrary();
                        break;
                    }
                }
                else if (keyMode == 2) {
                    random_device rd;
                    mt19937 generator(rd());
                    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

                    for (uint32_t i = 0; i < 8; i++) {
                        key[i] = dist(generator);
                    }
                    cout << "\nСгенерированный ключ:\n";
                    for (uint32_t i = 0; i < 8; i++) {
                        printf("key[%d] = %08X\n", i, key[i]);
                    }
                }
                else {
                    cout << "Некорректный режим\n";
                    manager.unloadLibrary();
                    break;
                }
                switch (static_cast<Mode>(modeChoice)) {
                    case Mode::TEXT: {
                        cin.ignore();
                        string text;
                        cout << "Введите текст: ";
                        getline(cin, text);
                        if (text.empty()) {
                            cout << "Ошибка: текст не может быть пустым!\n";
                            manager.unloadLibrary();
                            break;
                        }
                        vector<uint8_t> inputData(text.begin(), text.end());
                        try {
                            vector<uint8_t> encryptedData, decryptedData;
                            
                            uint8_t key_bytes[32]; // для Serpent ключ передаётся как uint8_t[32]
                            wordsToBytesBigEndian(key, key_bytes, 8); 
                            manager.serpentEncrypt(inputData, encryptedData, key_bytes);
                            manager.serpentDecrypt(encryptedData, decryptedData, key_bytes);

                            cout << "\nЗашифрованный текст:\n";
                            for (uint8_t byte : encryptedData) {
                                cout << static_cast<int>(byte) << ' ';
                            }
                            cout << "\n\nРасшифрованный текст:\n";
                            for (uint8_t byte : decryptedData) {
                                cout << static_cast<char>(byte);
                            }
                            cout << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    case Mode::FILE: {
                        string inputPath;
                        cout << "Введите путь к файлу: ";
                        cin >> inputPath;

                        try {
                            vector<uint8_t> inputData = readFile(inputPath);
                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            uint8_t key_bytes[32];
                            wordsToBytesBigEndian(key, key_bytes, 8); 

                            manager.serpentEncrypt(inputData, encryptedData, key_bytes);
                            filesystem::path path(inputPath);
                            string encryptedPath = "encrypted_" + path.filename().string() + ".serpent";
                            writeFile(encryptedPath, encryptedData);

                            manager.serpentDecrypt(encryptedData, decryptedData, key_bytes);
                            string decryptedPath = "decrypted_" + path.filename().string();
                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    default:
                        cout << "Некорректный режим\n";
                }
                manager.unloadLibrary();
                break;
            }
            case Menu::CHACHA20: {
                if (!manager.loadLibrary("./libChaCha20.so")) {
                    cout << "Алгоритм недоступен\n";
                    break;
                }
                int modeChoice;
                 if (!inputNumber(modeChoice, "\n1 - Ввод текста\n2 - Работа с файлом\nВыберите режим: ", 1, 2)) {
                    manager.unloadLibrary();
                    break;
                }
                uint32_t key[8], nonce[3];
                int keyMode;

                cout << "\n1 - Ввести ключ и nonce вручную\n2 - Сгенерировать ключ и nonce\nВыберите режим: ";
                cin >> keyMode;

                if (keyMode == 1) {
                    if (!inputChaCha20Key(key, nonce)) {
                        cout << "Ошибка ввода ключа или nonce!\n";
                        manager.unloadLibrary();
                        break;
                    }
                }
                else if (keyMode == 2) {
                    random_device rd;
                    mt19937 generator(rd());
                    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

                    for (uint32_t i = 0; i < 8; i++) {
                        key[i] = dist(generator);
                    }
                    for (uint32_t i = 0; i < 3; i++) {
                        nonce[i] = dist(generator);
                    }
                    cout << "\nСгенерированный ключ:\n";
                    for (uint32_t i = 0; i < 8; i++) {
                        printf("key[%d] = %08X\n", i, key[i]);
                    }
                    cout << "\nСгенерированный nonce:\n";
                    for (uint32_t i = 0; i < 3; i++) {
                        printf("nonce[%d] = %08X\n", i, nonce[i]);
                    }
                }
                else {
                    cout << "Некорректный режим\n";
                    manager.unloadLibrary();
                    break;
                }
                uint8_t key_bytes[32]; // преобразование ключа и nonce в байтовые массивы
                uint8_t nonce_bytes[12];
                wordsToBytesBigEndian(key, key_bytes, 8);     // 8 слов -> 32 байта
                wordsToBytesBigEndian(nonce, nonce_bytes, 3);

                switch (static_cast<Mode>(modeChoice)) {
                    case Mode::TEXT: {
                        cin.ignore();
                        string text;
                        cout << "Введите текст: ";
                        getline(cin, text);
                        if (text.empty()) {
                            cout << "Ошибка: текст не может быть пустым!\n";
                            manager.unloadLibrary();
                            break;
                        }
                        vector<uint8_t> inputData(text.begin(), text.end());

                        try {
                            vector<uint8_t> encryptedData, decryptedData;
                            manager.chacha20Encrypt(inputData, encryptedData, key_bytes, nonce_bytes);
                            manager.chacha20Decrypt(encryptedData, decryptedData, key_bytes, nonce_bytes);

                            cout << "\nЗашифрованный текст:\n";
                            for (uint8_t byte : encryptedData) {
                                cout << static_cast<int>(byte) << ' ';
                            }
                            cout << "\n\nРасшифрованный текст:\n";
                            for (uint8_t byte : decryptedData) {
                                cout << static_cast<char>(byte);
                            }
                            cout << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    case Mode::FILE: {
                        string inputPath;
                        cout << "Введите путь к файлу: ";
                        cin >> inputPath;

                        try {
                            vector<uint8_t> inputData = readFile(inputPath);
                            vector<uint8_t> encryptedData, decryptedData;

                            manager.chacha20Encrypt(inputData, encryptedData, key_bytes, nonce_bytes);
                            filesystem::path path(inputPath);
                            string encryptedPath = "encrypted_" + path.filename().string() + ".chacha";
                            writeFile(encryptedPath, encryptedData);

                            manager.chacha20Decrypt(encryptedData, decryptedData, key_bytes, nonce_bytes);
                            string decryptedPath = "decrypted_" + path.filename().string();
                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';
                        }
                        catch (const exception& error) {
                            cout << "Ошибка: " << error.what() << '\n';
                        }
                        break;
                    }
                    default:
                        cout << "Некорректный режим\n";
                }
                manager.unloadLibrary();
                break;
            }
            case Menu::EXIT:
                cout << "Завершение программы\n";
                break;
            default:
                cout << "Некорректный пункт меню\n";
        }
    } while (choice != static_cast<int>(Menu::EXIT));
    return 0;
}