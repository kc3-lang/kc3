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
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "../libkc3/mutex.h"
#include "../libkc3/pointer.h"
#include "../libkc3/socket.h"
#include "../libkc3/socket_addr.h"
#include "../libkc3/socket_buf.h"
#include "test.h"

enum socket_buf_test_failure {
  SOCKET_BUF_TEST_OK,
  SOCKET_BUF_TEST_BUFFERS,
  SOCKET_BUF_TEST_MUTEX,
  SOCKET_BUF_TEST_ADAPTERS,
  SOCKET_BUF_TEST_ADDRESS,
  SOCKET_BUF_TEST_STRING
};

static struct {
  enum socket_buf_test_failure failure;
  int fd;
  uw closes;
  uw buffers;
  uw mutexes;
  uw addresses;
  uw strings;
  uw addrinfo_frees;
  uw attached_at_clean;
  struct sockaddr_in addr;
  struct addrinfo info;
} g_socket_buf_test;

static s_buf_rw * socket_buf_test_buffers (s_buf_rw *rw, uw size)
{
  if (g_socket_buf_test.failure == SOCKET_BUF_TEST_BUFFERS ||
      ! buf_rw_init_alloc(rw, size))
    return NULL;
  g_socket_buf_test.buffers++;
  return rw;
}

static void socket_buf_test_buffers_clean (s_buf_rw *rw)
{
  if ((rw->r && rw->r->user_ptr) || (rw->w && rw->w->user_ptr))
    g_socket_buf_test.attached_at_clean++;
  g_socket_buf_test.buffers--;
  buf_rw_clean(rw);
}

static s_mutex * socket_buf_test_mutex (void)
{
  s_mutex *mutex;
  if (g_socket_buf_test.failure == SOCKET_BUF_TEST_MUTEX ||
      ! (mutex = mutex_new()))
    return NULL;
  g_socket_buf_test.mutexes++;
  return mutex;
}

static void socket_buf_test_mutex_delete (s_mutex *mutex)
{
  g_socket_buf_test.mutexes--;
  mutex_delete(mutex);
}

static s_buf_rw * socket_buf_test_adapters (s_buf_rw *rw, s32 fd)
{
  if (g_socket_buf_test.failure == SOCKET_BUF_TEST_ADAPTERS)
    return NULL;
  return buf_rw_fd_open(rw, fd);
}

static struct sockaddr * socket_buf_test_address (const struct sockaddr *addr,
                                                  u32 size)
{
  struct sockaddr *copy;
  if (g_socket_buf_test.failure == SOCKET_BUF_TEST_ADDRESS ||
      ! (copy = socket_addr_new_copy(addr, size)))
    return NULL;
  g_socket_buf_test.addresses++;
  return copy;
}

static void socket_buf_test_address_delete (struct sockaddr *addr)
{
  g_socket_buf_test.addresses--;
  socket_addr_delete(addr);
}

static s_str * socket_buf_test_string (s_str *str,
                                       const struct sockaddr *addr, s32 size)
{
  if (g_socket_buf_test.failure == SOCKET_BUF_TEST_STRING ||
      ! socket_addr_to_str(str, addr, size))
    return NULL;
  g_socket_buf_test.strings++;
  return str;
}

static void socket_buf_test_string_clean (s_str *str)
{
  g_socket_buf_test.strings--;
  str_clean(str);
}

/* Transport calls return one real socketpair FD. This exercises owning
   callers without DNS, a listening port, or a concurrent descriptor race. */
static int socket_buf_test_getaddrinfo (const char *host, const char *service,
                                        const struct addrinfo *hints,
                                        struct addrinfo **dest)
{
  (void) host;
  (void) service;
  (void) hints;
  *dest = &g_socket_buf_test.info;
  return 0;
}

static void socket_buf_test_freeaddrinfo (struct addrinfo *info)
{
  (void) info;
  g_socket_buf_test.addrinfo_frees++;
}

static int socket_buf_test_socket (int family, int type, int protocol)
{
  (void) family;
  (void) type;
  (void) protocol;
  return g_socket_buf_test.fd;
}

static int socket_buf_test_connect (int fd, const struct sockaddr *addr,
                                    socklen_t size)
{
  (void) fd;
  (void) addr;
  (void) size;
  return 0;
}

static int socket_buf_test_accept (int fd, struct sockaddr *addr,
                                   socklen_t *size)
{
  (void) fd;
  if (*size < sizeof(g_socket_buf_test.addr)) {
    errno = EINVAL;
    return -1;
  }
  memcpy(addr, &g_socket_buf_test.addr, sizeof(g_socket_buf_test.addr));
  *size = sizeof(g_socket_buf_test.addr);
  return g_socket_buf_test.fd;
}

static int socket_buf_test_fd_close (int fd)
{
  if (++g_socket_buf_test.closes > 1) {
    errno = EBADF;
    return -1;
  }
  return close(fd);
}

