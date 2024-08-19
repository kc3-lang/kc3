#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include "../http/types.h"

void print_offsets() {
    printf("Offset of subject: %zu\n", offsetof(struct fact_w, subject));
    printf("Offset of predicate: %zu\n", offsetof(struct fact_w, predicate));
    printf("Offset of object: %zu\n", offsetof(struct fact_w, object));
    printf("Offset of id: %zu\n", offsetof(struct fact_w, id));
}

int main (void) {
  print_offsets();
  return 0;
}
