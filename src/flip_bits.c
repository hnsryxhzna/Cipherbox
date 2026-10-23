#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flip_bits.h"

#define MAGIC_ENC "cipherbox_enc"
#define MAGIC_LEN 13

int flip_bits_enc(FILE *in, FILE *out, char *in_name) {
    char* slash = strrchr(in_name, '/');
    in_name = slash ? slash + 1 : in_name;
    size_t name_len = strlen(in_name);

    if (name_len == 0 || name_len > 255) {
        fprintf(stderr, "file name too long: %s\n", in_name);
        return 1;
    }

    unsigned char buffer[1024];
    unsigned char enc_name[name_len];
    unsigned char len_byte = (unsigned char) name_len;

    for (int i = 0; i < name_len; i++) {
        enc_name[i] = ~in_name[i];
    }

    unsigned char method = 128;
    fwrite(MAGIC_ENC, 1, MAGIC_LEN, out);
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

    return 0;
}

int flip_bits_dec(FILE *in, char *enc_name) {
    int name_len = strlen(enc_name);

    unsigned char out_name[name_len + 1];
    for (int i = 0; i < name_len; i++) {
        out_name[i] = ~enc_name[i];
    }
    out_name[name_len] = '\0';

    printf("Dec to %s\n", out_name);

    FILE *out = fopen(out_name, "wbx");
    if (!out) {
        perror(out_name);
        return -1;
    }

    unsigned char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < n; i++) {
            buffer[i] = ~buffer[i];
        }
        fwrite(buffer, 1, n, out);
    }

    fclose(out);
    return 0;
}
