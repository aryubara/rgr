#include "input.h"

#include "Affine/affine.h"
#include "TEA/tea.h"
#include "Utilities/fileManager.h"
#include "Utilities/cryptoMath.h"

#include<iostream>
#include<vector>
#include<string>
#include<cstdint>
#include<filesystem>
#include<random>

using namespace std;

enum class Mode {
    TEXT = 1,
    FILE = 2
};

//ВЫВОД ДЛЯ АФФИННОГО ШИФРА
void inputAffine() {

    //Выбор режима
    int modeChoice;

    cout << "\n1 - Ввод текста\n";
    cout << "2 - Работа с файлом\n";
    cout << "Выберите режим: ";

    cin >> modeChoice;

    //Ввод данных
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
        return;
    }

    switch (static_cast<Mode>(modeChoice)) {

        case Mode::TEXT: { //Режим шифрования текста

            cin.ignore();

            string text;

            cout << "Введите текст: "; //Ввод текста
            getline(cin, text);

            vector<uint8_t> inputData(text.begin(), text.end()); //преобразование в массив байтов

            try { //Работа с исключениями
                vector<uint8_t> encryptedData = affineEncrypt(inputData, a, b); //Шифрование

                vector<uint8_t> decryptedData = affineDecrypt(encryptedData, a, b); //Дешифрование

                cout << "\nЗашифрованный текст:\n"; 

                for (uint8_t byte : encryptedData) { //зашифрованные байты
                    cout << static_cast<int>(byte) << ' ';
                }

                cout << "\n\nРасшифрованный текст:\n";

                for (uint8_t byte : decryptedData) { //расшифрованный текст
                    cout << static_cast<char>(byte);
                }

                cout << endl;

            } catch (const exception& error) { //обработка исключения
                cout << "Ошибка: " << error.what() << '\n'; //вывод об ошибке
            }
            break;
        }

        case Mode::FILE: { //Режим шифрования файла

            string inputPath;

            cout << "Введите путь к файлу: ";
            cin >> inputPath;

            try {
                vector<uint8_t> inputData = readFile(inputPath); //входной файл (байты)

                vector<uint8_t> encryptedData = affineEncrypt(inputData, a, b); //шифрование файла

                filesystem::path path(inputPath); //получение имени и компонентов пути

                string encryptedPath = "encrypted_" + path.filename().string(); 

                writeFile(encryptedPath, encryptedData); //запись зашифрованного файла

                vector<uint8_t> decryptedData = affineDecrypt(encryptedData, a, b); //дешифрование

                string decryptedPath = "decrypted_" + path.filename().string();

                writeFile(decryptedPath, decryptedData); //запись расшифрованного файла

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
}

//ВЫВОД ДЛЯ TEA
void inputTea() {

    //Выбор режима
    int modeChoice;

    cout << "\n1 - Ввод текста\n";
    cout << "2 - Работа с файлом\n";
    cout << "Выберите режим: ";

    cin >> modeChoice;

    //Ввод даных
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

        uniform_int_distribution<uint32_t> dist(0,UINT32_MAX);

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
        return;
    }

    switch (static_cast<Mode>(modeChoice)) {

        case Mode::TEXT: { //Режим шифрования текста
            cin.ignore();

            string text;

            cout << "Введите текст: ";
            getline(cin, text);

            vector<uint8_t> inputData(text.begin(), text.end());

            try {
                vector<uint8_t> encryptedData = teaEncrypt(inputData, key);

                vector<uint8_t> decryptedData = teaDecrypt(encryptedData, key);

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

        case Mode::FILE: { //Режим шифрования файла

            string inputPath;

            cout << "Введите путь к файлу: ";
            cin >> inputPath;

            try {
                vector<uint8_t> inputData = readFile(inputPath);

                vector<uint8_t> encryptedData = teaEncrypt(inputData, key);

                filesystem::path path(inputPath);

                string encryptedPath = "encrypted_" + path.filename().string();

                writeFile(encryptedPath, encryptedData);

                vector<uint8_t> decryptedData = teaDecrypt(encryptedData, key);

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
}