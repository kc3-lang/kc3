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
#ifndef KC3_GLIB_H
#define KC3_GLIB_H

#include "types.h"

extern bool g_kc3_g_main_stop;

void   kc3_g_main (void);
bool * kc3_g_main_context_iteration (bool *dest);
void   kc3_g_signal_connect (GObject **instance, const s_str *signal,
			     p_callable *callback);

#endif /* KC3_GLIB_H */
