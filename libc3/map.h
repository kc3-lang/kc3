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
#ifndef LIBC3_MAP_H
#define LIBC3_MAP_H

#include "types.h"

/* Stack allocation compatible functions, call map_clean after use. */
void    map_clean (s_map *map);
s_map * map_init (s_map *map, uw size);
s_map * map_init_1 (s_map *map, const s8 *p);
s_map * map_init_from_lists (s_map *map, const s_list *keys,
                             const s_list *values);

/* Heap allocation functions, call map_delete after use. */
void    map_delete (s_map *map);
s_map * map_new (uw size);
s_map * map_new_1 (const s8 *p);
s_map * map_new_from_lists (const s_list *keys, const s_list *values);

/* Modifiers */
s_map * map_set (s_map *map, const s_tag *key, const s_tag *value);
s_map * map_sort (s_map *map);

/* Observers */
s_map * map_copy (const s_map *src, s_map *dest);

/* Operators */
s_tag * map_access (const s_map *map, const s_tag *key, s_tag *value);
s_map * map_cast (const s_tag *tag, s_map *map);
s_tag * map_get (const s_map *map, const s_tag *key, s_tag *dest);
s_map * map_update (const s_map *map, const s_tag *key,
                    const s_tag *value, s_map *dest);
s_map * map_update_list (const s_map *map, const s_list *alist,
                         s_map *dest);

#endif /* LIBC3_MAP_H */
