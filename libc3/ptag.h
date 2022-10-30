/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef PTAG_H
#define PTAG_H

#include "hash.h"
#include "types.h"

s8               ptag_compare (const p_tag a, const p_tag b);
t_hash_context * ptag_hash_update (t_hash_context *context,
                                   const p_tag ptag);

#endif /* PTAG_H */
