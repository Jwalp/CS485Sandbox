#include "../../include/common.h"

int main() {
    const char *paths[] = {"/etc/shadow", "/root/.ssh/id_rsa", "/var/lib/shadow", NULL};
    for (int i=0; paths[i]; i++) {
        errno = 0;
        char *s = read_small_file(paths[i], 64);
        if (s) {
            free(s);
            printf("INFO: readable sensitive path: %s\n", paths[i]);
            fail("Sensitive file readable");
        }
        printf("INFO: blocked/absent: %s (errno=%d)\n", paths[i], errno);
    }
    pass("sensitive reads blocked");
}
