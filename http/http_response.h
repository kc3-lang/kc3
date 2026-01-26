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
#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "types.h"

/* Stack-allocation compatible functions, call http_response_clean
   after use. */
void              http_response_clean (s_http_response *res);
s_http_response * http_response_init_copy (s_http_response *res,
                                           s_http_response *src);

/* Observers. */
sw                http_response_buf_write (const s_http_response *res,
                                           s_buf *buf, bool send_body);
s_tag *           http_response_find_header (const s_http_response *res,
                                             const s_str *key);

/* Operators. */
s_http_response * http_response_set_header (s_http_response *res,
                                            const s_str *key,
                                            const s_str *value,
                                            s_http_response *dest);

#endif /* HTTP_RESPONSE_H */
