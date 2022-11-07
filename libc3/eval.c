/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include "env.h"
#include "eval.h"
#include "tag.h"

s_tag * eval_tag (s_env *env, s_tag *dest, s_tag *tag)
{
  (void) env;
  switch (tag->type.type) {
  default:
    return tag_copy(tag, dest);
  }
  assert(! "error");
  return NULL;
}
