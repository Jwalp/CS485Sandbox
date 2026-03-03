#include "../../include/common.h"
#include <time.h>

static double now_sec() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
}

int main() {
    const int N = 500000;
    double t0 = now_sec();
    for (int i=0; i<N; i++) (void)getpid();
    double t1 = now_sec();
    printf("INFO: getpid() x %d took %.4f sec\n", N, (t1-t0));
    pass("minibench lite completed");
}
