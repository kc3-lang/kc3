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
#include <float.h>
#include <math.h>
#include <string.h>
#include "../libtommath/tommath.h"
#include "alloc.h"
#include "assert.h"
#include "array.h"
#include "buf.h"
#include "buf_inspect.h"
#include "buf_inspect_s32_decimal.h"
#include "buf_inspect_s64_decimal.h"
#include "buf_save.h"
#include "call.h"
#include "character.h"
#include "compare.h"
#include "cow.h"
#include "data.h"
#include "ident.h"
#include "integer.h"
#include "io.h"
#include "list.h"
#include "op.h"
#include "ops.h"
#include "plist.h"
#include "pretty.h"
#include "special_operator.h"
#include "str.h"
#include "tag.h"
#include "ucd.h"

const s_sym *g_buf_inspect_type = NULL;

sw buf_inspect_array_data (s_buf *buf, const s_array *array);
sw buf_inspect_array_data_rec (s_buf *buf, const s_array *array,
                               const u8 **data, const s_tag **tag,
                               uw *address, uw dimension);
sw buf_inspect_array_data_size (s_pretty *pretty, const s_array *array);
sw buf_inspect_array_data_size_rec (s_pretty *pretty,
                                    const s_array *array,
                                    const u8 **data,
                                    const s_tag **tag,
                                    uw *address, uw dimension);
sw buf_inspect_tag_type (s_buf *buf, e_tag_type type);

