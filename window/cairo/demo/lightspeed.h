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
#ifndef LIGHTSPEED_H
#define LIGHTSPEED_H

#include "../types.h"

#define LIGHTSPEED_STARS 1000

u8 lightspeed_load (s_sequence *seq);
u8 lightspeed_render (s_sequence *seq);
u8 lightspeed_unload (s_sequence *seq);

#endif /* LIGHTSPEED_H */
