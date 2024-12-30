#include <stdio.h>

#define DEBUG

void some_other_function() { int personId = 10; }

int main() {

  int personId = 10;
  int person2Id = personId + 1;

#ifdef DEBUG
  printf("We are in debug mode: %s:%d", __FILE__, __LINE__);
#endif
}
