#include "sh.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "deps/color-sh/color-sh.h"

void handle_cwd_error(char* file, int line, const char* fn);

//=== PARSING ==================================================================

static char* sh_read_line(void) {
    char* line = NULL;
    size_t bufsize = 0;
    if (getline(&line, &bufsize, stdin) == -1) {
        feof(stdin) ? exit(EXIT_SUCCESS) : exit(EXIT_FAILURE);
    }
    return line;
}

static char** sh_split_line(char* line) {
    int bufsize = BUFSIZE;
    int position = 0;
    char** tokens = malloc(bufsize * sizeof(char*));
    if (tokens == NULL) {
        fprintf(stderr, "sh: allocation error\n");
        exit(EXIT_FAILURE);
    }
    char* token = strtok(line, DELIMITER);
    while (token != NULL) {
        tokens[position] = token;
        position++;
        if (position >= bufsize) {
            bufsize += BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            if (tokens == NULL) {
                fprintf(stderr, "sh: reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, DELIMITER);
    }
    tokens[position] = NULL;
    return tokens;
}

//=== EXECUTING ================================================================

static int sh_launch(char** args) {
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

//=== BUILTINS =================================================================

static char* const builtins[] = {"cd", "help", "exit"};

static int sh_cd(char** args) {
    if (args[1] == NULL) {
        fprintf(stderr, "sh: expected argument to cd");
    } else {
        if (chdir(args[1]) != 0) {
            perror("sh: cd failed");
        }
    }
    return 1;
}

static int sh_help(char** args) {
    UNUSED(args);
    printf("SHELL\n");
    printf("Built-ins:\n");
#pragma unroll
    for (int i = 0; i < 3; i++) {
        printf("%s\n", builtins[i]);
    }
    return 1;
}

static int sh_exit(char** args) {
    UNUSED(args);
    return 0;
}

static int (*const builtins_func[])(char**) = {&sh_cd, &sh_help, &sh_exit};

static int sh_execute(char** args) {
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

//=== PROMPT ===================================================================

static inline void init_prompt(prompt_t* prompt) {
    prompt->username = getlogin();
    // clang-format off
    if (
        gethostname(prompt->hostname, sizeof(prompt->hostname)) == -1
        || prompt->username == NULL
    ) {
        // clang-format on
        fprintf(stderr, "sh: could not access hostname/username");
        prompt->init_error = true;
        return;
    }
    prompt->init_error = false;
}

static inline void draw_prompt(char* username, char* hostname) {
    char cwd[BUFSIZE];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        handle_cwd_error(__FILE_NAME__, __LINE__, __func__);
        return;
    }
    color_bgreen("%s@%s", hostname, username);
    printf(":");
    color_bpurple("%s", cwd);
    color_byellow("$ ");
}

void handle_cwd_error(char* file, int line, const char* fn) {
    switch (errno) {
        case EACCES: {
            fprintf(
                stderr,
                "%s:%i - %s: Permission to read/search a component of the filename was denied",
                file,
                line,
                fn
            );
            break;
        }
        case EFAULT: {
            fprintf(
                stderr,
                "%s:%i - %s: buffer points to a bad address",
                file,
                line,
                fn
            );
            break;
        }
        case EINVAL: {
            fprintf(
                stderr,
                "%s:%i - %s: The size argument is zero and buf is not a NULL pointer",
                file,
                line,
                fn
            );
            break;
        }
        case ENAMETOOLONG: {
            fprintf(
                stderr,
                "%s:%i - %s: The size of the null-terminated absolute pathname string exceeds %d bytes",
                file,
                line,
                fn,
                PATH_MAX
            );
            break;
        }
        case ENOENT: {
            fprintf(
                stderr,
                "%s:%i - %s: The current working directory has been unlinked",
                file,
                line,
                fn
            );
            break;
        }
        case ENOMEM: {
            fprintf(stderr, "%s:%i - %s: Out of memory", file, line, fn);
            break;
        }
        case ERANGE: {
            fprintf(
                stderr,
                "%s:%i - %s: The size argument is less than the length of the absolute pathname of the working directory, including the terminating null byte",
                file,
                line,
                fn
            );
            break;
        }
    }
}

//=== MAIN =====================================================================

int main(void) {
    int status = 0;
    prompt_t* prompt = (prompt_t*)malloc(sizeof(prompt_t));
    init_prompt(prompt);
    if (prompt->init_error) {
        free(prompt);
        return EXIT_FAILURE;
    }
    do {
        draw_prompt(prompt->username, prompt->hostname);
        char* line = sh_read_line();
        char** args = sh_split_line(line);
        status = sh_execute(args);
        free(args);
        free(line);
    } while (status);
    free(prompt);
    return EXIT_SUCCESS;
}
