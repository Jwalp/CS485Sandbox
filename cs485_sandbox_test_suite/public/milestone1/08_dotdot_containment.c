#include "../../include/common.h"
#include <limits.h>

int main() {
    system("mkdir -p /tmp/a/b/c/d/e >/dev/null 2>&1");
    if (chdir("/tmp/a/b/c/d/e") != 0) fail("Cannot chdir into test directory");

    char cwd[PATH_MAX];
    for (int i=0; i<50; i++) {
        if (chdir("..") != 0) break;
    }
    if (!getcwd(cwd, sizeof(cwd))) die("getcwd");
    printf("INFO: cwd after climbing: %s\n", cwd);

    if (strstr(cwd, "/var/lib/docker") || strstr(cwd, "/snap/")) fail("Traversal suggests host/container exposure");
    pass("Path traversal did not obviously escape sandbox");
}
