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
#include <string.h>
#include "../libkc3/cfn.h"
#include "../libkc3/list.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "test.h"

/* 1 */
bool cfn_test_not (bool a);

/* 2 */
TEST_CASE_PROTOTYPE(cfn_apply);
TEST_CASE_PROTOTYPE(cfn_init_clean);
TEST_CASE_PROTOTYPE(cfn_init_copy);
TEST_CASE_PROTOTYPE(cfn_link);
TEST_CASE_PROTOTYPE(cfn_prep_cif);

/* 3 */
void cfn_test (void);

void cfn_test (void)
{
  TEST_CASE_RUN(cfn_init_clean);
  TEST_CASE_RUN(cfn_init_copy);
  TEST_CASE_RUN(cfn_link);
  TEST_CASE_RUN(cfn_prep_cif);
  TEST_CASE_RUN(cfn_apply);
}

bool cfn_test_not (bool a)
{
  return a ? false : true;
}

TEST_CASE(cfn_apply)
{
  s_list *args;
  s_tag result;
  s_cfn a;
  cfn_init(&a, sym_1("cfn_test_not"),
           list_new_1("(:bool)"),
           sym_1("bool"));
  cfn_link(&a);
  cfn_prep_cif(&a);
  args = list_new(NULL);
  tag_init_bool(&args->tag, false);
  TEST_EQ(cfn_apply(&a, args, &result), &result);
  TEST_EQ(result.type, TAG_BOOL);
  TEST_EQ(result.data.bool_, true);
  list_delete_all(args);
  cfn_clean(&a);
}
TEST_CASE_END(cfn_apply)

TEST_CASE(cfn_init_copy)
{
  s_cfn a;
  s_list *a_arg_types;
  s_cfn b;
  s_list *b_arg_types;
  cfn_init(&a, sym_1("cfn_test_not"),
           list_new_1("(:bool)"),
           sym_1("bool"));
  TEST_EQ(cfn_init_copy(&b, &a), &b);
  TEST_EQ(a.c_name, b.c_name);
  TEST_EQ(a.ptr.p, b.ptr.p);
  TEST_EQ(a.arity, b.arity);
  TEST_EQ(a.result_type, b.result_type);
  TEST_EQ(a.arg_result, b.arg_result);
  a_arg_types = a.arg_types;
  b_arg_types = b.arg_types;
  while (a_arg_types && b_arg_types) {
    TEST_EQ(a_arg_types->tag.type, TAG_PSYM);
    TEST_EQ(a_arg_types->tag.type, b_arg_types->tag.type);
    TEST_EQ(a_arg_types->tag.data.psym, b_arg_types->tag.data.psym);
    a_arg_types = list_next(a_arg_types);
    b_arg_types = list_next(b_arg_types);
  }
  TEST_EQ(a.cif.abi, b.cif.abi);
  TEST_EQ(a.cif.nargs, b.cif.nargs);
  TEST_EQ(memcmp(a.cif.arg_types, b.cif.arg_types,
                 a.cif.nargs * sizeof(ffi_type *)), 0);
  TEST_EQ(a.cif.rtype, b.cif.rtype);
  TEST_EQ(a.cif.bytes, b.cif.bytes);
  TEST_EQ(a.cif.flags, b.cif.flags);
  cfn_clean(&b);
  cfn_clean(&a);
}
TEST_CASE_END(cfn_init_copy)

TEST_CASE(cfn_init_clean)
{
  s_cfn a;
  TEST_EQ(cfn_init(&a, sym_1("cfn_test_not"),
                   list_new_1("(:bool)"),
                   sym_1("bool")), &a);
  TEST_EQ(a.c_name, sym_1("cfn_test_not"));
  TEST_EQ(a.ptr.f, 0);
  TEST_EQ(a.ptr.p, 0);
  TEST_EQ(a.arity, 1);
  TEST_EQ(a.result_type, sym_1("bool"));
  TEST_EQ(a.arg_result, false);
  TEST_EQ(a.arg_types->tag.type, TAG_PSYM);
  TEST_EQ(a.arg_types->tag.data.psym, sym_1("bool"));
  TEST_EQ(a.arg_types->next.type, TAG_PLIST);
  TEST_EQ(a.arg_types->next.data.plist, NULL);
  TEST_EQ(a.cif.abi, 0);
  TEST_EQ(a.cif.nargs, 0);
  TEST_EQ(a.cif.arg_types, 0);
  TEST_EQ(a.cif.rtype, 0);
  TEST_EQ(a.cif.bytes, 0);
  TEST_EQ(a.cif.flags, 0);
  cfn_clean(&a);
  test_ok();
}
TEST_CASE_END(cfn_init_clean)

TEST_CASE(cfn_link)
{
  s_cfn a;
  s_cfn b;
  cfn_init(&a, sym_1("cfn_test_not"),
           list_new_1("(:bool)"),
           sym_1("bool"));
  b = a;
  TEST_EQ(cfn_link(&a), &a);
  TEST_EQ(a.c_name, b.c_name);
  TEST_EQ(a.ptr.f, &cfn_test_not);
  TEST_EQ(a.arity, b.arity);
  TEST_EQ(a.result_type, b.result_type);
  TEST_EQ(a.arg_result, b.arg_result);
  TEST_EQ(a.arg_types, b.arg_types);
  TEST_EQ(a.cif.abi, b.cif.abi);
  TEST_EQ(a.cif.nargs, b.cif.nargs);
  TEST_EQ(a.cif.arg_types, b.cif.arg_types);
  TEST_EQ(a.cif.rtype, b.cif.rtype);
  TEST_EQ(a.cif.bytes, b.cif.bytes);
  TEST_EQ(a.cif.flags, b.cif.flags);
  cfn_clean(&a);
}
TEST_CASE_END(cfn_link)

TEST_CASE(cfn_prep_cif)
{
  s_cfn a;
  s_cfn b;
  cfn_init(&a, sym_1("cfn_test_not"),
           list_new_1("(:bool)"),
           sym_1("bool"));
  b = a;
  TEST_EQ(cfn_prep_cif(&a), &a);
  TEST_EQ(a.c_name, b.c_name);
  TEST_EQ(a.ptr.f, b.ptr.f);
  TEST_EQ(a.ptr.p, b.ptr.p);
  TEST_EQ(a.arity, b.arity);
  TEST_EQ(a.result_type, b.result_type);
  TEST_EQ(a.arg_result, b.arg_result);
  TEST_EQ(a.arg_types, b.arg_types);
  TEST_EQ(a.cif.nargs, 1);
  TEST_ASSERT(a.cif.arg_types);
  TEST_EQ(a.cif.arg_types[0], &ffi_type_uint8);
  TEST_EQ(a.cif.rtype, &ffi_type_uint8);
  cfn_clean(&a);
}
TEST_CASE_END(cfn_prep_cif)
