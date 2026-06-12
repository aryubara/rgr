#pragma once

#include <string>
#include <vector>
#include <cstdint>

using namespace std;

//Аффинный шифр
typedef void (*AffineCipherFunc)(
    const vector<uint8_t>&,
    vector<uint8_t>&,
    uint32_t,
    uint32_t
);

//TEA
typedef void (*TeaCipherFunc)(
    const vector<uint8_t>&,
    vector<uint8_t>&,
    const uint32_t[4]
);

//Название алгоритма
typedef const char* (*GetNameFunc)();

class LibraryManager {

    void* libraryHandle;

public:

    LibraryManager();
    ~LibraryManager();

    bool loadLibrary(const string& path);
    void unloadLibrary();

    GetNameFunc getName;

    AffineCipherFunc affineEncrypt;
    AffineCipherFunc affineDecrypt;

    TeaCipherFunc teaEncrypt;
    TeaCipherFunc teaDecrypt;
};