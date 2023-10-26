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
#include <dlfcn.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include "cfn.h"
#include "list.h"
#include "str.h"
#include "sym.h"
#include "tag.h"
#include "type.h"

s_tag * cfn_tag_init (s_tag *tag, const s_sym *type);

s_tag * cfn_apply (s_cfn *cfn, s_list *args, s_tag *dest)
{
  s_list *a;
  void **arg_pointers = NULL;
  void **arg_values = NULL;
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
  if (cfn->arg_result) {
    cfn_tag_init(&tmp2, cfn->result_type);
    cfn->result_type = type_pointer(cfn->result_type);
  }
  cfn_tag_init(&tmp, cfn->result_type);
  /* make result point to tmp value */
  result = tag_to_ffi_pointer(&tmp, cfn->result_type);
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
          cfn_arg_types->tag.data.sym == sym_1("&result"))
        if (cfn->cif.rtype == &ffi_type_pointer) {
          arg_pointers[i] = tag_to_ffi_pointer(&tmp2, cfn->result_type);
          arg_values[i] = &arg_pointers[i];
        }
        else
          arg_values[i] = tag_to_ffi_pointer(&tmp2, cfn->result_type);
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

s_cfn * cfn_copy (const s_cfn *cfn, s_cfn *dest)
{
  assert(cfn);
  assert(dest);
  dest->name = cfn->name;
  dest->arg_result = cfn->arg_result;
  list_copy((const s_list **) &cfn->arg_types, &dest->arg_types);
  dest->arity = cfn->arity;
  dest->cif = cfn->cif;
  if (cfn->arity) {
    dest->cif.arg_types = calloc(cfn->cif.nargs + 1,
                                 sizeof(ffi_type *));
    memcpy(dest->cif.arg_types, cfn->cif.arg_types,
           (cfn->cif.nargs + 1) * sizeof(ffi_type *));
  }
  dest->result_type = cfn->result_type;
  dest->ptr = cfn->ptr;
  dest->macro = cfn->macro;
  dest->special_operator = cfn->special_operator;
  return dest;
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
  assert(cfn);
  bzero(cfn, sizeof(s_cfn));
  cfn->name = name;
  cfn->arg_types = arg_types;
  arity = list_length(arg_types);
  if (arity > 255) {
    assert(arity <= 255);
    errx(1, "cfn_init: arity > 255");
    return NULL;
  }
  cfn->arity = arity;
  cfn->result_type = result_type;
  return cfn;
}

s_cfn * cfn_link (s_cfn *cfn)
{
  assert(cfn);
  if (! (cfn->ptr.p = dlsym(NULL, cfn->name->str.ptr.ps8))) {
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
  cfn_copy(src, cfn);
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
  assert(tag);
  assert(type);
  bzero(tag, sizeof(s_tag));
  if (! sym_to_tag_type(type, &tag->type)) {
    assert(! "cfn_tag_init: invalid type");
    errx(1, "cfn_tag_init: invalid type: %s", type->str.ptr.ps8);
    return NULL;
  }
  return tag;
}
