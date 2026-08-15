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
#include "../../libkc3/str.h"
#include "../to_c.h"

static int usage (int ret, char *argv0);

int main (int argc, char **argv)
{
  s_to_c context = {0};
  s_str in_str = {0};
  s_str out_str = {0};
  sw r;
  if (argc < 3)
    return usage(1, argv[0]);
  if (! str_init_1(&in_str, NULL, argv[1]) ||
      ! str_init_1(&out_str, NULL, argv[2]))
    goto ko;
  if (! to_c_init(&context))
    goto ko;
  if (! to_c_open(&context, &out_str))
    goto ko;
  if ((r = to_c_file(&context, &in_str)) < 0)
    goto ko;
  to_c_clean(&context);
  return 0;
 ko:
  to_c_clean(&context);
  return 1;
}

static int usage (int ret, char *argv0)
{
  fprintf(stderr, "Usage: %s IN OUT\n", argv0);
  return ret;
}
