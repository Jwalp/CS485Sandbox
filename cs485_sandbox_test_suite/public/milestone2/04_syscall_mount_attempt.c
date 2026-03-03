#include "../../include/common.h"
#include <sys/mount.h>

int main() {
    errno = 0;
    int r = mount("none", "/tmp", "tmpfs", 0, "");
    if (r == 0) {
        umount("/tmp");
        fail("mount() succeeded (expected denied by seccomp/policy)");
    }
    printf("INFO: mount denied as expected (errno=%d)\n", errno);
    pass("mount blocked");
}
