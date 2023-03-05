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
#include <err.h>
#include <stdlib.h>
#include "cfn.h"
#include "list.h"

s_cfn * cfn_set_arg_types (s_cfn *cfn, s_list *arg_types)
{
  s_list *a;
  ffi_type **arg_ffi_type = NULL;
  sw arity;
  ffi_cif cif;
  u8 i = 0;
  ffi_type *result_type;
  assert(cfn);
  if ((arity = list_length(arg_types))) {
    if (arity > 255) {
      assert(arity <= 255);
      errx(1, "cfn_set_arg_types: arity > 255");
    }
    if (! (arg_types = malloc(sizeof(ffi_type *) * arity)))
      err(1, "cfn_set_arg_types");
    a = arg_types;
    while (a) {
      assert(i < arity);
      arg_ffi_type[i] = cfn_arg_type_to_ffi_type(&a->tag);
      i++;
      a = list_next(a);
    }
  }
  cfn->arg_types = arg_types;
  cfn->arity = arity;
  return cfn;
}

ffi_type * cfn_arg_type_to_ffi_type (const s_tag *tag)
{
  assert(tag);
  assert(tag->type.type == TAG_SYM);
  if (tag->data.sym == sym_1("s8"))
    return &ffi_type_sint8;
  if (tag->data.sym == sym_1("s16"))
    return &ffi_type_sint16;
  if (tag->data.sym == sym_1("s32"))
    return &ffi_type_sint32;
  if (tag->data.sym == sym_1("s64"))
    return &ffi_type_sint64;
  if (tag->data.sym == sym_1("sw"))
    return &ffi_type_sint;
  if (tag->data.sym == sym_1("u8"))
    return &ffi_type_uint8;
  if (tag->data.sym == sym_1("u16"))
    return &ffi_type_uint16;
  if (tag->data.sym == sym_1("u32"))
    return &ffi_type_uint32;
  if (tag->data.sym == sym_1("u64"))
    return &ffi_type_uint64;
  if (tag->data.sym == sym_1("uw"))
    return &ffi_type_uint;
}


s_tag * cfn_apply (s_cfn *cfn, s_list *args) {
  void **arg_values;
  sw i;
  sw num_args;
  void* result;
  assert(cfn);
  assert(args);
  num_args = list_length(args);
  if (! num_args) {
    /* TODO */
    assert(! "todo");
    err(1, "todo");
    return NULL;
  }
  if (! (arg_values = malloc(sizeof(void *) * num_args)))
    err(1, "cfn_apply");
  while (args) {
    switch (args->tag.type.type) {
    case TAG_S8:
      arg_types[num_args] = &ffi_type_sint;
      arg_values[num_args] = args->data;
      num_args++;
      break;
    case FLOAT:
      arg_types[num_args] = &ffi_type_float;
      arg_values[num_args] = args->data;
      num_args++;
      break;
    case STRING:
      arg_types[num_args] = &ffi_type_pointer;
      arg_values[num_args] = &args->data;
      num_args++;
      break;
    }

    args = args->next;
  }

  ffi_type** final_arg_types = malloc(sizeof(ffi_type*) * num_args);
  void** final_arg_values = malloc(sizeof(void*) * num_args);

  for (int i = 0; i < num_args; i++) {
    final_arg_types[i] = arg_types[i];
    final_arg_values[i] = arg_values[i];
  }

  ffi_prep_cif_var(&cif, FFI_DEFAULT_ABI, num_args, NULL, final_arg_types);

  switch (((Node*)fn_ptr)->tag) {
  case INT:
    result_type = &ffi_type_sint;
    break;
  case FLOAT:
    result_type = &ffi_type_float;
    break;
  case STRING:
    result_type = &ffi_type_pointer;
    break;
  }

  ffi_call(&cif, ((Node*)fn_ptr)->data, result, final_arg_values);

  free(final_arg_types);
  free(final_arg_values);

  if (((Node*)fn_ptr)->tag == STRING) {
    char* result_copy = strdup(result);
    free_result(result, ((Node*)fn_ptr)->tag);
    result = result_copy;
  }

  return result;
}

void cfn_clean (s_cfn *cfn)
{
  assert(cfn);
  list_delete_all(cfn->arg_types);
}

s_cfn * cfn_copy (const s_cfn *cfn, s_cfn *dest)
{
  assert(cfn);
  assert(dest);
  dest->name = cfn->name;
  dest->arg_types = list_copy(cfn->arg_types);
  return dest;
}

s_cfn * cfn_init (s_cfn *cfn)
{
  assert(cfn);
  bzero(cfn, sizeof(s_cfn));
  return cfn;
}
