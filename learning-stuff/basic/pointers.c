#include <stdbool.h>
#include <stdio.h>

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

int main() {

  struct employee_t employee;

  init_employee(&employee);

  printf("%d %b %d \n", employee.income, employee.staff, employee.id);

  return 0;
}
