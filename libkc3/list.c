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
#include "alloc.h"
#include "assert.h"
#include "array.h"
#include "bool.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "compare.h"
#include "data.h"
#include "eval.h"
#include "kc3_main.h"
#include "list.h"
#include "sym.h"
#include "tag.h"
#include "tuple.h"
#include "uw.h"

s_tag * list_access (s_list *list, s_list *key, s_tag *dest)
{
  s_tag  *key_first;
  s_list *key_next;
  s_tag *r;
  const s_sym *sym_Uw = &g_sym_Uw;
  s_tag tag;
  uw i;
  assert(list);
  assert(key);
  assert(dest);
  key_first = &key->tag;
  if (! uw_init_cast(&i, &sym_Uw, key_first)) {
    err_puts("list_access: invalid key");
    assert(! "list_access: invalid key");
    return NULL;
  }
  key_next = list_next(key);
  if (! key_next)
    return list_at(list, i, dest);
  if (! list_at(list, i, &tag))
    return NULL;
  r = kc3_access(&tag, &key_next, dest);
  tag_clean(&tag);
  return r;
}

s_tag * list_at (s_list *list, uw position, s_tag *dest)
{
  s_list *l;
  uw p;
  assert(list);
  assert(dest);
  l = list;
  p = position;
  while (p) {
    p--;
    l = list_next(l);
    if (! l) {
      err_puts("list_at: position exceeds list length");
      assert(! "list_at: position exceeds list length");
      return NULL;
    }
  }
  return tag_init_copy(dest, &l->tag);
}

void list_clean (s_list *list)
{
  assert(list);
  tag_clean(&list->tag);
  if (list->next.type != TAG_LIST)
    tag_clean(&list->next);
}

s_list * list_delete (s_list *list)
{
  s_list *next = NULL;
  if (list) {
    next = list_next(list);
    list_clean(list);
    free(list);
  }
  return next;
}

void list_delete_all (s_list *list)
{
  while (list)
    list = list_delete(list);
}

void list_f_clean (s_list **list)
{
  s_list *l;
  assert(list);
  l = *list;
  while (l)
    l = list_delete(l);
}

s_list ** list_filter (s_list **list, p_callable *function,
                       s_list **dest)
{
  s_list *arg;
  s_list *l;
  s_list **tail;
  s_list *tmp;
  if (! (arg = list_new(NULL)))
    return NULL;
  tmp = NULL;
  tail = &tmp;
  l = *list;
  while (l) {
    if (! tag_copy(&arg->tag, &l->tag))
      goto ko;
    *tail = list_new(NULL);
    if (! eval_callable_call(*function, arg, &(*tail)->tag))
      goto ko;
    if ((*tail)->tag.type == TAG_VOID)
      *tail = list_delete(*tail);
    else
      tail = &(*tail)->next.data.list;
    l = list_next(l);
  }
  list_delete_all(arg);
  *dest = tmp;
  return dest;
 ko:
  list_delete_all(tmp);
  list_delete_all(arg);
  return NULL;
}

bool * list_has (const s_list * const *list, const s_tag *tag,
                 bool *dest)
{
  const s_list *l;
  l = *list;
  while (l) {
    if (! compare_tag(tag, &l->tag)) {
      *dest = true;
      return dest;
    }
    l = list_next(l);
  }
  *dest = false;
  return dest;
}

s_list * list_init (s_list *list, s_list *next)
{
  assert(list);
  tag_init_void(&list->tag);
  tag_init_list(&list->next, next);
  return list;
}

s_list * list_init_1 (s_list *list, const char *p, s_list *next)
{
  assert(list);
  if (! tag_init_1(&list->tag, p))
    return NULL;
  tag_init_list(&list->next, next);
  return list;
}

s_list ** list_init_append (s_list **list, s_list **src,
                            s_tag *tag)
{
  s_list *s;
  s_list *tmp;
  s_list **tail;
  tmp = NULL;
  tail = &tmp;
  s = *src;
  while (s) {
    *tail = list_new_tag_copy(&s->tag, NULL);
    tail = &(*tail)->next.data.list;
    s = list_next(s);
  }
  *tail = list_new_tag_copy(tag, NULL);
  *list = tmp;
  return list;
}

