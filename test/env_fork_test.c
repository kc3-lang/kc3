#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/kc3.h"
#include "../libkc3/env_fork.h"
#include "../libkc3/memleak.h"
#include "../libkc3/mutex.h"
#include "../libkc3/ops.h"
#include "test.h"

static u8 g_env_fork_fail;
static sw g_env_fork_mutexes;
static bool g_env_fork_pin;
static bool g_env_fork_waiting;
static bool g_env_fork_done;

static int env_fork_yield_test (void)
{
  if (g_env_fork_pin)
    __atomic_store_n(&g_env_fork_waiting, true, __ATOMIC_SEQ_CST);
  return sched_yield();
}

#define sched_yield env_fork_yield_test
#define stacktrace_clean env_fork_trace_clean
#define stacktrace_delete env_fork_trace_delete
#define stacktrace_init env_fork_trace_init
#define stacktrace_new env_fork_trace_new
#define stacktrace_read_begin env_fork_trace_read_begin
#define stacktrace_read_end env_fork_trace_read_end
#define stacktrace_wait env_fork_trace_wait

#include "../libkc3/stacktrace.c"

#undef sched_yield
#undef stacktrace_clean
#undef stacktrace_delete
#undef stacktrace_init
#undef stacktrace_new
#undef stacktrace_read_begin
#undef stacktrace_read_end
#undef stacktrace_wait

static void * env_fork_alloc_test (uw size)
{
  return g_env_fork_fail == 1 ? NULL : alloc(size);
}

static s_frame * env_fork_frame_copy_test (const s_frame *src)
{
  return g_env_fork_fail == 2 ? NULL : frame_new_copy(src);
}

static s_ops * env_fork_ops_copy_test (s_ops *src)
{
  return g_env_fork_fail == 3 ? NULL : ops_new_copy(src);
}

static s_frame * env_fork_frame_test (s_frame *next)
{
  return g_env_fork_fail == 4 ? NULL : frame_new(next);
}

static s_stacktrace * env_fork_stacktrace_test (void)
{
  return g_env_fork_fail == 5 ? NULL : env_fork_trace_new();
}

#if HAVE_PTHREAD
static s_mutex * env_fork_mutex_init_test (s_mutex *mutex)
{
  s_mutex *result = mutex_init(mutex);
  if (result)
    g_env_fork_mutexes++;
  return result;
}

static void env_fork_mutex_clean_test (s_mutex *mutex)
{
  if (mutex->ready)
    g_env_fork_mutexes--;
  mutex_clean(mutex);
}
#endif

#define alloc env_fork_alloc_test
#define frame_new_copy env_fork_frame_copy_test
#define ops_new_copy env_fork_ops_copy_test
#define frame_new env_fork_frame_test
#define stacktrace_new env_fork_stacktrace_test
#define stacktrace_delete env_fork_trace_delete
#define mutex_init env_fork_mutex_init_test
#define mutex_clean env_fork_mutex_clean_test
#define env_fork_clean env_fork_fault_clean
#define env_fork_delete env_fork_fault_delete
#define env_fork_init env_fork_fault_init
#define env_fork_new env_fork_fault_new

#include "../libkc3/env_fork.c"

#undef alloc
#undef frame_new_copy
#undef ops_new_copy
#undef frame_new
#undef stacktrace_new
#undef stacktrace_delete
#undef mutex_init
#undef mutex_clean
#undef env_fork_clean
#undef env_fork_delete
#undef env_fork_init
#undef env_fork_new

static uw env_fork_live_test (void)
{
  s_memleak *item;
  uw count = 0;
  for (item = g_memleak; item; item = item->next)
    count++;
  return count;
}

static bool env_fork_tracked_test (uw address)
{
  s_memleak *item;
  for (item = g_memleak; item; item = item->next)
    if ((uw) item->ptr == address)
      return true;
  return false;
}

static void env_fork_tracking_end_test (void)
{
  g_memleak_enabled = false;
  memleak_remove_all();
}

