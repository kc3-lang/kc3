/* Copyright from 2020 to 2026 kmx.io <contact@kmx.io>
 * All rights reserved.
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "../../libkc3/types.h"
#include "../../libkc3/primehash.h"

#define PRIMEHASH64_BUF_SIZE (sizeof(uw) << 16)

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
    return usage(1, "primehash64");
  if (argc == 1) {
    out_path = "<stdout>";
    out_fp = stdout;
  }
  else {
    opt = argv[1];
    if (argc != 3)
      return usage(argc == 2 && ! strncmp(opt, "-h", 3) ? 0 : 1,
                   argv[0]);
    if (strncmp(opt, "-h", 3))
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
    if (! in_size) {
      r = 0;
      goto clean;
    }
    if (in_path[in_len - 1] == '\n')
      in_path[--in_len] = 0;
    if (! (in_fp = fopen(in_path, "rb"))) {
      e = errno;
      fprintf(stderr, "%s: %s: %s\n",
              argv[0], in_path, strerror(e));
      free(in_path);
      goto error;
    }
    char a[PRIMEHASH64_BUF_SIZE];
    t_hash hash;
    u64 h_u64;
    s_str str = {0};
    primehash_u64_init_inline(&hash, 0);
    str.ptr.p_pchar = a;
    while ((str.size = fread(a, 1, sizeof(a), in_fp)))
      primehash_u64_update_inline(&hash, str.ptr.p_pu8, str.size);
    h_u64 = primehash_u64_finalize_inline(&hash, 0);
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
