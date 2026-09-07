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
#ifndef KC3_TO_C_H
#define KC3_TO_C_H

#include "../libkc3/types.h"

typedef struct to_c s_to_c;

struct to_c {
  s_buf c_buf;
  FILE *c_fp;
  s_str c_path;
  s_buf h_buf;
  FILE *h_fp;
  s_str h_ifndef;
  s_str h_path;
  s_str name;
};

/* Stack-allocation compatible functions. Call to_c_clean after use. */
void     to_c_clean (s_to_c *context);
s_to_c * to_c_init (s_to_c *context);

/* Observers. */
s_str * to_c_defmodule_sym_to_str (const s_sym *sym, s_str *dest);

/* Operators. */
sw to_c_buf (s_to_c *context, s_buf *in);
sw to_c_defmodule_do_block (s_to_c *context, const s_do_block *do_block);
sw to_c_defmodule_tag (s_to_c *context, s_tag *tag);
sw to_c_tag (s_to_c *context, s_tag *tag);
sw to_c_tag_call (s_to_c *context, s_call *call);
sw to_c_tag_call_defmodule (s_to_c *context, const s_sym *sym,
                            const s_do_block *do_block);
sw to_c_file (s_to_c *context, s_str *in);

/* Output configuration. Call to_c_close after use. */
s_to_c * to_c_close (s_to_c *context);
s_to_c * to_c_open (s_to_c *context, const s_str *path);

#endif /* KC3_TO_C_H */