static char env_fork_cycles_test (void)
{
  s_env *env;
  s_env local;
  s_env *parent = env_global();
  s_stacktrace *parent_trace = parent->stacktrace;
  uw i;
  TEST_ASSERT((env = env_fork_new(parent)));
  env_fork_delete(env);
  memleak_init();
  for (i = 0; i < 32; i++) {
    TEST_EQ(env_fork_init(&local, parent), &local);
    TEST_ASSERT(local.stacktrace);
    TEST_ASSERT(local.stacktrace != parent_trace);
    TEST_EQ(stacktrace_get(local.stacktrace), NULL);
    env_fork_clean(&local);
    TEST_EQ(local.stacktrace, NULL);
    TEST_ASSERT((env = env_fork_new(parent)));
    TEST_ASSERT(env->stacktrace != parent_trace);
    env_fork_delete(env);
    TEST_EQ(env_fork_live_test(), 0);
    TEST_EQ(env_global(), parent);
    TEST_EQ(parent->stacktrace, parent_trace);
  }
  env_fork_tracking_end_test();
  return 0;
}

static char env_fork_threads_test (void)
{
  s_tag callable = {0};
  s_tag result = {0};
  u_ptr_w thread;
  uw i;
  TEST_ASSERT(tag_init_1(&callable, "fn () { 23 }"));
  TEST_EQ(kc3_thread_new(&thread, &callable.data.td_pcallable), &thread);
  TEST_EQ(kc3_thread_delete(&thread, &result), &result);
  tag_clean(&result);
  memleak_init();
  for (i = 0; i < 32; i++) {
    TEST_EQ(kc3_thread_new(&thread, &callable.data.td_pcallable), &thread);
    TEST_EQ(kc3_thread_delete(&thread, &result), &result);
    TEST_EQ(result.type, TAG_U8);
    TEST_EQ(result.data.td_u8, 23);
    tag_clean(&result);
    TEST_EQ(env_fork_live_test(), 0);
  }
  env_fork_tracking_end_test();
  tag_clean(&callable);
  return 0;
}

static char env_fork_thread_error_test (void)
{
  s_tag callable = {0};
  s_tag result = {0};
  u_ptr_w thread;
  uw i;
  TEST_ASSERT(tag_init_1(&callable, "fn () { KC3.f23_unbound }"));
  TEST_EQ(kc3_thread_new(&thread, &callable.data.td_pcallable), &thread);
  TEST_EQ(kc3_thread_delete(&thread, &result), &result);
  tag_clean(&result);
  memleak_init();
  for (i = 0; i < 8; i++) {
    TEST_EQ(kc3_thread_new(&thread, &callable.data.td_pcallable), &thread);
    TEST_EQ(kc3_thread_delete(&thread, &result), &result);
    TEST_EQ(result.type, TAG_VOID);
    tag_clean(&result);
    TEST_EQ(env_fork_live_test(), 0);
  }
  env_fork_tracking_end_test();
  tag_clean(&callable);
  return 0;
}

static char env_fork_root_test (void)
{
  s_env env;
  s_env *parent = env_global();
  uw i;
  uw trace;
  TEST_EQ(env_init(&env, NULL, NULL), &env);
  env_clean(&env);
  memleak_init();
  for (i = 0; i < 3; i++) {
    TEST_EQ(env_init(&env, NULL, NULL), &env);
    trace = (uw) env.stacktrace;
    TEST_ASSERT(env_fork_tracked_test(trace));
    env_clean(&env);
    TEST_EQ(env.stacktrace, NULL);
    TEST_EQ(env_global(), parent);
    TEST_ASSERT(! env_fork_tracked_test(trace));
  }
  env_fork_tracking_end_test();
  return 0;
}

static char env_fork_root_failure_test (void)
{
  char *args[] = {"/", NULL};
  char **argv = args;
  int argc = 1;
  s_env env = {0};
  s_env *parent = env_global();
  memleak_init();
  TEST_EQ(env_init(&env, &argc, &argv), NULL);
  TEST_EQ(env.stacktrace, NULL);
  TEST_EQ(env_global(), parent);
  TEST_EQ(env_fork_live_test(), 0);
  env_fork_tracking_end_test();
  return 0;
}

static char env_fork_partial_test (void)
{
  s_env env = {0};
  memleak_init();
  TEST_ASSERT((env.stacktrace = stacktrace_new()));
  env_clean(&env);
  TEST_EQ(env.stacktrace, NULL);
  TEST_EQ(env_fork_live_test(), 0);
  env_fork_tracking_end_test();
  return 0;
}

static char env_fork_default_test (void)
{
  kc3_clean(NULL);
  return env_global() || env_default() ? 1 : 0;
}

static char env_fork_default_failure_test (void)
{
  char *args[] = {"/", NULL};
  char **argv = args;
  int argc = 1;
  kc3_clean(NULL);
  if (env_global() || env_default())
    return 1;
  memleak_init();
  if (env_init(NULL, &argc, &argv) || env_global() || env_default() ||
      env_fork_live_test())
    return 1;
  env_fork_tracking_end_test();
  return 0;
}

