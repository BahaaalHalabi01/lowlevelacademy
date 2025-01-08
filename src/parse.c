#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/parse.h"

int create_db_header(int fd, struct db_header_t **header_out) {
  struct db_header_t *header = calloc(1, sizeof(struct db_header_t));

  if (header == NULL) {
    printf("Can not allocate memory for this struct\n");
    return STATUS_ERROR;
  }

  header->version = 0x1;
  header->signature = HEADER_SIGNATURE;
  header->employees_count = 0;
  header->filesize = sizeof(*header);

  *header_out = header;

  return STATUS_SUCCESS;
}

int validate_db_header(int fd, struct db_header_t **header_out) {

  if (fd < 0) {
    // do i really need to check this ? is not being checked
    // in db_open_file ?
    printf("Bad file descriptor from the user\n");
    return STATUS_ERROR;
  }

  struct db_header_t *header = calloc(1, sizeof(struct db_header_t));
  if (header == NULL) {
    printf("Can not allocate memory for this struct\n");
    return STATUS_ERROR;
  }

  if (read(fd, header, sizeof(struct db_header_t)) !=
      sizeof(struct db_header_t)) {
    perror("read");
    free(header);
    return -1;
  }
  // network to host byte order to use it correctly on our system
  // we are storing in network byte order ( BE) always
  header->version = ntohs(header->version);
  header->employees_count = ntohs(header->employees_count);
  header->filesize = ntohl(header->filesize);
  header->signature = ntohl(header->signature);

  if (CURRENT_VERSION != header->version) {
    printf("Invalid header version\n");
    free(header);
    return STATUS_ERROR;
  }

  if (header->signature != HEADER_SIGNATURE) {
    printf("Invalid header signature\n");
    free(header);
    return STATUS_ERROR;
  }

  struct stat db_stat = {0};
  if (fstat(fd, &db_stat) != 0) {
    perror("fstat");
    free(header);
    return STATUS_ERROR;
  }

  if (db_stat.st_size != header->filesize) {
    printf("Corrupt database file\n");
    free(header);
    return STATUS_ERROR;
  }

  *header_out = header;

  return STATUS_SUCCESS;
};

void output_file(int fd, struct db_header_t *header) {

  if (fd < 0) {
    printf("Bad file descriptor from the user\n");
    return;
  }

  // we are storing in network byte order ( BE) always
  header->version = htons(header->version);
  header->employees_count = htons(header->employees_count);
  header->filesize = htonl(header->filesize);
  header->signature = htonl(header->signature);

  if (lseek(fd, 0, SEEK_SET) == -1) {
    perror("lseek");
    return;
  };

  if (write(fd, header, sizeof(struct db_header_t)) == -1) {
    perror("write");
    return;
  };
}
