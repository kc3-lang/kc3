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
#include "buf.h"
#include "buf_inspect.h"
#include "buf_inspect_s8_decimal.h"
#include "buf_inspect_s16_decimal.h"
#include "buf_inspect_s32_decimal.h"
#include "buf_inspect_s64_decimal.h"
#include "buf_inspect_sw_decimal.h"
#include "buf_inspect_u8_decimal.h"
#include "buf_inspect_u16_decimal.h"
#include "buf_inspect_u32_decimal.h"
#include "buf_inspect_u64_decimal.h"
#include "buf_inspect_uw_decimal.h"
#include "env.h"
#include "io.h"
#include "rwlock.h"
#include "tag_type.h"

#define DEF_ERR_INSPECT(name, type)                                    \
  sw err_inspect_ ## name (type x)                                     \
  {                                                                    \
    s_env *env;                                                        \
    sw r;                                                              \
    env = env_global();                                                \
    r = buf_inspect_ ## name(env->err, x);                             \
    if (r < 0)                                                         \
      return r;                                                        \
    buf_flush(env->err);                                               \
    return r;                                                          \
  }

#define DEF_IO_INSPECT(name, type)                                     \
  sw io_inspect_ ## name (type x)                                      \
  {                                                                    \
    s_env *env;                                                        \
    sw r;                                                              \
    env = env_global();                                                \
    r = buf_inspect_ ## name(env->out, x);                             \
    if (r < 0)                                                         \
      return r;                                                        \
    buf_flush(env->out);                                               \
    return r;                                                          \
  }

#define DEF_ERR_IO_INSPECT(name, type)                                 \
  DEF_ERR_INSPECT(name, type)                                          \
  DEF_IO_INSPECT(name, type)

sw err_flush (void)
{
  s_env *env;
  env = env_global();
  return buf_flush(env->err);
}

sw err_inspect (const s_tag *x)
{
  return err_inspect_tag(x);
}

/*
sw err_inspect_buf (const s_buf *buf)
{
  uw pos;
  sw r;
  sw result = 0;
  sw size;
  s_str str = {0};
  pos = (buf->rpos < IO_INSPECT_BUF_SIZE) ? 0 :
    buf->rpos - IO_INSPECT_BUF_SIZE;
  str.ptr.pchar = buf->ptr.pchar + pos;
  str.size = buf->rpos - pos;
  if ((r = err_inspect_str(&str)) <= 0)
    return r;
  result += r;
  if ((r = err_write_1("\n---HERE---\n")) <= 0)
    return r;
  result += r;
  size = buf->wpos - buf->rpos;
  size = (size < IO_INSPECT_BUF_SIZE) ? size : IO_INSPECT_BUF_SIZE;
  str.ptr.pchar = buf->ptr.pchar + buf->rpos;
  str.size = size;
  if ((r = err_inspect_str(&str)) <= 0)
    return r;
  result += r;
  if ((r = err_write_1("\n")) <= 0)
    return r;
  result += r;
  return result;
}
*/

sw err_inspect_tag_type (e_tag_type type)
{
  return err_write_1(tag_type_to_string(type));
}

sw err_stacktrace (void)
{
  s_env *env;
  env = env_global();
  return err_inspect_stacktrace(env->stacktrace);
}

sw err_puts (const char *x)
{
  s_env *env;
  sw r;
  sw result = 0;
  env = env_global();
#if HAVE_PTHREAD
  rwlock_w(env->err->rwlock);
#endif
  if ((r = buf_write_1(env->err, x)) < 0)
    goto clean;
  result += r;
  if ((r = buf_write_u8(env->err, '\n')) < 0)
    goto clean;
  result += r;
  buf_flush(env->err);
  r = result;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(env->err->rwlock);
#endif
  return r;
}

sw err_write (const void *x, uw len)
{
  s_env *env;
  sw r;
  env = env_global();
  if ((r = buf_write(env->err, x, len)) > 0)
    buf_flush(env->err);
  return r;
}

