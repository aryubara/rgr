#include "ChaCha20.h"
#include <vector>
#include <iostream>
#include <cstdint>
#include <iomanip>
#include <fstream>

void quarterRound(uint32_t &a, uint32_t &b, uint32_t &c, uint32_t &d) { // получаем случайные байты 
    a += b; d ^= a; d = ROTL(d, 16);
    c += d; b ^= c; b = ROTL(b, 12);
    a += b; d ^= a; d = ROTL(d, 8);
    c += d; b ^= c; b = ROTL(b, 7);
}

void columnRound(vector<uint32_t>& state) { // перемешивание по колонкам
    for (int i = 0; i < 4; ++i) { // по 4 колонки
        quarterRound(
            state[i], // a
            state[i + 4], // b
            state[i + 8], // c
            state[i + 12] // d
        );
    }
}

void diagonalRound(vector<uint32_t>& state) { // перемешивание по диагонали
    quarterRound(state[0], state[5], state[10], state[15]);
    quarterRound(state[1], state[6], state[11], state[12]);
    quarterRound(state[2], state[7], state[8], state[13]);
    quarterRound(state[3], state[4], state[9], state[14]);
}

void chacha20block(vector<uint32_t>& state) {
    vector<uint32_t> copy_state = state;
    for (int i = 0; i < 10; ++ i) {
        columnRound(copy_state);
        diagonalRound(copy_state);
    }
    for (size_t i = 0; i < state.size(); ++i) {
        state[i] += copy_state[i]; 
    }
}

void initState(vector<uint32_t>& state, const uint32_t key_words[8], uint32_t counter, uint32_t nonce[3]) {
    state[0] = 0x61707865; state[1] = 0x3320646e; 
    state[2] = 0x79622d32; state[3] = 0x6b206574;
    for (int i = 0; i < 8; ++i) state[4 + i] = key_words[i];
    state[12] = counter;
    for (int i = 0; i < 3; ++i) state[i + 13] = nonce[i];
}

void chacha20_encrypt(vector <uint8_t>& plaintext, const uint32_t key_words[8], uint32_t nonce[3], 
                        vector<uint8_t>& ciphertext) {

    ciphertext.resize(plaintext.size());
    uint32_t counter = 0;
    vector<uint32_t> state(16); // используем безнаковые 32бит числа и линейную матрицу

    for (size_t offset = 0; offset < plaintext.size(); offset += 64) {
        initState(state, key_words, counter, nonce);
        chacha20block(state);

        uint8_t keystream_bytes[64];
        for (int i = 0; i < 16; ++i) { // преобразование 16 чисел в 64 байта
            keystream_bytes[4*i + 0] = (state[i] >> 0) & 0xFF; keystream_bytes[4*i + 1] = (state[i] >> 8) & 0xFF;
            keystream_bytes[4*i + 2] = (state[i] >> 16) & 0xFF; keystream_bytes[4*i + 3] = (state[i] >> 24) & 0xFF;
        }
        size_t block_size = min<size_t>(64, plaintext.size() - offset);
        for (size_t i = 0; i < block_size; ++i) {
            ciphertext[offset + i] = plaintext[offset + i] ^ keystream_bytes[i];
        }
        counter++;
    }
}

