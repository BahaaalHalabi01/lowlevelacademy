#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "../include/file.h"
#include "../include/parse.h"

void print_usage(char *argv[]) {


  printf("Usage: %s -n -f <datbase path>\n",argv[0]);
  printf("\t -n - create a new database file\n");
  printf("\t -f - (required) path to a database file\n");
  return;

}

int main(int argc, char *argv[]) {

  int c;
  char *db_path = NULL;
  bool new_file = false;

  while ((c = getopt(argc, argv, "nf:")) != -1) {
    switch (c) {
    case 'n':
      new_file = true;
      break;

    case 'f':
      db_path = optarg;
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

  printf("Create a new file %d\n", new_file);
  printf("With db path  %s\n", db_path);

  return 0;
}
