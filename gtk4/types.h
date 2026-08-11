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
#ifndef KC3_GTK4_TYPES_H
#define KC3_GTK4_TYPES_H

#if defined(__clang__)
# pragma clang diagnostic push
# pragma clang diagnostic ignored "-Wc2y-extensions"
# pragma clang diagnostic ignored "-Wextra-semi"
#endif

#include <gtk/gtk.h>
#include <gsk/gsk.h>

#if defined(__clang__)
# pragma clang diagnostic pop
#endif
#include "../libkc3/types.h"

typedef struct action_callback s_action_callback;

/* 1 */

struct action_callback {
  GSimpleAction *action;
  s_callable *callable;
  s_tag tag;
  s_action_callback *next;
};

#endif /* KC3_GTK4_TYPES_H */
