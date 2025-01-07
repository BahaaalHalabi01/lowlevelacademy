#ifndef PARSE_H
#define PARSE_H

struct database_header_t {
  unsigned short version;
  unsigned short employees;
  unsigned int filelength;
};

int parse_db_header(int fd,void* header);

#endif


