#ifndef SH_PROMPT_H
#define SH_PROMPT_H

#include "common.h"

typedef struct prompt_t prompt_t;
struct prompt_t {
    char* username;
    char hostname[BUFSIZE];
    bool init_error;
};

void init_prompt(prompt_t* prompt);
void draw_prompt(char* username, char* hostname);
void handle_cwd_error(char* file, int line, const char* fn);

#endif
