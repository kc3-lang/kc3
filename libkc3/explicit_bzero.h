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
#ifndef KC3_EXPLICIT_BZERO_H
#define KC3_EXPLICIT_BZERO_H

#if ! HAVE_EXPLICIT_BZERO
# define explicit_bzero(p, size) memset(p, 0, size)
#endif

#endif /* KC3_EXPLICIT_BZERO_H */
