#include "../../include/common.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        printf("INFO: TCP socket blocked (errno=%d) (ok if policy denies networking)\n", errno);
        pass("TCP socket attempt handled");
    }
    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(80);
    inet_pton(AF_INET, "1.1.1.1", &sa.sin_addr);
    connect(fd, (struct sockaddr*)&sa, sizeof(sa));
    close(fd);
    pass("TCP socket attempt handled");
}
