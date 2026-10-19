#include <string.h>
#include "../libkc3/ident.h"
#include "../libkc3/module.h"
#include "../libkc3/sh.h"
#include "../libkc3/str.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "test.h"

TEST_CASE_PROTOTYPE(sh_escape_valid);
TEST_CASE_PROTOTYPE(sh_escape_unicode);
TEST_CASE_PROTOTYPE(sh_escape_allowed);
TEST_CASE_PROTOTYPE(sh_escape_reserved);
TEST_CASE_PROTOTYPE(sh_escape_malformed);
TEST_CASE_PROTOTYPE(sh_escape_bytes);
TEST_CASE_PROTOTYPE(sh_escape_nul);

static char sh_test_policy (const char *name, const char *value,
                           const s_str *src, const s_str *expected);
static char sh_test_reject (const s_str *src);

void sh_test (void)
{
  TEST_CASE_RUN(sh_escape_valid);
  TEST_CASE_RUN(sh_escape_unicode);
  TEST_CASE_RUN(sh_escape_allowed);
  TEST_CASE_RUN(sh_escape_reserved);
  TEST_CASE_RUN(sh_escape_malformed);
  TEST_CASE_RUN(sh_escape_bytes);
  TEST_CASE_RUN(sh_escape_nul);
}

static char sh_test_policy (const char *name, const char *value,
                           const s_str *src, const s_str *expected)
{
  bool changed;
  s_str dest;
  s_ident ident;
  s_tag policy = {0};
  bool restored;
  s_str *result;
  s_tag saved = {0};
  ident_init(&ident, sym_1("Sh"), sym_1(name));
  TEST_EQ(ident_get(&ident, &saved), &saved);
  TEST_EQ(tag_init_str_1(&policy, NULL, value), &policy);
  changed = env_def(env_global(), &ident, &policy);
  result = changed ? sh_escape(src, &dest) : NULL;
  restored = env_def(env_global(), &ident, &saved);
  tag_clean(&policy);
  tag_clean(&saved);
  TEST_ASSERT(changed);
  TEST_ASSERT(restored);
  TEST_EQ(result, &dest);
  TEST_EQ(dest.size, expected->size);
  TEST_MEMCMP(dest.ptr.p_pchar, expected->ptr.p_pchar, dest.size);
  TEST_EQ(dest.ptr.p_pu8[dest.size], 0);
  str_clean(&dest);
  return 0;
}

static char sh_test_reject (const s_str *src)
{
  const s_str unchanged = STR("unchanged");
  s_str dest = unchanged;
  s_str *result;
  result = sh_escape(src, &dest);
  if (result)
    str_clean(&dest);
  TEST_EQ(result, NULL);
  TEST_EQ(dest.free.p_pvoid, unchanged.free.p_pvoid);
  TEST_EQ(dest.ptr.p_pvoid, unchanged.ptr.p_pvoid);
  TEST_EQ(dest.size, unchanged.size);
  TEST_MEMCMP(dest.ptr.p_pchar, unchanged.ptr.p_pchar, dest.size);
  return 0;
}

TEST_CASE(sh_escape_valid)
{
  const struct {
    s_str src;
    s_str expected;
  } cases[] = {
    {STR(""), STR("\"\"")},
    {STR("abc"), STR("abc")},
    {STR("-+/=,^_0123456789AZaz"), STR("-+/=,^_0123456789AZaz")},
    {STR(";"), STR("\";\"")},
    {STR("abc;"), STR("\"abc;\"")},
    {STR("$abc"), STR("\"\\$abc\"")},
    {STR("ab c"), STR("\"ab c\"")},
    {STR("ab\nc"), STR("\"ab\nc\"")},
    {STR("\t\r\n"), STR("\"\t\r\n\"")},
    {STR("'"), STR("\"'\"")},
    {STR("\""), STR("\"\\\"\"")},
    {STR("\\"), STR("\"\\\\\"")},
    {STR("\x60"), STR("\"\\\x60\"")},
    {STR("$(x)"), STR("\"\\$(x)\"")},
    {STR("*?[]{}#&|<>~!"), STR("\"*?[]{}#&|<>~!\"")},
    {STR("\x7F"), STR("\"\x7F\"")},
    {STR("\xC2\x80"), STR("\"\xC2\x80\"")},
    {STR("\xDF\xBF"), STR("\"\xDF\xBF\"")},
    {STR("\xE0\xA0\x80"), STR("\"\xE0\xA0\x80\"")},
    {STR("\xED\x9F\xBF"), STR("\"\xED\x9F\xBF\"")},
    {STR("\xEE\x80\x80"), STR("\"\xEE\x80\x80\"")},
    {STR("\xEF\xBF\xBF"), STR("\"\xEF\xBF\xBF\"")},
    {STR("\xF0\x90\x80\x80"), STR("\"\xF0\x90\x80\x80\"")},
    {STR("\xF4\x8F\xBF\xBF"), STR("\"\xF4\x8F\xBF\xBF\"")},
    {STR("$\xC3\xA9;"), STR("\"\\$\xC3\xA9;\"")},
    {STR("\xC2\xA2\xE2\x82\xAC\xF0\x9F\x98\x80"),
     STR("\"\xC2\xA2\xE2\x82\xAC\xF0\x9F\x98\x80\"")}
  };
  s_str dest;
  uw i;
  TEST_ASSERT(module_load(sym_1("Sh")));
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    TEST_EQ(sh_escape(&cases[i].src, &dest), &dest);
    TEST_EQ(dest.size, cases[i].expected.size);
    TEST_MEMCMP(dest.ptr.p_pchar, cases[i].expected.ptr.p_pchar,
                dest.size);
    TEST_EQ(dest.ptr.p_pu8[dest.size], 0);
    str_clean(&dest);
  }
}
TEST_CASE_END(sh_escape_valid)

