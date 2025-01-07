#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct database_header_t {
  unsigned short version;
  unsigned short employees;
  unsigned int filelength;
};

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Incorrect usage of program. Use %s <file_name> \n", argv[0]);
    return -1;
  }

  int fd = open(argv[1], O_RDONLY);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  struct database_header_t header = {0};

  if (read(fd, &header, sizeof(header)) != sizeof(header)) {
    perror("read");
    close(fd);
    return -1;
  }

  printf("Database Version %d\n", header.version);
  printf("Database employees %d\n", header.employees);
  printf("Database filelength %d\n", header.filelength);

  struct stat db_stat = {0};

  if (fstat(fd, &db_stat) != 0) {
    perror("fstat");
    close(fd);
    return -1;
  }

  printf("File size from fstat: %ld\n", db_stat.st_size);

  if (db_stat.st_size != header.filelength) {
    printf("File corrupted,existing \n");
    return -2;
  }

  close(fd);
  return 0;
}
