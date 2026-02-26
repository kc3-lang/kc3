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
#include <string.h>

#ifdef WIN32
# include <winsock2.h>
# include <ws2tcpip.h>
#else
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
#endif

#include <unistd.h>
#include "kc3.h"
#include "socket.h"
#include "socket_buf.h"

s_str * socket_addr_to_str(s_str *str, const struct sockaddr *addr,
                           s32 addr_len)
{
  char ip[INET6_ADDRSTRLEN];
  assert(addr);
  assert(addr_len);
  if (!inet_ntop(addr->sa_family, &addr->sa_data[2], ip, addr_len)) {
    err_puts("socket_addr_to_str: inet_ntop");
    return str;
  }
  return str_init_1_alloc(str, ip);
}

void socket_addr_delete (struct sockaddr *sa)
{
  assert(sa);
  alloc_free(sa);
}

struct sockaddr * socket_addr_new (u32 len)
{
    struct sockaddr *sa;
    assert(len);
    sa = alloc(len);
    if (! sa)
      return NULL;
    return sa;
}

struct sockaddr * socket_addr_new_copy (const struct sockaddr *addr,
                                        u32 len)
{
    struct sockaddr *sa;
    assert(len);
    sa = alloc(len);
    if (! sa)
      return NULL;
    memcpy(sa, addr, len);
    return sa;
}

s32 socket_addr_port(const struct sockaddr *addr)
{
  if (addr->sa_family == AF_INET) {
    const struct sockaddr_in *ipv4 = (const struct sockaddr_in *) addr;
    return ntohs(ipv4->sin_port);
  }
  else if (addr->sa_family == AF_INET6) {
    const struct sockaddr_in6 *ipv6 = (const struct sockaddr_in6 *) addr;
    return ntohs(ipv6->sin6_port);
  }
  else {
    return -1;
  }
}

int socket_addr_port_set (struct sockaddr *addr, s32 port)
{
  if (addr->sa_family == AF_INET) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *) addr;
    ipv4->sin_port = htons(port);
    return 0;
  }
  if (addr->sa_family == AF_INET6) {
    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) addr;
    ipv6->sin6_port = htons(port);
    return 0;
  }
  return -1;
}
