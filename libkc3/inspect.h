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
 * @file inspect.h
 * @brief Inspect any Tag type into a Str
 *
 * Functions to inspect a Tag into a Str.
 */
#ifndef LIBKC3_INSPECT_H
#define LIBKC3_INSPECT_H

#include "types.h"

s_str * inspect_array (const s_array *array, s_str *dest);
s_str * inspect_block (const s_block *block, s_str *dest);
s_str * inspect_bool (bool *b, s_str *dest);
s_str * inspect_sym (const s_sym *sym, s_str *dest);
s_str * inspect_tag (const s_tag *tag, s_str *dest);

#endif /* LIBKC3_INSPECT_H */
