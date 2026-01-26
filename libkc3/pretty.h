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
/**
 * @file pretty.h
 * @brief Pretty printer
 *
 * Pretty printer functions.
 */
#ifndef LIBKC3_PRETTY_H
#define LIBKC3_PRETTY_H

#include "types.h"

#define PRETTY_INDENT 2

/* Stack-allocation compatible functions, call *_clean after use. */
s_pretty *      pretty_save_clean (const s_pretty_save *pretty_save,
                                   s_pretty *pretty);
s_pretty_save * pretty_save_init (s_pretty_save *pretty_save,
                                  const s_pretty *pretty);

/* Operators. */
s_pretty * pretty_indent (s_pretty *pretty, sw indent);
s_pretty * pretty_indent_from_column (s_pretty *pretty, sw indent);
s_pretty * pretty_indent_at_column (s_pretty *pretty, uw column);

#endif /* LIBKC3_LIST_H */
