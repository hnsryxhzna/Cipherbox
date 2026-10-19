#include <stdio.h>
#include <string.h>

#include "xor_bits.h"

int xor_bits(char *in_name, char *out_name, char *key) {
    FILE *in = fopen(in_name, "rb");
    FILE *out = fopen(out_name, "wb");
    int key_len = strlen(key);

    if(!in || !out) {
        return 1;
    }

    unsigned char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < n; i++) {
            buffer[i] ^= key[i % key_len];
        }
        fwrite(buffer, 1, n, out);
    }

    fclose(in);
    fclose(out);

    return 0;
}
