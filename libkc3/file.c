/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#include "u32.h"

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
    fp = buf_file_fp(tag->data.pstruct->data);
    buf_file_close(tag->data.pstruct->data);
    fclose(fp);
  }
  else if (type == &g_sym_BufRW) {
    buf_rw = tag->data.pstruct->data;
    fp = buf_file_fp(buf_rw->r);
    buf_file_close(buf_rw->r);
    fclose(fp);
    fp = buf_file_fp(buf_rw->w);
    buf_file_close(buf_rw->w);
    fclose(fp);
  }
  else if (type == &g_sym_S8)
    close(tag->data.s8);
  else if (type == &g_sym_S16)
    close(tag->data.s16);
  else if (type == &g_sym_S32)
    close(tag->data.s32);
  else if (type == &g_sym_S64)
    close(tag->data.s64);
  else if (type == &g_sym_U8)
    close(tag->data.u8);
  else if (type == &g_sym_U16)
    close(tag->data.u16);
  else if (type == &g_sym_U32)
    close(tag->data.u32);
  else if (type == &g_sym_U64)
    close(tag->data.u64);
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
  if (path->size == 1 && path->ptr.pchar[0] == '/')
    return NULL;
  if ((dirsep_pos = str_rindex_character(path, '/', 0, -2)) < 0) {
    return str_init(dest, NULL, 0, "");
  }
  if (! dirsep_pos)
    return str_init(dest, NULL, 1, "/");
  return str_init_slice(dest, path, 0, dirsep_pos + 1);
}

