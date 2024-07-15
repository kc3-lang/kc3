/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "assert.h"
#include "alloc.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "cow.h"
#include "data.h"
#include "list.h"
#include "sym.h"
#include "tag.h"

void cow_clean (s_cow *cow)
{
  assert(cow);
  list_delete_all(cow->list);
}

void cow_delete (s_cow *cow)
{
  assert(cow);
  cow_clean(cow);
  free(cow);
}

s_cow * cow_freeze (s_cow *cow)
{
  s_list *tmp;
  assert(cow);
  assert(cow->list);
  tmp = list_new_tag_copy(&cow->list->tag, cow->list);
  if (! tmp)
    return NULL;
  cow->list = tmp;
  return cow;
}

s_cow * cow_freeze_copy (s_cow *cow, const s_tag *src)
{
  s_list *tmp;
  assert(cow);
  assert(cow->list);
  tmp = list_new_tag_copy(src, list_next(cow->list));
  if (! tmp)
    return NULL;
  cow->list->next.type = TAG_LIST;
  cow->list->next.data.list = tmp;
  return cow;
}

s_cow * cow_init (s_cow *cow, const s_sym *type)
{
  s_cow tmp = {0};
  assert(cow);
  assert(type);
  tmp.type = type;
  tmp.list = list_new(NULL);
  *cow = tmp;
  return cow;
}

s_cow * cow_init_1 (s_cow *cow, const char *utf8)
{
  s_buf buf;
  uw len;
  sw r;
  len = strlen(utf8);
  buf_init(&buf, false, len, (char *) utf8); // buf is read-only
  buf.wpos = len;
  r = buf_parse_cow(&buf, cow);
  if (r < 0 || (uw) r != len) {
    err_write_1("invalid cow: \"");
    err_write_1(utf8);
    err_write_1("\", ");
    err_inspect_uw(&len);
    err_write_1(" != ");
    err_inspect_sw(&r);
    assert(! "invalid cow");
    return NULL;
  }
  return cow;
}

s_cow * cow_init_cast (s_cow *cow, const s_sym * const *type,
                       const s_tag *tag)
{
  void *data;
  s_cow tmp;
  assert(tag);
  assert(type);
  assert(tag);
  if (*type == &g_sym_Cow) {
    err_puts("cow_init_cast: cannot cast to Cow");
    assert(! "cow_init_cast: cannot cast to Cow");
    return NULL;
  }
  tmp.type = *type;
  tmp.list = list_new(NULL);
  if (! sym_to_tag_type(*type, &tmp.list->tag.type))
    return NULL;
  if (! tag_to_pointer(&tmp.list->tag, *type, &data))
    return NULL;
  if (! data_init_cast(data, type, tag))
    return NULL;
  cow_freeze(&tmp);
  *cow = tmp;
  return cow;
}

s_cow * cow_init_copy (s_cow *cow, const s_cow *src)
{
  s_cow tmp = {0};
  assert(cow);
  assert(src);
  tmp.type = src->type;
  tmp.list = list_new_copy(src->list);
  if (! tmp.list)
    return NULL;
  *cow = tmp;
  return cow;
}

s_cow * cow_init_tag_copy (s_cow *cow, const s_sym *type,
                           const s_tag *src)
{
  assert(cow);
  assert(src);
  const s_sym *src_type;
  s_cow tmp = {0};
  assert(cow);
  assert(src);
  if (type != &g_sym_Tag) {
    if (! tag_type(src, &src_type))
      return NULL;
    if (type != src_type) {
      err_puts("cow_init_tag_copy: type mismatch");
      assert(! "cow_init_tag_copy: type mismatch");
      return NULL;
    }
  }
  tmp.type = type;
  tmp.list = list_new_tag_copy(src, NULL);
  if (! tmp.list)
    return NULL;
  *cow = tmp;
  return cow;
}

s_str * cow_inspect (const s_cow *cow, s_str *dest)
{
  s_buf buf;
  sw size;
  assert(cow);
  assert(dest);
  if ((size = buf_inspect_cow_size(cow)) < 0) {
    err_puts("tag_inspect: size error");
    assert(! "tag_inspect: size error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  if (buf_inspect_cow(&buf, cow) != size) {
    err_puts("tag_inspect: inspect error");
    assert(! "tag_inspect: inspect error");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_cow * cow_new (const s_sym *type)
{
  s_cow *cow;
  cow = alloc(sizeof(s_cow));
  if (! cow)
    return NULL;
  if (! cow_init(cow, type)) {
    free(cow);
    return NULL;
  }
  return cow;
}

s_cow * cow_new_1 (const char *utf8)
{
  s_cow *cow;
  cow = alloc(sizeof(s_cow));
  if (! cow)
    return NULL;
  if (! cow_init_1(cow, utf8)) {
    free(cow);
    return NULL;
  }
  return cow;
}

s_cow * cow_new_cast (const s_sym * const *type, const s_tag *tag)
{
  s_cow *cow;
  cow = alloc(sizeof(s_cow));
  if (! cow)
    return NULL;
  if (! cow_init_cast(cow, type, tag)) {
    free(cow);
    return NULL;
  }
  return cow;
}

s_cow * cow_new_copy (const s_cow *src)
{
  s_cow *cow;
  cow = alloc(sizeof(s_cow));
  if (! cow)
    return NULL;
  if (! cow_init_copy(cow, src)) {
    free(cow);
    return NULL;
  }
  return cow;
}

s_cow * cow_new_tag_copy (const s_sym *type, const s_tag *src)
{
  s_cow *cow;
  cow = alloc(sizeof(s_cow));
  if (! cow)
    return NULL;
  if (! cow_init_tag_copy(cow, type, src)) {
    free(cow);
    return NULL;
  }
  return cow;
}

const s_tag * cow_read_only (const s_cow *cow)
{
  assert(cow);
  assert(cow->list);
  assert(list_next(cow->list));
  return &list_next(cow->list)->tag;
}

s_tag * cow_read_write (s_cow *cow)
{
  assert(cow);
  assert(cow->list);
  return &cow->list->tag;
}

s_cow * cow_thaw (s_cow *cow)
{
  s_tag tmp;
  assert(cow);
  assert(cow->list);
  assert(list_next(cow->list));
  if (! list_next(cow->list)) {
    err_puts("cow_thaw: nothing to thaw");
    assert(! "cow_thaw: nothing to thaw");
    return NULL;
  }
  if (! tag_init_copy(&tmp, &list_next(cow->list)->tag))
    return NULL;
  tag_clean(&cow->list->tag);
  cow->list->tag = tmp;
  return cow;
}

s_cow * cow_thaw_copy (s_cow *cow, const s_tag *src)
{
  s_tag tmp;
  assert(cow);
  assert(cow->list);
  assert(src);
  if (! tag_init_copy(&tmp, src))
    return NULL;
  tag_clean(&cow->list->tag);
  cow->list->tag = tmp;
  return cow;
}
