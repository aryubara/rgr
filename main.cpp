#include "LibraryManager.hpp"
#include "Utilities/fileManager.h"
#include "Utilities/cryptoMath.h"
#include "Utilities/crypto_utils.h"

#include<iostream>
#include<vector>
#include<string>
#include<cstdint>
#include<filesystem>
#include<random>


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
        cout << "\n1 - XOR\n";
        cout << "2 - Шифр Скитала\n";
        cout << "3 - Аффинный шифр\n";
        cout << "4 - TEA\n";
        cout << "5 - Serpent\n";
        cout << "6 - ChaCha20\n";
        cout << "0 - Выход\n";
        cout << "Выберите алгоритм: ";

        inputNumber(choice, "Выберите алгоритм: ", 0, 6);

        switch (static_cast<Menu>(choice)) {
            case Menu::XOR: {

                if (!manager.loadLibrary("./libXOR.so")) {
                    cout << "Алгоритм недоступен\n";
                    break;
                }

                int modeChoice;

                inputMode(modeChoice);

                int keyMode;

                inputKeyMode(keyMode, "Ввести ключ вручную", "Сгенерировать ключ");

                string key;

                if (keyMode == 1) {

                    inputXorKey(key);

                } else {

                    random_device rd;
                    mt19937 generator(rd());
                    uniform_int_distribution<int> dist(33, 126);

                    key.clear();

                    for (int i = 0; i < 16; i++) {
                        key += static_cast<char>(dist(generator));
                    }
                    cout << "\nСгенерированный ключ: " << key << '\n';
                }

                switch (static_cast<Mode>(modeChoice)) {

                    case Mode::TEXT: {

                        vector<uint8_t> inputData;

                        inputTextData(inputData);

                        try {

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.xorEncrypt(inputData, encryptedData, key.c_str());

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

                        } catch (const exception& error) {

                            cout << "Ошибка: " << error.what() << '\n';
                        }

                        break;
                    }

                    case Mode::FILE: {

                        string inputPath;

                        inputFilePath(inputPath);

                        try {

                            vector<uint8_t> inputData = readFile(inputPath);

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.xorEncrypt(inputData, encryptedData, key.c_str());

                            filesystem::path path(inputPath);

                            string encryptedPath = "encrypted_" + path.filename().string();

                            writeFile(encryptedPath, encryptedData);

                            manager.xorDecrypt(encryptedData, decryptedData, key.c_str());

                            string decryptedPath = "decrypted_" + path.filename().string();

                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';

                        } catch (const exception& error) {

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

                inputMode(modeChoice);

                int keyMode;

                inputKeyMode(keyMode, "Ввести количество столбцов", "Сгенерировать количество столбцов");

                uint32_t columns;

                if (keyMode == 1) {

                    inputColumns(columns);

                } else {

                    random_device rd;
                    mt19937 generator(rd());
                    uniform_int_distribution<uint32_t> dist(2, 100);

                    columns = dist(generator);

                    cout << "\nСгенерированное количество столбцов: "
                        << columns << '\n';
                }

                switch (static_cast<Mode>(modeChoice)) {

                    case Mode::TEXT: {

                        vector<uint8_t> inputData;

                        inputTextData(inputData);

                        try {

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

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

                        } catch (const exception& error) {

                            cout << "Ошибка: " << error.what() << '\n';
                        }

                        break;
                    }

                    case Mode::FILE: {

                        string inputPath;

                        inputFilePath(inputPath);

                        try {

                            vector<uint8_t> inputData = readFile(inputPath);

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.scytaleEncrypt(inputData, encryptedData, columns);

                            filesystem::path path(inputPath);

                            string encryptedPath = "encrypted_" + path.filename().string();

                            writeFile(encryptedPath, encryptedData);

                            manager.scytaleDecrypt(encryptedData, decryptedData, columns);

                            string decryptedPath = "decrypted_" + path.filename().string();

                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';

                        } catch (const exception& error) {

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

                inputMode(modeChoice);

                int keyMode;

                inputKeyMode(keyMode, "Ввести ключ вручную", "Сгенерировать ключ");

                uint32_t a, b;

                if (keyMode == 1) {

                    inputAffineKey(a, b);

                } else {

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

                switch (static_cast<Mode>(modeChoice)) {

                    case Mode::TEXT: {

                        vector<uint8_t> inputData;

                        inputTextData(inputData);

                        try {

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.affineEncrypt(inputData, encryptedData, a, b);

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

                        } catch (const exception& error) {

                            cout << "Ошибка: " << error.what() << '\n';
                        }

                        break;
                    }

                    case Mode::FILE: {

                        string inputPath;

                        inputFilePath(inputPath);

                        try {

                            vector<uint8_t> inputData = readFile(inputPath);

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.affineEncrypt(inputData, encryptedData, a, b);

                            filesystem::path path(inputPath);

                            string encryptedPath = "encrypted_" + path.filename().string();

                            writeFile(encryptedPath, encryptedData);

                            manager.affineDecrypt(encryptedData, decryptedData, a, b);

                            string decryptedPath = "decrypted_" + path.filename().string();

                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';

                        } catch (const exception& error) {

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

                inputMode(modeChoice);

                int keyMode;

                inputKeyMode(keyMode, "Ввести ключ вручную", "Сгенерировать ключ");

                uint32_t key[4];

                if (keyMode == 1) {

                    inputTeaKey(key);

                } else {

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

                switch (static_cast<Mode>(modeChoice)) {

                    case Mode::TEXT: {

                        vector<uint8_t> inputData;

                        inputTextData(inputData);

                        try {

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

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

                        } catch (const exception& error) {

                            cout << "Ошибка: " << error.what() << '\n';
                        }

                        break;
                    }

                    case Mode::FILE: {

                        string inputPath;

                        inputFilePath(inputPath);

                        try {

                            vector<uint8_t> inputData = readFile(inputPath);

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.teaEncrypt(inputData, encryptedData, key);

                            filesystem::path path(inputPath);

                            string encryptedPath = "encrypted_" + path.filename().string();

                            writeFile(encryptedPath, encryptedData);

                            manager.teaDecrypt(encryptedData, decryptedData, key);

                            string decryptedPath = "decrypted_" + path.filename().string();

                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';

                        } catch (const exception& error) {

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

                inputMode(modeChoice);

                int keyMode;

                inputKeyMode(keyMode, "Ввести ключ вручную", "Сгенерировать ключ");

                uint32_t key[8];

                if (keyMode == 1) {

                    inputSerpentKey(key);

                } else {

                    random_device rd;
                    mt19937 generator(rd());
                    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

                    for (int i = 0; i < 8; i++) {
                        key[i] = dist(generator);
                    }

                    cout << "\nСгенерированный ключ:\n";

                    for (int i = 0; i < 8; i++) {
                        printf("key[%d] = %08X\n", i, key[i]);
                    }
                }

                uint8_t keyBytes[32];

                wordsToBytesBigEndian(key, keyBytes, 8);

                switch (static_cast<Mode>(modeChoice)) {

                    case Mode::TEXT: {

                        vector<uint8_t> inputData;

                        inputTextData(inputData);

                        try {

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.serpentEncrypt(inputData, encryptedData, keyBytes);

                            manager.serpentDecrypt(encryptedData, decryptedData, keyBytes);

                            cout << "\nЗашифрованный текст:\n";

                            for (uint8_t byte : encryptedData) {
                                cout << static_cast<int>(byte) << ' ';
                            }

                            cout << "\n\nРасшифрованный текст:\n";

                            for (uint8_t byte : decryptedData) {
                                cout << static_cast<char>(byte);
                            }

                            cout << '\n';

                        } catch (const exception& error) {

                            cout << "Ошибка: " << error.what() << '\n';
                        }

                        break;
                    }

                    case Mode::FILE: {

                        string inputPath;

                        inputFilePath(inputPath);

                        try {

                            vector<uint8_t> inputData = readFile(inputPath);

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.serpentEncrypt(inputData, encryptedData, keyBytes);

                            filesystem::path path(inputPath);

                            string encryptedPath = "encrypted_" + path.filename().string() + ".serpent";

                            writeFile(encryptedPath, encryptedData);

                            manager.serpentDecrypt(encryptedData, decryptedData, keyBytes);

                            string decryptedPath = "decrypted_" + path.filename().string();

                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';

                        } catch (const exception& error) {

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

                inputMode(modeChoice);

                int keyMode;

                inputKeyMode(keyMode, "Ввести ключ и nonce вручную", "Сгенерировать ключ и nonce");

                uint32_t key[8];
                uint32_t nonce[3];

                if (keyMode == 1) {

                    !inputChaCha20Key(key, nonce);

                } else {

                    random_device rd;
                    mt19937 generator(rd());
                    uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);

                    for (int i = 0; i < 8; i++) {
                        key[i] = dist(generator);
                    }

                    for (int i = 0; i < 3; i++) {
                        nonce[i] = dist(generator);
                    }

                    cout << "\nСгенерированный ключ:\n";

                    for (int i = 0; i < 8; i++) {
                        printf("key[%d] = %08X\n", i, key[i]);
                    }

                    cout << "\nСгенерированный nonce:\n";

                    for (int i = 0; i < 3; i++) {
                        printf("nonce[%d] = %08X\n", i, nonce[i]);
                    }
                }

                uint8_t keyBytes[32];
                uint8_t nonceBytes[12];

                wordsToBytesLittleEndian(key, keyBytes, 8);
                wordsToBytesLittleEndian(nonce, nonceBytes, 3);

                switch (static_cast<Mode>(modeChoice)) {

                    case Mode::TEXT: {

                        vector<uint8_t> inputData;

                        inputTextData(inputData);

                        try {

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.chacha20Encrypt(inputData, encryptedData, keyBytes, nonceBytes);

                            manager.chacha20Decrypt(encryptedData, decryptedData, keyBytes, nonceBytes);

                            cout << "\nЗашифрованный текст:\n";

                            for (uint8_t byte : encryptedData) {
                                cout << static_cast<int>(byte) << ' ';
                            }

                            cout << "\n\nРасшифрованный текст:\n";

                            for (uint8_t byte : decryptedData) {
                                cout << static_cast<char>(byte);
                            }

                            cout << '\n';

                        } catch (const exception& error) {

                            cout << "Ошибка: " << error.what() << '\n';
                        }

                        break;
                    }

                    case Mode::FILE: {

                        string inputPath;

                        inputFilePath(inputPath);

                        try {

                            vector<uint8_t> inputData = readFile(inputPath);

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.chacha20Encrypt(inputData, encryptedData, keyBytes, nonceBytes);

                            filesystem::path path(inputPath);

                            string encryptedPath = "encrypted_" + path.filename().string() + ".chacha";

                            writeFile(encryptedPath, encryptedData);

                            manager.chacha20Decrypt(encryptedData, decryptedData, keyBytes, nonceBytes);

                            string decryptedPath = "decrypted_" + path.filename().string();

                            writeFile(decryptedPath, decryptedData);

                            cout << "\nСоздан файл:\n" << encryptedPath << '\n';
                            cout << "\nСоздан файл:\n" << decryptedPath << '\n';

                        } catch (const exception& error) {

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
                cout << "Завершение работы программы\n";
                break;

            default:
                cout << "Некорректный пункт меню!\n";
        }

    } while (choice != static_cast<int>(Menu::EXIT));

    return 0;
}