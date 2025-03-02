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
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#ifdef WIN32
# include <winsock2.h>
#endif

#include "assert.h"
#include "buf.h"
#include "fd.h"
#include "io.h"
#include "list.h"
#include "str.h"

s_str * fd_read_until_eof (s32 fd, s_str *dest)
{
  s_buf buf;
  sw e;
  s_list **l;
  s_list  *list;
  sw r;
  s_str tmp;
  if (! buf_init_alloc(&buf, BUF_SIZE))
    return NULL;
  list = NULL;
  l = &list;
  while (1) {
    if ((r = read(fd, buf.ptr.pchar, buf.size)) < 0) {
      e = errno;
      err_write_1("fd_read_until_eof: read: ");
      err_puts(strerror(e));
      assert(! "kc3_system: read");
      goto clean;
    }
    if (! r)
      break;
    buf.rpos = 0;
    buf.wpos = r;
    *l = list_new(NULL);
    if (! buf_read_to_str(&buf, &(*l)->tag.data.str))
      goto clean;
    (*l)->tag.type = TAG_STR;
    l = &(*l)->next.data.list;
  }
  if (! str_init_concatenate_list(&tmp, (const s_list * const *) &list))
    goto clean;
  list_delete_all(list);
  buf_clean(&buf);
  *dest = tmp;
  return dest;
 clean:
  list_delete_all(list);
  buf_clean(&buf);
  return NULL;
}

bool * fd_set_blocking (s32 fd, bool blocking, bool *result)
{
#ifdef WIN32
  unsigned long b;
  b = blocking;
  if (ioctlsocket(fd, FIONBIO, &b) != NO_ERROR) {
    err_puts("fd_set_blocking: ioctlsocket");
    assert(! "fd_set_blocking: ioctlsocket");
    return NULL;
  }
#else
  s32 flags;
  flags = fcntl(fd, F_GETFL);
  if (flags == -1)
    return NULL;
  if (! blocking)
    flags |= O_NONBLOCK;
  else
    flags &= ~ (s32) O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) == -1)
    return NULL;
#endif
  *result = true;
  return result;
}
