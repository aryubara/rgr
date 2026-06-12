CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -I.
LDFLAGS = -ldl

TARGET = EncryptionAlgorithmRGR

# ИСХОДНЫЕ ФАЙЛЫ
SRC = main.cpp LibraryManager.cpp Utilities/fileManager.cpp Utilities/cryptoMath.cpp

# ЦЕЛЬ
all: libAffine.so libTEA.so $(TARGET)

# СБОРКА БИБЛИОТЕК

libAffine.so:
$(CXX) $(CXXFLAGS) -shared -fPIC Affine/affine.cpp Utilities/cryptoMath.cpp -o libAffine.so

libTEA.so:
$(CXX) $(CXXFLAGS) -shared -fPIC TEA/tea.cpp -o libTEA.so

# СБОРКА ПРОГРАММЫ
$(TARGET): $(SRC)
$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

# ЗАПУСК
run: all
LD_LIBRARY_PATH=. ./$(TARGET)

# ОЧИСТКА
clean:
rm -f $(TARGET) libAffine.so libTEA.so
