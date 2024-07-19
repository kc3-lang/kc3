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
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <libkc3/kc3.h>
#include "socket.h"

void socket_clean (p_socket s)
{
  assert(s);
  close(*s);
  *s = 0;
}

p_socket socket_init_listen (p_socket s, const s_str *host, u16 port)
{
  struct sockaddr *addr;
  struct sockaddr_in *addr_inet;
  struct sockaddr_in6 *addr_inet6;
  socklen_t addr_len;
  sw e;
  struct hostent *hostent;
  t_socket tmp;
  assert(s);
  assert(host);
  hostent = gethostbyname2(host->ptr.pchar, AF_INET);
  if (! hostent)
    hostent = gethostbyname2(host->ptr.pchar, AF_INET6);
  if (! hostent) {
    e = errno;
    err_write_1("socket_init_listen: gethostbyname2: ");
    err_puts(strerror(e));
    assert(!"socket_init_listen: gethostbyname2");
    return NULL;
  }
  addr_len = hostent->h_length;
  addr = alloc(addr_len);
  if (! addr)
    return NULL;
  memcpy(addr, hostent->h_addr_list[0], addr_len);
  switch (addr->sa_family) {
  case AF_INET:
    addr_inet = (struct sockaddr_in *) addr;
    addr_inet->sin_port = htons(port);
    break;
  case AF_INET6:
    addr_inet6 = (struct sockaddr_in6 *) addr;
    addr_inet6->sin6_port = htons(port);
    break;
  default:
    err_puts("socket_init_listen: unknown address family");
    assert(! "socket_init_listen: unknown address family");
    free(addr);
    return NULL;
  }
  tmp = socket(addr->sa_family, SOCK_STREAM, 0);
  if (tmp < 0) {
    e = errno;
    err_write_1("socket_init_listen: socket: ");
    err_puts(strerror(e));
    assert(! "socket_init_listen: socket");
    free(addr);
    return NULL;
  }
  if (bind(tmp, addr, addr_len) < 0) {
    e = errno;
    err_write_1("socket_init_listen: bind: ");
    err_puts(strerror(e));
    assert(! "socket_init_listen: bind");
    socket_clean(&tmp);
    free(addr);
    return NULL;
  }
  free(addr);
  if (listen(tmp, SOMAXCONN) < 0) {
    e = errno;
    err_write_1("socket_init_listen: listen: ");
    err_puts(strerror(e));
    assert(! "socket_init_listen: listen");
    socket_clean(&tmp);
    free(addr);
    return NULL;
  }
  *s = tmp;
  return s;
}
