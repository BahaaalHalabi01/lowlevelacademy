#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/parse.h"


int parse_db_header(int fd, void* header) {

  if (read(fd, header, sizeof(header)) != sizeof(header)) {
    perror("read");
    close(fd);
    return -1;
  }

  return 0;
}