#define buf_rw_init_alloc socket_buf_test_buffers
#define buf_rw_clean socket_buf_test_buffers_clean
#define mutex_new socket_buf_test_mutex
#define mutex_delete socket_buf_test_mutex_delete
#define buf_rw_fd_open socket_buf_test_adapters
#define socket_addr_new_copy socket_buf_test_address
#define socket_addr_delete socket_buf_test_address_delete
#define socket_addr_to_str socket_buf_test_string
#define str_clean socket_buf_test_string_clean
#define getaddrinfo socket_buf_test_getaddrinfo
#define freeaddrinfo socket_buf_test_freeaddrinfo
#define socket socket_buf_test_socket
#define connect socket_buf_test_connect
#define accept socket_buf_test_accept
#define close socket_buf_test_fd_close
#define socket_buf_clean socket_buf_test_clean
#define socket_buf_delete socket_buf_test_delete
#define socket_buf_can_close socket_buf_test_can_close
#define socket_buf_close socket_buf_test_close
#define socket_buf_set_tag socket_buf_test_set_tag
#define socket_buf_tag socket_buf_test_tag
#define socket_buf_init socket_buf_test_init
#define socket_buf_init_accept socket_buf_test_init_accept
#define socket_buf_init_connect socket_buf_test_init_connect
#include "../libkc3/socket_buf.c"
#undef buf_rw_init_alloc
#undef buf_rw_clean
#undef mutex_new
#undef mutex_delete
#undef buf_rw_fd_open
#undef socket_addr_new_copy
#undef socket_addr_delete
#undef socket_addr_to_str
#undef str_clean
#undef getaddrinfo
#undef freeaddrinfo
#undef socket
#undef connect
#undef accept
#undef close
#undef socket_buf_clean
#undef socket_buf_delete
#undef socket_buf_can_close
#undef socket_buf_close
#undef socket_buf_set_tag
#undef socket_buf_tag
#undef socket_buf_init
#undef socket_buf_init_accept
#undef socket_buf_init_connect

static char socket_buf_test_construct (uw method,
                                       enum socket_buf_test_failure failure)
{
  int fd[2];
  const s_str host = STR("127.0.0.1");
  t_socket listening;
  s_socket_buf *result;
  s_socket_buf saved;
  s_socket_buf sb;
  const s_str service = STR("1");
  memset(&g_socket_buf_test, 0, sizeof(g_socket_buf_test));
  TEST_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fd), 0);
  g_socket_buf_test.fd = fd[0];
  g_socket_buf_test.failure = failure;
  g_socket_buf_test.addr.sin_family = AF_INET;
  g_socket_buf_test.addr.sin_port = htons(1);
  g_socket_buf_test.addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  g_socket_buf_test.info.ai_family = AF_INET;
  g_socket_buf_test.info.ai_addr =
    (struct sockaddr *) &g_socket_buf_test.addr;
  g_socket_buf_test.info.ai_addrlen = sizeof(g_socket_buf_test.addr);
  memset(&sb, 0xA5, sizeof(sb));
  memcpy(&saved, &sb, sizeof(saved));
  listening = fd[1];
  if (! method)
    result = socket_buf_test_init(&sb, fd[0],
                (struct sockaddr *) &g_socket_buf_test.addr,
                sizeof(g_socket_buf_test.addr));
  else if (method == 1)
    result = socket_buf_test_init_accept(&sb, &listening);
  else
    result = socket_buf_test_init_connect(&sb, &host, &service);
  if (failure) {
    TEST_EQ(result, NULL);
    TEST_EQ(g_socket_buf_test.closes, method ? 1 : 0);
    TEST_MEMCMP((const char *) &sb, (const char *) &saved, sizeof(sb));
  }
  else {
    TEST_EQ(result, &sb);
    TEST_EQ(sb.sockfd, fd[0]);
    TEST_EQ(g_socket_buf_test.closes, 0);
    TEST_ASSERT(sb.buf_rw.r && sb.buf_rw.w && sb.closed_mutex);
    TEST_MEMCMP((const char *) sb.addr,
                 (const char *) &g_socket_buf_test.addr,
                 sizeof(g_socket_buf_test.addr));
    TEST_ASSERT(sb.addr_str.size);
    if (method)
      socket_buf_test_close(&sb);
    socket_buf_test_clean(&sb);
    TEST_EQ(g_socket_buf_test.closes, method ? 1 : 0);
  }
  TEST_EQ(g_socket_buf_test.attached_at_clean, 0);
  TEST_EQ(g_socket_buf_test.buffers, 0);
  TEST_EQ(g_socket_buf_test.mutexes, 0);
  TEST_EQ(g_socket_buf_test.addresses, 0);
  TEST_EQ(g_socket_buf_test.strings, 0);
  TEST_EQ(g_socket_buf_test.addrinfo_frees, method == 2 ? 1 : 0);
  if (method) {
    errno = 0;
    TEST_EQ(fcntl(fd[0], F_GETFD), -1);
    TEST_EQ(errno, EBADF);
  }
  else {
    TEST_ASSERT(fcntl(fd[0], F_GETFD) >= 0);
    TEST_EQ(close(fd[0]), 0);
  }
  TEST_EQ(close(fd[1]), 0);
  return 0;
}

void socket_buf_test (void);
TEST_CASE_PROTOTYPE(socket_buf_ownership);

void socket_buf_test (void)
{
  TEST_CASE_RUN(socket_buf_ownership);
}

TEST_CASE(socket_buf_ownership)
{
  const enum socket_buf_test_failure failures[] = {
    SOCKET_BUF_TEST_ADAPTERS, SOCKET_BUF_TEST_BUFFERS, SOCKET_BUF_TEST_MUTEX,
    SOCKET_BUF_TEST_ADDRESS, SOCKET_BUF_TEST_STRING, SOCKET_BUF_TEST_OK
  };
  uw i;
  uw method;
  for (i = 0; i < sizeof(failures) / sizeof(*failures); i++)
    for (method = 0; method < 3; method++)
      TEST_EQ(socket_buf_test_construct(method, failures[i]), 0);
}
TEST_CASE_END(socket_buf_ownership)
