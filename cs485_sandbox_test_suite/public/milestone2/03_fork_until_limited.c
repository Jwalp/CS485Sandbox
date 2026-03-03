#include "../../include/common.h"
#include <sys/wait.h>

int main() {
    int children = 0;
    for (int i=0; i<200; i++) {
        pid_t pid = fork();
        if (pid < 0) break;
        if (pid == 0) {
            usleep(200000);
            _exit(0);
        }
        children++;
    }
    int st;
    while (waitpid(-1, &st, 0) > 0) {}
    printf("INFO: forked children=%d\n", children);
    pass("Fork test completed (TA will verify process limit behavior)");
}