TEST_CASE(sh_escape_unicode)
{
  character c;
  s_str dest;
  s_buf expected;
  s_buf input;
  s_str src;
  TEST_ASSERT(module_load(sym_1("Sh")));
  TEST_ASSERT(buf_init_alloc(&input, 0x110000 * 4));
  TEST_ASSERT(buf_init_alloc(&expected, 0x110000 * 4 + 6));
  TEST_EQ(buf_write_1(&expected, "\""), 1);
  for (c = 1; c < 0x110000; c++) {
    if (c >= 0xD800 && c <= 0xDFFF)
      continue;
    TEST_ASSERT(buf_write_character_utf8(&input, c) > 0);
    if (c == '$' || c == 0x60 || c == '\\' || c == '"')
      TEST_EQ(buf_write_1(&expected, "\\"), 1);
    TEST_ASSERT(buf_write_character_utf8(&expected, c) > 0);
  }
  TEST_EQ(buf_write_1(&expected, "\""), 1);
  TEST_EQ(str_init(&src, NULL, input.wpos, input.ptr.p_pchar), &src);
  TEST_EQ(sh_escape(&src, &dest), &dest);
  TEST_EQ(dest.size, expected.wpos);
  TEST_MEMCMP(dest.ptr.p_pchar, expected.ptr.p_pchar, dest.size);
  TEST_EQ(dest.ptr.p_pu8[dest.size], 0);
  str_clean(&dest);
  buf_clean(&expected);
  buf_clean(&input);
}
TEST_CASE_END(sh_escape_unicode)

TEST_CASE(sh_escape_allowed)
{
  const struct {
    s_str src;
    s_str expected;
  } cases[] = {
    {STR("\xC3\xBB"), STR("\"\xC3\xBB\"")},
    {STR("\xC3\xA9"), STR("\xC3\xA9")},
    {STR("\xCE\xBB"), STR("\xCE\xBB")},
    {STR("\xE2\x82\xAC\xF0\x9F\x98\x80"),
     STR("\xE2\x82\xAC\xF0\x9F\x98\x80")},
    {STR("e\xCC\x81"), STR("\"e\xCC\x81\"")}
  };
  uw i;
  TEST_ASSERT(module_load(sym_1("Sh")));
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (sh_test_policy("allowed", "\xC3\xA9\xCE\xBB"
                       "\xE2\x82\xAC\xF0\x9F\x98\x80",
                       &cases[i].src, &cases[i].expected))
      return 1;
  }
}
TEST_CASE_END(sh_escape_allowed)

TEST_CASE(sh_escape_reserved)
{
  const struct {
    s_str src;
    s_str expected;
  } cases[] = {
    {STR("\xC3\xA9"), STR("\"\\\xC3\xA9\"")},
    {STR("\xC3\xAA"), STR("\"\xC3\xAA\"")},
    {STR("\xCE\xBB"), STR("\"\\\xCE\xBB\"")},
    {STR("\xCE\xBC"), STR("\"\xCE\xBC\"")},
    {STR("\xE2\x82\xAC"), STR("\"\\\xE2\x82\xAC\"")},
    {STR("\xF0\x9F\x98\x80"), STR("\"\\\xF0\x9F\x98\x80\"")},
    {STR("\xF0\x9F\x98\x81"), STR("\"\xF0\x9F\x98\x81\"")},
    {STR("$\xC3\xA9\xCE\xBB\xE2\x82\xAC\xF0\x9F\x98\x80"),
     STR("\"\\$\\\xC3\xA9\\\xCE\xBB\\\xE2\x82\xAC"
         "\\\xF0\x9F\x98\x80\"")}
  };
  uw i;
  TEST_ASSERT(module_load(sym_1("Sh")));
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (sh_test_policy("reserved", "$\x60\\\"\xC3\xA9\xCE\xBB"
                       "\xE2\x82\xAC\xF0\x9F\x98\x80",
                       &cases[i].src, &cases[i].expected))
      return 1;
  }
}
TEST_CASE_END(sh_escape_reserved)

