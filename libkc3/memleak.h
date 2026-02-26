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
#ifndef LIBKC3_MEMLEAK_H
#define LIBKC3_MEMLEAK_H

#include "types.h"

extern s_memleak *g_memleak;
extern bool       g_memleak_enabled;

void memleak_add (void *ptr, uw size, s_list *stacktrace);
void memleak_remove_all (void);
void memleak_remove (void *ptr);
void memleak_report (void);

#endif /* LIBKC3_MEMLEAK_H */
