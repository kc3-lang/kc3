#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/base64.h"
#include "../libkc3/memleak.h"
#include "../libkc3/str.h"
#include "test.h"

static uw g_base64_alloc_calls;
static u32 g_base64_alloc_size;
static u8 g_base64_fail;

static s_buf * base64_test_buf_init_alloc (s_buf *buf, uw size)
{
  return g_base64_fail == 1 ? NULL : buf_init_alloc(buf, size);
}

static sw base64_test_buf_read_to_str (s_buf *buf, s_str *dest)
{
  return g_base64_fail == 2 ? -1 : buf_read_to_str(buf, dest);
}

static s_str * base64_test_str_init_alloc (s_str *str, u32 size)
{
  (void) str;
  g_base64_alloc_calls++;
  g_base64_alloc_size = size;
  return NULL;
}

#define base64_decode base64_test_decode
#define base64_encode base64_test_encode
#define str_init_alloc base64_test_str_init_alloc
#define buf_init_alloc base64_test_buf_init_alloc
#define buf_read_to_str base64_test_buf_read_to_str
#include "../libkc3/base64.c"
#undef buf_read_to_str
#undef buf_init_alloc
#undef str_init_alloc
#undef base64_encode
#undef base64_decode

static int base64_child_test (char (*test)(bool), bool arg)
{
  pid_t child;
  pid_t waited;
  int status;
  if ((child = fork()) < 0)
    return -1;
  if (! child) {
    alarm(30);
    _exit(test(arg));
  }
  do {
    waited = waitpid(child, &status, 0);
  } while (waited < 0 && errno == EINTR);
  if (waited != child || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

static char base64_cleanup_test (bool encode)
{
  const struct {
    s_str plain;
    s_str encoded;
  } cases[] = {
    {STR(""), STR("")},
    {STR("f"), STR("Zg==")},
    {STR("fo"), STR("Zm8=")},
    {STR("foobar"), STR("Zm9vYmFy")},
    {STR("\0\xFF\0"), STR("AP8A")}
  };
  uw cycle;
  s_str dest;
  const s_str *expected;
  uw i;
  s_str src;
  memleak_init();
  g_memleak_enabled = true;
  for (cycle = 0; cycle < 32; cycle++) {
    for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
      src = encode ? cases[i].plain : cases[i].encoded;
      expected = encode ? &cases[i].encoded : &cases[i].plain;
      TEST_EQ(encode ? base64_encode(&src, &dest) :
                       base64_decode(&src, &dest), &dest);
      TEST_EQ(dest.size, expected->size);
      if (dest.size)
        TEST_MEMCMP(dest.ptr.p_pchar, expected->ptr.p_pchar, dest.size);
      TEST_EQ(src.ptr.p_pvoid, encode ? cases[i].plain.ptr.p_pvoid :
                                       cases[i].encoded.ptr.p_pvoid);
      TEST_EQ(src.free.p_pvoid, NULL);
      str_clean(&dest);
      TEST_EQ(g_memleak, NULL);
    }
  }
  g_memleak_enabled = false;
  memleak_remove_all();
  return 0;
}

static char base64_failure_cleanup_test (bool unused)
{
  uw cycle;
  s_str dest;
  s_str src = STR("Zm9vYmFy");
  const s_str unchanged = STR("unchanged");
  (void) unused;
  memleak_init();
  g_memleak_enabled = true;
  for (g_base64_fail = 1; g_base64_fail <= 2; g_base64_fail++) {
    for (cycle = 0; cycle < 16; cycle++) {
      dest = unchanged;
      TEST_EQ(base64_test_decode(&src, &dest), NULL);
      TEST_EQ(dest.ptr.p_pvoid, unchanged.ptr.p_pvoid);
      TEST_EQ(dest.free.p_pvoid, unchanged.free.p_pvoid);
      TEST_EQ(dest.size, unchanged.size);
      TEST_MEMCMP(dest.ptr.p_pchar, unchanged.ptr.p_pchar, dest.size);
      TEST_EQ(g_memleak, NULL);
    }
  }
  g_base64_fail = 0;
  g_memleak_enabled = false;
  memleak_remove_all();
  return 0;
}

TEST_CASE_PROTOTYPE(base64_encode_vectors);
TEST_CASE_PROTOTYPE(base64_encode_size_boundaries);
TEST_CASE_PROTOTYPE(base64_encode_size_overflow);
TEST_CASE_PROTOTYPE(base64_encode_cleanup);
TEST_CASE_PROTOTYPE(base64_decode_cleanup);
TEST_CASE_PROTOTYPE(base64_decode_failure_cleanup);

void base64_test (void)
{
  TEST_CASE_RUN(base64_encode_vectors);
  TEST_CASE_RUN(base64_encode_size_boundaries);
  TEST_CASE_RUN(base64_encode_size_overflow);
  TEST_CASE_RUN(base64_encode_cleanup);
  TEST_CASE_RUN(base64_decode_cleanup);
  TEST_CASE_RUN(base64_decode_failure_cleanup);
}

TEST_CASE(base64_encode_cleanup)
{
  TEST_EQ(base64_child_test(base64_cleanup_test, true), 0);
}
TEST_CASE_END(base64_encode_cleanup)

TEST_CASE(base64_decode_cleanup)
{
  TEST_EQ(base64_child_test(base64_cleanup_test, false), 0);
}
TEST_CASE_END(base64_decode_cleanup)

TEST_CASE(base64_decode_failure_cleanup)
{
  TEST_EQ(base64_child_test(base64_failure_cleanup_test, false), 0);
}
TEST_CASE_END(base64_decode_failure_cleanup)

TEST_CASE(base64_encode_vectors)
{
  const struct {
    s_str src;
    s_str expected;
  } cases[] = {
    {STR(""), STR("")},
    {STR("f"), STR("Zg==")},
    {STR("fo"), STR("Zm8=")},
    {STR("foo"), STR("Zm9v")},
    {STR("foob"), STR("Zm9vYg==")},
    {STR("fooba"), STR("Zm9vYmE=")},
    {STR("foobar"), STR("Zm9vYmFy")},
    {STR("\0"), STR("AA==")},
    {STR("\0\0"), STR("AAA=")},
    {STR("\0\0\0"), STR("AAAA")},
    {STR("\xFF"), STR("/w==")},
    {STR("\xFF\xFF"), STR("//8=")},
    {STR("\xFF\xFF\xFF"), STR("////")},
    {STR("\xFB\xEF\xFF"), STR("++//")},
    {STR("\0\xFF\0"), STR("AP8A")}
  };
  s_str dest;
  uw i;
  s_str src;
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    src = cases[i].src;
    TEST_EQ(base64_encode(&src, &dest), &dest);
    TEST_EQ(dest.size, cases[i].expected.size);
    if (dest.size) {
      TEST_MEMCMP(dest.ptr.p_pchar, cases[i].expected.ptr.p_pchar,
                  dest.size);
      TEST_EQ(dest.ptr.p_pu8[dest.size], 0);
    }
    else {
      TEST_EQ(dest.ptr.p_pvoid, NULL);
      TEST_EQ(dest.free.p_pvoid, NULL);
    }
    TEST_EQ(src.size, cases[i].src.size);
    TEST_EQ(src.ptr.p_pvoid, cases[i].src.ptr.p_pvoid);
    TEST_EQ(src.free.p_pvoid, cases[i].src.free.p_pvoid);
    str_clean(&dest);
  }
}
TEST_CASE_END(base64_encode_vectors)

