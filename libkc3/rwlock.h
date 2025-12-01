/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_RWLOCK_H
#define LIBKC3_RWLOCK_H

#include "types.h"

#if HAVE_PTHREAD

/* Stack-allocation compatible functions, call rwlock_clean
   after use. */
void       rwlock_clean (s_rwlock *rwlock);
s_rwlock * rwlock_init (s_rwlock *rwlock);

/* Heap allocation functions, call rwlock_delete after use. */
void       rwlock_delete (s_rwlock *rwlock);
s_rwlock * rwlock_new (void);

/* Operators. */
s_rwlock * rwlock_r (s_rwlock *rwlock);
s_rwlock * rwlock_unlock_r (s_rwlock *rwlock);
s_rwlock * rwlock_unlock_w (s_rwlock *rwlock);
s_rwlock * rwlock_w (s_rwlock *rwlock);

#endif /* HAVE_PTHREAD */

#endif /* LIBKC3_RWLOCK_H */
