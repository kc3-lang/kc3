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
#ifndef KC3_JSON_H
#define KC3_JSON_H

#include "../libkc3/types.h"

sw      json_buf_inspect (s_buf *buf, const s_tag *tag);
sw      json_buf_inspect_bool (s_buf *buf, bool b);
sw      json_buf_inspect_bool_size (s_pretty *pretty, bool b);
sw      json_buf_inspect_map (s_buf *buf, const s_map *map);
sw      json_buf_inspect_map_size (s_pretty *pretty, const s_map *map);
sw      json_buf_inspect_size (s_pretty *pretty, const s_tag *tag);
sw      json_buf_inspect_tag_number (s_buf *buf, const s_tag *tag);
sw      json_buf_inspect_tag_number_size (s_pretty *pretty,
                                          const s_tag *tag);
sw      json_buf_inspect_void (s_buf *buf);
sw      json_buf_inspect_void_size (s_pretty *pretty);
s_tag * json_buf_parse (s_buf *buf, s_tag *dest);
s_tag * json_buf_parse_bool (s_buf *buf, s_tag *dest);
s_tag * json_buf_parse_list (s_buf *buf, s_tag *dest);
s_tag * json_buf_parse_map (s_buf *buf, s_tag *dest);
s_tag * json_buf_parse_null (s_buf *buf, s_tag *dest);
s_tag * json_buf_parse_number (s_buf *buf, s_tag *dest);
s_tag * json_buf_parse_str (s_buf *buf, s_tag *dest);
s_tag * json_from_str (const s_str *src, s_tag *dest);

#endif /* KC3_JSON_H */
