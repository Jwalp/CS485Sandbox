#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	if (argc < 2) {
 	   printf("Usage: sandbox <create|launch|destroy>\n");
    	return 1;
	}

	if (strcmp(argv[1], "create") == 0) {
    		if (argc < 4) {
        		printf("Usage: sandbox create <name> <rootfs>\n");
        	return 1;
    		}
    		char *name   = argv[2];
    		char *rootfs = argv[3];
    		printf("Creating sandbox '%s' with rootfs '%s'\n", name, rootfs);
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
    		printf("Launching sandbox '%s' with program '%s'\n", name, program);
	} else if (strcmp(argv[1], "destroy") == 0) {
    		if (argc < 3) {
        		printf("Usage: sandbox destroy <name>\n");
        	return 1;
    		}
    		char *name   = argv[2];
    		printf("Destroying '%s'\n", name);
	} else {
    		printf("Unknown command: %s\n", argv[1]);
	}
}
