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
#ifndef BUF_PARSE_TEST_H
#define BUF_PARSE_TEST_H

#include <string.h>
#include "../libkc3/buf.h"
#include "../libkc3/buf_parse.h"
#include "test.h"

#define BUF_PARSE_TEST_S(bits, test, expected)                         \
  do {                                                                 \
    s_buf buf;                                                         \
    s ## bits s;                                                       \
    test_context("buf_parse_s" # bits "(" # test ")");                 \
    buf_init_1(&buf, (test));                                          \
    TEST_EQ(buf_parse_s ## bits(&buf, &s), strlen(test));              \
    TEST_EQ(s, (expected));                                            \
    buf_clean(&buf);                                                   \
    test_context(NULL);                                                \
  } while (0)

#define BUF_PARSE_TEST_U(bits, test, expected)                         \
  do {                                                                 \
    s_buf buf;                                                         \
    u ## bits u = 0;                                                   \
    test_context("buf_parse_u" # bits "(" # test ")");                 \
    buf_init_1(&buf, (test));                                          \
    TEST_EQ(buf_parse_u ## bits(&buf, &u), strlen(test));              \
    TEST_EQ(u, (expected));                                            \
    buf_clean(&buf);                                                   \
    test_context(NULL);                                                \
  } while (0)

#endif /* BUF_PARSE_TEST_H */
