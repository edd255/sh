#ifndef SH_H
#define SH_H

#include <stdbool.h>

enum { BUFSIZE = 1024 };
#define DELIMITER " \t\r\n\a"

#define UNUSED(x) (void)x

int sh_launch(char** args);
int sh_cd(char** args);
int sh_help(char** args);
int sh_exit(char** args);
int sh_execute(char** args);

#endif
