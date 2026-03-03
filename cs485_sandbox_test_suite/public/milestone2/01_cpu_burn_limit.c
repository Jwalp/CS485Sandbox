#include "../../include/common.h"
#include <time.h>

static double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main() {
    double start = now_sec();
    volatile unsigned long long x = 0;
    while (now_sec() - start < 2.0) {
        x = x * 1664525ULL + 1013904223ULL;
    }
    printf("INFO: cpu_burn done, x=%llu\n", x);
    pass("CPU burn completed (TA will verify CPU limit behavior)");
}