sw err_write_1 (const char *x)
{
  s_env *env;
  sw r;
  env = env_global();
  if ((r = buf_write_1(env->err, x)) > 0)
    buf_flush(env->err);
  return r;
}

sw err_write_str (const s_str *x)
{
  s_env *env;
  sw r;
  env = env_global();
  if ((r = buf_write_str(env->err, x)) > 0)
    buf_flush(env->err);
  return r;
}

sw err_write_u8 (u8 x)
{
  s_env *env;
  sw r;
  env = env_global();
  if ((r = buf_write_u8(env->err, x)) > 0)
    buf_flush(env->err);
  return r;
}

sw io_flush (void)
{
  s_env *env;
  env = env_global();
  return buf_flush(env->out);
}

sw io_inspect (const s_tag *x)
{
  s_env *env;
  sw r;
  sw result = 0;
  env = env_global();
  if ((r = buf_inspect_tag(env->out, x)) < 0)
    return r;
  result += r;
  if ((r = buf_write_u8(env->out, '\n')) < 0)
    return r;
  result += r;
  buf_flush(env->out);
  return result;
}

sw io_inspect_tag_type (e_tag_type type)
{
  return io_write_1(tag_type_to_string(type));
}

sw io_stacktrace (void)
{
  s_env *env;
  env = env_global();
  return io_inspect_stacktrace(env->stacktrace);
}

sw io_puts (const char *x)
{
  s_env *env;
  sw r;
  sw result = 0;
  env = env_global();
#if HAVE_PTHREAD
  rwlock_w(env->out->rwlock);
#endif
  if ((r = buf_write_1(env->out, x)) < 0)
    goto clean;
  result += r;
  if ((r = buf_write_u8(env->out, '\n')) < 0)
    goto clean;
  result += r;
  buf_flush(env->out);
  r = result;
 clean:
#if HAVE_PTHREAD
  rwlock_unlock_w(env->out->rwlock);
#endif
  return r;
}

sw io_write (const void *x, uw len)
{
  s_env *env;
  sw r;
  env = env_global();
  if ((r = buf_write(env->out, x, len)) > 0)
    buf_flush(env->out);
  return r;
}

sw io_write_1 (const char *x)
{
  s_env *env;
  sw r;
  env = env_global();
  if ((r = buf_write_1(env->out, x)) > 0)
    buf_flush(env->out);
  return r;
}

sw io_write_str (const s_str *x)
{
  s_env *env;
  sw r;
  env = env_global();
  if ((r = buf_write_str(env->out, x)) > 0)
    buf_flush(env->out);
  return r;
}

sw io_write_u8 (u8 x)
{
  s_env *env;
  sw r;
  env = env_global();
  if ((r = buf_write_u8(env->out, x)) > 0)
    buf_flush(env->out);
  return r;
}

