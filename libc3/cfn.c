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
#include <dlfcn.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include "cfn.h"
#include "list.h"
#include "str.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"
#include "type.h"

s_tag * cfn_tag_init (s_tag *tag, const s_sym *type);

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
  void *result;
  s_tag tmp;
  s_tag tmp2;
  assert(cfn);
  assert(cfn->arity == cfn->cif.nargs);
  num_args = list_length(args);
  arity = cfn->arity - (cfn->arg_result ? 1 : 0);
  if (arity != num_args) {
    warnx("cfn_apply: invalid number of arguments, expected %d, have %ld",
          arity, num_args);
    return NULL;
  }
  if (cfn->cif.rtype == &ffi_type_pointer) {
    cfn_tag_init(&tmp, cfn->result_type);
    /* make result point to result_pointer */
    result_pointer = tag_to_ffi_pointer(&tmp, cfn->result_type);
    result = &result_pointer;
  }
  else {
    cfn_tag_init(&tmp, cfn->result_type);
    /* make result point to tmp value */
    result = tag_to_ffi_pointer(&tmp, cfn->result_type);
  }
  if (cfn->arity) {
    if (! (arg_pointers = calloc(sizeof(void *), cfn->arity + 1)))
      err(1, "cfn_apply");
    if (! (arg_values = calloc(sizeof(void *), cfn->arity + 1)))
      err(1, "cfn_apply");
    cfn_arg_types = cfn->arg_types;
    a = args;
    while (cfn_arg_types) {
      assert(cfn_arg_types->tag.type == TAG_SYM);
      if (cfn_arg_types->tag.data.sym == sym_1("Result") ||
          cfn_arg_types->tag.data.sym == sym_1("&result")) {
        cfn_tag_init(&tmp2, cfn->result_type);
        if (cfn->cif.rtype == &ffi_type_pointer) {
          arg_pointers[i] = tag_to_ffi_pointer(&tmp2, cfn->result_type);
          arg_values[i] = &arg_pointers[i];
        }
        else
          arg_values[i] = tag_to_ffi_pointer(&tmp2, cfn->result_type);
      }
      else {
        if (cfn->cif.arg_types[i] == &ffi_type_pointer) {
          arg_pointers[i] =
            tag_to_ffi_pointer(&a->tag, cfn_arg_types->tag.data.sym);
          arg_values[i] = &arg_pointers[i];
        }
        else
          arg_values[i] =
            tag_to_ffi_pointer(&a->tag, cfn_arg_types->tag.data.sym);
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
    warnx("cfn_apply: NULL function pointer");
    tag_init_void(dest);
  }
  free(arg_pointers);
  free(arg_values);
  return dest;
}

void cfn_clean (s_cfn *cfn)
{
  assert(cfn);
  list_delete_all(cfn->arg_types);
  if (cfn->cif.nargs)
    free(cfn->cif.arg_types);
}

s_cfn * cfn_init_copy (s_cfn *cfn, const s_cfn *src)
{
  assert(src);
  assert(cfn);
  cfn->name = src->name;
  cfn->arg_result = src->arg_result;
  list_init_copy(&cfn->arg_types, (const s_list **) &src->arg_types);
  cfn->arity = src->arity;
  cfn->cif = src->cif;
  if (src->arity) {
    cfn->cif.arg_types = calloc(src->cif.nargs + 1,
                                 sizeof(ffi_type *));
    memcpy(cfn->cif.arg_types, src->cif.arg_types,
           (src->cif.nargs + 1) * sizeof(ffi_type *));
  }
  cfn->result_type = src->result_type;
  cfn->ptr = src->ptr;
  cfn->macro = src->macro;
  cfn->special_operator = src->special_operator;
  return cfn;
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
    assert(arity <= 255);
    err_puts("cfn_init: arity > 255");
    return NULL;
  }
  tmp.arity = arity;
  tmp.result_type = result_type;
  *cfn = tmp;
  return cfn;
}

s_cfn * cfn_link (s_cfn *cfn)
{
  assert(cfn);
  if (! (cfn->ptr.p = dlsym(RTLD_DEFAULT, cfn->name->str.ptr.ps8))) {
    warnx("cfn_link: %s: %s", cfn->name->str.ptr.ps8, dlerror());
    return NULL;
  }
  return cfn;
}

s_cfn * cfn_new_copy (const s_cfn *src)
{
  s_cfn *cfn;
  if (! (cfn = calloc(1, sizeof(s_cfn)))) {
    errx(1, "cfn_new_copy: out of memory");
    return NULL;
  }
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
  if (! (result_ffi_type = sym_to_ffi_type(cfn->result_type, NULL))) {
    assert(! "cfn_prep_cif: sym_to_ffi_type");
    errx(1, "cfn_prep_cif: sym_to_ffi_type: %s",
         cfn->result_type->str.ptr.ps8);
    return NULL;
  }
  if (cfn->arity) {
    if (! (arg_ffi_type = calloc(sizeof(ffi_type *), cfn->arity + 1))) {
      assert(! "cfn_prep_cif: arg_ffi_type");
      err(1, "cfn_prep_cif: arg_ffi_type");
      return NULL;
    }
    a = cfn->arg_types;
    while (a) {
      assert(i < cfn->arity);
      if (a->tag.type != TAG_SYM) {
        assert(! "cfn_prep_cif: invalid type");
        errx(1, "cfn_prep_cif: invalid type: %s",
             tag_type_to_string(a->tag.type));
      }
      if (! (arg_ffi_type[i] = sym_to_ffi_type(a->tag.data.sym, result_ffi_type))) {
        free(arg_ffi_type);
        return NULL;
      }
      if (a->tag.data.sym == sym_1("Result") ||
          a->tag.data.sym == sym_1("&result")) {
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
    warnx("cfn_prep_cif: ffi_prep_cif: FFI_BAD_TYPEDEF");
    return NULL;
  }
  if (status == FFI_BAD_ABI) {
    warnx("cfn_prep_cif: ffi_prep_cif: FFI_BAD_ABI");
    return NULL;
  }
  if (status != FFI_OK) {
    warnx("cfn_prep_cif: ffi_prep_cif: unknown error");
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
    err_puts(type->str.ptr.ps8);
    assert(! "cfn_tag_init: invalid type");
    return NULL;
  }
  *tag = tmp;
  return tag;
}
