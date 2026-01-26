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
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "types.h"

s_tag *          http_request_buf_parse (s_tag *req, s_buf *buf);
s_tag *          http_request_buf_parse_method (s_buf *buf,
                                                s_tag *dest);
s_http_request * http_request_cookie_add (s_http_request *req,
                                          s_str *cookies);
s_tag *          http_request_method_from_str (const s_str *str,
                                               s_tag *dest);

#endif /* HTTP_REQUEST_H */
