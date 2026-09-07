/* Copyright from 2020 to 2026 kmx.io <contact@kmx.io>
 * All rights reserved.
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "../../libkc3/types.h"
#include "../../libkc3/primehash.h"

#define BUF_SIZE (sizeof(uw) << 16)

int usage (int r, char *argv0)
{
  fprintf(stderr, "Usage: find . -type f | %s\n", argv0);
  return r;
}

int main (int argc, char **argv)
{
  int e;
  int i;
  FILE *in_fp = NULL;
  uw    in_len;
  char *in_path = NULL;
  uw    in_size = 0;
  const char *opt;
  FILE       *out_fp = NULL;
  const char *out_path;
  int r = 1;
  if (argc <= 0)
    return usage(1, "primehash");
  if (argc == 1) {
    out_path = "<stdout>";
    out_fp = stdout;
  }
  else {
    opt = argv[1];
    if (argc != 3 || opt[0] != '-' || opt[1] != 'h' || opt[2])
      return usage(1, argv[0]);
    out_path = argv[2];
    if (! (out_fp = fopen(out_path, "wb"))) {
      e = errno;
      fprintf(stderr, "%s: %s: %s\n",
              argv[0], out_path, strerror(e));
      goto error;
    }
  }
  while (1) {
    in_path = NULL;
    in_size = 0;
    if ((in_len = getline(&in_path, &in_size, stdin)) <= 0 ||
        ! in_path)
      return 0;
    if (in_path[in_len - 1] == '\n')
      in_path[--in_len] = 0;
    if (! (in_fp = fopen(in_path, "rb"))) {
      e = errno;
      fprintf(stderr, "%s: %s: %s\n",
              argv[0], in_path, strerror(e));
      free(in_path);
      goto error;
    }
    char a[BUF_SIZE];
    u64 h_u64 = 0;
    s_str str = {0};
    str.ptr.p_pchar = a;
    while ((str.size = fread(a, 1, sizeof(a), in_fp)))
      h_u64 = primehash_u64_inline(&str, h_u64);
    fclose(in_fp);
    static const char hex[] = "0123456789abcdef";
    i = 0;
    while (i < 16) {
      a[i] = hex[((u8 *) &h_u64)[i / 2] >> 4];
      a[i + 1] = hex[((u8 *) &h_u64)[i / 2] & 0x0f];
      i += 2;
    }
    a[i] = ' ';
    a[i + 1] = 0;
    if (fwrite(a, 17, 1, out_fp) != 1) {
      e = errno;
      fprintf(stderr, "%s: %s: %s\n",
              argv[0], out_path, strerror(e));
      goto error;
    }
    fputs(in_path, out_fp);
    fputc('\n', out_fp);
    free(in_path);
  }
  r = 0;
 clean:
  if (in_path)
    free(in_path);
  if (out_fp != stdout)
    fclose(out_fp);
  return r;
 error:
  r = 1;
  goto clean;
}
