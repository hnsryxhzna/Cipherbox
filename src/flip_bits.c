#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flip_bits.h"

int flip_bits_enc(FILE *in, FILE *out, char *in_name) {
    int name_len = strlen(in_name);
    
    unsigned char buffer[1024];
    unsigned char enc_name[name_len];
    unsigned char len_byte = (unsigned char) name_len;

    for (int i = 0; i < name_len; i++) {
        enc_name[i] = ~in_name[i];
    }

    unsigned char method = 0;
    fputc(method, out);
    fwrite(&len_byte, 1, 1, out);
    fwrite(enc_name, 1, name_len, out);

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

int flip_bits_dec(FILE *in, FILE *out) {
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
