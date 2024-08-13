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
#include <arpa/inet.h>
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
  free(sa);
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
