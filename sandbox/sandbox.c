#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sched.h>
#include <sys/stat.h>

#define STACK_SIZE (1024 * 1024)

typedef struct {
    char *program;
} ChildArgs;

int child_fn(void *arg) {
    ChildArgs *ca = arg;
    char *args[] = { ca->program, NULL };
    char *env[]  = { "PATH=/usr/bin:/bin", NULL };
    chroot("/srv/sandbox-rootfs");
    chdir("/");
    execve(ca->program, args, env);
    perror("execve");
    return 1;
}

int main(int argc, char* argv[]){
	if (argc < 2) {
 	   printf("Usage: sandbox <create|launch|destroy>\n");
    	return 1;
	}

	if (strcmp(argv[1], "create") == 0) {
    		if (argc < 4) {
        		printf("Usage: sandbox create <name>\n");
        	return 1;
    		}
    		char *name   = argv[2];
    		char *rootfs = "/var/sandbox";
//    		printf("Creating sandbox '%s' with rootfs '%s'\n", name, rootfs);
    		char path[256];
                snprintf(path, sizeof(path), "/tmp/sandbox_%s.conf", name);
                
                if (access(path, F_OK) == 0) {
                   printf("Error: sandbox '%s' already exists\n", name);
                   return 1;
                }
                
                FILE *f = fopen(path, "w");
                
                if (f == NULL) {
                  printf("Error: could not create config file\n");
                  return 1;
                }
                fprintf(f, "rootfs=%s\n", rootfs);
                fclose(f);
    
                printf("Sandbox '%s' created\n", name);
    	} 
	else if (strcmp(argv[1], "launch") == 0) {
    		if (argc < 4) {
        		printf("Usage: sandbox launch <name> <program>\n");
        	return 1;
    		}
    		char *name   = argv[2];
    		char *program = argv[3];
  //  		printf("Launching sandbox '%s' with program '%s'\n", name, program);
                ChildArgs ca = { program };

                char *stack = malloc(STACK_SIZE);
                if (!stack) { perror("malloc"); return 1; }

                int flags = CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUTS |
                            CLONE_NEWIPC | CLONE_NEWUSER | SIGCHLD;

                pid_t pid = clone(child_fn, stack + STACK_SIZE, flags, &ca);
                if (pid < 0) { perror("clone"); free(stack); return 1; }

                int status;
                waitpid(pid, &status, 0);
                free(stack);
                printf("Program exited with status %d\n", WEXITSTATUS(status));
	} else if (strcmp(argv[1], "destroy") == 0) {
    		if (argc < 3) {
        		printf("Usage: sandbox destroy <name>\n");
        	return 1;
    		}
    		char *name   = argv[2];
    //		printf("Destroying '%s'\n", name);
    		char path[256];
                snprintf(path, sizeof(path), "/tmp/sandbox_%s.conf", name);

                if (access(path, F_OK) != 0) {
                    printf("Error: sandbox '%s' does not exist\n", name);
                    return 1;
                }
              remove(path);
              printf("Sandbox '%s' destroyed\n", name);
	} else {
    		printf("Unknown command: %s\n", argv[1]);
	}
}