TEST_CASE(base64_encode_size_boundaries)
{
  const struct {
    u32 src_size;
    u32 dest_size;
  } cases[] = {
    {1, 4}, {2, 4}, {3, 4}, {4, 8}, {5, 8}, {6, 8},
    {0xBFFFFFF7, 0xFFFFFFF4},
    {0xBFFFFFF8, 0xFFFFFFF8},
    {0xBFFFFFF9, 0xFFFFFFF8},
    {0xBFFFFFFA, 0xFFFFFFF8},
    {0xBFFFFFFB, 0xFFFFFFFC},
    {0xBFFFFFFC, 0xFFFFFFFC},
    {0xBFFFFFFD, 0xFFFFFFFC}
  };
  s_str dest;
  uw i;
  s_str src = {0};
  const s_str unchanged = STR("unchanged");
  for (i = 0; i < sizeof(cases) / sizeof(*cases); i++) {
    src.size = cases[i].src_size;
    dest = unchanged;
    g_base64_alloc_calls = 0;
    g_base64_alloc_size = 0;
    TEST_EQ(base64_test_encode(&src, &dest), NULL);
    TEST_EQ(g_base64_alloc_calls, 1);
    TEST_EQ(g_base64_alloc_size, cases[i].dest_size);
    TEST_ASSERT((u64) g_base64_alloc_size + 1 <= U32_MAX);
    TEST_EQ(dest.free.p_pvoid, unchanged.free.p_pvoid);
    TEST_EQ(dest.ptr.p_pvoid, unchanged.ptr.p_pvoid);
    TEST_EQ(dest.size, unchanged.size);
    TEST_MEMCMP(dest.ptr.p_pchar, unchanged.ptr.p_pchar, dest.size);
  }
}
TEST_CASE_END(base64_encode_size_boundaries)

TEST_CASE(base64_encode_size_overflow)
{
  const u32 sizes[] = {
    0xBFFFFFFE, 0xBFFFFFFF, 0xC0000000, 0xC0000001,
    0xFFFFFFFC, 0xFFFFFFFD, 0xFFFFFFFE, 0xFFFFFFFF
  };
  s_str dest;
  uw i;
  s_str src = {0};
  const s_str unchanged = STR("unchanged");
  for (i = 0; i < sizeof(sizes) / sizeof(*sizes); i++) {
    src.size = sizes[i];
    dest = unchanged;
    g_base64_alloc_calls = 0;
    g_base64_alloc_size = 0;
    TEST_EQ(base64_test_encode(&src, &dest), NULL);
    TEST_EQ(g_base64_alloc_calls, 0);
    TEST_EQ(g_base64_alloc_size, 0);
    TEST_EQ(dest.free.p_pvoid, unchanged.free.p_pvoid);
    TEST_EQ(dest.ptr.p_pvoid, unchanged.ptr.p_pvoid);
    TEST_EQ(dest.size, unchanged.size);
    TEST_MEMCMP(dest.ptr.p_pchar, unchanged.ptr.p_pchar, dest.size);
  }
}
TEST_CASE_END(base64_encode_size_overflow)
