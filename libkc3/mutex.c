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
#include <errno.h>
#include "alloc.h"
#include "assert.h"
#include "mutex.h"

void mutex_clean (s_mutex *mutex)
{
  assert(mutex);
  if (mutex->ready &&
      pthread_mutex_destroy(&mutex->mutex)) {
    err_puts("mutex_clean: pthread_mutex_destroy");
    assert(! "mutex_clean: pthread_mutex_destroy");
    abort();
  }
}

void mutex_delete (s_mutex *mutex)
{
  assert(mutex);
  mutex_clean(mutex);
  free(mutex);
}

s_mutex * mutex_init (s_mutex *mutex)
{
  s_mutex tmp = {0};
  assert(mutex);
  if (pthread_mutex_init(&tmp.mutex, NULL)) {
    err_puts("mutex_init: pthread_mutex_init");
    assert(! "mutex_init: pthread_mutex_init");
    abort();
  }
  tmp.ready = true;
  *mutex = tmp;
  return mutex;
}

void mutex_lock (s_mutex *mutex)
{
  int e;
  if (! mutex || ! mutex->ready) {
    err_puts("mutex_lock: invalid argument");
    assert(! "mutex_lock: invalid argument");
    err_stacktrace();
    abort();
  }
  if ((e = pthread_mutex_lock(&mutex->mutex))) {
    err_write_1("mutex_lock: pthread_mutex_lock: ");
    switch (e) {
    case EINVAL: err_puts("invalid mutex"); break;
    case EAGAIN: err_puts("EAGAIN"); break;
    case EDEADLK: err_puts("EDEADLK"); break;
    default: err_inspect_s32_decimal(e);
    }
    err_stacktrace();
    assert(! "mutex_lock: pthread_mutex_lock");
    abort();
  }
}

s_marshall ** mutex_marshall (s_marshall **m, bool heap,
                              s_mutex **mutex)
{
  assert(m);
  assert(*m);
  assert(mutex);
  assert(*mutex);
  (void) heap;
  (void) mutex;
  return m;
}

s_mutex ** mutex_marshall_read (s_marshall_read **mr, bool heap,
                                s_mutex **dest)
{
  s_mutex *tmp;
  assert(mr);
  assert(*mr);
  assert(dest);
  (void) mr;
  (void) heap;
  if (! (tmp = mutex_new()))
    return NULL;
  *dest = tmp;
  return dest;
}

s_mutex * mutex_new (void)
{
  s_mutex *mutex;
  if (! (mutex = alloc(sizeof(s_mutex))))
    return NULL;
  if (! mutex_init(mutex)) {
    free(mutex);
    return NULL;
  }
  return mutex;
}

void mutex_unlock (s_mutex *mutex)
{
  if (pthread_mutex_unlock(&mutex->mutex)) {
    err_puts("mutex_unlock: pthread_mutex_unlock");
    assert(! "mutex_unlock: pthread_mutex_unlock");
    abort();
  }
}
