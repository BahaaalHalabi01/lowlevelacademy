#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    // in db_open_file ? just good practise as this  is kinda an api ?
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
    return STATUS_ERROR;
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

int read_employees(int fd, struct db_header_t *db_header,
                   struct employee_t **employees_out) {

  if (fd < 0) {
    printf("Bad file descriptor from the user\n");
    return STATUS_ERROR;
  }

  int employees_count = db_header->employees_count;

  struct employee_t *employees =
      calloc(employees_count, sizeof(struct employee_t));

  if (employees == NULL) {
    printf("Can not allocate memory for this struct\n");
    return STATUS_ERROR;
  }

  // which one to use?
  // if (read(fd, employees, employees_count * sizeof(struct employee_t)) !=
  // employees_count * sizeof(struct employee_t)) {
  if (read(fd, employees, employees_count * sizeof(struct employee_t)) == -1) {
    perror("read");
    free(employees);
    return STATUS_ERROR;
  }

  for (int i = 0; i < employees_count; i++) {
    employees[i].hours = ntohl(employees[i].hours);
  }

  *employees_out = employees;

  return STATUS_SUCCESS;
};

int add_employee(int fd, struct db_header_t *db_header,
                 struct employee_t *employees, char *input_string) {

  if (fd < 0) {
    printf("Bad file descriptor from the user\n");
    return STATUS_ERROR;
  }

  char *name = strtok(input_string, ",");
  if (name == NULL) {
    printf("No provided name in the input string: %s\n", input_string);
    return STATUS_ERROR;
  }
  char *address = strtok(NULL, ",");
  if (address == NULL) {
    printf("No provided address in the input string: %s\n", input_string);
    return STATUS_ERROR;
  }
  char *hours = strtok(NULL, ",");
  if (hours == NULL) {
    printf("No provided hours in the input string: %s\n", input_string);
    return STATUS_ERROR;
  }

  struct employee_t *current_employee =
      &employees[db_header->employees_count - 1];

  strncpy(current_employee->name, name, sizeof(current_employee->name));
  strncpy(current_employee->address, address,
          sizeof(current_employee->address));
  current_employee->hours = atoi(hours);

  printf("adding employee: %s %s %d \n", current_employee->name,
         current_employee->address, current_employee->hours);

  return STATUS_SUCCESS;
};

int remove_employee(struct db_header_t *db_header, struct employee_t *employees,
                    char *input_string, int *to_delete, int to_delete_count,
                    struct employee_t **out) {

  int current_size = db_header->employees_count;
  for (int y = 0; y < to_delete_count; y++) {

    // woowwwwww this is so horribale to look at
    int current = to_delete[y] - y;
    current_size--;

    printf("currently deleting element at index %d %s\n", current,
           employees[current].name);

    // last element
    if (current == current_size) {
      employees = realloc(employees, current_size * sizeof(struct employee_t));

      if (employees == NULL) {
        printf("Can not allocate memory for this struct\n");
        return STATUS_ERROR;
      }
      continue;
    }

    // this is a stupid way, but idk other than this if not using a linked
    // list??? shift the array to the back
    for (int i = current; i < current_size - 1; i++) {
      employees[i] = employees[i + 1];
    };

    employees = realloc(employees, current_size * sizeof(struct employee_t));

    if (employees == NULL) {
      printf("Can not allocate memory for this struct\n");
      return STATUS_ERROR;
    }
  }

  db_header->employees_count = current_size;
  *out = employees;

  return STATUS_SUCCESS;
}

void output_file(int fd, struct db_header_t *header,
                 struct employee_t *employees) {

  if (fd < 0) {
    printf("Bad file descriptor from the user\n");
    return;
  }

  int real_count = header->employees_count;
  int file_size =
      sizeof(struct db_header_t) + (sizeof(struct employee_t) * real_count);

  // we are storing in network byte order ( BE) always
  header->version = htons(header->version);
  header->employees_count = htons(header->employees_count);
  header->filesize = htonl(file_size);
  header->signature = htonl(header->signature);

  if (lseek(fd, 0, SEEK_SET) == -1) {
    perror("lseek");
    return;
  }

  if (write(fd, header, sizeof(struct db_header_t)) == -1) {
    perror("write");
    return;
  }

  for (int i = 0; i < real_count; i++) {
    printf("Writing employee with name %s\n", employees[i].name);
    // every time i am writing something i have to change endian
    employees[i].hours = htonl(employees[i].hours);

    if (write(fd, &employees[i], sizeof(struct employee_t)) == -1) {
      perror("write");
      return;
    };
  }
}

void list_employees(struct db_header_t *db_header,
                    struct employee_t *employees) {

  printf("Printing all employee information:\n");
  for (int i = 0; i < db_header->employees_count; i++) {
    printf("\tEmployee [%d\\%d]\n", i, db_header->employees_count - 1);
    printf("\tName: %s\n", employees[i].name);
    printf("\tAddress: %s\n", employees[i].address);
    printf("\tHours: %d\n", employees[i].hours);
    printf("\n");
  }
}

int find_by_name(struct db_header_t *db_header, struct employee_t *employees,
                 char *input_string, int **found_list, int *found_count) {

  int to_delete_count = 0;
  int to_delete_index = 0;
  int *to_delete = NULL;

  for (int i = 0; i < db_header->employees_count; i++) {

    if (strstr(employees[i].name, input_string) != NULL) {
      // debug
      //  printf("To delete %s at index %d\n", employees[i].name, i);

      to_delete_count++;
      to_delete = realloc(to_delete, to_delete_count * sizeof(int));
      to_delete[to_delete_index++] = i;
    }
  }

  *found_list = to_delete;
  *found_count = to_delete_count;

  return STATUS_SUCCESS;
}

void clear_file_employees(int fd, struct db_header_t *header) {

  if (fd < 0) {
    printf("Bad file descriptor from the user\n");
    return;
  }

  int real_count = header->employees_count;
  int file_size =
      sizeof(struct db_header_t) + (sizeof(struct employee_t) * real_count);

  // we are storing in network byte order ( BE) always
  header->version = htons(header->version);
  header->employees_count = htons(header->employees_count);
  header->filesize = htonl(file_size);
  header->signature = htonl(header->signature);

  if (lseek(fd, 0, SEEK_SET) == -1) {
    perror("lseek");
    return;
  }

  if (write(fd, header, sizeof(struct db_header_t)) == -1) {
    printf("here\n");
    perror("write");
    return;
  }

  ftruncate(fd,sizeof(struct db_header_t));

}
