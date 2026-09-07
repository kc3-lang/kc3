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
#include "../libkc3/assert.h"
#include "../libkc3/buf.h"
#include "../libkc3/buf_file.h"
#include "../libkc3/file.h"
#include "../libkc3/str.h"
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include "test.h"

void buf_file_test (void);
TEST_CASE_PROTOTYPE(buf_file_open_r_close);
TEST_CASE_PROTOTYPE(buf_file_open_r_refill);
TEST_CASE_PROTOTYPE(buf_file_open_w_close);
TEST_CASE_PROTOTYPE(buf_file_open_w_flush);
TEST_CASE_PROTOTYPE(file_rm_rf_symlink);

void buf_file_test (void)
{
  TEST_CASE_RUN(buf_file_open_r_close);
  TEST_CASE_RUN(buf_file_open_r_refill);
  TEST_CASE_RUN(buf_file_open_w_close);
  TEST_CASE_RUN(file_rm_rf_symlink);
}

TEST_CASE(buf_file_open_r_close)
{
  char b[16];
  s_buf buf;
  FILE *fp;
  fp = fopen("zero", "r");
  assert(fp);
  buf_init(&buf, false, sizeof(b), b);
  TEST_EQ(buf_file_open_r(&buf, fp), &buf);
  buf_file_close(&buf);
  buf_clean(&buf);
  test_ok();
  fclose(fp);
}
TEST_CASE_END(buf_file_open_r_close)

TEST_CASE(buf_file_open_r_refill)
{
  u8 b = 0x80;
  char bu[16];
  s_buf buf;
  FILE *fp;
  sw i = 64;
  test_context("buf_file_open_r_refill(zero)");
  fp = fopen("zero", "r");
  TEST_ASSERT(fp);
  buf_init(&buf, false, sizeof(bu), bu);
  buf_file_open_r(&buf, fp);
  while (i--) {
    TEST_EQ(buf_read_u8(&buf, &b), 1);
    TEST_EQ(b, 0);
  }
  buf_file_close(&buf);
  buf_clean(&buf);
  fclose(fp);
  test_context(NULL);
}
TEST_CASE_END(buf_file_open_r_refill)

TEST_CASE(buf_file_open_w_close)
{
}
TEST_CASE_END(buf_file_open_w_close)

TEST_CASE(file_rm_rf_symlink)
{
  char base[] = "/tmp/kc3_file_rm_rf.XXXXXXXXXX";
  s32 fd;
  char keep[PATH_MAX];
  char link[PATH_MAX];
  char outside[PATH_MAX];
  s_str path;
  char tree[PATH_MAX];
  TEST_ASSERT(mkdtemp(base));
  TEST_ASSERT(snprintf(outside, sizeof(outside), "%s/outside", base) > 0);
  TEST_ASSERT(snprintf(tree, sizeof(tree), "%s/tree", base) > 0);
  TEST_ASSERT(snprintf(keep, sizeof(keep), "%s/keep", outside) > 0);
  TEST_ASSERT(snprintf(link, sizeof(link), "%s/link", tree) > 0);
  TEST_EQ(mkdir(outside, 0700), 0);
  TEST_EQ(mkdir(tree, 0700), 0);
  fd = open(keep, O_CREAT | O_WRONLY, 0600);
  TEST_ASSERT(fd >= 0);
  TEST_EQ(close(fd), 0);
  TEST_EQ(symlink("../outside", link), 0);
  str_init_1(&path, NULL, tree);
  TEST_ASSERT(file_rm_rf(&path));
  TEST_EQ(access(tree, F_OK), -1);
  TEST_EQ(access(keep, F_OK), 0);
  TEST_EQ(unlink(keep), 0);
  TEST_EQ(rmdir(outside), 0);
  TEST_EQ(rmdir(base), 0);
}
TEST_CASE_END(file_rm_rf_symlink)
