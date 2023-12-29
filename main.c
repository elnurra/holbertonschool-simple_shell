#include "list.h"

/**
 * read_command - Reads a command from the standard input.
 *
 * Return: A dynamically allocated string containing the command.
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
 * parse_arguments - Parses the given command into arguments.
 *
 * @command: The command string to be parsed.
 * @args: An array to store the parsed arguments.
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
 * search_and_execute - Searches for and executes the specified command.
 *
 * @args: An array containing the command and its arguments.
 * @command: The original command string.
 */

void search_and_execute(char *args[], char *command)
{
	if (strchr(args[0], '/') != NULL)
	{
		if (access(args[0], X_OK) == 0)
		{
			if (execve(args[0], args, environ) == -1)
			{
				perror("execve");
				free(command);
				exit(EXIT_FAILURE); }
		}
	}
	else
	{
		char *token, *path = getenv("PATH");

		if (path == NULL)
		{
			fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
			free(command);
			exit(127); }
		token = strtok(path, ":");

		while (token != NULL)
		{
			char exec_path[256];

			snprintf(exec_path, sizeof(exec_path), "%s/%s", token, args[0]);

			if (access(exec_path, X_OK) == 0)
			{
				if (execve(exec_path, args, environ) == -1)
				{
					perror("execve");
					free(command);
					exit(EXIT_FAILURE); }
			}
			token = strtok(NULL, ":"); }
	}
	fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
	free(command);
	exit(127); }


/**
 * execute_command - Executes the given command.
 *
 * @command: The command to be executed.
 *
 * Return: The exit status of the executed command.
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
			char **env = environ;

			while (*env != NULL)
			{
				printf("%s\n", *env);
				env++;
			}
			free(command);
			exit(EXIT_SUCCESS);
		}
		search_and_execute(args, command); }
	else
	{
		waitpid(pid, &status, 0);
		free(command);
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
		else
			status = (1); }
		return (status); }


/**
 * main - The main function for the simple shell program.
 *
 * Return: The exit status of the program.
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
		if (status == 2)
		{
			exit(2);
		}
	}
	return (status);
}
