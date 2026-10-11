#include <stdio.h>
#include <limits.h>

#include "flip_bits.h"
#include "strip_suffix.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }

    char out_name[PATH_MAX];
    if (strip_suffix(argv[1], out_name, sizeof(out_name)) != 0) {
        fprintf(stderr, "name too long\n");
        return 1;
    }

    flip_bits(argv[1], out_name);

    return 0;
}