TEST_CASE(sh_escape_malformed)
{
  const s_str cases[] = {
    STR("\xFF;id"), STR("safe\xFF;id"), STR(" \xFF;id"),
    STR("\x80$()"), STR("\xC2;id"), STR("\xE2\x82;id"),
    STR("\xF0\x9F\x98;id"),
    STR("\xC2"), STR("\xE2"), STR("\xE2\x82"),
    STR("\xF0"), STR("\xF0\x9F"), STR("\xF0\x9F\x98"),
    STR("safe\xC2"), STR("safe\xE2\x82"), STR("safe\xF0\x9F\x98"),
    STR(" \xC2"), STR(" \xE2\x82"), STR(" \xF0\x9F\x98"),
    STR("\xC0\x80"), STR("\xC0\xA4()"), STR("\xC1\xBF"),
    STR("\xE0\x80\x80"), STR("\xE0\x81\xA1;id"),
    STR("\xE0\x9F\xBF"), STR("\xF0\x80\x80\x80"),
    STR("\xF0\x80\x81\xA1;id"), STR("\xF0\x8F\xBF\xBF"),
    STR("\xED\xA0\x80;"), STR("\xED\xBF\xBF;"),
    STR("\xF4\x90\x80\x80$"), STR("\xF7\xBF\xBF\xBF;"),
    STR("\xF8\xFF\\"), STR("\xE2(\xA1"), STR("\xF0\x9F(\x80"),
    STR("\xC3\xA9\xFF;id")
  };
  uw i;
  TEST_ASSERT(module_load(sym_1("Sh")));
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (sh_test_reject(cases + i))
      return 1;
  }
}
TEST_CASE_END(sh_escape_malformed)

TEST_CASE(sh_escape_bytes)
{
  const s_str prefixes[] = {STR(""), STR("safe"), STR(" ")};
  const char suffix[] = ";$()\x60\"\\\n";
  const char reserved[] = "$\x60\\\"";
  u16 byte;
  char data[32];
  s_str dest;
  char expected[64];
  uw i;
  uw j;
  uw size;
  s_str src;
  TEST_ASSERT(module_load(sym_1("Sh")));
  for (byte = 1; byte <= 255; byte++) {
    for (i = 0; i < sizeof(prefixes) / sizeof(*prefixes); i++) {
      memcpy(data, prefixes[i].ptr.p_pchar, prefixes[i].size);
      data[prefixes[i].size] = (char) byte;
      memcpy(data + prefixes[i].size + 1, suffix, sizeof(suffix) - 1);
      TEST_ASSERT(str_init(&src, NULL,
                            prefixes[i].size + sizeof(suffix), data));
      if (byte >= 0x80) {
        if (sh_test_reject(&src))
          return 1;
        continue;
      }
      size = 0;
      expected[size++] = '"';
      for (j = 0; j < src.size; j++) {
        if (strchr(reserved, (u8) data[j]))
          expected[size++] = '\\';
        expected[size++] = data[j];
      }
      expected[size++] = '"';
      TEST_EQ(sh_escape(&src, &dest), &dest);
      TEST_EQ(dest.size, size);
      TEST_MEMCMP(dest.ptr.p_pchar, expected, size);
      TEST_EQ(dest.ptr.p_pu8[dest.size], 0);
      str_clean(&dest);
    }
  }
}
TEST_CASE_END(sh_escape_bytes)

TEST_CASE(sh_escape_nul)
{
  const s_str cases[] = {
    STR("\0"),
    STR("\0;id"),
    STR("safe\0;id"),
    STR(" \0$()"),
    STR("\xFF\0;id"),
    STR("x\0")
  };
  uw i;
  TEST_ASSERT(module_load(sym_1("Sh")));
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    if (sh_test_reject(cases + i))
      return 1;
  }
}
TEST_CASE_END(sh_escape_nul)
