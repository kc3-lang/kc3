/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#include "alloc.h"
#include "assert.h"
#include <dlfcn.h>
#include <string.h>
#include "cfn.h"
#include "list.h"
#include "str.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

static s_tag * cfn_tag_init (s_tag *tag, const s_sym *type);

s_tag * cfn_apply (s_cfn *cfn, s_list *args, s_tag *dest)
{
  s_list *a;
  void **arg_pointers = NULL;
  void **arg_values = NULL;
  void **result_pointer = NULL;
  u8 arity;
  s_list *cfn_arg_types;
  sw i = 0;
  sw num_args;
  void *result = NULL;
  s_tag tmp;
  s_tag tmp2;
  assert(cfn);
  assert(cfn->arity == cfn->cif.nargs);
  num_args = list_length(args);
  arity = cfn->arity - (cfn->arg_result ? 1 : 0);
  if (arity != num_args) {
    err_write_1("cfn_apply: invalid number of arguments, expected ");
    err_inspect_u8(&arity);
    err_write_1(", have ");
    err_inspect_sw(&num_args);
    err_write_1("\n");
    return NULL;
  }
  cfn_tag_init(&tmp, cfn->result_type);
  if (cfn->cif.rtype == &ffi_type_pointer) {
    /* make result point to result_pointer */
    if (! tag_to_ffi_pointer(&tmp, cfn->result_type, (void **) &result_pointer))
      return NULL;
    result = &result_pointer;
  }
  else {
    /* make result point to tmp value */
    if (! tag_to_ffi_pointer(&tmp, cfn->result_type, result))
      return NULL;
  }
  if (cfn->arity) {
    arg_pointers = alloc((cfn->arity + 1) * sizeof(void *));
    if (! arg_pointers)
      return NULL;
    arg_values = alloc((cfn->arity + 1) * sizeof(void *));
    if (! arg_values) {
      free(arg_pointers);
      return NULL;
    }
    cfn_arg_types = cfn->arg_types;
    a = args;
    while (cfn_arg_types) {
      assert(cfn_arg_types->tag.type == TAG_SYM);
      if (cfn_arg_types->tag.data.sym == &g_sym_Result) {
        cfn_tag_init(&tmp2, cfn->result_type);
        if (cfn->cif.rtype == &ffi_type_pointer) {
          if (! tag_to_ffi_pointer(&tmp2, cfn->result_type, arg_pointers + i))
            goto ko;
          arg_values[i] = &arg_pointers[i];
        }
        else
          if (! tag_to_ffi_pointer(&tmp2, cfn->result_type, arg_values + i))
            goto ko;
      }
      else {
        if (cfn->cif.arg_types[i] == &ffi_type_pointer) {
          if (! tag_to_ffi_pointer(&a->tag, cfn_arg_types->tag.data.sym,
                                   arg_pointers + i))
            goto ko;
          arg_values[i] = &arg_pointers[i];
        }
        else
          if (! tag_to_ffi_pointer(&a->tag, cfn_arg_types->tag.data.sym,
                                   arg_values + i))
            goto ko;
        a = list_next(a);
      }
      cfn_arg_types = list_next(cfn_arg_types);
      i++;
    }
  }
  if (cfn->ptr.f) {
    ffi_call(&cfn->cif, cfn->ptr.f, result, arg_values);
    if (cfn->arg_result) {
      *dest = tmp2;
    }
    else
      *dest = tmp;
  }
  else {
    err_puts("cfn_apply: NULL function pointer");
    assert(! "cfn_apply: NULL function pointer");
    tag_init_void(dest);
  }
  free(arg_pointers);
  free(arg_values);
  return dest;
 ko:
  free(arg_pointers);
  free(arg_values);
  return NULL;
}

void cfn_clean (s_cfn *cfn)
{
  assert(cfn);
  list_delete_all(cfn->arg_types);
  if (cfn->cif.nargs)
    free(cfn->cif.arg_types);
}

void cfn_delete (s_cfn *cfn)
{
  assert(cfn);
  cfn_clean(cfn);
  free(cfn);
}

s_cfn * cfn_init (s_cfn *cfn, const s_sym *name, s_list *arg_types,
                  const s_sym *result_type)
{
  sw arity;
  s_cfn tmp = {0};
  assert(cfn);
  tmp.name = name;
  tmp.arg_types = arg_types;
  arity = list_length(tmp.arg_types);
  if (arity > 255) {
    err_puts("cfn_init: arity > 255");
    assert(arity <= 255);
    return NULL;
  }
  tmp.arity = arity;
  tmp.result_type = result_type;
  *cfn = tmp;
  return cfn;
}

