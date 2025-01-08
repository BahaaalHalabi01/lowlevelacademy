#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/file.h"

int db_open_file(char *f_name) {
  int fd = open(f_name, O_RDONLY);
  printf("opened file with fd %d\n",fd);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }

  return fd;
};
int db_create_file(char *f_name) {

  int fd = open(f_name, O_RDONLY);
  if (fd != -1) {
    printf("Database file already exists\n");
    close(fd);
    return STATUS_ERROR;
  }

  fd = open(f_name, O_RDWR | O_CREAT, 0644);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }

  return fd;
};