s_list ** list_init_cast (s_list **list, const s_sym * const *type,
                          s_tag *tag)
{
  assert(list);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_LIST:
    return list_init_copy(list, &tag->data.list);
  default:
    break;
  }
  err_write_1("list_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_List)
    err_puts(" to List");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka List");
  }
  assert(! "list_init_cast: cannot cast to List");
  return NULL;
}

s_list ** list_init_copy (s_list **list, s_list **src)
{
  s_list *tmp = NULL;
  assert(src);
  assert(list);
  if (*src && ! (tmp = list_new_copy(*src)))
    return NULL;
  *list = tmp;
  return list;
}

s_list * list_init_tag_copy (s_list *list, s_tag *tag, s_list *next)
{
  assert(list);
  assert(tag);
  list_init(list, next);
  if (! tag_init_copy(&list->tag, tag))
    return NULL;
  return list;
}

bool list_is_alist (const s_list *list)
{
  const s_list *l;
  l = list;
  while (l) {
    if (l->tag.type != TAG_TUPLE ||
        l->tag.data.tuple.count != 2)
      return false;
    l = list_next(l);
  }
  return true;
}

bool list_is_plist (const s_list *list)
{
  const s_list *l;
  if (! list)
    return false;
  l = list;
  while (l) {
    if (l->tag.type != TAG_TUPLE ||
        l->tag.data.tuple.count != 2 ||
        l->tag.data.tuple.tag[0].type != TAG_SYM)
      return false;
    l = list_next(l);
  }
  return true;
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

s_list ** list_map (s_list **list, p_callable *function,
                    s_list **dest)
{
  s_list *arg;
  s_list *l;
  s_list **tail;
  s_list *tmp;
  if (! (arg = list_new(NULL)))
    return NULL;
  tmp = NULL;
  tail = &tmp;
  l = *list;
  while (l) {
    if (! tag_copy(&arg->tag, &l->tag))
      goto ko;
    *tail = list_new(NULL);
    if (! eval_callable_call(*function, arg, &(*tail)->tag))
      goto ko;
    tail = &(*tail)->next.data.list;
    l = list_next(l);
  }
  list_delete_all(arg);
  *dest = tmp;
  return dest;
 ko:
  list_delete_all(tmp);
  list_delete_all(arg);
  return NULL;
}

s_list * list_next (const s_list *list)
{
  assert(list);
  if (! list)
    return NULL;
  switch (list->next.type) {
  case TAG_LIST: return list->next.data.list;
  default: return NULL;
  }
}

s_list * list_new (s_list *next)
{
  s_list *dest;
  dest = alloc(sizeof(s_list));
  if (! dest)
    return NULL;
  return list_init(dest, next);
}

s_list * list_new_1 (const char *p)
{
  s_buf buf;
  s_list *list;
  buf_init_1_const(&buf, p);
  if (buf_parse_list(&buf, &list) != (sw) strlen(p)) {
    err_puts("list_new_1: invalid list");
    assert(! "list_new_1: invalid list");
    return NULL;
  }
  return list;
}

/* FIXME: does not work on circular lists */
s_list * list_new_copy (s_list *src)
{
  s_list **i;
  s_list *next;
  s_list *s;
  s_list *list;
  list = NULL;
  i = &list;
  s = src;
  while (s) {
    *i = list_new(NULL);
    if (! tag_init_copy(&(*i)->tag, &s->tag))
      goto ko;
    if ((next = list_next(s))) {
      s = next;
      i = &(*i)->next.data.list;
    }
    else {
      if (! tag_init_copy(&(*i)->next, &s->next))
        goto ko;
      break;
    }
  }
  return list;
 ko:
  list_delete_all(list);
  return NULL;
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

s_list * list_new_tag_copy (s_tag *x, s_list *next)
{
  s_list *dest;
  dest = list_new(next);
  if (! dest)
    return NULL;
  if (! tag_init_copy(&dest->tag, x)) {
    free(dest);
    return NULL;
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

s_list ** list_sort (s_list **list, s_list **dest)
{
  s_list *l;
  s_list *new_;
  s_list *tmp;
  s_list **t;
  assert(list);
  assert(dest);
  tmp = NULL;
  l = *list;
  while (l) {
    t = &tmp;
    while (*t && compare_tag(&(*t)->tag, &l->tag) <= 0)
      t = &(*t)->next.data.list;
    if (! (new_ = list_new_tag_copy(&l->tag, *t))) {
      list_delete_all(tmp);
      return NULL;
    }
    *t = new_;
    l = list_next(l);
  }
  *dest = tmp;
  return dest;
}

s_list ** list_sort_by (s_list **list, p_callable *compare,
                        s_list **dest)
{
  s_list *arg1;
  s_list *arg2;
  bool b;
  s_list *l;
  s_list *new_;
  const s_sym *sym_Bool = &g_sym_Bool;
  s_list *tmp;
  s_list **t;
  s_tag tag;
  assert(list);
  assert(dest);
  if (! (arg2 = list_new(NULL)))
    return NULL;
  if (! (arg1 = list_new(arg2))) {
    list_delete(arg2);
    return NULL;
  }
  tmp = NULL;
  l = *list;
  while (l) {
    t = &tmp;
    while (*t) {
      if (! tag_init_copy(&arg1->tag, &(*t)->tag))
        goto ko;
      if (! tag_init_copy(&arg2->tag, &l->tag))
        goto ko;
      if (! eval_callable_call(*compare, arg1, &tag))
        goto ko;
      tag_void(&arg1->tag);
      tag_void(&arg2->tag);
      if (! bool_init_cast(&b, &sym_Bool, &tag)) {
        tag_clean(&tag);
        goto ko;
      }
      tag_clean(&tag);
      if (! b)
        break;
      t = &(*t)->next.data.list;
    }
    if (! (new_ = list_new_tag_copy(&l->tag, *t)))
      goto ko;
    *t = new_;
    l = list_next(l);
  }
  list_delete_all(arg1);
  *dest = tmp;
  return dest;
 ko:
  list_delete_all(tmp);
  list_delete_all(arg1);
  return NULL;
}

s_list ** list_tail (s_list **list)
{
  s_list **tail;
  tail = list;
  while (tail && *tail) {
    tail = &(*tail)->next.data.list;
  }
  return tail;
}

s_array * list_to_array (s_list *list, const s_sym *array_type,
                         s_array *dest)
{
  s8 *data;
  s_list *l;
  uw len;
  bool must_clean;
  uw size;
  s_array tmp = {0};
  assert(list);
  assert(array_type);
  assert(dest);
  len = list_length(list);
  tmp.array_type = array_type;
  tmp.element_type = sym_array_type(array_type);
  if (! sym_type_size(&tmp.element_type, &size))
    return NULL;
  if (! size) {
    err_puts("list_to_array: zero item size");
    assert(! "list_to_array: zero item size");
    return NULL;
  }
  if (len) {
    tmp.dimension = 1;
    tmp.dimensions = alloc(sizeof(s_array_dimension));
    if (! tmp.dimensions)
      return NULL;
    tmp.count = len;
    tmp.dimensions[0].count = len;
    tmp.dimensions[0].item_size = size;
    tmp.size = len * size;
    tmp.free_data = alloc(len * size);
    if (! tmp.free_data) {
      free(tmp.dimensions);
      return NULL;
    }
    data = tmp.data = tmp.free_data;
    l = list;
    while (l) {
      if (! data_init_cast(data, &tmp.element_type, &l->tag))
        goto ko;
      data += size;
      l = list_next(l);
    }
  }
  *dest = tmp;
  return dest;
 ko:
  if (sym_must_clean(tmp.element_type, &must_clean) &&
      must_clean) {
    while (data > (s8 *) tmp.data) {
      data -= size;
      data_clean(tmp.element_type, data);
    }
  }
  free(tmp.data);
  free(tmp.dimensions);
  return NULL;
}

s_list ** list_unique (s_list **list, s_list **dest)
{
  bool found;
  s_list *l;
  s_list **tail;
  s_list *tmp = NULL;
  assert(list);
  assert(dest);
  tail = &tmp;
  l = *list;
  while (l) {
    if (! list_has((const s_list * const *) &tmp, &l->tag, &found))
      goto ko;
    if (! found) {
      if (! (*tail = list_new_tag_copy(&l->tag, NULL)))
        goto ko;
      tail = &(*tail)->next.data.list;
    }
    l = list_next(l);
  }
  *dest = tmp;
  return dest;
 ko:
  list_delete_all(tmp);
  return NULL;
}
