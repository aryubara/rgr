#include <iostream>

#include "Utilities/input.h"

using namespace std;

enum class Menu {
    EXIT = 0,
    AFFINE = 1,
    TEA = 2
};

int main() {

    int choice;

    do {

        cout << endl;
        cout << "1 - Аффинный шифр\n";
        cout << "2 - TEA\n";
        cout << "0 - Выход\n";
        cout << "Выберите алгоритм: ";

        cin >> choice;

        switch (static_cast<Menu>(choice)) {

            case Menu::AFFINE:
                inputAffine();
                break;

            case Menu::TEA:
                inputTea();
                break;

            case Menu::EXIT:
                cout << "Завершение программы\n";
                break;

            default:
                cout << "Некорректный пункт меню\n";
        }

    } while (choice != static_cast<int>(Menu::EXIT));

    return 0;
}