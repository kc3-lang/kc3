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

void complex_test (void)
{
  TEST_CASE_RUN(complex_norm_arithmetic_failure);
}
