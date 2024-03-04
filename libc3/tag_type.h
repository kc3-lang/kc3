/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
 * @file tag.h
 * @brief Run-time type annotations enum.
 *
 * Enum for run-time tagged data types.
 */
#ifndef LIBC3_TAG_TYPE_H
#define LIBC3_TAG_TYPE_H

#include "types.h"

const char *    tag_type_to_string (e_tag_type type);

#endif /* LIBC3_TAG_TYPE */
