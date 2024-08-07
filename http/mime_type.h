/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#ifndef HTTP_MIME_TYPE_H
#define HTTP_MIME_TYPE_H

#include "types.h"

bool http_mime_type_buf_parse (s_buf *buf);
bool http_mime_type_buf_parse_type (s_buf *buf);
bool http_mime_type_def (const s_tag *ext,
                         const s_sym * const *mime_type);
bool http_mime_type_load (s_str *path);

#endif /* HTTP_MIME_TYPE_H */
