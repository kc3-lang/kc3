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
#include <dlfcn.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include "cfn.h"
#include "list.h"
#include "str.h"
#include "sym.h"

ffi_type * cfn_sym_to_ffi_type (const s_sym *sym);
s_tag * cfn_tag_init (s_tag *tag, const s_sym *type);
void * cfn_tag_to_ffi_value (s_tag *tag, const s_sym *type);

s_tag * cfn_apply (s_cfn *cfn, s_list *args, s_tag *dest)
{
  void **arg_values = NULL;
  s_list *cfn_arg_type;
  sw i;
  sw num_args;
  void* result;
  s_tag tmp;
  assert(cfn);
  num_args = list_length(args);
  if (cfn->arity != num_args + 1) {
    warnx("cfn_apply: invalid number of arguments, expected %d, have %ld",
          cfn->arity - 1, num_args);
    return NULL;
  }
  cfn_tag_init(&tmp, cfn->result_type);
  /* make result point to tmp value */
  result = cfn_tag_to_ffi_value(&tmp, cfn->result_type);
  if (args) {
    if (! (arg_values = malloc(sizeof(void *) * (num_args + 1))))
      err(1, "cfn_apply");
    cfn_arg_type = cfn->arg_types;
    i = 0;
    while (args) {
      assert(cfn_arg_type->tag.type.type == TAG_SYM);
      arg_values[i] = cfn_tag_to_ffi_value(&args->tag,
                                           cfn_arg_type->tag.data.sym);
      args = list_next(args);
      cfn_arg_type = list_next(cfn_arg_type);
      i++;
    }
    arg_values[i] = cfn_tag_to_ffi_value(&tmp,
                                         cfn_arg_type->tag.data.sym);
  }
  ffi_call(&cfn->cif, cfn->ptr.f, result, arg_values);
  free(arg_values);
  *dest = tmp;
  return dest;
}

void cfn_clean (s_cfn *cfn)
{
  assert(cfn);
  str_clean(&cfn->name);
  list_delete_all(cfn->arg_types);
}

s_cfn * cfn_copy (const s_cfn *cfn, s_cfn *dest)
{
  assert(cfn);
  assert(dest);
  str_copy(&cfn->name, &dest->name);
  list_copy(cfn->arg_types, &dest->arg_types);
  dest->arity = cfn->arity;
  dest->result_type = cfn->result_type;
  return dest;
}

s_cfn * cfn_init (s_cfn *cfn)
{
  assert(cfn);
  bzero(cfn, sizeof(s_cfn));
  return cfn;
}

s_cfn * cfn_link (s_cfn *cfn)
{
  assert(cfn);
  if (! (cfn->ptr.p = dlsym(RTLD_DEFAULT, cfn->name.ptr.ps8)))
    warnx("cfn_link: %s: %s", cfn->name.ptr.ps8, dlerror());
  return cfn;
}

s_cfn * cfn_set_type (s_cfn *cfn, s_list *arg_type,
                      const s_sym *result_type)
{
  s_list *a;
  ffi_type **arg_ffi_type = NULL;
  sw arity;
  ffi_cif cif;
  u8 i = 0;
  ffi_type *result_ffi_type;
  assert(cfn);
  result_ffi_type = cfn_sym_to_ffi_type(result_type);
  if ((arity = list_length(arg_type))) {
    if (arity > 255) {
      assert(arity <= 255);
      errx(1, "cfn_set_arg_types: arity > 255");
    }
    if (! (arg_ffi_type = malloc(sizeof(ffi_type *) * arity)))
      err(1, "cfn_set_arg_types");
    a = arg_type;
    while (a) {
      assert(i < arity);
      if (a->tag.type.type != TAG_SYM) {
        assert(! "cfn_set_type: invalid type");
        errx(1, "cfn_set_type: invalid type");
      }
      arg_ffi_type[i] = cfn_sym_to_ffi_type(a->tag.data.sym);
      i++;
      a = list_next(a);
    }
  }
  ffi_prep_cif(&cif, FFI_DEFAULT_ABI, arity, result_ffi_type, arg_ffi_type);  
  cfn->arg_types = arg_type;
  cfn->arity = arity;
  cfn->cif = cif;
  cfn->result_type = result_type;
  return cfn;
}

