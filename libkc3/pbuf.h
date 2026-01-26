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
#ifndef LIBKC3_PBUF_H
#define LIBKC3_PBUF_H

#include <stdarg.h>
#include <stdlib.h>
#include "types.h"

#define BUF_SIZE 65536

/* Stack-allocation compatible functions, call pbuf_clean after use. */
void    pbuf_clean (p_buf *buf);
p_buf * pbuf_init (p_buf *buf, bool p_free, uw size, char *p);
p_buf * pbuf_init_const (p_buf *buf, uw size, const char *p);
p_buf * pbuf_init_1 (p_buf *buf, bool p_free, char *p);
p_buf * pbuf_init_1_const (p_buf *buf, const char *p);
p_buf * pbuf_init_alloc (p_buf *buf, s_tag *size);
p_buf * pbuf_init_popen (p_buf *buf, const s_str *cmd,
                         const s_str *mode);
p_buf * pbuf_init_str (p_buf *buf, bool free, s_str *p);
p_buf * pbuf_init_str_const (p_buf *buf, const s_str *p);
p_buf * pbuf_init_str_copy (p_buf *buf, const s_str *str);

#endif /* LIBKC3_PBUF_H */
