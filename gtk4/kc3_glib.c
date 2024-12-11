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
#include "kc3_glib.h"

bool g_kc3_g_main_stop = false;

bool * kc3_g_main_context_iteration (bool *dest)
{
  *dest = g_main_context_iteration(NULL, TRUE);
  return dest;
}

void kc3_g_main (void)
{
  while (! g_kc3_g_main_stop)
    g_main_context_iteration(NULL, TRUE);
}
