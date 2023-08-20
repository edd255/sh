#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

//=== PARSING ==================================================================

char* sh_read_line(void) {
    char* line = NULL;
    size_t bufsize = 0;
    if (getline(&line, &bufsize, stdin) == -1) {
        feof(stdin) ? exit(EXIT_SUCCESS) : exit(EXIT_FAILURE);
    }
    return line;
}

char** sh_split_line(char* line) {
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
