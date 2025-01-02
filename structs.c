#include <stdbool.h>
#include <stdio.h>

#define MAX_IDS 32
#define MAX_EMPLOYEES 1000

struct __attribute__((__packed__)) employee_t {
  int id;
  char firstname[64];
  char lastname[64];
  float income;
  bool ismanager;
};

struct employee {
  int age;
  char name[64];
};

void init_employee(struct employee_t *employee) {

  employee->income = 0;
  employee->ismanager = false;
}

int main() {

  struct employee_t employees[MAX_EMPLOYEES];

  int i = 0;

  for (i = 0; i < MAX_EMPLOYEES; i++) {
    init_employee(&employees[i]);
  }

  struct employee test = {3, "test"};

  printf("%d  %s\n ", test.age,test.name);

  printf("Size of sturct in bytes %lu\n", sizeof(struct employee_t));
}
