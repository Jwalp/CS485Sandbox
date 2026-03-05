#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>   
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sched.h>

#define STACK_SIZE (1024 * 1024)

typedef struct {
    char *program;
    int   ready_pipe[2];    /* pipe so child waits for parent to write uid/gid maps */
} ChildArgs;

int child_fn(void *arg) {
    ChildArgs *ca = arg;

    /* Wait for parent to finish writing uid/gid maps.
     * We close the write end and block on read — parent
     * closing its write end is what unblocks us. */
    close(ca->ready_pipe[1]);
    char buf[1];
    read(ca->ready_pipe[0], buf, 1);
    close(ca->ready_pipe[0]);

    /* Jail the filesystem — from here on, / means /srv/sandbox-rootfs.
     * The process cannot see anything above this directory. */
    chroot("/srv/sandbox-rootfs");
    chdir("/");

    char *args[] = { ca->program, NULL };
    char *env[]  = { "PATH=/usr/bin:/bin", NULL };
    execve(ca->program, args, env);
    perror("execve");
    return 1;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: sandbox <create|launch|destroy>\n");
        return 1;
    }

    if (strcmp(argv[1], "create") == 0) {
        if (argc < 3) {
            printf("Usage: sandbox create <name>\n");
            return 1;
        }
        char *name   = argv[2];
        char *rootfs = "/srv/sandbox-rootfs"; /* static rootfs path */

        /* Build config file path: /tmp/sandbox_<name>.conf */
        char path[256];
        snprintf(path, sizeof(path), "/tmp/sandbox_%s.conf", name);

        /* Refuse to overwrite an existing sandbox */
        if (access(path, F_OK) == 0) {
            printf("Error: sandbox '%s' already exists\n", name);
            return 1;
        }

        /* Write rootfs path into config file */
        FILE *f = fopen(path, "w");
        if (f == NULL) {
            printf("Error: could not create config file\n");
            return 1;
        }
        fprintf(f, "rootfs=%s\n", rootfs);
        fclose(f);

        printf("Sandbox '%s' created\n", name);

    } else if (strcmp(argv[1], "launch") == 0) {
        if (argc < 4) {
            printf("Usage: sandbox launch <name> <program>\n");
            return 1;
        }
        char *name    = argv[2];
        char *program = argv[3];

        /* Create a pipe for synchronization.
         * Parent writes uid/gid maps, then closes write end.
         * Child blocks on read until parent is done. */
        ChildArgs ca = { program };
        if (pipe(ca.ready_pipe) < 0) { perror("pipe"); return 1; }

        char *stack = malloc(STACK_SIZE);
        if (!stack) { perror("malloc"); return 1; }

        /* Clone child into new namespaces:
         * CLONE_NEWPID  — child gets its own PID space (appears as PID 1)
         * CLONE_NEWNS   — child gets its own mount namespace
         * CLONE_NEWUTS  — child gets its own hostname
         * CLONE_NEWIPC  — child gets its own IPC objects
         * CLONE_NEWUSER — child gets its own user/group ID mappings */
        int flags = CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUTS |
                    CLONE_NEWIPC | CLONE_NEWUSER | SIGCHLD;

        pid_t pid = clone(child_fn, stack + STACK_SIZE, flags, &ca);
        if (pid < 0) { perror("clone"); free(stack); return 1; }

        /* Parent closes read end — only child reads from pipe */
        close(ca.ready_pipe[0]);

        /* Write uid_map: maps uid 0 inside sandbox to our real uid outside.
         * Format: <inside_uid> <outside_uid> <count> */
        char path[256], map[64];
        snprintf(path, sizeof(path), "/proc/%d/uid_map", pid);
        snprintf(map,  sizeof(map),  "0 %d 1\n", getuid());
        int fd = open(path, O_WRONLY);
        write(fd, map, strlen(map));
        close(fd);

        /* Must write "deny" to setgroups before writing gid_map.
         * This is a kernel requirement when using user namespaces. */
        snprintf(path, sizeof(path), "/proc/%d/setgroups", pid);
        fd = open(path, O_WRONLY);
        write(fd, "deny", 4);
        close(fd);

        /* Write gid_map: same idea as uid_map but for group IDs */
        snprintf(path, sizeof(path), "/proc/%d/gid_map", pid);
        snprintf(map,  sizeof(map),  "0 %d 1\n", getgid());
        fd = open(path, O_WRONLY);
        write(fd, map, strlen(map));
        close(fd);

        /* Signal child that maps are written — close write end of pipe.
         * Child's read() will return and it will proceed with chroot. */
        write(ca.ready_pipe[1], "1", 1);
        close(ca.ready_pipe[1]);

        /* Save PID to config file so destroy can kill this process */
        char conf[256];
        snprintf(conf, sizeof(conf), "/tmp/sandbox_%s.conf", name);
        FILE *f = fopen(conf, "a");
        if (f) { fprintf(f, "pid=%d\n", pid); fclose(f); }

        /* Wait for sandboxed program to finish */
        int status;
        waitpid(pid, &status, 0);
        free(stack);
        printf("Program exited with status %d\n", WEXITSTATUS(status));

    } else if (strcmp(argv[1], "destroy") == 0) {
        if (argc < 3) {
            printf("Usage: sandbox destroy <name>\n");
            return 1;
        }
        char *name = argv[2];
        char path[256];
        snprintf(path, sizeof(path), "/tmp/sandbox_%s.conf", name);

        if (access(path, F_OK) != 0) {
            printf("Error: sandbox '%s' does not exist\n", name);
            return 1;
        }

        /* Read PID from config file and kill the process if running */
        FILE *f = fopen(path, "r");
        if (f) {
            char line[256];
            while (fgets(line, sizeof(line), f)) {
                int pid;
                if (sscanf(line, "pid=%d", &pid) == 1 && pid > 0) {
                    /* Kill the sandbox process group */
                    kill(pid, SIGKILL);
                    printf("Killed process %d\n", pid);
                }
            }
            fclose(f);
        }

        remove(path);
        printf("Sandbox '%s' destroyed\n", name);

    } else {
        printf("Unknown command: %s\n", argv[1]);
    }
}
