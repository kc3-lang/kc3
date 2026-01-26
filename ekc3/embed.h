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
#ifndef EKC3_EMBED_H
#define EKC3_EMBED_H

#include "types.h"

typedef enum {
  EMBED_STATE_VOID = 0,
  EMBED_STATE_RAW,
  EMBED_STATE_SILENT,
  EMBED_STATE_VERBOSE,
  EMBED_STATE_VERBOSE_RAW
} e_embed_state;

s_tag * embed_parse_template (s_buf *input, s_tag *dest);
s_tag * embed_parse_template_1 (const char *input, s_tag *dest);
s_tag * embed_parse_template_file (const s_str *path, s_tag *dest);
s_tag * embed_parse_template_file_or_abort (const s_str *path,
                                            s_tag *dest);
s_tag * embed_parse_template_str (const s_str *input, s_tag *dest);

#endif /* EKC3_EMBED_H */
