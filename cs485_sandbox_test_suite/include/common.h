#ifndef CS485_TEST_COMMON_H
#define CS485_TEST_COMMON_H

#define _POSIX_C_SOURCE 199309L
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>


static inline void die(const char *msg) {
    perror(msg);
    exit(2);
}

static inline void pass(const char *msg) {
    if (msg) printf("PASS: %s\n", msg);
    exit(0);
}

static inline void fail(const char *msg) {
    if (msg) printf("FAIL: %s\n", msg);
    exit(1);
}

static inline int file_readable(const char *path) {
    return access(path, R_OK) == 0;
}

static inline char *read_small_file(const char *path, size_t max_bytes) {
    FILE *fp = fopen(path, "rb");
    if (!fp) return NULL;
    char *buf = (char*)calloc(1, max_bytes + 1);
    if (!buf) { fclose(fp); return NULL; }
    size_t n = fread(buf, 1, max_bytes, fp);
    buf[n] = '\0';
    fclose(fp);
    return buf;
}

#endif
