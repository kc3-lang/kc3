#include <libkc3/kc3.h>
#include "tag_test.h"
#include "test.h"
#include "../ekc3/embed.h"

#define EMBED_TEST_PARSE_TEMPLATE_1(template, expected)                \
  do {                                                                 \
    const char *EMBED_TEST_PARSE_TEMPLATE_1_template = (template);     \
    s_tag       EMBED_TEST_PARSE_TEMPLATE_1_expected = {0};            \
    s_tag result = {0};                                                \
    test_context("embed_parse_template_1(" # template ") -> "          \
                 # expected);                                          \
    TEST_ASSERT(tag_init_1(&EMBED_TEST_PARSE_TEMPLATE_1_expected,      \
                           (expected)));                               \
    TEST_ASSERT(embed_parse_template_1                                 \
                (EMBED_TEST_PARSE_TEMPLATE_1_template, &result));      \
    TAG_TEST_EQ(&result, &EMBED_TEST_PARSE_TEMPLATE_1_expected);       \
    tag_clean(&result);                                                \
    tag_clean(&EMBED_TEST_PARSE_TEMPLATE_1_expected);                  \
    test_context(NULL);                                                \
  } while (0)

void embed_test (void);
TEST_CASE_PROTOTYPE(embed_parse_template_1);

void embed_test (void)
{
  TEST_CASE_RUN(embed_parse_template_1);
}

TEST_CASE(embed_parse_template_1)
{
  EMBED_TEST_PARSE_TEMPLATE_1("Hello World",
                              "[\"Hello World\"]");
  EMBED_TEST_PARSE_TEMPLATE_1("Hello <%= name %>!",
                              "[\"Hello \", EKC3.verbose name, \"!\"]");
  EMBED_TEST_PARSE_TEMPLATE_1("Hello <% :world %>!",
                              "[\"Hello \", :world, \"!\"]");
  EMBED_TEST_PARSE_TEMPLATE_1("A<%= x %>B<% y %>C",
                              "[\"A\","
                              " EKC3.verbose x,"
                              " \"B\","
                              " y,"
                              " \"C\"]");
  EMBED_TEST_PARSE_TEMPLATE_1("", "[]");
}
TEST_CASE_END(embed_parse_template_1)
