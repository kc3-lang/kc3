/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
 * @file block.h
 * @brief A block of code (a tuple of tags).
 *
 * A block is a tuple of tags representing code.
 */
#ifndef LIBKC3_BLOCK_H
#define LIBKC3_BLOCK_H

#include "hash.h"
#include "types.h"

/* Stack allocation compatible functions */
s_block * block_init (s_block *block, uw count);
s_block * block_init_1 (s_block *block, const char *p);
s_block * block_init_cast (s_block *block, const s_tag *tag);
s_block * block_init_copy (s_block *block, const s_block *src);
s_block * block_init_from_list (s_block *block,
                                const s_list * const *list);
void      block_clean (s_block *block);

/* Constructors, call block_delete after use */
s_block * block_new (uw count);
s_block * block_new_1 (const char *p);

/* Destructor */
void block_delete (s_block *block);

/* Modifiers */
s_block * block_1 (s_block *block, const char *p);

/* Observers */
s_list * block_to_list (const s_block *block, s_list **list);

#endif /* LIBKC3_BLOCK_H */
