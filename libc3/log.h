/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef LOG_H
#define LOG_H

#include "types.h"

/* stack-allocation compatible functions */
void log_init (s_log *log);
void log_clean (s_log *log);

/* constructor */
s_log * log_new ();

/* destructor */
void log_delete (s_log *log);

/* modifiers */
void log_close (s_log *log);
sw log_open (s_log *log, FILE *fp);

#endif /* C3_H */
