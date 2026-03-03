#include "../../include/common.h"

static int has_host_init(const char *s) {
    if (!s) return 0;
    return strstr(s, "systemd") || strstr(s, "launchd");
}

int main() {
    char *cmd = read_small_file("/proc/1/cmdline", 512);
    char *st  = read_small_file("/proc/1/status", 2048);

    if (!cmd && !st) {
        printf("INFO: /proc/1 not readable (ok if documented)\n");
        pass("No clear host PID1 leak");
    }

    if (cmd) {
        for (size_t i=0; i<200; i++) {
            if (cmd[i] == '\0') cmd[i] = ' ';
            if (cmd[i] == '\0' || cmd[i] == '\n') break;
        }
        printf("INFO: /proc/1/cmdline: %s\n", cmd);
        if (has_host_init(cmd)) {
            free(cmd); free(st);
            fail("PID1 appears to be host init (systemd/launchd) — PID namespace likely missing");
        }
    }

    if (st) {
        printf("INFO: /proc/1/status excerpt:\n");
        int lines = 0;
        for (char *p = st; *p && lines < 15; p++) {
            putchar(*p);
            if (*p == '\n') lines++;
        }
        putchar('\n');
    }

    free(cmd); free(st);
    pass("No obvious host PID1 signature detected");
}
