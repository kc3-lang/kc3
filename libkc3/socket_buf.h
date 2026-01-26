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
#ifndef LIBKC3_SOCKET_BUF_H
#define LIBKC3_SOCKET_BUF_H

#include "types.h"

/* Stack-allocation compatible functions, call socket_buf_clean after
   use. */
void           socket_buf_clean (s_socket_buf *sb);
void           socket_buf_close (s_socket_buf *sb);
s_socket_buf * socket_buf_init (s_socket_buf *sb, t_socket sockfd,
                                struct sockaddr *addr, u32 addr_len);
s_socket_buf * socket_buf_init_accept (s_socket_buf *sb,
                                       p_socket listening);
s_socket_buf * socket_buf_init_connect (s_socket_buf *sb,
                                        const s_str *host,
                                        const s_str *service);

#endif /* LIBKC3_SOCKET_BUF_H */
