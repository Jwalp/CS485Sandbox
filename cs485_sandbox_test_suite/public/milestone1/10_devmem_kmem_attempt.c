#include "../../include/common.h"

int main() {
    const char *paths[] = {"/dev/mem", "/dev/kmem", "/dev/kcore", NULL};
    for (int i=0; paths[i]; i++) {
        errno = 0;
        FILE *fp = fopen(paths[i], "rb");
        if (fp) {
            fclose(fp);
            printf("INFO: %s readable\n", paths[i]);
            fail("Dangerous kernel memory device is readable");
        } else {
            printf("INFO: %s not readable (errno=%d)\n", paths[i], errno);
        }
    }
    pass("Kernel memory device files are not readable");
}
