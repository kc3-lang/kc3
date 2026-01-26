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
/**
 * @file mutex.h
 * @brief Mutual exclusion synchronization primitive.
 *
 * Structure to acquire a mutually exclusive lock.
 */
#ifndef LIBKC3_MUTEX_H
#define LIBKC3_MUTEX_H

#include "types.h"

#if HAVE_PTHREAD

/* Stack-allocation compatible functions, call mutex_clean after use. */
void      mutex_clean (s_mutex *mutex);
s_mutex * mutex_init (s_mutex *mutex);

/* Heap-allocation functions, call mutex_delete after use. */
void      mutex_delete (s_mutex *mutex);
s_mutex * mutex_new (void);

/* Operators. */
void mutex_lock (s_mutex *mutex);
void mutex_unlock (s_mutex *mutex);

#endif /* HAVE_PTHREAD */

#endif /* LIBKC3_MUTEX_H */