ffi_type * cfn_sym_to_ffi_type (const s_sym *sym)
{
  if (sym == sym_1("s8"))
    return &ffi_type_sint8;
  if (sym == sym_1("s16"))
    return &ffi_type_sint16;
  if (sym == sym_1("s32"))
    return &ffi_type_sint32;
  if (sym == sym_1("s64"))
    return &ffi_type_sint64;
  if (sym == sym_1("sw"))
    return &ffi_type_sint;
  if (sym == sym_1("u8"))
    return &ffi_type_uint8;
  if (sym == sym_1("u16"))
    return &ffi_type_uint16;
  if (sym == sym_1("u32"))
    return &ffi_type_uint32;
  if (sym == sym_1("u64"))
    return &ffi_type_uint64;
  if (sym == sym_1("uw"))
    return &ffi_type_uint;
  assert(! "cfn_sym_to_ffi_type: unknown type");
  errx(1, "cfn_sym_to_ffi_type: unknown type");
  return NULL;
}

e_tag_type cfn_sym_to_tag_type (const s_sym *sym)
{
  if (sym == sym_1("void"))
    return TAG_VOID;
  if (sym == sym_1("bool"))
    return TAG_BOOL;
  if (sym == sym_1("call"))
    return TAG_CALL;
  if (sym == sym_1("call_fn"))
    return TAG_CALL_FN;
  if (sym == sym_1("call_macro"))
    return TAG_CALL_MACRO;
  if (sym == sym_1("cfn"))
    return TAG_CFN;
  if (sym == sym_1("character"))
    return TAG_CHARACTER;
  if (sym == sym_1("f32"))
    return TAG_F32;
  if (sym == sym_1("f64"))
    return TAG_F64;
  if (sym == sym_1("fn"))
    return TAG_FN;
  if (sym == sym_1("ident"))
    return TAG_IDENT;
  if (sym == sym_1("integer"))
    return TAG_INTEGER;
  if (sym == sym_1("s64"))
    return TAG_S64;
  if (sym == sym_1("s32"))
    return TAG_S32;
  if (sym == sym_1("s16"))
    return TAG_S16;
  if (sym == sym_1("s8"))
    return TAG_S8;
  if (sym == sym_1("u8"))
    return TAG_U8;
  if (sym == sym_1("u16"))
    return TAG_U16;
  if (sym == sym_1("u32"))
    return TAG_U32;
  if (sym == sym_1("u64"))
    return TAG_U64;
  if (sym == sym_1("list"))
    return TAG_LIST;
  if (sym == sym_1("ptag"))
    return TAG_PTAG;
  if (sym == sym_1("quote"))
    return TAG_QUOTE;
  if (sym == sym_1("str"))
    return TAG_STR;
  if (sym == sym_1("sym"))
    return TAG_SYM;
  if (sym == sym_1("tag"))
    return TAG_VOID;
  if (sym == sym_1("tuple"))
    return TAG_TUPLE;
  assert(! "cfn_sym_to_tag_type: unknown type");
  errx(1, "cfn_sym_to_tag_type: unknown type: %s", sym->str.ptr.ps8);
  return TAG_VOID;
}

s_tag * cfn_tag_init (s_tag *tag, const s_sym *type)
{
  assert(tag);
  bzero(tag, sizeof(s_tag));
  tag->type.type = cfn_sym_to_tag_type(type);
  return tag;
}

const s_sym * cfn_tag_type_to_sym (e_tag_type tag_type)
{
  switch (tag_type) {
  case TAG_VOID:       return sym_1("void");
  case TAG_BOOL:       return sym_1("bool");
  case TAG_CALL:       return sym_1("call");
  case TAG_CALL_FN:    return sym_1("call_fn");
  case TAG_CALL_MACRO: return sym_1("call_macro");
  case TAG_CFN:        return sym_1("cfn");
  case TAG_CHARACTER:  return sym_1("character");
  case TAG_F32:        return sym_1("f32");
  case TAG_F64:        return sym_1("f64");
  case TAG_FN:         return sym_1("fn");
  case TAG_IDENT:      return sym_1("ident");
  case TAG_INTEGER:    return sym_1("integer");
  case TAG_S64:        return sym_1("s64");
  case TAG_S32:        return sym_1("s32");
  case TAG_S16:        return sym_1("s16");
  case TAG_S8:         return sym_1("s8");
  case TAG_U8:         return sym_1("u8");
  case TAG_U16:        return sym_1("u16");
  case TAG_U32:        return sym_1("u32");
  case TAG_U64:        return sym_1("u64");
  case TAG_LIST:       return sym_1("list");
  case TAG_PTAG:       return sym_1("ptag");
  case TAG_QUOTE:      return sym_1("quote");
  case TAG_STR:        return sym_1("str");
  case TAG_SYM:        return sym_1("sym");
  case TAG_TUPLE:      return sym_1("tuple");
  case TAG_VAR:        return sym_1("var");
  }
  assert(! "cfn_tag_type_to_sym: invalid tag type");
  errx(1, "cfn_tag_type_to_sym: invalid tag type");
  return NULL;
}

