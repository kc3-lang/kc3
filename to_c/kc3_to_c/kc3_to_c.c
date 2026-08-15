/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "../../libkc3/kc3.h"
#include "../to_c.h"

static int usage (int ret, char *argv0);

int main (int argc, char **argv)
{
  s_to_c context = {0};
  s_str in_str = {0};
  s_str out_str = {0};
  bool kc3_initialized = false;
  sw r;
  if (argc < 3)
    return usage(1, PROG);
  g_env_argv0_default = PROG;
  g_env_argv0_dir_default = PREFIX;
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  kc3_initialized = true;
  if (argc != 2)
    goto ko;
  if (! str_init_1(&in_str, NULL, argv[0]) ||
      ! str_init_1(&out_str, NULL, argv[1]))
    goto ko;
  if (! to_c_init(&context))
    goto ko;
  if (! to_c_open(&context, &out_str))
    goto ko;
  if ((r = to_c_file(&context, &in_str)) < 0)
    goto ko;
  to_c_clean(&context);
  kc3_clean(NULL);
  return 0;
 ko:
  to_c_clean(&context);
  if (kc3_initialized)
    kc3_clean(NULL);
  return 1;
}

static int usage (int ret, char *argv0)
{
  fprintf(stderr, "Usage: %s IN OUT\n", argv0);
  return ret;
}
