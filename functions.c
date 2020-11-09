#include "functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef MC_MAX_SIZE_STRING
#define MC_MAX_SIZE_STRING 256
#endif

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

    printf("HAPPY! %d, %s, j=%d\n", i, argv[i], j);

    if (argv[i][j] != '=') {
      continue; // not found
    }

    if ((strlen(argv[i]) < strlen(key)+3) || strlen(argv[i]) - strlen(key) - 2 < 1) {
      return -3; // not found
    }

    return 0;

    printf("HAPPY 2! %d, %s\n", i, argv[i]);

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
  while(port_c[i]) {
    if (port_c[i] < 48 || port_c[i] > 57) {
      return -2; // err contains not numbers
    }
    i++;
  }
  if (result < 0) {
    return -1; // err
  } else {
    return atoi(port_c);
  }
}