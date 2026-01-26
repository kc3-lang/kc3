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
 * @file do_block.h
 * @brief A block of code (a tuple of tags).
 *
 * A do_block is a tuple of tags representing code.
 */
#ifndef LIBKC3_DO_BLOCK_H
#define LIBKC3_DO_BLOCK_H

#include "hash.h"
#include "types.h"

/* Stack allocation compatible functions */
s_do_block * do_block_init (s_do_block *do_block, uw count);
s_do_block * do_block_init_1 (s_do_block *do_block, const char *p);
s_do_block * do_block_init_cast (s_do_block *do_block, p_sym *type,
                                 s_tag *tag);
s_do_block * do_block_init_copy (s_do_block *do_block, s_do_block *src);
s_do_block * do_block_init_from_list (s_do_block *do_block,
                                s_list **list);
void      do_block_clean (s_do_block *do_block);

/* Constructors, call do_block_delete after use */
s_do_block * do_block_new (uw count);
s_do_block * do_block_new_1 (const char *p);

/* Destructor */
void do_block_delete (s_do_block *do_block);

/* Modifiers */
s_do_block * do_block_1 (s_do_block *do_block, const char *p);

/* Observers */
s_list * do_block_to_list (const s_do_block *do_block, s_list **list);

#endif /* LIBKC3_DO_BLOCK_H */
