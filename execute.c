#include "shell.h"

/**
 * execute_command - executes command
 *
 * @command: The command string to execute.
 *
 * Return: the exit status of the executed command,
 * or -1 if an error occurs.
 */

int execute_command(char *command)
{
	int status = 0;
	pid_t pid = fork();

	if (pid == -1)
	{
		perror("fork");
		free(command);
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		char *args[64];

		parse_arguments(command, args);
		if (args[0] == NULL)
		{
			free(command);
			exit(EXIT_SUCCESS);
		}
		if (strcmp(args[0], "env") == 0)
		{
			print_environment();
		}
		if (execute_from_path(args[0], args) == 0)
		{
			perror("execve");
			free(command);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		waitpid(pid, &status, 0);
		free(command);
		status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
	}
	return (status);
}


/**
 * print_environment - prints the environment variables
 */

void print_environment(void)
{
	char **env = environ;

	while (*env != NULL)
	{
		printf("%s\n", *env);
		env++;
	}
	exit(EXIT_SUCCESS);
}

/**
 * execute_from_path - executes the command from the PATH environment variable
 *
 * @command: The command to execute.
 * @args: The arguments for the command.
 *
 * Return: 0 on success, exits with an error message otherwise.
 */
int execute_from_path(char *command, char *args[])
{
	if (strchr(command, '/') != NULL)
	{
		if (access(command, X_OK) == 0)
		{
			return (execve(command, args, environ));
		}
	}
	else
	{
		char *path = getenv("PATH");
		char *token;

		if (path != NULL)
		{
			token = strtok(path, ":");
			while (token != NULL)
			{
				char exec_path[256];

				snprintf(exec_path, sizeof(exec_path), "%s/%s", token, command);
				if (access(exec_path, X_OK) == 0)
				{
					return (execve(exec_path, args, environ));
				}
				token = strtok(NULL, ":");
			}
		}
	}
	fprintf(stderr, "./hsh: 1: %s: not found\n", command);
	exit(127);
}
