#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct employee_t {
  int id;
  int income;
  bool staff;
};

void init_employee(struct employee_t *e) {

  e->id = 0;
  e->income = 0;
  e->staff = false;

  return;
}

void destroy_employee(struct employee_t *e) {

  free(e);
  e = NULL;
}

int main() {

  int n = 4;
  struct employee_t *employees = malloc(sizeof(struct employee_t) * n);

  if (employees == NULL) {
    printf("Can not allocate memory for this struct\n");
    return -1;
  }

  init_employee(&employees[0]);

  printf("%d  \n", employees[0].income);

  destroy_employee(employees);


  return 0;
}
