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
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/alloc.h"

static bool g_audit_file_alloc_fail;

static void * audit_file_alloc (uw size)
{
  if (g_audit_file_alloc_fail) {
    g_audit_file_alloc_fail = false;
    return NULL;
  }
  return alloc(size);
}

#define file_access audit_file_access
#define file_basename audit_file_basename
#define file_close audit_file_close
#define file_copy audit_file_copy
#define file_dirname audit_file_dirname
#define file_ensure_directory audit_file_ensure_directory
#define file_ensure_parent_directory audit_file_ensure_parent_directory
#define file_exists audit_file_exists
#define file_ext audit_file_ext
#define file_is_directory audit_file_is_directory
#define file_is_directory_1 audit_file_is_directory_1
#define file_is_regular audit_file_is_regular
#define file_link audit_file_link
#define file_list audit_file_list
#define file_list_recursive audit_file_list_recursive
#define file_mmap audit_file_mmap
#define file_mtime audit_file_mtime
#define file_name audit_file_name
#define file_open audit_file_open
#define file_open_r audit_file_open_r
#define file_open_w audit_file_open_w
#define file_pwd audit_file_pwd
#define file_relative audit_file_relative
#define file_read_all audit_file_read_all
#define file_read_max audit_file_read_max
#define file_read_slice audit_file_read_slice
#define file_rm_rf audit_file_rm_rf
#define file_rename audit_file_rename
#define file_rmdir audit_file_rmdir
#define file_search audit_file_search
#define file_sha1 audit_file_sha1
#define file_sha1_base64url audit_file_sha1_base64url
#define file_sha512 audit_file_sha512
#define file_sha512_base64url audit_file_sha512_base64url
#define file_stat audit_file_stat
#define file_stat_init_struct_stat audit_file_stat_init_struct_stat
#define file_stat_to_struct_stat audit_file_stat_to_struct_stat
#define file_unlink audit_file_unlink
#define file_write audit_file_write
#define file_write_str audit_file_write_str
#include "../libkc3/kc3.h"
#define alloc audit_file_alloc
#include "../libkc3/file.c"
#undef alloc
#include "test.h"

char file_pwd_alloc_failure_check (void)
{
  const s_str old = STR("old");
  s_str dest = old;
  s_str *result;
  g_audit_file_alloc_fail = true;
  result = file_pwd(&dest);
  g_audit_file_alloc_fail = false;
  if (result) {
    if (dest.free.p_pvoid)
      str_clean(&dest);
    return 1;
  }
  return dest.free.p_pvoid != old.free.p_pvoid ||
    dest.ptr.p_pvoid != old.ptr.p_pvoid || dest.size != old.size;
}

static int file_pwd_child (char (*check)(void))
{
  pid_t pid;
  pid_t result;
  int status;
  if ((pid = fork()) < 0)
    return -1;
  if (! pid) {
    alarm(2);
    _exit(check());
  }
  do {
    result = waitpid(pid, &status, 0);
  } while (result < 0 && errno == EINTR);
  if (result != pid || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

TEST_CASE(file_pwd_alloc_failure)
{
  TEST_EQ(file_pwd_child(file_pwd_alloc_failure_check), 0);
}
TEST_CASE_END(file_pwd_alloc_failure)

void file_pwd_test (void)
{
  TEST_CASE_RUN(file_pwd_alloc_failure);
}
