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
#include "buf.h"
#include "buf_parse.h"
#include "compare.h"
#include "eval.h"
#include "list.h"
#include "map.h"
#include "tag.h"

s_tag * map_access (const s_map *map, const s_tag *key, s_tag *value)
{
  assert(map);
  assert(key);
  assert(value);
  if (key->type != TAG_SYM) {
    err_puts("map_access: only works with symbol key");
    return NULL;
  }
  return map_get(map, key, value);
}

s_map * map_init_cast (s_map *map, const s_tag *tag)
{
  assert(tag);
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
    if (compare_tag(key, map->key + i) == 0)
      return tag_init_copy(value, map->value + i);
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
  buf_init(&buf, false, len, (char *) p);
  buf.wpos = len;
  r = buf_parse_map(&buf, map);
  if (r < 0 || (uw) r != len) {
    err_write_1("map_init_1: invalid map: \"");
    err_write_1(p);
    err_write_1("\" => ");
    err_inspect_sw(&r);
    err_write_1(" != ");
    err_inspect_uw(&len);
    err_write_1("\n");
    assert(! "map_init_1: invalid map");
    return NULL;
  }
  return map;
}

s_map * map_init_copy (s_map *map, const s_map *src)
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

s_map * map_init_from_lists (s_map *map, const s_list *keys,
                             const s_list *values)
{
  sw i = 0;
  const s_list *k;
  sw len;
  s_map tmp = {0};
  const s_list *v;
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

s_list ** map_map (const s_map *map, const s_fn *fn, s_list **result)
{
  s_list *args;
  uw i = 0;
  s_list **t;
  s_list *tmp;
  assert(map);
  assert(fn);
  assert(result);
  t = &tmp;
  *t = NULL;
  while (i < map->count) {
    args = list_new_copy(map->key + i,
                         list_new_copy(map->value + i, NULL));
    *t = list_new(NULL);
    if (! eval_fn_call(fn, args, &(*t)->tag)) {
      list_delete_all(args);
      list_delete_all(tmp);
      return NULL;
    }
    t = &(*t)->next.data.list;
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

s_map * map_new_from_lists (const s_list *keys, const s_list *values)
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

s_map * map_set (s_map *map, const s_tag *key, const s_tag *value)
{
  uw i = 0;
  while (i < map->count) {
    if (compare_tag(key, map->key + i) == 0) {
      if (! tag_init_copy(map->value + i, value))
        return NULL;
      return map;
    }
    i++;
  }
  return NULL;
}

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

s_map * map_update (const s_map *map, const s_tag *key,
                    const s_tag *value, s_map *dest)
{
  s_map tmp;
  uw i = 0;
  map_init_copy(&tmp, map);
  while (i < map->count) {
    if (compare_tag(key, map->key + i) == 0) {
      if (! tag_init_copy(map->value + i, value))
        goto ko;
      *dest = tmp;
      return dest;
    }
    i++;
  }
 ko:
  map_clean(&tmp);
  return NULL;
}

s_map * map_update_list (const s_map *map, const s_list *alist, s_map *dest)
{
  const s_list *i;
  s_map tmp;
  assert(map);
  map_init_copy(&tmp, map);
  i = alist;
  while (i) {
    assert(i->tag.type == TAG_TUPLE && i->tag.data.tuple.count == 2);
    if (i->tag.type != TAG_TUPLE || i->tag.data.tuple.count != 2) {
      err_puts("map_update_list: not an associative list");
      assert(! "map_update_list: not an associative list");
      goto ko;
    }
    if (! map_set(&tmp, i->tag.data.tuple.tag, i->tag.data.tuple.tag + 1))
      goto ko;
    i = list_next(i);
  }
  *dest = tmp;
  return dest;
 ko:
  map_clean(&tmp);
  return NULL;
}
