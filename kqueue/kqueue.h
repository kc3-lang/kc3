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
#ifndef KC3_KQUEUE_H
#define KC3_KQUEUE_H

#include "../libkc3/types.h"

s64     kc3_kqueue (void);
s64     kc3_kqueue_add (s64 kqfd, s64 fd, s_tag *udata);
s_tag * kc3_kqueue_poll (s64 kqfd, s_tag *timeout, s_tag *dest);

#endif /* KC3_KQUEUE_H */
