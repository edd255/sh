#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "common.h"
#include "parser.h"
#include "prompt.h"

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
