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
#include <errno.h>
#include <string.h>

#ifdef WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
# undef errno
# define errno WSAGetLastError()
#else
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
#endif

#include <unistd.h>
#include "../libkc3/kc3.h"
#include "./socket.h"
#include "socket_addr.h"
#include "socket_buf.h"

void socket_buf_clean (s_socket_buf *sb)
{
  assert(sb);
  buf_rw_fd_close(&sb->buf_rw);
  if (sb->buf_rw.r || sb->buf_rw.w) {
    buf_rw_clean(&sb->buf_rw);
    sb->buf_rw.r = NULL;
    sb->buf_rw.w = NULL;
  }
  if (sb->addr_str.free.p) {
    str_clean(&sb->addr_str);
    sb->addr_str.free.p = NULL;
  }
  if (sb->addr) {
    socket_addr_delete(sb->addr);
    sb->addr = NULL;
  }
}

void socket_buf_close (s_socket_buf *sb)
{
  assert(sb);
  buf_rw_fd_close(&sb->buf_rw);
  if (sb->sockfd >= 0) {
    shutdown(sb->sockfd, SHUT_RDWR);
    close(sb->sockfd);
    sb->sockfd = -1;
  }
  if (sb->buf_rw.r || sb->buf_rw.w) {
    buf_rw_clean(&sb->buf_rw);
    sb->buf_rw.r = NULL;
    sb->buf_rw.w = NULL;
  }
  if (sb->addr_str.free.p) {
    str_clean(&sb->addr_str);
    sb->addr_str.free.p = NULL;
  }
  if (sb->addr) {
    socket_addr_delete(sb->addr);
    sb->addr = NULL;
  }
}

s_socket_buf * socket_buf_init (s_socket_buf *sb, s64 sockfd,
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
  tmp.buf_rw.r->user_ptr = sb;
  tmp.buf_rw.w->user_ptr = sb;
  if (! buf_rw_fd_open(&tmp.buf_rw, sockfd)) {
    err_puts("socket_buf_init: buf_rw_fd_open");
    assert(! "socket_buf_init: buf_rw_fd_open");
    close(sockfd);
    buf_rw_clean(&tmp.buf_rw);
    return NULL;
  }
  tmp.sockfd = sockfd;
  tmp.addr = socket_addr_new_copy(addr, addr_len);
  if (! tmp.addr) {
    err_puts("socket_buf_init: socket_addr_new_copy");
    assert(! "socket_buf_init: socket_addr_new_copy");
    return NULL;
  }
  socket_addr_to_str(&tmp.addr_str, addr, addr_len);
  tmp.addr_len = addr_len;
  *sb = tmp;
  return sb;
}

s_socket_buf * socket_buf_init_accept (s_socket_buf *sb, p_socket listening)
{
  struct sockaddr        *addr;
  struct sockaddr_storage addr_storage = {0};
  socklen_t               addr_len;
  s64 sockfd;
  s_socket_buf tmp = {0};
  assert(sb);
  assert(listening);
  addr = (struct sockaddr *) &addr_storage;
  addr_len = sizeof(addr_storage);
  sockfd = accept(*listening, addr, &addr_len);
  if (sockfd < 0)
    return NULL;
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
  struct addrinfo *res_last = NULL;
  s32 e;
  const char *error_reason = "error: ";
  char ipstr[128] = {0};
  s32 port = -1;
  s64 r = 0;
  s64 sockfd;
  s_socket_buf tmp;
  assert(sb);
  assert(host);
  assert(service);
  if (! libsocket_init())
    return NULL;
  e = getaddrinfo(host->ptr.pchar, service->ptr.pchar, &hints, &res0);
  if (e) {
    err_write_1("socket_buf_init_connect(");
    err_write_1(host->ptr.pchar);
    err_write_1(", ");
    err_write_1(service->ptr.pchar);
    err_write_1("): getaddrinfo: ");
    err_inspect_s32_decimal(e);
    err_write_1(" ");
    err_puts((const char *) gai_strerror(e));
    assert(! "socket_buf_init_connect: getaddrinfo");
    return NULL;
  }
  e = 0;
  sockfd = -1;
  res = res0;
  while (res) {
    res_last  = res;
    if (! socket_addr_port(res->ai_addr)) {
      e = errno;
      error_reason = "getaddrinfo port 0: ";
      goto next;
    }
    sockfd = socket(res->ai_family, SOCK_STREAM, res->ai_protocol);
    if (sockfd < 0) {
      e = errno;
      error_reason = "socket: ";
      goto next;
    }
    if ((r = connect(sockfd, res->ai_addr, res->ai_addrlen)) < 0) {
      e = errno;
      error_reason = "connect: ";
      goto next;
    }
    break;
  next:
    if (sockfd >= 0) {
      close(sockfd);
      sockfd = -1;
    }
    res = res->ai_next;
  }
  if (sockfd < 0) {
    err_write_1("socket_buf_init_connect(");
    err_write_1(host->ptr.pchar);
    err_write_1(", ");
    err_write_1(service->ptr.pchar);
    err_write_1("): ");
    if (res_last) {
      if (res_last->ai_family == AF_INET) {
        struct sockaddr_in *ipv4 = (struct sockaddr_in *) res_last->ai_addr;
        inet_ntop(AF_INET, &(ipv4->sin_addr), ipstr, sizeof(ipstr));
        err_write_1(ipstr);
        err_write_1(" ");
        port = socket_addr_port(res_last->ai_addr);
        err_inspect_s32_decimal(port);
        err_write_1(": ");
      }
      else if (res_last->ai_family == AF_INET6) {
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) res_last->ai_addr;
        inet_ntop(AF_INET6, &(ipv6->sin6_addr), ipstr, sizeof(ipstr));
        err_write_1(ipstr);
        err_write_1(" ");
        port = socket_addr_port(res_last->ai_addr);
        err_inspect_s32_decimal(port);
        err_write_1(": ");
      }
    }
    err_write_1(error_reason);
    err_inspect_s64_decimal(r);
    err_write_1(" ");
    err_inspect_s32_decimal(e);
    err_write_1(" ");
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
