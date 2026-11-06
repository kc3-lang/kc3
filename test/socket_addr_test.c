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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "../libkc3/socket_addr.h"
#include "../libkc3/str.h"
#include "test.h"

static char socket_addr_to_str_ipv6_check (void)
{
  const char *addresses[] = {
    "::1", "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"
  };
  struct sockaddr_in6 addr;
  s_str dest;
  s_str *result;
  uw i;
  char failed = 0;
  for (i = 0; i < sizeof(addresses) / sizeof(*addresses); i++) {
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    if (inet_pton(AF_INET6, addresses[i], &addr.sin6_addr) != 1)
      return 1;
    dest = (s_str) STR("sentinel");
    result = socket_addr_to_str(&dest, (struct sockaddr *) &addr,
                                sizeof(addr));
    if (! result || dest.size != strlen(addresses[i]) ||
        memcmp(dest.ptr.p_pchar, addresses[i], dest.size))
      failed = 1;
    if (dest.free.p_pvoid)
      str_clean(&dest);
  }
  return failed;
}

TEST_CASE(socket_addr_to_str_ipv6)
{
  TEST_EQ(socket_addr_to_str_ipv6_check(), 0);
}
TEST_CASE_END(socket_addr_to_str_ipv6)

TEST_CASE(socket_addr_to_str_invalid_family)
{
  struct sockaddr addr;
  const s_str old = STR("sentinel");
  s_str dest = old;
  memset(&addr, 0, sizeof(addr));
  addr.sa_family = AF_UNSPEC;
  TEST_EQ(socket_addr_to_str(&dest, &addr, sizeof(addr)), NULL);
  TEST_EQ(dest.free.p_pvoid, old.free.p_pvoid);
  TEST_EQ(dest.ptr.p_pvoid, old.ptr.p_pvoid);
  TEST_EQ(dest.size, old.size);
}
TEST_CASE_END(socket_addr_to_str_invalid_family)

void socket_addr_test (void)
{
  TEST_CASE_RUN(socket_addr_to_str_ipv6);
  TEST_CASE_RUN(socket_addr_to_str_invalid_family);
}
