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
#include <string.h>
#include "alloc.h"
#include "assert.h"
#include "arg.h"
#include "binding.h"
#include "buf.h"
#include "buf_parse.h"
#include "fn.h"
#include "fn_clause.h"
#include "list.h"
#include "tag_type.h"

void fn_clean (s_fn *fn)
{
  assert(fn);
  fn_clause_delete_all(fn->clauses);
}

void fn_delete (s_fn *fn)
{
  fn_clean(fn);
  free(fn);
}

s_fn * fn_init (s_fn *fn)
{
  assert(fn);
  *fn = (s_fn) {0};
  return fn;
}

s_fn * fn_init_1 (s_fn *fn, char *p)
{
  s_buf buf;
  uw len;
  sw r;
  assert(fn);
  assert(p);
  len = strlen(p);
  buf_init(&buf, false, len, (char *) p);
  buf.wpos = len;
  r = buf_parse_fn(&buf, fn);
  if (r < 0 || (uw) r != len) {
    err_write_1("fn_init_1: invalid function: \"");
    err_write_1(p);
    err_write_1("\": ");
    err_inspect_sw(&r);
    err_write_1(" != ");
    err_inspect_uw(&len);
    assert(! "fn_init_1: invalid function");
    return NULL;
  }
  return fn;
}

s_fn * fn_init_cast (s_fn *fn, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_FN:
    return fn_init_copy(fn, &tag->data.fn);
  default:
    break;
  }
  err_write_1("fn_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Fn");
  assert(! "fn_init_cast: cannot cast to Fn");
  return NULL;
}

s_fn * fn_init_copy (s_fn *fn, const s_fn *src)
{
  s_fn tmp = {0};
  fn_clause_copy(src->clauses, &tmp.clauses);
  tmp.macro = src->macro;
  tmp.special_operator = src->special_operator;
  *fn = tmp;
  return fn;
}

s_fn * fn_new (void)
{
  s_fn *fn;
  fn = alloc(sizeof(s_fn));
  if (! fn)
    return NULL;
  fn_init(fn);
  return fn;
}

s_fn * fn_new_copy (const s_fn *src)
{
  s_fn *fn;
  assert(src);
  fn = alloc(sizeof(s_fn));
  if (! fn)
    return NULL;
  if (! fn_init_copy(fn, src)) {
    free(fn);
    return NULL;
  }
  return fn;
}
