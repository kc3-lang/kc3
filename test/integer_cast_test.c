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
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "test.h"

static int integer_cast_child (char (*check)(void))
{
  pid_t pid;
  pid_t result;
  int status;
  if ((pid = fork()) < 0)
    return -1;
  if (! pid) {
    alarm(2);
    _exit(check());
  }
  do {
    result = waitpid(pid, &status, 0);
  } while (result < 0 && errno == EINTR);
  if (result != pid || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

#define INTEGER_CAST(name, type, sym)                                 \
  do {                                                                \
    type value = (type) 23;                                           \
    const s_sym *type_sym = &(sym);                                   \
    if (name ## _init_cast(&value, &type_sym, tag) ||                  \
        value != (type) 23)                                           \
      failed = 1;                                                     \
  } while (0)

static char integer_casts_reject (const s_tag *tag)
{
  char failed = 0;
  INTEGER_CAST(s8, s8, g_sym_S8);
  INTEGER_CAST(s16, s16, g_sym_S16);
  INTEGER_CAST(s32, s32, g_sym_S32);
  INTEGER_CAST(s64, s64, g_sym_S64);
  INTEGER_CAST(sw, sw, g_sym_Sw);
  INTEGER_CAST(u8, u8, g_sym_U8);
  INTEGER_CAST(u16, u16, g_sym_U16);
  INTEGER_CAST(u32, u32, g_sym_U32);
  INTEGER_CAST(u64, u64, g_sym_U64);
  INTEGER_CAST(uw, uw, g_sym_Uw);
  return failed;
}

static char integer_cast_float_out_of_range_check (void)
{
  s_tag tag = {0};
  char failed = 0;
  tag_init_f32(&tag, INFINITY);
  failed |= integer_casts_reject(&tag);
  tag_init_f64(&tag, NAN);
  failed |= integer_casts_reject(&tag);
  tag_init_f64(&tag, 1.0e300);
  failed |= integer_casts_reject(&tag);
  tag_init_f64(&tag, -1.0e300);
  failed |= integer_casts_reject(&tag);
#if HAVE_F80
  tag_init_f80(&tag, INFINITY);
  {
    const s_sym *type;
    u8 u8_value = 23;
    u16 u16_value = 23;
    u32 u32_value = 23;
    u64 u64_value = 23;
    uw uw_value = 23;
    type = &g_sym_U8;
    if (u8_init_cast(&u8_value, &type, &tag) || u8_value != 23)
      failed = 1;
    type = &g_sym_U16;
    if (u16_init_cast(&u16_value, &type, &tag) || u16_value != 23)
      failed = 1;
    type = &g_sym_U32;
    if (u32_init_cast(&u32_value, &type, &tag) || u32_value != 23)
      failed = 1;
    type = &g_sym_U64;
    if (u64_init_cast(&u64_value, &type, &tag) || u64_value != 23)
      failed = 1;
    type = &g_sym_Uw;
    if (uw_init_cast(&uw_value, &type, &tag) || uw_value != 23)
      failed = 1;
  }
#endif
  return failed;
}

#undef INTEGER_CAST

TEST_CASE(integer_cast_float_out_of_range)
{
  TEST_EQ(integer_cast_child(integer_cast_float_out_of_range_check), 0);
}
TEST_CASE_END(integer_cast_float_out_of_range)

TEST_CASE(integer_cast_float_truncation_boundaries)
{
  s_tag tag = {0};
  const s_sym *type;
  s8 s8_value;
  u8 u8_value;
  tag_init_f64(&tag, -128.75);
  type = &g_sym_S8;
  TEST_ASSERT(s8_init_cast(&s8_value, &type, &tag));
  TEST_EQ(s8_value, S8_MIN);
  tag_init_f64(&tag, 127.75);
  TEST_ASSERT(s8_init_cast(&s8_value, &type, &tag));
  TEST_EQ(s8_value, S8_MAX);
  tag_init_f64(&tag, -0.75);
  type = &g_sym_U8;
  TEST_ASSERT(u8_init_cast(&u8_value, &type, &tag));
  TEST_EQ(u8_value, 0);
  tag_init_f64(&tag, 255.75);
  TEST_ASSERT(u8_init_cast(&u8_value, &type, &tag));
  TEST_EQ(u8_value, U8_MAX);
}
TEST_CASE_END(integer_cast_float_truncation_boundaries)

void integer_cast_test (void)
{
  TEST_CASE_RUN(integer_cast_float_out_of_range);
  TEST_CASE_RUN(integer_cast_float_truncation_boundaries);
}
