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
#include <sched.h>
#include "alloc.h"
#include "assert.h"
#include "stacktrace.h"

void stacktrace_clean (s_stacktrace *stacktrace)
{
  assert(stacktrace);
  assert(! stacktrace_get(stacktrace));
  while (__atomic_load_n(&stacktrace->readers, __ATOMIC_SEQ_CST))
    sched_yield();
}

void stacktrace_delete (s_stacktrace *stacktrace)
{
  assert(stacktrace);
  stacktrace_clean(stacktrace);
  alloc_free(stacktrace);
}

s_stacktrace * stacktrace_init (s_stacktrace *stacktrace)
{
  assert(stacktrace);
  *stacktrace = (s_stacktrace) {0};
  return stacktrace;
}

s_stacktrace * stacktrace_new (void)
{
  s_stacktrace *stacktrace;
  if (! (stacktrace = alloc(sizeof(s_stacktrace))))
    return NULL;
  return stacktrace_init(stacktrace);
}

p_list stacktrace_read_begin (s_stacktrace *stacktrace)
{
  assert(stacktrace);
  /* Sequential consistency orders this pin before the head load. */
  __atomic_add_fetch(&stacktrace->readers, 1, __ATOMIC_SEQ_CST);
  return stacktrace_get(stacktrace);
}

void stacktrace_read_end (s_stacktrace *stacktrace)
{
  assert(stacktrace);
  assert(__atomic_load_n(&stacktrace->readers, __ATOMIC_SEQ_CST));
  __atomic_sub_fetch(&stacktrace->readers, 1, __ATOMIC_SEQ_CST);
}

void stacktrace_wait (s_stacktrace *stacktrace)
{
  assert(stacktrace);
  while (__atomic_load_n(&stacktrace->readers, __ATOMIC_SEQ_CST))
    sched_yield();
}
