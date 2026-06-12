#include "LibraryManager.hpp"
#include <dlfcn.h>
#include <iostream>

using namespace std;

LibraryManager::LibraryManager() : libraryHandle(nullptr), validateKey(nullptr), encrypt(nullptr), decrypt(nullptr), generateKey(nullptr) {}

LibraryManager::~LibraryManager() {
    unloadLibrary();
}

bool LibraryManager::loadLibrary(const string& path) {
    unloadLibrary(); 
    
    libraryHandle = dlopen(path.c_str(), RTLD_LAZY);
    if (!libraryHandle) {
        cerr << "Ошибка ОС при загрузке модуля: " << dlerror() << endl;
        return false;
    }

    validateKey = (ValidateKeyFunc)dlsym(libraryHandle, "validate_key");
    encrypt = (CipherFunc)dlsym(libraryHandle, "encrypt");
    decrypt = (CipherFunc)dlsym(libraryHandle, "decrypt");
    generateKey = (GenKeyFunc)dlsym(libraryHandle, "generate_key");

    if (!validateKey || !encrypt || !decrypt || !generateKey) {
        cerr << "Ошибка: Не удалось найти требуемые функции в файле шифра!" << endl;
        unloadLibrary();
        return false;
    }
    return true;
}

void LibraryManager::unloadLibrary() {
    if (libraryHandle) {
        dlclose(libraryHandle);
        libraryHandle = nullptr;
    }
    validateKey = nullptr;
    encrypt = nullptr;
    decrypt = nullptr;
    generateKey = nullptr;
}