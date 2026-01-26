/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
 * @file alist.h
 * @brief Associative list
 *
 * Linked list of tuples of key and value.
 */
#ifndef LIBKC3_ALIST_H
#define LIBKC3_ALIST_H

#include "types.h"

/* Observers */
s_tag * alist_access (s_list *alist, s_list *key, s_tag *dest);
s_tag * alist_get (s_list *alist, s_tag *key, s_tag *dest);

#endif /* LIBKC3_ALIST_H */
