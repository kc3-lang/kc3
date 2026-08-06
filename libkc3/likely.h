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
#ifndef LIBKC3_LIKELY_H
#define LIBKC3_LIKELY_H

#if (defined(__GNUC__) ||         \
     defined(__INTEL_COMPILER) || \
     defined(__clang__))
# define likely(x)  __builtin_expect(x,1)
# define unlikely(x)  __builtin_expect(x,0)
#else
# define likely(x) (x)
# define unlikely(x) (x)
#endif

#endif /* LIBKC3_LIKELY_H */