void encryptFile(const string& inputFile, const string& outputFile,
                 const uint32_t key_words[8], uint32_t nonce[3]) {
    
    ifstream in(inputFile, ios::binary);
    
    if (!in) {
        cerr << "Ошибка: файл " << inputFile << " не найден!" << endl;
        return;
    }
    
    vector<uint8_t> plaintext((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();
    
    cout << "Размер файла: " << plaintext.size() << " байт" << endl;

    vector<uint8_t> ciphertext; 
    chacha20_encrypt(plaintext, key_words, nonce, ciphertext);

    ofstream out(outputFile, ios::binary);
    out.write(reinterpret_cast<char*>(ciphertext.data()), ciphertext.size());
    out.close();

    cout << "Зашифровано -> " << outputFile << endl;    
}

void decryptFile(const string& inputFile, const string& outputFile, const uint32_t key_words[8], uint32_t nonce[3]) {
    
    ifstream in(inputFile, ios::binary);
    
    if (!in) {
        cerr << "Ошибка: файл " << inputFile << " не найден!" << endl;
        return;
    }
    
    vector<uint8_t> ciphertext((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    in.close();
    
    vector<uint8_t> plaintext;
    chacha20_encrypt(ciphertext, key_words, nonce, plaintext);
    
    ofstream out(outputFile, ios::binary);
    out.write(reinterpret_cast<char*>(plaintext.data()), plaintext.size());
    out.close();
    
    cout << "Расшифровано -> " << outputFile << endl;
}

void inputChaCha20() {
    cout << "   CHACHA20 ШИФРОВАНИЕ" << endl;
    cout << endl;

    // ========== АВТОГЕНЕРАЦИЯ КЛЮЧА И NONCE ==========
    cout << "--- ГЕНЕРАЦИЯ КЛЮЧА И NONCE ---" << endl;

    // Генерация случайного ключа (32 байта = 256 бит)
    uint32_t key_words[8];
    srand((unsigned int)time(nullptr));

    // Генерируем 32 байта ключа
    uint8_t key_bytes[32];
    for (int i = 0; i < 32; i++) {
        key_bytes[i] = rand() & 0xFF;
    }

    // Преобразуем в uint32_t слова
    for (int i = 0; i < 8; i++) {
        key_words[i] = (key_bytes[i*4] << 24) | (key_bytes[i*4+1] << 16) | 
                    (key_bytes[i*4+2] << 8) | key_bytes[i*4+3];
    }

    // Генерация случайного nonce (12 байт)
    uint32_t nonce[3];
    uint8_t nonce_bytes[12];
    for (int i = 0; i < 12; i++) {
        nonce_bytes[i] = rand() & 0xFF;
    }

    // Преобразуем в uint32_t слова
    for (int i = 0; i < 3; i++) {
        nonce[i] = (nonce_bytes[i*4] << 24) | (nonce_bytes[i*4+1] << 16) | 
                (nonce_bytes[i*4+2] << 8) | nonce_bytes[i*4+3];
    }

    // ========== ВЫВОД СГЕНЕРИРОВАННЫХ КЛЮЧЕЙ ==========
    cout << "Ключ (64 HEX): ";
    for (int i = 0; i < 32; i++) {
        printf("%02X", key_bytes[i]);
    }
    cout << endl;

    cout << "Nonce (24 HEX): ";
    for (int i = 0; i < 12; i++) {
        printf("%02X", nonce_bytes[i]);
    }
    cout << endl;

    // Сохранение ключа и nonce в файл
    string keyfile = "chacha20_key.txt";
    ofstream keyFile(keyfile);
    if (keyFile.is_open()) {
        keyFile << "# ChaCha20 Key and Nonce" << endl;
        keyFile << "# Сохраните для расшифровки" << endl;
        keyFile << endl;
        keyFile << "KEY: ";
        for (int i = 0; i < 32; i++) {
            keyFile << hex << uppercase << setw(2) << setfill('0') << (int)key_bytes[i];
        }
        keyFile << endl;
        keyFile << "NONCE: ";
        for (int i = 0; i < 12; i++) {
            keyFile << hex << uppercase << setw(2) << setfill('0') << (int)nonce_bytes[i];
        }
        keyFile << endl;
        keyFile.close();
        cout << "\nКлюч и nonce сохранены в файл: " << keyfile << endl;
    } else {
        cerr << "Предупреждение: не удалось сохранить ключ в файл" << endl;
    }

    cin.ignore();

    string filename;
    cout << "\nВведите имя файла для шифрования: ";
    getline(cin, filename);
    ifstream checkFile(filename);
    if (!checkFile.is_open()) {
        cerr << "\nОШИБКА: Файл '" << filename << "' не найден!" << endl;
        cerr << "Проверьте правильность имени файла и пути." << endl;
        cout << "\nНажмите Enter для выхода...";
        cin.get();
        return; // или exit(1) если в main
    }
    checkFile.close();

    // ========== ШИФРОВАНИЕ ==========
    cout << "\n--- ШИФРОВАНИЕ ---" << endl;
    string encryptedFile = filename + ".encrypted";
    encryptFile(filename, encryptedFile, key_words, nonce);
    cout << "Зашифрованный файл: " << encryptedFile << endl;

    // ========== ВЫВОД HEX ЗАШИФРОВАННОГО ФАЙЛА ==========
    cout << "\n--- ШИФРОТЕКСТ (HEX) ---" << endl;
    ifstream encFile(encryptedFile, ios::binary);
    if (encFile.is_open()) {
        vector<unsigned char> buffer((istreambuf_iterator<char>(encFile)), istreambuf_iterator<char>());
        encFile.close();
        
        for (size_t i = 0; i < buffer.size() && i < 256; i++) {
            printf("%02X", buffer[i]);
            if ((i + 1) % 32 == 0) cout << endl;
        }
        if (buffer.size() > 256) cout << "... (показано 256 байт)";
        cout << endl;
    }

    // ========== РАСШИФРОВКА ==========
    cout << "\n--- РАСШИФРОВКА ---" << endl;
    string decryptedFile = "decrypted_" + filename;
    decryptFile(encryptedFile, decryptedFile, key_words, nonce);

    // ========== ВЫВОД РАСШИФРОВАННОГО ТЕКСТА (НЕ HEX) ==========
    cout << "\n--- РАСШИФРОВАННЫЙ ТЕКСТ ---" << endl;
    ifstream decFile(decryptedFile);
    if (decFile.is_open()) {
        string content((istreambuf_iterator<char>(decFile)), istreambuf_iterator<char>());
        decFile.close();
        cout << content << endl;
    }

    // ========== СРАВНЕНИЕ ==========
    cout << "\n--- РЕЗУЛЬТАТ ---" << endl;
    cout << "Исходный файл:      " << filename << endl;
    cout << "Зашифрованный:      " << encryptedFile << endl;
    cout << "Расшифрованный:     " << decryptedFile << endl;

    // Проверка соответствия
    ifstream original(filename);
    ifstream decrypted(decryptedFile);

    if (original.is_open() && decrypted.is_open()) {
        string origContent((istreambuf_iterator<char>(original)), istreambuf_iterator<char>());
        string decContent((istreambuf_iterator<char>(decrypted)), istreambuf_iterator<char>());
        
        original.close();
        decrypted.close();
        
        if (origContent == decContent) {
            cout << "\nРАСШИФРОВАННЫЙ ТЕКСТ СООТВЕТСТВУЕТ ИСХОДНОМУ" << endl;
        } else {
            cout << "\nОШИБКА: Расшифрованный текст НЕ совпадает с исходным!" << endl;
        }
    }
}