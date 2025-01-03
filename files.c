#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Incorrect usage of program. Use %s <file_name> \n", argv[0]);
    return -1;
  }

  int fd = open(argv[1], O_RDWR | O_CREAT,0644);

  if (fd == -1) {

    perror("open");
    return -1;
  }
}
