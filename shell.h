#ifndef MAIN_H
#define MAIN_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
extern char **environ;

int delim_counter(char *str, char *delim);
char **token_parse(char *str, char *delim);
char *check_file(char **path_arr, char *filepath, int *flag);
int execute(char **args, char *filename, int *status);
void _printenv(void);
#endif
