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
#ifndef LIBKC3_INLINE_H
#define LIBKC3_INLINE_H

#ifdef _MSC_VER
# define ALWAYS_INLINE __forceinline
#elif defined(__GNUC__)
# define ALWAYS_INLINE inline __attribute__((__always_inline__))
#else
# define ALWAYS_INLINE inline
#endif

#define INLINE static ALWAYS_INLINE

#endif /* LIBKC3_INLINE_H */