s_cfn * cfn_init_cast (s_cfn *cfn, const s_tag *tag)
{
  switch (tag->type) {
  case TAG_CFN:
    return cfn_init_copy(cfn, &tag->data.cfn);
  default:
    break;
  }
  err_write_1("cfn_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  err_puts(" to Cfn");
  assert(! "cfn_init_cast: cannot cast to Cfn");
  return NULL;
}

s_cfn * cfn_init_copy (s_cfn *cfn, const s_cfn *src)
{
  s_cfn tmp = {0};
  assert(src);
  assert(cfn);
  tmp.name = src->name;
  tmp.arg_result = src->arg_result;
  if (! list_init_copy(&tmp.arg_types,
                       (const s_list * const *) &src->arg_types))
    return NULL;
  tmp.arity = src->arity;
  tmp.cif = src->cif;
  if (src->arity) {
    tmp.cif.arg_types = alloc((src->cif.nargs + 1) * sizeof(ffi_type *));
    if (! tmp.cif.arg_types) {
      list_delete_all(tmp.arg_types);
      return NULL;
    }
    memcpy(tmp.cif.arg_types, src->cif.arg_types,
           (src->cif.nargs + 1) * sizeof(ffi_type *));
  }
  tmp.result_type = src->result_type;
  tmp.ptr = src->ptr;
  tmp.macro = src->macro;
  tmp.special_operator = src->special_operator;
  *cfn = tmp;
  return cfn;
}

s_cfn * cfn_link (s_cfn *cfn)
{
  assert(cfn);
  if (! (cfn->ptr.p = dlsym(RTLD_DEFAULT, cfn->name->str.ptr.pchar))) {
    err_write_1("cfn_link: ");
    err_write_1(cfn->name->str.ptr.pchar);
    err_write_1(": ");
    err_puts(dlerror());
    assert(! "cfn_link: dlsym failed");
    return NULL;
  }
  return cfn;
}

s_cfn * cfn_new_copy (const s_cfn *src)
{
  s_cfn *cfn;
  cfn = alloc(sizeof(s_cfn));
  if (! cfn)
    return NULL;
  cfn_init_copy(cfn, src);
  return cfn;
}

s_cfn * cfn_prep_cif (s_cfn *cfn)
{
  s_list *a;
  ffi_type **arg_ffi_type = NULL;
  u8 i = 0;
  ffi_type *result_ffi_type;
  ffi_status status;
  assert(cfn);
  if (! sym_to_ffi_type(cfn->result_type, NULL, &result_ffi_type))
    return NULL;
  if (cfn->arity) {
    arg_ffi_type = alloc((cfn->arity + 1) * sizeof(ffi_type *));
    if (! arg_ffi_type)
      return NULL;
    a = cfn->arg_types;
    while (a) {
      assert(i < cfn->arity);
      if (a->tag.type != TAG_SYM) {
        err_write_1("cfn_prep_cif: invalid type: ");
        err_puts(tag_type_to_string(a->tag.type));
        assert(! "cfn_prep_cif: invalid type");
        free(arg_ffi_type);
        return NULL;
      }
      if (! sym_to_ffi_type(a->tag.data.sym, result_ffi_type, arg_ffi_type + i)) {
        free(arg_ffi_type);
        return NULL;
      }
      if (a->tag.data.sym == &g_sym_Result) {
        cfn->arg_result = true;
        arg_ffi_type[i] = &ffi_type_pointer;
        result_ffi_type = &ffi_type_pointer;
      }
      i++;
      a = list_next(a);
    }
  }
  status = ffi_prep_cif(&cfn->cif, FFI_DEFAULT_ABI, cfn->arity,
                        result_ffi_type, arg_ffi_type);
  if (status == FFI_BAD_TYPEDEF) {
    err_puts("cfn_prep_cif: ffi_prep_cif: FFI_BAD_TYPEDEF");
    assert(! "cfn_prep_cif: ffi_prep_cif: FFI_BAD_TYPEDEF");
    return NULL;
  }
  if (status == FFI_BAD_ABI) {
    err_puts("cfn_prep_cif: ffi_prep_cif: FFI_BAD_ABI");
    assert(! "cfn_prep_cif: ffi_prep_cif: FFI_BAD_ABI");
    return NULL;
  }
  if (status != FFI_OK) {
    err_puts("cfn_prep_cif: ffi_prep_cif: error");
    assert(! "cfn_prep_cif: ffi_prep_cif: error");
    return NULL;
  }
  return cfn;
}

s_tag * cfn_tag_init (s_tag *tag, const s_sym *type)
{
  s_tag tmp = {0};
  assert(tag);
  assert(type);
  if (! sym_to_tag_type(type, &tmp.type)) {
    err_write_1("cfn_tag_init: invalid type: ");
    err_puts(type->str.ptr.pchar);
    assert(! "cfn_tag_init: invalid type");
    return NULL;
  }
  *tag = tmp;
  return tag;
}
