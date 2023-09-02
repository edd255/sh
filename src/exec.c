#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"

//=== EXECUTING ================================================================

int sh_launch(char** args) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("sh: execvp error");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("sh: pid is smaller than 0");
        return 0;
    }
    int status = 0;
    do {
        waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    return 1;
}
