#include "LibraryManager.hpp"
#include "Utilities/fileManager.h"
#include "Utilities/cryptoMath.h"

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
    IDEA = 5,
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
        cout << "2 - Шифр Скиттла\n";
        cout << "3 - Аффинный шифр\n";
        cout << "4 - TEA\n";
        cout << "5 - IDEA\n";
        cout << "6 - ChaCha20\n";
        cout << "0 - Выход\n";
        cout << "Выберите алгоритм: ";

        cin >> choice;

        switch (static_cast<Menu>(choice)) {

            case Menu::XOR: {

                if (!manager.loadLibrary("./libXOR.so")) {
                    cout << "Алгоритм недоступен\n";
                    break;
                }

                int modeChoice;

                cout << "\n1 - Ввод текста\n";
                cout << "2 - Работа с файлом\n";
                cout << "Выберите режим: ";

                cin >> modeChoice;

                cin.ignore();

                string key;

                cout << "Введите ключ: ";
                getline(cin, key);

                switch (static_cast<Mode>(modeChoice)) {

                    case Mode::TEXT: {

                        string text;

                        cout << "Введите текст: ";
                        getline(cin, text);

                        vector<uint8_t> inputData(
                            text.begin(),
                            text.end()
                        );

                        try {

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.xorEncrypt(
                                inputData,
                                encryptedData,
                                key.c_str()
                            );

                            manager.xorDecrypt(
                                encryptedData,
                                decryptedData,
                                key.c_str()
                            );

                            cout << "\nЗашифрованный текст:\n";

                            for (uint8_t byte : encryptedData) {
                                cout << static_cast<int>(byte)
                                    << ' ';
                            }

                            cout << "\n\nРасшифрованный текст:\n";

                            for (uint8_t byte : decryptedData) {
                                cout << static_cast<char>(byte);
                            }

                            cout << '\n';
                        }
                        catch (const exception& error) {

                            cout << "Ошибка: "
                                << error.what()
                                << '\n';
                        }

                        break;
                    }

                    case Mode::FILE: {

                        string inputPath;

                        cout << "Введите путь к файлу: ";
                        cin >> inputPath;

                        try {

                            vector<uint8_t> inputData =
                                readFile(inputPath);

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.xorEncrypt(
                                inputData,
                                encryptedData,
                                key.c_str()
                            );

                            filesystem::path path(inputPath);

                            string encryptedPath =
                                "encrypted_" +
                                path.filename().string();

                            writeFile(
                                encryptedPath,
                                encryptedData
                            );

                            manager.xorDecrypt(
                                encryptedData,
                                decryptedData,
                                key.c_str()
                            );

                            string decryptedPath =
                                "decrypted_" +
                                path.filename().string();

                            writeFile(
                                decryptedPath,
                                decryptedData
                            );

                            cout << "\nСоздан файл:\n"
                                << encryptedPath
                                << '\n';

                            cout << "\nСоздан файл:\n"
                                << decryptedPath
                                << '\n';
                        }
                        catch (const exception& error) {

                            cout << "Ошибка: "
                                << error.what()
                                << '\n';
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

                cout << "\n1 - Ввод текста\n";
                cout << "2 - Работа с файлом\n";
                cout << "Выберите режим: ";

                cin >> modeChoice;

                uint32_t columns;

                cout << "Введите количество столбцов: ";
                cin >> columns;

                switch (static_cast<Mode>(modeChoice)) {

                    case Mode::TEXT: {

                        cin.ignore();

                        string text;

                        cout << "Введите текст: ";
                        getline(cin, text);

                        vector<uint8_t> inputData(text.begin(), text.end());

                        try {

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.scytaleEncrypt(
                                inputData,
                                encryptedData,
                                columns
                            );

                            manager.scytaleDecrypt(
                                encryptedData,
                                decryptedData,
                                columns
                            );

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

                            cout << "Ошибка: "
                                << error.what()
                                << '\n';
                        }

                        break;
                    }

                    case Mode::FILE: {

                        string inputPath;

                        cout << "Введите путь к файлу: ";
                        cin >> inputPath;

                        try {

                            vector<uint8_t> inputData =
                                readFile(inputPath);

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

                            manager.scytaleEncrypt(
                                inputData,
                                encryptedData,
                                columns
                            );

                            filesystem::path path(inputPath);

                            string encryptedPath =
                                "encrypted_" +
                                path.filename().string();

                            writeFile(
                                encryptedPath,
                                encryptedData
                            );

                            manager.scytaleDecrypt(
                                encryptedData,
                                decryptedData,
                                columns
                            );

                            string decryptedPath =
                                "decrypted_" +
                                path.filename().string();

                            writeFile(
                                decryptedPath,
                                decryptedData
                            );

                            cout << "\nСоздан файл:\n"
                                << encryptedPath
                                << '\n';

                            cout << "\nСоздан файл:\n"
                                << decryptedPath
                                << '\n';
                        }
                        catch (const exception& error) {

                            cout << "Ошибка: "
                                << error.what()
                                << '\n';
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

                cout << "\n1 - Ввод текста\n";
                cout << "2 - Работа с файлом\n";
                cout << "Выберите режим: ";

                cin >> modeChoice;

                uint32_t a, b;

                int keyMode;

                cout << "\n1 - Ввести ключ вручную\n";
                cout << "2 - Сгенерировать ключ\n";
                cout << "Выберите режим: ";

                cin >> keyMode;

                if (keyMode == 1) {

                    cout << "Введите ключ a: ";
                    cin >> a;

                    cout << "Введите ключ b: ";
                    cin >> b;
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

                        vector<uint8_t> inputData(text.begin(), text.end());

                        try {

                            vector<uint8_t> encryptedData;
                            vector<uint8_t> decryptedData;

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

                cout << "\n1 - Ввод текста\n";
                cout << "2 - Работа с файлом\n";
                cout << "Выберите режим: ";

                cin >> modeChoice;

                uint32_t key[4];

                int keyMode;

                cout << "\n1 - Ввести ключ вручную\n";
                cout << "2 - Сгенерировать ключ\n";
                cout << "Выберите режим: ";

                cin >> keyMode;

                if (keyMode == 1) {

                    cout << "Введите 4 части ключа:\n";

                    for (uint32_t i = 0; i < 4; i++) {

                        cout << "key[" << i << "] = ";
                        cin >> key[i];
                    }
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

                        vector<uint8_t> inputData(text.begin(), text.end());

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

            //case Menu::IDEA{}

            //case Menu::CHACHA20{}
            
            case Menu::EXIT:

                cout << "Завершение программы\n";
                break;

            default:

                cout << "Некорректный пункт меню\n";
        }

    } while (choice != static_cast<int>(Menu::EXIT));

    return 0;

}
