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
#include <errno.h>
#include <pthread.h>
#include "alloc.h"
#include "assert.h"
#include "io.h"
#include "rwlock.h"

#if HAVE_PTHREAD

void rwlock_clean (s_rwlock *rwlock)
{
  int e;
  assert(rwlock);
  if (rwlock->ready) {
    if ((e = pthread_rwlock_destroy(&rwlock->rwlock))) {
      err_write_1("rwlock_clean: pthread_rwlock_destroy: ");
      switch (e) {
      case EPERM: err_puts("permission denied"); break;
      case EBUSY: err_puts("rw lock is locked"); break;
      case EINVAL: err_puts("invalid rw lock"); break;
      default: err_puts("unknown error"); break;
      }
      err_stacktrace();
      assert(! "rwlock_clean: pthread_rwlock_destroy");
      abort();
    }
  }
}

s_rwlock * rwlock_init (s_rwlock *rwlock)
{
  assert(rwlock);
  *rwlock = (s_rwlock) {0};
  if (pthread_rwlock_init(&rwlock->rwlock, NULL)) {
    err_puts("rwlock_init: pthread_rwlock_init");
    assert(! "rwlock_init: pthread_rwlock_init");
    return NULL;
  }
  rwlock->ready = true;
  rwlock->self = rwlock;
  return rwlock;
}

void rwlock_delete (s_rwlock *rwlock)
{
  assert(rwlock);
  rwlock_clean(rwlock);
  alloc_free(rwlock);
}

s_rwlock * rwlock_new (void)
{
  s_rwlock *rwlock;
  if (! (rwlock = alloc(sizeof(s_rwlock))))
    return NULL;
  if (! rwlock_init(rwlock)) {
    alloc_free(rwlock);
    return NULL;
  }
  return rwlock;
}

s_rwlock * rwlock_r (s_rwlock *rwlock)
{
  sw e;
  uw i;
  pthread_t thread;
  assert(rwlock);
  assert(rwlock->ready);
  thread = pthread_self();
  if (! pthread_equal(rwlock->thread, thread)) {
    if (false) {
      i = (uw) rwlock;
      err_write_1("rwlock_r: ");
      err_inspect_uw_hexadecimal(i);
      err_write_1("\n");
    }
    if ((e = pthread_rwlock_rdlock(&rwlock->rwlock))) {
      err_write_1("rwlock_r: pthread_rwlock_rdlock: ");
      switch (e) {
      case EAGAIN:
        err_puts("the maximum number of read locks on this lock has"
                 " been exceeded");
        break;
      case EDEADLK:
        err_puts("the current thread already owns this lock for"
                 " writing");
        break;
      case EINVAL:
        err_puts("invalid lock");
        break;
      case ENOMEM:
        err_puts("not enough memory to initialize the lock");
        break;
      default:
        err_puts("unknown error");
        break;
      }
      assert(! "rwlock_r: pthread_rwlock_rdlock");
      abort();
      return NULL;
    }
  }
  return rwlock;
}

s_rwlock * rwlock_unlock_r (s_rwlock *rwlock)
{
  pthread_t thread;
  assert(rwlock);
  assert(rwlock->ready);
  thread = pthread_self();
  if (rwlock->thread != thread &&
      pthread_rwlock_unlock(&rwlock->rwlock)) {
    err_puts("rwlock_unlock_r: pthread_rwlock_unlock");
    assert(! "rwlock_unlock_r: pthread_rwlock_unlock");
    abort();
    return NULL;
  }
  return rwlock;
}

s_rwlock * rwlock_unlock_w (s_rwlock *rwlock)
{
  assert(rwlock);
  assert(rwlock->ready);
  assert(rwlock->count);
  rwlock->count--;
  if (! rwlock->count) {
    rwlock->thread = 0;
    if (rwlock != rwlock->self) {
      printf("rwlock_unlock_w: self check\n");
      assert(! "rwlock_w: self check\n");
    }
    if (pthread_rwlock_unlock(&rwlock->rwlock)) {
      err_puts("rwlock_unlock_w: pthread_rwlock_unlock");
      assert(! "rwlock_unlock_w: pthread_rwlock_unlock");
      return NULL;
    }
  }
  return rwlock;
}

s_rwlock * rwlock_w (s_rwlock *rwlock)
{
  pthread_t thread;
  assert(rwlock);
  if (! rwlock->ready || ! rwlock->self) {
    printf("rwlock_w: not initialized\n");
    assert(! "rwlock_w: not initialized\n");
  }
  thread = pthread_self();
  if (! pthread_equal(rwlock->thread, thread)) {
    if (rwlock != rwlock->self) {
      printf("rwlock_w: self check\n");
      assert(! "rwlock_w: self check\n");
    }
    if (pthread_rwlock_wrlock(&rwlock->rwlock)) {
      printf("rwlock_w: pthread_rwlock_wrlock\n");
      assert(! "rwlock_w: pthread_rwlock_wrlock");
      return NULL;
    }
    rwlock->thread = thread;
    rwlock->count = 0;
  }
  rwlock->count++;
  return rwlock;
}

#endif /* HAVE_PTHREAD */
