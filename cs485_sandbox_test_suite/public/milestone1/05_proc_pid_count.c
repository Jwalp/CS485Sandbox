#include "../../include/common.h"
#include <dirent.h>
#include <ctype.h>

static int is_all_digits(const char *s) {
    for (; *s; s++) if (!isdigit((unsigned char)*s)) return 0;
    return 1;
}

int main() {
    DIR *d = opendir("/proc");
    if (!d) fail("Cannot open /proc");
    int count = 0;
    struct dirent *de;
    while ((de = readdir(d))) {
        if (is_all_digits(de->d_name)) count++;
    }
    closedir(d);
    printf("INFO: numeric /proc entries (pids): %d\n", count);
    if (count > 200) fail("PID namespace isolation likely missing (too many host processes visible)");
    pass("PID view in /proc looks reasonably sandboxed");
}
