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
#include "buf.h"
#include "buf_parse.h"
#include "compare.h"
#include "eval.h"
#include "kc3_main.h"
#include "list.h"
#include "map.h"
#include "tag.h"

s_tag * map_access (const s_map *map, s_list *key,
                    s_tag *dest)
{
  s_tag *first;
  s_list *next;
  s_tag *r;
  s_tag tag;
  assert(map);
  assert(key);
  assert(dest);
  first = &key->tag;
  next = list_next(key);
  if (! next) {
    if (! map_get(map, first, dest))
      tag_init_void(dest);
    return dest;
  }
  if (! map_get(map, first, &tag)) {
    err_write_1("map_access: map_get(");
    err_inspect_map(map);
    err_write_1(", ");
    err_inspect_tag(first);
    err_write_1(")\n");
    assert(! "map_access: map_get");
    return NULL;
  }
  r = kc3_access(&tag, &next, dest);
  tag_clean(&tag);
  return r;
}

s_map * map_init_cast (s_map *map, p_sym *type, s_tag *tag)
{
  assert(tag);
  (void) type;
  if (tag->type == TAG_MAP)
    return map_init_copy(map, &tag->data.map);
  err_write_1("map_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Map");
  assert(! "map_init_cast: cannot cast to Map");
  return NULL;
}

void map_clean (s_map *map)
{
  uw i = 0;
  assert(map);
  while (i < map->count) {
    tag_clean(map->key + i);
    tag_clean(map->value + i);
    i++;
  }
  free(map->key);
  free(map->value);
}

void map_delete (s_map *map)
{
  assert(map);
  map_clean(map);
  free(map);
}

s_tag * map_get (const s_map *map, const s_tag *key, s_tag *value)
{
  uw i = 0;
  while (i < map->count) {
    if (! compare_tag(key, map->key + i))
      return tag_init_copy(value, map->value + i);
    i++;
  }
  return NULL;
}

const s_sym ** map_get_type (const s_map *map, const s_tag *key,
                             const s_sym **dest)
{
  uw i = 0;
  while (i < map->count) {
    if (compare_tag(key, map->key + i) == 0)
      return tag_type(map->value + i, dest);
    i++;
  }
  err_write_1("map_get_type: ");
  err_inspect_tag(key);
  err_puts(": key not found");
  return NULL;
}

const s_sym ** map_get_var_type (const s_map *map, const s_tag *key,
                                 const s_sym **dest)
{
  uw i = 0;
  while (i < map->count) {
    if (compare_tag(key, map->key + i) == 0)
      return tag_type_var(map->value + i, dest);
    i++;
  }
  err_write_1("map_get_type: ");
  err_inspect_map(map);
  err_write_1("\nkey not found: ");
  err_inspect_tag(key);
  err_write_1("\n");
  return NULL;
}

uw * map_index (const s_map *map, const s_tag *key, uw *dest)
{
  uw i;
  i = 0;
  while (i < map->count) {
    if (! compare_tag(key, map->key + i)) {
      *dest = i;
      return dest;
    }
    i++;
  }
  return NULL;
}

s_map * map_init (s_map *map, uw count)
{
  s_map tmp = {0};
  assert(map);
  if (count) {
    tmp.count = count;
    tmp.key = alloc(count * sizeof(s_tag));
    if (! tmp.key)
      return NULL;
    tmp.value = alloc(count * sizeof(s_tag));
    if (! tmp.value) {
      free(tmp.key);
      return NULL;
    }
  }
  *map = tmp;
  return map;
}

s_map * map_init_1 (s_map *map, const char *p)
{
  s_buf buf;
  uw len;
  sw r;
  assert(map);
  assert(p);
  len = strlen(p);
  buf_init_const(&buf, len, p);
  buf.wpos = len;
  r = buf_parse_map(&buf, map);
  buf_clean(&buf);
  if (r < 0 || (uw) r != len) {
    err_write_1("map_init_1: invalid map: \"");
    err_write_1(p);
    err_write_1("\" => ");
    err_inspect_sw(r);
    err_write_1(" != ");
    err_inspect_uw(len);
    err_write_1("\n");
    assert(! "map_init_1: invalid map");
    return NULL;
  }
  return map;
}

s_map * map_init_copy (s_map *map, s_map *src)
{
  uw i = 0;
  s_map tmp = {0};
  assert(src);
  assert(map);
  if (! map_init(&tmp, src->count))
    return NULL;
  while (i < tmp.count) {
    if (! tag_init_copy(tmp.key + i, src->key + i) ||
        ! tag_init_copy(tmp.value + i, src->value + i))
      goto ko;
    i++;
  }
  *map = tmp;
  return map;
 ko:
  map_clean(&tmp);
  return NULL;
}

s_map * map_init_from_alist (s_map *map, s_list *alist)
{
  sw i = 0;
  s_list *a;
  sw len;
  s_map tmp = {0};
  assert(map);
  len = list_length(alist);
  if (! list_is_alist(alist)) {
    err_write_1("map_init_from_alist: not an alist: ");
    err_inspect_list(alist);
    err_write_1("\n");
    assert(! "map_init_from_alist: not an alist");
  }
  map_init(&tmp, len);
  a = alist;
  while (i < len) {
    if (! tag_init_copy(tmp.key + i, a->tag.data.tuple.tag) ||
        ! tag_init_copy(tmp.value + i, a->tag.data.tuple.tag + 1))
      goto ko;
    a = list_next(a);
    i++;
  }
  if (! map_sort(&tmp))
    goto ko;
  *map = tmp;
  return map;
 ko:
  map_clean(&tmp);
  return NULL;
}

