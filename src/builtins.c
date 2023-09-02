#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "common.h"

//=== BUILTINS =================================================================

char* const builtins[] = {"cd", "help", "exit"};

int sh_cd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "sh: expected argument to cd");
    } else {
        if (chdir(args[1]) != 0) {
            perror("sh: cd failed");
        }
    }
    return 1;
}

int sh_help(char** args) {
    UNUSED(args);
    printf("SHELL\n");
    printf("Built-ins:\n");
#pragma unroll
    for (int i = 0; i < 3; i++) {
        printf("%s\n", builtins[i]);
    }
    return 1;
}

int sh_exit(char** args) {
    UNUSED(args);
    return 0;
}

int (*const builtins_func[])(char**) = {&sh_cd, &sh_help, &sh_exit};

int sh_execute(char** args) {
    if (args[0] == NULL) {
        return 1;
    }
#pragma unroll
    for (int i = 0; i < 3; i++) {
        if (strcmp(args[0], builtins[i]) == 0) {
            return (*builtins_func[i])(args);
        }
    }
    return sh_launch(args);
}
