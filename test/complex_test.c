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
#include "../libkc3/complex.h"
#include "../libkc3/tag.h"
#include "test.h"

s_tag * complex_norm (s_complex *c, s_tag *dest);

static uw g_complex_norm_tag_clean_count;

static void complex_norm_tag_clean_probe (s_tag *tag)
{
  if (tag->type == TAG_INTEGER || tag->type == TAG_RATIO)
    g_complex_norm_tag_clean_count++;
  tag_clean(tag);
}

#define complex_add complex_cleanup_probe_add
#define complex_clean complex_cleanup_probe_clean
#define complex_delete complex_cleanup_probe_delete
#define complex_div complex_cleanup_probe_div
#define complex_init complex_cleanup_probe_init
#define complex_init_cast complex_cleanup_probe_init_cast
#define complex_init_copy complex_cleanup_probe_init_copy
#define complex_init_f32 complex_cleanup_probe_init_f32
#define complex_init_f64 complex_cleanup_probe_init_f64
#define complex_init_f80 complex_cleanup_probe_init_f80
#define complex_init_f128 complex_cleanup_probe_init_f128
#define complex_init_integer complex_cleanup_probe_init_integer
#define complex_init_ratio complex_cleanup_probe_init_ratio
#define complex_init_s8 complex_cleanup_probe_init_s8
#define complex_init_s16 complex_cleanup_probe_init_s16
#define complex_init_s32 complex_cleanup_probe_init_s32
#define complex_init_s64 complex_cleanup_probe_init_s64
#define complex_init_sw complex_cleanup_probe_init_sw
#define complex_init_u8 complex_cleanup_probe_init_u8
#define complex_init_u16 complex_cleanup_probe_init_u16
#define complex_init_u32 complex_cleanup_probe_init_u32
#define complex_init_u64 complex_cleanup_probe_init_u64
#define complex_init_uw complex_cleanup_probe_init_uw
#define complex_is_zero complex_cleanup_probe_is_zero
#define complex_mul complex_cleanup_probe_mul
#define complex_new complex_cleanup_probe_new
#define complex_new_add complex_cleanup_probe_new_add
#define complex_new_cast complex_cleanup_probe_new_cast
#define complex_new_copy complex_cleanup_probe_new_copy
#define complex_new_div complex_cleanup_probe_new_div
#define complex_new_mul complex_cleanup_probe_new_mul
#define complex_new_pow complex_cleanup_probe_new_pow
#define complex_new_ref complex_cleanup_probe_new_ref
#define complex_new_sub complex_cleanup_probe_new_sub
#define complex_norm complex_cleanup_probe_norm
#define complex_pow complex_cleanup_probe_pow
#define complex_sub complex_cleanup_probe_sub
#define complex_to_f32 complex_cleanup_probe_to_f32
#define complex_to_f64 complex_cleanup_probe_to_f64
#define complex_to_f80 complex_cleanup_probe_to_f80
#define complex_to_f128 complex_cleanup_probe_to_f128
#define tag_clean complex_norm_tag_clean_probe
#undef LIBKC3_COMPLEX_H
#include "../libkc3/complex.c"
#undef tag_clean
#undef complex_to_f128
#undef complex_to_f80
#undef complex_to_f64
#undef complex_to_f32
#undef complex_sub
#undef complex_pow
#undef complex_norm
#undef complex_new_sub
#undef complex_new_ref
#undef complex_new_pow
#undef complex_new_mul
#undef complex_new_div
#undef complex_new_copy
#undef complex_new_cast
#undef complex_new_add
#undef complex_new
#undef complex_mul
#undef complex_is_zero
#undef complex_init_uw
#undef complex_init_u64
#undef complex_init_u32
#undef complex_init_u16
#undef complex_init_u8
#undef complex_init_sw
#undef complex_init_s64
#undef complex_init_s32
#undef complex_init_s16
#undef complex_init_s8
#undef complex_init_ratio
#undef complex_init_integer
#undef complex_init_f128
#undef complex_init_f80
#undef complex_init_f64
#undef complex_init_f32
#undef complex_init_copy
#undef complex_init_cast
#undef complex_init
#undef complex_div
#undef complex_delete
#undef complex_clean
#undef complex_add
#undef DEF_COMPLEX_INIT

