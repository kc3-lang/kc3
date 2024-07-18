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
#include <string.h>
#include "buf.h"
#include "buf_inspect.h"
#include "buf_parse.h"
#include "call.h"
#include "cfn.h"
#include "facts.h"
#include "facts_cursor.h"
#include "facts_with.h"
#include "facts_with_cursor.h"
#include "fn.h"
#include "ident.h"
#include "list.h"
#include "tag.h"

void call_clean (s_call *call)
{
  assert(call);
  list_delete_all(call->arguments);
  if (call->cfn)
    cfn_delete(call->cfn);
  if (call->fn)
    fn_delete(call->fn);
}

bool call_get (s_call *call, s_facts *facts)
{
  s_facts_cursor cursor;
  s_tag tag_cfn;
  s_tag tag_fn;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_macro;
  s_tag tag_module_name;
  s_tag tag_operator;
  s_tag tag_special_operator;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_tag tag_var;
  tag_init_1(    &tag_cfn,      ":cfn");
  tag_init_1(    &tag_fn,       ":fn");
  tag_init_ident(&tag_ident, &call->ident);
  tag_init_1(    &tag_is_a,     ":is_a");
  tag_init_1(    &tag_macro,    ":macro");
  tag_init_sym(  &tag_module_name, call->ident.module);
  tag_init_1(    &tag_operator, ":operator");
  tag_init_1(    &tag_special_operator, ":special_operator");
  tag_init_sym(  &tag_sym, call->ident.sym);
  tag_init_1(    &tag_symbol,   ":symbol");
  tag_init_var(  &tag_var);
  if (! facts_find_fact_by_tags(facts, &tag_module_name,
                                &tag_symbol, &tag_ident) &&
      ! facts_find_fact_by_tags(facts, &tag_module_name,
                                &tag_operator, &tag_ident)) {
    err_write_1("call_get: symbol ");
    err_write_1(call->ident.sym->str.ptr.ps8);
    err_write_1(" not found in module ");
    err_write_1(call->ident.module->str.ptr.ps8);
    err_write_1("\n");
    return false;
  }
  facts_with_tags(facts, &cursor, &tag_ident, &tag_fn, &tag_var);
  if (facts_cursor_next(&cursor)) {
    if (tag_var.type == TAG_FN)
      call->fn = fn_new_copy(&tag_var.data.fn);
    else {
      err_write_1("call_get: ");
      err_write_1(call->ident.module->str.ptr.ps8);
      err_write_1(".");
      err_write_1(call->ident.sym->str.ptr.ps8);
      err_puts(" is not a function");
      return false;
    }
  }
  facts_cursor_clean(&cursor);
  facts_with_tags(facts, &cursor, &tag_ident, &tag_cfn, &tag_var);
  if (facts_cursor_next(&cursor)) {
    if (tag_var.type == TAG_CFN)
      call->cfn = cfn_new_copy(&tag_var.data.cfn);
    else {
      err_write_1("call_get: ");
      err_write_1(call->ident.module->str.ptr.ps8);
      err_write_1(".");
      err_write_1(call->ident.sym->str.ptr.ps8);
      err_puts(" is not a C function");
      return false;
    }
  }
  facts_cursor_clean(&cursor);
  if (facts_find_fact_by_tags(facts, &tag_ident, &tag_is_a,
                              &tag_macro)) {
    if (call->fn)
      call->fn->macro = true;
    if (call->cfn)
      call->cfn->macro = true;
  }
  if (facts_find_fact_by_tags(facts, &tag_ident, &tag_is_a,
                              &tag_special_operator)) {
    if (call->fn)
      call->fn->special_operator = true;
    if (call->cfn)
      call->cfn->special_operator = true;
  }
  return true;
}

