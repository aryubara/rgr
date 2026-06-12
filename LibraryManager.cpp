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
      teaDecrypt(nullptr) {
}

LibraryManager::~LibraryManager() {
    unloadLibrary();
}

bool LibraryManager::loadLibrary(const string& path) {

    unloadLibrary();

    libraryHandle = dlopen(path.c_str(), RTLD_LAZY);

    if (libraryHandle == nullptr) {

        cerr << "Ошибка загрузки библиотеки: "
             << dlerror() << '\n';

        return false;
    }

    getName = reinterpret_cast<GetNameFunc>(
            dlsym(libraryHandle, "getName")
        );

    if (getName == nullptr) {

        cerr << "Не удалось получить название алгоритма\n";

        unloadLibrary();

        return false;
    }

    const char* name = getName();

    if (string(name) == "Affine") {

        affineEncrypt =
            reinterpret_cast<AffineCipherFunc>(
                dlsym(libraryHandle, "encrypt")
            );

        affineDecrypt =
            reinterpret_cast<AffineCipherFunc>(
                dlsym(libraryHandle, "decrypt")
            );

        if (affineEncrypt == nullptr ||
            affineDecrypt == nullptr) {

            cerr << "Ошибка получения функций Affine\n";

            unloadLibrary();

            return false;
        }
    }
    else if (string(name) == "TEA") {

        teaEncrypt =
            reinterpret_cast<TeaCipherFunc>(
                dlsym(libraryHandle, "encrypt")
            );

        teaDecrypt =
            reinterpret_cast<TeaCipherFunc>(
                dlsym(libraryHandle, "decrypt")
            );

        if (teaEncrypt == nullptr ||
            teaDecrypt == nullptr) {

            cerr << "Ошибка получения функций TEA\n";

            unloadLibrary();

            return false;
        }
    }
    else {

        cerr << "Неизвестный алгоритм: "
             << name << '\n';

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
}