/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef S64_H
#define S64_H

#include "hash.h"
#include "types.h"

s8               s64_compare (s64 a, s64 b);
s8               s64_compare_u64 (s64 a, u64 b);
t_hash_context * s64_hash_update (t_hash_context *context, s64 i);

#endif /* S64_H */
