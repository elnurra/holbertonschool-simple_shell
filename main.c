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
char **line_devider(char *buffer, char **arr)
{
	char *token;
	int i = 0;

	token = strtok(buffer, " \n\t");
	while (token != NULL && i < 63)
	{
		arr[i++] = token;
		token = strtok(NULL, " \n\t");
	}
	arr[i++] = NULL;
	return (arr);
}
/**
 * get_input - input part
 * @len: len
 * @buffer: buff
 *
 * Return: void
 */
char *get_input(void)
{
	char *buffer = NULL;
        size_t len = 0;
	int read;
	if (isatty(STDIN_FILENO))
		printf("$ ");
	read = getline(&buffer, &len, stdin);
	if (read == -1)
	{
		free(buffer);
		return (NULL);
	}
	if (buffer[read - 1] == '\n')
                buffer[read - 1] = '\0';
	return (buffer);
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
	arr = NULL;
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
	return strdup(file_name);
}
/**
 * main - main func
 *
 * Return: int
 */
int main(void)
{
	extern char **environ;
	char *buffer = NULL;
	int status = 0;
	pid_t pid;
	char *path;
	char *original_command;
	while (1)
	{
		buffer = get_input();
		if (buffer == NULL)
			break;
		if (strcmp(buffer, "exit") == 0)
		{
			free(buffer);
			exit(0);
		}
		pid = fork();
		if (pid == 0)
		{
			char *arr[100];
			line_devider(buffer, arr);
			original_command = strdup(arr[0]);
			path = getenv("PATH");
			if (path == NULL || *path == '\0')
			{
				if (arr[0][0] == '/')
				{
					arr[0] = path_handler(arr[0], path);
					if (execve(arr[0], arr, environ) == -1)
					{
						perror("ERROR");
						free_array(arr);
						free(buffer);
						exit(1);
					}
				}
				else
				{
					char error_message[CHAR_BUFFER];
					snprintf(error_message, sizeof(error_message), "./hsh: 1: %s: not found\n",original_command);
					write(STDERR_FILENO, error_message, strlen(error_message));
					exit(127);
				}
			}
			arr[0] = path_handler(arr[0], path);
			if (execve(arr[0], arr, environ) == -1)
			{
				perror("ERROR");
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
	}
	free(buffer);
	return (status);
}
