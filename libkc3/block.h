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
 * @file block.h
 * @brief A block of code (a tuple of tags).
 *
 * A block is a tuple of tags representing code.
 */
#ifndef LIBKC3_BLOCK_H
#define LIBKC3_BLOCK_H

#include "types.h"

/* Stack-allocation compatible functions, call block_clean after use. */
s_block * block_init (s_block *block);
void      block_clean (s_block *block);

#endif /* LIBKC3_BLOCK_H */
