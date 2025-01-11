#include <stdio.h>
#include <sys/socket.h>

int main() {
  int fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd == -1) {
    perror("socket");
    return -1;
  }

  printf("%d\n",fd);

  return 0;

}
