/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software excepted
 * on Apple computers granted the above copyright notice and
 * this permission paragraph are included in all copies and
 * substantial portions of this software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#include "../libc3/c3.h"
#include "test.h"

void bool_test ();
void buf_file_test ();
void buf_inspect_test ();
void buf_parse_test ();
void buf_test ();
void call_test ();
void character_test ();
void compare_test ();
void fact_test ();
void facts_test ();
void facts_cursor_test ();
void facts_with_test ();
void ident_test ();
void list_test ();
void set__fact_test ();
void set__tag_test ();
void skiplist__fact_test ();
void str_test ();
void sym_test ();
void tag_test ();
void tuple_test ();
void types_test ();

int main (int argc, char **argv)
{
  test_init(argc, argv);
  libc3_init();
  if (test_target("types")) {
    printf("\ntypes\n");
    types_test();
  }
  if (test_target("bool")) {
    printf("\nbool\n");
    bool_test();
  }
  if (test_target("character")) {
    printf("\ncharacter\n");
    character_test();
  }
  if (test_target("buf")) {
    printf("\nbuf\n");
    buf_test();
  }
  if (test_target("buf_parse")) {
    printf("\nbuf_parse\n");
    buf_parse_test();
  }
  if (test_target("buf_inspect")) {
    printf("\nbuf_inspect\n");
    buf_inspect_test();
  }
  if (test_target("buf_file")) {
    printf("\nbuf_file\n");
    buf_file_test();
  }
  if (test_target("str")) {
    printf("\nstr\n");
    str_test();
  }
  if (test_target("sym")) {
    printf("\nsym\n");
    sym_test();
  }
  if (test_target("ident")) {
    printf("\nident\n");
    ident_test();
  }
  if (test_target("list")) {
    printf("\nlist\n");
    list_test();
  }
  if (test_target("tuple")) {
    printf("\ntuple\n");
    tuple_test();
  }
  if (test_target("call")) {
    printf("\ncall\n");
    call_test();
  }
  if (test_target("tag")) {
    printf("\ntag\n");
    tag_test();
  }
  if (test_target("fact")) {
    printf("\nfact\n");
    fact_test();
  }
  if (test_target("compare")) {
      printf("\ncompare\n");
      compare_test();
  }
  if (test_target("set__tag")) {
    printf("\nset__tag\n");
    set__tag_test();
  }
  if (test_target("set__fact")) {
    printf("\nset__fact\n");
    set__fact_test();
  }
  if (test_target("skiplist__fact")) {
    printf("\nskiplist__fact\n");
    skiplist__fact_test();
  }
  if (test_target("facts")) {
    printf("\nfacts\n");
    facts_test();
  }
  if (test_target("facts_cursor")) {
    printf("\nfacts_cursor\n");
    facts_cursor_test();
  }
  if (test_target("facts_with")) {
    printf("\nfacts_with\n");
    facts_with_test();
  }
  test_summary();
  test_shutdown();
  libc3_shutdown();
  return 0;
}
