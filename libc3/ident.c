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
#include "assert.h"
#include "buf.h"
#include "buf_inspect.h"
#include "character.h"
#include "env.h"
#include "facts_with.h"
#include "facts_with_cursor.h"
#include "module.h"
#include "str.h"
#include "sym.h"
#include "tag.h"

bool ident_character_is_reserved (character c)
{
  return (character_is_space(c) ||
          c == '#' ||
          c == '(' ||
          c == ')' ||
          c == ',' ||
          c == '.' ||
          c == ';' ||
          c == '[' ||
          c == ']' ||
          c == '{' ||
          c == '}');
}

bool ident_first_character_is_reserved (character c)
{
  return (character_is_digit(c) ||
          character_is_uppercase(c) ||
          character_is_space(c) ||
          c == '"' ||
          c == '\'' ||
          c == '(' ||
          c == ')' ||
          c == ':' ||
          c == '[' ||
          c == ']' ||
          c == '{' ||
          c == '}');
}

s_tag * ident_get (const s_ident *ident, s_facts *facts, s_tag *dest)
{
  s_facts_with_cursor cursor;
  const s_sym *module;
  s_tag tag_cfn;
  s_tag tag_fn;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_macro;
  s_tag tag_module;
  s_tag tag_special_operator;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_tag tag_var;
  module = ident->module;
  if (! module)
    module = g_c3_env.current_module;
  if (! module_ensure_loaded(module, facts))
    return NULL;
  tag_init_1(    &tag_cfn,      ":cfn");
  tag_init_1(    &tag_fn,       ":fn");
  tag_init_ident(&tag_ident, ident);
  tag_init_1(    &tag_is_a,     ":is_a");
  tag_init_1(    &tag_macro,    ":macro");
  tag_init_1(    &tag_special_operator, ":special_operator");
  tag_init_sym(  &tag_sym, ident->sym);
  tag_init_sym(  &tag_module, module);
  tag_init_1(    &tag_symbol,   ":symbol");
  tag_init_var(  &tag_var);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_module,
      &tag_symbol, &tag_ident,    /* module exports symbol */
      NULL, NULL });
  if (! facts_with_cursor_next(&cursor)) {
    facts_with_cursor_clean(&cursor);
    return NULL;
  }
  facts_with_cursor_clean(&cursor);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_cfn, &tag_var,
      NULL, NULL });
  if (facts_with_cursor_next(&cursor)) {
    if (tag_var.type != TAG_CFN) {
      err_write_1("call_get: ");
      err_write_1(module->str.ptr.pchar);
      err_write_1(".");
      err_write_1(ident->sym->str.ptr.pchar);
      err_puts(" is not a C function");
      facts_with_cursor_clean(&cursor);
      return NULL;
    }
  }
  facts_with_cursor_clean(&cursor);
  if (tag_var.type == TAG_VAR) {
    facts_with(facts, &cursor, (t_facts_spec) {
        &tag_ident, &tag_fn, &tag_var,
        NULL, NULL });
    if (facts_with_cursor_next(&cursor)) {
      if (tag_var.type != TAG_FN) {
        err_write_1("call_get: ");
        err_write_1(module->str.ptr.pchar);
        err_write_1(".");
        err_write_1(ident->sym->str.ptr.pchar);
        err_puts(" is not a function");
        facts_with_cursor_clean(&cursor);
        return NULL;
      }
      facts_with_cursor_clean(&cursor);
    }
  }
  if (tag_var.type == TAG_VAR) {
    /*
    warnx("Neither fn nor cfn");
    */
    return NULL;
  }
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_is_a, &tag_macro, NULL, NULL });
  if (facts_with_cursor_next(&cursor)) {
    if (tag_var.type == TAG_CFN)
      tag_var.data.cfn.macro = true;
    else if (tag_var.type == TAG_FN)
      tag_var.data.fn.macro = true;
  }
  facts_with_cursor_clean(&cursor);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_is_a, &tag_special_operator, NULL, NULL});
  if (facts_with_cursor_next(&cursor)) {
    if (tag_var.type == TAG_CFN)
      tag_var.data.cfn.special_operator = true;
    else if (tag_var.type == TAG_FN)
      tag_var.data.fn.special_operator = true;
  }
  facts_with_cursor_clean(&cursor);
  *dest = tag_var;
  return dest;
}

bool ident_has_reserved_characters (const s_ident *ident)
{
  character c;
  sw r;
  s_str stra;
  str_init(&stra, NULL, ident->sym->str.size, ident->sym->str.ptr.p);
  if ((r = str_read_character_utf8(&stra, &c)) > 0) {
    if (ident_first_character_is_reserved(c))
      return true;
    while ((r = str_read_character_utf8(&stra, &c)) > 0) {
      if (ident_character_is_reserved(c))
        return true;
    }
  }
  if (r < 0)
    return true;
  return false;
}

s_ident * ident_init (s_ident *ident, const s_sym *module,
                      const s_sym *sym)
{
  assert(ident);
  assert(sym);
  ident->module = module;
  ident->sym = sym;
  return ident;
}

s_ident * ident_init_1 (s_ident *ident, const char *p)
{
  s_str tmp;
  str_init_1(&tmp, NULL, p);
  str_to_ident(&tmp, ident);
  return ident;
}

s_ident * ident_init_cast (s_ident *ident, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_IDENT:
    *ident = tag->data.ident;
    return ident;
  default:
    break;
  }
  err_write_1("ident_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Ident");
  assert(! "ident_init_cast: cannot cast to Ident");
  return NULL;
}

s_ident * ident_init_copy (s_ident *ident, const s_ident *src)
{
  ident->module = src->module;
  ident->sym = src->sym;
  return ident;
}

s_str * ident_inspect (const s_ident *ident, s_str *dest)
{
  sw r;
  sw size;
  s_buf buf;
  size = buf_inspect_ident_size(ident);
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

bool ident_is_special_operator (const s_ident *ident)
{
  return env_ident_is_special_operator(&g_c3_env, ident);
}

void ident_resolve_module (s_ident *ident)
{
  env_ident_resolve_module(&g_c3_env, ident);
}

bool ident_to_tag_type (const s_ident *ident, e_tag_type *dest)
{
  assert(ident);
  assert(dest);
  return sym_to_tag_type(ident->sym, dest);
}
