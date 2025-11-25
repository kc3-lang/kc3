/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "plist.h"
#include "str.h"
#include "sym.h"
#include "sw.h"
#include "tag.h"
#include "tuple.h"
#include "uw.h"

bool * plist_all (p_list *plist, p_callable *function, bool *dest)
{
  s_list *arg;
  bool b;
  s_list *l;
  const s_sym *sym_Bool = &g_sym_Bool;
  s_tag tmp;
  if (! (arg = list_new(NULL)))
    return NULL;
  l = *plist;
  while (l) {
    if (! tag_copy(&arg->tag, &l->tag))
      goto ko;
    if (! eval_callable_call(*function, arg, &tmp))
      goto ko;
    if (! bool_init_cast(&b, &sym_Bool, &tmp))
      goto ko;
    tag_void(&tmp);
    if (! b) {
      *dest = false;
      list_delete_all(arg);
      return dest;
    }
    l = list_next(l);
  }
  list_delete_all(arg);
  *dest = true;
  return dest;
 ko:
  tag_clean(&tmp);
  list_delete_all(arg);
  return NULL;
  
}

void plist_clean (p_list *plist)
{
  list_delete_all(*plist);
}

bool * plist_each (p_list *plist, p_callable *function, bool *dest)
{
  s_list *arg;
  s_list *l;
  s_tag tmp;
  if (! (arg = list_new(NULL)))
    return NULL;
  l = *plist;
  while (l) {
    if (! tag_copy(&arg->tag, &l->tag))
      goto ko;
    if (! eval_callable_call(*function, arg, &tmp))
      goto ko;
    tag_clean(&tmp);
    l = list_next(l);
  }
  list_delete_all(arg);
  *dest = true;
  return dest;
 ko:
  tag_clean(&tmp);
  list_delete_all(arg);
  return NULL;
}

