#include "prompt.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "deps/color-sh/color-sh.h"

//=== PROMPT ===================================================================

void init_prompt(prompt_t* prompt) {
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

void draw_prompt(char* username, char* hostname) {
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
