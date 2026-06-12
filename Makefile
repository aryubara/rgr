CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.
LDFLAGS = -ldl

TARGET = RGREncryptionAlgorithmRGR

# ИСХОДНЫЕ ФАЙЛЫ
SRC = main.cpp LibraryManager.cpp Utilities/fileManager.cpp Utilities/cryptoMath.cpp

# ЦЕЛЬ
all: libAffine.so libTEA.so libScytale.so libXOR.so $(TARGET)

# СБОРКА БИБЛИОТЕК
libAffine.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC Affine/affine.cpp Utilities/cryptoMath.cpp -o libAffine.so

libTEA.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC TEA/tea.cpp -o libTEA.so

libScytale.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC Scytale/scytale.cpp -o libScytale.so

libXOR.so:
	$(CXX) $(CXXFLAGS) -shared -fPIC XOR/xor.cpp -o libXOR.so

# СБОРКА ПРОГРАММЫ
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

# ЗАПУСК
run: all
	LD_LIBRARY_PATH=. ./$(TARGET)

# ОЧИСТКА
clean:
	rm -f $(TARGET) \
	      libAffine.so \
	      libTEA.so \
	      libScytale.so \
	      libXOR.so