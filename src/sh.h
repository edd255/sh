#ifndef SH_H
#define SH_H

#include <stdbool.h>

enum { BUFSIZE = 1024 };
#define DELIMITER " \t\r\n\a"

#define UNUSED(x) (void)x

typedef struct prompt_t prompt_t;
struct prompt_t {
    char* username;
    char hostname[256];
    bool init_error;
};

static char* sh_read_line(void);
static char** sh_split_line(char* line);
static int sh_launch(char** args);
static int sh_cd(char** args);
static int sh_help(char** args);
static int sh_exit(char** args);
static int sh_execute(char** args);
static inline void init_prompt(prompt_t* prompt);
static inline void draw_prompt(char* username, char* hostname);

#endif
