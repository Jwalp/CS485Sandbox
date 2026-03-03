#include "../../include/common.h"
#include <sys/wait.h>

int main() {
    int children = 0;
    for (int i=0; i<20; i++) {
        pid_t pid = fork();
        if (pid < 0) break;
        if (pid == 0) {
            size_t sz = 8 * 1024 * 1024; // 8MB
            char *p = malloc(sz);
            if (p) memset(p, 0x5A, sz);
            usleep(200000);
            free(p);
            _exit(0);
        }
        children++;
    }
    int st;
    while (waitpid(-1, &st, 0) > 0) {}
    printf("INFO: spawned children=%d\n", children);
    pass("resource pressure mix completed (TA verifies limits)");
}
