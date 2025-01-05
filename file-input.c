#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

struct database_header_t {
  unsigned short version;
  unsigned short employees;
  unsigned int filelength;
};

int main() {

  int fd = open("./local.db", O_RDONLY);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  struct database_header_t header = {0};

  if (read(fd, &header, sizeof(header)) != sizeof(header)) {
    perror("read");
    return -1;
  }

printf("Database Version %d\n", header.version);
printf("Database employees %d\n", header.employees);
printf("Database filelength %d\n", header.filelength);

  return 0;
}
