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
/**
 * @file list.h
 * @brief Tag list
 *
 * Linked list of tags.
 */
#ifndef LIBKC3_LIST_H
#define LIBKC3_LIST_H

#include "types.h"
#include "list_init.h"

/* Stack-allocation functions, call list_clean after use. */
void      list_clean (s_list *list);
s_list  * list_init (s_list *list, s_list *next);
s_list  * list_init_1 (s_list *list, const char *p, s_list *next);
s_list ** list_init_append (s_list **list, s_list **src,
                            s_tag *tag);
s_list ** list_init_cast (s_list **list, const s_sym * const *type,
                          s_tag *tag);
s_list ** list_init_copy (s_list **list, s_list **src);
s_list  * list_init_eval (s_list *list, const char *p);
s_list  * list_init_tag_copy (s_list *list, s_tag *tag,
                              s_list *next);

/* Heap-allocation functions, call list_delete after use */
s_list * list_delete (s_list *list);
void     list_delete_all (s_list *list);
void     list_f_clean (s_list **list);
s_list * list_new (s_list *next);
s_list * list_new_1 (const char *p);
s_list * list_new_copy (s_list *src);
s_list * list_new_f64 (f64 x, s_list *next);
s_list * list_new_list (s_list *list, s_list *next);
s_list * list_new_map (uw count, s_list *next);
s_list * list_new_str_1 (char *free, const char *p, s_list *next);
s_list * list_new_tag_copy (s_tag *tag, s_list *next);

/* Observers */
s_tag *   list_at (s_list *list, uw position, s_tag *dest);
s_tag *   list_access (s_list *list, s_list *key, s_tag *dest);
s_list ** list_cast (const s_tag *tag, s_list **list);
bool *    list_has (const s_list * const *list, const s_tag *tag,
                    bool *dest);
bool      list_is_alist (const s_list *list);
bool      list_is_plist (const s_list *list);
sw        list_length (const s_list *list);
s_list  * list_next (const s_list *list);
s_list ** list_slice (s_list **list, s_tag *start, s_tag *end,
                      s_list **dest);
s_list ** list_sort (s_list **list, s_list **dest);
s_list ** list_tail (s_list **list);
s_tuple * list_to_tuple_reverse (const s_list *list, s_tuple *dest);

/* Operators */
bool *    list_each (s_list **list, p_callable *function, bool *dest);
s_list ** list_filter (s_list **list, p_callable *function,
                       s_list **dest);
s_list ** list_map (s_list **list, p_callable *function, s_list **dest);
s_list ** list_remove_void (s_list **list);
s_list ** list_sort_by (s_list **list, p_callable *compare,
                        s_list **dest);
s_array * list_to_array (s_list *list, const s_sym *type,
                         s_array *dest);
s_list ** list_unique (s_list **list, s_list **dest);

#endif /* LIBKC3_LIST_H */
