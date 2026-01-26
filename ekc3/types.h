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
#ifndef EKC3_TYPES_H
#define EKC3_TYPES_H

#include "../libkc3/types.h"

typedef enum {
  EKC3_STATE_RAW = 0,
  EKC3_STATE_VERBOSE,
  EKC3_STATE_SILENT
} e_ekc3_state;

typedef s_list *p_ekc3;

#endif /* EKC3_TYPES_H */
