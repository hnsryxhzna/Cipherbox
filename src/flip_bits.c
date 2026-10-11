#include <stdio.h>

#include "flip_bits.h"

int flip_bits(char *in_name, char *out_name) {
    FILE *in = fopen(in_name, "rb");
    FILE *out = fopen(out_name, "wb");

    if(!in || !out) {
        return 1;
    }

    unsigned char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < n; i++) {
            buffer[i] = ~buffer[i];
        }
        fwrite(buffer, 1, n, out);
    }

    fclose(in);
    fclose(out);

    return 0;
}
