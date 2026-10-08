#include <stdio.h>

#include "flip_bits.h"

const PATH_MAX = 64;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 1;
    }

    flip_bits(argc, argv);

    return 0;
}
