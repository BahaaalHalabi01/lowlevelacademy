#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../include/common.h"
#include "../include/file.h"
#include "../include/parse.h"

void print_usage(char *argv[]) {

  printf("Usage: %s -n -f <datbase path>\n", argv[0]);
  printf("\t -n - create a new database file\n");
  printf("\t -f - (required) path to a database file\n");
  return;
}

int main(int argc, char *argv[]) {

  int c;
  char *db_path = NULL;
  char *input_string = NULL;
  char *remove_string = NULL;
  bool new_file = false;
  bool list_flag = false;

  while ((c = getopt(argc, argv, "nf:a:lr:")) != -1) {
    switch (c) {

    case 'n':
      new_file = true;
      break;
    case 'f':
      db_path = optarg;
      break;
    case 'a':
      input_string = optarg;
      break;
    case 'r':
      remove_string = optarg;
      break;
    case 'l':
      list_flag = true;
      break;

    case '?':
      printf("Unknown param -%c\n", c);
      break;

    default:
      return -1;
    }
  }

  if (db_path == NULL) {
    printf("Database path is a required argument\n");
    print_usage(argv);
    return 0;
  }

  int dbfd;
  struct db_header_t *dbhdr = NULL;

  if (new_file) {
    dbfd = db_create_file(db_path);
    if (dbfd == STATUS_ERROR) {
      close(dbfd);
      free(dbhdr);
      printf("Could not create the database file\n");
      return -1;
    }

    if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
      printf("Failed to create a database header\n");
      close(dbfd);
      free(dbhdr);
      return -1;
    };

  } else {
    dbfd = db_open_file(db_path);
    if (dbfd == STATUS_ERROR) {
      close(dbfd);
      free(dbhdr);
      printf("Could not open the database file\n");
      return -1;
    }
    if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
      close(dbfd);
      free(dbhdr);
      printf("Invalid database file header \n");
      return -1;
    }
  }

  printf("Database information:\n");
  printf("\temployees:%d\n", dbhdr->employees_count);
  printf("\tdatabase size:%d\n", dbhdr->filesize);
  printf("\tdatabase version:%d\n", dbhdr->version);

  struct employee_t *employees = NULL;

  if (read_employees(dbfd, dbhdr, &employees) == STATUS_ERROR) {
    close(dbfd);
    free(dbhdr);
    free(employees);
    printf("Could not read employees from database file \n");
    return -1;
  };

  if (input_string) {

    dbhdr->employees_count++;

    employees =
        realloc(employees, dbhdr->employees_count * sizeof(struct employee_t));

    if (employees == NULL) {
      printf("Can not allocate memory for this struct\n");
      close(dbfd);
      free(dbhdr);
      // can you free a null pointer???
      // free(employees);
      return -1;
    }

    if (add_employee(dbfd, dbhdr, employees, input_string) == STATUS_ERROR) {
      printf("Could not add employee with the provided input string\n");
      close(dbfd);
      free(dbhdr);
      free(employees);
      return -1;
    }
  };

  printf("size of employees before %lu\n",sizeof(*employees));;

  if (remove_string) {

    if (remove_employee(dbhdr, employees, remove_string) == STATUS_ERROR) {
      printf("Could remove  employee with the provided name \n");
      close(dbfd);
      free(dbhdr);
      free(employees);
      return -1;
    }
  }

  printf("size of employees after %lu\n",sizeof(*employees));;

  if (list_flag) {
    list_employees(dbhdr, employees);
  }

  output_file(dbfd, dbhdr, employees);

  close(dbfd);
  free(dbhdr);
  free(employees);
  printf("\n");
  return 0;
}
