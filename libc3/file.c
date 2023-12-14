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
#include <assert.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include "buf.h"
#include "buf_save.h"
#include "env.h"
#include "file.h"
#include "io.h"
#include "list.h"
#include "str.h"
#include "sym.h"
#include "time.h"
#include "config.h"

bool * file_access (const s_str *path, const s_sym *mode,
                    bool *dest)
{
  sw m;
  if (mode == sym_1("r"))
    m = R_OK;
  else if (mode == sym_1("rw"))
    m = R_OK | W_OK;
  else if (mode == sym_1("rwx"))
    m = R_OK | W_OK | X_OK;
  else if (mode == sym_1("rx"))
    m = R_OK | X_OK;
  else if (mode == sym_1("w"))
    m = W_OK;
  else if (mode == sym_1("wx"))
    m = W_OK | X_OK;
  else if (mode == sym_1("x"))
    m = X_OK;
  else
    m = F_OK;
  *dest = access(path->ptr.ps8, m) ? false : true;
  return dest;
}

sw file_copy_1 (const char *from, const char *to)
{
  s8 buf[4096];
  sw fd_from = -1;
  sw fd_to = -1;
  s8 *out;
  sw r;
  sw saved_errno;
  sw w;
  if ((fd_from = open(from, O_RDONLY | O_BINARY)) < 0) {
    warn("cp: %s", from);
    return -1;
  }
  if ((fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666)) < 0) {
    warn("cp: %s", to);
    goto error;
  }
  while ((r = read(fd_from, buf, sizeof buf)) > 0) {
    out = buf;
    do {
      if ((w = write(fd_to, out, r)) >= 0) {
        r -= w;
        out += w;
      }
      else if (errno != EINTR)
        goto error;
    } while (r > 0);
  }
  if (r == 0) {
    if (close(fd_to) < 0) {
      fd_to = -1;
      goto error;
    }
    close(fd_from);
    return 0;
  }
 error:
  saved_errno = errno;
  close(fd_from);
  if (fd_to >= 0)
    close(fd_to);
  errno = saved_errno;
  return -1;
}

s_str * file_dirname (const s_str *path, s_str *dest)
{
  uw dirsep_pos;
  assert(path);
  assert(dest);
  if (! str_rindex_character(path, '/', &dirsep_pos))
    return str_init(dest, NULL, 2, "./");
  if (! dirsep_pos)
    return str_init(dest, NULL, 1, "/");
  return str_init_slice(dest, path, 0, dirsep_pos + 1);
}

s_tag * file_mtime (const s_str *path, s_tag *dest)
{
  struct stat sb;
  assert(path);
  assert(dest);
  if (stat(path->ptr.ps8, &sb)) {
    warn("file_mtime: %s", path->ptr.ps8);
    return NULL;
  }
#if HAVE_STAT_MTIM
  return time_to_tag(&sb.st_mtim, dest);
#else
  s_time tmp;
  tmp.tv_sec = sb.st_mtime;
  tmp.tv_nsec = 0;
  return time_to_tag(&tmp, dest);
#endif
}

s_str * file_search (const s_str *suffix, const s_sym *mode,
                     s_str *dest)
{
  bool access;
  s8 buf_s[PATH_MAX];
  s_buf buf;
  const s_list *path;
  sw r;
  s_buf_save save;
  const s_str *str;
  s_str tmp;
  buf_init(&buf, false, PATH_MAX, buf_s);
  if ((r = buf_write_str(&buf, &g_c3_env.argv0_dir)) < 0)
    return NULL;
  buf_save_init(&buf, &save);
  path = g_c3_env.path;
  while (path) {
    if (path->tag.type == TAG_STR) {
      buf_save_restore_rpos(&buf, &save);
      buf_save_restore_wpos(&buf, &save);
      str = &path->tag.data.str;
      if ((r = buf_write_str(&buf, str)) < 0 ||
          (str->ptr.ps8[str->size - 1] != '/' &&
           (r = buf_write_1(&buf, "/")) < 0) ||
          (r = buf_write_str(&buf, suffix)) < 0)
        return NULL;
      buf_read_to_str(&buf, &tmp);
      //io_inspect_str(&tmp);
      file_access(&tmp, mode, &access);
      if (access) {
        *dest = tmp;
        return dest;
      }
      str_clean(&tmp);
    }
    path = list_next(path);
  }
  return NULL;
}
