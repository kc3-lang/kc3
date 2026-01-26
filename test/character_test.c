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
#include <stdlib.h>
#include <string.h>
#include "../libkc3/character.h"
#include "test.h"

#define CHARACTER_TEST_1_ASCII(string)                                 \
  do {                                                                 \
    test_context("character_1(" string ") -> " # string);              \
    TEST_EQ(character_1(string), string[0]);                           \
    test_context(NULL);                                                \
  } while (0)

#define CHARACTER_TEST_1_UTF8(string, expected)                        \
  do {                                                                 \
    test_context("character_1(" string ") -> " # expected ")");        \
    TEST_EQ(character_1(string), expected);                            \
    test_context(NULL);                                                \
  } while (0)

#define CHARACTER_TEST_IS_ALPHANUM_ASCII(character, expected)          \
  do {                                                                 \
    test_context("character_is_alphanum(" # character ") -> "          \
                 # expected ")");                                      \
    TEST_EQ(character_is_alphanum(character), expected);               \
    test_context(NULL);                                                \
  } while (0)

#define CHARACTER_TEST_IS_ALPHANUM_UTF8(string, expected)              \
  do {                                                                 \
  character c;                                                         \
    test_context("character_is_alphanum(" # string ") -> "             \
                 # expected ")");                                      \
    c = character_1(string);                                           \
    TEST_EQ(character_is_alphanum(c), expected);                       \
    test_context(NULL);                                                \
  } while (0)

TEST_CASE_PROTOTYPE(character_1);
TEST_CASE_PROTOTYPE(character_is_alphanum);
TEST_CASE_PROTOTYPE(character_is_digit);
TEST_CASE_PROTOTYPE(character_is_lowercase);
TEST_CASE_PROTOTYPE(character_is_space);
TEST_CASE_PROTOTYPE(character_is_uppercase);
TEST_CASE_PROTOTYPE(character_utf8);
TEST_CASE_PROTOTYPE(character_utf8_size);

void character_test (void)
{
  TEST_CASE_RUN(character_1);
  TEST_CASE_RUN(character_is_alphanum);
  TEST_CASE_RUN(character_is_digit);
  TEST_CASE_RUN(character_is_lowercase);
  TEST_CASE_RUN(character_is_space);
  TEST_CASE_RUN(character_is_uppercase);
  TEST_CASE_RUN(character_utf8_size);
  TEST_CASE_RUN(character_utf8);
}

TEST_CASE(character_1)
{
  CHARACTER_TEST_1_ASCII("_");
  CHARACTER_TEST_1_ASCII("0");
  CHARACTER_TEST_1_ASCII("1");
  CHARACTER_TEST_1_ASCII("2");
  CHARACTER_TEST_1_ASCII("3");
  CHARACTER_TEST_1_ASCII("4");
  CHARACTER_TEST_1_ASCII("5");
  CHARACTER_TEST_1_ASCII("6");
  CHARACTER_TEST_1_ASCII("7");
  CHARACTER_TEST_1_ASCII("8");
  CHARACTER_TEST_1_ASCII("9");
  CHARACTER_TEST_1_ASCII("A");
  CHARACTER_TEST_1_ASCII("B");
  CHARACTER_TEST_1_ASCII("C");
  CHARACTER_TEST_1_ASCII("X");
  CHARACTER_TEST_1_ASCII("Y");
  CHARACTER_TEST_1_ASCII("Z");
  CHARACTER_TEST_1_ASCII("a");
  CHARACTER_TEST_1_ASCII("b");
  CHARACTER_TEST_1_ASCII("c");
  CHARACTER_TEST_1_ASCII("x");
  CHARACTER_TEST_1_ASCII("y");
  CHARACTER_TEST_1_ASCII("z");
  CHARACTER_TEST_1_UTF8("À",  192);
  CHARACTER_TEST_1_UTF8("É",  201);
  CHARACTER_TEST_1_UTF8("à",  224);
  CHARACTER_TEST_1_UTF8("é",  233);
  CHARACTER_TEST_1_UTF8("Π",  928);
  CHARACTER_TEST_1_UTF8("π",  960);
  CHARACTER_TEST_1_UTF8("꒴", 42164);
  CHARACTER_TEST_1_UTF8("Ꝝ",  42844);
  CHARACTER_TEST_1_UTF8("ꝝ",  42845);
  CHARACTER_TEST_1_UTF8("𐅀", 0x10140);
}
TEST_CASE_END(character_1)

TEST_CASE(character_is_alphanum)
{
  character c;
  for (c = -10; c < 40; c++)
    CHARACTER_TEST_IS_ALPHANUM_ASCII(c, false);
  CHARACTER_TEST_IS_ALPHANUM_ASCII(' ', false);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('!', false);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('0', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('1', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('2', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('3', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('4', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('5', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('6', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('7', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('8', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('9', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('A', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('B', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('C', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('X', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('Y', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('Z', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('\n', false);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('\r', false);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('\x00', false);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('\x01', false);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('_', false);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('a', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('b', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('c', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('x', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('y', true);
  CHARACTER_TEST_IS_ALPHANUM_ASCII('z', true);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("À", true);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("É", true);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("à", true);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("é", true);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("Π", true);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("π", true);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("꒴", false);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("Ꝝ", true);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("ꝝ", true);
  CHARACTER_TEST_IS_ALPHANUM_UTF8("𐅀", true);
}
TEST_CASE_END(character_is_alphanum)

TEST_CASE(character_is_digit)
{
  character c;
  for (c = -10; c < 40; c++)
    TEST_ASSERT(! character_is_digit(c));
  TEST_ASSERT(! character_is_digit('\x00'));
  TEST_ASSERT(! character_is_digit('\x01'));
  TEST_ASSERT(! character_is_digit('\r'));
  TEST_ASSERT(! character_is_digit('\n'));
  TEST_ASSERT(! character_is_digit(' '));
  TEST_ASSERT(! character_is_digit('_'));
  TEST_ASSERT(character_is_digit('0'));
  TEST_ASSERT(character_is_digit('1'));
  TEST_ASSERT(character_is_digit('2'));
  TEST_ASSERT(character_is_digit('3'));
  TEST_ASSERT(character_is_digit('4'));
  TEST_ASSERT(character_is_digit('5'));
  TEST_ASSERT(character_is_digit('6'));
  TEST_ASSERT(character_is_digit('7'));
  TEST_ASSERT(character_is_digit('8'));
  TEST_ASSERT(character_is_digit('9'));
  TEST_ASSERT(! character_is_digit('A'));
  TEST_ASSERT(! character_is_digit('B'));
  TEST_ASSERT(! character_is_digit('C'));
  TEST_ASSERT(! character_is_digit('X'));
  TEST_ASSERT(! character_is_digit('Y'));
  TEST_ASSERT(! character_is_digit('Z'));
  TEST_ASSERT(! character_is_digit(character_1("À")));
  TEST_ASSERT(! character_is_digit(character_1("É")));
  TEST_ASSERT(! character_is_digit(character_1("Π")));
  TEST_ASSERT(! character_is_digit(character_1("Ꝝ")));
  TEST_ASSERT(! character_is_digit(character_1("꒴")));
  TEST_ASSERT(! character_is_digit(character_1("𐅀")));
  TEST_ASSERT(! character_is_digit('a'));
  TEST_ASSERT(! character_is_digit('b'));
  TEST_ASSERT(! character_is_digit('c'));
  TEST_ASSERT(! character_is_digit('x'));
  TEST_ASSERT(! character_is_digit('y'));
  TEST_ASSERT(! character_is_digit('z'));
  TEST_ASSERT(! character_is_digit(character_1("à")));
  TEST_ASSERT(! character_is_digit(character_1("é")));
  TEST_ASSERT(! character_is_digit(character_1("π")));
  TEST_ASSERT(! character_is_digit(character_1("ꝝ")));
}
TEST_CASE_END(character_is_digit)

TEST_CASE(character_is_lowercase)
{
  character c;
  for (c = -10; c < 40; c++)
    TEST_ASSERT(! character_is_lowercase(c));
  TEST_ASSERT(! character_is_lowercase('\x00'));
  TEST_ASSERT(! character_is_lowercase('\x01'));
  TEST_ASSERT(! character_is_lowercase('\r'));
  TEST_ASSERT(! character_is_lowercase('\n'));
  TEST_ASSERT(! character_is_lowercase(' '));
  TEST_ASSERT(! character_is_lowercase('_'));
  TEST_ASSERT(! character_is_lowercase('0'));
  TEST_ASSERT(! character_is_lowercase('1'));
  TEST_ASSERT(! character_is_lowercase('8'));
  TEST_ASSERT(! character_is_lowercase('9'));
  TEST_ASSERT(! character_is_lowercase('A'));
  TEST_ASSERT(! character_is_lowercase('B'));
  TEST_ASSERT(! character_is_lowercase('C'));
  TEST_ASSERT(! character_is_lowercase('X'));
  TEST_ASSERT(! character_is_lowercase('Y'));
  TEST_ASSERT(! character_is_lowercase('Z'));
  TEST_ASSERT(! character_is_lowercase(character_1("À")));
  TEST_ASSERT(! character_is_lowercase(character_1("É")));
  TEST_ASSERT(! character_is_lowercase(character_1("Π")));
  TEST_ASSERT(! character_is_lowercase(character_1("Ꝝ")));
  TEST_ASSERT(! character_is_lowercase(character_1("꒴")));
  TEST_ASSERT(! character_is_lowercase(character_1("𐅀")));
  TEST_ASSERT(character_is_lowercase('a'));
  TEST_ASSERT(character_is_lowercase('b'));
  TEST_ASSERT(character_is_lowercase('c'));
  TEST_ASSERT(character_is_lowercase('x'));
  TEST_ASSERT(character_is_lowercase('y'));
  TEST_ASSERT(character_is_lowercase('z'));
  TEST_ASSERT(character_is_lowercase(character_1("à")));
  TEST_ASSERT(character_is_lowercase(character_1("é")));
  TEST_ASSERT(character_is_lowercase(character_1("π")));
  TEST_ASSERT(character_is_lowercase(character_1("ꝝ")));
}
TEST_CASE_END(character_is_lowercase)

TEST_CASE(character_is_space)
{
  TEST_ASSERT(character_is_space('\n'));
  TEST_ASSERT(character_is_space('\r'));
  TEST_ASSERT(character_is_space('\t'));
  TEST_ASSERT(character_is_space('\v'));
  TEST_ASSERT(character_is_space(' '));
  TEST_ASSERT(! character_is_space('_'));
  TEST_ASSERT(! character_is_space('0'));
  TEST_ASSERT(! character_is_space('1'));
  TEST_ASSERT(! character_is_space('8'));
  TEST_ASSERT(! character_is_space('9'));
  TEST_ASSERT(! character_is_space('a'));
  TEST_ASSERT(! character_is_space('b'));
  TEST_ASSERT(! character_is_space('c'));
  TEST_ASSERT(! character_is_space('x'));
  TEST_ASSERT(! character_is_space('y'));
  TEST_ASSERT(! character_is_space('z'));
  TEST_ASSERT(! character_is_space(character_1("à")));
  TEST_ASSERT(! character_is_space(character_1("é")));
  TEST_ASSERT(! character_is_space(character_1("π")));
  TEST_ASSERT(! character_is_space(character_1("ꝝ")));
  TEST_ASSERT(! character_is_space(character_1("꒴")));
  TEST_ASSERT(! character_is_space(character_1("𐅀")));
  TEST_ASSERT(! character_is_space('A'));
  TEST_ASSERT(! character_is_space('B'));
  TEST_ASSERT(! character_is_space('C'));
  TEST_ASSERT(! character_is_space('X'));
  TEST_ASSERT(! character_is_space('Y'));
  TEST_ASSERT(! character_is_space('Z'));
  TEST_ASSERT(! character_is_space(character_1("À")));
  TEST_ASSERT(! character_is_space(character_1("É")));
  TEST_ASSERT(! character_is_space(character_1("Π")));
  TEST_ASSERT(! character_is_space(character_1("Ꝝ")));
}
TEST_CASE_END(character_is_space)

TEST_CASE(character_is_uppercase)
{
  character c;
  for (c = -10; c < 40; c++)
    TEST_ASSERT(! character_is_uppercase(c));
  TEST_ASSERT(! character_is_uppercase(' '));
  TEST_ASSERT(! character_is_uppercase('_'));
  TEST_ASSERT(! character_is_uppercase('0'));
  TEST_ASSERT(! character_is_uppercase('1'));
  TEST_ASSERT(! character_is_uppercase('8'));
  TEST_ASSERT(! character_is_uppercase('9'));
  TEST_ASSERT(! character_is_uppercase('a'));
  TEST_ASSERT(! character_is_uppercase('b'));
  TEST_ASSERT(! character_is_uppercase('c'));
  TEST_ASSERT(! character_is_uppercase('x'));
  TEST_ASSERT(! character_is_uppercase('y'));
  TEST_ASSERT(! character_is_uppercase('z'));
  TEST_ASSERT(! character_is_uppercase(character_1("à")));
  TEST_ASSERT(! character_is_uppercase(character_1("é")));
  TEST_ASSERT(! character_is_uppercase(character_1("π")));
  TEST_ASSERT(! character_is_uppercase(character_1("ꝝ")));
  TEST_ASSERT(! character_is_uppercase(character_1("꒴")));
  TEST_ASSERT(! character_is_uppercase(character_1("𐅀")));
  TEST_ASSERT(character_is_uppercase('A'));
  TEST_ASSERT(character_is_uppercase('B'));
  TEST_ASSERT(character_is_uppercase('C'));
  TEST_ASSERT(character_is_uppercase('X'));
  TEST_ASSERT(character_is_uppercase('Y'));
  TEST_ASSERT(character_is_uppercase('Z'));
  TEST_ASSERT(character_is_uppercase(character_1("À")));
  TEST_ASSERT(character_is_uppercase(character_1("É")));
  TEST_ASSERT(character_is_uppercase(character_1("Π")));
  TEST_ASSERT(character_is_uppercase(character_1("Ꝝ")));
}
TEST_CASE_END(character_is_uppercase)

TEST_CASE(character_utf8)
{
}
TEST_CASE_END(character_utf8)

TEST_CASE(character_utf8_size)
{
  TEST_EQ(character_utf8_size((character) -1), -1);
  TEST_EQ(character_utf8_size(0), 1);
  TEST_EQ(character_utf8_size('_'), 1);
  TEST_EQ(character_utf8_size('0'), 1);
  TEST_EQ(character_utf8_size('1'), 1);
  TEST_EQ(character_utf8_size('2'), 1);
  TEST_EQ(character_utf8_size('7'), 1);
  TEST_EQ(character_utf8_size('8'), 1);
  TEST_EQ(character_utf8_size('9'), 1);
  TEST_EQ(character_utf8_size('A'), 1);
  TEST_EQ(character_utf8_size('B'), 1);
  TEST_EQ(character_utf8_size('C'), 1);
  TEST_EQ(character_utf8_size('X'), 1);
  TEST_EQ(character_utf8_size('Y'), 1);
  TEST_EQ(character_utf8_size('Z'), 1);
  TEST_EQ(character_utf8_size('a'), 1);
  TEST_EQ(character_utf8_size('b'), 1);
  TEST_EQ(character_utf8_size('c'), 1);
  TEST_EQ(character_utf8_size('x'), 1);
  TEST_EQ(character_utf8_size('y'), 1);
  TEST_EQ(character_utf8_size('z'), 1);
  TEST_EQ(character_utf8_size(character_1("À")), 2);
  TEST_EQ(character_utf8_size(character_1("É")), 2);
  TEST_EQ(character_utf8_size(character_1("à")), 2);
  TEST_EQ(character_utf8_size(character_1("é")), 2);
  TEST_EQ(character_utf8_size(character_1("Π")), 2);
  TEST_EQ(character_utf8_size(character_1("Ꝝ")), 3);
  TEST_EQ(character_utf8_size(character_1("꒴")), 3);
  TEST_EQ(character_utf8_size(character_1("𐅀")), 4);
}
TEST_CASE_END(character_utf8_size)
