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
#ifndef LIBKC3_ERROR_HANDLER_H
#define LIBKC3_ERROR_HANDLER_H

#include "types.h"

/* stack-allocation compatible functions */
void              error_handler_clean (s_error_handler *eh);
s_error_handler * error_handler_init (s_error_handler *eh,
                                      s_error_handler *next);

/* constructors */
s_error_handler * error_handler_new (s_error_handler *next);

/* destructors */
s_error_handler * error_handler_delete (s_error_handler *eh);
void              error_handler_delete_all (s_error_handler *eh);

#endif /* LIBKC3_ERROR_HANDLER_H */
