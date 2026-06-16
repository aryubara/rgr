CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I. -IUtilities
LDFLAGS := -ldl
SHELL := /bin/bash

TARGET = RGREncryptionAlgorithmRGR

# Исходные файлы программы
SRC := main.cpp \
       LibraryManager.cpp \
       Utilities/fileManager.cpp \
       Utilities/cryptoMath.cpp

# Динамические библиотеки
LIBS := libAffine.so \
        libTEA.so \
        libScytale.so \
        libXOR.so \
        libSerpent.so \
        libChaCha20.so

# Сборка всего проекта
all: $(LIBS) $(TARGET)

# Сборка библиотек
libAffine.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC \
	Affine/affine.cpp Utilities/cryptoMath.cpp \
	-o $@

libTEA.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC \
	TEA/tea.cpp \
	-o $@

libScytale.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC \
	Scytale/scytale.cpp \
	-o $@

libXOR.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC \
	XOR/xor.cpp \
	-o $@

libSerpent.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC \
	Serpent/Serpent.cpp \
	-o $@

libChaCha20.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC \
	ChaCha20/ChaCha20.cpp \
	-o $@

# Сборка основной программы
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $@

# Запуск программы
run: all
	LD_LIBRARY_PATH=. ./$(TARGET)

# Очистка проекта
clean:
	rm -f $(TARGET) $(LIBS)

.PHONY: all run clean