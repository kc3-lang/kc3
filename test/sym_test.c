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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libkc3/str.h"
#include "../libkc3/sym.h"
#include "test.h"

#define SYM_TEST_1(test)                                               \
  do {                                                                 \
    TEST_ASSERT((sym = sym_1(test)));                                  \
    TEST_EQ(sym->str.size, strlen(test));                              \
    TEST_EQ(strncmp((test), sym->str.ptr.p_pvoid, strlen(test)), 0);   \
    TEST_EQ(sym_1(test), sym_1(test));                                 \
  } while (0)

TEST_CASE_PROTOTYPE(sym_1);
const s_sym * sym_test_1_test (const s8 *p);

void sym_test (void)
{
  TEST_CASE_RUN(sym_1);
}

TEST_CASE(sym_1)
{
  const s_sym *mod;
  const s_sym *mod_t;
  const s_sym *mod_test;
  const s_sym *mod_test123;
  const s_sym *sym;
  const s_sym *sym_123;
  const s_sym *sym_ident;
  const s_sym *sym_empty;
  const s_sym *sym_t;
  const s_sym *sym_test;
  const s_sym *sym_test123;
  SYM_TEST_1("");
  sym_empty = sym;
  SYM_TEST_1("");
  TEST_EQ(sym, sym_empty);
  SYM_TEST_1(" ");
  SYM_TEST_1(".");
  SYM_TEST_1("..");
  SYM_TEST_1("...");
  SYM_TEST_1("t");
  sym_t = sym;
  TEST_ASSERT(sym_t != sym_empty);
  SYM_TEST_1("T");
  mod_t = sym;
  TEST_ASSERT(mod_t != sym_empty);
  TEST_ASSERT(mod_t != sym_t);
  SYM_TEST_1("test");
  sym_test = sym;
  SYM_TEST_1("Test");
  mod_test = sym;
  TEST_ASSERT(mod_test != sym_empty);
  TEST_ASSERT(mod_test != sym_t);
  TEST_ASSERT(mod_test != mod_t);
  TEST_ASSERT(mod_test != sym_test);
  SYM_TEST_1("123");
  sym_123 = sym;
  SYM_TEST_1("test123");
  sym_test123 = sym;
  SYM_TEST_1("Test123");
  mod_test123 = sym;
  TEST_ASSERT(sym_test123 != mod_test123);
  SYM_TEST_1("123.456");
  TEST_ASSERT(sym != sym_empty);
  TEST_ASSERT(sym != sym_123);
  SYM_TEST_1("test123.test456");
  sym_ident = sym;
  TEST_ASSERT(sym_ident != sym_test123);
  SYM_TEST_1("Test123.Test456");
  mod = sym;
  TEST_ASSERT(mod != mod_test123);
  TEST_ASSERT(mod != sym_ident);
  SYM_TEST_1("A");
  SYM_TEST_1("É");
  SYM_TEST_1("Éo");
  SYM_TEST_1("Éoà \n\r\t\v\"");
  SYM_TEST_1("a");
  SYM_TEST_1("é");
  SYM_TEST_1("éo");
  SYM_TEST_1("éoà \n\r\t\v\"");
}
TEST_CASE_END(sym_1)
