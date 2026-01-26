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
#ifndef URL_H
#define URL_H

#include "../libkc3/types.h"

s_str * url_escape (const s_str *src, s_str *dest);
sw      url_escape_size (const s_str *src);
s_str * url_unescape (const s_str *url, s_str *dest);
sw      url_unescape_size (const s_str *url);
s_tag * url_www_form_decode (const s_str *src, s_tag *dest);

#endif /* URL_H */
