lib=libs/colorprint.c
shell=src/sh.c

all:
	gcc -Wall -Wpedantic -Wextra -Werror -fsanitize=address -fsanitize=undefined ${shell} ${lib} -o sh
