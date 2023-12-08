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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libtommath/tommath.h"
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_save.h"
#include "character.h"
#include "ident.h"
#include "integer.h"
#include "list.h"
#include "operator.h"
#include "str.h"
#include "tag.h"

sw buf_inspect_array_data (s_buf *buf, const s_array *array);
sw buf_inspect_array_data_rec (s_buf *buf, const s_array *array,
                               const u8 **data, f_buf_inspect inspect,
                               const s_tag **tag, uw *address,
                               uw dimension);
sw buf_inspect_array_data_size (const s_array *array);
sw buf_inspect_array_data_size_rec (const s_array *array,
                                    const u8 **data,
                                    f_buf_inspect_size inspect_size,
                                    const s_tag **tag,
                                    uw *address, uw dimension);
sw buf_inspect_tag_type (s_buf *buf, e_tag_type type);

sw buf_inspect_array (s_buf *buf, const s_array *array)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(array);
  if ((r = buf_inspect_paren_sym(buf, array->type)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    goto clean;
  result += r;
  if ((r = buf_inspect_array_data(buf, array)) < 0) {
    warnx("buf_inspect_array: buf_inspect_array_data");
    goto clean;
  }
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_array_data (s_buf *buf, const s_array *array)
{
  uw *address;
  u8 *data = NULL;
  f_buf_inspect inspect = NULL;
  s_tag *tag = NULL;
  sw r;
  assert(buf);
  assert(array);
  address = calloc(array->dimension, sizeof(uw));
  if (array->data) {
    data = array->data;
    inspect = sym_to_buf_inspect(array->type);
  }
  else 
    tag = array->tags;
  r = buf_inspect_array_data_rec(buf, array, (const u8 **) &data,
                                 inspect, (const s_tag **) &tag,
                                 address, 0);
  free(address);
  return r;
}

sw buf_inspect_array_data_rec (s_buf *buf, const s_array *array,
                               const u8 **data, f_buf_inspect inspect,
                               const s_tag **tag, uw *address,
                               uw dimension)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "{")) <= 0)
    goto clean;
  result += r;
  address[dimension] = 0;
  while (1) {
    if (dimension == array->dimension - 1) {
      if (*data) {
        if ((r = inspect(buf, *data)) <= 0)
          goto clean;
        result += r;
        *data += array->dimensions[dimension].item_size;
      }
      else if (*tag) {
        if ((r = buf_inspect_tag(buf, *tag)) <= 0)
          goto clean;
        result += r;
        (*tag)++;
      }
    }
    else {
      if ((r = buf_inspect_array_data_rec(buf, array, data, inspect,
                                          tag, address,
                                          dimension + 1)) <= 0)
        goto clean;
      result += r;
    }
    address[dimension]++;
    if (address[dimension] == array->dimensions[dimension].count)
      break;
    if ((r = buf_write_1(buf, ", ")) <= 0)
      goto clean;
    result += r;
  }
  if ((r = buf_write_1(buf, "}")) <= 0)
    goto clean;
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_array_data_size (const s_array *array)
{
  uw *address;
  u8 *data = NULL;
  f_buf_inspect_size inspect_size = NULL;
  s_tag *tag = NULL;
  sw r;
  assert(array);
  address = calloc(array->dimension, sizeof(uw));
  if (array->data) {
    data = array->data;
    inspect_size = sym_to_buf_inspect_size(array->type);
  }
  else 
    tag = array->tags;
  r = buf_inspect_array_data_size_rec(array, (const u8 **) &data,
                                      inspect_size,
                                      (const s_tag **) &tag,
                                      address, 0);
  free(address);
  return r;
}

