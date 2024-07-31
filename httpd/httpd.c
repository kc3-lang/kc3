/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include <libkc3/kc3.h>
#include "config.h"

int main (int argc, char **argv)
{
  s_str path;
  kc3_init(NULL, &argc, &argv);
  if (! module_path(sym_1("HTTPd"), &g_kc3_env.module_path, KC3_EXT,
                    &path)) {
    kc3_clean(NULL);
    return 1;
  }
  kc3_load(&path);
  kc3_clean(NULL);
  return 0;
}
