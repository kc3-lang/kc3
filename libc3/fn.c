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
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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
  if (r < 0 || (uw) r != len)
    errx(1, "fn_init_1: buf_parse_fn(%s): %ld != %lu", p, r, len);
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
  fn_clause_copy(src->clauses, &fn->clauses);
  fn->macro = src->macro;
  fn->special_operator = src->special_operator;
  return fn;
}

s_fn * fn_new (void)
{
  s_fn *fn;
  if (! (fn = calloc(1, sizeof(s_fn))))
    err(1, "fn_new: calloc");
  fn_init(fn);
  return fn;
}

s_fn * fn_new_copy (const s_fn *src)
{
  s_fn *fn;
  assert(src);
  if (! (fn = calloc(1, sizeof(s_fn))))
    err(1, "fn_new_copy: calloc");
  return fn_init_copy(fn, src);
}
