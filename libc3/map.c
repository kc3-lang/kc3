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
#include <err.h>
#include <stdlib.h>
#include <string.h>
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
    warnx("map_access: only works with symbol key");
    return NULL;
  }
  return map_get(map, key, value);
}

s_map * map_cast (const s_tag *tag, s_map *map)
{
  assert(tag);
  if (tag->type == TAG_MAP) {
    map_init_copy(map, &tag->data.map);
    return map;
  }
  return NULL;
}

void map_clean (s_map *map)
{
  uw i = 0;
  assert(map);
  while (i < map->count) {
    tag_clean(map->keys + i);
    tag_clean(map->values + i);
    i++;
  }
  free(map->keys);
  free(map->values);
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
    if (compare_tag(key, map->keys + i) == 0)
      return tag_init_copy(value, map->values + i);
    i++;
  }
  return NULL;
}

s_map * map_init (s_map *map, uw count)
{
  assert(map);
  map->count = count;
  map->keys = calloc(count, sizeof(s_tag));
  map->values = calloc(count, sizeof(s_tag));
  return map;
}

s_map * map_init_1 (s_map *map, const s8 *p)
{
  s_buf buf;
  sw r;
  assert(map);
  assert(p);
  buf_init_1(&buf, false, (s8 *) p);
  if ((r = buf_parse_map(&buf, map)) != (sw) strlen(p)) {
    assert(! "invalid map");
    warnx("invalid map: \"%s\" (%ld)", p, r);
    return NULL;
  }
  return map;
}

s_map * map_init_copy (s_map *map, const s_map *src)
{
  uw i = 0;
  assert(src);
  assert(map);
  map_init(map, src->count);
  while (i < src->count) {
    tag_init_copy(map->keys + i, src->keys + i);
    tag_init_copy(map->values + i, src->values + i);
    i++;
  }
  return map;
}

s_map * map_init_from_lists (s_map *map, const s_list *keys,
                             const s_list *values)
{
  sw i = 0;
  const s_list *k;
  sw len;
  const s_list *v;
  assert(map);
  if ((len = list_length(keys)) != list_length(values)) {
    warnx("map_init_from_lists: list length don't match");
    return NULL;
  }
  map_init(map, len);
  k = keys;
  v = values;
  while (i < len) {
    if (! tag_init_copy(map->keys + i, &k->tag) ||
        ! tag_init_copy(map->values + i, &v->tag))
      goto ko;
    k = list_next(k);
    v = list_next(v);
    i++;
  }
  if (! map_sort(map))
    goto ko;
  return map;
 ko:
  map_clean(map);
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
    args = list_new_copy(map->keys + i,
                         list_new_copy(map->values + i, NULL));
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
  if (! (map = malloc(sizeof(s_map)))) {
    warn("map_new");
    return NULL;
  }
  return map_init(map, count);
}

s_map * map_new_1 (const s8 *p)
{
  s_map *map;
  if (! (map = malloc(sizeof(s_map)))) {
    warn("map_new");
    return NULL;
  }
  return map_init_1(map, p);
}

s_map * map_new_from_lists (const s_list *keys, const s_list *values)
{
  s_map *map;
  if (! (map = malloc(sizeof(s_map)))) {
    warn("map_new");
    return NULL;
  }
  return map_init_from_lists(map, keys, values);
}

s_map * map_set (s_map *map, const s_tag *key, const s_tag *value)
{
  uw i = 0;
  while (i < map->count) {
    if (compare_tag(key, map->keys + i) == 0) {
      if (! tag_init_copy(map->values + i, value))
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
      if (compare_tag(map->keys + j, map->keys + (j - 1)) < 0) {
        k = map->keys[j];
        v = map->values[j];
        map->keys[j] = map->keys[j - 1];
        map->values[j] = map->values[j - 1];
        map->keys[j - 1] = k;
        map->values[j - 1] = v;
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
    if (compare_tag(key, map->keys + i) == 0) {
      if (! tag_init_copy(map->values + i, value))
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
      warnx("map_update_list: not an associative list");
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
