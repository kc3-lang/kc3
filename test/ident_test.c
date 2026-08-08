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
#include "../libkc3/assert.h"
#include <stdlib.h>
#include <string.h>
#include "../libkc3/character.h"
#include "../libkc3/ident.h"
#include "../libkc3/str.h"
#include "test.h"

#define IDENT_TEST_CHARACTER_IS_RESERVED(test, expected)               \
  do {                                                                 \
    TEST_EQ(ident_character_is_reserved(test), expected);              \
  } while (0)

#define IDENT_TEST_FIRST_CHARACTER_IS_RESERVED(test, expected)         \
  do {                                                                 \
    TEST_EQ(ident_first_character_is_reserved(test), (expected));      \
  } while (0)

void ident_test (void);
TEST_CASE_PROTOTYPE(ident_character_is_reserved);
TEST_CASE_PROTOTYPE(ident_first_character_is_reserved);

void ident_test (void)
{
  TEST_CASE_RUN(ident_first_character_is_reserved);
  TEST_CASE_RUN(ident_character_is_reserved);
}

TEST_CASE(ident_character_is_reserved)
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
TEST_CASE_END(ident_character_is_reserved)

TEST_CASE(ident_first_character_is_reserved)
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
TEST_CASE_END(ident_first_character_is_reserved)
