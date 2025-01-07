#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/file.h"
#include "../include/parse.h"

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Incorrect usage of program. Use %s <file_name> \n", argv[0]);
    return -1;
  }

  int fd = file_open(argv[1]);

  struct database_header_t header = {0};

  parse_db_header(fd, &header);

  printf("Database Version %d\n", header.version);
  printf("Database employees %d\n", header.employees);
  printf("Database filelength %d\n", header.filelength);

  close(fd);
  return 0;
}
