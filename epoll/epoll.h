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
#ifndef KC3_EPOLL_H
#define KC3_EPOLL_H

#include "../libkc3/types.h"

s64     kc3_epoll (void);
s64     kc3_epoll_add (s64 epfd, s64 fd, s_tag *timeout, s_tag **udata);
s64     kc3_epoll_delete (s64 epfd, s64 fd, s_tag *filter);
s_tag * kc3_epoll_poll (s64 epfd, s_tag *timeout, s_tag *dest);

#endif /* KC3_EPOLL_H */
