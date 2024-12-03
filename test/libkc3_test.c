/* kc3
 * Copyright 2022,2023,2024 kmx.io <contact@kmx.io>
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
#include "../libkc3/kc3.h"
#include "test.h"

void array_test (void);
void bool_test (void);
void buf_file_test (void);
void buf_inspect_test (void);
void buf_parse_test (void);
void buf_test (void);
void call_test (void);
void character_test (void);
void compare_test (void);
void env_test (void);
void fact_test (void);
void facts_test (void);
void facts_cursor_test (void);
void facts_with_test (void);
void fn_test (void);
void ident_test (void);
void inspect_test (void);
void list_test (void);
void set__fact_test (void);
void set__tag_test (void);
void skiplist__fact_test (void);
void str_test (void);
void struct_test (void);
void sym_test (void);
void tag_test (void);
void tuple_test (void);
void types_test (void);

#define TEST_TARGET(name)                           \
  do {                                              \
    if (test_target(# name)) {                      \
      fprintf(stderr, "\n" # name " ");             \
      name ## _test();                              \
    }                                               \
  } while (0)

int main (int argc, char **argv)
{
  if (! kc3_init(NULL, &argc, &argv))
    return 1;
  test_init(g_kc3_env->argv[0], &argc, &argv);
  TEST_TARGET(types);
  TEST_TARGET(array);
  TEST_TARGET(bool);
  TEST_TARGET(character);
  TEST_TARGET(buf);
  TEST_TARGET(buf_parse);
  TEST_TARGET(buf_inspect);
  TEST_TARGET(buf_file);
  TEST_TARGET(str);
  TEST_TARGET(sym);
  TEST_TARGET(ident);
  TEST_TARGET(list);
  TEST_TARGET(tuple);
  TEST_TARGET(fn);
  TEST_TARGET(call);
  TEST_TARGET(tag);
  TEST_TARGET(inspect);
  TEST_TARGET(fact);
  TEST_TARGET(compare);
  TEST_TARGET(set__tag);
  TEST_TARGET(set__fact);
  TEST_TARGET(skiplist__fact);
  TEST_TARGET(facts);
  TEST_TARGET(facts_cursor);
  TEST_TARGET(facts_with);
  TEST_TARGET(env);
  TEST_TARGET(struct);
  test_summary();
  test_clean();
  kc3_clean(NULL);
  return 0;
}