sw buf_inspect_array (s_buf *buf, const s_array *array)
{
  const s_sym *buf_inspect_type;
  sw r;
  sw result = 0;
  assert(buf);
  assert(array);
  assert(sym_is_array_type(array->array_type));
  if ((r = buf_inspect_paren_sym(buf, array->array_type)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    goto clean;
  result += r;
  if (! array->dimension_count) {
    if ((r = buf_write_1(buf, "{}")) < 0)
      goto clean;
    result += r;
    r = result;
    goto clean;
  }
  buf_inspect_type = g_buf_inspect_type;
  g_buf_inspect_type = sym_array_type(array->array_type);
  if ((r = buf_inspect_array_data(buf, array)) < 0) {
    err_puts("buf_inspect_array: buf_inspect_array_data");
    goto clean;
  }
  result += r;
  g_buf_inspect_type = buf_inspect_type;
  r = result;
 clean:
  return r;
}

sw buf_inspect_array_data (s_buf *buf, const s_array *array)
{
  uw *address;
  u8 *data = NULL;
  s_tag *tag = NULL;
  sw r;
  assert(buf);
  assert(array);
  if (! array->dimension_count)
    return buf_write_1(buf, "{}");
  if (array->data)
    data = array->data;
  else 
    tag = array->tags;
  address = alloc(array->dimension_count * sizeof(uw));
  if (! address)
    return -1;
  r = buf_inspect_array_data_rec(buf, array, (const u8 **) &data,
                                 (const s_tag **) &tag, address, 0);
  free(address);
  return r;
}

sw buf_inspect_array_data_rec (s_buf *buf, const s_array *array,
                               const u8 **data, const s_tag **tag,
                               uw *address, uw dimension)
{
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "{")) <= 0)
    goto clean;
  result += r;
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent_from_column(&buf->pretty, 0);
  address[dimension] = 0;
  while (1) {
    if (dimension == array->dimension_count - 1) {
      if (*data) {
        if ((r = data_buf_inspect(buf, array->element_type,
                                  *data)) <= 0)
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
      if ((r = buf_inspect_array_data_rec(buf, array, data, tag,
                                          address, dimension + 1)) <= 0)
        goto clean;
      result += r;
    }
    address[dimension]++;
    if (address[dimension] == array->dimensions[dimension].count)
      break;
    if (dimension == array->dimension_count - 1) {
      if ((r = buf_write_1(buf, ", ")) <= 0)
        goto clean;
    }
    else {
      if ((r = buf_write_1(buf, ",\n")) <= 0)
        goto clean;
    }
    result += r;
  }
  pretty_save_clean(&pretty_save, &buf->pretty);
  if ((r = buf_write_1(buf, "}")) <= 0)
    goto clean;
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_array_data_size (s_pretty *pretty, const s_array *array)
{
  uw *address;
  const s_sym *buf_inspect_type_save;
  u8 *data = NULL;
  s_tag *tag = NULL;
  sw r;
  assert(array);
  if (! array->dimension_count)
    return buf_write_1_size(pretty, "{}");
  if (array->data)
    data = array->data;
  else 
    tag = array->tags;
  address = alloc(array->dimension_count * sizeof(uw));
  if (! address)
    return -1;
  buf_inspect_type_save = g_buf_inspect_type;
  g_buf_inspect_type = sym_array_type(array->array_type);
  r = buf_inspect_array_data_size_rec(pretty, array,
                                      (const u8 **) &data,
                                      (const s_tag **) &tag,
                                      address, 0);
  g_buf_inspect_type = buf_inspect_type_save;
  free(address);
  return r;
}

sw buf_inspect_array_data_size_rec (s_pretty *pretty,
                                    const s_array *array,
                                    const u8 **data,
                                    const s_tag **tag,
                                    uw *address, uw dimension)
{
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, "{")) < 0)
    return r;
  result += r;
  pretty_save_init(&pretty_save, pretty);
  pretty_indent_from_column(pretty, 0);
  address[dimension] = 0;
  while (1) {
    if (dimension == array->dimension_count - 1) {
      if (*data) {
        if ((r = data_buf_inspect_size(pretty, array->element_type,
                                       *data)) <= 0)
          goto clean;
        result += r;
        *data += array->dimensions[dimension].item_size;
      }
      else if (*tag) {
        if ((r = buf_inspect_tag_size(pretty, *tag)) <= 0)
          goto clean;
        result += r;
        (*tag)++;
      }
    }
    else {
      if ((r = buf_inspect_array_data_size_rec(pretty, array, data, tag,
                                               address,
                                               dimension + 1)) <= 0)
        goto clean;
      result += r;
    }
    address[dimension]++;
    if (address[dimension] == array->dimensions[dimension].count)
      break;
    if (dimension == array->dimension_count - 1) {
      if ((r = buf_write_1_size(pretty, ", ")) < 0)
        goto clean;
    }
    else {
      if ((r = buf_write_1_size(pretty, ",\n")) < 0)
        goto clean;
    }
    result += r;
  }
  pretty_save_clean(&pretty_save, pretty);
  if ((r = buf_write_1_size(pretty, "}")) < 0)
    goto clean;
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_array_size (s_pretty *pretty, const s_array *array)
{
  sw r;
  sw result = 0;
  assert(array);
  if ((r = buf_inspect_paren_sym_size(pretty, array->array_type)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_write_1_size(pretty, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_array_data_size(pretty, array)) <= 0) {
    err_puts("buf_inspect_array_size: buf_inspect_array_data");
    goto clean;
  }
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_binding (s_buf *buf, const s_binding *binding)
{
  const s_binding *b;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  assert(buf);
  pretty_save_init(&pretty_save, &buf->pretty);
  if ((r = buf_write_1(buf, "[")) < 0)
    goto clean;
  result += r;
  pretty_indent_from_column(&buf->pretty, 0);
  b = binding;
  while (b) {
    if ((r = buf_inspect_ident_sym(buf, b->name)) < 0)
      goto clean;
    result += r;
    if ((r = buf_write_1(buf, ": ")) < 0)
      goto clean;
    result += r;
    if ((r = buf_inspect_tag(buf, &b->value)) < 0)
      goto clean;
    result += r;
    b = b->next;
    if (b) {
      if ((r = buf_write_1(buf, ",\n")) < 0)
        goto clean;
      result += r;
    }
  }
  if ((r = buf_write_1(buf, "]")) < 0)
    goto clean;
  result += r;
  pretty_save_clean(&pretty_save, &buf->pretty);
  return result;
 clean:
  pretty_save_clean(&pretty_save, &buf->pretty);
  return r;
}

sw buf_inspect_do_block (s_buf *buf, const s_do_block *do_block)
{
  s_pretty_save pretty_save;
  u64 i = 0;
  sw r;
  sw result = 0;
  if (! do_block->count) {
    if (do_block->short_form)
      return buf_write_1(buf, "{}");
    else
      return buf_write_1(buf, "do end");
  }
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent(&buf->pretty, PRETTY_INDENT);
  if (do_block->short_form) {
    if ((r = buf_write_1(buf, "{ ")) < 0)
      return r;
  }
  else {
    if ((r = buf_write_1(buf, "do\n")) < 0)
      return r;
  }
  result += r;
  while (i < do_block->count - 1) {
    if ((r = buf_inspect_tag(buf, do_block->tag + i)) < 0)
      return r;
    result += r;
    if (do_block->short_form) {
      if ((r = buf_write_1(buf, "; ")) < 0)
        return r;
    }
    else {
      if ((r = buf_write_1(buf, "\n")) < 0)
        return r;
    }
    result += r;
    i++;
  }
  if ((r = buf_inspect_tag(buf, do_block->tag + i)) < 0)
    return r;
  result += r;
  pretty_save_clean(&pretty_save, &buf->pretty);
  if (do_block->short_form) {
    if ((r = buf_write_1(buf, " }")) < 0)
      return r;
  }
  else {
    if ((r = buf_write_1(buf, "\nend")) < 0)
      return r;
  }
  result += r;
  return result;
}

sw buf_inspect_do_block_inner (s_buf *buf, const s_do_block *do_block)
{
  u64 i = 0;
  sw r;
  sw result = 0;
  if (! do_block->count)
    return 0;
  if (do_block->short_form) {    
    if ((r = buf_write_1(buf, " ")) < 0)
      return r;
  }
  else {
    if ((r = buf_write_1(buf, "\n")) < 0)
      return r;
  }
  result += r;
  while (i < do_block->count - 1) {
    if ((r = buf_inspect_tag(buf, do_block->tag + i)) < 0)
      return r;
    result += r;
    if (do_block->short_form) {
      if ((r = buf_write_1(buf, "; ")) < 0)
        return r;
    }
    else {
      if ((r = buf_write_1(buf, "\n")) < 0)
        return r;
    }
    result += r;
    i++;
  }
  if ((r = buf_inspect_tag(buf, do_block->tag + i)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_do_block_inner_size (s_pretty *pretty,
                                    const s_do_block *do_block)
{
  u64 i = 0;
  sw r;
  sw result = 0;
  if (! do_block->count)
    return 0;
  if (do_block->short_form) {    
    if ((r = buf_write_1_size(pretty, " ")) < 0)
      return r;
  }
  else {
    if ((r = buf_write_1_size(pretty, "\n")) < 0)
      return r;
  }
  result += r;
  while (i < do_block->count - 1) {
    if ((r = buf_inspect_tag_size(pretty, do_block->tag + i)) < 0)
      return r;
    result += r;
    if (do_block->short_form) {
      if ((r = buf_write_1_size(pretty, "; ")) < 0)
        return r;
    }
    else {
      if ((r = buf_write_1_size(pretty, "\n")) < 0)
        return r;
    }
    result += r;
    i++;
  }
  if ((r = buf_inspect_tag_size(pretty, do_block->tag + i)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_do_block_size (s_pretty *pretty,
                              const s_do_block *do_block)
{
  s_pretty_save pretty_save;
  u64 i = 0;
  sw r;
  sw result = 0;
  if (! do_block->count) {
    if (do_block->short_form)
      return buf_write_1_size(pretty, "{}");
    else
      return buf_write_1_size(pretty, "do end");
  }
  pretty_save_init(&pretty_save, pretty);
  pretty_indent(pretty, PRETTY_INDENT);
  if (do_block->short_form) {    
    if ((r = buf_write_1_size(pretty, "{ ")) < 0)
      return r; 
  }
  else {
    if ((r = buf_write_1_size(pretty, "do\n")) < 0)
      return r;
  }
  result += r;
  while (i < do_block->count - 1) {
    if ((r = buf_inspect_tag_size(pretty, do_block->tag + i)) < 0)
      return r;
    result += r;
    if (do_block->short_form) {
      if ((r = buf_write_1_size(pretty, "; ")) < 0)
        return r;
    }
    else {
      if ((r = buf_write_1_size(pretty, "\n")) < 0)
        return r;
    }
    result += r;
    i++;
  }
  if ((r = buf_inspect_tag_size(pretty, do_block->tag + i)) < 0)
    return r;
  result += r;
  pretty_save_clean(&pretty_save, pretty);
  if (do_block->short_form) {
    if ((r = buf_write_1_size(pretty, " }")) < 0)
      return r;
  }
  else {
    if ((r = buf_write_1_size(pretty, "\nend")) < 0)
      return r;
  }
  result += r;
  return result;
}

sw buf_inspect_bool (s_buf *buf, bool b)
{
  if (b)
    return buf_write_1(buf, "true");
  return buf_write_1(buf, "false");
}

sw buf_inspect_bool_size (s_pretty *pretty, bool b)
{
  assert(pretty);
  if (b)
    return buf_write_1_size(pretty, "true");
  return buf_write_1_size(pretty, "false");
}

sw buf_inspect_buf (s_buf *buf, const s_buf *src)
{
  uw pos;
  sw r;
  sw result = 0;
  sw size;
  s_str str = {0};
  pos = (src->rpos < BUF_INSPECT_BUF_SIZE) ? 0 :
    src->rpos - BUF_INSPECT_BUF_SIZE;
  str.ptr.pchar = src->ptr.pchar + pos;
  str.size = src->rpos - pos;
  if ((r = buf_inspect_str(buf, &str)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n---HERE---\n")) <= 0)
    return r;
  result += r;
  size = src->wpos - src->rpos;
  size = (size < BUF_INSPECT_BUF_SIZE) ? size : BUF_INSPECT_BUF_SIZE;
  str.ptr.pchar = src->ptr.pchar + src->rpos;
  str.size = size;
  if ((r = buf_inspect_str(buf, &str)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "\n")) <= 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_buf_size (s_pretty *pretty, const s_buf *src)
{
  uw pos;
  sw r;
  sw result = 0;
  sw size;
  s_str str = {0};
  pos = (src->rpos < BUF_INSPECT_BUF_SIZE) ? 0 :
    src->rpos - BUF_INSPECT_BUF_SIZE;
  str.ptr.pchar = src->ptr.pchar + pos;
  str.size = src->rpos - pos;
  if ((r = buf_inspect_str_size(pretty, &str)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, "\n---HERE---\n")) <= 0)
    return r;
  result += r;
  size = src->wpos - src->rpos;
  size = (size < BUF_INSPECT_BUF_SIZE) ? size : BUF_INSPECT_BUF_SIZE;
  str.ptr.pchar = src->ptr.pchar + src->rpos;
  str.size = size;
  if ((r = buf_inspect_str_size(pretty, &str)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, "\n")) <= 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_call (s_buf *buf, const s_call *call)
{
  sw arity;
  bool b;
  s_op *op;
  s_tag op_tag = {0};
  s_ops *ops = NULL;
  sw r;
  sw result = 0;
  if (call->ident.module == &g_sym_KC3) {
    if (call->ident.sym == &g_sym_access)
      return buf_inspect_call_access(buf, call);
    if (call->ident.sym == &g_sym_if_then_else)
      return buf_inspect_call_if_then_else(buf, call);
    if (call->ident.sym == &g_sym_match)
      return buf_inspect_match(buf, call);
  }
  if ((! call->ident.module ||
       call->ident.module == &g_sym_KC3) &&
      call->ident.sym == &g_sym_str &&
      call->arguments &&
      call->arguments->tag.type == TAG_PLIST)
    return buf_inspect_call_str(buf, call);
  if (call->ident.sym == &g_sym_cast)
    return buf_inspect_cast(buf, call);
  arity = call_arity(call);
  if (arity > 0 && arity <= 2) {
    ops = env_global()->ops;
    if (ops_get_tag(ops, call->ident.sym, arity, &op_tag)) {
      if (op_tag.type != TAG_PSTRUCT ||
          ! op_tag.data.pstruct ||
          op_tag.data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
        err_puts("buf_inspect_call: ops_get did not return a valid"
                 " %KC3.Op{}");
        assert(!("buf_inspect_call: ops_get did not return a valid"
                 " %KC3.Op{}"));
        return -1;
      }
      op = op_tag.data.pstruct->data;
      if (op->sym == &g_sym__brackets) {
        tag_clean(&op_tag);
        return buf_inspect_call_brackets(buf, call);
      }
      if (arity == 1) {
        tag_clean(&op_tag);
        return buf_inspect_call_op_unary(buf, call);
      }
      if (arity == 2 && op->precedence) {
        r = buf_inspect_call_op(buf, call, op->precedence);
        tag_clean(&op_tag);
        if (r < 0) {
          err_puts("buf_inspect_call: buf_inspect_call_op");
          assert(! "buf_inspect_call: buf_inspect_call_op");
        }
        return r;
      }
      tag_clean(&op_tag);
    }
  }
  if (! ident_is_special_operator(&call->ident, &b))
    return -1;
  if (b)
    return buf_inspect_call_special_operator(buf, call);
  if ((r = buf_inspect_ident(buf, &call->ident)) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_call_args(buf, call->arguments)) < 0)
    return r;
   result += r;
  return result;
}

sw buf_inspect_call_access (s_buf *buf, const s_call *call)
{
  s_list *args;
  sw r;
  sw result = 0;
  s_list *key;
  assert(buf);
  assert(call);
  args = call->arguments;
  if ((r = buf_inspect_tag(buf, &args->tag)) < 0)
    return r;
  result += r;
  key = list_next(args);
  if (key->tag.type != TAG_PLIST)
    goto invalid_key;
  key = key->tag.data.plist;
  while (key) {
    if (key->tag.type != TAG_PSYM)
      goto invalid_key;
    if ((r = buf_write_1(buf, ".")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_ident_sym(buf, key->tag.data.psym)) < 0)
      return r;
    result += r;
    key = list_next(key);
  }
  return result;
 invalid_key:
  err_write_1("buf_inspect_call_access: invalid key: ");
  err_inspect_tag(&list_next(args)->tag);
  err_write_1("\n");
  assert(! "buf_inspect_call_access: invalid key");
  return -1;
}

sw buf_inspect_call_access_size (s_pretty *pretty, const s_call *call)
{
  s_list *args;
  sw r;
  sw result = 0;
  s_tag *tag_sym;
  assert(call);
  args = call->arguments;
  tag_sym = &args->next.data.plist->tag;
  if ((r = buf_inspect_tag_size(pretty, &args->tag)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, ".")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_ident_sym_size(pretty, tag_sym->data.psym)) < 0)
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

sw buf_inspect_call_args_size (s_pretty *pretty, const s_list *args)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, "(")) < 0)
    return r;
  result += r;
  while (args) {
    if ((r = buf_inspect_tag_size(pretty, &args->tag)) < 0)
      return r;
    result += r;
    if ((args = list_next(args))) {
      if ((r = buf_write_1_size(pretty, ", ")) < 0)
        return r;
      result += r;
    }
  }
  if ((r = buf_write_1_size(pretty, ")")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_call_brackets (s_buf *buf, const s_call *call)
{
  s_list *key;
  s_list *next;
  sw r;
  sw result = 0;
  s_tag *tag;
  assert(buf);
  assert(call);
  assert(call->arguments);
  next = list_next(call->arguments);
  assert(next);
  assert(! list_next(next));
  assert(next->tag.type == TAG_PLIST);
  key = next->tag.data.plist;
  tag = &call->arguments->tag;
  if ((r = buf_inspect_tag(buf, tag)) < 0)
    return r;
  result += r;
  while (key) {
    if ((r = buf_write_1(buf, "[")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, &key->tag)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, "]")) < 0)
      return r;
    result += r;
    key = list_next(key);
  }
  return result;
}

sw buf_inspect_call_brackets_size (s_pretty *pretty, const s_call *call)
{
  s_array *address;
  s_tag *array;
  uw i = 0;
  s_list *next;
  sw r;
  sw result = 0;
  assert(call);
  assert(call->arguments);
  next = list_next(call->arguments);
  assert(next);
  assert(! list_next(next));
  assert(next->tag.type == TAG_ARRAY);
  address = &next->tag.data.array;
  assert(address->dimension_count == 1);
  array = &call->arguments->tag;
  if ((r = buf_inspect_tag_size(pretty, array)) < 0)
    return r;
  result += r;
  while (i < address->dimensions[0].count) {
    if ((r = buf_write_1_size(pretty, "[")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_uw_size(pretty,
                                 ((uw *) address->data)[i])) < 0)
      return r;
    result += r;
    if ((r = buf_write_1_size(pretty, "]")) < 0)
      return r;
    result += r;
    i++;
  }
  return result;
}

sw buf_inspect_call_if_then_else (s_buf *buf, const s_call *call)
{
  s_tag *condition;
  s_tag *else_;
  uw i;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  s_tag *then;
  if (call->ident.module != &g_sym_KC3 ||
      call->ident.sym != &g_sym_if_then_else ||
      list_length(call->arguments) != 3)
    return -2;
  if ((r = buf_write_1(buf, "if ")) < 0)
    return r;
  result += r;
  condition = &call->arguments->tag;
  if ((r = buf_inspect_tag(buf, condition)) < 0)
    return r;
  result += r;
  then = &list_next(call->arguments)->tag;
  pretty_save_init(&pretty_save, &buf->pretty);
  if (then->type == TAG_DO_BLOCK) {
    if ((r = buf_write_1(buf, " do")) < 0)
      return r;
    result += r;
    pretty_indent(&buf->pretty, PRETTY_INDENT);
    i = 0;
    while (i < then->data.do_block.count) {
      if ((r = buf_write_1(buf, "\n")) < 0)
        return r;
      result += r;
      if ((r = buf_inspect_tag(buf, then->data.do_block.tag + i)) < 0)
        return r;
      i++;
    }
  }
  else
    if ((r = buf_inspect_tag(buf, then)) < 0)
      return r;
  result += r;
  pretty_save_clean(&pretty_save, &buf->pretty);
  else_ = &list_next(list_next(call->arguments))->tag;
  if (else_->type != TAG_VOID) {
    if ((r = buf_write_1(buf, "\nelse")) < 0)
      return r;
    result += r;
    pretty_save_init(&pretty_save, &buf->pretty);
    pretty_indent(&buf->pretty, PRETTY_INDENT);
    if (then->type == TAG_DO_BLOCK) {
      if ((r = buf_inspect_do_block_inner(buf,
                                          &else_->data.do_block)) < 0)
        return r;
    }
    else
      if ((r = buf_inspect_tag(buf, else_)) < 0)
        return r;
    result += r;
  }
  pretty_save_clean(&pretty_save, &buf->pretty);
  if ((r = buf_write_1(buf, "\nend")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_call_if_then_else_size (s_pretty *pretty,
                                       const s_call *call)
{
  s_tag *condition;
  s_tag *else_;
  uw i;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  s_tag *then;
  if (call->ident.module != &g_sym_KC3 ||
      call->ident.sym != &g_sym_if_then_else ||
      list_length(call->arguments) != 3)
    return -2;
  if ((r = buf_write_1_size(pretty, "if ")) < 0)
    return r;
  result += r;
  condition = &call->arguments->tag;
  if ((r = buf_inspect_tag_size(pretty, condition)) < 0)
    return r;
  result += r;
  then = &list_next(call->arguments)->tag;
  pretty_save_init(&pretty_save, pretty);
  if (then->type == TAG_DO_BLOCK) {
    if ((r = buf_write_1_size(pretty, " do")) < 0)
      return r;
    result += r;
    pretty_indent(pretty, PRETTY_INDENT);
    i = 0;
    while (i < then->data.do_block.count) {
      if ((r = buf_write_1_size(pretty, "\n")) < 0)
        return r;
      result += r;
      if ((r = buf_inspect_tag_size(pretty,
                                    then->data.do_block.tag + i)) < 0)
        return r;
      i++;
    }
  }
  else {
    pretty_indent(pretty, PRETTY_INDENT);
    if ((r = buf_inspect_tag_size(pretty, then)) < 0)
      return r;
  }
  result += r;
  pretty_save_clean(&pretty_save, pretty);
  else_ = &list_next(list_next(call->arguments))->tag;
  if (else_->type != TAG_VOID) {
    if ((r = buf_write_1_size(pretty, "\nelse")) < 0)
      return r;
    result += r;
    pretty_save_init(&pretty_save, pretty);
    if (then->type == TAG_DO_BLOCK) {
      if ((r = buf_inspect_do_block_inner_size(pretty,
                                            &else_->data.do_block)) < 0)
        return r;
    }
    else
      if ((r = buf_inspect_tag_size(pretty, else_)) < 0)
        return r;
    result += r;
  }
  pretty_save_clean(&pretty_save, pretty);
  if ((r = buf_write_1_size(pretty, "\nend")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_call_op (s_buf *buf, const s_call *call,
                        u8 op_precedence)
{
  sw arity;
  s_tag *left;
  s_op *op;
  s_tag op_tag = {0};
  s_ops *ops = NULL;
  bool paren;
  sw r;
  sw result = 0;
  s_tag *right;
  left = &call->arguments->tag;
  assert(list_next(call->arguments));
  right = &list_next(call->arguments)->tag;
  paren = false;
  ops = env_global()->ops;
  if (left->type == TAG_PCALL) {
    arity = call_arity(left->data.pcall);
    if (ops_get_tag(ops, left->data.pcall->ident.sym, arity, &op_tag)) {
      if (op_tag.type != TAG_PSTRUCT ||
          ! op_tag.data.pstruct ||
          op_tag.data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
        err_puts("buf_inspect_call_op: left: ops_get did not return"
                 " a valid %KC3.Op{}");
        assert(!("buf_inspect_call_op: left: ops_get did not return"
                 " a valid %KC3.Op{}"));
        return -1;
      }
      op = op_tag.data.pstruct->data;
      if (op->precedence < op_precedence) {
        paren = true;
        if ((r = buf_write_1(buf, "(")) < 0) {
          tag_clean(&op_tag);
          return r;
        }
        result += r;
      }
      tag_clean(&op_tag);
    }
  }
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
  if ((r = buf_inspect_ident(buf, &call->ident)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  paren = false;
  if (right->type == TAG_PCALL) {
    arity = call_arity(right->data.pcall);
    if (ops_get_tag(ops, right->data.pcall->ident.sym, arity, &op_tag)) {
      if (op_tag.type != TAG_PSTRUCT ||
          ! op_tag.data.pstruct ||
          op_tag.data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
        err_puts("buf_inspect_call_op: right: ops_get did not return"
                 " a valid %KC3.Op{}");
        assert(!("buf_inspect_call_op: right: ops_get did not return"
                 " a valid %KC3.Op{}"));
        tag_clean(&op_tag);
        return -1;
      }
      op = op_tag.data.pstruct->data;
      if (op->precedence < op_precedence) {
        paren = true;
        if ((r = buf_write_1(buf, "(")) < 0) {
          tag_clean(&op_tag);
          return r;
        }
        result += r;
      }
      tag_clean(&op_tag);
    }
  }
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

sw buf_inspect_call_op_size (s_pretty *pretty, const s_call *call,
                             u8 op_precedence)
{
  sw arity;
  s_tag *left;
  s_op *op;
  s_tag op_tag = {0};
  s_ops *ops = NULL;
  bool paren;
  sw r;
  sw result = 0;
  s_tag *right;
  left = &call->arguments->tag;
  assert(list_next(call->arguments));
  right = &list_next(call->arguments)->tag;
  paren = false;
  ops = env_global()->ops;
  if (left->type == TAG_PCALL) {
    arity = call_arity(left->data.pcall);
    if (ops_get_tag(ops, left->data.pcall->ident.sym, arity, &op_tag)) {
      if (op_tag.type != TAG_PSTRUCT ||
          ! op_tag.data.pstruct ||
          op_tag.data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
        err_puts("buf_inspect_call_op_size: left: ops_get did not"
                 " return a valid %KC3.Op{}");
        assert(!("buf_inspect_call_op_size: left: ops_get did not"
                 " return a valid %KC3.Op{}"));
        tag_clean(&op_tag);
        return -1;
      }
      op = op_tag.data.pstruct->data;
      if (op->precedence < op_precedence) {
        paren = true;
        if ((r = buf_write_1_size(pretty, "(")) < 0) {
          tag_clean(&op_tag);
          return r;
        }
        result += r;
      }
      tag_clean(&op_tag);
    }
  }
  if ((r = buf_inspect_tag_size(pretty, left)) < 0)
    return r;
  result += r;
  if (paren) {
    if ((r = buf_write_1_size(pretty, ")")) < 0)
      return r;
    result += r;
  }
  if ((r = buf_write_1_size(pretty, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_ident_size(pretty, &call->ident)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, " ")) < 0)
    return r;
  result += r;
  paren = false;
  if (right->type == TAG_PCALL) {
    arity = call_arity(right->data.pcall);
    if (ops_get_tag(ops, right->data.pcall->ident.sym, arity, &op_tag)) {
      if (op_tag.type != TAG_PSTRUCT ||
          ! op_tag.data.pstruct ||
          op_tag.data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
        err_puts("buf_inspect_call_op: right: ops_get did not return"
                 " a valid %KC3.Op{}");
        assert(!("buf_inspect_call_op: right: ops_get did not return"
                 " a valid %KC3.Op{}"));
        tag_clean(&op_tag);
        return -1;
      }
      op = op_tag.data.pstruct->data;
      if (op->precedence < op_precedence) {
        paren = true;
        if ((r = buf_write_1_size(pretty, "(")) < 0) {
          tag_clean(&op_tag);
          return r;
        }
        result += r;
      }
      tag_clean(&op_tag);
    }
  }
  if ((r = buf_inspect_tag_size(pretty, right)) < 0)
    return r;
  result += r;
  if (paren) {
    if ((r = buf_write_1_size(pretty, ")")) < 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_call_op_unary (s_buf *buf, const s_call *call)
{
  s_ident ident = {0};
  s_op  *op;
  s_tag  op_tag = {0};
  s_ops *ops;
  sw r;
  sw result = 0;
  assert(buf);
  assert(call);
  ops = env_global()->ops;
  if (! ops_get_tag(ops, call->ident.sym, 1, &op_tag))
    return -1;
  if (op_tag.type != TAG_PSTRUCT ||
      ! op_tag.data.pstruct ||
      op_tag.data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
    err_puts("buf_inspect_call_op: right: ops_get did not return"
             " a valid %KC3.Op{}");
    assert(!("buf_inspect_call_op: right: ops_get did not return"
             " a valid %KC3.Op{}"));
    return -1;
  }
  op = op_tag.data.pstruct->data;
  if (op->sym == &g_sym__paren) {
    tag_clean(&op_tag);
    return buf_inspect_call_paren(buf, call);
  }
  ident.sym = op->sym;
  tag_clean(&op_tag);
  if ((r = buf_inspect_ident(buf, &ident)) < 0)
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

sw buf_inspect_call_op_unary_size (s_pretty *pretty, const s_call *call)
{
  s_op  *op;
  s_tag  op_tag = {0};
  s_ops *ops;
  sw r;
  sw result = 0;
  assert(pretty);
  assert(call);
  ops = env_global()->ops;
  if (! ops_get_tag(ops, call->ident.sym, 1, &op_tag))
    return -1;
  if (op_tag.type != TAG_PSTRUCT ||
      ! op_tag.data.pstruct ||
      op_tag.data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
    err_puts("buf_inspect_call_op: right: ops_get did not return"
             " a valid %KC3.Op{}");
    assert(!("buf_inspect_call_op: right: ops_get did not return"
             " a valid %KC3.Op{}"));
    return -1;
  }
  op = op_tag.data.pstruct->data;
  if (op->sym == &g_sym__paren) {
    tag_clean(&op_tag);
    return buf_inspect_call_paren_size(pretty, call);
  }
  if ((r = buf_inspect_sym_size(pretty, op->sym)) < 0) {
    tag_clean(&op_tag);
    return r;
  }
  tag_clean(&op_tag);
  result += r;
  if ((r = buf_write_1_size(pretty, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag_size(pretty, &call->arguments->tag)) < 0)
    return r;
  result += r;
  return result;
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

sw buf_inspect_call_paren_size (s_pretty *pretty, const s_call *call)
{
  sw r;
  sw result = 0;
  assert(pretty);
  assert(call);
  if ((r = buf_write_1_size(pretty, "(")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag_size(pretty, &call->arguments->tag)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, ")")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_call_size (s_pretty *pretty, const s_call *call)
{
  sw arity;
  bool b;
  s_op *op;
  s_tag op_tag = {0};
  s_ops *ops = NULL;
  sw r;
  sw result = 0;
  if (call->ident.module == &g_sym_KC3 &&
      call->ident.sym == &g_sym_access)
    return buf_inspect_call_access_size(pretty, call);
  if (call->ident.module == &g_sym_KC3 &&
      call->ident.sym == &g_sym_if_then_else)
    return buf_inspect_call_if_then_else_size(pretty, call);
  if ((! call->ident.module ||
       call->ident.module == &g_sym_KC3) &&
      call->ident.sym == &g_sym_str &&
      call->arguments &&
      call->arguments->tag.type == TAG_PLIST)
    return buf_inspect_call_str_size(pretty, call);
  if (call->ident.sym == &g_sym_cast)
    return buf_inspect_cast_size(pretty, call);
  arity = call_arity(call);
  if (arity > 0 && arity <= 2) {
    ops = env_global()->ops;
    if (ops_get_tag(ops, call->ident.sym, arity, &op_tag)) {
      if (op_tag.type != TAG_PSTRUCT ||
          ! op_tag.data.pstruct ||
          op_tag.data.pstruct->pstruct_type->module != &g_sym_KC3_Op) {
        err_puts("buf_inspect_call_size: ops_get did not return a valid"
                 " %KC3.Op{}");
        assert(!("buf_inspect_call_size: ops_get did not return a valid"
                 " %KC3.Op{}"));
        return -1;
      }
      op = op_tag.data.pstruct->data;
      if (op->sym == &g_sym__brackets) {
        tag_clean(&op_tag);
        return buf_inspect_call_brackets_size(pretty, call);
      }
      if (arity == 1) {
        tag_clean(&op_tag);
        return buf_inspect_call_op_unary_size(pretty, call);
      }
      if (arity == 2 && op->precedence) {
        r = buf_inspect_call_op_size(pretty, call, op->precedence);
        tag_clean(&op_tag);
        return r;
      }
      tag_clean(&op_tag);
    }
  }
  if (! ident_is_special_operator(&call->ident, &b))
    return -1;
  if (b)
    return buf_inspect_call_special_operator_size(pretty, call);
  if ((r = buf_inspect_ident_size(pretty, &call->ident)) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_call_args_size(pretty, call->arguments)) < 0)
    return r;
   result += r;
  return result;
}

sw buf_inspect_call_special_operator (s_buf *buf, const s_call *call)
{
  const s_list *args;
  bool b;
  sw r;
  sw result = 0;
  if (! ident_is_special_operator(&call->ident, &b))
    return -1;
  if (! b) {
    err_puts("buf_inspect_call_special_operator:"
             " not a special operator");
    assert(! "buf_inspect_call_special_operator:"
             " not a special operator");
    return -1;
  }
  if (special_operator_arity(&call->ident) !=
      list_length(call->arguments)) {
    err_puts("buf_inspect_call_special_operator:"
             " invalid arity");
    assert(! "buf_inspect_call_special_operator:"
             " invalid arity");
    return -1;
  }
  args = call->arguments;
  if ((r = buf_inspect_ident(buf, &call->ident)) < 0)
    return r;
  result += r;
  while (args) {
    if ((r = buf_write_1(buf, " ")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, &args->tag)) < 0)
      return r;
    result += r;
    args = list_next(args);
  }
  return result;
}

sw buf_inspect_call_special_operator_size (s_pretty *pretty,
                                           const s_call *call)
{
  const s_list *args;
  bool b;
  sw r;
  sw result = 0;
  if (! ident_is_special_operator(&call->ident, &b))
    return -1;
  if (! b) {
    err_puts("buf_inspect_call_special_operator_size:"
             " not a special operator");
    assert(! "buf_inspect_call_special_operator_size:"
             " not a special operator");
    return -1;
  }
  if (special_operator_arity(&call->ident) !=
      list_length(call->arguments)) {
    err_puts("buf_inspect_call_special_operator_size:"
             " invalid arity");
    assert(! "buf_inspect_call_special_operator_size:"
             " invalid arity");
    return -1;
  }
  args = call->arguments;
  if ((r = buf_inspect_ident_size(pretty, &call->ident)) < 0)
    return r;
  result += r;
  while (args) {
    if ((r = buf_write_1_size(pretty, " ")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag_size(pretty, &args->tag)) < 0)
      return r;
    result += r;
    args = list_next(args);
  }
  return result;
}

sw buf_inspect_call_str (s_buf *buf, const s_call *call)
{
  if ((call->ident.module &&
       call->ident.module != &g_sym_KC3) ||
      call->ident.sym != &g_sym_str ||
      ! call->arguments ||
      call->arguments->tag.type != TAG_PLIST) {
    err_puts("buf_inspect_call_str: invalid call");
    assert(! "buf_inspect_call_str: invalid call");
    return -1;
  }
  return buf_inspect_str_eval(buf, call->arguments->tag.data.plist);
}

sw buf_inspect_call_str_size (s_pretty *pretty, const s_call *call)
{
  if ((call->ident.module &&
       call->ident.module != &g_sym_KC3) ||
      call->ident.sym != &g_sym_str ||
      ! call->arguments ||
      call->arguments->tag.type != TAG_PLIST) {
    err_puts("buf_inspect_call_str_size: invalid call");
    assert(! "buf_inspect_call_str_size: invalid call");
    return -1;
  }
  return buf_inspect_str_eval_size(pretty,
                                   call->arguments->tag.data.plist);
}

sw buf_inspect_callable (s_buf *buf, const s_callable *callable)
{
  switch (callable->type) {
  case CALLABLE_CFN:
    return buf_inspect_cfn(buf, &callable->data.cfn);
  case CALLABLE_FN:
    return buf_inspect_fn(buf, &callable->data.fn);
  case CALLABLE_VOID:
    err_puts("buf_inspect_callable: CALLABLE_VOID");
    assert(! "buf_inspect_callable: CALLABLE_VOID");
    return -1;
  }
  err_puts("buf_inspect_callable: unknown callable type");
  assert(! "buf_inspect_callable: unknown callable type");
  return -1;
}

sw buf_inspect_callable_size (s_pretty *pretty,
                              const s_callable *callable)
{
  switch (callable->type) {
  case CALLABLE_CFN:
    return buf_inspect_cfn_size(pretty, &callable->data.cfn);
  case CALLABLE_FN:
    return buf_inspect_fn_size(pretty, &callable->data.fn);
  case CALLABLE_VOID:
    err_puts("buf_inspect_callable_size: CALLABLE_VOID");
    assert(! "buf_inspect_callable_size: CALLABLE_VOID");
    return -1;
  }
  err_puts("buf_inspect_callable_size: unknown callable type");
  assert(! "buf_inspect_callable_size: unknown callable type");
  return -1;
}

sw buf_inspect_cast (s_buf *buf, const s_call *call)
{
  s_tag *arg;
  const s_sym *buf_inspect_type;
  const s_sym *module;
  sw r;
  sw result = 0;
  assert(buf);
  assert(call);
  assert(call->arguments);
  assert(list_next(call->arguments));
  assert(! list_next(list_next(call->arguments)));
  module = call->ident.module;
  if ((r = buf_inspect_paren_sym(buf, module)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  arg = &list_next(call->arguments)->tag;
  buf_inspect_type = g_buf_inspect_type;
  if (! tag_type(arg, &g_buf_inspect_type)) {
    g_buf_inspect_type = buf_inspect_type;
    return -1;
  }
  if ((r = buf_inspect_tag(buf, arg)) < 0) {
    g_buf_inspect_type = buf_inspect_type;
    return r;
  }
  result += r;
  g_buf_inspect_type = buf_inspect_type;
  return result;
}

sw buf_inspect_cast_size (s_pretty *pretty, const s_call *call)
{
  s_tag *arg;
  const s_sym *module;
  sw r;
  sw result = 0;
  assert(call);
  assert(call->arguments);
  assert(! list_next(call->arguments));
  module = call->ident.module;
  if ((r = buf_inspect_paren_sym_size(pretty, module)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, " ")) < 0)
    return r;
  result += r;
  arg = &call->arguments->tag;
  if ((r = buf_inspect_tag_size(pretty, arg)) <= 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_cfn (s_buf *buf, const s_cfn *cfn)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(cfn);
  assert(cfn->c_name);
  if (cfn->name.module &&
      cfn->name.module != env_global()->current_defmodule) {
    if ((r = buf_inspect_sym(buf, cfn->name.module)) <= 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, ".")) <= 0)
      return r;
    result += r;
  }
  if ((r = buf_write_1(buf, "cfn ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_sym(buf, cfn->result_type)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_str(buf, &cfn->c_name->str)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_list_paren(buf, cfn->arg_types)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_cfn_size (s_pretty *pretty, const s_cfn *cfn)
{
  sw r;
  sw result = 0;
  assert(cfn);
  if (cfn->name.module &&
      cfn->name.module != env_global()->current_defmodule) {
    if ((r = buf_inspect_sym_size(pretty, cfn->name.module)) <= 0)
      return r;
    result += r;
    if ((r = buf_write_1_size(pretty, ".")) <= 0)
      return r;
    result += r;
  }
  if ((r = buf_write_1_size(pretty, "cfn ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_sym_size(pretty, cfn->result_type)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_str_size(pretty, &cfn->c_name->str)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_list_size(pretty, cfn->arg_types)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_character (s_buf *buf, character c)
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

sw buf_inspect_character_size (s_pretty *pretty, character c)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, "'")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_str_character_size(pretty, c)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, "'")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_complex (s_buf *buf, const s_complex *c)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_inspect_tag(buf, &c->x)) < 0)
    goto restore;
  result += r;
  if ((r = buf_write_1(buf, " +i ")) < 0)
    goto restore;
  result += r;
  if ((r = buf_inspect_tag(buf, &c->y)) < 0)
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

sw buf_inspect_complex_size (s_pretty *pretty, const s_complex *c)
{
  sw r;
  sw result = 0;
  if ((r = buf_inspect_tag_size(pretty, &c->x)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, " +i ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag_size(pretty, &c->y)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_cow (s_buf *buf, s_cow *cow)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_write_1(buf, "cow ")) < 0)
    goto restore;
  if ((r = buf_inspect_tag(buf, cow_read_only(cow))) < 0)
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

sw buf_inspect_cow_size (s_pretty *pretty, s_cow *cow)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, "cow ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag_size(pretty, cow_read_only(cow))) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_f32 (s_buf *buf, f32 x)
{
  s32 exp;
  u8 i;
  u8 j;
  sw r;
  sw result = 0;
  assert(buf);
  exp = 0;
  if ((r = buf_write_1(buf, "(F32) ")) < 0)
    return r;
  result += r;
  if (x == 0.0f) {
    if ((r = buf_write_1(buf, "0.0")) < 0)
      return r;
    result += r;
    return result;
  }
  if (x < 0) {
    if ((r = buf_write_1(buf, "-")) <= 0)
      return r;
    result += r;
    x = -x;
  }
  if (x >= 1.0)
    while (x >= 10.0) {
      x /= 10.0;
      exp++;
    }
  else
    while (x < 1.0) {
      x *= 10.0;
      exp--;
    }
  i = (u8) x;
  x -= i;
  i += '0';
  if ((r = buf_write_character_utf8(buf, i)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, ".")) <= 0)
    return r;
  result += r;
  j = 6;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    if ((r = buf_write_character_utf8(buf, i)) <= 0)
      return r;
    result += r;
    j--;
  } while (x >= pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1(buf, "e")) <= 0)
      return r;
    result += r;
    if (exp > 0) {
      if ((r = buf_write_1(buf, "+")) <= 0)
        return r;
      result += r;
    }
    if ((r = buf_inspect_s32_decimal(buf, exp)) <= 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_f32_size (s_pretty *pretty, f32 x)
{
  s32 exp;
  u8 i;
  u8 j;
  sw r;
  sw result = 0;
  assert(pretty);
  exp = 0;
  if ((r = buf_write_1_size(pretty, "(F32) ")) < 0)
    return r;
  result += r;
  if (x == 0.0f) {
    if ((r = buf_write_1_size(pretty, "0.0")) < 0)
      return r;
    result += r;
    return result;
  }
  if (x < 0) {
    if ((r = buf_write_1_size(pretty, "-")) <= 0)
      return r;
    result += r;
    x = -x;
  }
  if (x >= 1.0)
    while (x >= 10.0) {
      x /= 10.0;
      exp++;
    }
  else
    while (x < 1.0) {
      x *= 10.0;
      exp--;
    }
  i = (u8) x;
  x -= i;
  i += '0';
  result += 1;
  if ((r = buf_write_1_size(pretty, ".")) <= 0)
    return r;
  result += r;
  j = 6;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    result += 1;
    j--;
  } while (x >= pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1_size(pretty, "e")) <= 0)
      return r;
    result += r;
    if (exp > 0) {
      if ((r = buf_write_1_size(pretty, "+")) <= 0)
        return r;
      result += r;
    }
    if ((r = buf_inspect_s32_decimal_size(pretty, exp)) <= 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_f64 (s_buf *buf, f64 x)
{
  s64 b;
  s32 exp;
  u8 i;
  u8 j;
  sw r;
  sw result = 0;
  assert(buf);
  exp = 0;
  if ((r = buf_write_1(buf, "(F64) ")) < 0)
    return r;
  result += r;
  if (x == 0.0) {
    if ((r = buf_write_1(buf, "0.0")) < 0)
      return r;
    result += r;
    return result;
  }
  if (x < 0) {
    if ((r = buf_write_1(buf, "-")) <= 0)
      return r;
    result += r;
    x = -x;
  }
  if (x >= 1.0) {
    b = 10;
    while (x >= b) {
      b *= 10;
      exp++;
    }
  }
  else {
    b = 1;
    while (x * b < 1.0) {
      b *= 10;
      exp--;
    }
  }
  x *= pow(10, -exp);
  i = (u8) x;
  x -= i;
  i += '0';
  if ((r = buf_write_character_utf8(buf, i)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, ".")) <= 0)
    return r;
  result += r;
  j = 14;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    if ((r = buf_write_character_utf8(buf, i)) <= 0)
      return r;
    result += r;
    j--;
  } while (x > pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1(buf, "e")) <= 0)
      return r;
    result += r;
    if (exp > 0) {
      if ((r = buf_write_1(buf, "+")) <= 0)
        return r;
      result += r;
    }
    if ((r = buf_inspect_s64_decimal(buf, exp)) <= 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_f64_size (s_pretty *pretty, f64 x)
{
  s64 b;
  s32 exp;
  u8 i;
  u8 j;
  sw r;
  sw result = 0;
  assert(pretty);
  exp = 0;
  if ((r = buf_write_1_size(pretty, "(F64) ")) < 0)
    return r;
  result += r;
  if (x == 0.0) {
    if ((r = buf_write_1_size(pretty, "0.0")) < 0)
      return r;
    result += r;
    return result;
  }
  if (x < 0) {
    if ((r = buf_write_1_size(pretty, "-")) <= 0)
      return r;
    result += r;
    x = -x;
  }
  if (x >= 1.0) {
    b = 10;
    while (x >= b) {
      b *= 10;
      exp++;
    }
  }
  else {
    b = 1;
    while (x * b < 1.0) {
      b *= 10;
      exp--;
    }
  }
  x *= pow(10, -exp);
  i = (u8) x;
  x -= i;
  i += '0';
  if ((r = buf_write_character_utf8_size(pretty, i)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, ".")) <= 0)
    return r;
  result += r;
  j = 14;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    if ((r = buf_write_character_utf8_size(pretty, i)) <= 0)
      return r;
    result += r;
    j--;
  } while (x > pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1_size(pretty, "e")) <= 0)
      return r;
    result += r;
    if (exp > 0) {
      if ((r = buf_write_1_size(pretty, "+")) <= 0)
        return r;
      result += r;
    }
    if ((r = buf_inspect_s64_decimal_size(pretty, exp)) <= 0)
      return r;
    result += r;
  }
  return result;
}

#if HAVE_F80

sw buf_inspect_f80 (s_buf *buf, f80 x)
{
  s64 exp;
  u8 i;
  u8 j;
  sw r;
  sw result = 0;
  assert(buf);
  exp = 0.0;
  if ((r = buf_write_1(buf, "(F80) ")) < 0)
    return r;
  result += r;
  if (x == 0.0) {
    if ((r = buf_write_1(buf, "0.0")) < 0)
      return r;
    result += r;
    return result;
  }
  if (x < 0) {
    if ((r = buf_write_1(buf, "-")) <= 0)
      return r;
    result += r;
    x = -x;
  }
  if (x >= 1.0)
    while (x >= 10.0) {
      x /= 10.0;
      exp++;
    }
  else
    while (x < 1.0) {
      x *= 10.0;
      exp--;
    }
  i = (u8) x;
  x -= i;
  i += '0';
  if ((r = buf_write_character_utf8(buf, i)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, ".")) <= 0)
    return r;
  result += r;
  j = 20;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    if ((r = buf_write_character_utf8(buf, i)) <= 0)
      return r;
    result += r;
    j--;
  } while (x > pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1(buf, "e")) <= 0)
      return r;
    result += r;
    if (exp > 0) {
      if ((r = buf_write_1(buf, "+")) <= 0)
        return r;
      result += r;
    }
    if ((r = buf_inspect_s64_decimal(buf, exp)) <= 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_f80_size (s_pretty *pretty, f80 x)
{
  s64 exp;
  u8 i;
  u8 j;
  sw r;
  sw result = 0;
  assert(pretty);
  exp = 0;
  if ((r = buf_write_1_size(pretty, "(F80) ")) < 0)
    return r;
  result += r;
  if (x == 0.0) {
    if ((r = buf_write_1_size(pretty, "0.0")) < 0)
      return r;
    result += r;
    return result;
  }
  if (x < 0) {
    if ((r = buf_write_1_size(pretty, "-")) <= 0)
      return r;
    result += r;
    x = -x;
  }
  if (x >= 1.0)
    while (x >= 10.0) {
      x /= 10.0;
      exp++;
    }
  else
    while (x < 1.0) {
      x *= 10.0;
      exp--;
    }
  i = (u8) x;
  x -= i;
  i += '0';
  result += 1;
  if ((r = buf_write_1_size(pretty, ".")) <= 0)
    return r;
  result += r;
  j = 20;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    result += 1;
    j--;
  } while (x > pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1_size(pretty, "e")) <= 0)
      return r;
    result += r;
    if (exp > 0) {
      if ((r = buf_write_1_size(pretty, "+")) <= 0)
        return r;
      result += r;
    }
    if ((r = buf_inspect_s64_decimal_size(pretty, exp)) <= 0)
      return r;
    result += r;
  }
  return result;
}

#endif

#if HAVE_F128

sw buf_inspect_f128 (s_buf *buf, f128 x)
{
  s64 exp;
  u8 i;
  u8 j;
  sw r;
  sw result = 0;
  assert(buf);
  exp = 0.0;
  if ((r = buf_write_1(buf, "(F128) ")) < 0)
    return r;
  result += r;
  if (x == 0.0) {
    if ((r = buf_write_1(buf, "0.0")) < 0)
      return r;
    result += r;
    return result;
  }
  if (x < 0) {
    if ((r = buf_write_1(buf, "-")) <= 0)
      return r;
    result += r;
    x = -x;
  }
  if (x >= 1.0)
    while (x >= 10.0) {
      x /= 10.0;
      exp++;
    }
  else
    while (x < 1.0) {
      x *= 10.0;
      exp--;
    }
  i = (u8) x;
  x -= i;
  i += '0';
  if ((r = buf_write_character_utf8(buf, i)) <= 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, ".")) <= 0)
    return r;
  result += r;
  j = 33;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    if ((r = buf_write_character_utf8(buf, i)) <= 0)
      return r;
    result += r;
    j--;
  } while (x > pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1(buf, "e")) <= 0)
      return r;
    result += r;
    if (exp > 0) {
      if ((r = buf_write_1(buf, "+")) <= 0)
        return r;
      result += r;
    }
    if ((r = buf_inspect_s64_decimal(buf, exp)) <= 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_f128_size (s_pretty *pretty, f128 x)
{
  f128 exp;
  u8 i;
  u8 j;
  sw r;
  sw result = 0;
  assert(pretty);
  exp = 0.0;
  if ((r = buf_write_1_size(pretty, "(F128) ")) < 0)
    return r;
  result += r;
  if (x == 0.0) {
    if ((r = buf_write_1_size(pretty, "0.0")) < 0)
      return r;
    result += r;
    return result;
  }
  if (x < 0) {
    if ((r = buf_write_1_size(pretty, "-")) <= 0)
      return r;
    result += r;
    x = -x;
  }
  if (x >= 1.0)
    while (x >= 10.0) {
      x /= 10.0;
      exp++;
    }
  else
    while (x < 1.0) {
      x *= 10.0;
      exp--;
    }
  i = (u8) x;
  x -= i;
  i += '0';
  result += 1;
  if ((r = buf_write_1_size(pretty, ".")) <= 0)
    return r;
  result += r;
  j = 33;
  do {
    x *= 10;
    i = (u8) x;
    x -= i;
    i += '0';
    result += 1;
    j--;
  } while (x > pow(0.1, j) && j);
  if (exp) {
    if ((r = buf_write_1_size(pretty, "e")) <= 0)
      return r;
    result += r;
    if (exp > 0) {
      if ((r = buf_write_1_size(pretty, "+")) <= 0)
        return r;
      result += r;
    }
    if ((r = buf_inspect_s64_decimal_size(pretty, exp)) <= 0)
      return r;
    result += r;
  }
  return result;
}

#endif

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

sw buf_inspect_fact_size (s_pretty *pretty, const s_fact *fact)
{
  sw r;
  sw result = 0;
  if (fact) {
    if ((r = buf_write_1_size(pretty, "{")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag_size(pretty, fact->subject)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1_size(pretty, ", ")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag_size(pretty, fact->predicate)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1_size(pretty, ", ")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag_size(pretty, fact->object)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1_size(pretty, "}")) < 0)
      return r;
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
  s_binding *binding = NULL;
  const s_fn_clause *clause = NULL;
  s_ident ident = {0};
  s_pretty_save pretty_save = {0};
  sw r;
  sw result = 0;
  s_tag *tag = NULL;
  assert(buf);
  assert(fn);
  if (fn->macro)
    ident.sym = &g_sym_macro;
  else
    ident.sym = &g_sym_fn;
  ident.module = fn->module;
  pretty_save_init(&pretty_save, &buf->pretty);
  if ((r = buf_inspect_ident(buf, &ident)) < 0)
    goto clean;
  result += r;
  if ((r = buf_write_1(buf, " ")) < 0)
    goto clean;
  result += r;
  clause = fn->clauses;
  assert(clause);
  if (clause->next) {
    if ((r = buf_write_1(buf, "{\n")) < 0)
      goto clean;
    result += r;
    while (clause) {
      if ((r = buf_write_1(buf, "  ")) < 0)
        goto clean;
      result += r;
      if ((r = buf_inspect_fn_clause(buf, clause)) < 0)
        goto clean;
      result += r;
      if ((r = buf_write_1(buf, "\n")) < 0)
        goto clean;
      result += r;
      clause = clause->next;
    }
    if ((r = buf_write_1(buf, "}")) < 0)
      goto clean;
    result += r;
  }
  else {
    if ((r = buf_inspect_fn_clause(buf, clause)) < 0)
      goto clean;
    result += r;
  }
  if (fn->frame && fn->frame->bindings) {
    if ((r = buf_write_1(buf, " [")) < 0)
      goto clean;
    result += r;
    pretty_indent_from_column(&buf->pretty, 0);
    binding = fn->frame->bindings;
    while (binding) {
      if ((r = buf_inspect_ident_sym(buf, binding->name)) < 0)
        goto clean;
      result += r;
      if ((r = buf_write_1(buf, ": ")) < 0)
        goto clean;
      result += r;
      if (binding->value.type == TAG_PVAR &&
          binding->value.data.pvar->bound) {
        tag = &binding->value.data.pvar->tag;
        if (tag->type == TAG_PCALLABLE &&
            tag->data.pcallable->type == CALLABLE_FN &&
            tag->data.pcallable->data.fn.name.module ==
            fn->name.module &&
            tag->data.pcallable->data.fn.name.sym ==
            fn->name.sym) {
          if ((r = buf_inspect_ident(buf, &fn->name)) <= 0)
            goto clean;
          result += r;
          goto next;
        }
      }
      if ((r = buf_inspect_tag(buf, &binding->value)) < 0)
        goto clean;
      result += r;
    next:
      binding = binding->next;
      if (binding &&
          (r = buf_write_1(buf, ",\n")) < 0)
        goto clean;
    }
    if ((r = buf_write_1(buf, "]")) < 0)
      goto clean;
    result += r;
  }
  r = result;
 clean:
  pretty_save_clean(&pretty_save, &buf->pretty);
  return r;
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
  if ((r = buf_inspect_do_block(buf, &clause->algo)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_fn_clause_size (s_pretty *pretty,
                               const s_fn_clause *clause)
{
  sw r;
  sw result = 0;
  assert(pretty);
  assert(clause);
  if ((r = buf_inspect_fn_pattern_size(pretty, clause->pattern)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, " ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_do_block_size(pretty, &clause->algo)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_fn_pattern (s_buf *buf, const s_list *pattern)
{
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "(")) < 0)
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
  if ((r = buf_write_1(buf, ")")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_fn_pattern_size (s_pretty *pretty, const s_list *pattern)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, "(")) < 0)
    return r;
  result += r;
  while (pattern) {
    if ((r = buf_inspect_tag_size(pretty, &pattern->tag)) < 0)
      return r;
    result += r;
    pattern = list_next(pattern);
    if (pattern) {
      if ((r = buf_write_1_size(pretty, ", ")) < 0)
        return r;
      result += r;
    }
  }
  if ((r = buf_write_1_size(pretty, ")")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_fn_size (s_pretty *pretty, const s_fn *fn)
{
  const s_binding *binding = NULL;
  const s_fn_clause *clause = NULL;
  s_ident ident = {0};
  sw r;
  sw result = 0;
  s_tag *tag;
  assert(pretty);
  assert(fn);
  if (fn->macro)
    ident.sym = &g_sym_macro;
  else
    ident.sym = &g_sym_fn;
  ident.module = fn->module;
  if ((r = buf_inspect_ident_size(pretty, &ident)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, " ")) < 0)
    return r;
  result += r;
  clause = fn->clauses;
  assert(clause);
  if (clause->next) {
    if ((r = buf_write_1_size(pretty, "{\n")) < 0)
      return r;
    result += r;
    while (clause) {
      if ((r = buf_write_1_size(pretty, "  ")) < 0)
        return r;
      result += r;
      if ((r = buf_inspect_fn_clause_size(pretty, clause)) < 0)
        return r;
      result += r;
      if ((r = buf_write_1_size(pretty, "\n")) < 0)
        return r;
      result += r;
      clause = clause->next;
    }
    if ((r = buf_write_1_size(pretty, "}")) < 0)
      return r;
    result += r;
  }
  else {
    if ((r = buf_inspect_fn_clause_size(pretty, clause)) < 0)
      return r;
    result += r;
  }
  if (fn->frame && fn->frame->bindings) {
    if ((r = buf_write_1_size(pretty, " [")) < 0)
      return r;
    result += r;
    binding = fn->frame->bindings;
    while (binding) {
      if ((r = buf_inspect_ident_sym_size(pretty, binding->name)) < 0)
        return r;
      result += r;
      if ((r = buf_write_1_size(pretty, ": ")) < 0)
        return r;
      result += r;
      if (binding->value.type == TAG_PVAR &&
          binding->value.data.pvar->bound) {
        tag = &binding->value.data.pvar->tag;
        if (tag->type == TAG_PCALLABLE &&
            tag->data.pcallable->type == CALLABLE_FN &&
            tag->data.pcallable->data.fn.name.module ==
            fn->name.module &&
            tag->data.pcallable->data.fn.name.sym ==
            fn->name.sym) {
          if ((r = buf_inspect_ident_size(pretty, &fn->name)) <= 0)
            return r;
          result += r;
          goto next;
        }
      }
      if ((r = buf_inspect_tag_size(pretty, &binding->value)) < 0)
        return r;
      result += r;
    next:
      binding = binding->next;
    }
    if ((r = buf_write_1_size(pretty, "]")) < 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_frame (s_buf *buf, const s_frame *frame)
{
  const s_frame *f;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  assert(buf);
  pretty_save_init(&pretty_save, &buf->pretty);
  if ((r = buf_write_1(buf, "[")) < 0)
    goto clean;
  result += r;
  pretty_indent_from_column(&buf->pretty, 0);
  f = frame;
  while (f) {
    if ((r = buf_inspect_binding(buf, f->bindings)) < 0)
      goto clean;
    result += r;
    f = f->next;
    if (f) {
      if ((r = buf_write_1(buf, ",\n")) < 0)
        goto clean;
      result += r;
    }
  }
  if ((r = buf_write_1(buf, "]")) < 0)
    goto clean;
  result += r;
  pretty_save_clean(&pretty_save, &buf->pretty);
  return result;
 clean:
  pretty_save_clean(&pretty_save, &buf->pretty);
  return r;
}

sw buf_inspect_ident (s_buf *buf, const s_ident *ident)
{
  s_env *env;
  sw r;
  sw result = 0;
  assert(buf);
  assert(ident);
  env = env_global();
  result = 0;
  // FIXME
  if (ident->module && ident->module != env->current_defmodule) {
    if ((r = buf_inspect_sym(buf, ident->module)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, ".")) < 0)
      return r;
    result += r;
  }
  if ((r = buf_inspect_ident_sym(buf, ident->sym)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ident_size (s_pretty *pretty, const s_ident *ident)
{
  s_env *env;
  sw r;
  sw result = 0;
  assert(ident);
  env = env_global();
  if (ident->module && ident->module != env->current_defmodule) {
    if ((r = buf_inspect_sym_size(pretty, ident->module)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1_size(pretty, ".")) < 0)
      return r;
    result += r;
  }
  if ((r = buf_inspect_ident_sym_size(pretty, ident->sym)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ident_sym (s_buf *buf, const s_sym *sym)
{
  if (! sym)
    return buf_write_1(buf, "void");
  if (sym->str.size == 0)
    return buf_write_1(buf, "_\"\"");
  if (sym_has_ident_reserved_characters(sym))
    return buf_inspect_ident_sym_reserved(buf, sym);
  return buf_write_str(buf, &sym->str);
}

sw buf_inspect_ident_sym_reserved (s_buf *buf, const s_sym *sym)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(sym);
  if ((r = buf_write_1(buf, "_")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_str(buf, &sym->str)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ident_sym_reserved_size (s_pretty *pretty,
                                        const s_sym *sym)
{
  sw r;
  sw result = 0;
  assert(sym);
  if ((r = buf_write_1_size(pretty, "_")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_str_size(pretty, &sym->str)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ident_sym_size (s_pretty *pretty, const s_sym *sym)
{
  if (sym->str.size == 0)
    return buf_write_1_size(pretty, "_\"\"");
  if (sym_has_ident_reserved_characters(sym))
    return buf_inspect_ident_sym_reserved_size(pretty, sym);
  return sym->str.size;
}

sw buf_inspect_integer (s_buf *buf, const s_integer *x)
{
  sw r;
  sw result = 0;
  if (integer_needs_cast(x)) {
    if ((r = buf_write_1(buf, "(Integer) ")) <= 0)
      return r;
    result += r;
  }
  if ((r = buf_inspect_integer_decimal(buf, x)) <= 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_integer_decimal (s_buf *buf, const s_integer *x)
{
  s_buf buf_tmp;
  mp_digit digit;
  size_t maxlen;
  u8 p;
  sw r;
  const mp_digit radix = 10;
  sw result = 0;
  s32 size = 0;
  mp_int t;
  if (MP_IS_ZERO(&x->mp_int))
    return buf_write_1(buf, "0");
  if (mp_radix_size(&x->mp_int, radix, &size) != MP_OKAY)
    return -1;
  maxlen = size;
  if (mp_init_copy(&t, &x->mp_int) != MP_OKAY)
    return -1;
  if (t.sign == MP_NEG) {
    t.sign = MP_ZPOS;
    maxlen--;
    if ((r = buf_write_1(buf, "-")) <= 0)
      return r;
    result += r;
  }
  buf_init_alloc(&buf_tmp, maxlen);
  while (! MP_IS_ZERO(&t)) {
    if (mp_div_d(&t, radix, &t, &digit) != MP_OKAY)
      goto error;
    p = '0' + digit;
    if (p > '9')
      goto error;
    if ((r = buf_write_character_utf8(&buf_tmp, p)) <= 0)
      goto error;
    result += r;
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

sw buf_inspect_integer_decimal_size (s_pretty *pretty,
                                     const s_integer *x)
{
  mp_digit digit;
  sw r;
  const mp_digit radix = 10;
  sw result = 0;
  mp_int t;
  if (MP_IS_ZERO(&x->mp_int))
    return buf_write_1_size(pretty, "0");
  if (mp_init_copy(&t, &x->mp_int) != MP_OKAY)
    return -1;
  if (t.sign == MP_NEG) {
    t.sign = MP_ZPOS;
    if ((r = buf_write_1_size(pretty, "-")) <= 0)
      goto ko;
    result += r;
  }
  while (! MP_IS_ZERO(&t)) {
    if (mp_div_d(&t, radix, &t, &digit) != MP_OKAY ||
        digit > 9) {
      r = -1;
      goto ko;
    }
    if ((r = buf_write_1_size(pretty, "9")) <= 0)
      goto ko;
    result += r;
  }
  mp_clear(&t);
  return result;
 ko:
  mp_clear(&t);
  return r;
}

sw buf_inspect_integer_hexadecimal (s_buf *buf, const s_integer *x)
{
  s_buf buf_tmp;
  mp_digit digit;
  sw result = 0;
  size_t maxlen;
  u8 p;
  const mp_digit radix = 16;
  s32 size = 0;
  mp_int t;
  if (MP_IS_ZERO(&x->mp_int))
    return buf_write_character_utf8(buf, '0');
  if (mp_radix_size(&x->mp_int, radix, &size) != MP_OKAY)
    return -1;
  maxlen = size;
  if (mp_init_copy(&t, &x->mp_int) != MP_OKAY)
    return -1;
  if (t.sign == MP_NEG) {
    t.sign = MP_ZPOS;
    maxlen--;
    buf_write_character_utf8(buf, '-');
    result++;
  }
  buf_init_alloc(&buf_tmp, maxlen);
  while (! MP_IS_ZERO(&t)) {
    if (mp_div_d(&t, radix, &t, &digit) != MP_OKAY ||
        digit > radix)
      goto error;
    if (digit <= 9)
      p = '0' + digit;
    else
      p = 'a' + digit - 10;
    buf_write_character_utf8(&buf_tmp, p);
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

sw buf_inspect_integer_size (s_pretty *pretty, const s_integer *x)
{
  sw r;
  sw result = 0;
  if (integer_needs_cast(x)) {
    if ((r = buf_write_1_size(pretty, "(Integer) ")) <= 0)
      return r;
    result += r;
  }
  if ((r = buf_inspect_integer_decimal_size(pretty, x)) <= 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_list (s_buf *buf, const s_list *x)
{
  bool alist;
  const s_list *i;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "[")) <= 0)
    return r;
  result += r;
  alist = list_is_alist(x);
  if (alist) {
    pretty_save_init(&pretty_save, &buf->pretty);
    pretty_indent_from_column(&buf->pretty, 0);
  }
  i = x;
  while (i) {
    if ((r = buf_inspect_list_tag(buf, &i->tag)) < 0)
      return r;
    result += r;
    switch (i->next.type) {
    case TAG_PLIST:
      if (i->next.data.plist) {
        if ((r = buf_write_1(buf, ",\n")) < 0)
          return r;
        result += r;
      }
      i = i->next.data.plist;
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
  if (alist)
    pretty_save_clean(&pretty_save, &buf->pretty);
  if ((r = buf_write_1(buf, "]")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_list_paren (s_buf *buf, const s_list *x)
{
  const s_list *i;
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "(")) <= 0)
    return r;
  result++;
  i = x;
  while (i) {
    if ((r = buf_inspect_tag(buf, &i->tag)) < 0)
      return r;
    result += r;
    switch (i->next.type) {
    case TAG_PLIST:
      if (i->next.data.plist) {
        if ((r = buf_write_1(buf, ", ")) < 0)
          return r;
        result += r;
      }
      i = i->next.data.plist;
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

sw buf_inspect_list_size (s_pretty *pretty, const s_list *x)
{
  bool alist;
  const s_list *i;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  assert(pretty);
  if ((r = buf_write_1_size(pretty, "[")) <= 0)
    return r;
  result += r;
  alist = list_is_alist(x);
  if (alist) {
    pretty_save_init(&pretty_save, pretty);
    pretty_indent_from_column(pretty, 0);
  }
  i = x;
  while (i) {
    if ((r = buf_inspect_list_tag_size(pretty, &i->tag)) < 0)
      return r;
    result += r;
    switch (i->next.type) {
    case TAG_PLIST:
      if (i->next.data.plist) {
        if ((r = buf_write_1_size(pretty, ",\n")) < 0)
          return r;
        result += r;
      }
      i = i->next.data.plist;
      continue;
    default:
      if ((r = buf_write_1_size(pretty, " | ")) < 0)
        return r;
      result += r;
      if ((r = buf_inspect_tag_size(pretty, &i->next)) < 0)
        return r;
      result += r;
      i = NULL;
    }
  }
  if (alist)
    pretty_save_clean(&pretty_save, pretty);
  if ((r = buf_write_1_size(pretty, "]")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_list_tag (s_buf *buf, const s_tag *tag)
{
  sw r;
  sw result = 0;
  const s_sym *sym;
  assert(buf);
  assert(tag);
  if (tag->type == TAG_TUPLE &&
      tag->data.tuple.count == 2 &&
      tag->data.tuple.tag[0].type == TAG_PSYM) {
    sym = tag->data.tuple.tag[0].data.psym;
    if (sym_has_reserved_characters(sym)) {
      if ((r = buf_inspect_str(buf, &sym->str)) < 0)
        return r;
    }
    else
      if ((r = buf_write_str(buf, &sym->str)) < 0)
        return r;
    result += r;
    if ((r = buf_write_1(buf, ": ")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, tag->data.tuple.tag + 1)) < 0)
      return r;
    result += r;
    return result;
  }
  return buf_inspect_tag(buf, tag);
}

sw buf_inspect_list_tag_size (s_pretty *pretty, const s_tag *tag)
{
  sw r;
  sw result = 0;
  const s_sym *sym;
  assert(tag);
  if (tag->type == TAG_TUPLE &&
      tag->data.tuple.count == 2 &&
      tag->data.tuple.tag[0].type == TAG_PSYM) {
    sym = tag->data.tuple.tag[0].data.psym;
    if (sym_has_reserved_characters(sym)) {
      if ((r = buf_inspect_str_size(pretty, &sym->str)) < 0)
        return r;
    }
    else
      r = sym->str.size;
    result += r;
    if ((r = buf_write_1_size(pretty, ": ")) < 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag_size(pretty, tag->data.tuple.tag + 1)) < 0)
      return r;
    result += r;
    return result;
  }
  return buf_inspect_tag_size(pretty, tag);
}

sw buf_inspect_map (s_buf *buf, const s_map *map)
{
  uw i = 0;
  s_tag *k;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  assert(buf);
  assert(map);
  if ((r = buf_write_1(buf, "%{")) < 0)
    return r;
  result += r;
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent_from_column(&buf->pretty, 0);
  while (i < map->count) {
    k = map->key + i;
    if (k->type == TAG_PSYM) {
      if (sym_has_reserved_characters(k->data.psym)) {
        if ((r = buf_inspect_str(buf, &k->data.psym->str)) < 0)
          return r;
      }
      else
        if ((r = buf_write_str(buf, &k->data.psym->str)) < 0)
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
      if ((r = buf_write_1(buf, ",\n")) < 0)
        return r;
      result += r;
    }
  }
  pretty_save_clean(&pretty_save, &buf->pretty);
  if ((r = buf_write_1(buf, "}")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_map_size (s_pretty *pretty, const s_map *map)
{
  uw i = 0;
  s_tag *k;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  assert(map);
  if ((r = buf_write_1_size(pretty, "%{")) < 0)
    return r;
  result += r;
  pretty_save_init(&pretty_save, pretty);
  pretty_indent_from_column(pretty, 0);
  while (i < map->count) {
    k = map->key + i;
    if (k->type == TAG_PSYM) {
      if (sym_has_reserved_characters(k->data.psym)) {
        if ((r = buf_inspect_str_size(pretty, &k->data.psym->str)) < 0)
          return r;
      }
      else
        if ((r = buf_write_str_size(pretty, &k->data.psym->str)) < 0)
          return r;
      result += r;
      if ((r = buf_write_1_size(pretty, ": ")) < 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_inspect_tag_size(pretty, map->key + i)) < 0)
        return r;
      result += r;
      if ((r = buf_write_1_size(pretty, " => ")) < 0)        
        return r;
      result += r;
    }
    if ((r = buf_inspect_tag_size(pretty, map->value + i)) < 0)
      return r;
    result += r;
    i++;
    if (i < map->count) {
      if ((r = buf_write_1_size(pretty, ",\n")) < 0)
        return r;
      result += r;
    }
  }
  pretty_save_clean(&pretty_save, pretty);
  if ((r = buf_write_1_size(pretty, "}")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_match (s_buf *buf, const s_call *match)
{
  p_list arg;
  s_do_block *do_block;
  uw i;
  uw j;
  s_map *map;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  assert(buf);
  assert(match);
  if (! match->arguments ||
      ! (arg = list_next(match->arguments)) ||
      list_next(arg)) {
    err_puts("buf_inspect_match: invalid arity");
    assert(! "buf_inspect_match: invalid arity");
    return -1;
  }
  if (arg->tag.type != TAG_MAP) {
    err_puts("buf_inspect_match: invalid match: not a Map");
    assert(! "buf_inspect_match: invalid match: not a Map");
    return -1;
  }
  map = &arg->tag.data.map;
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent_from_column(&buf->pretty, 0);
  if ((r = buf_write_1(buf, "match ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag(buf, &match->arguments->tag)) < 0)
    return r;
  result += r;
  pretty_indent(&buf->pretty, 2);
  if ((r = buf_write_1(buf, "\n")) < 0)
    return r;
  result += r;
  i = 0;
  while (i < map->count) {
    if ((r = buf_inspect_tag(buf, map->key + i)) < 0)
      return r;
    result += r;
    pretty_indent(&buf->pretty, 2);
    if ((r = buf_write_1(buf, " →\n")) < 0)
      return r;
    result += r;
    if (map->value[i].type != TAG_DO_BLOCK) {
      err_puts("buf_inspect_match: invalid match: not a Block");
      assert(! "buf_inspect_match: invalid match: not a Block");
      return -1;
    }
    do_block = &map->value[i].data.do_block;
    j = 0;
    while (j < do_block->count) {
      if ((r = buf_inspect_tag(buf, do_block->tag + j)) < 0)
        return r;
      result += r;
      if (j == do_block->count - 1)
        pretty_indent(&buf->pretty, (i == map->count - 1) ? -4 : -2);
      if ((r = buf_write_1(buf, "\n")) < 0)
        return r;
      result += r;
      j++;
    }
    i++;
  }
  if ((r = buf_write_1(buf, "end")) < 0)
    return r;
  result += r;
  pretty_save_clean(&pretty_save, &buf->pretty);
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
  if (sym_is_array_type(sym)) {
    if ((r = buf_write_str(buf, &sym->str)) <= 0)
      goto clean;
    result += r;
  }
  else {
    if ((r = buf_inspect_sym(buf, sym)) <= 0)
      goto clean;
    result += r;
  }
  if ((r = buf_write_1(buf, ")")) <= 0)
    goto clean;
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_paren_sym_size (s_pretty *pretty, const s_sym *sym)
{
  sw r;
  sw result = 0;
  assert(sym);
  if ((r = buf_write_1_size(pretty, "(")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_sym_size(pretty, sym)) <= 0)
    goto clean;
  result += r;
  if ((r = buf_write_1_size(pretty, ")")) < 0)
    return r;
  result += r;
  r = result;
 clean:
  return r;
}

sw buf_inspect_pcallable (s_buf *buf,
                          const s_callable * const *pcallable)
{
  return buf_inspect_callable(buf, *pcallable);
}

sw buf_inspect_pcallable_size (s_pretty *pretty,
                               const s_callable * const *pcallable)
{
  return buf_inspect_callable_size(pretty, *pcallable);
}

sw buf_inspect_pcomplex (s_buf *buf, p_complex const *pcomplex)
{
  return buf_inspect_complex(buf, *pcomplex);
}

sw buf_inspect_pcomplex_size (s_pretty *pretty,
                              p_complex const *pcomplex)
{
  return buf_inspect_complex_size(pretty, *pcomplex);
}

sw buf_inspect_pcow (s_buf *buf, p_cow const *pcow)
{
  return buf_inspect_cow(buf, *pcow);
}

sw buf_inspect_pcow_size (s_pretty *pretty, p_cow const *pcow)
{
  return buf_inspect_cow_size(pretty, *pcow);
}

sw buf_inspect_pfacts (s_buf *buf, const p_facts *x)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, "(Facts*) 0x")) <= 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal(buf, (uw) *x)))
    return r;
  result += r;
  return result;
}

sw buf_inspect_pfacts_size (s_pretty *pretty, const p_facts *x)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, "(Facts*) 0x")) <= 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal_size(pretty, (uw) *x)))
    return r;
  result += r;
  return result;
}

sw buf_inspect_plist (s_buf *buf, const p_list *x)
{
  return buf_inspect_list(buf, *x);
}

sw buf_inspect_plist_paren (s_buf *buf, const p_list *x)
{
  return buf_inspect_list_paren(buf, *x);
}

sw buf_inspect_plist_size (s_pretty *pretty, const p_list *x)
{
  return buf_inspect_list_size(pretty, *x);
}

sw buf_inspect_c_pointer (s_buf *buf, const void *ptr)
{
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal(buf, (uw) ptr)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_pstruct (s_buf *buf, p_struct const *pstruct)
{
  return buf_inspect_struct(buf, *pstruct);
}

sw buf_inspect_pstruct_size (s_pretty *pretty, p_struct const *pstruct)
{
  return buf_inspect_struct_size(pretty, *pstruct);
}
  
sw buf_inspect_pstruct_type (s_buf *buf,
                             p_struct_type const *pstruct_type)
{
  return buf_inspect_struct_type(buf, *pstruct_type);
}
  
sw buf_inspect_pstruct_type_size (s_pretty *pretty,
                                  p_struct_type const *pstruct_type)
{
  return buf_inspect_struct_type_size(pretty, *pstruct_type);
}

sw buf_inspect_pointer (s_buf *buf, const s_pointer *pointer)
{
  s_env *env;
  sw r;
  sw result = 0;
  assert(buf);
  env = env_global();
  assert(env);
  if ((r = buf_inspect_paren_sym(buf, pointer->pointer_type)) < 0)
    return r;
  result += r;
  if (env->bool_ptr) {
    if (pointer->ptr.uw) {
      if ((r = buf_write_1(buf, " 1")) < 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_write_1(buf, " 0")) < 0)
        return r;
      result += r;
    }
    return result;
  }
  if ((r = buf_write_1(buf, " 0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal(buf, pointer->ptr.uw)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_pointer_size (s_pretty *pretty, const s_pointer *pointer)
{
  s_env *env;
  sw r;
  sw result = 0;
  assert(pretty);
  assert(pointer);
  env = env_global();
  assert(env);
  if ((r = buf_inspect_paren_sym_size(pretty,
                                      pointer->pointer_type)) < 0)
    return r;
  result += r;
  if (env->bool_ptr) {
    if (pointer->ptr.uw) {
      if ((r = buf_write_1_size(pretty, " 1")) < 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_write_1_size(pretty, " 0")) < 0)
        return r;
      result += r;
    }
    return result;
  }
  if ((r = buf_write_1_size(pretty, " 0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal_size(pretty,
                                           pointer->ptr.uw)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_psym (s_buf *buf, p_sym const *psym)
{
  return buf_inspect_sym(buf, *psym);
}

sw buf_inspect_psym_size (s_pretty *pretty, p_sym const *psym)
{
  return buf_inspect_sym_size(pretty, *psym);
}

sw buf_inspect_ptag (s_buf *buf, const p_tag *ptag)
{
  sw r;
  sw result = 0;
  assert(buf);
  if ((r = buf_write_1(buf, "@0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal(buf, (uw) ptag)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ptag_size (s_pretty *pretty, const p_tag *ptag)
{
  sw r;
  sw result = 0;
  (void) ptag;
  if ((r = buf_write_1_size(pretty, "@0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal_size(pretty, (uw) ptag)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ptr (s_buf *buf, const u_ptr_w *ptr)
{
  s_env *env;
  sw r;
  sw result = 0;
  assert(buf);
  env = env_global();
  assert(env);
  if (env->bool_ptr) {
    if (ptr->uw) {
      if ((r = buf_write_1(buf, "(Ptr) 1")) < 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_write_1(buf, "(Ptr) 0")) < 0)
        return r;
      result += r;
    }
    return result;
  }
  if ((r = buf_write_1(buf, "(Ptr) 0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal(buf, ptr->uw)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ptr_free (s_buf *buf, const u_ptr_w *ptr_free)
{
  s_env *env;
  sw r;
  sw result = 0;
  assert(buf);
  assert(ptr_free);
  env = env_global();
  assert(env);
  if (env->bool_ptr) {
    if (ptr_free->uw) {
      if ((r = buf_write_1(buf, "(PtrFree) 1")) < 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_write_1(buf, "(PtrFree) 0")) < 0)
        return r;
      result += r;
    }
    return result;
  }
  if ((r = buf_write_1(buf, "(PtrFree) 0x")) < 0)
    return r;
  result += r;
  r = buf_inspect_uw_hexadecimal(buf, ptr_free->uw);
  if (r < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ptr_free_size (s_pretty *pretty, const u_ptr_w *ptr)
{
  s_env *env;
  sw r;
  sw result = 0;
  env = env_global();
  assert(env);
  if (env->bool_ptr) {
    if (ptr->uw) {
      if ((r = buf_write_1_size(pretty, "(PtrFree) 1")) < 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_write_1_size(pretty, "(PtrFree) 0")) < 0)
        return r;
      result += r;
    }
    return result;
  }
  if ((r = buf_write_1_size(pretty, "(PtrFree) 0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal_size(pretty, ptr->uw)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ptr_size (s_pretty *pretty, const u_ptr_w *ptr)
{
  s_env *env;
  sw r;
  sw result = 0;
  env = env_global();
  assert(env);
  if (env->bool_ptr) {
    if (ptr->uw) {
      if ((r = buf_write_1_size(pretty, "(Ptr) 1")) < 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_write_1_size(pretty, "(Ptr) 0")) < 0)
        return r;
      result += r;
    }
    return result;
  }
  if ((r = buf_write_1_size(pretty, "(Ptr) 0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal_size(pretty, ptr->uw)) < 0)
    return r;
  result += r;
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
  result += r;
  r = result;
  goto clean;
 restore:
  buf_save_restore_wpos(buf, &save);
 clean:
  buf_save_clean(buf, &save);
  return r;
}

sw buf_inspect_quote_size (s_pretty *pretty, const s_quote *quote)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, "quote ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag_size(pretty, quote->tag)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_ratio (s_buf *buf, const s_ratio *ratio)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_inspect_integer_decimal(buf, &ratio->numerator)) < 0)
    goto restore;
  result += r;
  if ((r = buf_write_1(buf, "/")) < 0)
    goto restore;
  result += r;
  if ((r = buf_inspect_integer_decimal(buf, &ratio->denominator)) < 0)
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

sw buf_inspect_ratio_size (s_pretty *pretty, const s_ratio *ratio)
{
  sw r;
  sw result = 0;
  if ((r = buf_inspect_integer_decimal_size(pretty,
                                            &ratio->numerator)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, "/")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_integer_decimal_size(pretty,
                                            &ratio->denominator)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_stacktrace (s_buf *buf, p_list stacktrace)
{
  p_list arg;
  sw count = 10;
  sw depth;
  sw i;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  p_list reverse = NULL;
  p_list s;
  s_tag **trace;
  assert(buf);
  depth = list_length(stacktrace);
  if (depth < count)
    count = depth;
  if (! count)
    return 0;
  if (! (trace = alloc(count * sizeof(s_tag *))))
    return -1;
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent_at_column(&buf->pretty, 0);
  if ((r = buf_write_1(buf, "Stacktrace:\n")) < 0) {
    pretty_save_clean(&pretty_save, &buf->pretty);
    return r;
  }
  i = 0;
  s = stacktrace;
  while (s && i < count) {
    trace[i] = &s->tag;
    s = list_next(s);
    i++;
  }
  while (i--) {
    if ((r = buf_inspect_sw_decimal(buf, i)) < 0)
      goto clean;
    result += r;
    if ((r = buf_write_1(buf, ": ")) < 0)
      goto clean;
    result += r;
    if (trace[i]->type == TAG_PLIST) {
      if ((r = buf_inspect_tag(buf, &trace[i]->data.plist->tag)) < 0)
        goto clean;
      result += r;
      if ((r = buf_write_1(buf, "(")) < 0)
        goto clean;
      result += r;
      arg = list_next(trace[i]->data.plist);
      while (arg) {
        if ((r = buf_inspect_tag(buf, &arg->tag)) < 0)
          goto clean;
        result += r;
        arg = list_next(arg);
        if (arg) {
          if ((r = buf_write_1(buf, ", ")) < 0)
            goto clean;
          result += r;
        }
      }
      if ((r = buf_write_1(buf, ")\n")) < 0)
        goto clean;
      result += r;
    }
    else {
      if ((r = buf_write_1(buf, "???\n")) < 0)
        goto clean;
      result += r;
    }
  }
  r = result;
 clean:
  list_delete_all(reverse);
  pretty_save_clean(&pretty_save, &buf->pretty);
  return r;
}

sw buf_inspect_stacktrace_size (s_pretty *pretty,
                                const s_list *stacktrace);

sw buf_inspect_str (s_buf *buf, const s_str *str)
{
  bool b = false;
  sw r;
  sw result = 0;
  s_buf_save save;
  assert(buf);
  assert(str);
  if (! str_has_reserved_characters(str, &b))
    return -1;
  if (b)
    return buf_inspect_str_reserved(buf, str, true);
  buf_save_init(buf, &save);
  if ((r = buf_write_1(buf, "\"")) <= 0)
    goto clean;
  result += r;
  if ((r = buf_write_str_without_indent(buf, str)) < 0)
    goto restore;
  result += r;
  if ((r = buf_write_1(buf, "\"")) <= 0)
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
  if ((r = buf_write_1(buf, "\\")) != 1) {
    r = -1;
    goto clean;
  }
  result += r;
  if ((r = buf_write_1(buf, "x")) != 1)
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

sw buf_inspect_str_byte_size (s_pretty *pretty, const u8 *byte)
{
  (void) byte;
  (void) pretty;
  return 4;
}

sw buf_inspect_str_character (s_buf *buf, character c)
{
  char b[4];
  s_buf char_buf = {0};
  int i;
  int j;
  sw r;
  sw result = 0;
  sw result1 = 0;
  s_buf_save save;
  if (! str_character_is_reserved(c))
    return buf_write_character_utf8(buf, c);
  buf_save_init(buf, &save);
  if ((r = buf_write_1(buf, "\\")) <= 0)
    goto restore;
  result += r;
  switch (c) {
  case '\0':
    if ((r = buf_write_1(buf, "0")) <= 0)
      goto restore;
    break;
  case '\n':
    if ((r = buf_write_character_utf8(buf, 'n')) <= 0)
      goto restore;
    break;
  case '\r':
    if ((r = buf_write_character_utf8(buf, 'r')) <= 0)
      goto restore;
    break;
  case '\t':
    if ((r = buf_write_character_utf8(buf, 't')) <= 0)
      goto restore;
    break;
  case '\v':
    if ((r = buf_write_character_utf8(buf, 'v')) <= 0)
      goto restore;
    break;
  case '\"':
    if ((r = buf_write_character_utf8(buf, '"')) <= 0)
      goto restore;
    break;
  case '\'':
    if ((r = buf_write_character_utf8(buf, '\'')) <= 0)
      goto restore;
    break;
  case '\\':
    if ((r = buf_write_character_utf8(buf, '\\')) <= 0)
      goto restore;
    break;
  default:
    buf_init(&char_buf, false, sizeof(b), b);
    if ((r = buf_write_character_utf8(&char_buf, c)) <= 0) {
      buf_clean(&char_buf);
      goto restore;
    }
    i = r - 1;
    j = 0;
    if ((r = buf_write_character_utf8(buf, 'x')) != 1) {
      buf_clean(&char_buf);
      goto restore;
    }
    result1 += r;
    if ((r = buf_u8_to_hex(buf, &char_buf.ptr.pu8[j++])) != 2) {
      buf_clean(&char_buf);
      goto restore;
    }
    result1 += r;
    while (i--) {
      if ((r = buf_write_1(buf, "\\x")) != 2) {
	buf_clean(&char_buf);
        goto restore;
      }
      result1 += r;
      if ((r = buf_u8_to_hex(buf, &char_buf.ptr.pu8[j++])) != 2) {
	buf_clean(&char_buf);
        goto restore;
      }
      result1 += r;
    }
    buf_clean(&char_buf);
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

sw buf_inspect_str_character_size (s_pretty *pretty, character c)
{
  char b[4];
  s_buf char_buf;
  int i;
  sw r;
  sw result = 0;
  sw result1 = 0;
  if (c < UCD_MAX && ! str_character_is_reserved(c))
    return buf_write_character_utf8_size(pretty, c);
  if ((r = buf_write_1_size(pretty, "\\")) <= 0) {
    err_puts("buf_inspect_str_character_size: buf_write_1_size 1");
    assert(! "buf_inspect_str_character_size: buf_write_1_size 1");
    goto restore;
  }
  result += r;
  switch (c) {
  case '\0':
    if ((r = buf_write_1_size(pretty, "0")) <= 0) {
      err_puts("buf_inspect_str_character_size: buf_write_1_size 2");
      assert(! "buf_inspect_str_character_size: buf_write_1_size 2");
      goto restore;
    }
    break;
  case '\n':
    if ((r = buf_write_character_utf8_size(pretty, 'n')) <= 0)
      goto restore;
    break;
  case '\r':
    if ((r = buf_write_character_utf8_size(pretty, 'r')) <= 0)
      goto restore;
    break;
  case '\t':
    if ((r = buf_write_character_utf8_size(pretty, 't')) <= 0)
      goto restore;
    break;
  case '\v':
    if ((r = buf_write_character_utf8_size(pretty, 'v')) <= 0)
      goto restore;
    break;
  case '\"':
    if ((r = buf_write_character_utf8_size(pretty, '"')) <= 0)
      goto restore;
    break;
  case '\'':
    if ((r = buf_write_character_utf8_size(pretty, '\'')) <= 0)
      goto restore;
    break;
  case '\\':
    if ((r = buf_write_character_utf8_size(pretty, '\\')) <= 0)
      goto restore;
    break;
  default:
    buf_init(&char_buf, false, sizeof(b), b);
    if ((r = buf_write_character_utf8(&char_buf, c)) <= 0) {
      err_puts("buf_inspect_str_character_size: buf_write_character_utf8");
      assert(! "buf_inspect_str_character_size: buf_write_character_utf8");
      buf_clean(&char_buf);
      goto restore;
    }
    i = r - 1;
    if ((r = buf_write_character_utf8_size(pretty, 'x')) != 1) {
      err_puts("buf_inspect_str_character_size: buf_write_character_utf8_size");
      assert(! "buf_inspect_str_character_size: buf_write_character_utf8_size");
      buf_clean(&char_buf);
      goto restore;
    }
    result1 += r;
    r = 2;
    result1 += r;
    while (i--) {
      if ((r = buf_write_1_size(pretty, "\\x")) != 2) {
        buf_clean(&char_buf);
        goto restore;
      }
      result1 += r;
      r = 2;
      result1 += r;
    }
    r = result1;
    buf_clean(&char_buf);
  }
  result += r;
  r = result;
  goto clean;
 restore:
  if (r >= 0)
    r = -1;
 clean:
  return r;
}

sw buf_inspect_str_eval (s_buf *buf, const s_list *list)
{
  const s_list *l;
  sw r;
  sw result = 0;
  const s_tag *tag;
  if ((r = buf_write_1(buf, "\"")) <= 0)
    return r;
  result += r;
  l = list;
  while (l) {
    if (l->tag.type == TAG_STR) {
      if ((r = buf_inspect_str_reserved(buf, &l->tag.data.str,
                                        false)) <= 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_write_1(buf, "#{")) <= 0)
        return r;
      result += r;
      tag = &l->tag;
      if (tag_is_cast(tag, &g_sym_Str))
        tag = &list_next(tag->data.pcall->arguments)->tag;
      if ((r = buf_inspect_tag(buf, tag)) <= 0)
        return r;
      result += r;
      if ((r = buf_write_1(buf, "}")) <= 0)
        return r;
      result += r;
    }
    l = list_next(l);
  }
  if ((r = buf_write_1(buf, "\"")) <= 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_str_eval_size (s_pretty *pretty, const s_list *list)
{
  const s_list *l;
  sw r;
  sw result = 0;
  const s_tag *tag;
  if ((r = buf_write_1_size(pretty, "\"")) <= 0)
    return r;
  result += r;
  l = list;
  while (l) {
    if (l->tag.type == TAG_STR) {
      if ((r = buf_inspect_str_reserved_size(pretty, &l->tag.data.str,
                                             false)) <= 0)
        return r;
      result += r;
    }
    else {
      if ((r = buf_write_1_size(pretty, "#{")) <= 0)
        return r;
      result += r;
      tag = &l->tag;
      if (tag_is_cast(tag, &g_sym_Str))
        tag = &list_next(tag->data.pcall->arguments)->tag;
      if ((r = buf_inspect_tag_size(pretty, tag)) <= 0)
        return r;
      result += r;
      if ((r = buf_write_1_size(pretty, "}")) <= 0)
        return r;
      result += r;
    }
    l = list_next(l);
  }
  if ((r = buf_write_1_size(pretty, "\"")) <= 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_str_hex (s_buf *buf, const s_str *str)
{
  uw i;
  s_pretty_save pretty_save = {0};
  sw r;
  sw result = 0;
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent_from_column(&buf->pretty, 0);
  if ((r = buf_write_1(buf, "\"")) <= 0)
    goto clean;
  result += r;
  i = 0;
  while (i < str->size) {
    if ((r = buf_write_1(buf, "\\x")) != 2)
      goto clean;
    result += r;
    if ((r = buf_inspect_u8_hexadecimal_pad(buf, 2, '0',
                                            str->ptr.pu8[i])) != 2)
      goto clean;
    result += r;
    i++;
    if (i < str->size && ! (i % 8)) {
      if ((r = buf_write_1(buf, "\"\n\"")) <= 0)
        goto clean;
      result += r;
    }
  }
  if ((r = buf_write_1(buf, "\"")) <= 0)
    goto clean;
  result += r;
  r = result;
 clean:
  pretty_save_clean(&pretty_save, &buf->pretty);
  return r;
}

sw buf_inspect_str_hex_size (s_pretty *pretty, const s_str *str)
{
  uw i;
  s_pretty_save pretty_save = {0};
  sw r;
  sw result = 0;
  pretty_save_init(&pretty_save, pretty);
  pretty_indent_from_column(pretty, 0);
  if ((r = buf_write_1_size(pretty, "\"")) <= 0)
    goto clean;
  result += r;
  i = 0;
  while (i < str->size) {
    if ((r = buf_write_1_size(pretty, "\\x")) != 2)
      goto clean;
    result += r;
    r = 2; // buf_inspect_u8_hexadecimal_pad_size == size
    result += r;
    i++;
    if (i < str->size && ! (i % 8)) {
      if ((r = buf_write_1_size(pretty, "\"\n\"")) <= 0)
        goto clean;
      result += r;
    }
  }
  if ((r = buf_write_1_size(pretty, "\"")) <= 0)
    goto clean;
  result += r;
  r = result;
 clean:
  pretty_save_clean(&pretty_save, pretty);
  return r;
}

/* keep in sync with buf_inspect_str_reserved_size */
sw buf_inspect_str_reserved (s_buf *buf, const s_str *str, bool quotes)
{
  u8 byte;
  character c;
  sw r = 1;
  sw result = 0;
  s_str s;
  s_buf_save save;
  buf_save_init(buf, &save);
  if (quotes) {
    if ((r = buf_write_1(buf, "\"")) <= 0)
      goto clean;
    result += r;
  }
  s = *str;
  while (r) {
    if ((r = str_read_character_utf8(&s, &c)) > 0) {
      if ((r = buf_inspect_str_character(buf, c)) <= 0)
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
  if (quotes) {
    if ((r = buf_write_1(buf, "\"")) <= 0)
      goto restore;
    result += r;
  }
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
sw buf_inspect_str_reserved_size (s_pretty *pretty, const s_str *str,
                                  bool quotes)
{
  u8 byte;
  character c;
  sw r = 1;
  sw result = 0;
  s_str s;
  if (quotes) {
    if ((r = buf_write_1_size(pretty, "\"")) <= 0) {
      err_puts("buf_inspect_str_reserved_size: buf_write_1_size 1");
      assert(! "buf_inspect_str_reserved_size: buf_write_1_size 1");
      return r;
    }
    result += r;
  }
  s = *str;
  while (r) {
    if ((r = str_read_character_utf8(&s, &c)) > 0) {
      if ((r = buf_inspect_str_character_size(pretty, c)) <= 0) {
        err_puts("buf_inspect_str_reserved_size:"
                 " buf_inspect_str_character_size");
        assert(!("buf_inspect_str_reserved_size:"
                 " buf_inspect_str_character_size"));
        goto restore;
      }
      result += r;
    }
    else if ((r = str_read_u8(&s, &byte)) < 0) {
      err_puts("buf_inspect_str_reserved_size: str_read_u8");
      assert(!("buf_inspect_str_reserved_size: str_read_u8"));
      goto restore;
    }
    else if (r) {
      if ((r = buf_inspect_str_byte_size(pretty, &byte)) <= 0) {
        err_puts("buf_inspect_str_reserved_size:"
                 " buf_inspect_str_byte_size");
        assert(!("buf_inspect_str_reserved_size:"
                 " buf_inspect_str_byte_size"));
        goto restore;
      }
      result += r;
    }
  }
  if (quotes) {
    if ((r = buf_write_1_size(pretty, "\"")) <= 0) {
      err_puts("buf_inspect_str_reserved_size: buf_write_1_size 2");
      assert(! "buf_inspect_str_reserved_size: buf_write_1_size 2");
      goto restore;
    }
    result += r;
  }
  return result;
 restore:
  return -1;
}

sw buf_inspect_str_size (s_pretty *pretty, const s_str *str)
{
  bool b;
  sw r;
  sw result = 0;
  if (! str_has_reserved_characters(str, &b)) {
    err_puts("buf_inspect_str_size: str_has_reserved_characters");
    assert(! "buf_inspect_str_size: str_has_reserved_characters");
    return -1;
  }
  if (b)
    return buf_inspect_str_reserved_size(pretty, str, true);
  if ((r = buf_write_1_size(pretty, "\"")) < 0) {
    err_puts("buf_inspect_str_size: buf_write_1_size 1");
    assert(! "buf_inspect_str_size: buf_write_1_size 1");
    return r;
  }
  result += r; 
  if ((r = buf_write_str_without_indent_size(pretty, str)) < 0) {
    err_puts("buf_inspect_str_size: buf_write_str_without_indent_size");
    assert(! "buf_inspect_str_size: buf_write_str_without_indent_size");
    return r;
  }
  result += r;
  if ((r = buf_write_1_size(pretty, "\"")) < 0) {
    err_puts("buf_inspect_str_size: buf_write_1_size 2");
    assert(! "buf_inspect_str_size: buf_write_1_size 2");
    return r;
  }
  result += r;
  return result;
}

sw buf_inspect_struct (s_buf *buf, const s_struct *s)
{
  bool display;
  uw   display_last = 0;
  uw i;
  s_tag *k;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  const s_sym *type;
  assert(buf);
  assert(s);
  assert(sym_is_module(s->pstruct_type->module));
  if ((r = buf_write_1(buf, "%")) < 0) {
    assert(! "buf_inspect_struct: buf_write_1: %");
    return r;
  }
  result += r;
  if (! sym_is_module(s->pstruct_type->module)) {
    err_puts("buf_inspect_struct: "
             "sym_is_module(s->pstruct_type->module)");
    assert(! "buf_inspect_struct: "
             "sym_is_module(s->pstruct_type->module)");
    return -1;
  }
  if ((r =
       buf_write_str_without_indent(buf, &s->pstruct_type->module->str))
      < 0) {
    assert(! "buf_inspect_struct: "
             "buf_write_str_without_indent: module");
    return r;
  }
  result += r;
  if ((r = buf_write_1(buf, "{")) < 0) {
    assert(! "buf_inspect_struct: buf_write_1: {");
    return r;
  }
  result += r;
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent_from_column(&buf->pretty, 0);
  if (s->data || s->tag) {
    i = 0;
    while (i < s->pstruct_type->map.count) {
      if (s->data ||
          compare_tag(s->tag + i, s->pstruct_type->map.value + i))
        display_last = i;
      i++;
    }
    i = 0;
    while (i < s->pstruct_type->map.count) {
      display = s->data ||
        compare_tag(s->tag + i, s->pstruct_type->map.value + i);
      if (display) {
        k = s->pstruct_type->map.key + i;
        if (k->type != TAG_PSYM) {
          err_write_1("buf_inspect_struct: key type is not a symbol: ");
          err_inspect_tag(k);
          err_write_1(" (");
          err_write_1(tag_type_to_string(k->type));
          err_puts(")");
          assert(k->type == TAG_PSYM);
          r = -1;
          goto clean;
        }
        if (sym_has_reserved_characters(k->data.psym)) {
          if ((r = buf_inspect_str(buf, &k->data.psym->str)) < 0) {
            assert(! "buf_inspect_struct: buf_inspect_str: k");
            goto clean;
          }
        }
        else {
          r = buf_write_str_without_indent(buf, &k->data.psym->str);
          if (r < 0) {
            assert(! "buf_inspect_struct: "
                     "buf_write_str_without_ident: k");
            goto clean;
          }
        }
        result += r;
        if ((r = buf_write_1(buf, ": ")) < 0) {
          assert(! "buf_inspect_struct: buf_write_1(\": \")");
          goto clean;
        }
        result += r;
        if (s->data) {
          if (! tag_type_var(s->pstruct_type->map.value + i, &type)) {
            assert(! "buf_inspect_struct: tag_type_var");
            goto clean;
          }
          assert(s->pstruct_type->offset[i] < s->pstruct_type->size);
          if ((r = data_buf_inspect(buf, type, (char *) s->data +
                                    s->pstruct_type->offset[i])) < 0) {
            assert(! "buf_inspect_struct: data_buf_inspect");
            goto clean;
          }
          result += r;
        }
        else if (s->tag) {
          if ((r = buf_inspect_tag(buf, s->tag + i)) < 0) {
            assert(! "buf_inspect_struct: buf_inspect_tag");
            goto clean;
          }
          result += r;
        }
      }
      if (display &&
          i < display_last &&
          i < s->pstruct_type->map.count - 1) {
        if ((r = buf_write_1(buf, ",\n")) < 0) {
          assert(! "buf_inspect_struct: buf_write_1(\",\\n\")");
          goto clean;
        }
        result += r;
      }
      i++;
    }
  }
  pretty_save_clean(&pretty_save, &buf->pretty);
  if ((r = buf_write_1(buf, "}")) < 0)
    return r;
  result += r;
  return result;
 clean:
  pretty_save_clean(&pretty_save, &buf->pretty);
  return -1;
}

sw buf_inspect_struct_size (s_pretty *pretty, const s_struct *s)
{
  bool display;
  uw   display_last = 0;
  uw i;
  s_tag *k;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  const s_sym *type;
  assert(pretty);
  assert(s);
  assert(sym_is_module(s->pstruct_type->module));
  if ((r = buf_write_1_size(pretty, "%")) < 0) {
    assert(! "buf_inspect_struct_size: buf_write_1: %");
    return r;
  }
  result += r;
  if (! sym_is_module(s->pstruct_type->module)) {
    err_puts("buf_inspect_struct_size: "
             "sym_is_module(s->pstruct_type->module)");
    assert(! "buf_inspect_struct_size: "
             "sym_is_module(s->pstruct_type->module)");
    return -1;
  }
  if ((r = buf_write_str_without_indent_size
       (pretty, &s->pstruct_type->module->str)) < 0) {
    assert(! "buf_inspect_struct_size: "
             "buf_write_str_without_indent: module");
    return r;
  }
  result += r;
  if ((r = buf_write_1_size(pretty, "{")) < 0) {
    assert(! "buf_inspect_struct_size: buf_write_1: {");
    return r;
  }
  result += r;
  pretty_save_init(&pretty_save, pretty);
  pretty_indent_from_column(pretty, 0);
  if (s->data || s->tag) {
    i = 0;
    while (i < s->pstruct_type->map.count) {
      if (s->data ||
          compare_tag(s->tag + i, s->pstruct_type->map.value + i))
        display_last = i;
      i++;
    }
    i = 0;
    while (i < s->pstruct_type->map.count) {
      display = s->data ||
        compare_tag(s->tag + i, s->pstruct_type->map.value + i);
      if (display) {
        k = s->pstruct_type->map.key + i;
        if (k->type != TAG_PSYM) {
          err_write_1("buf_inspect_struct_size: "
                      "key type is not a symbol: ");
          err_inspect_tag(k);
          err_write_1(" (");
          err_write_1(tag_type_to_string(k->type));
          err_puts(")");
          assert(k->type == TAG_PSYM);
          r = -1;
          goto clean;
        }
        if (sym_has_reserved_characters(k->data.psym)) {
          if ((r = buf_inspect_str_size(pretty,
                                        &k->data.psym->str)) < 0) {
            assert(! "buf_inspect_struct_size: buf_inspect_str: k");
            goto clean;
          }
        }
        else
          if ((r = buf_write_str_without_indent_size
               (pretty, &k->data.psym->str)) < 0) {
            assert(! "buf_inspect_struct_size: "
                     "buf_write_str_without_ident: k");
            goto clean;
          }
        result += r;
        if ((r = buf_write_1_size(pretty, ": ")) < 0)
          goto clean;
        result += r;
        if (s->data) {
          if (s->pstruct_type->map.value[i].type == TAG_PVAR)
            type = s->pstruct_type->map.value[i].data.pvar->type;
          else if (! tag_type(s->pstruct_type->map.value + i, &type)) {
            assert(! "buf_inspect_struct_size: tag_type");
            goto clean;
          }
          assert(s->pstruct_type->offset[i] < s->pstruct_type->size);
          if ((r = data_buf_inspect_size
                   (pretty, type, (char *) s->data +
                    s->pstruct_type->offset[i])) < 0) {
            assert(! "buf_inspect_struct_size: data_buf_inspect");
            goto clean;
          }
          result += r;
        }
        else if (s->tag) {
          if ((r = buf_inspect_tag_size(pretty, s->tag + i)) < 0) {
            assert(! "buf_inspect_struct_size: buf_inspect_tag");
            goto clean;
          }
          result += r;
        }
      }
      if (display &&
          i < display_last &&
          i < s->pstruct_type->map.count - 1) {
        if ((r = buf_write_1_size(pretty, ",\n")) < 0) {
          assert(! "buf_inspect_struct_size: buf_write_1(\",\\n\")");
          goto clean;
        }
        result += r;
      }
      i++;
    }
  }
  pretty_save_clean(&pretty_save, pretty);
  if ((r = buf_write_1_size(pretty, "}")) < 0)
    return r;
  result += r;
  return result;
 clean:
  pretty_save_clean(&pretty_save, pretty);
  return -1;
}

sw buf_inspect_struct_type (s_buf *buf, const s_struct_type *st)
{
  s_array offset_array;
  uw offset_array_dimension;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  assert(buf);
  assert(st);
  assert(sym_is_module(st->module));
  assert(st->offset);
  assert(st->size);
  if ((r = buf_write_1(buf, "%StructType{")) < 0)
    return r;
  result += r;
  pretty_save_init(&pretty_save, &buf->pretty);
  pretty_indent_from_column(&buf->pretty, 0);
  if ((r = buf_write_1(buf, "module: ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_sym(buf, st->module)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, ",\nmap: ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_map(buf, &st->map)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, ",\noffset: ")) < 0)
    return r;
  result += r;
  offset_array_dimension = st->map.count;
  array_init(&offset_array, sym_1("Uw[]"), 1, &offset_array_dimension);
  offset_array.data = st->offset;
  if ((r = buf_inspect_array(buf, &offset_array)) < 0) {
    array_clean(&offset_array);
    return r;
  }
  result += r;
  array_clean(&offset_array);
  if ((r = buf_write_1(buf, ",\nsize: ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw(buf, st->size)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1(buf, "}")) < 0)
    return r;
  result += r;
  pretty_save_clean(&pretty_save, &buf->pretty);
  return result;
}

sw buf_inspect_struct_type_size (s_pretty *pretty,
                                 const s_struct_type *st)
{
  s_array offset_array;
  uw offset_array_dimension;
  s_pretty_save pretty_save;
  sw r;
  sw result = 0;
  assert(pretty);
  assert(st);
  assert(sym_is_module(st->module));
  assert(st->offset);
  assert(st->size);
  if ((r = buf_write_1_size(pretty, "%StructType{")) < 0)
    return r;
  result += r;
  pretty_save_init(&pretty_save, pretty);
  pretty_indent_from_column(pretty, 0);
  if ((r = buf_write_1_size(pretty, "module: ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_sym_size(pretty, st->module)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, ",\nmap: ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_map_size(pretty, &st->map)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, ",\noffset: ")) < 0)
    return r;
  result += r;
  offset_array_dimension = st->map.count;
  array_init(&offset_array, &g_sym_Uw_brackets, 1,
             &offset_array_dimension);
  offset_array.data = st->offset;
  if ((r = buf_inspect_array_size(pretty, &offset_array)) < 0) {
    array_clean(&offset_array);
    return r;
  }
  result += r;
  array_clean(&offset_array);
  if ((r = buf_write_1_size(pretty, ",\nsize: ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_size(pretty, st->size)) < 0)
    return r;
  result += r;
  if ((r = buf_write_1_size(pretty, "}")) < 0)
    return r;
  result += r;
  pretty_save_clean(&pretty_save, pretty);
  return result;
}

sw buf_inspect_sym (s_buf *buf, const s_sym *sym)
{
  sw r;
  sw size;
  assert(buf);
  assert(sym);
  if (sym->str.size == 0)
    return buf_write_1(buf, ":\"\"");
  if (sym_has_reserved_characters(sym))
    return buf_inspect_sym_reserved(buf, sym);
  if (sym_is_module(sym) || sym_is_array_type(sym))
    return buf_write_str(buf, &sym->str);
  size = sym->str.size + 1;
  if ((r = buf_write_1(buf, ":")) < 0 ||
      (r = buf_write_str(buf, &sym->str)) < 0)
    return r;
  return size;
}

sw buf_inspect_sym_size (s_pretty *pretty, const s_sym *sym)
{
  sw r;
  sw size;
  assert(pretty);
  assert(sym);
  if (sym->str.size == 0)
    return buf_write_1_size(pretty, ":\"\"");
  if (sym_has_reserved_characters(sym))
    return buf_inspect_sym_reserved_size(pretty, sym);
  if (sym_is_module(sym) || sym_is_array_type(sym))
    return buf_write_str_size(pretty, &sym->str);
  size = sym->str.size + 1;
  if ((r = buf_write_1_size(pretty, ":")) < 0 ||
      (r = buf_write_str_size(pretty, &sym->str)) < 0)
    return r;
  return size;
}

sw buf_inspect_sym_reserved (s_buf *buf, const s_sym *x)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1(buf, ":")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_str(buf, &x->str)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_sym_reserved_size (s_pretty *pretty, const s_sym *x)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, ":")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_str_size(pretty, &x->str)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_tag (s_buf *buf, const s_tag *tag)
{
  if (! tag)
    return buf_write_1(buf, "NULL");
  switch (tag->type) {
  case TAG_ARRAY:   return buf_inspect_array(buf, &tag->data.array);
  case TAG_DO_BLOCK:
    return buf_inspect_do_block(buf, &tag->data.do_block);
  case TAG_BOOL:    return buf_inspect_bool(buf, tag->data.bool_);
  case TAG_CHARACTER:
    return buf_inspect_character(buf, tag->data.character);
  case TAG_F32:     return buf_inspect_f32(buf, tag->data.f32);
  case TAG_F64:     return buf_inspect_f64(buf, tag->data.f64);
#if HAVE_F80
  case TAG_F80:     return buf_inspect_f80(buf, tag->data.f80);
#endif
#if HAVE_F128
  case TAG_F128:    return buf_inspect_f128(buf, tag->data.f128);
#endif
  case TAG_FACT:    return buf_inspect_fact(buf, &tag->data.fact);
  case TAG_IDENT:   return buf_inspect_ident(buf, &tag->data.ident);
  case TAG_INTEGER: return buf_inspect_integer(buf, &tag->data.integer);
  case TAG_MAP:     return buf_inspect_map(buf, &tag->data.map);
  case TAG_PCALL:   return buf_inspect_call(buf, tag->data.pcall);
  case TAG_PCALLABLE:
    return buf_inspect_callable(buf, tag->data.pcallable);
  case TAG_PCOMPLEX:
    return buf_inspect_complex(buf, tag->data.pcomplex);
  case TAG_PCOW:    return buf_inspect_cow(buf, tag->data.pcow);
  case TAG_PFACTS:  return buf_inspect_pfacts(buf, &tag->data.pfacts);
  case TAG_PLIST:   return buf_inspect_list(buf, tag->data.plist);
  case TAG_POINTER: return buf_inspect_pointer(buf, &tag->data.pointer);
  case TAG_PSTRUCT: return buf_inspect_struct(buf, tag->data.pstruct);
  case TAG_PSTRUCT_TYPE:
    return buf_inspect_struct_type(buf, tag->data.pstruct_type);
  case TAG_PSYM:    return buf_inspect_sym(buf, tag->data.psym);
  case TAG_PTAG:    return buf_inspect_ptag(buf, &tag->data.ptag);
  case TAG_PTR:     return buf_inspect_ptr(buf, &tag->data.ptr);
  case TAG_PTR_FREE:
    return buf_inspect_ptr_free(buf, &tag->data.ptr_free);
  case TAG_PVAR:    return buf_inspect_var(buf, tag->data.pvar);
  case TAG_QUOTE:   return buf_inspect_quote(buf, &tag->data.quote);
  case TAG_RATIO:   return buf_inspect_ratio(buf, &tag->data.ratio);
  case TAG_S8:      return buf_inspect_s8(buf, tag->data.s8);
  case TAG_S16:     return buf_inspect_s16(buf, tag->data.s16);
  case TAG_S32:     return buf_inspect_s32(buf, tag->data.s32);
  case TAG_S64:     return buf_inspect_s64(buf, tag->data.s64);
  case TAG_SW:      return buf_inspect_sw(buf, tag->data.sw);
  case TAG_STR:     return buf_inspect_str(buf, &tag->data.str);
  case TAG_TIME:    return buf_inspect_time(buf, &tag->data.time);
  case TAG_TUPLE:   return buf_inspect_tuple(buf, &tag->data.tuple);
  case TAG_U8:      return buf_inspect_u8(buf, tag->data.u8);
  case TAG_U16:     return buf_inspect_u16(buf, tag->data.u16);
  case TAG_U32:     return buf_inspect_u32(buf, tag->data.u32);
  case TAG_U64:     return buf_inspect_u64(buf, tag->data.u64);
  case TAG_UNQUOTE: return buf_inspect_unquote(buf, &tag->data.unquote);
  case TAG_UW:      return buf_inspect_uw(buf, tag->data.uw);
  case TAG_VOID:    return buf_inspect_void(buf, NULL);
  }
  err_puts("buf_inspect_tag: unknown tag_type");
  assert(! "buf_inspect_tag: unknown tag type");
  return -1;
}

sw buf_inspect_tag_size (s_pretty *pretty, const s_tag *tag)
{
  assert(tag);
  switch(tag->type) {
  case TAG_ARRAY:
    return buf_inspect_array_size(pretty, &tag->data.array);
  case TAG_DO_BLOCK:
    return buf_inspect_do_block_size(pretty, &tag->data.do_block);
  case TAG_BOOL:
    return buf_inspect_bool_size(pretty, tag->data.bool_);
  case TAG_CHARACTER:
    return buf_inspect_character_size(pretty, tag->data.character);
  case TAG_F32:
    return buf_inspect_f32_size(pretty, tag->data.f32);
  case TAG_F64:
    return buf_inspect_f64_size(pretty, tag->data.f64);
#if HAVE_F80
  case TAG_F80:
    return buf_inspect_f80_size(pretty, tag->data.f80);
#endif
#if HAVE_F128
  case TAG_F128:
    return buf_inspect_f128_size(pretty, tag->data.f128);
#endif
  case TAG_FACT:
    return buf_inspect_fact_size(pretty, &tag->data.fact);
  case TAG_IDENT:
    return buf_inspect_ident_size(pretty, &tag->data.ident);
  case TAG_INTEGER:
    return buf_inspect_integer_size(pretty, &tag->data.integer);
  case TAG_MAP:
    return buf_inspect_map_size(pretty, &tag->data.map);
  case TAG_PCALL:
    return buf_inspect_call_size(pretty, tag->data.pcall);
  case TAG_PCALLABLE:
    return buf_inspect_callable_size(pretty, tag->data.pcallable);
  case TAG_PCOMPLEX:
    return buf_inspect_complex_size(pretty, tag->data.pcomplex);
  case TAG_PCOW:
    return buf_inspect_cow_size(pretty, tag->data.pcow);
  case TAG_PFACTS:
    return buf_inspect_pfacts_size(pretty, &tag->data.pfacts);
  case TAG_PLIST:
    return buf_inspect_list_size(pretty, tag->data.plist);
  case TAG_PSTRUCT:
    return buf_inspect_struct_size(pretty, tag->data.pstruct);
  case TAG_PSTRUCT_TYPE:
    return buf_inspect_struct_type_size(pretty, tag->data.pstruct_type);
  case TAG_PSYM:
    return buf_inspect_sym_size(pretty, tag->data.psym);
  case TAG_PTAG:
    return buf_inspect_ptag_size(pretty, &tag->data.ptag);
  case TAG_POINTER:
    return buf_inspect_pointer_size(pretty, &tag->data.pointer);
  case TAG_PTR:
    return buf_inspect_ptr_size(pretty, &tag->data.ptr);
  case TAG_PTR_FREE:
    return buf_inspect_ptr_free_size(pretty, &tag->data.ptr_free);
  case TAG_PVAR:
    return buf_inspect_var_size(pretty, tag->data.pvar);
  case TAG_QUOTE:
    return buf_inspect_quote_size(pretty, &tag->data.quote);
  case TAG_RATIO:
    return buf_inspect_ratio_size(pretty, &tag->data.ratio);
  case TAG_S8:
    return buf_inspect_s8_size(pretty, tag->data.s8);
  case TAG_S16:
    return buf_inspect_s16_size(pretty, tag->data.s16);
  case TAG_S32:
    return buf_inspect_s32_size(pretty, tag->data.s32);
  case TAG_S64:
    return buf_inspect_s64_size(pretty, tag->data.s64);
  case TAG_STR:
    return buf_inspect_str_size(pretty, &tag->data.str);
  case TAG_SW:
    return buf_inspect_sw_size(pretty, tag->data.sw);
  case TAG_TIME:
    return buf_inspect_time_size(pretty, &tag->data.time);
  case TAG_TUPLE:
    return buf_inspect_tuple_size(pretty, &tag->data.tuple);
  case TAG_U8:
    return buf_inspect_u8_size(pretty, tag->data.u8);
  case TAG_U16:
    return buf_inspect_u16_size(pretty, tag->data.u16);
  case TAG_U32:
    return buf_inspect_u32_size(pretty, tag->data.u32);
  case TAG_U64:
    return buf_inspect_u64_size(pretty, tag->data.u64);
  case TAG_UNQUOTE:
    return buf_inspect_unquote_size(pretty, &tag->data.unquote);
  case TAG_UW:
    return buf_inspect_uw_size(pretty, tag->data.uw);
  case TAG_VOID:
    return buf_inspect_void_size(pretty, NULL);
  }
  err_puts("buf_inspect_tag_size: unknown tag type");
  assert(! "buf_inspect_tag_size: unknown tag type");
  return -1;
}

sw buf_inspect_tag_type (s_buf *buf, e_tag_type type)
{
  const char *s;
  assert(buf);
  s = tag_type_to_string(type);
  if (! s)
    return -1;
  return buf_write_1(buf, s);
}

sw buf_inspect_tag_type_size (s_pretty *pretty, e_tag_type type)
{
  const char *s;
  s = tag_type_to_string(type);
  if (! s)
    return -1;
  return buf_write_1_size(pretty, s);
}

sw buf_inspect_time (s_buf *buf, const s_time *time)
{
  sw r;
  sw result = 0;
  bool sec;
  if ((r = buf_write_1(buf, "%Time{")) < 0)
    return r;
  result += r;
  if ((sec = time->tag ?
       time->tag->type != TAG_SW || time->tag->data.sw :
       time->tv_sec)) {
    if ((r = buf_write_1(buf, "tv_sec: ")) < 0)
      return r;
    result += r;
    if (time->tag) {
      if ((r = buf_inspect_tag(buf, time->tag)) < 0)
        return r;
    }
    else {
      if ((r = buf_inspect_sw_decimal(buf, time->tv_sec)) < 0)
        return r;
    }
    result += r;
  }
  if (time->tag ?
      time->tag[1].type != TAG_SW || time->tag[1].data.sw :
      time->tv_nsec) {
    if (sec) {
      if ((r = buf_write_1(buf, ", ")) < 0)
        return r;
      result += r;
    }
    if ((r = buf_write_1(buf, "tv_nsec: ")) < 0)
      return r;
    result += r;
    if (time->tag) {
      if ((r = buf_inspect_tag(buf, time->tag + 1)) < 0)
        return r;
    }
    else {
      if ((r = buf_inspect_sw_decimal(buf, time->tv_nsec)) < 0)
        return r;
    }
    result += r;
  }
  if ((r = buf_write_1(buf, "}")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_time_size (s_pretty *pretty, const s_time *time)
{
  sw r;
  sw result = 0;
  bool sec;
  if ((r = buf_write_1_size(pretty, "%Time{")) < 0)
    return r;
  result += r;
  if ((sec = time->tag ?
       time->tag->type != TAG_SW || time->tag->data.sw :
       time->tv_sec)) {
    if ((r = buf_write_1_size(pretty, "tv_sec: ")) < 0)
      return r;
    result += r;
    if (time->tag) {
      if ((r = buf_inspect_tag_size(pretty, time->tag)) < 0)
        return r;
    }
    else {
      if ((r = buf_inspect_sw_decimal_size(pretty, time->tv_sec)) < 0)
        return r;
    }
    result += r;
  }
  if (time->tag ?
      time->tag[1].type != TAG_SW || time->tag[1].data.sw :
      time->tv_nsec) {
    if (sec) {
      if ((r = buf_write_1_size(pretty, ", ")) < 0)
        return r;
      result += r;
    }
    if ((r = buf_write_1_size(pretty, "tv_nsec: ")) < 0)
      return r;
    result += r;
    if (time->tag) {
      if ((r = buf_inspect_tag_size(pretty, time->tag + 1)) < 0)
        return r;
    }
    else {
      if ((r = buf_inspect_sw_decimal_size(pretty, time->tv_nsec)) < 0)
        return r;
    }
    result += r;
  }
  if ((r = buf_write_1_size(pretty, "}")) < 0)
    return r;
  result += r;
  return result;
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

sw buf_inspect_tuple_size (s_pretty *pretty, const s_tuple *tuple)
{
  u64 i = 0;
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, "{")) < 0)
    return r;
  result += r;
  if (tuple->count) {
    while (i < tuple->count - 1) {
      if ((r = buf_inspect_tag_size(pretty, tuple->tag + i)) < 0)
        return r;
      result += r;
      if ((r = buf_write_1_size(pretty, ", ")) < 0)
        return r;
      result += r;
      i++;
    }
    if ((r = buf_inspect_tag_size(pretty, tuple->tag + i)) < 0)
      return r;
    result += r;
  }
  if ((r = buf_write_1_size(pretty, "}")) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_unquote (s_buf *buf, const s_unquote *unquote)
{
  sw r;
  sw result = 0;
  s_buf_save save;
  buf_save_init(buf, &save);
  if ((r = buf_write_1(buf, "unquote ")) < 0)
    goto clean;
  result += r;
  if ((r = buf_inspect_tag(buf, unquote->tag)) < 0)
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

sw buf_inspect_unquote_size (s_pretty *pretty, const s_unquote *unquote)
{
  sw r;
  sw result = 0;
  if ((r = buf_write_1_size(pretty, "unquote ")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_tag_size(pretty, unquote->tag)) < 0)
    return r;
  result += r;
  return result;
}

sw buf_inspect_var (s_buf *buf, const s_var *var)
{
  sw r;
  sw result = 0;
  assert(buf);
  assert(var);
  assert(var->type);
  if (var->type == &g_sym_Tag) {
    if ((r = buf_write_1(buf, "?")) < 0)
      return r;
    result += r;
  }
  else {
    if ((r = buf_inspect_paren_sym(buf, var->type)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1(buf, " ?")) < 0)
      return r;
    result += r;
  }
  if (var->bound) {
    if ((r = buf_write_1(buf, " = ")) <= 0)
      return r;
    result += r;
    if ((r = buf_inspect_tag(buf, &var->tag)) <= 0)
      return r;
    result += r;
  }
  return result;
}

sw buf_inspect_var_size (s_pretty *pretty, const s_var *var)
{
  sw r;
  sw result = 0;
  assert(pretty);
  assert(var);
  assert(var->type);
  if (var->bound)
    return buf_inspect_tag_size(pretty, &var->tag);
  if (var->type == &g_sym_Tag) {
    if ((r = buf_write_1_size(pretty, "?")) < 0)
      return r;
    result += r;
  }
  else {
    if ((r = buf_inspect_paren_sym_size(pretty, var->type)) < 0)
      return r;
    result += r;
    if ((r = buf_write_1_size(pretty, " ?")) < 0)
      return r;
    result += r;
  }
  /*
  if ((r = buf_write_1_size(pretty, "0x")) < 0)
    return r;
  result += r;
  if ((r = buf_inspect_uw_hexadecimal_size(pretty, (uw) var)) < 0)
    return r;
  result += r;
  */
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

sw buf_inspect_void_size (s_pretty *pretty, const void *v)
{
  (void) v;
  return buf_write_1_size(pretty, "void");
}
