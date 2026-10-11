#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "strip_suffix.h"

int strip_suffix(const char *name, char *out, size_t out_size) {
    const char *slash = strrchr(name, '/');
    const char *base = slash ? slash + 1 : name;
    const char *dot = strrchr(base, '.');

    int r;
    if (dot && dot != base) {
        r = snprintf(out, out_size, "%.*s", (int) (dot - name), name);
    } else {
        r = snprintf(out, out_size, "%s", name);
    }

    if (r < 0 || (size_t) r >= out_size) {
        return -1;
    }

    return 0;
}
