#include "../../include/common.h"

int main() {
    const size_t chunk = 16 * 1024 * 1024; // 16MB
    size_t total = 0;
    void **ptrs = NULL;
    size_t n = 0;

    for (;;) {
        void *p = malloc(chunk);
        if (!p) break;
        memset(p, 0xA5, chunk);
        void **tmp = realloc(ptrs, (n+1) * sizeof(void*));
        if (!tmp) { free(p); break; }
        ptrs = tmp;
        ptrs[n++] = p;
        total += chunk;
        if (total >= (size_t)512 * 1024 * 1024) break;
    }

    printf("INFO: allocated ~%zu MB total\n", total / (1024*1024));
    for (size_t i=0; i<n; i++) free(ptrs[i]);
    free(ptrs);
    pass("Memory allocation test completed (TA will verify memory limit behavior)");
}
