/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#include "buf.h"
#include "buf_inspect.h"
#include "env.h"
#include "io.h"

#define DEF_ERR_INSPECT(name, type)                                    \
  sw err_inspect_ ## name (const type *x)                              \
  {                                                                    \
    sw r;                                                              \
    sw result = 0;                                                     \
    if ((r = buf_inspect_ ## name(&g_c3_env.err, x)) < 0)              \
      return r;                                                        \
    result += r;                                                       \
    if ((r = buf_write_u8(&g_c3_env.err, '\n')) < 0)                   \
      return r;                                                        \
    result += r;                                                       \
    buf_flush(&g_c3_env.err);                                          \
    return result;                                                     \
  }

#define DEF_IO_INSPECT(name, type)                                     \
  sw io_inspect_ ## name (const type *x)                               \
  {                                                                    \
    sw r;                                                              \
    sw result = 0;                                                     \
    if ((r = buf_inspect_ ## name(&g_c3_env.out, x)) < 0)              \
      return r;                                                        \
    result += r;                                                       \
    if ((r = buf_write_u8(&g_c3_env.out, '\n')) < 0)                   \
      return r;                                                        \
    result += r;                                                       \
    buf_flush(&g_c3_env.out);                                          \
    return result;                                                     \
  }

sw err_inspect (const s_tag *x)
{
  sw r;
  sw result = 0;
  if ((r = buf_inspect_tag(&g_c3_env.err, x)) < 0)
    return r;
  result += r;
  if ((r = buf_write_u8(&g_c3_env.err, '\n')) < 0)
    return r;
  result += r;
  buf_flush(&g_c3_env.err);
  return result;
}

sw err_inspect_fn_pattern (const s_list *x)
{
  sw r;
  r = buf_inspect_fn_pattern(&g_c3_env.err, x);
  buf_flush(&g_c3_env.err);
  return r;
}

sw err_inspect_list (const s_list *x)
{
  sw r;
  r = buf_inspect_list(&g_c3_env.err, &x);
  buf_flush(&g_c3_env.err);
  return r;
}

sw err_puts (const s8 *x)
{
  sw r;
  r = buf_write_1(&g_c3_env.err, x);
  buf_flush(&g_c3_env.err);
  return r;
}

sw io_inspect (const s_tag *x)
{
  sw r;
  sw result = 0;
  if ((r = buf_inspect_tag(&g_c3_env.out, x)) < 0)
    return r;
  result += r;
  if ((r = buf_write_u8(&g_c3_env.out, '\n')) < 0)
    return r;
  result += r;
  buf_flush(&g_c3_env.out);
  return result;
}

DEF_IO_INSPECT(fact, s_fact)
DEF_IO_INSPECT(str,  s_str)

sw io_puts (const s8 *x)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(&g_c3_env.out, x)) < 0)
    return r;
  result += r;
  if ((r = buf_write_u8(&g_c3_env.out, '\n')) < 0)
    return r;
  result += r;
  buf_flush(&g_c3_env.out);
  return result;
}
