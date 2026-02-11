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
#include "alloc.h"
#include "assert.h"
#include "compare.h"
#include "counter.h"
#include "ht.h"
#include "ident.h"
#include "io.h"
#include "marshall.h"
#include "marshall_read.h"
#include "mutex.h"
#include "sym.h"
#include "tag.h"

void counter_clean (s_counter *counter)
{
  assert(counter);
  tag_clean(&counter->count);
#if HAVE_PTHREAD
  if (counter->mutex)
    mutex_delete(counter->mutex);
#endif
}

s_tag * counter_decrease (s_counter *counter, s_tag *positive,
                          s_tag *dest)
{
  s_tag tmp = {0};
  assert(counter);
  assert(positive);
  assert(dest);
#if HAVE_PTHREAD
  mutex_lock(counter->mutex);
#endif
  if (! tag_is_positive_integer(positive)) {
    err_puts("counter_decrease: tag_is_positive_integer");
    assert(! "counter_decrease: tag_is_positive_integer");
    goto clean;
  }
  if (! tag_sub(&counter->count, positive, &tmp)) {
    err_puts("counter_decrease: tag_sub");
    assert(! "counter_decrease: tag_sub");
    tag_clean(&tmp);
    goto clean;
  }
  tag_clean(&counter->count);
  if (! tag_init_copy(&counter->count, &tmp)) {
    tag_clean(&tmp);
    goto clean;
  }
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  *dest = tmp;
  return dest;
 clean:
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  return NULL;
}

void counter_delete (s_counter *counter)
{
  assert(counter);
  counter_clean(counter);
  free(counter);
}

s_tag * counter_get (s_counter *counter, s_tag *dest)
{
  s_tag *result;
  assert(counter);
  assert(dest);
#if HAVE_PTHREAD
  mutex_lock(counter->mutex);
#endif
  result = tag_init_copy(dest, &counter->count);
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  return result;
}

s_tag * counter_increase (s_counter *counter, s_tag *positive,
                          s_tag *dest)
{
  s_tag tmp = {0};
  assert(counter);
  assert(positive);
  assert(dest);
#if HAVE_PTHREAD
  mutex_lock(counter->mutex);
#endif
  if (! tag_is_positive_integer(positive)) {
    err_write_1("counter_increase: tag_is_positive_integer: ");
    err_inspect_tag(positive);
    err_write_1("\n");
    assert(! "counter_increase: tag_is_positive_integer");
    goto clean;
  }
  if (! tag_add(&counter->count, positive, &tmp)) {
    err_puts("counter_increase: tag_sub");
    assert(! "counter_increase: tag_sub");
    tag_clean(&tmp);
    goto clean;
  }
  tag_clean(&counter->count);
  if (! tag_init_copy(&counter->count, &tmp)) {
    tag_clean(&tmp);
    goto clean;
  }
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  *dest = tmp;
  return dest;
 clean:
#if HAVE_PTHREAD
  mutex_unlock(counter->mutex);
#endif
  return NULL;
}

s_counter * counter_init (s_counter *counter, s_tag *value)
{
  assert(counter);
  assert(value);
  if (! tag_is_integer(value)) {
    err_puts("counter_init: value is not an integer");
    assert(! "counter_init: value is not an integer");
    return NULL;
  }
  *counter = (s_counter) {0};
  if (! tag_init_copy(&counter->count, value))
    return NULL;
#if HAVE_PTHREAD
  if (! (counter->mutex = mutex_new())) {
    tag_clean(&counter->count);
    return NULL;
  }
#endif
  return counter;
}

s_marshall ** counter_marshall (s_marshall **m, bool heap,
                                s_counter **counter)
{
  assert(m);
  assert(*m);
  assert(counter);
  assert(*counter);
  if (! tag_is_integer(&(*counter)->count)) {
    err_puts("counter_marshall: count is not an integer");
    assert(! "counter_marshall: count is not an integer");
    return NULL;
  }
  if (! marshall_tag(*m, heap, &(*counter)->count))
    return NULL;
  return m;
}

s_counter ** counter_marshall_read (s_marshall_read **mr, bool heap,
                                    s_counter **dest)
{
  s_tag count = {0};
  s_counter *tmp = NULL;
  assert(mr);
  assert(*mr);
  assert(dest);
  if (! marshall_read_tag(*mr, heap, &count))
    return NULL;
  if (! (tmp = counter_new(&count))) {
    tag_clean(&count);
    return NULL;
  }
  tag_clean(&count);
  *dest = tmp;
  return dest;
}

s_counter * counter_new (s_tag *value)
{
  s_counter *counter = NULL;
  assert(value);
  if (! (counter = alloc(sizeof(s_counter))))
    return NULL;
  if (! counter_init(counter, value)) {
    free(counter);
    return NULL;
  }
  return counter;
}
