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
#ifndef LIBKC3_STACKTRACE_H
#define LIBKC3_STACKTRACE_H

#include "types.h"
#include "assert.h"
#include "inline.h"

/* Stack-allocation compatible functions. */
void           stacktrace_clean (s_stacktrace *stacktrace);
s_stacktrace * stacktrace_init (s_stacktrace *stacktrace);

/* Heap-allocation functions. */
void           stacktrace_delete (s_stacktrace *stacktrace);
s_stacktrace * stacktrace_new (void);

/* Observers. */
p_list stacktrace_read_begin (s_stacktrace *stacktrace);
void   stacktrace_read_end (s_stacktrace *stacktrace);

/* Operators. */
void stacktrace_wait (s_stacktrace *stacktrace);

INLINE p_list stacktrace_get (const s_stacktrace *stacktrace)
{
  assert(stacktrace);
  return __atomic_load_n(&stacktrace->list, __ATOMIC_SEQ_CST);
}

INLINE void stacktrace_pop (s_stacktrace *stacktrace, p_list list)
{
  assert(stacktrace);
  /* After this store, new readers cannot acquire the removed head. */
  __atomic_store_n(&stacktrace->list, list, __ATOMIC_SEQ_CST);
  /* Existing readers keep every node in their acquired chain alive. */
  if (__atomic_load_n(&stacktrace->readers, __ATOMIC_SEQ_CST))
    stacktrace_wait(stacktrace);
}

INLINE void stacktrace_push (s_stacktrace *stacktrace, p_list list)
{
  assert(stacktrace);
  assert(list);
  __atomic_store_n(&stacktrace->list, list, __ATOMIC_RELEASE);
}

#endif /* LIBKC3_STACKTRACE_H */
