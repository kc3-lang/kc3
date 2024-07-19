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
#include <libkc3/kc3.h>
#include <sys/socket.h>
#include "socket.h"

int * socket_init_listen(int *s, const struct sockaddr *addr, socklen_t addr_len)
{
    assert(s);
    assert(addr);
    assert(addr_len);
    socket = socket(addr->sa_family, SOCK_STREAM, 0);
    if (socket < 0) {
        err_puts("socket_init_listen: socket: %s", strerror(errno));
        assert(! "socket_init_listen: socket");
        return NULL;
    }
    if (bind(socket, addr, addr_len) < 0) {
        err_puts("socket_init_listen: bind: %s", strerror(errno));
        assert(! "socket_init_listen: bind");
        return NULL;
    }
}

buf * socket_to_buf(int *s, s_buf *dest);