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
#include "alloc.h"
#include "assert.h"
#include <dlfcn.h>
#include <string.h>
#include "cfn.h"
#include "list.h"
#include "mutex.h"
#include "pstruct.h"
#include "str.h"
#include "struct.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

static s_tag * cfn_tag_init (s_tag *tag, const s_sym *type);

s_tag * cfn_apply (s_cfn *cfn, s_list *args, s_tag * volatile dest)
{
  s_list *a;
  void ** volatile arg_pointer_result = NULL;
  void ** volatile arg_pointers = NULL;
  void ** volatile arg_values = NULL;
  void **result_pointer = NULL;
  u8 arity;
  s_list *cfn_arg_types;
  s_env *env;
  sw i = 0;
  sw num_args;
  void *p;
  void *result = NULL;
  s_tag tmp = {0};
  s_tag tmp2 = {0};
  s_list * volatile trace;
  s_unwind_protect unwind_protect;
  assert(cfn);
  assert(cfn->arity == cfn->cif.nargs);
  env = env_global();
  num_args = list_length(args);
  arity = cfn->arity - (cfn->arg_result ? 1 : 0);
  if (arity != num_args) {
    err_write_1("cfn_apply: ");
    err_inspect_str(&cfn->name->str);
    err_write_1(": invalid number of arguments, expected ");
    err_inspect_u8(&arity);
    err_write_1(", have ");
    err_inspect_sw_decimal(&num_args);
    err_write_1("\n");
    return NULL;
  }
  cfn_tag_init(&tmp, cfn->result_type);
  if ((cfn->result_type != &g_sym_Ptr ||
       cfn->arg_result) &&
      cfn->cif.rtype == &ffi_type_pointer) {
    /* make result point to result_pointer */
    if (! tag_to_ffi_pointer(&tmp, cfn->result_type,
                             (void **) &result_pointer)) {
      err_puts("cfn_apply: tag_to_ffi_pointer 1");
      assert(! "cfn_apply: tag_to_ffi_pointer 1");
      return NULL;
    }
    result = &result_pointer;
  }
  else {
    /* make result point to tmp value */
    if (! tag_to_ffi_pointer(&tmp, cfn->result_type, &result)) {
      err_puts("cfn_apply: tag_to_ffi_pointer 2");
      assert(! "cfn_apply: tag_to_ffi_pointer 2");
      return NULL;
    }
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
        assert(cfn->cif.rtype == &ffi_type_pointer);
        cfn_tag_init(&tmp2, cfn->result_type);
        if (! tag_to_ffi_pointer(&tmp2, cfn->result_type, &p)) {
          err_puts("cfn_apply: tag_to_ffi_pointer 3");
          assert(! "cfn_apply: tag_to_ffi_pointer 3");
          goto ko;
        }
        arg_pointers[i] = p;
        arg_values[i] = &arg_pointers[i];
        arg_pointer_result = p;
      }
      else {
        if (cfn->cif.arg_types[i] == &ffi_type_pointer) {
          if (! tag_to_ffi_pointer(&a->tag, cfn_arg_types->tag.data.sym,
                                   &p)) {
            err_puts("cfn_apply: tag_to_ffi_pointer 4");
            err_stacktrace();
            assert(! "cfn_apply: tag_to_ffi_pointer 4");
            goto ko;
          }
          arg_pointers[i] = p;
          arg_values[i] = &arg_pointers[i];
        }
        else {
          if (! tag_to_ffi_pointer(&a->tag, cfn_arg_types->tag.data.sym,
                                   &p)) {
            err_write_1("cfn_apply: ");
            err_inspect_str(&cfn->name->str);
            err_write_1(" ");
            err_inspect_list(args);
            err_puts(": tag_to_ffi_pointer 5");
            assert(! "cfn_apply: tag_to_ffi_pointer 5");
            goto ko;
          }
          arg_values[i] = p;
        }
        a = list_next(a);
      }
      cfn_arg_types = list_next(cfn_arg_types);
      i++;
    }
  }
  if (cfn->ptr.f) {
    if (! (trace = list_new(env->stacktrace)))
      goto ko;
    tag_init_list(&trace->tag, list_new_sym
                  (cfn->name, list_new_copy
                   (args)));
    env->stacktrace = trace;
    env_unwind_protect_push(env, &unwind_protect);
    if (setjmp(unwind_protect.buf)) {
      env_unwind_protect_pop(env, &unwind_protect);
      assert(env->stacktrace == trace);
      env->stacktrace = list_delete(trace);
      free(arg_pointers);
      free(arg_values);
      longjmp(*unwind_protect.jmp, 1);
      abort();
    }
    ffi_call(&cfn->cif, cfn->ptr.f, result, arg_values);
    env_unwind_protect_pop(env, &unwind_protect);
    if (cfn->arg_result) {
      if (result_pointer != arg_pointer_result) {
        err_write_1("cfn_apply: ");
        err_inspect_str(&cfn->name->str);
        err_write_1(" ");
        err_inspect_list(args);
        err_write_1(": ");
        err_inspect_pointer(result_pointer);
        err_write_1(" != ");
        err_inspect_pointer(arg_pointer_result);
        err_write_1("\n");
        assert(env->stacktrace == trace);
        env->stacktrace = list_delete(trace);
        goto ko;
      }
      tag_clean(&tmp);
      *dest = tmp2;
    }
    else
      *dest = tmp;
    assert(env->stacktrace == trace);
    env->stacktrace = list_delete(trace);
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

s8 cfn_arity (const s_cfn *cfn)
{
  sw arity;
  arity = cfn->arity - (cfn->arg_result ? 1 : 0);
  if (arity < 0 || arity > S8_MAX) {
    err_puts("fn_arity: invalid arity");
    assert(! "fn_arity: invalid arity");
    return -1;
  }
  return arity;
}

void cfn_clean (s_cfn *cfn)
{
  assert(cfn);
  list_delete_all(cfn->arg_types);
  if (cfn->cif.nargs)
    free(cfn->cif.arg_types);
#if HAVE_PTHREAD
  mutex_clean(&cfn->mutex);
#endif
}

void cfn_delete (s_cfn *cfn)
{
  assert(cfn);
  cfn_clean(cfn);
  free(cfn);
}

bool cfn_eval (s_cfn *cfn)
{
  assert(cfn);
#if HAVE_PTHREAD
  mutex_lock(&cfn->mutex);
#endif
  if (! cfn->ready) {
    if (! cfn_prep_cif(cfn))
      goto ko;
    if (! cfn_link(cfn))
      goto ko;
    cfn->ready = true;
  }
#if HAVE_PTHREAD
  mutex_unlock(&cfn->mutex);
#endif
  return true;
 ko:
#if HAVE_PTHREAD
  mutex_unlock(&cfn->mutex);
#endif
  return false;
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
#if HAVE_PTHREAD
  mutex_init(&tmp.mutex);
#endif
  *cfn = tmp;
  return cfn;
}

/*
s_cfn * cfn_init_cast (s_cfn *cfn, const s_sym * const *type,
                       const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_CFN:
    return cfn_init_copy(cfn, &tag->data.cfn);
  default:
    break;
  }
  err_write_1("cfn_init_cast: cannot cast ");
  err_write_1(tag_type_to_string(tag->type));
  if (*type == &g_sym_Cfn)
    err_puts(" to Cfn");
  else {
    err_write_1(" to ");
    err_inspect_sym(type);
    err_puts(" aka Cfn");
  }
  assert(! "cfn_init_cast: cannot cast to Cfn");
  return NULL;
}
*/

s_cfn * cfn_init_copy (s_cfn *cfn, const s_cfn *src)
{
  s_cfn tmp = {0};
  assert(src);
  assert(cfn);
  tmp.name = src->name;
  tmp.arg_result = src->arg_result;
  if (src->arg_types &&
      ! (tmp.arg_types = list_new_copy(src->arg_types)))
    return NULL;
  tmp.arity = src->arity;
  tmp.cif = src->cif;
  if (src->arity && src->cif.arg_types) {
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
#if HAVE_PTHREAD
  mutex_init(&tmp.mutex);
#endif
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
#if HAVE_PTHREAD
    mutex_unlock(&cfn->mutex);
#endif
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
  s_cfn *result = NULL;
  ffi_type *result_ffi_type;
  ffi_status status;
  assert(cfn);
  if (cfn->ready) {
    result = cfn;
    goto clean;
  }
  if (! sym_to_ffi_type(cfn->result_type, NULL, &result_ffi_type))
    goto clean;
  if (cfn->arity) {
    arg_ffi_type = alloc((cfn->arity + 1) * sizeof(ffi_type *));
    if (! arg_ffi_type)
      goto clean;
    a = cfn->arg_types;
    while (a) {
      assert(i < cfn->arity);
      if (a->tag.type != TAG_SYM) {
        err_write_1("cfn_prep_cif: invalid type: ");
        err_puts(tag_type_to_string(a->tag.type));
        assert(! "cfn_prep_cif: invalid type");
        free(arg_ffi_type);
        goto clean;
      }
      if (! sym_to_ffi_type(a->tag.data.sym, result_ffi_type,
                            arg_ffi_type + i)) {
        free(arg_ffi_type);
        goto clean;
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
    goto clean;
  }
  if (status == FFI_BAD_ABI) {
    err_puts("cfn_prep_cif: ffi_prep_cif: FFI_BAD_ABI");
    assert(! "cfn_prep_cif: ffi_prep_cif: FFI_BAD_ABI");
    goto clean;
  }
  if (status != FFI_OK) {
    err_puts("cfn_prep_cif: ffi_prep_cif: error");
    assert(! "cfn_prep_cif: ffi_prep_cif: error");
    goto clean;
  }
  cfn->ready = true;
  result = cfn;
 clean:
  return result;
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
  if (tmp.type == TAG_PSTRUCT) {
    if (! pstruct_init(&tmp.data.pstruct, type)) {
      err_write_1("cfn_tag_init: struct_init: ");
      err_puts(type->str.ptr.pchar);
      assert(! "cfn_tag_init: struct_init");
      return NULL;
    }
    if (! struct_allocate(tmp.data.pstruct)) {
      err_write_1("cfn_tag_init: struct_allocate: ");
      err_puts(type->str.ptr.pchar);
      assert(! "cfn_tag_init: struct_allocate");
      return NULL;
    }
  }
  *tag = tmp;
  return tag;
}
