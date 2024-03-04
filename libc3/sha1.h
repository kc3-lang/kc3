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
#ifndef LIBC3_SHA1_H
#define LIBC3_SHA1_H

#include "config.h"

#if HAVE_SHA1_H
# include <sha1.h>
#else
# if HAVE_SHA_H
#  include <sha.h>
#  define SHA1_DIGEST_LENGTH SHA_DIGEST_LENGTH
#  define SHA1Final SHA1_Final
#  define SHA1Init SHA1_Init
#  define SHA1Update SHA1_Update
# endif
#endif

#endif /* LIBC3_SHA1_H */