p_list * plist_filter (p_list *plist, p_callable *function,
                       p_list *dest)
{
  s_list *arg;
  bool b;
  s_list *l;
  const s_sym *sym_Bool = &g_sym_Bool;
  p_list *tail;
  s_list *tmp;
  if (! (arg = list_new(NULL)))
    return NULL;
  tmp = NULL;
  tail = &tmp;
  l = *plist;
  while (l) {
    if (! tag_copy(&arg->tag, &l->tag))
      goto ko;
    *tail = list_new(NULL);
    if (! eval_callable_call(*function, arg, &(*tail)->tag))
      goto ko;
    if (! bool_init_cast(&b, &sym_Bool, &(*tail)->tag))
      goto ko;
    if (b)
      tail = &(*tail)->next.data.plist;
    else
      *tail = list_delete(*tail);
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

s_tag * plist_find_if (p_list *plist, p_callable *function,
                       s_tag *dest)
{
  s_list *arg;
  bool b;
  s_list *list;
  const s_sym *sym_Bool = &g_sym_Bool;
  s_tag tmp = {0};
  if (! (arg = list_new(NULL)))
    return NULL;
  tag_init_bool(&tmp, false);
  list = *plist;
  while (list) {
    arg->tag = list->tag;
    if (! eval_callable_call(*function, arg, &tmp)) {
      tag_init_void(&arg->tag);
      break;
    }
    tag_init_void(&arg->tag);
    if (! bool_init_cast(&b, &sym_Bool, &tmp)) {
      tag_bool(&tmp, false);
      break;
    }
    if (b)
      break;
    tag_void(&tmp);
    tag_bool(&tmp, false);
    list = list_next(list);
  }
  *dest = tmp;
  list_delete_all(arg);
  return dest;
}

bool * plist_has (const s_list * const *plist, const s_tag *tag,
                  bool *dest)
{
  const s_list *l;
  l = *plist;
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

p_list * plist_init_1 (p_list *plist, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  p_list tmp = NULL;
  assert(plist);
  assert(p);
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_plist(&buf, &tmp);
  if (r < 0 || (uw) r != len) {
    err_puts("plist_init_1: invalid list");
    assert(! "plist_init_1: invalid list");
    if (r > 0)
      plist_clean(&tmp);
    return NULL;
  }
  *plist = tmp;
  return plist;
}

p_list * plist_init_append (p_list *plist, p_list *a, p_list *b)
{
  p_list src;
  p_list tmp;
  p_list *tail;
  assert(plist);
  assert(a);
  assert(b);
  if (false) {
    err_write_1("plist_init_append: a = ");
    err_inspect_uw_hexadecimal((uw) *a);
    err_write_1(" b = ");
    err_inspect_uw_hexadecimal((uw) *b);
    err_write_1("\n");
  }
  tmp = NULL;
  tail = &tmp;
  src = *a;
  while (src) {
    if (! (*tail = list_new_tag_copy(&src->tag, NULL)))
      return NULL;
    tail = &(*tail)->next.data.plist;
    src = list_next(src);
  }
  src = *b;
  while (src) {
    if (! (*tail = list_new_tag_copy(&src->tag, NULL)))
      return NULL;
    tail = &(*tail)->next.data.plist;
    src = list_next(src);
  }
  *plist = tmp;
  return plist;
}

p_list * plist_init_append_one (p_list *plist, p_list *a, s_tag *b)
{
  p_list list;
  p_list tmp;
  p_list *tail;
  tmp = NULL;
  tail = &tmp;
  list = *a;
  while (list) {
    *tail = list_new_tag_copy(&list->tag, NULL);
    tail = &(*tail)->next.data.plist;
    list = list_next(list);
  }
  *tail = list_new_tag_copy(b, NULL);
  tail = &(*tail)->next.data.plist;
  *plist = tmp;
  return plist;
}

p_list * plist_init_cast (p_list *plist, const s_sym * const *type,
                          s_tag *tag)
{
  assert(plist);
  assert(type);
  assert(tag);
  switch (tag->type) {
  case TAG_PLIST:
    return plist_init_copy(plist, &tag->data.plist);
  default:
    break;
  }
  err_write_1("plist_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_List)
    err_puts(" to List");
  else {
    err_write_1(" to ");
    err_inspect_psym(type);
    err_puts(" aka List");
  }
  assert(! "plist_init_cast: cannot cast to List");
  return NULL;
}

p_list * plist_init_copy (p_list *plist, p_list *src)
{
  s_list *tmp = NULL;
  assert(src);
  assert(plist);
  if (*src && ! (tmp = list_new_copy_all(*src)))
    return NULL;
  *plist = tmp;
  return plist;
}

p_list * plist_init_count (p_list *plist, s_tag *number)
{
  uw i;
  uw number_uw;
  p_sym sym_Uw = &g_sym_Uw;
  s_list **tail;
  s_list *tmp = NULL;
  assert(plist);
  assert(number);
  if (! uw_init_cast(&number_uw, &sym_Uw, number)) {
    err_puts("plist_init_count: cannot cast to Uw");
    assert(! "plist_init_count: cannot cast to Uw");
    return NULL;
  }
  tail = &tmp;
  i = 0;
  while (i++ < number_uw) {
    if (! (*tail = list_new_uw_reduce(i, NULL))) {
      err_puts("plist_init_count: list_new_uw_reduce");
      assert(! "plist_init_count: list_new_uw_reduce");
      list_delete_all(tmp);
      return NULL;
    }
    tail = &(*tail)->next.data.plist;
  }
  *plist = tmp;
  return plist;
}

s_str * plist_join (p_list *plist, s_str *sep, s_str *dest)
{
  p_list i;
  p_list *tail;
  p_list tmp = NULL;
  i = *plist;
  tail = &tmp;
  while (i) {
    if (! (*tail = list_new_tag_copy(&i->tag, NULL)))
      goto ko;
    tail = &(*tail)->next.data.plist;
    if (list_next(i)) {
      if (! (*tail = list_new_str_copy(sep, NULL)))
        goto ko;
      tail = &(*tail)->next.data.plist;
    }
    i = list_next(i);
  }
  if (! str_init_concatenate_list(dest, tmp))
    goto ko;
  list_delete_all(tmp);
  return dest;
 ko:
  list_delete_all(tmp);
  return NULL;
}

s_tag * plist_last (p_list *plist, s_tag *dest)
{
  s_list *l;
  s_list *last;
  s_tag tag = {0};
  last = NULL;
  l = *plist;
  while (l) {
    last = l;
    l = list_next(l);
  }
  if (last) {
    if (! tag_init_copy(&tag, &last->tag))
      return NULL;
  }
  *dest = tag;
  return dest;
}

p_list * plist_map (p_list *plist, p_callable *function,
                    p_list *dest)
{
  s_list *arg;
  s_list *l;
  p_list *tail;
  s_list *tmp;
  if (! (arg = list_new(NULL)))
    return NULL;
  tmp = NULL;
  tail = &tmp;
  l = *plist;
  while (l) {
    if (! tag_copy(&arg->tag, &l->tag))
      goto ko;
    *tail = list_new(NULL);
    if (! eval_callable_call(*function, arg, &(*tail)->tag))
      goto ko;
    tail = &(*tail)->next.data.plist;
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

p_list * plist_remove_void (p_list *plist)
{
  s_list *tmp;
  p_list *l;
  assert(plist);
  tmp = *plist;
  l = &tmp;
  while (*l) {
    if ((*l)->tag.type == TAG_VOID)
      *l = list_delete(*l);
    else if ((*l)->next.type == TAG_PLIST)
      l = &(*l)->next.data.plist;
    else
      break;
  }
  *plist = tmp;
  return plist;
}

p_list * plist_reverse (p_list *plist, p_list *dest)
{
  p_list acc = NULL;
  p_list i;
  i = *plist;
  while (i) {
    if (! (acc = list_new_tag_copy(&i->tag, acc))) {
      list_delete_all(acc);
      return NULL;
    }
    i = list_next(i);
  }
  *dest = acc;
  return dest;
}

p_list * plist_slice (p_list *plist, s_tag *start_tag, s_tag *end_tag,
                      p_list *dest)
{
  const s_sym *sym_Sw = &g_sym_Sw;
  sw end;
  sw i;
  s_list *l;
  sw start;
  p_list *tail;
  s_list *tmp = NULL;
  assert(plist);
  assert(start_tag);
  assert(end_tag);
  assert(dest);
  if (! sw_init_cast(&start, &sym_Sw, start_tag))
    return NULL;
  if (! sw_init_cast(&end, &sym_Sw, end_tag))
    return NULL;
  tail = &tmp;
  i = 0;
  l = *plist;
  while (l && i < end) {
    if (i >= start) {
      *tail = list_new_tag_copy(&l->tag, NULL);
      tail = &(*tail)->next.data.plist;
    }
    i++;
    l = list_next(l);
  }
  *dest = tmp;
  return dest;
}

p_list * plist_sort (p_list *plist, p_list *dest)
{
  s_list *l;
  s_list *new_;
  s_list *tmp;
  p_list *t;
  assert(plist);
  assert(dest);
  tmp = NULL;
  l = *plist;
  while (l) {
    t = &tmp;
    while (*t && compare_tag(&(*t)->tag, &l->tag) <= 0)
      t = &(*t)->next.data.plist;
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

p_list * plist_sort_by (p_list *plist, p_callable *compare,
                        p_list *dest)
{
  s_list *arg1;
  s_list *arg2;
  bool b;
  s_list *l;
  s_list *new_;
  const s_sym *sym_Bool = &g_sym_Bool;
  s_list *tmp;
  p_list *t;
  s_tag tag;
  assert(plist);
  assert(dest);
  if (! (arg2 = list_new(NULL)))
    return NULL;
  if (! (arg1 = list_new(arg2))) {
    list_delete(arg2);
    return NULL;
  }
  tmp = NULL;
  l = *plist;
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
      t = &(*t)->next.data.plist;
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

p_list * plist_tail (p_list *plist)
{
  p_list *tail;
  tail = plist;
  while (tail && *tail) {
    tail = &(*tail)->next.data.plist;
  }
  return tail;
}

p_list * plist_unique (p_list *plist, p_list *dest)
{
  bool found;
  s_list *l;
  p_list *tail;
  s_list *tmp = NULL;
  assert(plist);
  assert(dest);
  tail = &tmp;
  l = *plist;
  while (l) {
    if (! plist_has((const s_list * const *) &tmp, &l->tag, &found))
      goto ko;
    if (! found) {
      if (! (*tail = list_new_tag_copy(&l->tag, NULL)))
        goto ko;
      tail = &(*tail)->next.data.plist;
    }
    l = list_next(l);
  }
  *dest = tmp;
  return dest;
 ko:
  list_delete_all(tmp);
  return NULL;
}
