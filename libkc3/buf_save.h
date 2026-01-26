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
#ifndef LIBKC3_BUF_SAVE_H
#define LIBKC3_BUF_SAVE_H

#include "types.h"

s_buf * buf_save_clean (s_buf *buf, s_buf_save *save);
s_buf * buf_save_init (s_buf *buf, s_buf_save *save);
uw      buf_save_min_rpos (const s_buf *buf);
uw      buf_save_min_wpos (const s_buf *buf);
s_buf * buf_save_restore_rpos (s_buf *buf, const s_buf_save *save);
s_buf * buf_save_restore_wpos (s_buf *buf, const s_buf_save *save);
s_buf * buf_save_update (s_buf *buf, s_buf_save *save);

#endif /* LIBKC3_BUF_SAVE */
