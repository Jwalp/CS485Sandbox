#include "../../include/common.h"
#include <sys/wait.h>

int main() {
    pid_t pid = fork();
    if (pid < 0) die("fork");
    if (pid == 0) {
        char *argv[] = {"/bin/echo", "hello-from-sandbox", NULL};
        execv("/bin/echo", argv);
        _exit(127);
    }
    int st=0;
    waitpid(pid, &st, 0);
    if (WIFEXITED(st) && WEXITSTATUS(st) == 0) pass("execve /bin/echo allowed");
    fail("execve failed (missing /bin/echo or blocked too aggressively)");
}
