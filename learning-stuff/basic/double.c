#include <stdio.h>
#include <stdlib.h>

typedef enum { Ok, Error } status_t;

status_t try_realloc(int **data, int len) {

  int *temp;

  temp = realloc(*data, len);

  if (NULL == temp) {
    *data = NULL;
    return Error;
  }

  *data = temp;

  return Ok;
}

int main() {
  int *data = malloc(64);

  if (Error == try_realloc(&data, 128)) {
    printf("Could not realloc data\n");
  }

  return 0;
}
