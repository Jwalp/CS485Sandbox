#include "../../include/common.h"

int main() {
    errno = 0;
    char *s = read_small_file("/etc/shadow", 256);
    if (s) {
        free(s);
        fail("/etc/shadow is readable inside sandbox (should be absent/denied or sandbox-specific)");
    }
    printf("INFO: /etc/shadow not readable (errno=%d)\n", errno);
    pass("Sensitive file not readable");
}
