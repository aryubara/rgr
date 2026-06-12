#include "cryptoMath.h"

using namespace std;


//ПРОВЕРКА НА ПРОСТОТУ
bool isPrime(uint64_t x) {
    if (x <= 1) return false;
    if (x <= 3) return true;
    if (x % 2 == 0) return false;
    for (uint64_t i = 3; i <= x / i; i += 2) {
        if (x % i == 0) {
            return false;
        }
    }
    return true;
}

//ПОИСК ОБЩЕГО ДЕЛИТЕЛЯ
uint64_t gcd(uint64_t a, uint64_t b) {
    while (b != 0) {
        uint64_t t = b;
        b = a % b;
        a = t;
    }
    return a;
}

//РАСШИРЕННЫЙ ЕВКЛИД
int64_t exEvclid(int64_t a, int64_t b, int64_t& u, int64_t& v) {

    int64_t u0 = 1, u1 = 0;
    int64_t v0 = 0, v1 = 1;

    while (b != 0) {

        int64_t q = a / b;
        int64_t r = a % b;

        int64_t temp = b;
        b = r;
        a = temp;

        temp = u1;
        u1 = u0 - q * u1;
        u0 = temp;

        temp = v1;
        v1 = v0 - q * v1;
        v0 = temp;
    }
    u = u0;
    v = v0;

    return a;
}

//ОБРАТНЫЙ ПО МОДУЛЮ
int64_t modInverse(int64_t c, int64_t m) {
    int64_t u, v;

    int64_t gcd = exEvclid(c, m, u, v);

    if (gcd != 1) {

        return -1;
    }
    int64_t d = (u % m + m) % m;
    return d;
}