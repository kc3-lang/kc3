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
#include "buf_inspect.h"
#include "buf_parse.h"
#include "env.h"
#include "io.h"
#include "mutex.h"
#include "ptuple.h"
#include "sym.h"
#include "tuple.h"
#include "tag.h"

void tuple_clean (s_tuple *tuple)
{
  uw i;
  assert(tuple);
  if (tuple->tag) {
    i = tuple->count;
    while (i--)
      tag_clean(tuple->tag + i);
    free(tuple->tag);
  }
}

void tuple_delete (s_tuple *tuple)
{
  assert(tuple);
  if (env_global()->pass_by_copy)
    assert(tuple->ref_count == 1);
  else {
#if HAVE_PTHREAD
    mutex_lock(&tuple->mutex);
#endif
    if (tuple->ref_count <= 0) {
      err_puts("tuple_delete: invalid reference count");
      assert(! "tuple_delete: invalid reference count");
#if HAVE_PTHREAD
      mutex_unlock(&tuple->mutex);
#endif
      return;
    }
    if (--tuple->ref_count) {
#if HAVE_PTHREAD
      mutex_unlock(&tuple->mutex);
#endif
      return;
    }
#if HAVE_PTHREAD
    mutex_unlock(&tuple->mutex);
#endif
  }
  tuple_clean(tuple);
  free(tuple);
}

s_tuple * tuple_init (s_tuple *tuple, uw count)
{
  uw i;
  assert(tuple);
  assert(2 <= count);
  tuple->count = count;
  tuple->tag = alloc(count * sizeof(s_tag));
  if (! tuple->tag)
    return NULL;
  i = count;
  while (i--)
    tag_init_void(tuple->tag + i);
  tuple->ref_count = 1;
#if HAVE_PTHREAD
  mutex_init(&tuple->mutex);
#endif
  return tuple;
}

s_tuple * tuple_init_1 (s_tuple *tuple, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  assert(tuple);
  assert(p);
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_tuple(&buf, tuple);
  buf_clean(&buf);
  if (r < 0 || (uw) r != len) {
    err_write_1("tuple_init_1: invalid tuple: \"");
    err_write_1(p);
    err_write_1("\", ");
    err_inspect_uw(len);
    err_write_1(" != ");
    err_inspect_sw(r);
    err_write_1("\n");
    assert(! "tuple_init_1: invalid tuple");
    return NULL;
  }
  return tuple;
}

s_tuple * tuple_init_2 (s_tuple *tuple, s_tag *a, s_tag *b)
{
  tuple_init(tuple, 2);
  tag_init_copy(tuple->tag + 0, a);
  tag_init_copy(tuple->tag + 1, b);
  return tuple;
}

s_tuple * tuple_init_cast (s_tuple *tuple, const s_sym * const *type,
                           s_tag *tag)
{
  switch (tag->type) {
  case TAG_PTUPLE:
    return tuple_init_copy(tuple, tag->data.ptuple);
  default:
    break;
  }
  err_write_1("tuple_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Tuple)
    err_puts(" to Tuple");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka Tuple");
  }
  assert(! "tuple_init_cast: cannot cast to Tuple");
  return NULL;
}

s_tuple * tuple_init_copy (s_tuple *tuple, s_tuple *src)
{
  uw i = 0;
  assert(src);
  assert(tuple);
  tuple_init(tuple, src->count);
  while (i < src->count) {
    tag_init_copy(tuple->tag + i, src->tag + i);
    i++;
  }
  return tuple;
}

s_tuple * tuple_new_copy (s_tuple *src)
{
  s_tuple *tuple;
  assert(src);
  tuple = alloc(sizeof(s_tuple));
  if (! tuple)
    return NULL;
  if (! tuple_init_copy(tuple, src)) {
    free(tuple);
    return NULL;
  }
  return tuple;
}

s_tuple * tuple_new_ref (s_tuple *tuple)
{
  assert(tuple);
#if HAVE_PTHREAD
  mutex_lock(&tuple->mutex);
#endif
  if (tuple->ref_count <= 0) {
    err_puts("tuple_new_ref: invalid reference count");
    assert(! "tuple_new_ref: invalid reference count");
    return NULL;
  }
  tuple->ref_count++;
#if HAVE_PTHREAD
  mutex_unlock(&tuple->mutex);
#endif
  return tuple;
}

s_tuple * tuple_new (uw count)
{
  s_tuple *tuple;
  tuple = alloc(sizeof(s_tuple));
  if (! tuple)
    return NULL;
  if (! tuple_init(tuple, count)) {
    free(tuple);
    return NULL;
  }
  return tuple;
}

s_tuple * tuple_new_1 (const char *p)
{
  s_tuple *tuple;
  tuple = alloc(sizeof(s_tuple));
  if (! tuple)
    return NULL;
  if (! tuple_init_1(tuple, p)) {
    free(tuple);
    return NULL;
  }
  return tuple;
}
