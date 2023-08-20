#ifndef SH_BUILTINS_H
#define SH_BUILTINS_H

#include "common.h"

int sh_cd(char** args);
int sh_help(char** args);
int sh_exit(char** args);
int sh_execute(char** args);

#endif
