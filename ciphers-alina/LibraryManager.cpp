#include "LibraryManager.hpp"

#include <dlfcn.h>
#include <iostream>

using namespace std;

LibraryManager::LibraryManager()
    : libraryHandle(nullptr),
      getName(nullptr),
      affineEncrypt(nullptr),
      affineDecrypt(nullptr),
      teaEncrypt(nullptr),
      teaDecrypt(nullptr),
      scytaleEncrypt(nullptr),
      scytaleDecrypt(nullptr),
      xorEncrypt(nullptr),
      xorDecrypt(nullptr),
      chacha20Encrypt(nullptr),
      chacha20Decrypt(nullptr),
      serpentEncrypt(nullptr),
      serpentDecrypt(nullptr) {
}

LibraryManager::~LibraryManager() {
    unloadLibrary();
}

bool LibraryManager::loadLibrary(const string& path) {
    unloadLibrary();
    libraryHandle = dlopen(path.c_str(), RTLD_LAZY);

    if (libraryHandle == nullptr) {
        cerr << "Ошибка загрузки библиотеки: " << dlerror() << '\n';
        return false;
    }
    getName = reinterpret_cast<GetNameFunc>(dlsym(libraryHandle, "getName"));
    if (getName == nullptr) {
        cerr << "Не удалось получить название алгоритма\n";
        unloadLibrary();
        return false;
    }
    const char* name = getName();

    if (string(name) == "Affine") {
        affineEncrypt = reinterpret_cast<AffineCipherFunc>(dlsym(libraryHandle, "encrypt"));
        affineDecrypt = reinterpret_cast<AffineCipherFunc>(dlsym(libraryHandle, "decrypt"));

        if (affineEncrypt == nullptr || affineDecrypt == nullptr) {
            cerr << "Ошибка получения функций Affine\n";
            unloadLibrary();
            return false;
        }
    }
    else if (string(name) == "TEA") {
        teaEncrypt = reinterpret_cast<TeaCipherFunc>(dlsym(libraryHandle, "encrypt"));
        teaDecrypt = reinterpret_cast<TeaCipherFunc>(dlsym(libraryHandle, "decrypt"));

        if (teaEncrypt == nullptr || teaDecrypt == nullptr) {
            cerr << "Ошибка получения функций TEA\n";
            unloadLibrary();
            return false;
        }
    }
    else if (string(name) == "Scytale Cipher") {
        scytaleEncrypt = reinterpret_cast<ScytaleCipherFunc>(dlsym(libraryHandle, "encrypt"));
        scytaleDecrypt = reinterpret_cast<ScytaleCipherFunc>(dlsym(libraryHandle, "decrypt"));

        if (scytaleEncrypt == nullptr || scytaleDecrypt == nullptr) {
            cerr << "Ошибка получения функций Scytale\n";
            unloadLibrary();
            return false;
        }
    }
    else if (string(name) == "XOR Cipher") {
        xorEncrypt = reinterpret_cast<XorCipherFunc>(dlsym(libraryHandle, "encrypt"));
        xorDecrypt = reinterpret_cast<XorCipherFunc>(dlsym(libraryHandle, "decrypt"));

        if (xorEncrypt == nullptr || xorDecrypt == nullptr) {
            cerr << "Ошибка получения функций XOR\n";
            unloadLibrary();
            return false;
        }
    }
    else if (string(name) == "ChaCha20") {
        chacha20Encrypt = reinterpret_cast<ChaCha20CipherFunc>(dlsym(libraryHandle, "encrypt"));
        chacha20Decrypt = reinterpret_cast<ChaCha20CipherFunc>(dlsym(libraryHandle, "decrypt"));

        if (chacha20Encrypt == nullptr || chacha20Decrypt == nullptr) {
            cerr << "Ошибка получения функций Scytale\n";
            unloadLibrary();
            return false;
        }
    }
    else if (string(name) == "Serpent") {
        serpentEncrypt = reinterpret_cast<SerpentCipherFunc>(dlsym(libraryHandle, "encrypt"));
        serpentDecrypt = reinterpret_cast<SerpentCipherFunc>(dlsym(libraryHandle, "decrypt"));

        if (serpentEncrypt == nullptr || serpentDecrypt == nullptr) {
            cerr << "Ошибка получения функций Scytale\n";
            unloadLibrary();
            return false;
        }
    } else {
        cerr << "Неизвестный алгоритм: " << name << '\n';
        unloadLibrary();
        return false;
    }
    return true;
}

void LibraryManager::unloadLibrary() {
    if (libraryHandle != nullptr) {
        dlclose(libraryHandle);
        libraryHandle = nullptr;
    }
    getName = nullptr;

    affineEncrypt = nullptr;
    affineDecrypt = nullptr;

    teaEncrypt = nullptr;
    teaDecrypt = nullptr;

    scytaleEncrypt = nullptr;
    scytaleDecrypt = nullptr;

    xorEncrypt = nullptr;
    xorDecrypt = nullptr;

    chacha20Encrypt = nullptr;
    chacha20Decrypt = nullptr;

    serpentEncrypt = nullptr;
    serpentDecrypt = nullptr;
}