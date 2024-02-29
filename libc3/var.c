/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "assert.h"
#include "tag.h"
#include "var.h"

s_tag * var_init_copy (s_tag *var, const s_tag *src)
{
  assert(var);
  assert(src);
  assert(src->type == TAG_VAR);
  if (src->type != TAG_VAR)
    return NULL;
  tag_init_var(var);
  return var;
}
