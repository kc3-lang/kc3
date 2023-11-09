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
#include "file.h"
#include "timespec.h"

int file_copy (const char *from, const char *to)
{
  char buf[4096];
  int fd_from = -1;
  int fd_to = -1;
  ssize_t r;
  int saved_errno;

  if ((fd_from = open(from, O_RDONLY)) < 0) {
    warn("cp: %s", from);
    return -1;
  }
  if ((fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC, 0666)) < 0) {
    warn("cp: %s", to);
    goto error;
  }
  while ((r = read(fd_from, buf, sizeof buf)) > 0) {
    char *out = buf;
    ssize_t w;
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

s_tag * file_mtime (const s_str *path, s_tag *dest)
{
  struct stat sb;
  assert(path);
  assert(dest);
  if (stat(path->ptr.ps8, &sb)) {
    warn("file_mtime: %s", path->ptr.ps8);
    return NULL;
  }
  return timespec_to_tag(&sb.st_mtim, dest);
}
