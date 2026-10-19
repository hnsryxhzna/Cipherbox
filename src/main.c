#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "flip_bits.h"
#include "xor_bits.h"
#include "strip_suffix.h"

int get_enc_method(FILE *in) {
    int method = fgetc(in);
    return method;
}

int get_enc_name(FILE *in, char *name_buffer) {
    int name_len = fgetc(in);
    if (name_len == EOF || name_len == 0) {
        fclose(in);
        return -1;
    }

    fread(name_buffer, 1, name_len, in);
    int position = ftell(in);
    fclose(in);
    return position;
}

int process(char *in_name, char *out_name, char *key) {
    FILE *in = fopen(in_name, "rb");

    if (!in) {
        return 1;
    }

    unsigned char *name_buffer[255];

    int method = get_enc_method(in);
    int position = get_enc_name(in, name_buffer);
    
    FILE *out = fopen(name_buffer, "rb");
    if (!out) {
        return 1;
    }

    if (method == 0) {
        flip_bits_enc(in, out, in_name);
    } else if (method == 128) {
        flip_bits_dec(in, out);
    } else if (method == 1) {
        xor_bits_enc(in, out, key, in_name);
    } else if (method == 129) {
        xor_bits_dec(in, out, key);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    char out_name[PATH_MAX];
    if (strip_suffix(argv[1], out_name, sizeof(out_name)) != 0) {
        fprintf(stderr, "name too long\n");
        return 1;
    }

    // flip_bits(argv[1], out_name);
    // xor_bits(argv[1], out_name, "2753863");

    process(argv[1], out_name, "2753863");

    return 0;
}
