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
#ifndef SOCKET_H
#define SOCKET_H

#include "types.h"

/* Stack-allocation compatible functions. */
p_socket socket_init_accept (p_socket s, p_socket listening);
p_socket socket_init_listen (p_socket s, const s_str *host,
                             const s_str *service);

/* Operators. */
void socket_close (p_socket s);

#endif /* SOCKET_H */