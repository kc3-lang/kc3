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
 * @file block.h
 * @brief A return or return_from block.
 *
 * A block has a symbolic name which can be NULL, a tag value to be
 * returned, and a next pointer to restore the environment.
 */
#ifndef LIBKC3_BLOCK_H
#define LIBKC3_BLOCK_H

#include "types.h"

/* Stack-allocation compatible functions, call block_clean after use. */
s_block * block_init (s_block *block, const s_sym *name);
void      block_clean (s_block *block);

/* Observers. */
s_block ** block_find (const s_sym *name);

/* Operators. */
void block_jump (s_block *block);
void block_return (s_block *block, s_tag *value);
void block_return_from (const s_sym *name, s_tag *value);

#endif /* LIBKC3_BLOCK_H */