static int complex_child (char (*check)(void))
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

static char complex_norm_arithmetic_failure_check (void)
{
  s_complex c = {0};
  s_complex nested = {0};
  s_tag dest = {0};
  char failed;
  tag_init_f64(&nested.x, NAN);
  if (! tag_init_ratio_1(&nested.y, "1/2"))
    return 1;
  c.x.type = TAG_PCOMPLEX;
  c.x.data.td_pcomplex = &nested;
  tag_init_f64(&c.y, 1.0);
  tag_init_u8(&dest, 23);
  failed = complex_norm(&c, &dest) || dest.type != TAG_U8 ||
    dest.data.td_u8 != 23;
  complex_clean(&nested);
  return failed;
}

TEST_CASE(complex_norm_arithmetic_failure)
{
  TEST_EQ(complex_child(complex_norm_arithmetic_failure_check), 0);
}
TEST_CASE_END(complex_norm_arithmetic_failure)

static char complex_to_float_norm_failure_check (void)
{
  s_complex c = {0};
  s_complex nested = {0};
  char failed;
  tag_init_f64(&nested.x, NAN);
  if (! tag_init_ratio_1(&nested.y, "1/2"))
    return 1;
  c.x.type = TAG_PCOMPLEX;
  c.x.data.td_pcomplex = &nested;
  tag_init_f64(&c.y, 1.0);
  failed = ! isnan(complex_to_f32(&c)) ||
    ! isnan(complex_to_f64(&c));
#if HAVE_F80
  failed |= ! isnan(complex_to_f80(&c));
#endif
#if HAVE_F128
  failed |= ! isnan((f64) complex_to_f128(&c));
#endif
  complex_clean(&nested);
  return failed;
}

TEST_CASE(complex_to_float_norm_failure)
{
  TEST_EQ(complex_child(complex_to_float_norm_failure_check), 0);
}
TEST_CASE_END(complex_to_float_norm_failure)

static char complex_to_float_norm_cleanup_check (void)
{
  s_complex c = {0};
  s_tag norm = {0};
  uw expected;
  char failed = 0;
  if (! tag_init_ratio_1(&c.x, "3/5") ||
      ! tag_init_ratio_1(&c.y, "4/5")) {
    complex_clean(&c);
    return 1;
  }
  g_complex_norm_tag_clean_count = 0;
  if (! complex_cleanup_probe_norm(&c, &norm)) {
    complex_clean(&c);
    return 1;
  }
  complex_norm_tag_clean_probe(&norm);
  expected = g_complex_norm_tag_clean_count;
  failed = ! expected;
  g_complex_norm_tag_clean_count = 0;
  failed |= complex_cleanup_probe_to_f32(&c) != 1.0f ||
    g_complex_norm_tag_clean_count != expected;
  g_complex_norm_tag_clean_count = 0;
  failed |= complex_cleanup_probe_to_f64(&c) != 1.0 ||
    g_complex_norm_tag_clean_count != expected;
#if HAVE_F80
  g_complex_norm_tag_clean_count = 0;
  failed |= complex_cleanup_probe_to_f80(&c) != (f80) 1.0 ||
    g_complex_norm_tag_clean_count != expected;
#endif
#if HAVE_F128
  g_complex_norm_tag_clean_count = 0;
  failed |= complex_cleanup_probe_to_f128(&c) != (f128) 1.0 ||
    g_complex_norm_tag_clean_count != expected;
#endif
  complex_clean(&c);
  return failed;
}

TEST_CASE(complex_to_float_norm_cleanup)
{
  TEST_EQ(complex_to_float_norm_cleanup_check(), 0);
}
TEST_CASE_END(complex_to_float_norm_cleanup)

void complex_test (void)
{
  TEST_CASE_RUN(complex_norm_arithmetic_failure);
  TEST_CASE_RUN(complex_to_float_norm_failure);
  TEST_CASE_RUN(complex_to_float_norm_cleanup);
}
