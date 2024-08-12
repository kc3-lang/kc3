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
#ifndef SOCKET_ADDR_H
#define SOCKET_ADDR_H

#include "types.h"

/* Heap-allocation functions. */
s_str * socket_addr_to_str(s_str *str, const struct sockaddr *addr,
                           s32 addr_len);
void              socket_addr_delete (struct sockaddr *sa);
struct sockaddr * socket_addr_new (u32 len);
struct sockaddr * socket_addr_new_copy (const struct sockaddr *addr,
                                        u32 len);

#endif /* SOCKET_H */