static char env_fork_failure_test (void)
{
  s_env *env;
  s_env local = {0};
  s_env *parent = env_global();
  TEST_ASSERT((env = env_fork_fault_new(parent)));
  env_fork_fault_delete(env);
  TEST_EQ(g_env_fork_mutexes, 0);
  memleak_init();
  for (g_env_fork_fail = 1; g_env_fork_fail <= 5; g_env_fork_fail++) {
    TEST_EQ(env_fork_fault_new(parent), NULL);
    TEST_EQ(g_env_fork_mutexes, 0);
    TEST_EQ(env_fork_live_test(), 0);
    if (g_env_fork_fail > 1) {
      local.stacktrace = parent->stacktrace;
      TEST_EQ(env_fork_fault_init(&local, parent), NULL);
      TEST_EQ(local.stacktrace, parent->stacktrace);
      TEST_EQ(g_env_fork_mutexes, 0);
      TEST_EQ(env_fork_live_test(), 0);
    }
  }
  g_env_fork_fail = 0;
  env_fork_tracking_end_test();
  return 0;
}

static void * env_fork_delete_thread_test (void *arg)
{
  env_global_set(arg);
  env_fork_fault_delete(arg);
  __atomic_store_n(&g_env_fork_done, true, __ATOMIC_SEQ_CST);
  return NULL;
}

static char env_fork_reader_test (void)
{
  s_env *env;
  s_stacktrace *trace;
  pthread_t thread;
  memleak_init();
  TEST_ASSERT((env = env_fork_fault_new(env_global())));
  trace = env->stacktrace;
  TEST_EQ(stacktrace_read_begin(trace), NULL);
  g_env_fork_pin = true;
  TEST_EQ(pthread_create(&thread, NULL, env_fork_delete_thread_test,
                         env), 0);
  while (! __atomic_load_n(&g_env_fork_waiting, __ATOMIC_SEQ_CST) &&
         ! __atomic_load_n(&g_env_fork_done, __ATOMIC_SEQ_CST))
    sched_yield();
  TEST_ASSERT(__atomic_load_n(&g_env_fork_waiting, __ATOMIC_SEQ_CST));
  TEST_ASSERT(! __atomic_load_n(&g_env_fork_done, __ATOMIC_SEQ_CST));
  TEST_EQ(stacktrace_get(trace), NULL);
  stacktrace_read_end(trace);
  TEST_EQ(pthread_join(thread, NULL), 0);
  TEST_ASSERT(__atomic_load_n(&g_env_fork_done, __ATOMIC_SEQ_CST));
  TEST_EQ(g_env_fork_mutexes, 0);
  TEST_EQ(env_fork_live_test(), 0);
  env_fork_tracking_end_test();
  return 0;
}

static int env_fork_child_test (char (*test)(void))
{
  pid_t pid;
  pid_t result;
  int status;
  if ((pid = fork()) < 0)
    return -1;
  if (! pid) {
    alarm(30);
    _exit(test());
  }
  do {
    result = waitpid(pid, &status, 0);
  } while (result < 0 && errno == EINTR);
  if (result != pid || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

#define ENV_FORK_CASE(name)                            \
  TEST_CASE(env_fork_ ## name)                         \
  {                                                   \
    TEST_EQ(env_fork_child_test(env_fork_ ## name ## _test), 0); \
  }                                                   \
  TEST_CASE_END(env_fork_ ## name)

ENV_FORK_CASE(cycles)
ENV_FORK_CASE(threads)
ENV_FORK_CASE(thread_error)
ENV_FORK_CASE(root)
ENV_FORK_CASE(root_failure)
ENV_FORK_CASE(partial)
ENV_FORK_CASE(default)
ENV_FORK_CASE(default_failure)
ENV_FORK_CASE(failure)
ENV_FORK_CASE(reader)

void env_fork_test (void)
{
  TEST_CASE_RUN(env_fork_cycles);
  TEST_CASE_RUN(env_fork_threads);
  TEST_CASE_RUN(env_fork_thread_error);
  TEST_CASE_RUN(env_fork_root);
  TEST_CASE_RUN(env_fork_root_failure);
  TEST_CASE_RUN(env_fork_partial);
  TEST_CASE_RUN(env_fork_default);
  TEST_CASE_RUN(env_fork_default_failure);
  TEST_CASE_RUN(env_fork_failure);
  TEST_CASE_RUN(env_fork_reader);
}
