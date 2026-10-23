#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

#include "flip_bits.h"
#include "xor_bits.h"
#include "strip_suffix.h"

#define MAGIC_RAW "cipherbox_raw"
#define MAGIC_ENC "cipherbox_enc"
#define MAGIC_LEN 13

int file_type_check(FILE *file) {
    char buffer[MAGIC_LEN];
    size_t n = fread(buffer, 1, MAGIC_LEN, file);

    if (n < MAGIC_LEN) {
        if (ferror(file)) {
            return -1;
        }
        return 0;
    }

    return memcmp(buffer, MAGIC_ENC, sizeof buffer);
}

int is_encrypted(FILE *file) {
    unsigned char buffer[MAGIC_LEN];
    return fread(buffer, 1, MAGIC_LEN, file) == MAGIC_LEN && memcmp(buffer, MAGIC_ENC, MAGIC_LEN) == 0;
}

int get_enc_method(FILE *file) {
    int method = fgetc(file);
    return method;
}

int get_enc_name(FILE *file, char *name_buffer) {
    int name_len = fgetc(file);
    if (name_len == EOF || name_len == 0) {
        return -1;
    }

    fread(name_buffer, 1, name_len, file);
    name_buffer[name_len] = '\0';
    return ftell(file);
}

int process(char *in_name, int enc_method, char *out_name, char *key) {
    FILE *in = fopen(in_name, "rb");
    if (!in) {
        perror(in_name);
        return 1;
    } else if (enc_method == -1) {
        fclose(in);
        return 1;
    }

    if (!is_encrypted(in)) {
        rewind(in);
        FILE *out = fopen(out_name, "wbx");
        if (!out) {
            perror(out_name);
            fclose(in);
            return 1;
        }

        printf("Enc %s to %s\n", in_name, out_name);
        if (enc_method == 0) {
            flip_bits_enc(in, out, in_name);
        } else if (enc_method == 1) {
            xor_bits_enc(in, out, key, in_name);
        }
    } else {
        int method = get_enc_method(in);
        printf("%d\n", method);

        char name_buffer[256];
        int position = get_enc_name(in, name_buffer);
    
        if (method > 127) {
            if (method == 128) {
                flip_bits_dec(in, name_buffer);
            } else if (method == 129) {
                xor_bits_dec(in, name_buffer, key);
            }
        } else {
            fclose(in);
            return 1;
        }
    }

    fclose(in);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return 1;
    }

    char out_name[PATH_MAX];
    // if (strip_suffix(argv[1], out_name, sizeof(out_name)) != 0) {
    //     fprintf(stderr, "name too long\n");
    //     return 1;
    // }
    snprintf(out_name, sizeof(out_name), "%s.cbx", argv[1]);

    char* key = NULL;
    int method = -1;

    if (strcmp(argv[2], "-flip") == 0) {
        method = 0;
    } else if (strcmp(argv[2], "-xor") == 0) {
        if (argc < 4) {
            printf("Password is needed\n");
            return 1;
        }
        method = 1;
        key = argv[3];
    } else {
        return 1;
    }

    process(argv[1], method, out_name, key);

    return 0;
}
