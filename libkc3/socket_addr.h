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
#ifndef LIBKC3_SOCKET_ADDR_H
#define LIBKC3_SOCKET_ADDR_H

#include "types.h"

/* Heap-allocation functions. */
s_str * socket_addr_to_str(s_str *str, const struct sockaddr *addr,
                           s32 addr_len);
void              socket_addr_delete (struct sockaddr *sa);
struct sockaddr * socket_addr_new (u32 len);
struct sockaddr * socket_addr_new_copy (const struct sockaddr *addr,
                                        u32 len);

/* Observers. */
s32 socket_addr_port(const struct sockaddr *addr);

/* Operators. */
s32 socket_addr_port_set(const struct sockaddr *addr, s32 port);

#endif /* LIBKC3_SOCKET_ADDR_H */
