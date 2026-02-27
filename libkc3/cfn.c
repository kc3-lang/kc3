/* kc3
 * Copyright from 2022 to 2026 kmx.io <contact@kmx.io>
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
#include "pointer.h"
#include "pstruct.h"
#include "securelevel.h"
#include "str.h"
#include "struct.h"
#include "sym.h"
#include "tag.h"
#include "tag_type.h"

s_tag * cfn_apply (s_cfn *cfn, s_list *args, s_tag *dest)
{
  s_list *a;
  void ** volatile arg_pointer_result = NULL;
  void ** volatile arg_pointers = NULL;
  void ** volatile arg_values = NULL;
  s_tag * volatile dest_v = dest;
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
    err_inspect_str(&cfn->c_name->str);
    err_write_1(": invalid number of arguments, expected ");
    err_inspect_u8(arity);
    err_write_1(", have ");
    err_inspect_sw_decimal(num_args);
    err_write_1("\n");
    return NULL;
  }
  tag_init_sym_type(&tmp, cfn->result_type);
  if (((cfn->result_type != &g_sym_Ptr &&
        ! sym_is_pointer_type(cfn->result_type, NULL)) ||
       cfn->arg_result) &&
      cfn->cif.rtype == &ffi_type_pointer) {
    if (! tag_to_ffi_pointer(&tmp, cfn->result_type,
                             (void **) &result_pointer)) {
      err_puts("cfn_apply: tag_to_ffi_pointer 1");
      assert(! "cfn_apply: tag_to_ffi_pointer 1");
      tag_clean(&tmp);
      return NULL;
    }
    result = &result_pointer;
  }
  else {
    if (! tag_to_ffi_pointer(&tmp, cfn->result_type, &result)) {
      err_puts("cfn_apply: tag_to_ffi_pointer 2");
      assert(! "cfn_apply: tag_to_ffi_pointer 2");
      tag_clean(&tmp);
      return NULL;
    }
  }
  if (cfn->arity) {
    arg_pointers = alloc((cfn->arity + 1) * sizeof(void *));
    if (! arg_pointers) {
      tag_clean(&tmp);
      return NULL;
    }
    arg_values = alloc((cfn->arity + 1) * sizeof(void *));
    if (! arg_values) {
      tag_clean(&tmp);
      alloc_free(arg_pointers);
      return NULL;
    }
    cfn_arg_types = cfn->arg_types;
    a = args;
    while (cfn_arg_types) {
      assert(cfn_arg_types->tag.type == TAG_PSYM);
      if (cfn_arg_types->tag.data.td_psym == &g_sym_Result) {
        assert(cfn->cif.rtype == &ffi_type_pointer);
        tag_init_sym_type(&tmp2, cfn->result_type);
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
          if (! tag_to_ffi_pointer(&a->tag,
                                   cfn_arg_types->tag.data.td_psym,
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
          if (! tag_to_ffi_pointer(&a->tag,
                                   cfn_arg_types->tag.data.td_psym,
                                   &p)) {
            err_write_1("cfn_apply: ");
            err_inspect_str(&cfn->c_name->str);
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
  if (cfn->ptr.p_f) {
    s_list *args_copy;
    s_list *trace_plist;
    if (! (trace = list_new(env->stacktrace)))
      goto ko;
    args_copy = list_new_copy_all(args);
    trace_plist = list_new_psym(cfn->c_name, args_copy);
    if (! trace_plist) {
      list_delete_all(args_copy);
      list_delete(trace);
      goto ko;
    }
    tag_init_plist(&trace->tag, trace_plist);
    env->stacktrace = trace;
    env_unwind_protect_push(env, &unwind_protect);
    if (setjmp(unwind_protect.buf)) {
      env_unwind_protect_pop(env, &unwind_protect);
      assert(env->stacktrace == trace);
      env->stacktrace = list_delete(trace);
      alloc_free(arg_pointers);
      alloc_free(arg_values);
      longjmp(*unwind_protect.jmp, 1);
      abort();
    }
    ffi_call(&cfn->cif, cfn->ptr.p_f, result, arg_values);
    env_unwind_protect_pop(env, &unwind_protect);
    if (cfn->arg_result) {
      if (result_pointer != arg_pointer_result) {
        err_write_1("cfn_apply: ");
        err_inspect_str(&cfn->c_name->str);
        err_write_1(" ");
        err_inspect_list(args);
        err_write_1(": ");
        err_inspect_c_pointer(result_pointer);
        err_write_1(" != ");
        err_inspect_c_pointer(arg_pointer_result);
        err_write_1("\n");
        assert(env->stacktrace == trace);
        env->stacktrace = list_delete(trace);
        goto ko;
      }
      tag_clean(&tmp);
      *dest_v = tmp2;
    }
    else
      *dest_v = tmp;
    assert(env->stacktrace == trace);
    env->stacktrace = list_delete(trace);
  }
  else {
    err_puts("cfn_apply: NULL function pointer");
    assert(! "cfn_apply: NULL function pointer");
    tag_init_void(dest);
  }
  alloc_free(arg_pointers);
  alloc_free(arg_values);
  return dest_v;
 ko:
  tag_clean(&tmp);
  tag_clean(&tmp2);
  alloc_free(arg_pointers);
  alloc_free(arg_values);
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
    alloc_free(cfn->cif.arg_types);
#if HAVE_PTHREAD
  mutex_clean(&cfn->cif_mutex);
#endif
}

void cfn_delete (s_cfn *cfn)
{
  assert(cfn);
  cfn_clean(cfn);
  alloc_free(cfn);
}

bool cfn_eval (s_cfn *cfn)
{
  assert(cfn);
  if (securelevel(0)) {
    err_puts("cfn_eval: cannot eval Cfn with securelevel > 0");
    abort();
  }
#if HAVE_PTHREAD
  mutex_lock(&cfn->cif_mutex);
#endif
  if (! cfn->cif_ready) {
    if (! cfn_prep_cif(cfn))
      goto ko;
    if (! cfn_link(cfn))
      goto ko;
    cfn->cif_ready = true;
  }
#if HAVE_PTHREAD
  mutex_unlock(&cfn->cif_mutex);
#endif
  return true;
 ko:
#if HAVE_PTHREAD
  mutex_unlock(&cfn->cif_mutex);
#endif
  return false;
}

s_cfn * cfn_init (s_cfn *cfn, const s_sym *c_name, s_list *arg_types,
                  const s_sym *result_type)
{
  sw arity;
  assert(cfn);
  assert(c_name);
  assert(result_type);
  arity = list_length(arg_types);
  if (arity > 255) {
    err_puts("cfn_init: arity > 255");
    assert(arity <= 255);
    return NULL;
  }
  *cfn = (s_cfn) {0};
  cfn->c_name = c_name;
  cfn->arg_types = arg_types;
  cfn->arity = arity;
  cfn->result_type = result_type;
#if HAVE_PTHREAD
  mutex_init(&cfn->cif_mutex);
#endif
  return cfn;
}

/*
s_cfn * cfn_init_cast (s_cfn *cfn, const s_sym * const *type,
                       const s_tag *tag)
{
  (void) type;
  switch (tag->type) {
  case TAG_CFN:
    return cfn_init_copy(cfn, &tag->data.td_cfn);
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
  assert(src);
  assert(cfn);
  *cfn = (s_cfn) {0};
  cfn->c_name = src->c_name;
  cfn->name = src->name;
  cfn->arg_result = src->arg_result;
  if (src->arg_types &&
      ! (cfn->arg_types = list_new_copy_all(src->arg_types)))
    return NULL;
  cfn->arity = src->arity;
  cfn->cif = src->cif;
  if (src->arity && src->cif.arg_types) {
    cfn->cif.arg_types = alloc((src->cif.nargs + 1) *
                               sizeof(ffi_type *));
    if (! cfn->cif.arg_types) {
      list_delete_all(cfn->arg_types);
      return NULL;
    }
    memcpy(cfn->cif.arg_types, src->cif.arg_types,
           (src->cif.nargs + 1) * sizeof(ffi_type *));
  }
  cfn->result_type = src->result_type;
  cfn->ptr = src->ptr;
  cfn->macro = src->macro;
  cfn->special_operator = src->special_operator;
#if HAVE_PTHREAD
  mutex_init(&cfn->cif_mutex);
#endif
  return cfn;
}

s_cfn * cfn_link (s_cfn *cfn)
{
  assert(cfn);
  if (securelevel(0)) {
    err_puts("cfn_link: cannot eval Cfn or call dlsym with"
             " securelevel > 0");
    abort();
  }
  if (! (cfn->ptr.p_pvoid = dlsym(RTLD_DEFAULT, cfn->c_name->str.ptr.p_pchar))) {
    err_write_1("cfn_link: ");
    err_write_1(cfn->c_name->str.ptr.p_pchar);
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
  s_cfn *result = NULL;
  ffi_type *result_ffi_type;
  ffi_status status;
  assert(cfn);
  if (securelevel(0)) {
    err_puts("cfn_prep_cif: cannot eval Cfn with securelevel > 0");
    abort();
  }
  if (cfn->cif_ready) {
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
      if (a->tag.type != TAG_PSYM) {
        err_write_1("cfn_prep_cif: invalid type: ");
        err_puts(tag_type_to_string(a->tag.type));
        assert(! "cfn_prep_cif: invalid type");
        alloc_free(arg_ffi_type);
        goto clean;
      }
      if (! sym_to_ffi_type(a->tag.data.td_psym, result_ffi_type,
                            arg_ffi_type + i)) {
        alloc_free(arg_ffi_type);
        goto clean;
      }
      if (a->tag.data.td_psym == &g_sym_Result) {
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
  cfn->cif_ready = true;
  result = cfn;
 clean:
  return result;
}

s_cfn * cfn_set_name (s_cfn *cfn, const s_ident *name)
{
  cfn->name = *name;
  return cfn;
}

s_cfn * cfn_set_name_if_null (s_cfn *cfn, const s_ident *name)
{
  if (! cfn->name.sym)
    return cfn_set_name(cfn, name);
  return cfn;
}
