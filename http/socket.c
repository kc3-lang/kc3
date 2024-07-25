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
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <libkc3/kc3.h>
#include "socket.h"
#include "socket_buf.h"

void socket_close (p_socket s)
{
  assert(s);
  close(*s);
  *s = -1;
}

p_socket socket_init_accept (p_socket s, p_socket listening)
{
  struct sockaddr   *addr;
  struct sockaddr_in addr_in = {0};
  socklen_t          addr_len;
  sw e;
  t_socket tmp;
  assert(s);
  assert(listening);
  addr = (struct sockaddr *) &addr_in;
  addr_len = sizeof(addr_in);
  tmp = accept(*listening, addr, &addr_len);
  if (tmp < 0) {
    e = errno;
    err_write_1("socket_init_accept: ");
    err_inspect_s32(listening);
    err_write_1(": accept: ");
    err_puts(strerror(e));
    assert(! "socket_init_accept: accept");
    return NULL;
  }
  *s = tmp;
  return s;
}

p_socket socket_init_listen (p_socket s, const s_str *host,
                             const s_str *service)
{
  struct addrinfo hints = {0};
  struct addrinfo *res;
  struct addrinfo *res0;
  s32 e;
  const char *error_reason = "error";
  t_socket sockfd;
  assert(s);
  assert(host);
  hints.ai_family = AF_INET;
  e = getaddrinfo(host->ptr.pchar, service->ptr.pchar, &hints, &res0);
  if (e) {
    err_write_1("socket_init_listen(");
    err_write_1(host->ptr.pchar);
    err_write_1(", ");
    err_write_1(service->ptr.pchar);
    err_write_1("): getaddrinfo: ");
    err_puts(gai_strerror(e));
    assert(! "socket_init_listen: getaddrinfo");
    return NULL;
  }
  e = 0;
  sockfd = -1;
  res = res0;
  while (res) {
    sockfd = socket(res->ai_family, SOCK_STREAM, res->ai_protocol);
    if (sockfd < 0) {
      e = errno;
      error_reason = "socket_init_listen: socket: ";
      goto next;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1},
                   sizeof(int)) < 0) {
      error_reason = "setsockopt(SO_REUSEADDR)";
      goto next;
    }
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
      e = errno;
      error_reason = "socket_init_listen: bind: ";
      goto next;
    }
    if (listen(sockfd, SOMAXCONN) < 0) {
      e = errno;
      error_reason = "socket_init_listen: listen: ";
      goto next;
    }
  next:
    res = res->ai_next;
  }
  if (sockfd < 0) {
    err_write_1(error_reason);
    err_puts(strerror(e));
    assert(! "socket_init_listen");
    return NULL;
  }
  *s = sockfd;
  return s;
}
