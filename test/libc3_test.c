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
#include "../libc3/c3.h"
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
void list_test (void);
void set__fact_test (void);
void set__tag_test (void);
void skiplist__fact_test (void);
void str_test (void);
void sym_test (void);
void tag_test (void);
void tuple_test (void);
void types_test (void);

int main (int argc, char **argv)
{
  test_init(argc, argv);
  c3_init(NULL);
  if (test_target("types")) {
    fprintf(stderr, "\ntypes\n");
    types_test();
  }
  if (test_target("array")) {
    fprintf(stderr, "\narray\n");
    array_test();
  }
  if (test_target("bool")) {
    fprintf(stderr, "\nbool\n");
    bool_test();
  }
  if (test_target("character")) {
    fprintf(stderr, "\ncharacter\n");
    character_test();
  }
  if (test_target("buf")) {
    fprintf(stderr, "\nbuf\n");
    buf_test();
  }
  if (test_target("buf_parse")) {
    fprintf(stderr, "\nbuf_parse\n");
    buf_parse_test();
  }
  if (test_target("buf_inspect")) {
    fprintf(stderr, "\nbuf_inspect\n");
    buf_inspect_test();
  }
  if (test_target("buf_file")) {
    fprintf(stderr, "\nbuf_file\n");
    buf_file_test();
  }
  if (test_target("str")) {
    fprintf(stderr, "\nstr\n");
    str_test();
  }
  if (test_target("sym")) {
    fprintf(stderr, "\nsym\n");
    sym_test();
  }
  if (test_target("ident")) {
    fprintf(stderr, "\nident\n");
    ident_test();
  }
  if (test_target("list")) {
    fprintf(stderr, "\nlist\n");
    list_test();
  }
  if (test_target("tuple")) {
    fprintf(stderr, "\ntuple\n");
    tuple_test();
  }
  if (test_target("fn")) {
    fprintf(stderr, "\nfn\n");
    fn_test();
  }
  if (test_target("call")) {
    fprintf(stderr, "\ncall\n");
    call_test();
  }
  if (test_target("tag")) {
    fprintf(stderr, "\ntag\n");
    tag_test();
  }
  if (test_target("fact")) {
    fprintf(stderr, "\nfact\n");
    fact_test();
  }
  if (test_target("compare")) {
      fprintf(stderr, "\ncompare\n");
      compare_test();
  }
  if (test_target("set__tag")) {
    fprintf(stderr, "\nset__tag\n");
    set__tag_test();
  }
  if (test_target("set__fact")) {
    fprintf(stderr, "\nset__fact\n");
    set__fact_test();
  }
  if (test_target("skiplist__fact")) {
    fprintf(stderr, "\nskiplist__fact\n");
    skiplist__fact_test();
  }
  if (test_target("facts")) {
    fprintf(stderr, "\nfacts\n");
    facts_test();
  }
  if (test_target("facts_cursor")) {
    fprintf(stderr, "\nfacts_cursor\n");
    facts_cursor_test();
  }
  if (test_target("facts_with")) {
    fprintf(stderr, "\nfacts_with\n");
    facts_with_test();
  }
  if (test_target("env")) {
    fprintf(stderr, "\nenv\n");
    env_test();
  }
  test_summary();
  test_clean();
  c3_clean(NULL);
  return 0;
}
