#include <stdio.h>
// something is wrong here :(
int swap(int *a, int *b) {
  int temp;
  temp = *a;
  printf("temp %d\n",temp);
  *a = *b;
  printf("a %d\n",*a);
  *b = temp;
  printf("b %d\n",*b);

  return 0;
}

int main() {
  int x = 10, y = 20;
  printf("x %d y %d\n", x, y);
  swap(&x, &y);
  printf("x %d y %d\n", x, y);
  return 0;
}
