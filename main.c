#include "shell.h"
#define MAX_ARGS 64
#define MAX_PATH_LENGTH 256
/**
 * read_command - reads command
 *
 * Return: string containing the command,
 * or NULL if an error occurs.
 */

char *read_command(void)
{
	char *command = NULL;
	size_t bufsize = 0;
	ssize_t read_bytes = getline(&command, &bufsize, stdin);

	if (read_bytes == -1)
	{
		free(command);
		return (NULL);
	}
	if (command[read_bytes - 1] == '\n')
		command[read_bytes - 1] = '\0';
	return (command);
}

/**
 * parse_arguments - parse arguments
 *
 *
 *@command: The command string to parse.
 *@args: An array of strings to store the parsed arguments.
 */

void parse_arguments(char *command, char **args)
{
	int arg_count = 0;
	char *token;

	token = strtok(command, " ");

	while (token != NULL && arg_count < 63)
	{
		args[arg_count++] = token;
		token = strtok(NULL, " ");
	}
	args[arg_count] = NULL;
}

/**
 * execute_command - executes command
 *
 * 
 *@command: The command string to execute.
 *
 * Return: the exit status of the executed command,
 * or -1 if an error occurs.
 */

void execute_env_command() {
    char **env = environ;

    while (*env != NULL) {
        printf("%s\n", *env);
        env++;
    }
    exit(EXIT_SUCCESS);
}

void execute_found_executable(char *executable_path, char *args[]) {
    if (execve(executable_path, args, environ) == -1) {
        perror("execve");
        exit(EXIT_FAILURE);
    }
}

void execute_path_command(char *args[]) {
    char *path = getenv("PATH");
    char *token;

    if (path == NULL) {
        fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
        exit(127);
    }

    token = strtok(path, ":");

    while (token != NULL) {
        char executable_path[256];

        snprintf(executable_path, sizeof(executable_path), "%s/%s", token, args[0]);
        if (access(executable_path, X_OK) == 0) {
            execute_found_executable(executable_path, args);
        }

        token = strtok(NULL, ":");
    }

    fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
    exit(127);
}

int execute_command(char *command) {
    int status = 0;
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        free(command);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        char *args[64];

        parse_arguments(command, args);
        if (args[0] == NULL) {
            free(command);
            exit(EXIT_SUCCESS);
        }
        if (strcmp(args[0], "env") == 0) {
            execute_env_command();
        } else if (strchr(args[0], '/') != NULL) {
            if (access(args[0], X_OK) == 0) {
                execute_found_executable(args[0], args);
            }
        } else {
            execute_path_command(args);
        }
    } else {
        waitpid(pid, &status, 0);
        free(command);
        if (WIFEXITED(status)) {
            status = WEXITSTATUS(status);
        } else {
            status = 1;
        }
    }
    return status;
}

/**
 * main - Entry poin
 *
 * Reads commands from the user and executes them.
 * Return: status
 */

int main(void)
{
	int is_piped = !isatty(fileno(stdin));
	char *command;
	int status = 0;

	while (1)
	{
		if (!is_piped)
		{
			printf("#cisfun$ ");
			fflush(stdout);
		}
		command = read_command();
		if (command == NULL)
		{
			break;
		}
		if (strcmp(command, "exit") == 0)
		{
			free(command);
			exit(0);
		}
		status = execute_command(command);
		if (status == 2 && is_piped)
		{
			exit(2);
		}
	}
	return (status);
}
