#include <stdio.h>
#include <stdint.h>

typedef uint64_t u64;
typedef unsigned __int128 u128;
typedef struct {
    u64 x;
    u64 y;
    int inf;
} Point;

static const u64 P = (1ULL << 61) - 1;
static const u64 A = 2;
static const u64 B = 3;
static const Point INF = {0, 0, 1};

u64 fadd(u64 a, u64 b) {
    u64 r = a + b;
    return r >= P ? r - P : r;
}

u64 fsub(u64 a, u64 b) {
    return a >= b ? a - b : a + P - b;
}

u64 fmul(u64 a, u64 b) {
    return (u64) ((u128) a * b % P);
}

u64 fpow(u64 base, u64 e) {
    u64 r = 1;

    while (e) {
        if (e & 1) r = fmul(r, base);
        base = fmul(base, base);
        e >>= 1;
    }

    return r;
}

u64 finv(u64 a) {
    return fpow(a, P - 2);
}
