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
#include <string.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_parse.h"
#include "env.h"
#include "ptuple.h"
#include "sym.h"
#include "tag.h"
#include "tuple.h"

void ptuple_clean (p_tuple *tuple)
{
  bool nullify;
  assert(tuple);
  if (! *tuple)
    return;
  nullify = (*tuple)->ref_count == 1;
  tuple_delete(*tuple);
  if (nullify)
    *tuple = NULL;
}

p_tuple * ptuple_init (p_tuple *tuple, uw count)
{
  p_tuple tmp;
  if (! (tmp = tuple_new(count)))
    return NULL;
  *tuple = tmp;
  return tuple;
}

p_tuple * ptuple_init_1 (p_tuple *tuple, const char *p)
{
  p_tuple tmp;
  if (! (tmp = tuple_new_1(p)))
    return NULL;
  *tuple = tmp;
  return tuple;
}

p_tuple * ptuple_init_2 (p_tuple *tuple, s_tag *a, s_tag *b)
{
  p_tuple tmp;
  if (! (tmp = tuple_new(2)))
    return NULL;
  tag_init_copy(tmp->tag + 0, a);
  tag_init_copy(tmp->tag + 1, b);
  *tuple = tmp;
  return tuple;
}

p_tuple * ptuple_init_cast (p_tuple *tuple, const s_sym * const *type,
                            s_tag *tag)
{
  assert(tuple);
  assert(tag);
  switch (tag->type) {
  case TAG_PTUPLE:
    return ptuple_init_copy(tuple, &tag->data.td_ptuple);
  default:
    break;
  }
  err_write_1("ptuple_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Tuple)
    err_puts(" to Tuple");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Tuple");
  }
  assert(! "ptuple_init_cast: cannot cast to Tuple");
  return NULL;
}

p_tuple * ptuple_init_copy (p_tuple *tuple, p_tuple *src)
{
  p_tuple tmp;
  if (env_global()->pass_by_copy)
    tmp = tuple_new_copy(*src);
  else
    tmp = tuple_new_ref(*src);
  if (! tmp)
    return NULL;
  *tuple = tmp;
  return tuple;
}
