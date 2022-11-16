/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include "c3.h"
#include "sym.h"

void c3_clean (s_module *c3)
{
  /* TODO */
  (void) c3;
}

s_module * c3_init (s_module *c3, s_facts *facts)
{
  assert(c3);
  assert(facts);
  c3->name = sym_1("C3");
  c3->facts = facts;
  /* TODO */
  return c3;
}

void libc3_init ()
{
#ifdef DEBUG
  buf_init_alloc(&g_debug_buf, 1024);
  buf_file_open_w(&g_debug_buf, stderr);
#endif
}

void libc3_shutdown ()
{
  sym_delete_all();
#ifdef DEBUG
  buf_file_close(&g_debug_buf);
  buf_clean(&g_debug_buf);
#endif
}
