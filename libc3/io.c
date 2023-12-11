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
  sw err_inspect_ ## name (type x)                                     \
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
  sw io_inspect_ ## name (type x)                                      \
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

#define DEF_ERR_IO_INSPECT(name, type)                                 \
  DEF_ERR_INSPECT(name, type)                                          \
  DEF_IO_INSPECT(name, type)

sw err_inspect (const s_tag *x)
{
  return err_inspect_tag(x);
}

sw err_puts (const s8 *x)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(&g_c3_env.err, x)) < 0)
    return r;
  result += r;
  if ((r = buf_write_u8(&g_c3_env.err, '\n')) < 0)
    return r;
  result += r;
  buf_flush(&g_c3_env.err);
  return result;
}

sw err_write_1 (const s8 *x)
{
  sw r;
  if ((r = buf_write_1(&g_c3_env.err, x)) > 0)
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

sw io_write_1 (const s8 *x)
{
  sw r;
  if ((r = buf_write_1(&g_c3_env.out, x)) > 0)
    buf_flush(&g_c3_env.out);
  return r;
}

DEF_ERR_IO_INSPECT(array,      const s_array *)
DEF_ERR_IO_INSPECT(fact,       const s_fact *)
DEF_ERR_IO_INSPECT(fn_pattern, const s_list *)
DEF_ERR_IO_INSPECT(list,       const s_list **)
DEF_ERR_IO_INSPECT(map,        const s_map *)
DEF_ERR_IO_INSPECT(str,        const s_str *)
DEF_ERR_IO_INSPECT(tag,        const s_tag *)
DEF_ERR_IO_INSPECT(s8,         const s8 *)
DEF_ERR_IO_INSPECT(s16,        const s16 *)
DEF_ERR_IO_INSPECT(s32,        const s32 *)
DEF_ERR_IO_INSPECT(s64,        const s64 *)
DEF_ERR_IO_INSPECT(sw,         const sw *)
DEF_ERR_IO_INSPECT(u8,         const u8 *)
DEF_ERR_IO_INSPECT(u16,        const u16 *)
DEF_ERR_IO_INSPECT(u32,        const u32 *)
DEF_ERR_IO_INSPECT(u64,        const u64 *)
DEF_ERR_IO_INSPECT(uw,         const uw *)
