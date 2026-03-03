#include "../../include/common.h"

int main() {
    char *osrel = read_small_file("/etc/os-release", 2048);
    if (!osrel) fail("Could not read /etc/os-release (rootfs not configured?)");
    printf("INFO: /etc/os-release (first 200 bytes):\n%.*s\n", 200, osrel);
    free(osrel);

    char *hn = read_small_file("/etc/hostname", 256);
    if (hn) {
        printf("INFO: /etc/hostname: %s\n", hn);
        free(hn);
    } else {
        printf("INFO: /etc/hostname not readable (ok)\n");
    }
    pass("Able to read basic /etc identity tests");
}