bool file_ensure_directory (const s_str *path, const s_tag *mode)
{
  bool b;
  s_str dirname;
  s_str dir;
  s32 e;
  u32 m;
  s32 r;
  static const s_sym *sym_U32 = &g_sym_U32;
  if (false) {
    err_write_1("file_ensure_directory: ");
    err_inspect_str(path);
    err_write_1("\n");
  }
  if (! file_exists(path, &b))
    return false;
  if (b)
    return true;
  if (file_dirname(path, &dirname)) {
    if (str_ends_with_1(&dirname, "/", &b) && b) {
      if (! str_init_slice(&dir, &dirname, 0, -2))
        return false;
      str_clean(&dirname);
    }
    else
      dir = dirname;
    if (dir.size > 0) {
      if (! file_ensure_directory(&dir, mode)) {
        str_clean(&dir);
        return false;
      }
    }
    str_clean(&dir);
  }
  if (! u32_init_cast(&m, &sym_U32, mode))
    return false;
  if (true) {
    io_write_1("file_ensure_directory: MKDIR ");
    io_inspect_str(path);
    io_write_1("\n");
  }
#if defined(WIN32) || defined(WIN64)
  r = mkdir(path->ptr.pchar);
#else
  r = mkdir(path->ptr.pchar, m);
#endif
  if (r) {
    e = errno;
    err_write_1("file_ensure_directory: mkdir: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_write_1(strerror(e));
    return false;
  }
  return true;
}

bool file_ensure_parent_directory (const s_str *path, const s_tag *mode)
{
  bool b;
  s_str dir;
  if (! file_dirname(path, &dir))
    return false;
  b = file_ensure_directory(&dir, mode);
  str_clean(&dir);
  return b;
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
  if ((dot_pos = str_rindex_character(path, '.', 0, -1)) <= 0)
    return str_init_empty(dest);
  return str_init_slice(dest, path, dot_pos + 1, -1);
}

bool file_rename (const s_str *from, const s_str *to)
{
  sw e;
  if (rename(from->ptr.pchar, to->ptr.pchar)) {
    e = errno;
    err_write_1("file_rename: rename: ");
    err_inspect_str(from);
    err_write_1(": ");
    err_inspect_str(to);
    err_write_1(": ");
    err_write_1(strerror(e));
    err_write_1("\n");
    return false;
  }
  return true;
}

#if ! (defined(WIN32) || defined(WIN64))
bool file_link (const s_str *from, const s_str *to)
{
  sw e;
  if (link(from->ptr.pchar, to->ptr.pchar)) {
    e = errno;
    err_write_1("file_link: link: ");
    err_inspect_str(from);
    err_write_1(": ");
    err_inspect_str(to);
    err_write_1(": ");
    err_write_1(strerror(e));
    err_write_1("\n");
    return false;
  }
  return true;
}
#endif

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

s_str * file_name (const s_str *path, s_str *dest)
{
  sw slash_pos;
  assert(path);
  assert(dest);
  if ((slash_pos = str_rindex_character(path, '/', 0, -1)) < 0)
    return str_init_copy(dest, path);
  if (slash_pos - (sw) path->size < -1)
    return str_init_slice(dest, path, slash_pos + 1, -1);
  return str_init_empty(dest);
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

s32 * file_open_w (const s_str *path, s32 *dest)
{
  sw e;
  s32 fd;
  static const mode_t mode = 0666;
  assert(path);
  assert(dest);
  if ((fd = open(path->ptr.pchar,
                 O_CREAT |
                 O_TRUNC |
                 O_WRONLY |
                 O_BINARY,
                 mode)) < 0) {
    e = errno;
    err_write_1("file_open_w: ");
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

s_str * file_read_all (const s_str *path, s_str *dest)
{
  char *buf;
  s32 e;
  s32 fd;
  sw pos = 0;
  sw r;
  struct stat sb;
  if (stat(path->ptr.pchar, &sb)) {
    e = errno;
    err_write_1("file_read_all: stat: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_inspect_str(path);
    err_write_1("\n");
    return NULL;
  }
  fd = open(path->ptr.pchar, O_RDONLY | O_BINARY);
  if (fd < 0) {
    e = errno;
    err_write_1("file_read_all: open: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_inspect_str(path);
    err_write_1("\n");
    return NULL;
  }
  buf = alloc(sb.st_size);
  if (! buf) {
    err_puts("file_read_all: failed to allocate buf");
    close(fd);
    return NULL;
  }
  while (pos < sb.st_size) {
    if ((r = read(fd, buf + pos, sb.st_size - pos)) <= 0) {
      if (r < 0)
        e = errno;
      err_write_1("file_read_all: read: ");
      if (r < 0) {
        err_write_1(strerror(e));
        err_write_1(": ");
      }
      err_inspect_str(path);
      err_write_1("\n");
      return NULL;
    }
    pos += r;
  }
  close(fd);
  str_init(dest, buf, sb.st_size, buf);
  return dest;
}

s_str * file_read_max (const s_str *path, uw max, s_str *dest)
{
  char *buf;
  s32 e;
  s32 fd;
  sw r;
  uw size;
  s_str tmp;
  fd = open(path->ptr.pchar, O_RDONLY | O_BINARY);
  if (fd < 0) {
    e = errno;
    err_write_1("file_read_max: open: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_inspect_str(path);
    err_write_1("\n");
    return NULL;
  }
  buf = alloc(max);
  if (! buf) {
    err_puts("file_read_max: failed to allocate buf");
    close(fd);
    return NULL;
  }
  if ((r = read(fd, buf, max)) < 0) {
    e = errno;
    err_write_1("file_read_max: read: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_inspect_str(path);
    err_write_1("\n");
    free(buf);
    close(fd);
    return NULL;
  }
  size = r;
  close(fd);
  if (! str_init_alloc_copy(&tmp, size, buf)) {
    err_puts("file_read_max: str_init_alloc_copy");
    assert(! "file_read_max: str_init_alloc_copy");
    free(buf);
    close(fd);
    return NULL;
  }
  free(buf);
  *dest = tmp;
  return dest;
}

s_str * file_search (const s_str *suffix, const s_sym *mode,
                     s_str *dest)
{
  s_env *env;
  char buf_s[PATH_MAX];
  s_buf buf;
  const s_list *path;
  sw r;
  s_buf_save save;
  const s_str *str;
  s_str tmp = {0};
  env = env_global();
  buf_init(&buf, false, sizeof(buf_s), buf_s);
  if ((r = buf_write_str(&buf, env->argv0_dir)) < 0)
    return NULL;
  buf_save_init(&buf, &save);
  path = env->path;
  while (path) {
    if (path->tag.type == TAG_STR) {
      buf_save_restore_rpos(&buf, &save);
      buf_save_restore_wpos(&buf, &save);
      str = &path->tag.data.str;
      if ((r = buf_write_str(&buf, str)) < 0)
        return NULL;
      if (str->size > 0 &&
          str->ptr.pchar[str->size - 1] != '/' &&
          suffix->size > 0 &&
          suffix->ptr.pchar[0] != '/' &&
          (r = buf_write_1(&buf, "/")) < 0)
        return NULL;
      if ((r = buf_write_str(&buf, suffix)) < 0)
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
  return file_stat_init_struct_stat(dest, &sb);
}

s_file_stat * file_stat_init_struct_stat (s_file_stat *dest,
                                          const struct stat *sb)
{
  s_file_stat tmp = {0};
  assert(dest);
  assert(sb);
  tmp.st_dev = sb->st_dev;
  tmp.st_ino = sb->st_ino;
  tmp.st_mode = 0;
  if (sb->st_mode & S_IFREG)
    tmp.st_mode = list_new_sym(&g_sym_file, tmp.st_mode);
  else if (sb->st_mode & S_IFDIR)
    tmp.st_mode = list_new_sym(&g_sym_directory, tmp.st_mode);
  tmp.st_nlink = sb->st_nlink;
  tmp.st_uid = sb->st_uid;
  tmp.st_gid = sb->st_gid;
  tmp.st_rdev = sb->st_rdev;
  tmp.st_size = sb->st_size;
#ifdef WIN32
  tmp.st_blksize = 0;
  tmp.st_blocks = 0;
#else
  tmp.st_blksize = sb->st_blksize;
  tmp.st_blocks = sb->st_blocks;
#endif
#if HAVE_STAT_MTIM
# ifdef __APPLE__
  tmp.st_atim.tv_sec  = sb->st_mtimespec.tv_sec;
  tmp.st_atim.tv_nsec = sb->st_mtimespec.tv_nsec;
# else
  tmp.st_atim.tv_sec  = sb->st_atim.tv_sec;
  tmp.st_atim.tv_nsec = sb->st_atim.tv_nsec;
  tmp.st_mtim.tv_sec  = sb->st_mtim.tv_sec;
  tmp.st_mtim.tv_nsec = sb->st_mtim.tv_nsec;
  tmp.st_ctim.tv_sec  = sb->st_ctim.tv_sec;
  tmp.st_ctim.tv_nsec = sb->st_ctim.tv_nsec;
# endif
#else
  tmp.st_atim.tv_sec  = sb->st_atime;
  tmp.st_atim.tv_nsec = 0;
  tmp.st_mtim.tv_sec  = sb->st_mtime;
  tmp.st_mtim.tv_nsec = 0;
  tmp.st_ctim.tv_sec  = sb->st_ctime;
  tmp.st_ctim.tv_nsec = 0;
#endif
  *dest = tmp;
  return dest;
}

struct stat * file_stat_to_struct_stat (const s_file_stat *file_stat,
                                        struct stat *dest)
{
  bool b;
  s_tag tag_sym_directory;
  s_tag tag_sym_file;
  struct stat tmp = {0};
  assert(file_stat);
  assert(dest);
  tmp.st_dev = file_stat->st_dev;
  tmp.st_ino = file_stat->st_ino;
  tmp.st_mode = 0;
  tag_init_sym(&tag_sym_file, &g_sym_file);
  if (! list_has((const s_list * const *) &file_stat->st_mode,
                 &tag_sym_file, &b)) {
    err_puts("file_stat_to_struct_stat: list_has(:file)");
    err_puts("file_stat_to_struct_stat: list_has(:file)");
  }
  if (b)
    tmp.st_mode |= S_IFREG;
  else {
    tag_init_sym(&tag_sym_directory, &g_sym_directory);
    if (! list_has((const s_list * const *) &file_stat->st_mode,
                   &tag_sym_directory, &b)) {
      err_puts("file_stat_to_struct_stat: list_has(:directory)");
      err_puts("file_stat_to_struct_stat: list_has(:directory)");
    }
    if (b)
      tmp.st_mode |= S_IFDIR;
  }
  tmp.st_nlink = file_stat->st_nlink;
  tmp.st_uid = file_stat->st_uid;
  tmp.st_gid = file_stat->st_gid;
  tmp.st_rdev = file_stat->st_rdev;
  tmp.st_size = file_stat->st_size;
#ifndef WIN32
  tmp.st_blksize = file_stat->st_blksize;
  tmp.st_blocks = file_stat->st_blocks;
#endif
#if HAVE_STAT_MTIM
# ifdef __APPLE__
  tmp.st_atimespec.tv_sec = file_stat->st_atim.tv_sec;
  tmp.st_atimespec.tv_nsec = file_stat->st_atim.tv_nsec;
  tmp.st_ctimespec.tv_sec = file_stat->st_ctim.tv_sec;
  tmp.st_ctimespec.tv_nsec = file_stat->st_ctim.tv_nsec;
  tmp.st_mtimespec.tv_sec = file_stat->st_mtim.tv_sec;
  tmp.st_mtimespec.tv_nsec = file_stat->st_mtim.tv_nsec;
# else
  tmp.st_atim.tv_sec  = file_stat->st_atim.tv_sec;
  tmp.st_atim.tv_nsec = file_stat->st_atim.tv_nsec;
  tmp.st_ctim.tv_sec  = file_stat->st_ctim.tv_sec;
  tmp.st_ctim.tv_nsec = file_stat->st_ctim.tv_nsec;
  tmp.st_mtim.tv_sec  = file_stat->st_mtim.tv_sec;
  tmp.st_mtim.tv_nsec = file_stat->st_mtim.tv_nsec;
# endif
#else
  tmp.st_atime = file_stat->st_atim.tv_sec;
  tmp.st_ctime = file_stat->st_ctim.tv_sec;
  tmp.st_mtime = file_stat->st_mtim.tv_sec;
#endif
  *dest = tmp;
  return dest;
}

#if ! (defined(WIN32) || defined(WIN64))
bool file_unlink (const s_str *path)
{
  sw e;
  if (unlink(path->ptr.pchar)) {
    e = errno;
    err_write_1("file_unlink: unlink: ");
    err_inspect_str(path);
    err_write_1(": ");
    err_write_1(strerror(e));
    err_write_1("\n");
    return false;
  }
  return true;
}
#endif

bool file_write (const s_str *path, const s_str *data)
{
  s32 e;
  s32 fd;
  static const mode_t mode = 0666;
  sw pos = 0;
  sw w;
  fd = open(path->ptr.pchar, O_CREAT | O_TRUNC | O_WRONLY | O_BINARY,
	    mode);
  if (fd < 0) {
    e = errno;
    err_write_1("file_write: open: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_inspect_str(path);
    err_write_1("\n");
    return false;
  }
  while (pos < data->size) {
    if ((w = write(fd, data->ptr.pchar, data->size - pos)) <= 0) {
      if (w < 0)
        e = errno;
      err_write_1("file_write: write: ");
      if (w < 0) {
        err_write_1(strerror(e));
        err_write_1(": ");
      }
      err_inspect_str(path);
      err_write_1("\n");
      return false;
    }
    pos += w;
  }
  close(fd);
  return true;
}
