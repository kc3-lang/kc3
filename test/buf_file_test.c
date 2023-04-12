/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "../libc3/buf.h"
#include "../libc3/buf_file.h"
#include "test.h"

void buf_file_test ();
TEST_CASE_PROTOTYPE(buf_file_open_r_close);
TEST_CASE_PROTOTYPE(buf_file_open_r_refill);
TEST_CASE_PROTOTYPE(buf_file_open_w_close);
TEST_CASE_PROTOTYPE(buf_file_open_w_flush);

void buf_file_test ()
{
  buf_file_test_open_r_close();
  buf_file_test_open_r_refill();
  buf_file_test_open_w_close();
}

TEST_CASE(buf_file_open_r_close)
{
  s8 b[16];
  s_buf buf;
  FILE *fp;
  fp = fopen("/dev/null", "r");
  assert(fp);
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_file_open_r(&buf, fp), &buf);
  buf_file_close(&buf);
  test_ok();
  fclose(fp);
}
TEST_CASE_END(buf_file_open_r_close)

TEST_CASE(buf_file_open_r_refill)
{
  u8 b = 0x80;
  s8 bu[16];
  s_buf buf;
  FILE *fp;
  sw i = 64;
  test_context("buf_file_open_r_refill(/dev/zero)");
  fp = fopen("/dev/zero", "r");
  assert(fp);
  buf_init(&buf, false, sizeof(bu), bu);
  buf_file_open_r(&buf, fp);
  while (i--) {
    TEST_EQ(buf_read_u8(&buf, &b), 1);
    TEST_EQ(b, 0);
  }
  buf_file_close(&buf);
  fclose(fp);
  test_context(NULL);
}
TEST_CASE_END(buf_file_open_r_refill)

TEST_CASE(buf_file_open_w_close)
{
}
TEST_CASE_END(buf_file_open_w_close)
