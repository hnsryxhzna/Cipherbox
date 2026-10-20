#include <stdio.h>
#include <string.h>

#include "xor_bits.h"

#define MAGIC_ENC "cipherbox_enc"
#define MAGIC_LEN 13

int xor_bits_enc(FILE *in, FILE *out, char *key, char *in_name) {
    int key_len = strlen(key);
    int name_len = strlen(in_name);

    unsigned char buffer[1024];
    unsigned char enc_name[name_len];
    unsigned char len_byte = (unsigned char) name_len;

    for (int i = 0; i < name_len; i++) {
        enc_name[i] = in_name[i] ^ key[i % key_len];
    }

    unsigned char enc_method = 129;
    fwrite(MAGIC_ENC, 1, MAGIC_LEN, out);
    fputc(enc_method, out);
    fwrite(&len_byte, 1, 1, out);
    fwrite(enc_name, 1, name_len, out);

    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < n; i++) {
            buffer[i] ^= key[i % key_len];
        }
        fwrite(buffer, 1, n, out);
    }

    return 0;
}

int xor_bits_dec(FILE *in, char *enc_name, char *key) {
    int key_len = strlen(key);
    int name_len = strlen(enc_name);

    unsigned char out_name[name_len];
    for (int i = 0; i < name_len; i++) {
        out_name[i] = enc_name[i] ^ key[i % key_len];
    }

    FILE *out = fopen(out_name, "wbx");
    if (!out) {
        perror(out_name);
        return -1;
    }

    unsigned char buffer[1024];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        for (size_t i = 0; i < n; i++) {
            buffer[i] ^= key[i % key_len];
        }
        fwrite(buffer, 1, n, out);
    }

    return 0;
}