bool call_op_get (s_call *call, s_facts *facts)
{
  s_facts_with_cursor cursor;
  s_tag tag_cfn;
  s_tag tag_fn;
  s_tag tag_ident;
  s_tag tag_is_a;
  s_tag tag_macro;
  s_tag tag_module_name;
  s_tag tag_special_operator;
  s_tag tag_sym;
  s_tag tag_symbol;
  s_tag tag_var;
  tag_init_1(    &tag_cfn,      ":cfn");
  tag_init_1(    &tag_fn,       ":fn");
  tag_init_ident(&tag_ident, &call->ident);
  tag_init_1(    &tag_is_a,     ":is_a");
  tag_init_1(    &tag_macro,    ":macro");
  tag_init_sym(  &tag_module_name, call->ident.module);
  tag_init_1(    &tag_special_operator, ":special_operator");
  tag_init_sym(  &tag_sym, call->ident.sym);
  tag_init_1(    &tag_symbol,   ":symbol");
  tag_init_var(  &tag_var);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_module_name,
      &tag_symbol, &tag_ident,    /* module exports symbol */
      NULL, NULL });
  if (! facts_with_cursor_next(&cursor)) {
    err_write_1("call_get: symbol ");
    err_write_1(call->ident.sym->str.ptr.ps8);
    err_write_1(" not found in module ");
    err_write_1(call->ident.module->str.ptr.ps8);
    err_write_1("\n");
    facts_with_cursor_clean(&cursor);
    return false;
  }
  facts_with_cursor_clean(&cursor);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_fn, &tag_var,
      NULL, NULL });
  if (facts_with_cursor_next(&cursor)) {
    if (tag_var.type != TAG_FN) {
      err_write_1("call_get: ");
      err_write_1(call->ident.module->str.ptr.ps8);
      err_write_1(".");
      err_write_1(call->ident.sym->str.ptr.ps8);
      err_puts(" is not a function");
      facts_with_cursor_clean(&cursor);
      return false;
    }
    call->fn = fn_new_copy(&tag_var.data.fn);
  }
  facts_with_cursor_clean(&cursor);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_cfn, &tag_var,
      NULL, NULL });
  if (facts_with_cursor_next(&cursor)) {
    if (tag_var.type != TAG_CFN) {
      err_write_1("call_get: ");
      err_write_1(call->ident.module->str.ptr.ps8);
      err_write_1(".");
      err_write_1(call->ident.sym->str.ptr.ps8);
      err_puts(" is not a C function");
      facts_with_cursor_clean(&cursor);
      return false;
    }
    call->cfn = cfn_new_copy(&tag_var.data.cfn);
  }
  facts_with_cursor_clean(&cursor);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_is_a, &tag_macro, NULL, NULL });
  if (facts_with_cursor_next(&cursor)) {
    if (call->fn)
      call->fn->macro = true;
    if (call->cfn)
      call->cfn->macro = true;
  }
  facts_with_cursor_clean(&cursor);
  facts_with(facts, &cursor, (t_facts_spec) {
      &tag_ident, &tag_is_a, &tag_special_operator, NULL, NULL});
  if (facts_with_cursor_next(&cursor)) {
    if (call->fn)
      call->fn->special_operator = true;
    if (call->cfn)
      call->cfn->special_operator = true;
  }
  facts_with_cursor_clean(&cursor);
  return true;
}

s_call * call_init (s_call *call)
{
  assert(call);
  *call = (s_call) {0};
  return call;
}

s_call * call_init_1 (s_call *call, const s8 *p)
{
  s_buf buf;
  uw len;
  sw r;
  len = strlen(p);
  buf_init(&buf, false, len, (s8 *) p);
  buf.wpos = len;
  r = buf_parse_call(&buf, call);
  if (r < 0 || (uw) r != len) {
    err_write_1("call_init_1: invalid call: ");
    err_write_1(p);
    err_write_1(": ");
    err_inspect_sw(&r);
    err_write_1(" != ");
    err_inspect_uw(&len);
    err_write_1("\n");
    return NULL;
  }
  return call;
}

s_call * call_init_cast (s_call *call, const s_sym *type,
                         const s_tag *tag)
{
  s_call tmp = {0};
  assert(call);
  assert(type);
  assert(sym_is_module(type));
  assert(tag);
  ident_init(&tmp.ident, type, sym_1("cast"));
  tmp.arguments = list_new_copy(tag, NULL);
  *call = tmp;
  return call;
}

s_call * call_init_copy (s_call *call, const s_call *src)
{
  assert(src);
  assert(call);
  ident_init_copy(&call->ident, &src->ident);
  list_init_copy(&call->arguments, (const s_list **) &src->arguments);
  call->cfn = src->cfn;
  call->fn = src->fn;
  return call;
}

s_call * call_init_op (s_call *call)
{
  s_call tmp = {0};
  assert(call);
  tmp.arguments = list_new(list_new(NULL));
  *call = tmp;
  return call;
}

s_call * call_init_op_unary (s_call *call)
{
  s_call tmp = {0};
  assert(call);
  tmp.arguments = list_new(NULL);
  *call = tmp;
  return call;
}

s_str * call_inspect (const s_call *call, s_str *dest)
{
  sw size;
  s_buf tmp;
  size = buf_inspect_call_size(call);
  if (size < 0) {
    assert(! "error");
    return NULL;
  }
  buf_init_alloc(&tmp, size);
  buf_inspect_call(&tmp, call);
  assert(tmp.wpos == tmp.size);
  return buf_to_str(&tmp, dest);
}
