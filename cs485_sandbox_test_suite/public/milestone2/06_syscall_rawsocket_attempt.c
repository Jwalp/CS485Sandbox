#include "../../include/common.h"
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    errno = 0;
    int fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (fd >= 0) {
        close(fd);
        fail("Raw socket creation succeeded (expected denied)");
    }
    printf("INFO: raw socket denied as expected (errno=%d)\n", errno);
    pass("raw socket blocked");
}