s_map * map_init_from_lists (s_map *map, s_list *keys,
                             s_list *values)
{
  sw i = 0;
  s_list *k;
  sw len;
  s_map tmp = {0};
  s_list *v;
  assert(map);
  if ((len = list_length(keys)) != list_length(values)) {
    err_puts("map_init_from_lists:"
             " keys and values length do not match");
    assert(! "map_init_from_lists:"
             " keys and values length do not match");
    return NULL;
  }
  map_init(&tmp, len);
  k = keys;
  v = values;
  while (i < len) {
    if (! tag_init_copy(tmp.key + i, &k->tag) ||
        ! tag_init_copy(tmp.value + i, &v->tag))
      goto ko;
    k = list_next(k);
    v = list_next(v);
    i++;
  }
  if (! map_sort(&tmp))
    goto ko;
  *map = tmp;
  return map;
 ko:
  map_clean(&tmp);
  return NULL;
}

s_list ** map_map (const s_map *map, s_callable *callable,
                   s_list **result)
{
  s_list *args = NULL;
  uw i = 0;
  s_list **t = NULL;
  s_list *tmp = NULL;
  assert(map);
  assert(callable);
  assert(result);
  t = &tmp;
  *t = NULL;
  while (i < map->count) {
    args = list_new_tag_copy(map->key + i,
                             list_new_tag_copy(map->value + i, NULL));
    *t = list_new(NULL);
    if (! eval_callable_call(callable, args, &(*t)->tag)) {
      list_delete_all(args);
      list_delete_all(tmp);
      return NULL;
    }
    t = &(*t)->next.data.plist;
    list_delete_all(args);
    i++;
  }
  *result = tmp;
  return result;
}

s_map * map_new (uw count)
{
  s_map *map;
  map = alloc(sizeof(s_map));
  if (! map)
    return NULL;
  if (! map_init(map, count)) {
    free(map);
    return NULL;
  }
  return map;
}

s_map * map_new_1 (const char *p)
{
  s_map *map;
  map = alloc(sizeof(s_map));
  if (! map)
    return NULL;
  if (! map_init_1(map, p)) {
    free(map);
    return NULL;
  }
  return map;
}

s_map * map_new_from_lists (s_list *keys, s_list *values)
{
  s_map *map;
  map = alloc(sizeof(s_map));
  if (! map)
    return NULL;
  if (! map_init_from_lists(map, keys, values)) {
    free(map);
    return NULL;
  }
  return map;
  
}

s_map * map_put (s_map *map, s_tag *key,
                 s_tag *value, s_map *dest)
{
  s_map tmp;
  uw i;
  uw j;
  if (map_index(map, key, &i)) {
    if (! map_init(&tmp, map->count))
      return NULL;
    j = 0;
    while (j < tmp.count) {
      if (! tag_init_copy(tmp.key + j, map->key + j))
        goto ko;
      if (i == j) {
        if (! tag_init_copy(tmp.value + j, value))
          goto ko;
      }
      else {
        if (! tag_init_copy(tmp.value + j, map->value + j))
          goto ko;
      }
      j++;
    }
  }
  else {
    if (! map_init(&tmp, map->count + 1))
      return NULL;
    j = 0;
    while (j < map->count) {
      if (! tag_init_copy(tmp.key + j, map->key + j))
        goto ko;
      if (! tag_init_copy(tmp.value + j, map->value + j))
        goto ko;
      j++;
    }
    if (! tag_init_copy(tmp.key + j, key))
      goto ko;
    if (! tag_init_copy(tmp.value + j, value))
      goto ko;
    if (! map_sort(&tmp))
      goto ko;
  }
  *dest = tmp;
  return dest;
 ko:
  map_clean(&tmp);
  return NULL;
}

/*
s_map * map_put_list (const s_map *map, const s_list *alist,
                      s_map *dest)
{
  const s_list *i = NULL;
  s_map tmp = {0};
  assert(map);
  map_init_copy(&tmp, map);
  i = alist;
  while (i) {
    assert(i->tag.type == TAG_TUPLE && i->tag.data.tuple.count == 2);
    if (i->tag.type != TAG_TUPLE || i->tag.data.tuple.count != 2) {
      err_puts("map_put_list: not an associative list");
      assert(! "map_put_list: not an associative list");
      goto ko;
    }
    if (! map_set(&tmp, i->tag.data.tuple.tag,
                  i->tag.data.tuple.tag + 1))
      goto ko;
    i = list_next(i);
  }
  *dest = tmp;
  return dest;
 ko:
  map_clean(&tmp);
  return NULL;
}
*/

/* bubble sort */
s_map * map_sort (s_map *map)
{
  uw i;
  uw j;
  s_tag k;
  s_tag v;
  i = map->count;
  while (i > 0) {
    i--;
    j = 1;
    while (j <= i) {
      if (compare_tag(map->key + j, map->key + (j - 1)) < 0) {
        k = map->key[j];
        v = map->value[j];
        map->key[j] = map->key[j - 1];
        map->value[j] = map->value[j - 1];
        map->key[j - 1] = k;
        map->value[j - 1] = v;
      }
      j++;
    }
  }
  return map;
}
