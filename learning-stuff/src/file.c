#include <fcntl.h>
#include <stdio.h>

int file_open(char *f_name) {

  int fd = open(f_name, O_RDONLY);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  return fd;
}
