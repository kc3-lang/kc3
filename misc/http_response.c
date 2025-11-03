#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include "../http/types.h"

int main() {
  printf("offsetof(s_http_response, protocol) = %zu\n", offsetof(s_http_response, protocol));
  printf("offsetof(s_http_response, code) = %zu\n", offsetof(s_http_response, code));
  printf("offsetof(s_http_response, message) = %zu\n", offsetof(s_http_response, message));
  printf("offsetof(s_http_response, headers) = %zu\n", offsetof(s_http_response, headers));
  printf("offsetof(s_http_response, body) = %zu\n", offsetof(s_http_response, body));
  printf("sizeof(s_http_response) = %zu\n", sizeof(s_http_response));
  return 0;
}
