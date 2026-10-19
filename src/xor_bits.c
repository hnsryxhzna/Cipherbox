#include <stdio.h>
#include <string.h>

#include "xor_bits.h"

int xor_bits_enc(FILE *in, FILE *out, char *key, char *in_name) {
    int key_len = strlen(key);
    int name_len = strlen(in_name);

    unsigned char buffer[1024];
    unsigned char enc_name[name_len];
    unsigned char len_byte = (unsigned char) name_len;

    for (int i = 0; i < name_len; i++) {
        enc_name[i] = in_name[i] ^ key[i % key_len];
    }

    unsigned char enc_method = 1;
    fputc(enc_method, in);
    fwrite(&len_byte, 1, 1, out);
    fwrite(enc_name, 1, name_len, out);

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

int xor_bits_dec(FILE *in, FILE *out, char *key) {
    int key_len = strlen(key);
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
