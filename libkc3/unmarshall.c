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

#include "types.h"
#include "assert.h"
#include "unmarshall.h"
#include "marshall.h"
#include "buf.h"

#define UNMARSHALL_READ(type, m, t)                                    \
sw unmarshall_read_ ## type (s_marshall *m, s_type *t)                 \
{                                                                      \
  if (buf_read_ ## type (m->buf, t) < 0)                               \
    return -1;                                                         \
  return 0;                                                            \
}                                                                      \

#define UNMARSHALL_TYPE(type, m, t)                                    \
sw unmarshall_ ## type (s_type *t, s_marshall *m)                      \
{                                                                      \
  assert(t);                                                           \
  assert(m);                                                           \
  return unmarshall_read_ ## type (t, m);                              \
}

