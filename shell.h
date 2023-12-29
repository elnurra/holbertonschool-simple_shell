#ifndef MAIN_H
#define MAIN_H
char **line_devider(char *buffer);
char **get_input(char **buffer, size_t *len);
void free_array(char ***arr);
char *path_handler(char *file_name);
#endif
