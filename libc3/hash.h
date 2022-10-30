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
#ifndef HASH_H
#define HASH_H

#include <sys/types.h>
#include <sha1.h>
#include "types.h"

#define HASH_SIZE SHA1_DIGEST_LENGTH

typedef SHA1_CTX t_hash_context;

t_hash_context * hash_init (t_hash_context *context);
t_hash_context * hash_update (t_hash_context *context,
                              const void *data, uw size);
uw               hash_result (t_hash_context *context);

#endif /* HASH_H */
