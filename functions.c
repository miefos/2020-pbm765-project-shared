#include "functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // toupper

#ifndef MC_MAX_SIZE_STRING
#define MC_MAX_SIZE_STRING 256
#endif


void remove_newline(char *str) {
      if (strlen(str) > 0 && str[strlen(str)-1] == '\n')
      	str[strlen(str)-1] = '\0';
}

int get_username_color(char* username, char* color) {
  char username_temp[260]; // actually max 255
  char color_temp[10]; // actually max 6
  // insert username
  int username_ok = 1;
  printf("Please enter your username: ");
  fgets(username_temp, 260, stdin);
  remove_newline(username_temp);
  if (strlen(username_temp) > 255 || strlen(username_temp) < 2) {
      printf("Username should be between 2 and 255 chars. \n");
      username_ok = 0;
  }

  while (!username_ok) {
    printf("\nPlease try again: ");
    fgets(username_temp, 260, stdin);
    remove_newline(username_temp);
    if (strlen(username_temp) > 255 || strlen(username_temp) < 2) {
      printf("Username should be between 2 and 255 chars. \n");
      username_ok = 0;
    } else {
      username_ok = 1;
    }
  }

  // insert color
  int color_ok = 1;
  printf("Please enter your color (6 hex digits): ");
  fgets(color_temp, 10, stdin);
  remove_newline(color_temp);
  if (strlen(color_temp) != 6) {
    printf("Color should be exactly 6 hex digits. \n");
    color_ok = 0;
  }

  char c;
  if ((c = contains_only_hex_digits(color_temp)) != -1) {
    printf("Color contains non-hex-digit character: %c\n", c);
    color_ok = 0;
  }

  while (!color_ok) {
    printf("\nPlease try again: ");
    fgets(color_temp, 10, stdin);
    remove_newline(color_temp);
    if (strlen(color_temp) != 6) {
      printf("Color should be exactly 6 hex digits. \n");
      color_ok = 0;
    } else if ((c = contains_only_hex_digits(color_temp)) != -1) {
      printf("Color contains non-hex-digit character: %c\n", c);
      color_ok = 0;
    } else {
      color_ok = 1;
    }
  }

  strcpy(username, username_temp);
  strcpy(color, color_temp);

  return 0;
}

int contains_only_hex_digits(char* str) {
  for (int i = 0; str[i] != '\0'; i++) {
    char c = toupper(str[i]);
    if ((c < 48 || c > 57) && // decimal digit
        (c < 65 || c > 70)) { // 'A' - 'F'
        return str[i]; // invalid char
    } else {
      str[i] = c; // make uppercase
    }
  }

  return -1; // yes, contains only hex digits
}

int client_setup(int argc, char **argv, int *port, char *ip) {
  // validate parameters
  if (argc != 3) {
    printf("[Error] Please provide IP and port argument only (ex: -a=123.123.123.123 -p=9001).\n");
    return -1;
  }

  *port = get_port("p", argc, argv);

  if (*port < 0) {
    printf("[ERROR] Cannot get port number, errno %d\n", *port);
    return -1;
  }

  char* iptemp = NULL;
  int result = get_named_argument("a", argc, argv, &iptemp);
  strcpy(ip, iptemp);

  if (result < 0 || ip == NULL) {
    printf("[ERROR] Cannot get IP, errno %d\n", result);
    return -1;
  }

  printf("[OK] Client params successful. Port: %d, ip: %s\n", *port, ip);

  return 0;
}


int server_setup(int argc, char **argv, int *port) {
    // validate parameters
    if (argc != 2) {
      printf("[Error] Please provide port argument only (ex: -p=9001).\n");
      return -1;
    }

    *port = get_port("p", argc, argv);

    if (*port < 0) {
      printf("[ERROR] Cannot get port number, %d\n", *port);
      return -1;
    }

    printf("[OK] Server starting on port %d.\n", *port);

    return 0;
}

int get_named_argument(char* key, int argc, char **argv, char** result) {
  int i;
  for (i = 0; i < argc; i++) {
    if (!strcmp(argv[i], "--"))
      return -1; // end

    if (strlen(key) + 2 > strlen(argv[i]))
      continue;

    // check key
    if (argv[i][0] != '-') {
      continue; // not found
    }

    int j = 1;
    int breaked = 0;
    while (key[j-1]) {
      if (key[j-1] != argv[i][j]) {
        breaked = 1;
        break; // not found
      }
      j++;
    }

    if (breaked) {
      continue; // not found
    }

    if (argv[i][j] != '=') {
      continue; // not found
    }

    if ((strlen(argv[i]) < strlen(key)+3) || strlen(argv[i]) - strlen(key) - 2 < 1) {
      return -3; // not found
    }

    // copy
    *result = (char*) malloc(sizeof(char)*(strlen(argv[i]) - strlen(key) - 1));
    if (*result == NULL) {
      return -4; // err malloc
    }

    strcpy(*result, (argv[i] + strlen(key)+2));
    return strlen(*result); // found, return strlen

  }

  return -1; // not found
}


int get_port(char* key, int argc, char** argv) {
  char* port_c = NULL;
  int result = get_named_argument(key, argc, argv, &port_c);
  int i = 0;

  if (result < 0) {
    return -1; // err
  }

  while(port_c[i]) {
    /* printf("Currently checking %c\n", port_c[i]); */
    if (port_c[i] < 48 || port_c[i] > 57) {
      /* printf("Not only nums in port provided\n"); */
      return -2; // err contains not numbers
    }
    i++;
  }

  int port = atoi(port_c);
  free(port_c);

  return port;

}
