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
/**
 * @file list.h
 * @brief Tag list
 *
 * Linked list of tags.
 */
#ifndef LIBC3_LIST_H
#define LIBC3_LIST_H

#include <stdarg.h>
#include <stdio.h>
#include "hash.h"
#include "types.h"

/* Stack-allocation functions, call list_clean after use. */
void      list_clean (s_list **list);
s_list  * list_init (s_list *list, s_list *next);
s_list ** list_init_copy (s_list **list, const s_list **src);
s_list  * list_init_copy_tag (s_list *list, const s_tag *tag, s_list *next);

/* Heap-allocation functions, call list_delete after use */
s_list * list_delete (s_list *list);
void     list_delete_all (s_list *list);
s_list * list_new (s_list *next);
s_list * list_new_1 (const s8 *p);
s_list * list_new_f64 (f64 x, s_list *next);
s_list * list_new_copy (const s_tag *tag, s_list *next);
s_list * list_new_list (s_list *list, s_list *next);
s_list * list_new_str_1 (s8 *free, const s8 *p, s_list *next);

/* Observers */
s_list ** list_cast (const s_tag *tag, s_list **list);
sw        list_length (const s_list *list);
s_list  * list_next (const s_list *list);
s_array * list_to_array (s_list *list, const s_sym *type,
                         s_array *dest);
s_tuple * list_to_tuple_reverse (const s_list *list, s_tuple *dest);
/* Call str_delete after use. */
s_str   * list_inspect (const s_list *list, s_str *dest);

/* Operators */
s_list ** list_remove_void (s_list **list);

#endif /* LIBC3_LIST_H */
