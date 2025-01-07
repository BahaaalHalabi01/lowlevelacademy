#include <stdio.h>
#include <stdlib.h>

/**
Correct this code to update the value in the heap
to 10 before giving the address to main
**/

int foo(int **ptr) {
  int value = 10;
  // change where the address
  *ptr = (int *)malloc(sizeof(int));

  if (ptr == NULL) {
    fprintf(stderr, "Memory allocation failed\n");
    return -1;
  }

  // change the value
  **ptr = value;
  return 0;
}

int main() {
  int *ptr = NULL;
  if (0 != foo(&ptr)) {
    return -1;
  }

  if (ptr != NULL) {
    printf("Value: %d\n", *ptr);
    free(ptr);
  }

  return 0;
}
