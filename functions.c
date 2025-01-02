#include <stdio.h>

int add(int a, int b) {
  int z = a + b;

  return z;
}

int main(int argc, char *argv[]) {

  int added = add(1, 2);
  printf("%d\n", added);

  return 0;
}
