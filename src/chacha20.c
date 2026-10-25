#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 4096

static void die(const char *msg) {
    fprintf(stderr, "Error: %s\n", msg);
    exit(1);
}

static int hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int hex_to_bytes(const char *hex, uint8_t *out, size_t out_len) {
    size_t n = strlen(hex);
    if (n != out_len * 2) return 0;
    for (size_t i = 0; i < out_len; i++) {
        int hi = hexval(hex[2 * i]);
        int lo = hexval(hex[2 * i + 1]);
        if (hi < 0 || lo < 0) return 0;
        out[i] = (uint8_t)((hi << 4) | lo);
    }
    return 1;
}

static inline uint32_t rotl32(uint32_t x, int r) {
    return (x << r) | (x >> (32 - r));
}

static inline uint32_t load32_le(const uint8_t *p) {
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static inline void store32_le(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xff);
    p[1] = (uint8_t)((v >> 8) & 0xff);
    p[2] = (uint8_t)((v >> 16) & 0xff);
    p[3] = (uint8_t)((v >> 24) & 0xff);
}

static inline void quarter_round(uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d) {
    *a += *b; *d ^= *a; *d = rotl32(*d, 16);
    *c += *d; *b ^= *c; *b = rotl32(*b, 12);
    *a += *b; *d ^= *a; *d = rotl32(*d, 8);
    *c += *d; *b ^= *c; *b = rotl32(*b, 7);
}

static void chacha20_block(uint8_t out[64], const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    uint32_t st[16] = {0x61707865u, 0x3320646eu, 0x79622d32u, 0x6b206574u, load32_le(key + 0), load32_le(key + 4), load32_le(key + 8), load32_le(key + 12), load32_le(key + 16), load32_le(key + 20), load32_le(key + 24), load32_le(key + 28), counter, load32_le(nonce + 0), load32_le(nonce + 4), load32_le(nonce + 8)};

    uint32_t x[16];
    for (int i = 0; i < 16; i++) x[i] = st[i];

    for (int i = 0; i < 10; i++) {
        quarter_round(&x[0], &x[4], &x[8], &x[12]);
        quarter_round(&x[1], &x[5], &x[9], &x[13]);
        quarter_round(&x[2], &x[6], &x[10], &x[14]);
        quarter_round(&x[3], &x[7], &x[11], &x[15]);

        quarter_round(&x[0], &x[5], &x[10], &x[15]);
        quarter_round(&x[1], &x[6], &x[11], &x[12]);
        quarter_round(&x[2], &x[7], &x[8], &x[13]);
        quarter_round(&x[3], &x[4], &x[9], &x[14]);
    }

    for (int i = 0; i < 16; i++) {
        x[i] += st[i];
    }
    for (int i = 0; i < 16; i++) {
        store32_le(out + 4 * i, x[i]);
    }
}

static void chacha20_xor_stream(FILE *in, FILE *out, const uint8_t key[32], const uint8_t nonce[12], uint32_t counter) {
    uint8_t buf[BUF_SIZE];
    uint8_t ks[64];

    size_t n;
    uint64_t block_index = 0;

    while ((n = fread(buf, 1, sizeof(buf), in)) > 0) {
        size_t off = 0;
        while (off < n) {
            uint32_t ctr = counter + (uint32_t)block_index;
            chacha20_block(ks, key, nonce, ctr);

            size_t chunk = n - off;
            if (chunk > 64) chunk = 64;

            for (size_t i = 0; i < chunk; i++) {
                buf[off + i] ^= ks[i];
            }

            off += chunk;
            block_index++;
        }

        if (fwrite(buf, 1, n, out) != n) {
            die("???");
        }
    }

    if (ferror(in)) die("???");
}

static cc20(int argc, char **argv) {
    if (argc != 7) {
        fprintf(stderr, "???\n");
        return 1;
    }

    const char *mode = argv[1];
    const char *inpath = argv[2];
    const char *outpath = argv[3];
    const char *keyhex = argv[4];
    const char *noncehex = argv[5];
    const char *ctrstr = argv[6];

    if (strcmp(mode, "enc") != 0 && strcmp(mode, "dec") != 0) {
        die("???");
    }

    uint8_t key[32], nonce[12];
    if (!hex_to_bytes(keyhex, key, sizeof(key))) {
        die("64 hex chars");
    }
    if (!hex_to_bytes(noncehex, nonce, sizeof(nonce))) {
        die("24 hex chars");
    }

    char *endp = NULL;
    unsigned long ctr_ul = strtoul(ctrstr, &endp, 10);
    if (!endp || *endp != '\0' || ctr_ul > 0xFFFFFFFFul) {
        die("counter must be a uint32");
    }
    uint32_t counter = (uint32_t)ctr_ul;

    FILE *in = fopen(inpath, "rb");
    if (!in) {
        die("cannot open input");
    }
    FILE *out = fopen(outpath, "wb");
    if (!out) {
        die("cannot open output");
    }

    chacha20_xor_stream(in, out, key, nonce, counter);

    fclose(in);
    fclose(out);
    return 0;
}
