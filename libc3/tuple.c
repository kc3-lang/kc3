/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <stdlib.h>
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "tuple.h"
#include "tag.h"

void tuple_clean (s_tuple *tuple)
{
  uw i;
  assert(tuple);
  i = tuple->count;
  while (i--)
    tag_clean(tuple->tag + i);
  free(tuple->tag);
}

s8 tuple_compare (const s_tuple *a, const s_tuple *b)
{
  uw i = 0;
  s8 r;
  assert(a);
  assert(b);
  if (a == b)
    return 0;
  if (a->count < b->count)
    return -1;
  if (a->count > b->count)
    return 1;
  while (i < a->count) {
    if ((r = tag_compare(a->tag + i, b->tag + i)))
      return r;
    i++;
  }
  return 0;
}

s_tuple * tuple_copy (const s_tuple *src, s_tuple *dest)
{
  uw i = 0;
  assert(src);
  assert(dest);
  tuple_init(dest, src->count);
  while (i < src->count) {
    tag_copy(src->tag + i, dest->tag + i);
    i++;
  }
  return dest;
}

void tuple_delete (s_tuple *tuple)
{
  tuple_clean(tuple);
  free(tuple);
}

t_hash_context * tuple_hash_update (t_hash_context *context,
                                    const s_tuple *tuple)
{
  uw i = 0;
  assert(tuple);
  hash_update(context, &tuple->count, sizeof(tuple->count));
  while (i < tuple->count) {
    tag_hash_update(context, tuple->tag + i);
    i++;
  }
  return context;
}

s_tuple * tuple_init (s_tuple *tuple, uw count)
{
  uw i;
  assert(tuple);
  assert(2 <= count);
  tuple->count = count;
  tuple->tag = calloc(count, sizeof(s_tag));
  if (! tuple->tag)
    err(1, "tuple_init: out of memory");
  i = count;
  while (i--)
    tag_init_void(tuple->tag + i);
  return tuple;
}

s_tuple * tuple_init_1 (s_tuple *tuple, const s8 *p)
{
  s_buf buf;
  assert(tuple);
  assert(p);
  buf_init_1(&buf, p);
  if (buf_parse_tuple(&buf, tuple) <= 0) {
    assert(! "invalid tuple");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return tuple;
}

s_str * tuple_inspect (const s_tuple *x, s_str *dest)
{
  s_buf buf;
  sw r;
  sw size;
  size = buf_inspect_tuple_size(x);
  buf_init_alloc(&buf, size);
  if ((r = buf_inspect_tuple(&buf, x)) < 0)
    goto error;
  assert(r == size);
  if (r != size)
    goto error;
  return buf_to_str(&buf, dest);
 error:
  buf_clean(&buf);
  return NULL;
}

s_tuple * tuple_new (uw count)
{
  s_tuple *t;
  t = malloc(sizeof(s_tuple));
  if (! t)
    errx(1, "tuple_new: out of memory");
  return tuple_init(t, count);
}

s_tuple * tuple_new_1 (const s8 *p)
{
  s_tuple *t;
  t = malloc(sizeof(s_tuple));
  if (! t)
    errx(1, "tuple_new_1: out of memory");
  return tuple_init_1(t, p);
}
