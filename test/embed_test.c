#include <libkc3/kc3.h>
#include "test.h"
#include "../ekc3/embed.h"

#define EMBED_TEST_PARSE_TEMPLATE_1(template, expected)                \
  do {                                                                 \
    const char *EMBED_TEST_PARSE_TEMPLATE_1_template = (template);     \
    s_tag       EMBED_TEST_PARSE_TEMPLATE_1_expected = {0};            \
    s_tag result = {0};                                                \
    s_list *list;                                                      \
    uw count = 0;                                                      \
    test_context("embed_parse_template_1(" # template ") -> "          \
                 # expected);                                          \
    TEST_ASSERT(tag_init_1(&EMBED_TEST_PARSE_TEMPLATE_1_expected,      \
                           (expected)));                               \
    TEST_ASSERT(embed_parse_template_1                                 \
                (EMBED_TEST_PARSE_TEMPLATE_1_template, &result));      \
    TEST_EQ(count, expected_items);                                    \
    tag_clean(&result);                                                \
    test_context(NULL);                                                \
  } while (0)

TEST_CASE_PROTOTYPE(embed_parse_template_1);

TEST_CASE(embed_parse_template_1)
{
  EMBED_TEST_PARSE_TEMPLATE_1("Hello World", 1);
  EMBED_TEST_PARSE_TEMPLATE_1("Hello <%= name %>!", 4);
  EMBED_TEST_PARSE_TEMPLATE_1("Hello <% puts \"world\" %>!", 3);
  EMBED_TEST_PARSE_TEMPLATE_1("A<%= x %>B<% y %>C", 4);
  EMBED_TEST_PARSE_TEMPLATE_1("", 0);
}
TEST_CASE_END(embed_parse_template_1)
