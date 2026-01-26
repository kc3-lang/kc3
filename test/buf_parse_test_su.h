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
#ifndef BUF_PARSE_TEST_SU

#include "test.h"

#ifdef C3_TEST_BUF_PARSE_SU

#define BUF_PARSE_TEST_S_PROTOTYPE(bits)                               \
  TEST_CASE_PROTOTYPE(buf_parse_s ## bits ## _binary);                 \
  TEST_CASE_PROTOTYPE(buf_parse_s ## bits ## _binary_negative);        \
  TEST_CASE_PROTOTYPE(buf_parse_s ## bits ## _decimal);                \
  TEST_CASE_PROTOTYPE(buf_parse_s ## bits ## _decimal_negative);       \
  TEST_CASE_PROTOTYPE(buf_parse_s ## bits ## _hexadecimal);            \
  TEST_CASE_PROTOTYPE(buf_parse_s ## bits ## _hexadecimal_negative);   \
  TEST_CASE_PROTOTYPE(buf_parse_s ## bits ## _octal);                  \
  TEST_CASE_PROTOTYPE(buf_parse_s ## bits ## _octal_negative)

#define BUF_PARSE_TEST_U_PROTOTYPE(bits)                               \
  TEST_CASE_PROTOTYPE(buf_parse_u ## bits ## _binary);                 \
  TEST_CASE_PROTOTYPE(buf_parse_u ## bits ## _decimal);                \
  TEST_CASE_PROTOTYPE(buf_parse_u ## bits ## _hexadecimal);            \
  TEST_CASE_PROTOTYPE(buf_parse_u ## bits ## _octal)

BUF_PARSE_TEST_S_PROTOTYPE(8);
BUF_PARSE_TEST_S_PROTOTYPE(16);
BUF_PARSE_TEST_S_PROTOTYPE(32);
BUF_PARSE_TEST_S_PROTOTYPE(64);
TEST_CASE_PROTOTYPE(buf_parse_sw);
BUF_PARSE_TEST_U_PROTOTYPE(8);
BUF_PARSE_TEST_U_PROTOTYPE(16);
BUF_PARSE_TEST_U_PROTOTYPE(32);
BUF_PARSE_TEST_U_PROTOTYPE(64);
TEST_CASE_PROTOTYPE(buf_parse_uw);

#endif /* ifndef BUF_PARSE_TEST_SU */
#endif /* ifdef C3_TEST_BUF_PARSE_SU */