DEF_ERR_IO_INSPECT(array,               const s_array *)
DEF_ERR_IO_INSPECT(bool,                bool)
DEF_ERR_IO_INSPECT(buf,                 const s_buf *)
DEF_ERR_IO_INSPECT(do_block,            const s_do_block *)
DEF_ERR_IO_INSPECT(c_pointer,           const void *)
DEF_ERR_IO_INSPECT(call,                const s_call *)
DEF_ERR_IO_INSPECT(callable,            const s_callable *)
DEF_ERR_IO_INSPECT(cfn,                 const s_cfn *)
DEF_ERR_IO_INSPECT(character,           character)
DEF_ERR_IO_INSPECT(f32,                 f32)
DEF_ERR_IO_INSPECT(fact,                const s_fact *)
DEF_ERR_IO_INSPECT(facts_spec,          const p_facts_spec)
DEF_ERR_IO_INSPECT(fn,                  const s_fn *)
DEF_ERR_IO_INSPECT(fn_clause,           const s_fn_clause *)
DEF_ERR_IO_INSPECT(fn_pattern,          const s_list *)
DEF_ERR_IO_INSPECT(frame,               const s_frame *)
DEF_ERR_IO_INSPECT(ident,               const s_ident *)
DEF_ERR_IO_INSPECT(integer,             const s_integer *)
DEF_ERR_IO_INSPECT(integer_decimal,     const s_integer *)
DEF_ERR_IO_INSPECT(integer_hexadecimal, const s_integer *)
DEF_ERR_IO_INSPECT(list,                const s_list *)
DEF_ERR_IO_INSPECT(map,                 const s_map *)
DEF_ERR_IO_INSPECT(plist,               p_list *)
DEF_ERR_IO_INSPECT(psym,                p_sym const *)
DEF_ERR_IO_INSPECT(ptr,                 const u_ptr_w *)
DEF_ERR_IO_INSPECT(quote,               const s_quote *)
DEF_ERR_IO_INSPECT(ratio,               const s_ratio *)
DEF_ERR_IO_INSPECT(s8,                  s8)
DEF_ERR_IO_INSPECT(s8_decimal,          s8)
DEF_ERR_IO_INSPECT(s8_hexadecimal,      s8)
DEF_ERR_IO_INSPECT(s16,                 s16)
DEF_ERR_IO_INSPECT(s16_decimal,         s16)
DEF_ERR_IO_INSPECT(s16_hexadecimal,     s16)
DEF_ERR_IO_INSPECT(s32,                 s32)
DEF_ERR_IO_INSPECT(s32_decimal,         s32)
DEF_ERR_IO_INSPECT(s32_hexadecimal,     s32)
DEF_ERR_IO_INSPECT(s64,                 s64)
DEF_ERR_IO_INSPECT(s64_decimal,         s64)
DEF_ERR_IO_INSPECT(s64_hexadecimal,     s64)
DEF_ERR_IO_INSPECT(stacktrace,          p_list)
DEF_ERR_IO_INSPECT(str,                 const s_str *)
DEF_ERR_IO_INSPECT(str_hex,             const s_str *)
DEF_ERR_IO_INSPECT(struct,              const s_struct *)
DEF_ERR_IO_INSPECT(struct_type,         const s_struct_type *)
DEF_ERR_IO_INSPECT(sw,                  sw)
DEF_ERR_IO_INSPECT(sw_decimal,          sw)
DEF_ERR_IO_INSPECT(sw_hexadecimal,      sw)
DEF_ERR_IO_INSPECT(sym,                 const s_sym *)
DEF_ERR_IO_INSPECT(tag,                 const s_tag *)
DEF_ERR_IO_INSPECT(time,                const s_time *)
DEF_ERR_IO_INSPECT(tuple,               const s_tuple *)
DEF_ERR_IO_INSPECT(u8,                  u8)
DEF_ERR_IO_INSPECT(u8_decimal,          u8)
DEF_ERR_IO_INSPECT(u8_hexadecimal,      u8)
DEF_ERR_IO_INSPECT(u16,                 u16)
DEF_ERR_IO_INSPECT(u16_decimal,         u16)
DEF_ERR_IO_INSPECT(u16_hexadecimal,     u16)
DEF_ERR_IO_INSPECT(u32,                 u32)
DEF_ERR_IO_INSPECT(u32_decimal,         u32)
DEF_ERR_IO_INSPECT(u32_hexadecimal,     u32)
DEF_ERR_IO_INSPECT(u64,                 u64)
DEF_ERR_IO_INSPECT(u64_decimal,         u64)
DEF_ERR_IO_INSPECT(u64_hexadecimal,     u64)
DEF_ERR_IO_INSPECT(uw,                  uw)
DEF_ERR_IO_INSPECT(uw_decimal,          uw)
DEF_ERR_IO_INSPECT(uw_hexadecimal,      uw)
DEF_ERR_IO_INSPECT(var,                 const s_var *)
