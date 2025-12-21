/* sizeof_f80.c - Debug f80 size for marshall_read */
#include <stdio.h>

int main(void)
{
  printf("sizeof(long double) = %zu\n", sizeof(long double));
  printf("sizeof(long double) < 16 ? %s\n",
         sizeof(long double) < 16 ? "true" : "false");
  printf("16 - sizeof(long double) = %zu\n",
         16 - sizeof(long double));
  return 0;
}
