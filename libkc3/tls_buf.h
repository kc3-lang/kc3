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

#ifndef  LIBKC3_TLS_BUF_H
#define  LIBKC3_TLS_BUF_H

#include "types.h"

/* TLS buf open functions, call tls_buf_close after use. */
void    tls_buf_close(s_buf *buf);
s_buf * tls_buf_open_r(s_buf *buf, p_tls ctx);
s_buf * tls_buf_open_w(s_buf *buf, p_tls ctx);

#endif // ! LIBKC3_TLS_BUF_H
