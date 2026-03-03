#include "../../include/common.h"
#include <dirent.h>
#include <sys/stat.h>

static void list_dir(const char *path, int depth) {
    if (depth > 2) return;
    DIR *d = opendir(path);
    if (!d) return;
    struct dirent *de;
    while ((de = readdir(d))) {
        if (!strcmp(de->d_name,".") || !strcmp(de->d_name,"..")) continue;
        printf("%*s%s/%s\n", depth*2, "", path, de->d_name);
    }
    closedir(d);
}

int main() {
    const char *must_exist[] = {"/bin", "/dev", "/etc", "/proc", "/tmp", NULL};
    for (int i=0; must_exist[i]; i++) {
        struct stat st;
        if (stat(must_exist[i], &st) != 0 || !S_ISDIR(st.st_mode)) {
            fail("Missing expected directory inside sandbox rootfs");
        }
    }
    printf("INFO: listing some entries under /\n");
    list_dir("/", 0);
    pass("Rootfs basic structure present");
}
