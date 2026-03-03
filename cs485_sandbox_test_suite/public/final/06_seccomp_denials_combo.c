#include "../../include/common.h"
#include <sys/mount.h>
#include <sys/ptrace.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    int ok = 1;

    errno=0;
    if (mount("none","/tmp","tmpfs",0,"")==0) { umount("/tmp"); ok = 0; printf("INFO: mount succeeded\n"); }
    else printf("INFO: mount denied errno=%d\n", errno);

    errno=0;
    if (ptrace(PTRACE_TRACEME,0,NULL,NULL)==0) { ok = 0; printf("INFO: ptrace succeeded\n"); }
    else printf("INFO: ptrace denied errno=%d\n", errno);

    errno=0;
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (fd >= 0) { close(fd); ok = 0; printf("INFO: raw socket succeeded\n"); }
    else printf("INFO: raw socket denied errno=%d\n", errno);

    if (!ok) fail("One or more dangerous operations were allowed");
    pass("dangerous operations denied");
}
