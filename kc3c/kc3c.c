/* Copyright from 2020 to 2026 kmx.io <contact@kmx.io>
 * All rights reserved.
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "../libkc3/kc3.h"

static int usage (int r, const char *argv0)
{
  fprintf(stderr, "Usage: find . -name '*.kc3' | %s [-v]\n", argv0);
  return r;
}

int main (int argc, char **argv)
{
  s_env *env = NULL;
  int e = 0;
  ssize_t in_len;
  char *in_path = NULL;
  size_t in_size = 0;
  s_str path = {0};
  const char *prog;
  int r = 1;
  bool verbose = false;
  if (argc <= 0)
    return usage(1, "kc3c");
  prog = argv[0];
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  if (! (env = env_global()))
    goto clean;
  if (argc == 1 && ! strcmp(argv[0], "-v"))
    verbose = true;
  else if (argc) {
    usage(1, prog);
    goto clean;
  }
  while (1) {
    errno = 0;
    in_len = getline(&in_path, &in_size, stdin);
    if (in_len < 0) {
      if (ferror(stdin)) {
        e = errno;
        fprintf(stderr, "%s: stdin: %s\n", prog,
                e ? strerror(e) : "read error");
        goto clean;
      }
      r = 0;
      goto clean;
    }
    if (in_len && in_path[in_len - 1] == '\n')
      in_path[--in_len] = 0;
    if (in_len && in_path[in_len - 1] == '\r')
      in_path[--in_len] = 0;
    if (! in_len) {
      fprintf(stderr, "%s: stdin: empty path\n", prog);
      goto clean;
    }
    if (memchr(in_path, 0, (size_t) in_len)) {
      fprintf(stderr, "%s: stdin: path contains a NUL byte\n", prog);
      goto clean;
    }
    if (verbose) {
      fprintf(stderr, "%s\n", in_path);
      fflush(stderr);
    }
    str_init(&path, NULL, (uw) in_len, in_path);
    if (! env_load(env, &path, false)) {
      err_write_1("kc3c: failed to compile ");
      err_inspect_str(&path);
      err_write_1("\n");
      err_flush();
      goto clean;
    }
    free(in_path);
    in_path = NULL;
    in_size = 0;
  }
 clean:
  if (in_path)
    free(in_path);
  if (env)
    kc3_clean(env);
  return r;
}
