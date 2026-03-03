#include "../../include/common.h"

int main() {
    unlink("/tmp/escape");
    if (symlink("/", "/tmp/escape") != 0) {
        printf("INFO: symlink creation blocked (errno=%d)\n", errno);
        pass("Symlink escape not possible (blocked)");
    }

    errno = 0;
    char *s = read_small_file("/tmp/escape/etc/shadow", 64);
    if (s) {
        free(s);
        fail("Able to read /etc/shadow through symlink path");
    }
    printf("INFO: read via symlink denied (errno=%d)\n", errno);
    pass("Symlink path did not allow sensitive read");
}
