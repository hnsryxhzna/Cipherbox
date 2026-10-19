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

int pre_process(char *in_name, char *out_name) {
    FILE *in = fopen(in_name, "rb");
    FILE *out = fopen(out_name, "rb");

    if (!in || !out) {
        return 1;
    }

    unsigned char *name_buffer[255];

    int method = get_enc_method(in);
    int get_enc_name(in, name_buffer);

    if (method == 1) {
        flip_bits();
    } else if (method == 2) {
        xor_bits();
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
    xor_bits(argv[1], out_name, "2753863");

    return 0;
}
