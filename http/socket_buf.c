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
#include "socket_addr.h"
#include "socket_buf.h"

void socket_buf_clean (s_socket_buf *sb)
{
  assert(sb);
  close(sb->sockfd);
  socket_addr_delete(sb->addr);
  buf_rw_clean(&sb->buf_rw);
}

s_socket_buf * socket_buf_init (s_socket_buf *sb, t_socket sockfd,
                                struct sockaddr *addr,
                                u32 addr_len)
{
  s_socket_buf tmp = {0};
  assert(sb);
  assert(sockfd >= 0);
  assert(addr);
  assert(addr_len);
  if (! buf_rw_init_alloc(&tmp.buf_rw, BUF_SIZE)) {
    err_puts("socket_buf_init: buf_rw_init_alloc");
    assert(! "socket_buf_init: buf_rw_init_alloc");
    return NULL;
  }
  tmp.buf_rw.r.user_ptr = sb;
  tmp.buf_rw.w.user_ptr = sb;
  tmp.sockfd = sockfd;
  tmp.addr = socket_addr_new_copy(addr, addr_len);
  if (! tmp.addr) {
    err_puts("socket_buf_init: socket_addr_new_copy");
    assert(! "socket_buf_init: socket_addr_new_copy");
    return NULL;
  }
  tmp.addr_len = addr_len;
  *sb = tmp;
  return sb;
}

s_socket_buf * socket_buf_init_accept (s_socket_buf *sb, p_socket listening)
{
  struct sockaddr        *addr;
  struct sockaddr_storage addr_storage = {0};
  socklen_t               addr_len;
  sw e;
  t_socket sockfd;
  s_socket_buf tmp = {0};
  assert(sb);
  assert(listening);
  addr = (struct sockaddr *) &addr_storage;
  addr_len = sizeof(addr_storage);
  sockfd = accept(*listening, addr, &addr_len);
  if (sockfd < 0) {
    e = errno;
    err_write_1("socket_buf_init_accept: ");
    err_inspect_s32(listening);
    err_write_1(": accept: ");
    err_puts(strerror(e));
    assert(! "socket_buf_init_accept: accept");
    return NULL;
  }
  if (! socket_buf_init(&tmp, sockfd, addr, addr_len)) {
    err_puts("socket_buf_init_accept: socket_buf_init");
    assert(! "socket_buf_init_accept: socket_buf_init");
    close(sockfd);
    return NULL;
  }
  *sb = tmp;
  return sb;
}

s_socket_buf * socket_buf_init_connect (s_socket_buf *sb,
                                        const s_str *host,
                                        const s_str *service)
{
  struct addrinfo hints = {0};
  struct addrinfo *res;
  struct addrinfo *res0;
  s32 e;
  const char *error_reason = "error";
  t_socket sockfd;
  s_socket_buf tmp;
  assert(sb);
  assert(host);
  assert(service);
  hints.ai_family = AF_INET;
  e = getaddrinfo(host->ptr.pchar, service->ptr.pchar, &hints, &res0);
  if (e) {
    err_write_1("socket_buf_init_connect(");
    err_write_1(host->ptr.pchar);
    err_write_1(", ");
    err_write_1(service->ptr.pchar);
    err_write_1("): getaddrinfo: ");
    err_puts(gai_strerror(e));
    assert(! "socket_buf_init_connect: getaddrinfo");
    return NULL;
  }
  e = 0;
  sockfd = -1;
  res = res0;
  while (res) {
    sockfd = socket(res->ai_family, SOCK_STREAM, res->ai_protocol);
    if (sockfd < 0) {
      e = errno;
      error_reason = "socket_buf_init_connect: socket: ";
      goto next;
    }
    if (connect(sockfd, res->ai_addr, res->ai_addrlen) < 0) {
      e = errno;
      error_reason = "socket_buf_init_connect: connect: ";
      goto next;
    }
    break;
  next:
    res = res->ai_next;
  }
  if (sockfd < 0) {
    err_write_1(error_reason);
    err_puts(strerror(e));
    assert(! "socket_buf_init_connect");
    return NULL;
  }
  if (! socket_buf_init(&tmp, sockfd, res->ai_addr, res->ai_addrlen)) {
    err_puts("socket_buf_init_connect: socket_buf_init");
    assert(! "socket_buf_init_connect: socket_buf_init");
    return NULL;
  }
  freeaddrinfo(res0);
  *sb = tmp;
  return sb;
}