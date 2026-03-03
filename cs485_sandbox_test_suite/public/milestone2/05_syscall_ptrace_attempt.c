#include "../../include/common.h"
#include <sys/ptrace.h>

int main() {
    errno = 0;
    long r = ptrace(PTRACE_TRACEME, 0, NULL, NULL);
    if (r == 0) fail("ptrace(PTRACE_TRACEME) succeeded (expected denied)");
    printf("INFO: ptrace denied as expected (errno=%d)\n", errno);
    pass("ptrace blocked");
}
