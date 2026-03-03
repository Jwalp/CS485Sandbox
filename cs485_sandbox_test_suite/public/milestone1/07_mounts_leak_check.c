#include "../../include/common.h"

int main() {
    char *m = read_small_file("/proc/mounts", 8192);
    if (!m) m = read_small_file("/etc/mtab", 8192);
    if (!m) fail("Cannot read mount table (/proc/mounts or /etc/mtab)");

    const char *bad_markers[] = {"/var/lib/docker", "/snap/", NULL};
    for (int i=0; bad_markers[i]; i++) {
        if (strstr(m, bad_markers[i])) {
            printf("INFO: found marker '%s' in mount table\n", bad_markers[i]);
            free(m);
            fail("Mount table suggests host/container runtime exposure (document exceptions if needed)");
        }
    }
    free(m);
    pass("Mount view does not obviously expose host/container-runtime mounts");
}
