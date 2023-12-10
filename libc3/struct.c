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
#include <string.h>
#include "env.h"
#include "map.h"
#include "struct.h"
#include "struct_type.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

void struct_clean (s_struct *s)
{
  f_clean clean;
  s8 *data;
  uw i = 0;
  const s_sym *sym;
  assert(s);
  data = s->data;
  while (i < s->type.map.count) {
    if (tag_type(s->type.map.value + i, &sym)) {
      clean = sym_to_clean(sym);
      if (clean)
        clean(data + s->type.offset[i]);
      i++;
    }
  }
  free(data);
  struct_type_clean(&s->type);
}

s_struct * struct_init_copy (s_struct *s, const s_struct *src)
{
  f_clean clean;
  f_init_copy init_copy;
  uw i = 0;
  uw size;
  const s_sym *sym;
  s_struct tmp;
  assert(s);
  assert(src);
  if (! struct_type_init_copy(&tmp.type, &src->type))
    return NULL;
  tmp.free = true;
  tmp.data = calloc(1, tmp.type.size);
  while (i < tmp.type.map.count) {
    if (tag_type(tmp.type.map.value + i, &sym)) {
      init_copy = sym_to_init_copy(sym);
      if (init_copy) {
        if (! init_copy((s8 *) tmp.data + tmp.type.offset[i],
                        (s8 *) src->data + tmp.type.offset[i]))
          goto ko;
      }
      else {
        size = tag_size(tmp.type.map.value + i);
        memcpy((s8 *) tmp.data + tmp.type.offset[i],
               (s8 *) src->data + tmp.type.offset[i],
               size);
      }
    }
    i++;
  }
  *s = tmp;
  return s;
 ko:
  while (i > 0) {
    i--;
    tag_type(tmp.type.map.value + i, &sym);
    clean = sym_to_clean(sym);
    if (clean)
      clean((s8 *) tmp.data + tmp.type.offset[i]);
  }
  free(tmp.data);
  struct_type_clean(&tmp.type);
  return NULL;
}

void struct_delete (s_struct *s)
{
  assert(s);
  struct_clean(s);
  free(s);
}

s_struct * struct_init (s_struct *s, const s_sym *module)
{
  assert(s);
  assert(module);
  if (! struct_type_init_from_env(&s->type, module, &g_c3_env))
    return NULL;
  s->data = calloc(s->type.size, 1);
  return s;
}

s_struct * struct_init_1 (s_struct *s, const s8 *p)
{
  assert(s);
  assert(p);
  (void) s;
  (void) p;
  return s;
}

s_struct * struct_new (const s_sym *module)
{
  s_struct *s;
  assert(module);
  s = calloc(1, sizeof(s_struct));
  if (! s) {
    warn("struct_new: %s: calloc", module->str.ptr.ps8);
    return NULL;
  }
  if (! struct_init(s, module)) {
    free(s);
    return NULL;
  }
  return s;
}

s_struct * struct_new_1 (const s8 *p)
{
  s_struct *s;
  assert(p);
  s = calloc(1, sizeof(s_struct));
  if (! s) {
    warn("struct_new_1: %s: calloc", p);
    return NULL;
  }
  if (! struct_init_1(s, p)) {
    free(s);
    return NULL;
  }
  return s;
}
