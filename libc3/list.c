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
#include <stdlib.h>
#include <string.h>
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "list.h"
#include "sym.h"
#include "tag.h"
#include "tuple.h"

void list_clean (s_list **list)
{
  s_list *l;
  s_list *next;
  assert(list);
  l = *list;
  while (l) {
    tag_clean(&l->tag);
    next = list_next(l);
    if (l->next.type != TAG_LIST)
      tag_clean(&l->next);
    free(l);
    l = next;
  }
}

s_list ** list_cast (const s_tag *tag, s_list **list)
{
  assert(tag);
  if (tag->type == TAG_LIST) {
    list_init_copy(list, (const s_list **) &tag->data.list);
    return list;
  }
  return NULL;
}

s_list * list_delete (s_list *list)
{
  s_list *next = NULL;
  if (list) {
    next = list_next(list);
    tag_clean(&list->tag);
    next = list_next(list);
    if (list->next.type != TAG_LIST)
      tag_clean(&list->next);
    free(list);
  }
  return next;
}

void list_delete_all (s_list *list)
{
  while (list)
    list = list_delete(list);
}

s_list * list_init (s_list *list, s_list *next)
{
  assert(list);
  tag_init_void(&list->tag);
  tag_init_list(&list->next, next);
  return list;
}

s_list * list_init_1 (s_list *list, const s8 *p, s_list *next)
{
  assert(list);
  tag_init_1(&list->tag, p);
  tag_init_list(&list->next, next);
  return list;
}

/* FIXME: does not work on circular lists */
s_list ** list_init_copy (s_list **list, const s_list **src)
{
  s_list **i;
  s_list *next;
  const s_list *s;
  assert(src);
  assert(list);
  i = list;
  *i = NULL;
  s = *src;
  while (s) {
    *i = list_new(NULL);
    tag_init_copy(&(*i)->tag, &s->tag);
    if ((next = list_next(s))) {
      s = next;
      i = &(*i)->next.data.list;
    }
    else {
      tag_init_copy(&(*i)->next, &s->next);
      break;
    }
  }
  return list;
}

s_list * list_init_copy_tag (s_list *list, const s_tag *tag, s_list *next)
{
  assert(list);
  assert(tag);
  list_init(list, next);
  if (! tag_init_copy(&list->tag, tag))
    return NULL;
  return list;
}

s_str * list_inspect (const s_list *x, s_str *dest)
{
  s_buf buf;
  sw r;
  sw size;
  size = buf_inspect_list_size(&x);
  buf_init_alloc(&buf, size);
  if ((r = buf_inspect_list(&buf, &x)) < 0)
    goto error;
  assert(r == size);
  if (r != size)
    goto error;
  return buf_to_str(&buf, dest);
 error:
  buf_clean(&buf);
  return NULL;
}

sw list_length (const s_list *list)
{
  sw length = 0;
  while (list) {
    length++;
    list = list_next(list);
  }
  return length;
}

s_list * list_next (const s_list *list)
{
  assert(list);
  switch (list->next.type) {
  case TAG_LIST: return list->next.data.list;
  default: return NULL;
  }
}

s_list * list_new (s_list *next)
{
  s_list *dest;
  if (! (dest = calloc(1, sizeof(s_list)))) {
    warnx("list_new: out of memory");
    return NULL;
  }
  return list_init(dest, next);
}

s_list * list_new_1 (const s8 *p)
{
  s_buf buf;
  s_list *list;
  buf_init_1(&buf, p);
  if (buf_parse_list(&buf, &list) != (sw) strlen(p)) {
    assert(! "invalid list");
    buf_clean(&buf);
    return NULL;
  }
  buf_clean(&buf);
  return list;
}

s_list * list_new_copy (const s_tag *x, s_list *next)
{
  s_list *dest;
  if ((dest = list_new(next))) {
    if (! tag_init_copy(&dest->tag, x)) {
      free(dest);
      return NULL;
    }
  }
  return dest;
}

s_list * list_new_f64 (f64 x, s_list *next)
{
  s_list *dest;
  if ((dest = list_new(next))) {
    if (! tag_init_f64(&dest->tag, x)) {
      free(dest);
      return NULL;
    }
  }
  return dest;
}

s_list * list_new_list (s_list *x, s_list *next)
{
  s_list *dest;
  if ((dest = list_new(next))) {
    if (! tag_init_list(&dest->tag, x)) {
      free(dest);
      return NULL;
    }
  }
  return dest;
}

s_list * list_new_map (uw count, s_list *next)
{
  s_list *dest;
  if ((dest = list_new(next))) {
    if (! tag_init_map(&dest->tag, count)) {
      free(dest);
      return NULL;
    }
  }
  return dest;
}

s_list * list_new_str_1 (s8 *x_free, const s8 *x, s_list *next)
{
  s_list *dest;
  if ((dest = list_new(next))) {
    if (! tag_init_str_1(&dest->tag, x_free, x)) {
      free(dest);
      return NULL;
    }
  }
  return dest;
}

s_array * list_to_array (s_list *list, const s_sym *type,
                         s_array *dest)
{
  f_init_copy init_copy;
  s8 *data;
  void *data_list;
  s_list *l;
  uw len;
  uw size;
  assert(list);
  assert(dest);
  len = list_length(list);
  size = sym_type_size(type);
  dest->dimension = 1;
  dest->type = type;
  if (! (dest->dimensions = calloc(1, sizeof(s_array_dimension))))
    errx(1, "list_to_array: out of memory: 1");
  dest->count = len;
  dest->dimensions[0].count = len;
  dest->dimensions[0].item_size = size;
  dest->size = len * size;
  if (! (data = dest->data = calloc(len, size)))
    errx(1, "list_to_array: out of memory: 2");
  init_copy = sym_to_init_copy(type);
  l = list;
  while (l) {
    data_list = tag_to_pointer(&l->tag, type);
    init_copy(data, data_list);
    data += size;
    l = list_next(l);
  }
  return dest;
}

s_list ** list_remove_void (s_list **list)
{
  s_list *tmp;
  s_list **l;
  assert(list);
  tmp = *list;
  l = &tmp;
  while (*l) {
    if ((*l)->tag.type == TAG_VOID)
      *l = list_delete(*l);
    else if ((*l)->next.type == TAG_LIST)
      l = &(*l)->next.data.list;
    else
      break;
  }
  *list = tmp;
  return list;
}