sw buf_inspect_array_data_size_rec (const s_array *array,
                                    const u8 **data,
                                    f_buf_inspect_size inspect_size,
                                    const s_tag **tag,
                                    uw *address, uw dimension)
{
  sw r;
  sw result = 0;
  r = strlen("{");
  result += r;
  address[dimension] = 0;
  while (1) {
    if (dimension == array->dimension - 1) {
      if (*data) {
        if ((r = inspect_size(*data)) <= 0)
          goto clean;
        result += r;
        *data += array->dimensions[dimension].item_size;
      }
      else if (*tag) {
        if ((r = buf_inspect_tag_size(*tag)) <= 0)
          goto clean;
        result += r;
        (*tag)++;
      }
    }
    else {
      if ((r = buf_inspect_array_data_size_rec(array, data,
                                               inspect_size,
                                               tag, address,
                                               dimension + 1)) <= 0)
        goto clean;
      result += r;
    }
    address[dimension]++;
    if (address[dimension] == array->dimensions[dimension].count)
      break;
    r = strlen(", ");
    result += r;
  }
  r = strlen("}");
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_array_size (const s_array *array)
{
  sw r;
  sw result = 0;
  assert(array);
  if ((r = buf_inspect_paren_sym_size(array->type)) <= 0)
    goto clean;
  result += r;
  r = strlen(" ");
  result += r;
  if ((r = buf_inspect_array_data_size(array)) <= 0) {
    warnx("buf_inspect_array_size: buf_inspect_array_data");
    goto clean;
  }
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_bool (s_buf *buf, const bool *b)
{
  if (*b)
    return buf_write_1(buf, "true");
  return buf_write_1(buf, "false");
}

sw buf_inspect_bool_size (const bool *b)
{
  if (*b)
    return strlen("true");
  return strlen("false");
}

sw buf_inspect_call (s_buf *buf, const s_call *call)
{
  s8 op_precedence;
  sw r;
  sw result = 0;
  if (operator_find(&call->ident) &&
      operator_symbol(&call->ident) == sym_1("[]"))
    return buf_inspect_call_brackets(buf, call);
  if (call->ident.sym == sym_1("cast"))
    return buf_inspect_cast(buf, call);
  if (operator_find(&call->ident) &&
      operator_arity(&call->ident) == 1)
    return buf_inspect_call_op_unary(buf, call);
  if (operator_find(&call->ident) &&
      operator_arity(&call->ident) == 2 &&
      (op_precedence = operator_precedence(&call->ident)) > 0)
    return buf_inspect_call_op(buf, call, op_precedence);
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
  sw r;
  sw result = 0;
  result += strlen("(");
  while (args) {
    if ((r = buf_inspect_tag_size(&args->tag)) < 0)
      return r;
    result += r;
    if ((args = list_next(args))) {
      result += strlen(", ");
    }
  }
  result += strlen(")");
  return result;
}

sw buf_inspect_call_brackets (s_buf *buf, const s_call *call)
{
  s_array *address;
  s_tag *array;
  uw i = 0;
  s_list *next;
  sw r;
  sw result = 0;
  assert(buf);
  assert(call);
  assert(call->arguments);
  next = list_next(call->arguments);
  assert(next);
  assert(! list_next(next));
  assert(next->tag.type == TAG_ARRAY);
  address = &next->tag.data.array;
  assert(address->dimension == 1);
  array = &call->arguments->tag;
  if ((r = buf_inspect_tag(buf, array)) < 0)
    return r;
  result += r;
  while (i < address->dimensions[0].count) {
    if ((r = buf_write_1(buf, "[")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_uw(buf, ((uw *) address->data)
                            + i)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, "]")) < 0)
      return r;
    result += r;
    i++;
  }
  return result;
}

sw buf_inspect_call_op (s_buf *buf, const s_call *call, s8 op_precedence)
{
  s_ident ident;
  s_tag *left;
  bool paren;
  s8 precedence;
  sw r;
  sw result = 0;
  s_tag *right;
  left = &call->arguments->tag;
  right = &list_next(call->arguments)->tag;
  if (left->type == TAG_CALL &&
      operator_find(&left->data.call.ident) &&
      (precedence = operator_precedence(&left->data.call.ident))
      < op_precedence) {
    paren = true;
    if ((r = buf_write_1(buf, "(")) < 0)
      return r;
    result += r;
  }
  else
    paren = false;
  if ((r = buf_inspect_tag(buf, left)) < 0)
    return r;
  result += r;
  if (paren) {
    if ((r = buf_write_1(buf, ")")) < 0)
      return r;
    result += r;
  }
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if (! operator_ident(&call->ident, &ident))
    return -1;
  if ((r = buf_inspect_ident(buf, &ident)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if (right->type == TAG_CALL &&
      operator_find(&right->data.call.ident) &&
      (precedence = operator_precedence(&right->data.call.ident))
      < op_precedence) {
    paren = true;
    if ((r = buf_write_1(buf, "(")) < 0)
      return r;
    result += r;
  }
  else
    paren = false;
  if ((r = buf_inspect_tag(buf, right)) < 0)
    return r;
  result += r;
  if (paren) {
    if ((r = buf_write_1(buf, ")")) < 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_call_op_size (const s_call *call, s8 op_precedence)
{
  (void) call;
  (void) op_precedence;
  return -1;
}

sw buf_inspect_call_op_unary (s_buf *buf, const s_call *call)
{
  sw r;
  sw result = 0;
  s_ident tmp;
  assert(buf);
  assert(call);
  if (operator_symbol(&call->ident) == sym_1("()"))
    return buf_inspect_call_paren(buf, call);
  if (operator_ident(&call->ident, &tmp) != &tmp)
    return -1;
  if ((r = buf_inspect_ident(buf, &tmp)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag(buf, &call->arguments->tag)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_call_op_unary_size (const s_call *call)
{
  (void) call;
  return -1;
}

sw buf_inspect_call_paren (s_buf *buf, const s_call *call)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(call);
  if ((r = buf_write_1(buf, "(")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag(buf, &call->arguments->tag)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, ")")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_call_size (const s_call *call)
{
  s8 op_precedence;
  sw r;
  sw result = 0;
  if (operator_find(&call->ident) &&
      operator_arity(&call->ident) == 1)
    return buf_inspect_call_op_unary_size(call);
  if (operator_find(&call->ident) &&
      operator_arity(&call->ident) == 2 &&
      (op_precedence = operator_precedence(&call->ident)) > 0)
    return buf_inspect_call_op_size(call, op_precedence);
  if ((r = buf_inspect_ident_size(&call->ident)) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_call_args_size(call->arguments)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_cast (s_buf *buf, const s_call *call)
{
  s_tag *arg;
  const s_sym *module;
  sw r;
  sw result = 0;
  assert(buf);
  assert(call);
  assert(call->arguments);
  assert(! list_next(call->arguments));
  module = call->ident.module;
  if ((r = buf_inspect_paren_sym(buf, module)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  arg = &call->arguments->tag;
  if ((r = buf_inspect_tag(buf, arg)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_cast_size (const s_call *call)
{
  s_tag *arg;
  const s_sym *module;
  sw r;
  sw result = 0;
  assert(call);
  assert(call->arguments);
  assert(! list_next(call->arguments));
  module = call->ident.module;
  if ((r = buf_inspect_paren_sym_size(module)) <= 0)
    return r;
  result += r;
  result += strlen(" ");
  arg = &call->arguments->tag;
  if ((r = buf_inspect_tag_size(arg)) <= 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_paren_sym (s_buf *buf, const s_sym *sym)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(sym);
  if ((r = buf_write_1(buf, "(")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_inspect_sym(buf, sym)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_write_1(buf, ")")) <= 0)
    goto clean;
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_paren_sym_size (const s_sym *sym)
{
  sw r;
  sw result = 0;
  assert(sym);
  result += strlen("(");
  if ((r = buf_inspect_sym_size(sym)) <= 0)
    goto clean;
  result += r;
  result += strlen(")");
  r = result;
 clean:
  return r;
}

sw buf_inspect_cfn (s_buf *buf, const s_cfn *cfn)
{
  sw r;
  sw result = 0;
  assert(cfn);
  if ((r = buf_write_1(buf, "cfn ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_sym(buf, cfn->result_type)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_str(buf, &cfn->name->str)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_list_paren(buf, (const s_list **) &cfn->arg_types)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_cfn_size (const s_cfn *cfn)
{
  s_list *arg_type;
  sw r;
  sw result = 0;
  assert(cfn);
  if ((r = buf_inspect_str_size(&cfn->name->str)) < 0)
    return r;
  result += r;
  result += strlen("(");
  arg_type = cfn->arg_types;
  while (arg_type) {
    if ((r = buf_inspect_tag_size(&arg_type->tag)) < 0)
      return r;
    arg_type = list_next(arg_type);
    if (arg_type)
      result += strlen(", ");
  }
  result += strlen(")");
  return result;
}

sw buf_inspect_character (s_buf *buf, const character *c)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_write_1(buf, "'")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_str_character(buf, c)) <= 0)
    goto restore;
  result += r;
  if ((r = buf_write_1(buf, "'")) < 0)
    goto restore;
  result += r;
  r = result;
  goto clean;
 restore:
  buf_save_restore_wpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return result;
}

sw buf_inspect_character_size (const character *c)
{
  sw r;
  sw result = 0;
  result += strlen("'");
  if ((r = buf_inspect_str_character_size(c)) <= 0)
    return r;
  result += r;
  result += strlen("'");
  return result;
}

sw buf_inspect_f32 (s_buf *buf, const f32 *f)
{
  return buf_f(buf, "%g", *f);
}

sw buf_inspect_f32_size (const f32 *f)
{
  s8 b[16];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  return buf_inspect_f32(&buf, f);
}

sw buf_inspect_f64 (s_buf *buf, const f64 *f)
{
  return buf_f(buf, "%g", *f);
}

sw buf_inspect_f64_size (const f64 *f)
{
  s8 b[16];
  s_buf buf;
  buf_init(&buf, false, sizeof(b), b);
  return buf_inspect_f64(&buf, f);
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

sw buf_inspect_facts_spec (s_buf *buf, p_facts_spec spec)
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
    if ((r = buf_write_1(buf, ", ")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, spec[1])) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, "}")) < 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_fn (s_buf *buf, const s_fn *fn)
{
  const s_fn_clause *clause;
  sw r;
  sw result = 0;
  assert(buf);
  assert(fn);
  if ((r = buf_write_1(buf, "fn ")) < 0)
    return r;
  result += r;
  clause = fn->clauses;
  assert(clause);
  if (clause->next_clause) {
    if ((r = buf_write_1(buf, "{\n")) < 0)
      return r;
    result += r;
    while (clause) {
      if ((r = buf_write_1(buf, "  ")) < 0)
        return r;
      result += r;
      if ((r = buf_inspect_fn_clause(buf, clause)) < 0)
        return r;
      result += r;
      if ((r = buf_write_1(buf, "\n")) < 0)
        return r;
      result += r;
      clause = clause->next_clause;
    }
    if ((r = buf_write_1(buf, "}")) < 0)
      return r;
    result += r;
  }
  else {
    if ((r = buf_inspect_fn_clause(buf, clause)) < 0)
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
      if ((r = buf_write_1(buf, "\n  ")) < 0)
        return r;
      result += r;
    }
  }
  if ((r = buf_write_1(buf, "\n}")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_fn_algo_size (const s_list *algo)
{
  sw r;
  sw result = 0;
  if (! algo) {
    r = strlen("{}");
    result += r;
    return result;
  }
  if (! list_next(algo)) {
    r = strlen("{ ");
    result += r;
    if ((r = buf_inspect_tag_size(&algo->tag)) < 0)
      return r;
    result += r;
    r = strlen(" }");
    result += r;
    return result;
  }
  r = strlen("{\n  ");
  result += r;
  while (algo) {
    if ((r = buf_inspect_tag_size(&algo->tag)) < 0)
      return r;
    result += r;
    algo = list_next(algo);
    if (algo) {
      r = strlen(";\n  ");
      result += r;
    }
  }
  r = strlen("\n}");
  result += r;
  return result;
}

sw buf_inspect_fn_clause (s_buf *buf, const s_fn_clause *clause)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(clause);
  if ((r = buf_inspect_fn_pattern(buf, clause->pattern)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_fn_algo(buf, clause->algo)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_fn_clause_size (const s_fn_clause *clause)
{
  sw r;
  sw result = 0;
  assert(clause);
  if ((r = buf_inspect_fn_pattern_size(clause->pattern)) < 0)
    return r;
  result += r;
  r = strlen(" ");
  result += r;
  if ((r = buf_inspect_fn_algo_size(clause->algo)) < 0)
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

sw buf_inspect_fn_pattern_size (const s_list *pattern)
{
  sw r;
  sw result = 0;
  r = strlen("(");
  result += r;
  while (pattern) {
    if ((r = buf_inspect_tag_size(&pattern->tag)) < 0)
      return r;
    result += r;
    pattern = list_next(pattern);
    if (pattern) {
      r = strlen(", ");
      result += r;
    }
  }
  r = strlen(")");
  result += r;
  return result;
}

sw buf_inspect_fn_size (const s_fn *fn)
{
  const s_fn_clause *clause;
  sw r;
  sw result = 0;
  assert(fn);
  r = strlen("fn ");
  result += r;
  clause = fn->clauses;
  assert(clause);
  if (clause->next_clause) {
    r = strlen("{\n");
    result += r;
    while (fn) {
      r = strlen("  ");
      result += r;
      r = buf_inspect_fn_clause_size(clause);
      result += r;
      r = strlen("\n");
      result += r;
      clause = clause->next_clause;
    }
    r = strlen("}");
    result += r;
  }
  else {
    r = buf_inspect_fn_clause_size(clause);
    result += r;
  }
  return result;
}

sw buf_inspect_ident (s_buf *buf, const s_ident *ident)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(ident);
  result = 0;
  if (ident->module) {
    if ((r = buf_inspect_sym(buf, ident->module)) < 0)
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
  if (ident->module) {
    if ((r = buf_inspect_sym_size(ident->module)) < 0)
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

sw buf_inspect_list (s_buf *buf, const s_list **x)
{
  const s_list *i;
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "[")) <= 0)
    return r;
  result++;
  i = *x;
  while (i) {
    if ((r = buf_inspect_tag(buf, &i->tag)) < 0)
      return r;
    result += r;
    switch (i->next.type) {
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

sw buf_inspect_list_paren (s_buf *buf, const s_list **x)
{
  const s_list *i;
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "(")) <= 0)
    return r;
  result++;
  i = *x;
  while (i) {
    if ((r = buf_inspect_tag(buf, &i->tag)) < 0)
      return r;
    result += r;
    switch (i->next.type) {
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
  if ((r = buf_write_1(buf, ")")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_list_size (const s_list **list)
{
  const s_list *i;
  sw r;
  sw result = 0;
  result += strlen("[");
  i = *list;
  while (i) {
    if ((r = buf_inspect_tag_size(&i->tag)) < 0)
      return r;
    result += r;
    switch (i->next.type) {
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

sw buf_inspect_map (s_buf *buf, const s_map *map)
{
  uw i = 0;
  s_tag *k;
  sw r;
  sw result = 0;
  assert(buf);
  assert(map);
  if ((r = buf_write_1(buf, "%{")) < 0)
    return r;
  result += r;
  while (i < map->count) {
    k = map->key + i;
    if (k->type == TAG_SYM) {
      if ((r = buf_write_1(buf, k->data.sym->str.ptr.ps8)) < 0)
        return r;
      result += r;
      if ((r = buf_write_1(buf, ": ")) < 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_inspect_tag(buf, map->key + i)) < 0)
        return r;
      result += r;
      if ((r = buf_write_1(buf, " => ")) < 0)
        return r;
      result += r;
    }
    if ((r = buf_inspect_tag(buf, map->value + i)) < 0)
      return r;
    result += r;
    i++;
    if (i < map->count) {
      if ((r = buf_write_1(buf, ", ")) < 0)
        return r;
      result += r;
    }
  }
  if ((r = buf_write_1(buf, "}")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_map_size (const s_map *map)
{
  assert(map);
  assert(! "buf_inspect_map_size: not implemented");
  (void) map;
  return -1;
}

sw buf_inspect_ptag (s_buf *buf, const p_tag *ptag)
{
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "@0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal(buf, (uw *) ptag)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ptag_size (const p_tag *ptag)
{
  sw result = 0;
  (void) ptag;
  result += strlen("@0x");
  result += sizeof(uw) / 4;
  return result;
}

sw buf_inspect_ptr (s_buf *buf, const s_ptr *ptr)
{
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "(")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_sym(buf, ptr->type)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " *) ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal(buf, (uw *) ptr->p)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ptr_size (const s_ptr *ptr)
{
  sw r;
  sw result = 0;
  (void) ptr;
  result += strlen("(");
  if ((r = buf_inspect_sym_size(ptr->type)) < 0)
    return r;
  result += r;
  result += strlen(" *) ");
  result += sizeof(uw) / 4;
  return result;
}

sw buf_inspect_quote (s_buf *buf, const s_quote *quote)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_write_1(buf, "quote ")) < 0)
    goto clean;
  result += r;
  if ((r = buf_inspect_tag(buf, quote->tag)) < 0)
    goto restore;
  r = result;
  goto clean;
 restore:
  buf_save_restore_wpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_inspect_quote_size (const s_quote *quote)
{
  sw r;
  sw result = 0;
  result += strlen("quote ");
  if ((r = buf_inspect_tag_size(quote->tag)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_str (s_buf *buf, const s_str *str)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(str);
  if (str_has_reserved_characters(str))
    return buf_inspect_str_reserved(buf, str);
  buf_save_init(buf, &save);
  if ((r = buf_write_u8(buf, '"')) <= 0)
    goto clean;
  result += r;
  if ((r = buf_write_str(buf, str)) < 0)
    goto restore;
  result += r;
  if ((r = buf_write_u8(buf, '"')) <= 0)
    goto restore;
  result += r;
  r = result;
  goto clean;
 restore:
  buf_save_restore_wpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_inspect_str_byte (s_buf *buf, const u8 *byte)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_write_1(buf, "\\")) <= 0) {
    r = -1;
    goto clean;
  }
  result += r;
  if ((r = buf_write_1(buf, "x")) <= 0)
    goto restore;
  result += r;
  if ((r = buf_u8_to_hex(buf, byte)) != 2)
    goto restore;
  result += r;
  r = result;
  goto clean;
 restore:
  if (r >= 0)
    r = -1;
  buf_save_restore_wpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_inspect_str_byte_size (const u8 *byte)
{
  sw r;
  sw result = 0;
  (void) byte;
  r = strlen("\\");
  result += r;
  r = strlen("x");
  result += r;
  r = 2;
  result += r;
  return result;
}

sw buf_inspect_str_character (s_buf *buf, const character *c)
{
  s8 b[4];
  s_buf char_buf;
  int i;
  int j;
  sw r;
  sw result = 0;
  sw result1 = 0;
  s_buf_save save;
  if (! str_character_is_reserved(*c))
    return buf_write_character_utf8(buf, *c);
  buf_save_init(buf, &save);
  if ((r = buf_write_1(buf, "\\")) <= 0)
    goto restore;
  result += r;
  switch (*c) {
  case '\0': if ((r = buf_write_u8(buf, '0')) <= 0) goto restore; break;
  case '\n': if ((r = buf_write_u8(buf, 'n')) <= 0) goto restore; break;
  case '\r': if ((r = buf_write_u8(buf, 'r')) <= 0) goto restore; break;
  case '\t': if ((r = buf_write_u8(buf, 't')) <= 0) goto restore; break;
  case '\v': if ((r = buf_write_u8(buf, 'v')) <= 0) goto restore; break;
  case '\"': if ((r = buf_write_u8(buf, '"')) <= 0) goto restore; break;
  case '\'': if ((r = buf_write_u8(buf, '\'')) <= 0) goto restore; break;
  case '\\': if ((r = buf_write_u8(buf, '\\')) <= 0) goto restore; break;
  default:
    buf_init(&char_buf, false, sizeof(b), b);
    if ((r = buf_write_character_utf8(&char_buf, *c)) <= 0)
      goto restore;
    i = r - 1;
    j = 0;
    if ((r = buf_write_u8(buf, 'x')) != 1)
      goto restore;
    result1 += r;
    if ((r = buf_u8_to_hex(buf, &char_buf.ptr.pu8[j++])) != 2)
      goto restore;
    result1 += r;
    while (i--) {
      if ((r = buf_write_1(buf, "\\x")) != 2)
        goto restore;
      result1 += r;
      if ((r = buf_u8_to_hex(buf, &char_buf.ptr.pu8[j++])) != 2)
        goto restore;
      result1 += r;
    }
    r = result1;
  }
  result += r;
  r = result;
  goto clean;
 restore:
  if (r >= 0)
    r = -1;
  buf_save_restore_wpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_inspect_str_character_size (const character *c)
{
  sw csize;
  sw size;
  if (! str_character_is_reserved(*c))
    return character_utf8_size(*c);
  size = 0;
  switch (*c) {
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
    csize = character_utf8_size(*c);
    if (csize <= 0)
      return -1;
    size += csize * 4;
  }
  return size;
}

/* keep in sync with buf_inspect_str_reserved_size */
sw buf_inspect_str_reserved (s_buf *buf, const s_str *str)
{
  u8 byte;
  character c;
  sw r;
  sw result = 0;
  s_str s;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_write_1(buf, "\"")) <= 0) {
    if (! r)
      r = -1;
    goto clean;
  }
  result += r;
  s = *str;
  while (r) {
    if ((r = str_read_character_utf8(&s, &c)) < 0)
      goto restore;
    if (r) {
      if ((r = buf_inspect_str_character(buf, &c)) <= 0)
        goto restore;
      result += r;
    }
    else if ((r = str_read_u8(&s, &byte)) < 0)
      goto restore;
    else if (r) {
      if ((r = buf_inspect_str_byte(buf, &byte)) <= 0)
        goto restore;
      result += r;
    }
  }
  if ((r = buf_write_u8(buf, '"')) <= 0)
    goto restore;
  result += r;
  r = result;
  goto clean;
 restore:
  if (! r)
    r = -1;
  buf_save_restore_wpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

/* keep in sync with buf_inspect_str_reserved */
sw buf_inspect_str_reserved_size (const s_str *str)
{
  u8 byte;
  character c;
  sw r;
  sw result = 0;
  s_str s;
  r = strlen("\"");
  result += r;
  s = *str;
  while (r) {
    if ((r = str_read_character_utf8(&s, &c)) < 0)
      goto restore;
    if (r) {
      if ((r = buf_inspect_str_character_size(&c)) <= 0)
        goto restore;
      result += r;
    }
    else if ((r = str_read_u8(&s, &byte)) < 0)
      goto restore;
    else if (r) {
      r = buf_inspect_str_byte_size(&byte);
      result += r;
    }
  }
  r = strlen("\"");
  result += r;
  return result;
 restore:
  return -1;
}

sw buf_inspect_str_size (const s_str *str)
{
  const sw quote_size = strlen("\"");
  sw size;
  if (str_has_reserved_characters(str))
    return buf_inspect_str_reserved_size(str);
  size = str->size + 2 * quote_size;
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
  switch(tag->type) {
  case TAG_ARRAY:   return buf_inspect_array(buf, &tag->data.array);
  case TAG_BOOL:    return buf_inspect_bool(buf, &tag->data.bool);
  case TAG_CALL:    return buf_inspect_call(buf, &tag->data.call);
  case TAG_CFN:     return buf_inspect_cfn(buf, &tag->data.cfn);
  case TAG_CHARACTER:
    return buf_inspect_character(buf, &tag->data.character);
  case TAG_F32:     return buf_inspect_f32(buf, &tag->data.f32);
  case TAG_F64:     return buf_inspect_f64(buf, &tag->data.f64);
  case TAG_FACT:    return buf_inspect_fact(buf, &tag->data.fact);
  case TAG_FN:      return buf_inspect_fn(buf, &tag->data.fn);
  case TAG_IDENT:   return buf_inspect_ident(buf, &tag->data.ident);
  case TAG_INTEGER: return buf_inspect_integer(buf, &tag->data.integer);
  case TAG_LIST:
    return buf_inspect_list(buf, (const s_list **) &tag->data.list);
  case TAG_MAP:     return buf_inspect_map(buf, &tag->data.map);
  case TAG_PTAG:    return buf_inspect_ptag(buf, &tag->data.ptag);
  case TAG_PTR:     return buf_inspect_ptr(buf, &tag->data.ptr);
  case TAG_QUOTE:   return buf_inspect_quote(buf, &tag->data.quote);
  case TAG_S8:      return buf_inspect_s8(buf, &tag->data.s8);
  case TAG_S16:     return buf_inspect_s16(buf, &tag->data.s16);
  case TAG_S32:     return buf_inspect_s32(buf, &tag->data.s32);
  case TAG_S64:     return buf_inspect_s64(buf, &tag->data.s64);
  case TAG_SW:      return buf_inspect_sw(buf, &tag->data.sw);
  case TAG_STR:     return buf_inspect_str(buf, &tag->data.str);
  case TAG_SYM:     return buf_inspect_sym(buf, tag->data.sym);
  case TAG_TUPLE:   return buf_inspect_tuple(buf, &tag->data.tuple);
  case TAG_U8:      return buf_inspect_u8(buf, &tag->data.u8);
  case TAG_U16:     return buf_inspect_u16(buf, &tag->data.u16);
  case TAG_U32:     return buf_inspect_u32(buf, &tag->data.u32);
  case TAG_U64:     return buf_inspect_u64(buf, &tag->data.u64);
  case TAG_UW:      return buf_inspect_uw(buf, &tag->data.uw);
  case TAG_VAR:     return buf_inspect_var(buf, tag);
  case TAG_VOID:    return buf_inspect_void(buf, &tag);
  }
  assert(! "buf_inspect_tag: unknown tag type");
  errx(1, "buf_inspect_tag: unknown tag_type");
  return -1;
}

sw buf_inspect_tag_size (const s_tag *tag)
{
  assert(tag);
  switch(tag->type) {
  case TAG_ARRAY:    return buf_inspect_array_size(&tag->data.array);
  case TAG_BOOL:     return buf_inspect_bool_size(&tag->data.bool);
  case TAG_CALL:     return buf_inspect_call_size(&tag->data.call);
  case TAG_CFN:      return buf_inspect_cfn_size(&tag->data.cfn);
  case TAG_CHARACTER:
    return buf_inspect_character_size(&tag->data.character);
  case TAG_F32:      return buf_inspect_f32_size(&tag->data.f32);
  case TAG_F64:      return buf_inspect_f64_size(&tag->data.f64);
  case TAG_FACT:     return buf_inspect_fact_size(&tag->data.fact);
  case TAG_FN:       return buf_inspect_fn_size(&tag->data.fn);
  case TAG_IDENT:    return buf_inspect_ident_size(&tag->data.ident);
  case TAG_INTEGER:
    return buf_inspect_integer_size(&tag->data.integer);
  case TAG_LIST:
    return buf_inspect_list_size((const s_list **) &tag->data.list);
  case TAG_MAP:      return buf_inspect_map_size(&tag->data.map);
  case TAG_PTAG:     return buf_inspect_ptag_size(&tag->data.ptag);
  case TAG_PTR:      return buf_inspect_ptr_size(&tag->data.ptr);
  case TAG_QUOTE:    return buf_inspect_quote_size(&tag->data.quote);
  case TAG_S8:       return buf_inspect_s8_size(&tag->data.s8);
  case TAG_S16:      return buf_inspect_s16_size(&tag->data.s16);
  case TAG_S32:      return buf_inspect_s32_size(&tag->data.s32);
  case TAG_S64:      return buf_inspect_s64_size(&tag->data.s64);
  case TAG_SW:       return buf_inspect_sw_size(&tag->data.sw);
  case TAG_STR:      return buf_inspect_str_size(&tag->data.str);
  case TAG_SYM:      return buf_inspect_sym_size(tag->data.sym);
  case TAG_TUPLE:    return buf_inspect_tuple_size(&tag->data.tuple);
  case TAG_U8:       return buf_inspect_u8_size(&tag->data.u8);
  case TAG_U16:      return buf_inspect_u16_size(&tag->data.u16);
  case TAG_U32:      return buf_inspect_u32_size(&tag->data.u32);
  case TAG_U64:      return buf_inspect_u64_size(&tag->data.u64);
  case TAG_UW:       return buf_inspect_uw_size(&tag->data.uw);
  case TAG_VAR:      return buf_inspect_var_size(tag);
  case TAG_VOID:     return buf_inspect_void_size(tag);
  }
  assert(! "buf_inspect_tag_size: unknown tag type");
  errx(1, "buf_inspect_tag_size: unknown tag type");
  return -1;
}

sw buf_inspect_tag_type (s_buf *buf, e_tag_type type)
{
  switch(type) {
  case TAG_VOID:
    return buf_write_1(buf, "Void");
  case TAG_ARRAY:
    return buf_write_1(buf, "Array");
  case TAG_BOOL:
    return buf_write_1(buf, "Bool");
  case TAG_CALL:
    return buf_write_1(buf, "Call");
  case TAG_CFN:
    return buf_write_1(buf, "Cfn");
  case TAG_CHARACTER:
    return buf_write_1(buf, "Character");
  case TAG_F32:
    return buf_write_1(buf, "F32");
  case TAG_F64:
    return buf_write_1(buf, "F64");
  case TAG_FACT:
    return buf_write_1(buf, "Fact");
  case TAG_FN:
    return buf_write_1(buf, "Fn");
  case TAG_IDENT:
    return buf_write_1(buf, "Ident");
  case TAG_INTEGER:
    return buf_write_1(buf, "Integer");
  case TAG_LIST:
    return buf_write_1(buf, "List");
  case TAG_MAP:
    return buf_write_1(buf, "Map");
  case TAG_PTAG:
    return buf_write_1(buf, "Ptag");
  case TAG_PTR:
    return buf_write_1(buf, "Void*");
  case TAG_QUOTE:
    return buf_write_1(buf, "Quote");
  case TAG_S8:
    return buf_write_1(buf, "S8");
  case TAG_S16:
    return buf_write_1(buf, "S16");
  case TAG_S32:
    return buf_write_1(buf, "S32");
  case TAG_S64:
    return buf_write_1(buf, "S64");
  case TAG_SW:
    return buf_write_1(buf, "Sw");
  case TAG_STR:
    return buf_write_1(buf, "Str");
  case TAG_SYM:
    return buf_write_1(buf, "Sym");
  case TAG_TUPLE:
    return buf_write_1(buf, "Tuple");
  case TAG_U8:
    return buf_write_1(buf, "U8");
  case TAG_U16:
    return buf_write_1(buf, "U16");
  case TAG_U32:
    return buf_write_1(buf, "U32");
  case TAG_U64:
    return buf_write_1(buf, "U64");
  case TAG_UW:
    return buf_write_1(buf, "Uw");
  case TAG_VAR:
    return buf_write_1(buf, "Var");
  }
  assert(! "buf_inspect_tag_type: unknown tag type");
  errx(1, "buf_inspect_tag_type: unknown tag type");
  return -1;
}

sw buf_inspect_tag_type_size (e_tag_type type)
{
  switch(type) {
  case TAG_VOID:
    return strlen("Void");
  case TAG_ARRAY:
    return strlen("Array");
  case TAG_BOOL:
    return strlen("Bool");
  case TAG_CALL:
    return strlen("Call");
  case TAG_CFN:
    return strlen("Cfn");
  case TAG_CHARACTER:
    return strlen("Character");
  case TAG_F32:
    return strlen("F32");
  case TAG_F64:
    return strlen("F64");
  case TAG_FACT:
    return strlen("Fact");
  case TAG_FN:
    return strlen("Fn");
  case TAG_IDENT:
    return strlen("Ident");
  case TAG_INTEGER:
    return strlen("Integer");
  case TAG_LIST:
    return strlen("List");
  case TAG_MAP:
    return strlen("Map");
  case TAG_PTAG:
    return strlen("Ptag");
  case TAG_PTR:
    return strlen("Void*");
  case TAG_QUOTE:
    return strlen("Quote");
  case TAG_S8:
    return strlen("S8");
  case TAG_S16:
    return strlen("S16");
  case TAG_S32:
    return strlen("S32");
  case TAG_S64:
    return strlen("S64");
  case TAG_SW:
    return strlen("Sw");
  case TAG_STR:
    return strlen("Str");
  case TAG_SYM:
    return strlen("Sym");
  case TAG_TUPLE:
    return strlen("Tuple");
  case TAG_U8:
    return strlen("U8");
  case TAG_U16:
    return strlen("U16");
  case TAG_U32:
    return strlen("U32");
  case TAG_U64:
    return strlen("U64");
  case TAG_UW:
    return strlen("Uw");
  case TAG_VAR:
    return strlen("Var");
  }
  assert(! "buf_inspect_tag_type_size: unknown tag type");
  errx(1, "buf_inspect_tag_type_size: unknown tag type");
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

sw buf_inspect_var (s_buf *buf, const s_tag *var)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "var(0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal(buf, (uw *) var)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, ")")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_var_size (const s_tag *var)
{
  sw r;
  sw result = 0;
  r = strlen("var(0x");
  result += r;
  r = buf_inspect_uw_hexadecimal_size((uw *) var);
  result += r;
  r = strlen(")");
  result += r;
  return result;
}

sw buf_inspect_void (s_buf *buf, const void *_)
{
  sw r;
  sw result = 0;
  assert(buf);
  (void) _;
  if ((r = buf_write_1(buf, "void")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_void_size (const void *_)
{
  (void) _;
  return strlen("void");
}
