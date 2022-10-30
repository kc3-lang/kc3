/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include "buf.h"
#include "buf_parse_c.h"
#include "buf_save.h"

sw buf_parse_c_ident (s_buf *buf, const s_sym **dest)
{
  (void) buf;
  (void) dest;
  return -1;
}

sw buf_parse_c_prototype (s_buf *buf, s_tuple *dest)
{
  (void) buf;
  (void) dest;
  return -1;
}

sw buf_parse_c_type (s_buf *buf, s_sym_list **dest)
{
  (void) buf;
  (void) dest;
  return -1;
}

sw buf_parse_c_typedef (s_buf *buf, s_c_typedef *dest)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(dest);
  buf_save_init(buf, &save);
  if ((r = buf_read_1(buf, "typedef")) < 0)
    goto clean;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_c_type(buf, &dest->type)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_parse_c_ident(buf, &dest->ident)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_ignore_spaces(buf)) < 0)
    goto restore;
  result += r;
  r = result;
  goto clean;
 restore:
  buf_save_restore_rpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}
