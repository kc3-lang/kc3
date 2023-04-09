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
#include <string.h>
#include "../libc3/cfn.h"
#include "../libc3/list.h"
#include "../libc3/sym.h"
#include "../libc3/tag.h"
#include "test.h"

void cfn_test ();
void cfn_test_apply ();
void cfn_test_copy ();
void cfn_test_init_clean ();
void cfn_test_link ();
bool cfn_test_not (bool a);
void cfn_test_set_type ();

void cfn_test ()
{
  cfn_test_init_clean();
  cfn_test_apply();
  cfn_test_copy();
  cfn_test_link();
  cfn_test_set_type();
}

bool cfn_test_not (bool a)
{
  return a ? false : true;
}

void cfn_test_init_clean ()
{
  s_cfn tmp;
  TEST_EQ(cfn_init(&tmp, sym_1("cfn_test_not"),
                   list_1("(:bool)"),
                   sym_1("bool")), &tmp);
  cfn_clean(&tmp);
  test_ok();
}

void cfn_test_apply ()
{
  s_list *args;
  s_tag result;
  s_cfn tmp;
  cfn_init(&tmp, sym_1("cfn_test_not"),
           list_1("(:bool)"),
           sym_1("bool"));
  args = list_new(NULL, NULL);
  tag_init_bool(&args->tag, false);
  TEST_EQ(cfn_apply(&tmp, args, &result), &result);
  TEST_EQ(result.data.bool, true);
  list_delete_all(args);
  cfn_clean(&tmp);
}

void cfn_test_copy ()
{
  s_cfn a;
  s_list *a_arg_types;
  s_cfn b;
  s_list *b_arg_types;
  cfn_init(&a, sym_1("cfn_test_not"),
           list_1("(:bool)"),
           sym_1("bool"));
  TEST_EQ(cfn_copy(&a, &b), &b);
  TEST_EQ(a.name, b.name);
  TEST_EQ(a.ptr.p, b.ptr.p);
  TEST_EQ(a.arity, b.arity);
  TEST_EQ(a.result_type, b.result_type);
  TEST_EQ(a.arg_result, b.arg_result);
  a_arg_types = a.arg_types;
  b_arg_types = b.arg_types;
  while (a_arg_types && b_arg_types) {
    TEST_EQ(a_arg_types->tag.type.type, TAG_SYM);
    TEST_EQ(a_arg_types->tag.type.type, b_arg_types->tag.type.type);
    TEST_EQ(a_arg_types->tag.data.sym, b_arg_types->tag.data.sym);
    a_arg_types = list_next(a_arg_types);
    b_arg_types = list_next(b_arg_types);
  }
  TEST_EQ(a.cif, b.cif);
  cfn_clean(&b);
  cfn_clean(&a);
}

void cfn_test_link ()
{
  s_cfn tmp;
  cfn_init(&tmp);
  tmp.name
  cfn_clean(&tmp);
}

void cfn_test_set_type ()
{
}
