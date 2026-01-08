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
#ifndef LIBKC3_MAP_H
#define LIBKC3_MAP_H

#include "types.h"

/* Stack allocation compatible functions, call map_clean after use. */
void    map_clean (s_map *map);
s_map * map_init (s_map *map, uw count);
s_map * map_init_1 (s_map *map, const char *p);
s_map * map_init_cast (s_map *map, p_sym *type, s_tag *tag);
s_map * map_init_copy (s_map *map, s_map *src);
s_map * map_init_from_lists (s_map *map, s_list *keys, s_list *values);
s_map * map_init_from_alist (s_map *map, s_list *alist);

/* Heap allocation functions, call map_delete after use. */
void    map_delete (s_map *map);
s_map * map_new (uw size);
s_map * map_new_1 (const char *p);
s_map * map_new_from_lists (s_list *keys, s_list *values);

/* Operators. */
s_map * map_sort (s_map *map);

/* Observers. */
s_tag *        map_access (const s_map *map, s_list *key,
                           s_tag *value);
s_map *        map_cast (const s_tag *tag, s_map *map);
s_tag *        map_get (const s_map *map, const s_tag *key,
                        s_tag *dest);
const s_sym ** map_get_type (const s_map *map, const s_tag *key,
                             const s_sym **dest);
const s_sym ** map_get_var_type (const s_map *map, const s_tag *key,
                                 const s_sym **dest);
uw *           map_index (const s_map *map, const s_tag *key, uw *dest);
p_list *       map_keys (const s_map *map, p_list *dest);
s_map *        map_put (s_map *map, s_tag *key,
                        s_tag *value, s_map *dest);
/*
s_map *        map_put_list (const s_map *map, const s_list *alist,
                             s_map *dest);
*/

#endif /* LIBKC3_MAP_H */
