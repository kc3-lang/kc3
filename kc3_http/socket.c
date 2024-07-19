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
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <libkc3/kc3.h>
#include "socket.h"

int * socket_init_listen(int *s, const s_str *host, u16 port)
{
    struct sockaddr *addr;
    socklen_t addr_len;
    sw e;
    struct hostent *hostent;
    assert(s);
    assert(host);
    hostent = gethostbyname2(host->ptr.pchar, AF_INET);
    if (! hostent) {
        e = errno;
        err_write_1("socket_init_listen: gethostbyname2: ");
        err_puts(strerror(e));
        assert(!"socket_init_listen: gethostbyname2");
        return NULL;
    }
    addr = (struct sockaddr *) hostent->h_addr_list[0];
    addr_len = hostent->h_length;
    socket = socket(addr->sa_family, SOCK_STREAM, 0);
    if (socket < 0) {
        e = errno;
        err_write_1("socket_init_listen: socket: ");
        err_puts(strerror(e));
        assert(! "socket_init_listen: socket");
        return NULL;
    }
    addr->port = htons(port);
    if (bind(socket, addr, addr_len) < 0) {
        e = errno;
        err_write_1("socket_init_listen: bind: ");
        err_puts(strerror(e));
        assert(! "socket_init_listen: bind");
        return NULL;
    }
    return s;
}