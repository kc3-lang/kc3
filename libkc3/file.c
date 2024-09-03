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
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "alloc.h"
#include "assert.h"
#include "buf.h"
#include "buf_fd.h"
#include "buf_file.h"
#include "buf_save.h"
#include "config.h"
#include "env.h"
#include "file.h"
#include "io.h"
#include "list.h"
#include "str.h"
#include "sym.h"
#include "tag.h"
#include "time.h"
#include "config.h"

#ifndef O_BINARY
# define O_BINARY 0
#endif

bool file_access (const s_str *path, const s_sym *mode)
{
  sw m;
  if (mode == &g_sym_r)
    m = R_OK;
  else if (mode == &g_sym_rw)
    m = R_OK | W_OK;
  else if (mode == &g_sym_rwx)
    m = R_OK | W_OK | X_OK;
  else if (mode == &g_sym_rx)
    m = R_OK | X_OK;
  else if (mode == &g_sym_w)
    m = W_OK;
  else if (mode == &g_sym_wx)
    m = W_OK | X_OK;
  else if (mode == &g_sym_x)
    m = X_OK;
  else
    m = F_OK;
  return ! access(path->ptr.pchar, m);
}

void file_close (const s_tag *tag)
{
  s_buf_rw *buf_rw;
  FILE *fp;
  const s_sym *type;
  if (! tag_type(tag, &type))
    return;
  if (type == &g_sym_Buf) {
    fp = buf_file_fp(tag->data.struct_.data);
    buf_file_close(tag->data.struct_.data);
    fclose(fp);
  }
  else if (type == &g_sym_BufRW) {
    buf_rw = tag->data.struct_.data;
    fp = buf_file_fp(buf_rw->r);
    buf_file_close(buf_rw->r);
    fclose(fp);
    fp = buf_file_fp(buf_rw->w);
    buf_file_close(buf_rw->w);
    fclose(fp);
  }
  else {
    err_write_1("file_close: unknown tag type: ");
    err_inspect_sym(&type);
    err_write_1("\n");
  }
}

