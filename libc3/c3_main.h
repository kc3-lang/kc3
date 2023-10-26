/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#ifndef C3_MAIN_H
#define C3_MAIN_H

#include "types.h"

#define C3_EXT ".c3"

extern const s_str g_c3_base_binary;
extern const s_str g_c3_base_octal;
extern const s_str g_c3_base_decimal;
extern const s_str g_c3_base_hexadecimal;
extern const s_str g_c3_bases_hexadecimal[2];
extern const s8 *g_c3_license;

/* stack-allocation compatible functions */
void c3_init (s_env *env);
void c3_clean (s_env *env);

/* debug */
void c3_break ();

#endif /* C3_MAIN_H */
