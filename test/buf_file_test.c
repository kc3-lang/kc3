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
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include "test.h"

void buf_file_test (void);
TEST_CASE_PROTOTYPE(buf_file_open_r_close);
TEST_CASE_PROTOTYPE(buf_file_open_r_refill);
TEST_CASE_PROTOTYPE(buf_file_open_w_close);
TEST_CASE_PROTOTYPE(buf_file_open_w_flush);
TEST_CASE_PROTOTYPE(buf_file_flush_error);
TEST_CASE_PROTOTYPE(buf_read_until_file);
TEST_CASE_PROTOTYPE(buf_read_until_file_error);
TEST_CASE_PROTOTYPE(file_rm_rf_symlink);

void buf_file_test (void)
{
  TEST_CASE_RUN(buf_file_open_r_close);
  TEST_CASE_RUN(buf_file_open_r_refill);
  TEST_CASE_RUN(buf_file_open_w_close);
  TEST_CASE_RUN(buf_file_flush_error);
  TEST_CASE_RUN(buf_read_until_file);
  TEST_CASE_RUN(buf_read_until_file_error);
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

TEST_CASE(buf_read_until_file)
{
  s_buf buf;
  char filename[] = "/tmp/kc3_buf_until.XXXXXXXXXX";
  s32 fd;
  const s_str end = STR("BOUNDARY");
  uw i;
  uw j;
  s_str path;
  s_str result;
  struct stat st;
  const uw sizes[] = {0, 1, BUF_SIZE, BUF_SIZE + 1, 2 * BUF_SIZE + 7};
  TEST_ASSERT((fd = mkstemp(filename)) >= 0);
  TEST_EQ(close(fd), 0);
  TEST_ASSERT(str_init_1(&path, NULL, filename));
  for (i = 0; i < sizeof(sizes) / sizeof(*sizes); i++) {
    TEST_ASSERT(buf_init_alloc(&buf, sizes[i] + end.size + 4));
    for (j = 0; j < sizes[i]; j++)
      buf.ptr.p_pu8[j] = j % 251;
    memcpy(buf.ptr.p_pchar + sizes[i], "BOUNDARYtail", 12);
    buf.wpos = sizes[i] + end.size + 4;
    TEST_EQ(buf_read_until_str_into_file(&buf, &end, &path),
             sizes[i] + end.size);
    TEST_EQ(stat(filename, &st), 0);
    TEST_EQ(st.st_size, sizes[i]);
    if (sizes[i]) {
      TEST_ASSERT(file_read_all(&path, &result));
      TEST_EQ(result.size, sizes[i]);
      TEST_EQ(memcmp(result.ptr.p_pchar, buf.ptr.p_pchar, sizes[i]), 0);
      str_clean(&result);
    }
    TEST_EQ(buf_read_1(&buf, "tail"), 4);
    buf_clean(&buf);
  }
  TEST_EQ(unlink(filename), 0);
}
TEST_CASE_END(buf_read_until_file)

TEST_CASE(buf_read_until_file_error)
{
  s_buf buf;
  char filename[] = "/tmp/kc3_buf_until_error.XXXXXXXXXX";
  s32 fd;
  const s_str end = STR("END");
  uw i;
  struct rlimit limit = {0, 0};
  s_str path;
  pid_t pid;
  sw r;
  const uw sizes[] = {1, BUF_SIZE + 1};
  int status;
  TEST_ASSERT((fd = mkstemp(filename)) >= 0);
  TEST_EQ(close(fd), 0);
  TEST_ASSERT(str_init_1(&path, NULL, filename));
  for (i = 0; i < sizeof(sizes) / sizeof(*sizes); i++) {
    TEST_ASSERT(buf_init_alloc(&buf, sizes[i] + end.size));
    memset(buf.ptr.p_pchar, 'x', sizes[i]);
    memcpy(buf.ptr.p_pchar + sizes[i], "END", 3);
    buf.wpos = sizes[i] + end.size;
    TEST_ASSERT((pid = fork()) >= 0);
    if (! pid) {
      fd = open("/dev/null", O_WRONLY);
      if (fd < 0 || dup2(fd, STDERR_FILENO) < 0)
        _exit(2);
      close(fd);
      if (signal(SIGXFSZ, SIG_IGN) == SIG_ERR ||
          setrlimit(RLIMIT_FSIZE, &limit))
        _exit(2);
      r = buf_read_until_str_into_file(&buf, &end, &path);
      buf_clean(&buf);
      _exit(r < 0 ? 0 : 1);
    }
    TEST_EQ(waitpid(pid, &status, 0), pid);
    buf_clean(&buf);
    TEST_EQ(unlink(filename), 0);
    TEST_EQ(WIFSIGNALED(status) ? WTERMSIG(status) : 0, 0);
    TEST_ASSERT(WIFEXITED(status));
    TEST_EQ(WEXITSTATUS(status), 0);
  }
}
TEST_CASE_END(buf_read_until_file_error)

TEST_CASE(buf_file_flush_error)
{
  s_buf buf;
  FILE *fp;
  struct rlimit limit;
  struct rlimit saved;
  pid_t pid;
  int status;
  TEST_ASSERT((fp = tmpfile()));
  TEST_ASSERT((pid = fork()) >= 0);
  if (! pid) {
    if (getrlimit(RLIMIT_FSIZE, &saved) ||
        signal(SIGXFSZ, SIG_IGN) == SIG_ERR)
      _exit(2);
    limit = saved;
    limit.rlim_cur = 0;
    if (setrlimit(RLIMIT_FSIZE, &limit) ||
        ! buf_init_alloc(&buf, 64) ||
        ! buf_file_open_w(&buf, fp) ||
        buf_write_1(&buf, "failed record") != 13)
      _exit(2);
    if (buf_flush(&buf) >= 0)
      _exit(3);
    if (setrlimit(RLIMIT_FSIZE, &saved))
      _exit(2);
    clearerr(fp);
    if (buf_file_is_open(&buf))
      _exit(5);
    if (buf_flush(&buf) >= 0)
      _exit(4);
    buf_file_close(&buf);
    buf_clean(&buf);
    _exit(0);
  }
  TEST_EQ(waitpid(pid, &status, 0), pid);
  fclose(fp);
  TEST_ASSERT(WIFEXITED(status));
  TEST_EQ(WEXITSTATUS(status), 0);
}
TEST_CASE_END(buf_file_flush_error)

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
