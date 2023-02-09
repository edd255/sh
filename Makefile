CC   := clang
LIB  := libs/colorful-printf/colorprint.c
MAIN := src/sh.c
OUT  := bin/sh

ERROR_FLAGS  := -Wall -Wpedantic -Wextra -Werror
DEBUG_FLAGS  := -Og -g -fsanitize=address -fsanitize=undefined
OPT_FLAGS    := -Ofast


debug:
	${CC} ${COMMON_FLAGS} ${DEBUG_FLAGS} ${ERROR_FLAGS} ${MAIN} ${LIB} -o ${OUT}_debug

opt:
	${CC} ${COMMON_FLAGS} ${OPT_FLAGS} ${ERROR_FLAGS} ${MAIN} ${LIB} -o ${OUT}_opt
