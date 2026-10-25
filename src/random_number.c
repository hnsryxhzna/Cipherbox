#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>


uint64_t linear_congruential_random(uint64_t seed) {
    // srand(time(NULL));
    
    uint64_t a = pow(13, 13);
    uint64_t b = 0;
    uint64_t m = pow(2, 59);
    // uint64_t seed = rand();
    int iteration = m % seed;
    
    for (int i = 0; i < iteration; i++) {
        seed = (a * seed + b) % m;
    }
    
    seed = seed + 100000000000000000ULL;
    printf("%llu\n", seed);
    
    return seed;
}

uint64_t key_to_seed(char* key) {
    uint64_t h = 14695981039346656037ULL;
    for (const unsigned char *p = (const unsigned char *) key; *p; p++) {
        h ^= *p;
        h *= 1099511628211ULL;
    }
    h %= (1ULL << 59);
    printf("%llu\n", h | 1);
    return h | 1;
}

char* random_key(char* key) {
    uint64_t seed = key_to_seed(key);
    seed = linear_congruential_random(seed);
    return key;
}
