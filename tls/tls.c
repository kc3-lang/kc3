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
#include <tls.h>
#include <libkc3/kc3.h>
#include "tls.h"

s_pointer * kc3_tls_config_new (s_pointer *dest)
{
  s_pointer tmp = {0};
  tmp.pointer_type = sym_1("KC3.Config*");
  tmp.target_type = sym_1("KC3.Config");
  if (! (tmp.ptr.p = tls_config_new()))
    return NULL;
  *dest = tmp;
  return dest;
}
