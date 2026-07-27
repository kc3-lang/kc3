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
#include "assert.h"
#include "buf.h"
#include "buf_file.h"
#include "sym.h"
#include "uw.h"
#include "pbuf.h"

void pbuf_clean (p_buf *pbuf)
{
  FILE *fp;
  assert(pbuf);
  if ((*pbuf)->user_ptr) {
    fp = buf_file_fp(*pbuf);
    buf_file_close(*pbuf);
    pclose(fp);
  }
  buf_delete(*pbuf);
}

p_buf * pbuf_init_alloc (p_buf *pbuf, s_tag *size_tag)
{
  uw size;
  const s_sym *sym_Uw = &g_sym_Uw;
  p_buf tmp = NULL;
  if (! uw_init_cast(&size, &sym_Uw, size_tag))
    return NULL;
  if (! (tmp = buf_new_alloc(size)))
    return NULL;
  *pbuf = tmp;
  return pbuf;
}

p_buf * pbuf_init_popen (p_buf *buf, const s_str *cmd,
                         const s_str *mode)
{
  p_buf tmp = NULL;
  if (! (tmp = buf_new_popen(cmd, mode)))
    return NULL;
  *buf = tmp;
  return buf;
}
