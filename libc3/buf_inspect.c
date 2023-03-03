/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libtommath/tommath.h"
#include "c3.h"

sw buf_inspect_ident_reserved (s_buf *buf, const s_ident *x);
sw buf_inspect_ident_reserved_size (const s_ident *x);
sw buf_inspect_str_byte (s_buf *buf, u8 x);
sw buf_inspect_str_reserved (s_buf *buf, const s_str *x);
sw buf_inspect_str_reserved_size (const s_str *x);
sw buf_inspect_sym_reserved (s_buf *buf, const s_sym *x);
sw buf_inspect_sym_reserved_size (const s_sym *x);

sw buf_inspect_bool (s_buf *buf, e_bool x)
{
  if (x)
    return buf_write_1(buf, "true");
  return buf_write_1(buf, "false");
}

sw buf_inspect_bool_size (e_bool x)
{
  if (x)
    return strlen("true");
  return strlen("false");
}

sw buf_inspect_call (s_buf *buf, const s_call *call)
{
  sw r;
  sw result = 0;
  if ((r = buf_inspect_ident(buf, &call->ident)) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_call_args(buf, call->arguments)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_call_args (s_buf *buf, const s_list *args)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "(")) < 0)
    return r;
  result += r;
  while (args) {
    if ((r = buf_inspect_tag(buf, &args->tag)) < 0)
      return r;
    result += r;
    if ((args = list_next(args))) {
      if ((r = buf_write_1(buf, ", ")) < 0)
        return r;
      result += r;
    }
  }
  if ((r = buf_write_1(buf, ")")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_call_args_size (const s_list *args)
{
  return buf_inspect_list_size(args);
}

sw buf_inspect_call_size (const s_call *call)
{
  sw r;
  sw result = 0;
  if ((r = buf_inspect_ident_size(&call->ident)) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_call_args_size(call->arguments)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_character (s_buf *buf, character x)
{
  sw r;
  sw size;
  size = buf_inspect_character_size(x);
  if ((r = buf_write_u8(buf, '\'')) < 0 ||
      (r = buf_inspect_str_character(buf, x)) < 0 ||
      (r = buf_write_u8(buf, '\'')) < 0)
    return r;
  return size;
}

sw buf_inspect_character_size (character c)
{
  sw size;
  size = buf_inspect_str_character_size(c);
  if (size < 0)
    return size;
  if (size == 0)
    return -1;
  size += 2;
  return size;
}

sw buf_inspect_f32 (s_buf *buf, f32 x)
{
  return buf_f(buf, "%g", x);
}

sw buf_inspect_f32_size (f32 x)
{
  s8 b[16];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  return buf_inspect_f32(&buf, x);
}

sw buf_inspect_f64 (s_buf *buf, f64 x)
{
  return buf_f(buf, "%g", x);
}

sw buf_inspect_f64_size (f64 x)
{
  s8 b[16];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  return buf_inspect_f64(&buf, x);
}

sw buf_inspect_fact (s_buf *buf, const s_fact *fact)
{
  sw r;
  sw result = 0;
  if (fact) {
    if ((r = buf_write_1(buf, "{")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, fact->subject)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, ", ")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, fact->predicate)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, ", ")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, fact->object)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, "}")) < 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_fact_size (const s_fact *fact)
{
  sw r;
  sw result = 0;
  if (fact) {
    r = strlen("{");
    result += r;
    r = buf_inspect_tag_size(fact->subject);
    result += r;
    r = strlen(", ");
    result += r;
    r = buf_inspect_tag_size(fact->predicate);
    result += r;
    r = strlen(", ");
    result += r;
    r = buf_inspect_tag_size(fact->subject);
    result += r;
    r = strlen("}");
    result += r;
  }
  return result;
}

sw buf_inspect_fact_spec (s_buf *buf, p_facts_spec spec)
{
  sw r;
  sw result = 0;
  if (spec) {
    if ((r = buf_write_1(buf, "{")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, spec[0])) < 0)
      return r;
    result += r;
    while (spec[0] || spec[1]) {
      spec++;
      if ((r = buf_write_1(buf, ", ")) < 0)
        return r;
      result += r;
      if ((r = buf_inspect_tag(buf, spec[0])) < 0)
        return r;
      result += r;
    }
    if ((r = buf_write_1(buf, "}")) < 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_fn (s_buf *buf, const s_fn *fn)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "fn ")) < 0)
    return r;
  result += r;
  if (fn->next_clause) {
    if ((r = buf_write_1(buf, "{\n")) < 0)
      return r;
    result += r;
    while (fn) {
      if ((r = buf_write_1(buf, "  ")) < 0)
        return r;
      result += r;
      if ((r = buf_inspect_fn_clause(buf, fn)) < 0)
        return r;
      result += r;
      if ((r = buf_write_1(buf, "\n")) < 0)
        return r;
      result += r;
      fn = fn->next_clause;
    }
    if ((r = buf_write_1(buf, "}\n")) < 0)
      return r;
    result += r;
  }
  else {
    if ((r = buf_inspect_fn_clause(buf, fn)) < 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_fn_algo (s_buf *buf, const s_list *algo)
{
  sw r;
  sw result = 0;
  assert(buf);
  if (! algo) {
    if ((r = buf_write_1(buf, "{}")) < 0)
      return r;
    result += r;
    return result;
  }
  if (! list_next(algo)) {
    if ((r = buf_write_1(buf, "{ ")) <= 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, &algo->tag)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, " }")) <= 0)
      return r;
    result += r;
    return result;
  }
  if ((r = buf_write_1(buf, "{\n  ")) <= 0)
    return r;
  result += r;
  while (algo) {
    if ((r = buf_inspect_tag(buf, &algo->tag)) < 0)
      return r;
    result += r;
    algo = list_next(algo);
    if (algo) {
      if ((r = buf_write_1(buf, ";\n  ")) < 0)
        return r;
      result += r;
    }
  }
  if ((r = buf_write_1(buf, "\n}")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_fn_clause (s_buf *buf, const s_fn *fn)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(fn);
  if ((r = buf_inspect_fn_pattern(buf, fn->pattern)) < 0)
    return r;
  result += r;
  if ((r = buf_write_u8(buf, ' ')) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_fn_algo(buf, fn->algo)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_fn_pattern (s_buf *buf, const s_list *pattern)
{
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_u8(buf, '(')) <= 0)
    return r;
  result += r;
  while (pattern) {
    if ((r = buf_inspect_tag(buf, &pattern->tag)) < 0)
      return r;
    result += r;
    pattern = list_next(pattern);
    if (pattern) {
      if ((r = buf_write_1(buf, ", ")) < 0)
        return r;
      result += r;
    }
  }
  if ((r = buf_write_u8(buf, ')')) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ident (s_buf *buf, const s_ident *ident)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(ident);
  result = 0;
  if (ident->module_name) {
    if ((r = buf_inspect_sym(buf, ident->module_name)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, ".")) < 0)
      return r;
    result += r;
  }
  if (ident->sym->str.size == 0) {
    if ((r = buf_write_1(buf, "_\"\"")) < 0)
      return r;
    result += r;
    return result;
  }
  if (ident_has_reserved_characters(ident)) {
    if ((r = buf_inspect_ident_reserved(buf, ident)) < 0)
      return r;
    result += r;
    return result;
  }
  if ((r = buf_write_str(buf, &ident->sym->str)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ident_reserved (s_buf *buf, const s_ident *x)
{
  sw size;
  assert(buf);
  assert(x);
  size = buf_inspect_ident_reserved_size(x);
  if (buf->wpos + size > buf->size) {
    assert(! "buffer overflow");
    return -1;
  }
  buf_write_1(buf, "_");
  buf_inspect_str_reserved(buf, &x->sym->str);
  return size;
}

sw buf_inspect_ident_reserved_size (const s_ident *x)
{
  sw size;
  assert(x);
  size = strlen("_");
  size += buf_inspect_str_reserved_size(&x->sym->str);
  return size;
}

sw buf_inspect_ident_size (const s_ident *ident)
{
  sw r;
  sw result = 0;
  assert(ident);
  if (ident->module_name) {
    if ((r = buf_inspect_sym_size(ident->module_name)) < 0)
      return r;
    result += r;
    result += strlen(".");
  }
  if (ident->sym->str.size == 0)
    return result + strlen("_\"\"");
  if (ident_has_reserved_characters(ident))
    return result + buf_inspect_ident_reserved_size(ident);
  return result + ident->sym->str.size;
}

sw buf_inspect_integer (s_buf *buf, const s_integer *x)
{
  s_buf buf_tmp;
  mp_digit d;
  sw result = 0;
  size_t maxlen;
  u8 p;
  const mp_digit radix = 10;
  s32 size = 0;
  mp_int t;
  if (mp_radix_size(&x->mp_int, radix, &size) != MP_OKAY)
    return -1;
  maxlen = size;
  if (MP_IS_ZERO(&x->mp_int))
    return buf_write_u8(buf, '0');
  if (mp_init_copy(&t, &x->mp_int) != MP_OKAY)
    return -1;
  if (t.sign == MP_NEG) {
    t.sign = MP_ZPOS;
    maxlen--;
    buf_write_u8(buf, '-');
    result++;
  }
  buf_init_alloc(&buf_tmp, maxlen);
  while (!MP_IS_ZERO(&t)) {
    if (mp_div_d(&t, radix, &t, &d) != MP_OKAY)
      goto error;
    p = '0' + d;
    if (p > '9')
      goto error;
    buf_write_u8(&buf_tmp, p);
    result++;
  }
  buf_xfer_reverse(&buf_tmp, buf);
  mp_clear(&t);
  buf_clean(&buf_tmp);
  return result;
 error:
  mp_clear(&t);
  buf_clean(&buf_tmp);
  return -1;
}

sw buf_inspect_integer_size (const s_integer *x)
{
  int size;
  if (mp_radix_size(&x->mp_int, 10, &size) != MP_OKAY)
    return -1;
  return size - 1;
}

sw buf_inspect_list (s_buf *buf, const s_list *x)
{
  const s_list *i;
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_u8(buf, '[')) <= 0)
    return r;
  result++;
  i = x;
  while (i) {
    if ((r = buf_inspect_tag(buf, &i->tag)) < 0)
      return r;
    result += r;
    switch (i->next.type.type) {
    case TAG_LIST:
      if (i->next.data.list) {
        if ((r = buf_write_1(buf, ", ")) < 0)
          return r;
        result += r;
      }
      i = i->next.data.list;
      continue;
    default:
      if ((r = buf_write_1(buf, " | ")) < 0)
        return r;
      result += r;
      if ((r = buf_inspect_tag(buf, &i->next)) < 0)
        return r;
      result += r;
      i = NULL;
    }
  }
  if ((r = buf_write_1(buf, "]")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_list_size (const s_list *list)
{
  const s_list *i;
  sw r;
  sw result = 0;
  result += strlen("[");
  i = list;
  while (i) {
    if ((r = buf_inspect_tag_size(&i->tag)) < 0)
      return r;
    result += r;
    switch (i->next.type.type) {
    case TAG_LIST:
      if (i->next.data.list)
        result += strlen(", ");
      i = i->next.data.list;
      continue;
    default:
      result += strlen(" | ");
      if ((r = buf_inspect_tag_size(&i->next)) < 0)
        return r;
      result += r;
      break;
    }
  }
  result += strlen("]");
  return result;
}

sw buf_inspect_ptag (s_buf *buf, p_tag ptag)
{
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "@0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hex(buf, (uw) ptag)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ptag_size (p_tag ptag)
{
  sw result = 0;
  (void) ptag;
  result += strlen("@0x");
  result += sizeof(uw) / 4;
  return result;
}

sw buf_inspect_quote (s_buf *buf, const s_quote *quote)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "quote ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag(buf, quote->tag)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_quote_size (const s_quote *quote)
{
  sw result = 0;
  result += strlen("'");
  result += buf_inspect_tag_size(quote->tag);
  return result;
}

sw buf_inspect_s8 (s_buf *buf, s8 x)
{
  sw r;
  sw result = 0;
  if (x < 0) {
    if ((r = buf_write_1(buf, "-")) < 0)
      return r;
    result += r;
    x = -x;
  }
  if ((r = buf_inspect_u8(buf, x)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_s8_size (s8 x)
{
  sw result = 0;
  if (x < 0) {
    result += strlen("-");
    x = -x;
  }
  result += buf_inspect_u8_size(x);
  return result;
}

sw buf_inspect_s16 (s_buf *buf, s16 x)
{
  sw r;
  sw r1;
  if (x >= 0)
    return buf_inspect_u16(buf, (u16) x);
  if ((r = buf_write_1(buf, "-")) < 0)
    return r;
  if ((r1 = buf_inspect_u16(buf, (u16) -x)) < 0)
    return r1;
  return r + r1;
}

sw buf_inspect_s16_size (s16 x)
{
  sw result = 0;
  if (x < 0) {
    result += strlen("-");
    x = -x;
  }
  result += buf_inspect_u16_size(x);
  return result;
}

sw buf_inspect_s32 (s_buf *buf, s32 x)
{
  sw r;
  sw r1;
  if (x >= 0)
    return buf_inspect_u32(buf, (u32) x);
  if ((r = buf_write_1(buf, "-")) < 0)
    return r;
  if ((r1 = buf_inspect_u32(buf, (u32) -x)) < 0)
    return r1;
  return r + r1;
}

sw buf_inspect_s32_size (s32 x)
{
  sw result = 0;
  if (x < 0) {
    result += strlen("-");
    x = -x;
  }
  result += buf_inspect_u32_size(x);
  return result;
}

sw buf_inspect_s64 (s_buf *buf, s64 x)
{
  sw r;
  sw r1;
  if (x >= 0)
    return buf_inspect_u64(buf, (u64) x);
  if ((r = buf_write_1(buf, "-")) < 0)
    return r;
  if ((r1 = buf_inspect_u64(buf, (u64) -x)) < 0)
    return r1;
  return r + r1;
}

sw buf_inspect_s64_size (s64 x)
{
  sw result = 0;
  if (x < 0) {
    result += strlen("-");
    x = -x;
  }
  result += buf_inspect_u64_size(x);
  return result;
}

sw buf_inspect_str (s_buf *buf, const s_str *x)
{
  sw r;
  sw size;
  assert(buf);
  assert(x);
  if (str_has_reserved_characters(x))
    return buf_inspect_str_reserved(buf, x);
  size = x->size + 2;
  if ((r = buf_write_u8(buf, '"')) < 0 ||
      (r = buf_write_str(buf, x)) < 0 ||
      (r = buf_write_u8(buf, '"') < 0))
    return r;
  return size;
}

sw buf_inspect_str_byte (s_buf *buf, u8 x)
{
  sw r;
  const sw size = 4;
  if ((r = buf_write_u8(buf, '\\')) < 0 ||
      (r = buf_write_u8(buf, 'x')) < 0 ||
      (r = buf_u8_to_hex(buf, x)) < 0)
    return r;
  return size;
}

sw buf_inspect_str_character (s_buf *buf, character x)
{
  s8 b[4];
  s_buf char_buf;
  int i;
  int j;
  sw r;
  sw size;
  size = buf_inspect_str_character_size(x);
  if (size <= 0)
    return size;
  if (! str_character_is_reserved(x))
    return buf_write_character_utf8(buf, x);
  buf_write_u8(buf, '\\');
  switch (x) {
  case '\0': if ((r = buf_write_u8(buf, '0')) < 0) return r; break;
  case '\n': if ((r = buf_write_u8(buf, 'n')) < 0) return r; break;
  case '\r': if ((r = buf_write_u8(buf, 'r')) < 0) return r; break;
  case '\t': if ((r = buf_write_u8(buf, 't')) < 0) return r; break;
  case '\v': if ((r = buf_write_u8(buf, 'v')) < 0) return r; break;
  case '\"': if ((r = buf_write_u8(buf, '"')) < 0) return r; break;
  case '\'': if ((r = buf_write_u8(buf, '\'')) < 0) return r; break;
  case '\\': if ((r = buf_write_u8(buf, '\\')) < 0) return r; break;
  default:
    buf_init(&char_buf, false, sizeof(b), b);
    if ((i = buf_write_character_utf8(&char_buf, x)) < 0)
      return i;
    j = 0;
    if (i-- > 0) {
      if ((r = buf_write_u8(buf, 'x')) < 0)
        return r;
      if ((r = buf_u8_to_hex(buf, char_buf.ptr.pu8[j++])) < 0)
        return r;
      while (i--) {
        if ((r = buf_write_1(buf, "\\x")) < 0 ||
            (r = buf_u8_to_hex(buf, char_buf.ptr.pu8[j++])) < 0)
          return r;
      }
    }
  }
  return size;
}

sw buf_inspect_str_character_size (character x)
{
  sw csize;
  sw size;
  if (! str_character_is_reserved(x))
    return character_utf8_size(x);
  size = 0;
  switch (x) {
  case '\0':
  case '\n':
  case '\r':
  case '\t':
  case '\v':
  case '\"':
  case '\'':
  case '\\':
    size += 2;
    break;
  default:
    csize = character_utf8_size(x);
    if (csize <= 0)
      return -1;
    size += csize * 4;
  }
  return size;
}

/* XXX keep in sync with buf_inspect_str_reserved_size */
sw buf_inspect_str_reserved (s_buf *buf, const s_str *x)
{
  u8 b;
  character c;
  sw r;
  sw size;
  s_str s;
  size = buf_inspect_str_reserved_size(x);
  if (size <= 0)
    return size;
  if ((r = buf_write_u8(buf, '"')) < 0)
    return r;
  str_init_str(&s, x);
  while (1) {
    if ((r = str_read_character(&s, &c)) > 0) {
      if ((r = buf_inspect_str_character(buf, c)) < 0)
        return r;
    }
    else if ((r = str_read(&s, &b)) == 1) {
      if ((r = buf_inspect_str_byte(buf, b)) < 0)
        return r;
    }
    else if (r < 0)
      return r;
    else if (r == 0)
      break;
  }
  if ((r = buf_write_u8(buf, '"')) < 0)
    return r;
  return size;
}

/* XXX keep in sync with buf_inspect_str_reserved */
sw buf_inspect_str_reserved_size (const s_str *x)
{
  u8 b;
  character c;
  const sw quote_size = 1;
  sw r;
  s_str s;
  sw size;
  size = 2 * quote_size;
  str_init_str(&s, x);
  while (1) {
    if ((r = str_read_character(&s, &c)) > 0)
      size += buf_inspect_str_character_size(c);
    else if (r && (r = str_read(&s, &b)) == 1)
      size += buf_inspect_str_byte_size;
    else if (r < 0)
      return -1;
    else if (r == 0)
      break;
  }
  return size;
}

sw buf_inspect_str_size (const s_str *x)
{
  const sw quote_size = 1;
  sw size;
  if (str_has_reserved_characters(x))
    return buf_inspect_str_reserved_size(x);
  size = x->size + 2 * quote_size;
  return size;
}

sw buf_inspect_sym (s_buf *buf, const s_sym *x)
{
  sw r;
  sw size;
  assert(buf);
  assert(x);
  if (x->str.size == 0)
    return buf_write_1(buf, ":\"\"");
  if (sym_has_reserved_characters(x))
    return buf_inspect_sym_reserved(buf, x);
  if (sym_is_module(x))
    return buf_write_str(buf, &x->str);
  size = x->str.size + 1;
  if ((r = buf_write_1(buf, ":")) < 0 ||
      (r = buf_write_str(buf, &x->str)) < 0)
    return r;
  return size;
}

sw buf_inspect_sym_size (const s_sym *x)
{
  const sw colon_size = 1;
  assert(x);
  if (x->str.size == 0)
    return 3;
  if (sym_has_reserved_characters(x))
    return buf_inspect_sym_reserved_size(x);
  if (sym_is_module(x))
    return x->str.size;
  return x->str.size + colon_size;
}

/* XXX keep in sync with buf_inspect_sym_reserved_size */
sw buf_inspect_sym_reserved (s_buf *buf, const s_sym *x)
{
  sw r;
  sw size;
  size = buf_inspect_sym_reserved_size(x);
  if (size <= 0)
    return size;
  if ((r = buf_write_u8(buf, ':')) < 0 ||
      (r = buf_inspect_str(buf, &x->str)) < 0)
    return r;
  return size;
}

/* XXX keep in sync with buf_inspect_sym_reserved */
sw buf_inspect_sym_reserved_size (const s_sym *x)
{
  const sw colon_size = 1;
  sw size;
  size = buf_inspect_str_size(&x->str);
  if (size < 0)
    return size;
  size += colon_size;
  return size;
}

sw buf_inspect_tag (s_buf *buf, const s_tag *tag)
{
  if (! tag)
    return buf_write_1(buf, "NULL");
  switch(tag->type.type) {
  case TAG_VOID:    return 0;
  case TAG_BOOL:    return buf_inspect_bool(buf, tag->data.bool);
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
                    return buf_inspect_call(buf, &tag->data.call);
  case TAG_CHARACTER:
    return buf_inspect_character(buf, tag->data.character);
  case TAG_F32:     return buf_inspect_f32(buf, tag->data.f32);
  case TAG_F64:     return buf_inspect_f64(buf, tag->data.f64);
  case TAG_FN:      return buf_inspect_fn(buf, tag->data.fn);
  case TAG_IDENT:   return buf_inspect_ident(buf, &tag->data.ident);
  case TAG_INTEGER: return buf_inspect_integer(buf, &tag->data.integer);
  case TAG_LIST:    return buf_inspect_list(buf, tag->data.list);
  case TAG_PTAG:    return buf_inspect_ptag(buf, tag->data.ptag);
  case TAG_QUOTE:   return buf_inspect_quote(buf, &tag->data.quote);
  case TAG_S8:      return buf_inspect_s8(buf, tag->data.s8);
  case TAG_S16:     return buf_inspect_s16(buf, tag->data.s16);
  case TAG_S32:     return buf_inspect_s32(buf, tag->data.s32);
  case TAG_S64:     return buf_inspect_s64(buf, tag->data.s64);
  case TAG_STR:     return buf_inspect_str(buf, &tag->data.str);
  case TAG_SYM:     return buf_inspect_sym(buf, tag->data.sym);
  case TAG_TUPLE:   return buf_inspect_tuple(buf, &tag->data.tuple);
  case TAG_U8:      return buf_inspect_u8(buf, tag->data.u8);
  case TAG_U16:     return buf_inspect_u16(buf, tag->data.u16);
  case TAG_U32:     return buf_inspect_u32(buf, tag->data.u32);
  case TAG_U64:     return buf_inspect_u64(buf, tag->data.u64);
  case TAG_VAR:     return buf_inspect_var(buf, tag);
  }
  assert(! "unknown tag type");
  return -1;
}

sw buf_inspect_tag_size (const s_tag *tag)
{
  assert(tag);
  switch(tag->type.type) {
  case TAG_VOID:     return 0;
  case TAG_BOOL:     return buf_inspect_bool_size(tag->data.bool);
  case TAG_CALL:
  case TAG_CALL_FN:
  case TAG_CALL_MACRO:
    return buf_inspect_call_size(&tag->data.call);
  case TAG_CHARACTER:
    return buf_inspect_character_size(tag->data.character);
  case TAG_F32:      return buf_inspect_f32_size(tag->data.f32);
  case TAG_F64:      return buf_inspect_f64_size(tag->data.f64);
  case TAG_FN:       return BUF_INSPECT_UW_HEX_SIZE;
  case TAG_IDENT:    return buf_inspect_ident_size(&tag->data.ident);
  case TAG_INTEGER:
    return buf_inspect_integer_size(&tag->data.integer);
  case TAG_LIST:     return buf_inspect_list_size(tag->data.list);
  case TAG_PTAG:     return buf_inspect_ptag_size(tag->data.ptag);
  case TAG_QUOTE:    return buf_inspect_quote_size(&tag->data.quote);
  case TAG_S8:       return buf_inspect_s8_size(tag->data.s8);
  case TAG_S16:      return buf_inspect_s16_size(tag->data.s16);
  case TAG_S32:      return buf_inspect_s32_size(tag->data.s32);
  case TAG_S64:      return buf_inspect_s64_size(tag->data.s64);
  case TAG_STR:      return buf_inspect_str_size(&tag->data.str);
  case TAG_SYM:      return buf_inspect_sym_size(tag->data.sym);
  case TAG_TUPLE:    return buf_inspect_tuple_size(&tag->data.tuple);
  case TAG_U8:       return buf_inspect_u8_size(tag->data.u8);
  case TAG_U16:      return buf_inspect_u16_size(tag->data.u16);
  case TAG_U32:      return buf_inspect_u32_size(tag->data.u32);
  case TAG_U64:      return buf_inspect_u64_size(tag->data.u64);
  case TAG_VAR:      return BUF_INSPECT_VAR_SIZE;
  }
  assert(! "unknown tag type");
  return -1;
}

sw buf_inspect_tuple (s_buf *buf, const s_tuple *tuple)
{
  u64 i = 0;
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "{")) < 0)
    return r;
  result += r;
  if (tuple->count) {
    while (i < tuple->count - 1) {
      if ((r = buf_inspect_tag(buf, tuple->tag + i)) < 0)
        return r;
      result += r;
      if ((r = buf_write_1(buf, ", ")) < 0)
        return r;
      result += r;
      i++;
    }
    if ((r = buf_inspect_tag(buf, tuple->tag + i)) < 0)
      return r;
    result += r;
  }
  if ((r = buf_write_1(buf, "}")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_tuple_size (const s_tuple *tuple)
{
  u64 i = 0;
  sw r;
  sw result;
  result = strlen("{");
  if (tuple->count) {
    while (i < tuple->count - 1) {
      if ((r = buf_inspect_tag_size(tuple->tag + i)) < 0)
        return r;
      result += r;
      result += strlen(", ");
      i++;
    }
    if ((r = buf_inspect_tag_size(tuple->tag + i)) < 0)
      return r;
    result += r;
  }
  result += strlen("}");
  return result;
}

sw buf_inspect_u8 (s_buf *buf, u8 x)
{
  u8 digit;
  sw i;
  sw r;
  sw size = 0;
  s8 t[3];
  s_buf tmp;
  buf_init(&tmp, false, sizeof(t), t);
  if (x == 0)
    return buf_write_u8(buf, '0');
  while (x > 0) {
    digit = x % 10;
    x /= 10;
    if (buf_write_u8(&tmp, '0' + digit) != 1) {
      return -1;
    }
    size++;
  }
  i = size;
  while (i--)
    if ((r = buf_write_u8(buf, tmp.ptr.pu8[i])) < 0)
      return r;
  return size;
}

sw buf_inspect_u8_size (u8 x)
{
  sw size = 0;
  if (x == 0)
    return 1;
  while (x > 0) {
    x /= 10;
    size++;
  }
  return size;
}

sw buf_inspect_u16 (s_buf *buf, u16 x)
{
  u8 digit;
  sw i;
  sw r;
  sw size = 0;
  s8 t[5];
  s_buf tmp;
  buf_init(&tmp, false, sizeof(t), t);
  if (x == 0)
    return buf_write_u8(buf, '0');
  while (x > 0) {
    digit = x % 10;
    x /= 10;
    if (buf_write_u8(&tmp, '0' + digit) != 1) {
      return -1;
    }
    size++;
  }
  i = size;
  while (i--)
    if ((r = buf_write_u8(buf, tmp.ptr.pu8[i])) < 0)
      return r;
  return size;
}

sw buf_inspect_u16_size (u16 x)
{
  sw size = 0;
  if (x == 0)
    return 1;
  while (x > 0) {
    x /= 10;
    size++;
  }
  return size;
}

sw buf_inspect_u32 (s_buf *buf, u32 x)
{
  u8 digit;
  sw i;
  sw r;
  sw size = 0;
  s8 t[10];
  s_buf tmp;
  buf_init(&tmp, false, sizeof(t), t);
  if (x == 0)
    return buf_write_u8(buf, '0');
  while (x > 0) {
    digit = x % 10;
    x /= 10;
    if (buf_write_u8(&tmp, '0' + digit) != 1) {
      return -1;
    }
    size++;
  }
  i = size;
  while (i--)
    if ((r = buf_write_u8(buf, tmp.ptr.pu8[i])) < 0)
      return r;
  return size;
}

sw buf_inspect_u32_size (u32 x)
{
  sw size = 0;
  if (x == 0)
    return 1;
  while (x > 0) {
    x /= 10;
    size++;
  }
  return size;
}

sw buf_inspect_u64 (s_buf *buf, u64 x)
{
  u8 digit;
  sw i;
  sw r;
  sw size = 0;
  s8 t[20];
  s_buf tmp;
  buf_init(&tmp, false, sizeof(t), t);
  if (x == 0)
    return buf_write_u8(buf, '0');
  while (x > 0) {
    digit = x % 10;
    x /= 10;
    if (buf_write_u8(&tmp, '0' + digit) != 1) {
      return -1;
    }
    size++;
  }
  i = size;
  while (i--)
    if ((r = buf_write_u8(buf, tmp.ptr.pu8[i])) < 0)
      return r;
  return size;
}

sw buf_inspect_u32_hex (s_buf *buf, u32 i)
{
  const s8 basis[] = "0123456789ABCDEF";
  uw shift = 32;
  while (shift) {
    shift -= 4;
    buf_write_s8(buf, basis[(i >> shift) & 0xF]);
  }
  return 8;
}

sw buf_inspect_u64_hex (s_buf *buf, u64 i)
{
  const s8 basis[] = "0123456789ABCDEF";
  uw shift = 64;
  while (shift) {
    shift -= 4;
    buf_write_s8(buf, basis[(i >> shift) & 0xF]);
  }
  return 16;
}

sw buf_inspect_u64_size (u64 i)
{
  sw size = 0;
  if (i == 0)
    return 1;
  while (i > 0) {
    i /= 10;
    size++;
  }
  return size;
}

sw buf_inspect_uw_hex (s_buf *buf, uw i)
{
  if (sizeof(uw) == 8)
    return buf_inspect_u64_hex(buf, i);
  if (sizeof(uw) == 4)
    return buf_inspect_u32_hex(buf, i);
  assert(! "unknown word size");
  return -1;
}

sw buf_inspect_var (s_buf *buf, const s_tag *var)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "var(0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hex(buf, (uw) var)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, ")")) < 0)
    return r;
  result += r;
  return result;
}
