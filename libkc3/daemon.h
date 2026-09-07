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
#ifndef LIBKC3_DAEMON_H
#define LIBKC3_DAEMON_H

#include "types.h"

void daemon_stop (void);
void daemon_stop_clean (void);
s64  daemon_stop_fd (void);
bool daemon_stop_init (void);
void daemon_stop_notify (s32 signal);
bool daemon_stopping (void);

#endif /* LIBKC3_DAEMON_H */
