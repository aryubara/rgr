#ifndef LIBRARY_MANAGER_HPP
#define LIBRARY_MANAGER_HPP

#include <string>

using namespace std;

typedef bool (*ValidateKeyFunc)(const char*);
typedef void (*CipherFunc)(const unsigned char*, unsigned char*, int, const char*);
typedef void (*GenKeyFunc)(char*, int);

class LibraryManager {
private:
    void* libraryHandle;

public:
    LibraryManager();
    ~LibraryManager();

    bool loadLibrary(const string& path);
    void unloadLibrary();

    ValidateKeyFunc validateKey;
    CipherFunc encrypt;
    CipherFunc decrypt;
    GenKeyFunc generateKey;
};

#endif