#include "prompt.h"

#include <errno.h>
#include <string.h>
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
        fprintf(
            stderr,
            "%s:%d in %s: %s",
            __FILE__,
            __LINE__,
            __func__,
            strerror(errno)
        );
        return;
    }
    color_bgreen("%s@%s", hostname, username);
    printf(":");
    color_bpurple("%s", cwd);
    color_byellow("$ ");
}
