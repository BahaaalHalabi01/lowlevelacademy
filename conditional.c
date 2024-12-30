#include <stdio.h>

int main() {

  int temp;

  printf("what is the temperature is it now?\n");
  scanf("%d", &temp);

  printf("you entered this temp: %d\n",temp);

  if (temp >= 70) {
    printf("it is hot out here\n");
  } else if (temp >= 30 && temp < 70) {
    printf("it is kinda mild here\n");
  } else {
    printf("it is hella cold here\n");
  }

  return 0;
}
