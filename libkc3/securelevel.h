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
/** @brief Securelevel : limit code loading functionality.
 *  @desc  There are 4 security levels, from 0 to 3.
 *         Each higher security level inherits all the security features
 *         of previous levels :
 *          securelevel(0) → all code loading is enabled (by default)
 *          securelevel(1) → disable Cfn eval and parsing,
 *                           disable kc3_system() and related functions,
 *                           disable kc3_dlopen() and related functions.
 *          securelevel(2) → disable all KC3 code loading and make
 *                           environment database read-only.
 *          securelevel(3) → disable KC3 code evaluation completely
 *                           (C-only mode).
 */
#ifndef LIBKC3_SECURELEVEL_H
#define LIBKC3_SECURELEVEL_H

#include "types.h"

u8 securelevel (u8 level);

#endif /* LIBKC3_SECURELEVEL_H */
