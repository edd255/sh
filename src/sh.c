#include "deps/colorful-printf/colorprint.h"
#include "sh.h"

//=== PARSING ==================================================================

static char* sh_read_line(void)
{
	char* line = NULL;
	size_t bufsize = 0;
	if (getline(&line, &bufsize, stdin) == -1) {
		feof(stdin) ? exit(EXIT_SUCCESS) : exit(EXIT_FAILURE);
	}
	return line;
}


static char** sh_split_line(char* line)
{
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
				fprintf(stderr, "sh: realocation error\n");
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, DELIMITER);
	}
	tokens[position] = NULL;
	return tokens;
}

//=== EXECUTING ================================================================

static int sh_launch(char** args)
{
	pid_t pid = fork();
	if (pid == 0) {
		// Child process
		if (execvp(args[0], args) == -1) {
			perror("sh: execvp error");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("sh: pid is smaller than 0");
		return 0;
	}
	int status;
	do {
		waitpid(pid, &status, WUNTRACED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	return 1;
}

//=== BUILTINS =================================================================

static char* builtins[] = {
	"cd",
	"help",
	"exit"
};

static int sh_cd(char** args) {
	if (args[1] == NULL) {
		fprintf(stderr, "sh: expected argument to cd");
	} else {
		if (chdir(args[1]) != 0) {
			perror("sh: cd failed");
		}
	}
	return 1;
}

static int sh_help(char** args) {
	UNUSED(args);
	printf("SHELL\n");
	printf("Built-ins:\n");
	for (int i = 0; i < 3; i++) {
		printf("%s\n", builtins[i]);
	}
	return 1;
}

static int sh_exit(char** args)
{
	UNUSED(args);
	return 0;
}

static int (*builtins_func[]) (char**) = {
	&sh_cd,
	&sh_help,
	&sh_exit
};

static int sh_execute(char** args)
{
	if (args[0] == NULL) {
		return 1;
	}
	for (int i = 0; i < 3; i++) {
		if (strcmp(args[0], builtins[i]) == 0) {
			return (*builtins_func[i])(args);
		}
	}
	return sh_launch(args);
}

//=== PROMPT ===================================================================

static inline void init_prompt(prompt_t* prompt)
{
	prompt -> username = getlogin();
	if (gethostname(prompt -> hostname, sizeof(prompt -> hostname)) == -1 || prompt -> username == NULL) {
		fprintf(stderr, "sh: could not access hostname/username");
		prompt -> init_error = true;
	}
	prompt -> init_error = false;
}


static inline void draw_prompt(char* username, char* hostname)
{
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf_color(1, PROMPT, hostname, username, cwd);
}

//=== MAIN =====================================================================

int main(void)
{
	int status;
	prompt_t* prompt = (prompt_t*) malloc(sizeof(prompt_t));
	init_prompt(prompt);
	if (prompt -> init_error) {
		free(prompt);
		return EXIT_FAILURE;
	}
	do {
		draw_prompt(prompt -> username, prompt -> hostname);
		char* line = sh_read_line();
		char** args = sh_split_line(line);
		status = sh_execute(args);
		free(args);
		free(line);
	} while (status);
	free(prompt);
	return EXIT_SUCCESS;
}
