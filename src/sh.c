#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "../libs/colorprint.h"
#include "sh.h"

#define BUFSIZE 64
#define DELIMITER " \t\r\n\a"


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
        pid_t wpid;
        int status;

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

        do {
                wpid = waitpid(pid, &status, WUNTRACED);
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
        printf("SHELL\n");
        printf("Built-ins:\n");
        for (int i = 0; i < 3; i++) {
                printf("%s\n", builtins[i]);
        }
        return 1;
}


int sh_exit(char** args)
{
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


int main(int argc, char** argv)
{
        char* line;
        char** args;
        int status;
        char* username = getlogin();
        char hostname[256];
        if (gethostname(hostname, sizeof(hostname)) == -1 || username == NULL) {
                fprintf(stderr, "sh: could not access hostname or username");
        }

        do {
                char cwd[1024];
                getcwd(cwd, sizeof(cwd));
                printf_color(1, "[g]\e[1m%s\e[m[/g]:[r][[/r][b]\e[1m%s\e[m[/b][r]][/r]:[m]%s[/m][y]$[/y] ", hostname, username, cwd);
                char* line = sh_read_line();
                args = sh_split_line(line);
                status = sh_execute(args);
                free(line);
                free(args);
        } while (status);

        return EXIT_SUCCESS;
}
