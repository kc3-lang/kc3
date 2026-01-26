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
#include "io.h"
#include "pretty.h"

s_pretty * pretty_indent (s_pretty *pretty, sw indent)
{
  if ((sw) pretty->base_column + indent < 0)
    return NULL;
  pretty->base_column += indent;
  return pretty;
}

s_pretty * pretty_indent_at_column (s_pretty *pretty, uw column)
{
  pretty->base_column = column;
  return pretty;
}

s_pretty * pretty_indent_from_column (s_pretty *pretty, sw indent)
{
  if (false) {
    err_write_1("pretty->column = ");
    err_inspect_sw_decimal(pretty->column);
    err_write_1("\n");
  }
  if ((sw) pretty->column + indent < 0)
    return NULL;
  pretty->base_column = pretty->column + indent;
  if (false) {
    err_write_1("pretty->base_column = ");
    err_inspect_uw_decimal(pretty->base_column);
    err_write_1("\n");
  }
  return pretty;
}

s_pretty * pretty_save_clean (const s_pretty_save *pretty_save,
                              s_pretty *pretty)
{
  pretty->base_column = pretty_save->base_column;
  return pretty;
}

s_pretty_save * pretty_save_init (s_pretty_save *pretty_save,
                                  const s_pretty *pretty)
{
  s_pretty_save tmp = {0};
  tmp.base_column = pretty->base_column;
  *pretty_save = tmp;
  return pretty_save;
}
