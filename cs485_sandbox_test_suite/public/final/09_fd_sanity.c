#include "../../include/common.h"
#include <fcntl.h>

int main() {
    int fd = open("/tmp/fdcheck.txt", O_CREAT|O_TRUNC|O_WRONLY, 0644);
    if (fd < 0) die("open");
    write(fd, "fdcheck\n", 8);
    close(fd);
    pass("fd sanity ok");
}
