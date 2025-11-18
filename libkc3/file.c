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
#include "env.h"
#include "error.h"
#include "file.h"
#include "io.h"
#include "list.h"
#include "plist.h"
#include "sha1.h"
#include "sha512.h"
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
  s32 e;
  sw m;
  bool r;
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
  if (false) {
    err_write_1("file_access: ");
    err_inspect_str(path);
  }
  r = ! access(path->ptr.pchar, m);
  if (false) {
    if (r)
      err_puts(": OK");
    else {
      e = errno;
      err_write_1(": ERROR: ");
      err_puts(strerror(e));
    }
  }
  return r;
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
    err_inspect_sym(type);
    err_write_1("\n");
  }
}

sw file_copy (const char *from, const char *to)
{
  char buf[4096];
  s32 e;
  t_fd fd_from = -1;
  t_fd fd_to = -1;
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
  if ((fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
                    0666)) < 0) {
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
  if (false) {
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
  *dest = ! lstat(path->ptr.pchar, &sb);
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

bool * file_is_directory (const s_str *path, bool *dest)
{
  s32 e;
  struct stat sb;
  assert(path);
  if (stat(path->ptr.pchar, &sb)) {
    e = errno;
    if (e == ENOENT) {
      *dest = false;
      return dest;
    }
    err_write_1("file_is_directory: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_puts(path->ptr.pchar);
    return NULL;
  }
  *dest = S_ISDIR(sb.st_mode) ? true : false;
  return dest;
}

bool file_is_directory_1 (const char *path)
{
  s_str str;
  bool tmp;
  str_init_1(&str, NULL, path);
  if (! file_is_directory(&str, &tmp)) {
    ERROR("file_is_directory_1: file_is_directory");
    abort();
  }
  return tmp;
}

bool * file_is_regular (const s_str *path, bool *dest)
{
  s32 e;
  struct stat sb;
  assert(path);
  if (lstat(path->ptr.pchar, &sb)) {
    e = errno;
    if (e == ENOENT) {
      *dest = false;
      return dest;
    }
    err_write_1("file_is_regular: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_puts(path->ptr.pchar);
    return NULL;
  }
  *dest = S_ISREG(sb.st_mode) ? true : false;
  return dest;
}

bool file_link (const s_str *from, const s_str *to)
{
#if ! (defined(WIN32) || defined(WIN64))
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
#else
  (void) from;
  (void) to;
  err_puts("file_link");
  assert(! "file_link");
  return false;
#endif
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
    tail = &(*tail)->next.data.plist;
  }
  closedir(dir);
  *dest = tmp;
  return dest;
}

s_list ** file_list_recursive (const s_str *path, s_list **dest)
{
  char b[PATH_MAX];
  u32  b_pos = 0;
  DIR           *dir;
  struct dirent *dirent;
  s32 e;
  s_list  *in;
  s_list **in_tail;
  sw len;
  s_list  *tmp;
  s_list **tmp_tail;
  tmp = NULL;
  tmp_tail = &tmp;
  if (! (in = list_new_str(NULL, path->size, path->ptr.p, NULL)))
    return NULL;
  in_tail = &in->next.data.plist;
  while (in) {
    if (in->tag.type != TAG_STR) {
      ERROR("not an Str");
      return NULL;
    }
    if (in->tag.data.str.size >= PATH_MAX - 2) {
      ERROR("path is longer than PATH_MAX 1");
      return NULL;
    }
    memcpy(b, in->tag.data.str.ptr.pchar, in->tag.data.str.size);
    b_pos = in->tag.data.str.size;
    b[b_pos++] = '/';
    b[b_pos] = 0;
    dir = opendir(in->tag.data.str.ptr.pchar);
    if (! dir) {
      e = errno;
      err_write_1("file_list_recursive: opendir: ");
      err_write_1(strerror(e));
      err_write_1(": ");
      err_inspect_str(path);
      goto ko;
    }
    while ((dirent = readdir(dir))) {
      if (dirent->d_name[0] == '.')
        continue;
      len = strlen(dirent->d_name);
      if (b_pos + len >= PATH_MAX) {
        ERROR("path is longer than PATH_MAX 2");
        goto ko;
      }
      memcpy(b + b_pos, dirent->d_name, len);
      b[b_pos + len] = 0;
      if (file_is_directory_1(b)) {
        if (! (*in_tail = list_new_str_1_alloc(b, NULL)))
          goto ko;
        in_tail = &(*in_tail)->next.data.plist;
      }
      else {
        if (! (*tmp_tail = list_new_str_1_alloc(b, NULL)))
          goto ko;
        tmp_tail = &(*tmp_tail)->next.data.plist;
      }
    }
    closedir(dir);
    dir = NULL;
    in = list_delete(in);
  }
  *dest = tmp;
  return dest;
 ko:
  list_delete_all(tmp);
  list_delete_all(in);
  closedir(dir);
  return NULL;
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

FILE * file_open (const s_str *path, const char *mode)
{
  s32 e;
  FILE *fp;
  fp = fopen(path->ptr.pchar, mode);
  if (! fp) {
    e = errno;
    err_write_1("file_open: ");
    err_write_1(strerror(e));
    err_write_1(": ");
    err_inspect_str(path);
    err_write_1("\n");
    assert(! "facts_open_file: fopen");
    return NULL;
  }
  return fp;
}

t_fd * file_open_r (const s_str *path, t_fd *dest)
{
  sw e;
  t_fd fd;
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

t_fd * file_open_w (const s_str *path, t_fd *dest)
{
  sw e;
  t_fd fd;
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
  t_fd fd;
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
  t_fd fd;
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

s_str * file_read_slice (s_str *path, u64 start, u64 end, s_str *dest)
{
  t_fd fd;
  char *p = NULL;
  s64 r;
  s64 remaining;
  s64 size;
  s_str tmp = {0};
  if (! file_open_r(path, &fd))
    return NULL;
  if ((r = lseek(fd, start, SEEK_SET)) < 0 ||
      (u64) r != start) {
    err_puts("file_read_slice: lseek 1");
    assert(! "file_read_slice: lseek 1");
    close(fd);
    return NULL;
  }
  if ((r = lseek(fd, 0, SEEK_END)) < 0) {
    err_puts("file_read_slice: lseek 2");
    assert(! "file_read_slice: lseek 2");
    close(fd);
    return NULL;
  }
  if ((u64) r < end)
    end = r;
  if ((r = lseek(fd, start, SEEK_SET)) < 0 ||
      (u64) r != start) {
    err_puts("file_read_slice: lseek 3");
    assert(! "file_read_slice: lseek 3");
    close(fd);
    return NULL;
  }
  if (! (size = end - start)) {
    close(fd);
    return str_init_empty(dest);
  }
  if (! (p = alloc(size))) {
    close(fd);
    return NULL;
  }
  str_init(&tmp, p, size, p);
  remaining = size;
  while (remaining > 0) {
    if ((r = read(fd, p, remaining)) <= 0) {
      err_puts("file_read_slice: read");
      assert(! "file_read_slice: read");
      free(p);
      close(fd);
      return NULL;
    }
    remaining -= r;
    p += r;
  }
  if (remaining < 0) {
    err_puts("file_read_slice: buffer overflow");
    assert(! "file_read_slice: buffer overflow");
    abort();
  }
  *dest = tmp;
  return dest;
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
  if (suffix && suffix[0] == '/') {
    if (file_access(suffix, mode))
      return str_init_copy(dest, suffix);
    return NULL;
  }      
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

// dest should be 20 bytes long
u8 * file_sha1 (const s_str *path, u8 *dest)
{
  u8 buf[BUF_SIZE];
  sw e;
  t_fd fd = -1;
  sw r;
  SHA1_CTX sha1;
  if (! file_open_r(path, &fd))
    return NULL;
  SHA1Init(&sha1);
  while (1) {
    if ((r = read(fd, buf, sizeof(buf))) < 0) {
      e = errno;
      err_write_1("file_sha1: read: ");
      err_write_1(strerror(e));
      err_write_1("\n");
      assert(! "file_sha1: read");
      return NULL;
    }
    if (! r)
      break;
    SHA1Update(&sha1, buf, r);
  }
  close(fd);
  SHA1Final(dest, &sha1);
  return dest;
}

s_str * file_sha1_base64url (const s_str *path, s_str *dest)
{
  u8 sha1[20];
  if (! file_sha1(path, sha1))
    return NULL;
  return str_init_base64url(dest, sha1, 20);
}

// dest should be 64 bytes long
u8 * file_sha512 (const s_str *path, u8 *dest)
{
  u8 buf[BUF_SIZE];
  sw e;
  t_fd fd = -1;
  sw r;
  s_sha512 sha512 = {0};
  if (! file_open_r(path, &fd))
    return NULL;
  sha512_init(&sha512);
  while (1) {
    if ((r = read(fd, buf, sizeof(buf))) < 0) {
      e = errno;
      err_write_1("file_sha512: read: ");
      err_write_1(strerror(e));
      err_write_1("\n");
      assert(! "file_sha512: read");
      return NULL;
    }
    if (! r)
      break;
    sha512_update(&sha512, buf, r);
  }
  close(fd);
  sha512_sum(&sha512, dest);
  return dest;
}

s_str * file_sha512_base64url (const s_str *path, s_str *dest)
{
  u8 sha512[64];
  if (! file_sha512(path, sha512))
    return NULL;
  return str_init_base64url(dest, sha512, 64);
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
    tmp.st_mode = list_new_psym(&g_sym_file, tmp.st_mode);
  else if (sb->st_mode & S_IFDIR)
    tmp.st_mode = list_new_psym(&g_sym_directory, tmp.st_mode);
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
  tag_init_psym(&tag_sym_file, &g_sym_file);
  if (! plist_has((const s_list * const *) &file_stat->st_mode,
                 &tag_sym_file, &b)) {
    err_puts("file_stat_to_struct_stat: plist_has(:file)");
    err_puts("file_stat_to_struct_stat: plist_has(:file)");
  }
  if (b)
    tmp.st_mode |= S_IFREG;
  else {
    tag_init_psym(&tag_sym_directory, &g_sym_directory);
    if (! plist_has((const s_list * const *) &file_stat->st_mode,
                   &tag_sym_directory, &b)) {
      err_puts("file_stat_to_struct_stat: plist_has(:directory)");
      err_puts("file_stat_to_struct_stat: plist_has(:directory)");
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

bool file_write (const s_str *path, const s_str *data)
{
  s32 e;
  t_fd fd;
  static const mode_t mode = 0666;
  u32 pos = 0;
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

sw file_write_str (const s_str *path, const s_str *str)
{
  t_fd fd;
  sw r;
  u32 result = 0;
  if (! file_open_w(path, &fd))
    return -1;
  while (result < str->size) {
    if ((r = write(fd, str->ptr.pchar + result,
                   str->size - result)) <= 0)
      goto clean;
    result += r;
  }
  if (result != str->size) {
    ERROR("buffer overflow");
    abort();
  }
  r = result;
 clean:
  close(fd);
  return r;
}