sw file_copy (const char *from, const char *to)
{
  char buf[4096];
  s32 e;
  sw fd_from = -1;
  sw fd_to = -1;
  char *out;
  sw r;
  sw saved_errno;
  sw w;
  if ((fd_from = open(from, O_RDONLY | O_BINARY)) < 0) {
    e = errno;
    err_write_1("file_copy: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_puts(from);
    assert(! "file_copy: failed to open file for reading");
    return -1;
  }
  if ((fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666)) < 0) {
    e = errno;
    err_write_1("file_copy: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_puts(to);
    assert(! "file_copy: failed to open file for writing");
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
  sw dirsep_pos;
  assert(path);
  assert(dest);
  if ((dirsep_pos = str_rindex_character(path, '/')) < 0)
    return str_init(dest, NULL, 2, "./");
  if (! dirsep_pos)
    return str_init(dest, NULL, 1, "/");
  return str_init_slice(dest, path, 0, dirsep_pos + 1);
}

bool * file_exists (const s_str *path, bool *dest)
{
  struct stat sb;
  assert(path);
  assert(dest);
  *dest = ! stat(path->ptr.pchar, &sb);
  return dest;
}

s_str * file_ext (const s_str *path, s_str *dest)
{
  sw dot_pos;
  assert(path);
  assert(dest);
  if ((dot_pos = str_rindex_character(path, '.')) <= 0)
    return str_init_empty(dest);
  return str_init_slice(dest, path, dot_pos + 1, -1);
}

s_list ** file_list (const s_str *path, s_list **dest)
{
  DIR           *dir;
  struct dirent *dirent;
  s32 e;
  s_list **tail;
  s_list *tmp = NULL;
  dir = opendir(path->ptr.pchar);
  if (! dir) {
    e = errno;
    err_write_1("file_list: opendir: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_write_str(path);
    return NULL;
  }
  tail = &tmp;
  while ((dirent = readdir(dir))) {
    *tail = list_new_str_1_alloc(dirent->d_name, NULL);
    if (! *tail) {
      list_delete_all(tmp);
      closedir(dir);
      return NULL;
    }
    tail = &(*tail)->next.data.list;
  }
  closedir(dir);
  *dest = tmp;
  return dest;
}

s_time * file_mtime (const s_str *path, s_time *dest)
{
  s32 e;
  struct stat sb;
  s_time tmp = {0};
  assert(path);
  assert(dest);
  if (stat(path->ptr.pchar, &sb)) {
    e = errno;
    err_write_1("file_mtime: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_puts(path->ptr.pchar);
    return NULL;
  }
#if HAVE_STAT_MTIM
# ifdef __APPLE__
  tmp.tv_sec = sb.st_mtimespec.tv_sec;
  tmp.tv_nsec = sb.st_mtimespec.tv_nsec;
# else
  tmp.tv_sec = sb.st_mtim.tv_sec;
  tmp.tv_nsec = sb.st_mtim.tv_nsec;
# endif
#else
  tmp.tv_sec = sb.st_mtime;
  tmp.tv_nsec = 0;
#endif
  *dest = tmp;
  return dest;
}

FILE * file_open (const char *path, const char *mode)
{
  s32 e;
  FILE *fp;
  fp = fopen(path, mode);
  if (! fp) {
    e = errno;
    err_write_1("file_open: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_puts(path);
    assert(! "facts_open_file: fopen");
    return NULL;
  }
  return fp;
}

s32 * file_open_r (const s_str *path, s32 *dest)
{
  sw e;
  s32 fd;
  assert(path);
  assert(dest);
  if ((fd = open(path->ptr.pchar, O_RDONLY | O_BINARY)) < 0) {
    e = errno;
    err_write_1("file_open_r: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_puts(strerror(e));
    return NULL;
  }
  *dest = fd;
  return dest;
}

s_str * file_pwd (s_str *dest)
{
  char buf[PATH_MAX];
  char *pchar;
  uw size;
  if (! getcwd(buf, sizeof(buf)))
    return NULL;
  size = strlen(buf);
  pchar = calloc(1, size + 1);
  memcpy(pchar, buf, size);
  str_init(dest, pchar, size, pchar);
  return dest;
}

s_str * file_read (const s_str *path, s_str *dest)
{
  char *buf;
  s32 e;
  s32 fd;
  uw r;
  sw size = 0;
  struct stat sb;
  if (stat(path->ptr.pchar, &sb)) {
    err_puts("file_read: stat");
    return NULL;
  }
  fd = open(path->ptr.pchar, O_RDONLY | O_BINARY);
  if (fd < 0) {
    e = errno;
    err_write_1("file_read: open: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_write_str(path);
    return NULL;
  }
  buf = alloc(sb.st_size);
  if (! buf) {
    err_puts("file_read: failed to allocate buf");
    close(fd);
    return NULL;
  }
  while (size < sb.st_size) {
    if (! (r = read(fd, buf, sb.st_size))) {
      err_puts("file_read: read = 0");
      free(buf);
      close(fd);
      return NULL;
    }
    size += r;
  }
  close(fd);
  str_init(dest, buf, sb.st_size, buf);
  return dest;
}

s_str * file_search (const s_str *suffix, const s_sym *mode,
                     s_str *dest)
{
  char buf_s[PATH_MAX];
  s_buf buf;
  const s_list *path;
  sw r;
  s_buf_save save;
  const s_str *str;
  s_str tmp = {0};
  buf_init(&buf, false, PATH_MAX, buf_s);
  if ((r = buf_write_str(&buf, &g_kc3_env.argv0_dir)) < 0)
    return NULL;
  buf_save_init(&buf, &save);
  path = g_kc3_env.path;
  while (path) {
    if (path->tag.type == TAG_STR) {
      buf_save_restore_rpos(&buf, &save);
      buf_save_restore_wpos(&buf, &save);
      str = &path->tag.data.str;
      if ((r = buf_write_str(&buf, str)) < 0 ||
          (str->ptr.pchar[str->size - 1] != '/' &&
           (r = buf_write_1(&buf, "/")) < 0) ||
          (r = buf_write_str(&buf, suffix)) < 0)
        return NULL;
      buf_read_to_str(&buf, &tmp);
      //io_inspect_str(&tmp);
      if (file_access(&tmp, mode)) {
        *dest = tmp;
        return dest;
      }
      str_clean(&tmp);
    }
    path = list_next(path);
  }
  return NULL;
}

s_file_stat * file_stat (const s_str *path, s_file_stat *dest)
{
  s32 e;
  struct stat sb;
  s_file_stat tmp = {0};
  assert(path);
  assert(dest);
  if (stat(path->ptr.pchar, &sb)) {
    e = errno;
    err_write_1("file_stat: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_puts(path->ptr.pchar);
    return NULL;
  }
  tmp.st_dev = sb.st_dev;
  tmp.st_ino = sb.st_ino;
  tmp.st_mode = NULL;
  if (sb.st_mode & S_IFREG)
    tmp.st_mode = list_new_sym(&g_sym_file, tmp.st_mode);
  else if (sb.st_mode & S_IFDIR)
    tmp.st_mode = list_new_sym(&g_sym_directory, tmp.st_mode);
  tmp.st_nlink = sb.st_nlink;
  tmp.st_uid = sb.st_uid;
  tmp.st_gid = sb.st_gid;
  tmp.st_rdev = sb.st_rdev;
  tmp.st_size = sb.st_size;
  tmp.st_blksize = sb.st_blksize;
  tmp.st_blocks = sb.st_blocks;
#if HAVE_STAT_MTIM
# ifdef __APPLE__
  tmp.st_atim.tv_sec  = sb.st_mtimespec.tv_sec;
  tmp.st_atim.tv_nsec = sb.st_mtimespec.tv_nsec;
# else
  tmp.st_atim.tv_sec  = sb.st_atim.tv_sec;
  tmp.st_atim.tv_nsec = sb.st_atim.tv_nsec;
  tmp.st_mtim.tv_sec  = sb.st_mtim.tv_sec;
  tmp.st_mtim.tv_nsec = sb.st_mtim.tv_nsec;
  tmp.st_ctim.tv_sec  = sb.st_ctim.tv_sec;
  tmp.st_ctim.tv_nsec = sb.st_ctim.tv_nsec;
# endif
#else
  tmp.st_atim.tv_sec  = sb.st_atime;
  tmp.st_atim.tv_nsec = 0;
  tmp.st_mtim.tv_sec  = sb.st_mtime;
  tmp.st_mtim.tv_nsec = 0;
  tmp.st_ctim.tv_sec  = sb.st_ctime;
  tmp.st_ctim.tv_nsec = 0;
#endif
  *dest = tmp;
  return dest;
}
