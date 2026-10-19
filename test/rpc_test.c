#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/memleak.h"
#include "../libkc3/module.h"
#include "../libkc3/rpc.h"
#include "../libkc3/str.h"
#include "../libkc3/struct.h"
#include "../libkc3/tag.h"
#include "test.h"

static u8 g_rpc_fail;
static uw g_rpc_inits;
static uw g_rpc_cleans;
static uw g_rpc_reads;
static bool g_rpc_clean_borrowed;

static s_buf * rpc_test_buf_init (s_buf *buf, bool p_free, uw size,
                                 char *p)
{
  g_rpc_inits++;
  if (g_rpc_fail && g_rpc_fail == g_rpc_inits)
    return NULL;
  return buf_init(buf, p_free, size, p);
}

static void rpc_test_buf_clean (s_buf *buf)
{
  s_env *env = env_global();
  g_rpc_cleans++;
  if (env->err == buf || env->out == buf)
    g_rpc_clean_borrowed = true;
  buf_clean(buf);
}

static s_tag * rpc_test_tag_init_from_str (s_tag *tag, const s_str *str)
{
  return g_rpc_fail == 3 ? NULL : tag_init_from_str(tag, str);
}

static s_tag * rpc_test_tag_init_pstruct (s_tag *tag, const s_sym *module)
{
  return g_rpc_fail == 4 ? NULL : tag_init_pstruct(tag, module);
}

static s_struct * rpc_test_struct_allocate (s_struct *s)
{
  return g_rpc_fail == 5 ? NULL : struct_allocate(s);
}

static sw rpc_test_buf_read_to_str (s_buf *buf, s_str *dest)
{
  g_rpc_reads++;
  return g_rpc_fail == 5 + g_rpc_reads ? -1 : buf_read_to_str(buf, dest);
}

#define buf_init rpc_test_buf_init
#define buf_clean rpc_test_buf_clean
#define tag_init_from_str rpc_test_tag_init_from_str
#define tag_init_pstruct rpc_test_tag_init_pstruct
#define struct_allocate rpc_test_struct_allocate
#define buf_read_to_str rpc_test_buf_read_to_str
#define rpc_request rpc_test_request
#define rpc_response_clean rpc_test_response_clean
#include "../libkc3/rpc.c"
#undef buf_init
#undef buf_clean
#undef tag_init_from_str
#undef tag_init_pstruct
#undef struct_allocate
#undef buf_read_to_str
#undef rpc_request
#undef rpc_response_clean

static int rpc_child_test (char (*test)(bool), bool arg)
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

static char rpc_response_test (const s_str *input, bool output)
{
  s_tag dest;
  s_env *env = env_global();
  s_buf *err = env->err;
  s_buf *out = env->out;
  s_rpc_response *response;
  TEST_EQ(rpc_request(input, &dest), &dest);
  TEST_EQ(env->err, err);
  TEST_EQ(env->out, out);
  TEST_EQ(dest.type, TAG_PSTRUCT);
  response = dest.data.td_pstruct->data;
  TEST_ASSERT(response);
  TEST_EQ(response->result.type, TAG_STR);
  TEST_EQ(response->result.data.td_str.size, 5);
  TEST_MEMCMP(response->result.data.td_str.ptr.p_pchar, "value", 5);
  TEST_EQ(response->out.size, output ? 3 : 0);
  TEST_EQ(response->err.size, output ? 3 : 0);
  if (output) {
    TEST_MEMCMP(response->out.ptr.p_pchar, "out", 3);
    TEST_MEMCMP(response->err.ptr.p_pchar, "err", 3);
  }
  tag_clean(&dest);
  return 0;
}

static char rpc_cleanup_test (bool unused)
{
  const s_str input = STR("{ IO.write_str(\"out\"); "
                          "Err.write_str(\"err\"); \"value\" }");
  const s_str quiet = STR("\"value\"");
  uw i;
  (void) unused;
  TEST_ASSERT(module_load(&g_sym_RPC_Response));
  TEST_EQ(rpc_response_test(&input, true), 0);
  TEST_EQ(rpc_response_test(&quiet, false), 0);
  memleak_init();
  g_memleak_enabled = true;
  for (i = 0; i < 32; i++) {
    TEST_EQ(rpc_response_test(&input, true), 0);
    TEST_EQ(g_memleak, NULL);
    TEST_EQ(rpc_response_test(&quiet, false), 0);
    TEST_EQ(g_memleak, NULL);
  }
  g_memleak_enabled = false;
  memleak_remove_all();
  return 0;
}

static char rpc_failure_cleanup_test (bool init_only)
{
  s_tag dest;
  s_env *env = env_global();
  s_buf *err = env->err;
  uw i;
  const s_str input = STR("{ IO.write_str(\"out\"); "
                          "Err.write_str(\"err\"); \"value\" }");
  s_buf *out = env->out;
  const s_tag unchanged = {.type = TAG_U8, .data.td_u8 = 123};
  TEST_ASSERT(module_load(&g_sym_RPC_Response));
  TEST_EQ(rpc_response_test(&input, true), 0);
  memleak_init();
  g_memleak_enabled = true;
  for (g_rpc_fail = init_only ? 1 : 3;
       g_rpc_fail <= (init_only ? 2 : 7); g_rpc_fail++) {
    for (i = 0; i < 8; i++) {
      dest = unchanged;
      g_rpc_inits = 0;
      g_rpc_cleans = 0;
      g_rpc_reads = 0;
      g_rpc_clean_borrowed = false;
      TEST_EQ(rpc_test_request(&input, &dest), NULL);
      TEST_EQ(env->err, err);
      TEST_EQ(env->out, out);
      TEST_EQ(g_rpc_clean_borrowed, false);
      TEST_EQ(g_rpc_inits, g_rpc_fail == 1 ? 1 : 2);
      TEST_EQ(g_rpc_cleans, init_only ? g_rpc_fail - 1 : 2);
      TEST_EQ(dest.type, unchanged.type);
      TEST_EQ(dest.data.td_u8, unchanged.data.td_u8);
      TEST_EQ(g_memleak, NULL);
    }
  }
  g_rpc_fail = 0;
  g_memleak_enabled = false;
  memleak_remove_all();
  return 0;
}

TEST_CASE_PROTOTYPE(rpc_request_cleanup);
TEST_CASE_PROTOTYPE(rpc_request_init_failure);
#ifndef DEBUG
TEST_CASE_PROTOTYPE(rpc_request_failure_cleanup);
#endif

void rpc_test (void)
{
  (void) rpc_test_buf_clean;
  TEST_CASE_RUN(rpc_request_cleanup);
  TEST_CASE_RUN(rpc_request_init_failure);
#ifndef DEBUG
  TEST_CASE_RUN(rpc_request_failure_cleanup);
#endif
}

TEST_CASE(rpc_request_cleanup)
{
  TEST_EQ(rpc_child_test(rpc_cleanup_test, false), 0);
}
TEST_CASE_END(rpc_request_cleanup)

TEST_CASE(rpc_request_init_failure)
{
  TEST_EQ(rpc_child_test(rpc_failure_cleanup_test, true), 0);
}
TEST_CASE_END(rpc_request_init_failure)

#ifndef DEBUG
TEST_CASE(rpc_request_failure_cleanup)
{
  TEST_EQ(rpc_child_test(rpc_failure_cleanup_test, false), 0);
}
TEST_CASE_END(rpc_request_failure_cleanup)
#endif
