#ifndef PARSE_H
#define PARSE_H

#define HEADER_SIGNATURE 0x4c4c4144
#define CURRENT_VERSION 0x1

struct db_header_t {
  unsigned int signature;
  unsigned short version;
  unsigned short employees_count;
  unsigned int filesize;
};

struct employee_t {
  char name[256];
  char address[256];
  unsigned int hours;
};

int create_db_header(int fd,struct db_header_t **header_out);
int validate_db_header(int fd,struct db_header_t **header_out);
int read_employees(int fd, struct db_header_t *db_header,struct employee_t **employees_out);
int add_employee(int fd, struct db_header_t *db_header,struct employee_t *employees,char *input_string);
void output_file(int fd, struct db_header_t *db_header);

#endif
