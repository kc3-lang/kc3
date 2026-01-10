/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted the above
 * copyright notice and this permission paragraph are included in all
 * copies and substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <unistd.h>
#include "../libkc3/kc3.h"

int main (int argc, char **argv)
{
  s_env *env = NULL;
  s32 i;
  s_str path = {0};
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  if (! (env = env_global()))
    goto clean;
  i = 0;
  while (i < argc) {
    str_init_1(&path, NULL, argv[i]);
    err_write_1(PROG ": ");
    err_inspect_str(&path);
    err_write_1("\n");
    if (! env_load(env, &path)) {
      err_write_1(PROG ": ");
      err_inspect_str(&path);
      err_write_1("\n");
      i++;
    }
  kc3_clean(env);
  return 0;
 clean:
  kc3_clean(env);
  return 1;
}
