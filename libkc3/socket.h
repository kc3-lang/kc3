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
#ifndef LIBKC3_SOCKET_H
#define LIBKC3_SOCKET_H

#include "types.h"

/* Call libsocket_init before any socket operation. Call
 * libsocket_clean after all socket operations were completed. */
void libsocket_clean (void);
bool libsocket_init (void);

/* Stack-allocation compatible functions. */
p_socket socket_init_accept (p_socket s, p_socket listening);
p_socket socket_init_connect (p_socket s, const s_str *host,
                              const s_str *service);
p_socket socket_init_listen (p_socket s, const s_str *host,
                             const s_str *service);

/* Operators. */
void socket_close (p_socket s);

#endif /* LIBKC3_SOCKET_H */
