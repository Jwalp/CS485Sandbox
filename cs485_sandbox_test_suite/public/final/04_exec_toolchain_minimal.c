#include "../../include/common.h"
#include <sys/wait.h>

static int run(const char *path, char *const argv[]) {
    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) { execv(path, argv); _exit(127); }
    int st=0; waitpid(pid, &st, 0);
    if (WIFEXITED(st)) return WEXITSTATUS(st);
    return 128;
}

int main() {
    char *a1[]={"/bin/echo","ok",NULL};
    if (run("/bin/echo", a1) != 0) fail("/bin/echo not runnable");

    if (access("/bin/sh", X_OK)==0) {
        char *a2[]={"/bin/sh","-c","echo sh-ok",NULL};
        if (run("/bin/sh", a2) != 0) fail("/bin/sh not runnable");
    }
    pass("basic exec works");
}
