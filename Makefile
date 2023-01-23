lib=libs/colorprint.c
shell=src/sh.c

all:
	gcc ${shell} ${lib} -o sh
