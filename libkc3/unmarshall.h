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

#ifndef LIBKC3_UNMARSHALL_H
#define LIBKC3_UNMARSHALL_H

#include "types.h"

#define PROTO_UNMARSHALL_READ(type, m, t)                              \
sw unmarshall_read_ ## type (s_marshall *m, s_type *t)

#define PROTO_UNMARSHALL(type, m, t)                                   \
sw unmarshall_ ## type (s_type *t, s_marshall *m)

PROTO_UNMARSHALL_READ(u8, m, t);
PROTO_UNMARSHALL_READ(u16, m, t);
PROTO_UNMARSHALL_READ(u32, m, t);
PROTO_UNMARSHALL_READ(u64, m, t);
PROTO_UNMARSHALL_READ(uw, m, t);

PROTO_UNMARSHALL_READ(sw, m, t);
PROTO_UNMARSHALL_READ(s8, m, t);
PROTO_UNMARSHALL_READ(s16, m, t);
PROTO_UNMARSHALL_READ(s32, m, t);
PROTO_UNMARSHALL_READ(s64, m, t);


PROTO_UNMARSHALL(u8, m, t);
PROTO_UNMARSHALL(u16, m, t);
PROTO_UNMARSHALL(u32, m, t);
PROTO_UNMARSHALL(u64, m, t);
PROTO_UNMARSHALL(uw, m, t);

PROTO_UNMARSHALL(sw, m, t);
PROTO_UNMARSHALL(s8, m, t);
PROTO_UNMARSHALL(s16, m, t);
PROTO_UNMARSHALL(s32, m, t);
PROTO_UNMARSHALL_READ(s64, m, t);

#endif /* LIBKC3_UNMARSHALL_H */
