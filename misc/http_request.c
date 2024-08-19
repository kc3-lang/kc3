#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include "../http/types.h"

void print_offsets() {
    printf("Offset of method: %zu\n", offsetof(struct http_request, method));
    printf("Offset of url: %zu\n", offsetof(struct http_request, url));
    printf("Offset of protocol: %zu\n", offsetof(struct http_request, protocol));
    printf("Offset of headers: %zu\n", offsetof(struct http_request, headers));
}

int main (void) {
  print_offsets();
  return 0;
}
