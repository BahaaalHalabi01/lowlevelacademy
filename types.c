#include <stdio.h>

int main(int argc, char *argv[]) {

  char c;
  short y;
  // i guess unsigned int and long are kinda the same size depending on
  // archetecture?
  unsigned int x;
  long xx;
  long long int z;

  //look up two's compliment for this representation
  //incase you wonder why this is (0xffffffff)
  //i mean i guess in hex it makes sense, i just thought it 
  //would be something like 1000000....1 ( in binary)
  //but for hex they use two's compliment? need to read more
  int other_var = -1;

  unsigned int xax = (unsigned int)other_var;

  printf("%d %u\n",other_var,xax);

  return  0;
}
