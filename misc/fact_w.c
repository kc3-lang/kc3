#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include "../http/types.h"

int main (void) {
  printf("offsetof(s_fact_w, subject) = %zu\n",
         offsetof(s_fact_w, subject));
  printf("offsetof(s_fact_w, predicate) = %zu\n",
         offsetof(s_fact_w, predicate));
  printf("offsetof(s_fact_w, object) = %zu\n",
         offsetof(s_fact_w, object));
  printf("offsetof(s_fact_w, id) = %zu\n",
         offsetof(s_fact_w, id));
  printf("sizeof(s_fact_w) = %zu\n",
         sizeof(s_fact_w));
  return 0;
}
