#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#define CHAR_BUFFER 1024
/**
 * line_devider - devide the line
 * @buffer: string
 *
 * Return: char ptr to ptr
 */
char **line_devider(char *buffer)
{
	char **arr;
	char *token;
	int i = 0;

	arr = malloc((strlen(buffer) + 1) * sizeof(char *));
	if (arr == NULL)
	{
		perror("ERROR");
		exit(1);
	}
	token = strtok(buffer, " \n\t");
	while (token)
	{
		arr[i] = strdup(token);
		if (arr[i] == NULL)
		{
			perror("ERROR");
			exit(1);
		}
		i++;
		token = strtok(NULL, " \n\t");
	}
	arr[i] = NULL;
	free(token);
	return (arr);
}
/**
 * get_input - input part
 * @len: len
 * @buffer: buff
 *
 * Return: void
 */
char **get_input(char **buffer, size_t *len)
{
	int read;
	char **arr;

	if (isatty(STDIN_FILENO))
		printf("$ ");
	read = getline(buffer, len, stdin);
	if (read == -1)
	{
		return (NULL);
	}
	arr = line_devider(*buffer);
	return (arr);
}
/**
 * free_array - free array memory
 * @arr: buffer
 *
 * Return - nothing
 */
void free_array(char **arr)
{
	int i;

	if (arr == NULL)
		return;
	for (i = 0; arr[i] != NULL; i++)
	{
		free(arr[i]);
	}
	free(arr);
}
/**
 * path_handler - path handler
 * @file_name: name of file
 *
 * @path: path
 *
 * return: file_name
 */
char *path_handler(char *file_name, char *path)
{
	char *token = strtok(path, ":");
	char cmd[100];

	if (file_name[0] == '/')
	{
		if (access(file_name, X_OK) == 0)
		{
			return strdup(file_name);
		}
	}
	while (token)
	{
		snprintf(cmd, sizeof(cmd), "%s/%s", token, file_name);
		if (access(cmd, X_OK) == 0)
		{
			return strdup(cmd);
		}
		token = strtok(NULL, ":");
	}
	free(token);
	return file_name;
}
/**
 * main - main func
 *
 * Return: int
 */
int main(int argc, char **argv)
{
	extern char **environ;
	char *buffer = NULL, **arr;
	size_t len = argc * 512;
	int status = 0;
	pid_t pid;

	while (1)
	{
		arr = get_input(&buffer, &len);
		if (arr == NULL)
			break;
		if (arr[0] == NULL)
		{
			continue;
		}
		pid = fork();
		if (pid == 0)
		{
			char *path = getenv("PATH");
			char *original_command = strdup(arr[0]);
			if (path == NULL || *path == '\0')
			{
				if (arr[0][0] == '/')
				{
					arr[0] = path_handler(arr[0], path);
					if (execve(arr[0], arr, environ) == -1)
					{
						perror("ERROR");
						free(original_command);
						free_array(arr);
						exit(1);
					}
				}
				else
				{
					char error_message[CHAR_BUFFER];
					snprintf(error_message, sizeof(error_message), "%s: 1: %s: not found\n", argv[0], original_command);
					write(STDERR_FILENO, error_message, strlen(error_message));
					free(original_command);
					free_array(arr);
					exit(127);
				}
			}
			free(arr[0]);
			arr[0] = path_handler(arr[0], path);
			if (strcmp(arr[0], "exit") == 0)
                        {
                                free(buffer);
                                free_array(arr);
                                exit(EXIT_SUCCESS);
                        }
			if (execve(arr[0], arr, environ) == -1)
			{
				perror("ERROR");
				free(original_command);
				free_array(arr);
				free(buffer);
				exit(1);
			}
		}
		else if (pid > 0)
		{
			if (waitpid(pid, &status, 0) == -1)
			{
				perror("ERROR");
			}
			if (WIFEXITED(status))
			{
				int exit_status = WEXITSTATUS(status);
				if (exit_status == 127)
				{
					exit(127);
				}
			}
		}
		else
		{
			perror("ERROR");
		}
		if (pid == -1)
			perror("ERROR");
		free_array(arr);
	}
	free(buffer);
	return (status);
}
