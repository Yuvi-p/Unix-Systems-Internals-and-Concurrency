#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdbool.h>

int main() {
    char command[2000], newcmd[2000], dup_env[2000];
    char *env, *token;
    int pid, stat;

    while (true) {
        printf("type your command -> ");
        if (fgets(command, 2000, stdin) == NULL) break;
        
        command[strlen(command) - 1] = 0; 
        
        if (strcmp(command, "leave") == 0) {
            break;
        }

        if ((pid = fork()) == 0) {
            env = getenv("PATH");
            if (env != NULL) {
                strcpy(dup_env, env);
                token = strtok(dup_env, ":");
                
                while (token != NULL) {
                    strcpy(newcmd, token);
                    strcat(newcmd, "/");
                    strcat(newcmd, command);
                    
                    char* arr[] = {newcmd, NULL};
                    execv(newcmd, arr);
                    
                    token = strtok(NULL, ":");
                }
            }
            printf("File not found\n");
            exit(1);
        } else {
            // Father process
            wait(&stat);
        }
    }
    return 0;
}