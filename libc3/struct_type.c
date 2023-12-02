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
#include <assert.h>
#include <err.h>
#include <stdlib.h>
#include "map.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag_type.h"

void struct_type_clean (s_struct_type *s)
{
  assert(s);
  map_clean(&s->map);
  free(s->offsets);
}

void struct_type_delete (s_struct_type *s)
{
  assert(s);
  struct_type_clean(s);
  free(s);
}

s_struct_type * struct_type_init (s_struct_type *s, const s_sym *module)
{
  assert(s);
  assert(module);
  (void) module;
  return s;
}

s_struct_type * struct_type_new (const s_sym *module)
{
  s_struct_type *s;
  assert(module);
  s = calloc(1, sizeof(s_struct_type));
  if (! s) {
    warn("struct_type_new: %s: calloc", module->str.ptr.ps8);
    return NULL;
  }
  if (! struct_type_init(s, module)) {
    free(s);
    return NULL;
  }
  return s;
}
