#include "../../include/common.h"

int main() {
    const char *paths[] = {"/proc/version", "/proc/cpuinfo", "/proc/meminfo", NULL};
    for (int i=0; paths[i]; i++) {
        char *s = read_small_file(paths[i], 256);
        if (s) {
            printf("INFO: %s: %.*s\n", paths[i], 120, s);
            free(s);
        } else {
            printf("INFO: %s not readable (ok if restricted)\n", paths[i]);
        }
    }
    if (file_readable("/sys/kernel/debug")) fail("/sys/kernel/debug readable");
    pass("system info probe completed");
}
