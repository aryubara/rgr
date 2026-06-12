CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -I.
LDFLAGS = -L. -lAffine -lTEA

TARGET = RGREncryptionAlgorithmRGR

# ИСХОДНЫЕ ФАЙЛЫ
SRC = main.cpp Utilities/input.cpp Utilities/fileManager.cpp 

# ЦЕЛЬ
all: libAffine.so libTEA.so $(TARGET)

# СБОРКА БИБЛИОТЕК

libAffine.so: 
	$(CXX) $(CXXFLAGS) -shared -fPIC Affine/affine.cpp Utilities/cryptoMath.cpp -o libAffine.so 

libTEA.so: 
	$(CXX) $(CXXFLAGS) -shared -fPIC TEA/tea.cpp -o libTEA.so


# СБОРКА
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

# ЗАПУСК
run: all
	LD_LIBRARY_PATH=. ./$(TARGET)

clean:
	rm -f $(TARGET) libAffine.so libTEA.so