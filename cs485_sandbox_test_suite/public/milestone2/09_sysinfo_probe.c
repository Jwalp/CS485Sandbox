#include "../../include/common.h"
#include <sys/utsname.h>

int main() {
    struct utsname u;
    if (uname(&u) != 0) die("uname");
    printf("INFO: sysname=%s release=%s machine=%s\n", u.sysname, u.release, u.machine);

    const char *hi[] = {"/proc/kcore", "/proc/kmsg", "/sys/kernel/debug", NULL};
    for (int i=0; hi[i]; i++) {
        if (file_readable(hi[i])) {
            printf("INFO: high-risk path readable: %s\n", hi[i]);
            fail("High-risk kernel interface readable (should be denied/absent)");
        }
    }
    pass("sysinfo ok; high-risk kernel interfaces not readable");
}
