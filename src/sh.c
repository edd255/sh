#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../libs/colorful-printf/colorprint.h"
#include "sh.h"

#define BUFSIZE 64
#define DELIMITER " \t\r\n\a"

#define UNUSED(x) \
{                 \
        (void) x; \
}

char* USERNAME;
char HOSTNAME[256];

char* sh_read_line(void)
{
        char* line = NULL;
        size_t bufsize = 0;
        if (getline(&line, &bufsize, stdin) == -1) {
                feof(stdin) ? exit(EXIT_SUCCESS) : exit(EXIT_FAILURE);
        }
        return line;
}


char** sh_split_line(char* line)
{
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
                                fprintf(stderr, "sh: realocation error\n");
                                exit(EXIT_FAILURE);
                        }
                }
                token = strtok(NULL, DELIMITER);
        }
        tokens[position] = NULL;
        return tokens;
}


int sh_launch(char** args)
{
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
        int status;
        do {
                waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        return 1;
}


char* builtins[] = {
        "cd",
        "help",
        "exit"
};


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
        for (int i = 0; i < 3; i++) {
                printf("%s\n", builtins[i]);
        }
        return 1;
}


int sh_exit(char** args)
{
        UNUSED(args);
        return 0;
}


int (*builtins_func[]) (char**) = {
        &sh_cd,
        &sh_help,
        &sh_exit
};


int sh_execute(char** args)
{
        if (args[0] == NULL) {
                return 1;
        }
        for (int i = 0; i < 3; i++) {
                if (strcmp(args[0], builtins[i]) == 0) {
                        return (*builtins_func[i])(args);
                }
        }
        return sh_launch(args);
}

int init_prompt(void)
{
        USERNAME = getlogin();
        if (gethostname(HOSTNAME, sizeof(HOSTNAME)) == -1 || USERNAME == NULL) {
                fprintf(stderr, "sh: could not access hostname/username");
                return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
}


inline void draw_prompt(void)
{
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf_color(
                1,
                "[g]\033[1m%s\033[m[/g]:[r][[/r][b]\033[1m%s\033[m[/b][r]][/r]:[m]%s[/m][y]\033[1m$\033[m[/y] ",
                HOSTNAME,
                USERNAME,
                cwd
        );
}


int main(void)
{
        int status;
        if (init_prompt() == EXIT_FAILURE) {
                return EXIT_FAILURE;
        }
        do {
                draw_prompt();
                char* line = sh_read_line();
                char** args = sh_split_line(line);
                status = sh_execute(args);
                free(args);
                free(line);
        } while (status);
        return EXIT_SUCCESS;
}
