#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct employee_t {
  int id;
  int income;
  bool staff;
};

int init_employee(struct employee_t *e) {
  static int employee_count = 0;

  e->id = employee_count;
  e->income = 0;
  e->staff = false;

  employee_count++;

  return employee_count - 1;
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

  for (int i = 0; i < n; i++) {
    int count = init_employee(&employees[i]);
    printf("Created new employee with Id: %d\n", count);
  }

  destroy_employee(employees);

  return 0;
}
