#ifndef SHELL_H
#define SHELL_H

char* sh_read_line(void);
char** sh_split_line(char* line);
int sh_launch(char** args);
int sh_cd(char** args);
int sh_help(char** args);
int sh_exit(char** args);
int sh_execute(char** args);

#endif
