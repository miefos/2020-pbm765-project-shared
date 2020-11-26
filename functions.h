#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

int get_named_argument(char* key, int argc, char **argv, char** result);
int get_port(char* key, int argc, char** argv);
int contains_only_hex_digits(char* str);

int server_setup(int argc, char **argv, int *port);
int client_setup(int argc, char **argv, int *port, char *ip);
int get_username_color(char* username, char* color);
void remove_newline(char *str);

#endif
