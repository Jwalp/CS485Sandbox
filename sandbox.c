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
#include <sys/socket.h>
#include <sched.h>
#include <signal.h>
#include <seccomp.h>

#define STACK_SIZE (1024 * 1024)

void apply_seccomp_filter() {
	scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);
	if (!ctx) { perror("SECCOMP_INIT"); exit(1); }

	// Below are allowed system calls by section

	// file I/O
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read),    0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write),   0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open),    0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat),  0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close),   0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(stat),    0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fstat),   0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(lstat),   0);
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(mount), 0);
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(ptrace), 0);
	seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(sysinfo), 0);

	// process control
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit),         0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group),   0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(wait4),        0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getpid),       0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getuid),       0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getgid),       0);

	// memory management
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap),    0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(munmap),  0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect),0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(brk),     0);

	// filesystems
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(getcwd),  0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(chdir),   0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mkdir),   0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(unlink),  0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rename),  0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(dup),     0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(dup2),    0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(pipe),    0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(pipe2),   0);

	// signals and timing
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigaction),  0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(rt_sigprocmask),0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(nanosleep),     0);

	// executables and processes
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(execve),  0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(clone),   0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(fork),    0);

	// network
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(socket),  0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(connect), 0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(sendto),  0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(recvfrom),0);
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(socket), 1, SCMP_A1(SCMP_CMP_EQ, SOCK_STREAM));
	seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(socket), 1, SCMP_A1(SCMP_CMP_EQ, SOCK_DGRAM));

	if (seccomp_load(ctx) < 0) { perror("SECCOMP_LOAD"); exit(1); }
	seccomp_release(ctx);
}

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
	mount("proc", "/proc", "proc", 0, NULL);
	chmod("/etc/shadow", 0000); // remove user permission
	chmod("/proc/kcore", 0000); // removes process read permissions

	char *args[] = { ca->program, NULL };
	char *env[]  = { "PATH=/usr/bin:/bin", NULL };
	apply_seccomp_filter();
	execve(ca->program, args, env);
	perror("execve");
	return 1;
}

int main(int argc, char* argv[]) {
	if (geteuid() != 0) {
		printf("sandbox must be run as root (use sudo)\n");
		return 1;
	}

	if (argc < 2) {
		printf("Usage: sandbox <create|launch|destroy>\n");
		return 1;
	}

	if (strcmp(argv[1], "create") == 0) {
		if (argc < 3) {
			printf("Usage: sandbox create <name> [SHARED_HOST_FOLDER]\n");
			return 1;
		}
		char *name   = argv[2];
		char *mount_path;
		if (argc >= 4) mount_path = argv[3]; // use this mount path to allow sharing testsuite
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
		if (!f) {
			printf("Error: could not create config file\n");
			return 1;
		}
		fprintf(f, "rootfs=%s\n", rootfs);
		if (argc >= 4) fprintf(f, "mount=%s\n", mount_path); // write mount path
		fclose(f);

		remove("/srv/sandbox-rootfs/etc/shadow"); // removes /etc/shadow

		printf("Sandbox '%s' created\n", name);

	} else if (strcmp(argv[1], "launch") == 0) {
		if (argc < 4) {
			printf("Usage: sandbox launch <name> <program>\n");
			return 1;
		}
		char *name    = argv[2];
		char *program = argv[3];

		// Initially test if our config file exists, if not, we simply deny launch
		char conf_path[256];
		snprintf(conf_path, sizeof(conf_path), "/tmp/sandbox_%s.conf", name);
		FILE *f = fopen(conf_path, "r");
		if (f == NULL) {
			printf("ERROR: could not find config file for %s\n", name);
			printf("INFO: Your sandbox likely hasn't been created\n");
			return 1;
		} else {
			printf("INFO: Config file successfully found\n");	
			fclose(f);
		}

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
			CLONE_NEWIPC | SIGCHLD; //REMOVED CLONE_NEWUSER for netowrk troublshooting

		// required for ping to work, currently runs on each launch for consistency
		chmod("/srv/sandbox-rootfs/bin/ping", 04755);

		// mount shared folder with read only permissions
		mkdir("/srv/sandbox-rootfs/mnt/shared", 0755); // creates shared folder for mounting

		f = fopen(conf_path, "r");
		char line[256];
		while (fgets(line, sizeof(line), f)) {
			char mount_path[256];
			if (sscanf(line, "mount=%255s", mount_path) == 1) {
				// find mount and notify successful mount (with error handling)
				if (mount(mount_path, "/srv/sandbox-rootfs/mnt/shared", NULL, MS_BIND, NULL) < 0) perror("MOUNT");
				if (mount(mount_path, "/srv/sandbox-rootfs/mnt/shared", NULL, MS_BIND | MS_REMOUNT | MS_RDONLY, NULL) < 0) perror ("REMOUNT");
				else printf("INFO: Successfully mounted directory %s\n", mount_path);
				break;
			}
		}
		fclose(f);

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
		f = fopen(conf_path, "r");
		if (f) { 
			FILE *pid_f = fopen(conf_path, "a");
			if (pid_f) { fprintf(pid_f, "pid=%d\n", pid); fclose(pid_f); }
		}

		/* Wait for sandboxed program to finish */
		int status;
		waitpid(pid, &status, 0);

		umount("/srv/sandbox-rootfs/proc");
		umount("/srv/sandbox-rootfs/mnt/shared");

		free(stack);

		if (WIFSIGNALED(status))
			printf("Process killed by signal %d\n", WTERMSIG(status));
		else
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
			remove(path);
			printf("Sandbox '%s' destroyed\n", name);
		}



	} else {
		printf("Unknown command: %s\n", argv[1]);
	}
}
