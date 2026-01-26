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
#ifndef LIBKC3_HOTP_H
#define LIBKC3_HOTP_H

#include "types.h"

void    hotp_value (const s_str *k, u64 c, char dest[7]);
s_str * hotp_value_str (const s_str *k, const s_tag *c, s_str *dest);

#endif /* LIBKC3_HOTP_H */
