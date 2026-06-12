#pragma once

#include<cstdint>

//ИНИЦИАЛИЗАЦИЯ ФУНКЦИЙ
bool isPrime(uint64_t x);
uint64_t gcd(uint64_t a, uint64_t b);
int64_t exEvclid(int64_t a, int64_t b, int64_t& u, int64_t& v);
int64_t modInverse(int64_t c, int64_t m);