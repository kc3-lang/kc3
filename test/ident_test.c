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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../libc3/character.h"
#include "../libc3/ident.h"
#include "../libc3/str.h"
#include "test.h"

#define IDENT_TEST_CHARACTER_IS_RESERVED(test, expected)               \
  do {                                                                 \
    TEST_EQ(ident_character_is_reserved(test), expected);              \
  } while (0)

#define IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(test, expected)         \
  do {                                                                 \
    TEST_EQ(ident_first_character_is_reserved(test), (expected));      \
  } while (0)

#define IDENT_TEST_INSPECT(test, result)                               \
  do {                                                                 \
    s_ident ident;                                                     \
    s_str str;                                                         \
    assert(test);                                                      \
    assert(result);                                                    \
    test_context("ident_inspect(" #test ") -> " #result);              \
    ident_init_1(&ident, (test));                                      \
    TEST_EQ(ident_inspect(&ident, &str), &str);                        \
    TEST_STRNCMP(str.ptr.p, (result), str.size);                       \
    str_clean(&str);                                                   \
    test_context(NULL);                                                \
  } while (0)

void ident_test ();
void ident_test_character_is_reserved ();
void ident_test_first_character_is_reserved ();
void ident_test_inspect ();

void ident_test_character_is_reserved ()
{
  IDENT_TEST_CHARACTER_IS_RESERVED('_', false);
  IDENT_TEST_CHARACTER_IS_RESERVED('0', false);
  IDENT_TEST_CHARACTER_IS_RESERVED('9', false);
  IDENT_TEST_CHARACTER_IS_RESERVED('A', false);
  IDENT_TEST_CHARACTER_IS_RESERVED('Z', false);
  IDENT_TEST_CHARACTER_IS_RESERVED('a', false);
  IDENT_TEST_CHARACTER_IS_RESERVED('z', false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("À"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("É"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("à"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("é"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("ÿ"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("Ÿ"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("Π"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("꒴"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("𐅀"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("🎳"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("😄"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("🟣"), false);
  IDENT_TEST_CHARACTER_IS_RESERVED(character_1("🤩"), false);
}

void ident_test_first_character_is_reserved ()
{
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED('_', false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED('0', true);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED('9', true);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED('A', true);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED('Z', true);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED('a', false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED('z', false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("À"), true);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("É"), true);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("à"), false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("é"), false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("ÿ"), false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("Ÿ"), true);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("Π"), true);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("꒴"), false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("𐅀"), false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("🎳"), false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("😄"), false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("🟣"), false);
  IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(character_1("🤩"), false);
}

void ident_test ()
{
  ident_test_inspect();
  ident_test_first_character_is_reserved();
  ident_test_character_is_reserved();
}

void ident_test_inspect ()
{
  IDENT_TEST_INSPECT("", "_\"\"");
  IDENT_TEST_INSPECT(" ", "_\" \"");
  IDENT_TEST_INSPECT("\n", "_\"\\n\"");
  IDENT_TEST_INSPECT("\r", "_\"\\r\"");
  IDENT_TEST_INSPECT("\t", "_\"\\t\"");
  IDENT_TEST_INSPECT("\v", "_\"\\v\"");
  IDENT_TEST_INSPECT("\"", "_\"\\\"\"");
  IDENT_TEST_INSPECT(".", ".");
  IDENT_TEST_INSPECT("..", "_\"..\"");
  IDENT_TEST_INSPECT("...", "_\"...\"");
  IDENT_TEST_INSPECT(".. .", "_\".. .\"");
  IDENT_TEST_INSPECT("t", "t");
  IDENT_TEST_INSPECT("T", "_\"T\"");
  IDENT_TEST_INSPECT("test", "test");
  IDENT_TEST_INSPECT("Test", "_\"Test\"");
  IDENT_TEST_INSPECT("123", "_\"123\"");
  IDENT_TEST_INSPECT("test123", "test123");
  IDENT_TEST_INSPECT("Test123", "_\"Test123\"");
  IDENT_TEST_INSPECT("test 123", "_\"test 123\"");
  IDENT_TEST_INSPECT("Test 123", "_\"Test 123\"");
  IDENT_TEST_INSPECT("test123.test456", "_\"test123.test456\"");
  IDENT_TEST_INSPECT("Test123.Test456", "_\"Test123.Test456\"");
  IDENT_TEST_INSPECT("test123(test456)", "_\"test123(test456)\"");
  IDENT_TEST_INSPECT("Test123(Test456)", "_\"Test123(Test456)\"");
  IDENT_TEST_INSPECT("test123{test456}", "_\"test123{test456}\"");
  IDENT_TEST_INSPECT("Test123{Test456}", "_\"Test123{Test456}\"");
  IDENT_TEST_INSPECT("É", "_\"É\"");
  IDENT_TEST_INSPECT("Éo", "_\"Éo\"");
  IDENT_TEST_INSPECT("Éoà \n\r\t\v\"",
                     "_\"Éoà \\n\\r\\t\\v\\\"\"");
  IDENT_TEST_INSPECT("éoà \n\r\t\v\"",
                     "_\"éoà \\n\\r\\t\\v\\\"\"");
  IDENT_TEST_INSPECT("é", "é");
  IDENT_TEST_INSPECT("éoπꝝ꒴", "éoπꝝ꒴");
}
