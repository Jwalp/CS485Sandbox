#include "../../include/common.h"
#include <dirent.h>
#include <ctype.h>

static int is_pid(const char *s){ for(;*s;s++) if(!isdigit((unsigned char)*s)) return 0; return 1; }

int main() {
    DIR *d = opendir("/proc");
    if (!d) fail("cannot open /proc");
    int pids=0;
    struct dirent *de;
    while ((de=readdir(d))) if (is_pid(de->d_name)) pids++;
    closedir(d);
    printf("INFO: visible pids=%d\n", pids);
    if (pids > 300) fail("Too many pids visible: likely host process leakage");
    pass("proc pid view looks sandboxed");
}
