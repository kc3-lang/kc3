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
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "inspect.h"
#include "integer.h"

s_str * inspect_array (const s_array *array, s_str *dest)
{
  s_pretty pretty = {0};
  sw size;
  s_buf tmp;
  size = buf_inspect_array_size(&pretty, array);
  if (size < 0) {
    err_puts("inspect_array: buf_inspect_array_size error");
    assert(! "inspect_array: buf_inspect_array_size error");
    return NULL;
  }
  if (! buf_init_alloc(&tmp, size)) {
    err_puts("inspect_array: buf_init alloc");
    assert(! "inspect_array: buf_init_alloc");
    return NULL;
  }
  buf_inspect_array(&tmp, array);
  if (tmp.wpos != tmp.size) {
    err_puts("inspect_array: tmp.wpos != tmp.size");
    assert(! "inspect_array: tmp.wpos != tmp.size");
    buf_clean(&tmp);
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}

s_str * inspect_bool (bool b, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw size;
  size = buf_inspect_bool_size(&pretty, b);
  if (size < 0) {
    err_write_1("inspect_bool: error: ");
    err_inspect_u8(b);
    assert(! "inspect_bool: error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  buf_inspect_bool(&buf, b);
  assert(buf.wpos == buf.size);
  if (buf.wpos != buf.size) {
    buf_clean(&buf);
    err_write_1("inspect_bool: buf_inspect_bool");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_buf (const s_buf *src, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw size;
  size = buf_inspect_buf_size(&pretty, src);
  if (size < 0) {
    err_write_1("inspect_buf: buf_inspect_buf_size: ");
    err_inspect_sw_decimal(size);
    assert(! "inspect_buf: buf_inspect_buf_size");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  buf_inspect_buf(&buf, src);
  assert(buf.wpos == buf.size);
  if (buf.wpos != buf.size) {
    buf_clean(&buf);
    err_write_1("inspect_buf: buf_inspect_buf");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_call (const s_call *call, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw size;
  size = buf_inspect_call_size(&pretty, call);
  if (size < 0) {
    err_puts("inspect_call: buf_inspect_call_size");
    assert(! "inspect_call: buf_inspect_call_size");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  buf_inspect_call(&buf, call);
  assert(buf.wpos == buf.size);
  if (buf.wpos != buf.size) {
    buf_clean(&buf);
    err_puts("inspect_call: buf_inspect_call");
    assert(! "inspect_call: buf_inspect_call");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_character (character b, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw size;
  size = buf_inspect_character_size(&pretty, b);
  if (size < 0) {
    err_write_1("inspect_character: error: ");
    err_inspect_u8(b);
    assert(! "inspect_character: error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  buf_inspect_character(&buf, b);
  assert(buf.wpos == buf.size);
  if (buf.wpos != buf.size) {
    buf_clean(&buf);
    err_write_1("inspect_character: buf_inspect_character");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_cow (s_cow *cow, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw size;
  assert(cow);
  assert(dest);
  if ((size = buf_inspect_cow_size(&pretty, cow)) < 0) {
    err_puts("inspect_cow: size error");
    assert(! "inspect_cow: size error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  if (buf_inspect_cow(&buf, cow) != size) {
    err_puts("inspect_cow: inspect error");
    assert(! "inspect_cow: inspect error");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_do_block (const s_do_block *x, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw r;
  sw size;
  size = buf_inspect_do_block_size(&pretty, x);
  if (size < 0) {
    err_puts("inspect_do_block: buf_inspect_do_block_size error");
    assert(! "inspect_do_block: buf_inspect_do_block_size error");
    return NULL;
  }
  if (! buf_init_alloc(&buf, size))
    return NULL;
  if ((r = buf_inspect_do_block(&buf, x)) < 0)
    goto error;
  assert(r == size);
  if (r != size)
    goto error;
  return buf_to_str(&buf, dest);
 error:
  buf_clean(&buf);
  return NULL;
}

s_str * inspect_fact (const s_fact *fact, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw size;
  if ((size = buf_inspect_fact_size(&pretty, fact)) < 0) {
    err_puts("inspect_fact: size error");
    assert(! "inspect_fact: size error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  if (buf_inspect_fact(&buf, fact) != size) {
    err_puts("inspect_fact: inspect error");
    assert(! "inspect_fact: inspect error");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_ident (const s_ident *ident, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw r;
  sw size;
  size = buf_inspect_ident_size(&pretty, ident);
  if (size < 0)
    return NULL;
  buf_init_alloc(&buf, size);
  r = buf_inspect_ident(&buf, ident);
  if (r != size) {
    buf_clean(&buf);
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_list (const s_list *x, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw r;
  sw size;
  size = buf_inspect_list_size(&pretty, x);
  buf_init_alloc(&buf, size);
  if ((r = buf_inspect_list(&buf, x)) < 0)
    goto error;
  assert(r == size);
  if (r != size)
    goto error;
  return buf_to_str(&buf, dest);
 error:
  buf_clean(&buf);
  return NULL;
}

s_str * inspect_ratio (const s_ratio *src, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw r;
  sw size;
  assert(src);
  assert(dest);
  assert(integer_is_positive(&src->denominator));
  size = buf_inspect_ratio_size(&pretty, src);
  if (size <= 0)
    return NULL;
  if (! buf_init_alloc(&buf, size))
    return NULL;
  r = buf_inspect_ratio(&buf, src);
  if (r != size) {
   err_write_1("inspect_ratio: ");
   err_inspect_sw(r);
   err_write_1(" != ");
   err_inspect_sw(size);
   err_write_1("\n");
   assert(! "inspect_ratio: buf_inspect_ratio != size");
   return NULL;
  }
  assert(buf.wpos == (uw) size);
  return buf_to_str(&buf, dest);
}

s_str * inspect_str (const s_str *str, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw r;
  sw size;
  size = buf_inspect_str_size(&pretty, str);
  if (size < 0) {
    err_puts("inspect_str: buf_inspect_str_size");
    assert(! "inspect_str: buf_inspect_str_size");
    return NULL;
  }
  if (! buf_init_alloc(&buf, size))
    return NULL;
  r = buf_inspect_str(&buf, str);
  if (r != size) {
    buf_clean(&buf);
    err_puts("inspect_str: buf_inspect_str");
    assert(! "inspect_str: buf_inspect_str");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_str_hex (const s_str *str, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw r;
  sw size;
  size = buf_inspect_str_hex_size(&pretty, str);
  if (size < 0) {
    err_puts("inspect_str_hex: buf_inspect_str_hex_size");
    assert(! "inspect_str_hex: buf_inspect_str_hex_size");
    return NULL;
  }
  if (! buf_init_alloc(&buf, size))
    return NULL;
  r = buf_inspect_str_hex(&buf, str);
  if (r != size) {
    buf_clean(&buf);
    err_puts("inspect_str_hex: buf_inspect_str_hex");
    assert(! "inspect_str_hex: buf_inspect_str_hex");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_str_eval (const s_list *list, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw r;
  sw size;
  size = buf_inspect_str_eval_size(&pretty, list);
  if (size < 0) {
    err_puts("inspect_str: buf_inspect_str_size");
    assert(! "inspect_str: buf_inspect_str_size");
    return NULL;
  }
  if (! buf_init_alloc(&buf, size))
    return NULL;
  if ((r = buf_inspect_str_eval(&buf, list)) <= 0) {
    buf_clean(&buf);
    err_puts("inspect_str: buf_inspect_str_eval <= 0");
    assert(! "inspect_str: buf_inspect_str_eval <= 0");
    return NULL;
  }
  if (r != size) {
    buf_clean(&buf);
    err_puts("inspect_str: buf_inspect_str_eval != size");
    assert(! "inspect_str: buf_inspect_str_eval != size");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_struct (const s_struct *s, s_str *dest)
{
  s_pretty pretty = {0};
  sw size;
  s_buf tmp = {0};
  size = buf_inspect_struct_size(&pretty, s);
  if (size < 0) {
    err_puts("inspect_struct: buf_inspect_struct_size error");
    assert(! "inspect_struct: buf_inspect_struct_size error");
    return NULL;
  }
  if (! buf_init_alloc(&tmp, size)) {
    err_puts("inspect_struct: buf_init alloc");
    assert(! "inspect_struct: buf_init_alloc");
    return NULL;
  }
  if (buf_inspect_struct(&tmp, s) < 0) {
    err_puts("inspect_struct: buf_inspect_struct error");
    assert(! "inspect_struct: buf_inspect_struct error");
    buf_clean(&tmp);
    return NULL;
  }
  if (tmp.wpos != tmp.size) {
    err_puts("inspect_struct: tmp.wpos != tmp.size");
    assert(! "inspect_struct: tmp.wpos != tmp.size");
    buf_clean(&tmp);
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}

s_str * inspect_sym (const s_sym *sym, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw size;
  size = buf_inspect_sym_size(&pretty, sym);
  if (size < 0) {
    assert(! "error");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  buf_inspect_sym(&buf, sym);
  assert(buf.wpos == buf.size);
  return buf_to_str(&buf, dest);
}

s_str * inspect_tag (const s_tag *tag, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw r;
  sw size;
  assert(tag);
  assert(dest);
  if ((size = buf_inspect_tag_size(&pretty, tag)) < 0) {
    err_puts("inspect_tag: buf_inspect_tag_size: error");
    assert(! "inspect_tag: buf_inspect_tag_size: error");
    return NULL;
  }
  if (false) {
    err_write_1("inspect_tag: size = ");
    err_inspect_sw_decimal(size);
    err_write_1("\n");
  }
  buf_init_alloc(&buf, size);
  if ((r = buf_inspect_tag(&buf, tag)) != size) {
    err_write_1("inspect_tag: buf_inspect_tag: expected ");
    err_inspect_sw_decimal(size);
    err_write_1(" got ");
    err_inspect_sw_decimal(r);
    err_write_1(", buffer :\n");
    err_inspect_buf(&buf);
    err_write_1("\n");
    assert(! "inspect_tag: buf_inspect_tag: error");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_tuple (const s_tuple *tuple, s_str *dest)
{
  s_buf buf;
  s_pretty pretty = {0};
  sw size;
  size = buf_inspect_tuple_size(&pretty, tuple);
  if (size < 0) {
    err_puts("inspect_tuple: buf_inspect_tuple_size");
    assert(! "inspect_tuple: buf_inspect_tuple_size");
    return NULL;
  }
  buf_init_alloc(&buf, size);
  buf_inspect_tuple(&buf, tuple);
  assert(buf.wpos == buf.size);
  if (buf.wpos != buf.size) {
    buf_clean(&buf);
    err_puts("inspect_tuple: buf_inspect_tuple");
    assert(! "inspect_tuple: buf_inspect_tuple");
    return NULL;
  }
  return buf_to_str(&buf, dest);
}

s_str * inspect_var (const s_var *var, s_str *dest)
{
  s_pretty pretty = {0};
  sw size;
  s_buf tmp;
  size = buf_inspect_var_size(&pretty, var);
  if (size < 0) {
    err_puts("inspect_var: buf_inspect_var_size error");
    assert(! "inspect_var: buf_inspect_var_size error");
    return NULL;
  }
  if (! buf_init_alloc(&tmp, size)) {
    err_puts("inspect_var: buf_init alloc");
    assert(! "inspect_var: buf_init_alloc");
    return NULL;
  }
  buf_inspect_var(&tmp, var);
  if (tmp.wpos != tmp.size) {
    err_puts("inspect_var: tmp.wpos != tmp.size");
    assert(! "inspect_var: tmp.wpos != tmp.size");
    buf_clean(&tmp);
    return NULL;
  }
  return buf_to_str(&tmp, dest);
}
