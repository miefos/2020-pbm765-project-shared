#ifndef MY_FUNCTIONS_H
#define MY_FUNCTIONS_H

int get_named_argument(char* key, int argc, char **argv, char** result);
int get_port(char* key, int argc, char** argv);
int contains_only_hex_digits(char* str);

int server_setup(int argc, char **argv, int *port);
int client_setup(int argc, char **argv, int *port, char *ip);
int get_username_color(char* username, char* color);
void remove_newline(char *str);
int is_little_endian_system();
// void set_leave_flag(int *flag);

char printable_char(char c);
void print_bytes(void* packet, int count);
void assign_int_to_bytes_lendian(unsigned char* packet_part, int n);
unsigned char get_checksum(unsigned char* packet, int length);
int create_packet(unsigned char* packet, int type, char* data);



#endif
