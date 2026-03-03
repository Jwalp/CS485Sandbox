#include "../../include/common.h"
#include <sys/wait.h>

int main() {
    int fds[2];
    if (pipe(fds) != 0) die("pipe");
    pid_t pid = fork();
    if (pid < 0) die("fork");
    if (pid == 0) {
        close(fds[1]);
        char buf[128]={0};
        int n = read(fds[0], buf, sizeof(buf)-1);
        close(fds[0]);
        if (n <= 0) _exit(3);
        FILE *fp = fopen("/tmp/ipc_out.txt", "wb");
        if (!fp) _exit(4);
        fwrite(buf, 1, (size_t)n, fp);
        fclose(fp);
        _exit(0);
    }
    close(fds[0]);
    const char *msg="pipe-message\n";
    write(fds[1], msg, strlen(msg));
    close(fds[1]);
    int st=0; waitpid(pid, &st, 0);
    if (!(WIFEXITED(st) && WEXITSTATUS(st)==0)) fail("IPC test failed");
    pass("pipe+fork IPC works");
}