void * cfn_tag_to_ffi_value (s_tag *tag, const s_sym *type)
{
  if (type == sym_1("tag"))
    return tag;
  switch (tag->type.type) {
  case TAG_VOID:
    if (type == sym_1("void"))
      return NULL;
    goto invalid_type;
  case TAG_BOOL:
    if (type == sym_1("bool"))
      return &tag->data.bool;
    goto invalid_type;
  case TAG_CALL:
    if (type == sym_1("call"))
      return &tag->data.call;
    goto invalid_type;
  case TAG_CALL_FN:
    if (type == sym_1("call_fn"))
      return &tag->data.call;
    goto invalid_type;
  case TAG_CALL_MACRO:
    if (type == sym_1("call_macro"))
      return &tag->data.call;
    goto invalid_type;
  case TAG_CFN:
    if (type == sym_1("cfn"))
      return &tag->data.cfn;
    goto invalid_type;
  case TAG_CHARACTER:
    if (type == sym_1("character"))
      return &tag->data.character;
    goto invalid_type;
  case TAG_F32:
    if (type == sym_1("f32"))
      return &tag->data.f32;
    goto invalid_type;
  case TAG_F64:
    if (type == sym_1("f64"))
      return &tag->data.f64;
    goto invalid_type;
  case TAG_FN:
    if (type == sym_1("fn"))
      return tag->data.fn;
    goto invalid_type;
  case TAG_IDENT:
    if (type == sym_1("ident"))
      return &tag->data.ident;
    goto invalid_type;
  case TAG_INTEGER:
    if (type == sym_1("integer"))
      return &tag->data.integer;
    goto invalid_type;
  case TAG_S64:
    if (type == sym_1("s64"))
      return &tag->data.s64;
    goto invalid_type;
  case TAG_S32:
    if (type == sym_1("s32"))
      return &tag->data.s32;
    goto invalid_type;
  case TAG_S16:
    if (type == sym_1("s16"))
      return &tag->data.s16;
    goto invalid_type;
  case TAG_S8:
    if (type == sym_1("s8"))
      return &tag->data.s8;
    goto invalid_type;
  case TAG_U8:
    if (type == sym_1("u8"))
      return &tag->data.u8;
    goto invalid_type;
  case TAG_U16:
    if (type == sym_1("u16"))
      return &tag->data.u16;
    goto invalid_type;
  case TAG_U32:
    if (type == sym_1("u32"))
      return &tag->data.u32;
    goto invalid_type;
  case TAG_U64:
    if (type == sym_1("u64"))
      return &tag->data.u64;
    goto invalid_type;
  case TAG_LIST:
    if (type == sym_1("list"))
      return tag->data.list;
    goto invalid_type;
  case TAG_PTAG:
    if (type == sym_1("ptag"))
      return (void *) tag->data.ptag;
    goto invalid_type;
  case TAG_QUOTE:
    if (type == sym_1("quote"))
      return &tag->data.quote;
    goto invalid_type;
  case TAG_STR:
    if (type == sym_1("str"))
      return &tag->data.str;
    if (type == sym_1("char*"))
      return (void *) tag->data.str.ptr.ps8;
    goto invalid_type;
  case TAG_SYM:
    if (type == sym_1("sym"))
      return (void *) tag->data.sym;
    if (type == sym_1("str"))
      return (void *) &tag->data.sym->str;
    if (type == sym_1("char*"))
      return (void *) tag->data.sym->str.ptr.ps8;
    goto invalid_type;
  case TAG_TUPLE:
    if (type == sym_1("tuple"))
      return &tag->data.tuple;
    goto invalid_type;
  case TAG_VAR:
    goto invalid_type;
  }
  assert(! "cfn_tag_to_ffi_value: invalid tag type");
  errx(1, "cfn_tag_to_ffi_value: invalid tag type");
  return NULL;
 invalid_type:
  warnx("cfn_tag_to_ffi_value: cannot cast %s to %s",
        cfn_tag_type_to_sym(tag->type.type)->str.ptr.ps8,
        type->str.ptr.ps8);
  return NULL;  
}
