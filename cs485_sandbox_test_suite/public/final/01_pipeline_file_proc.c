#include "../../include/common.h"
#include <sys/wait.h>

int main() {
    FILE *fp = fopen("/tmp/final_pipeline.txt", "wb");
    if (!fp) die("fopen");
    fputs("final demo pipeline start\n", fp);
    fclose(fp);

    pid_t pid = fork();
    if (pid < 0) die("fork");
    if (pid == 0) {
        char *st = read_small_file("/proc/self/status", 4096);
        if (!st) _exit(3);
        FILE *out = fopen("/tmp/proc_status_excerpt.txt", "wb");
        if (!out) _exit(4);
        int lines=0;
        for (char *c=st; *c && lines<30; c++) {
            fputc(*c, out);
            if (*c=='\n') lines++;
        }
        fclose(out);
        free(st);
        _exit(0);
    }
    int st=0;
    waitpid(pid, &st, 0);
    if (!(WIFEXITED(st) && WEXITSTATUS(st)==0)) fail("child pipeline failed (proc or file I/O blocked too aggressively)");
    pass("file+fork+/proc pipeline succeeded");
}
