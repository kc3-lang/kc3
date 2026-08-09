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
#include <pthread.h>
#include <sched.h>
#include "../libkc3/buf_inspect.h"
#include "../libkc3/list.h"
#include "../libkc3/stacktrace.h"
#include "../libkc3/str.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "test.h"

#define STACKTRACE_TEST_ITERATIONS 100000

typedef struct stacktrace_test_pop_data {
  bool          done;
  p_list        list;
  s_stacktrace *stacktrace;
} s_stacktrace_test_pop_data;

typedef struct stacktrace_test_stress_data {
  s_list        base;
  bool          error;
  bool          start;
  s_stacktrace *stacktrace;
} s_stacktrace_test_stress_data;

static void * stacktrace_test_pop_thread (void *arg);
static void * stacktrace_test_stress_reader (void *arg);
static void * stacktrace_test_stress_writer (void *arg);
void stacktrace_test (void);
TEST_CASE_PROTOTYPE(pin);
TEST_CASE_PROTOTYPE(short_format);
TEST_CASE_PROTOTYPE(stress);

void stacktrace_test (void)
{
  TEST_CASE_RUN(pin);
  TEST_CASE_RUN(short_format);
  TEST_CASE_RUN(stress);
}

static void * stacktrace_test_pop_thread (void *arg)
{
  s_stacktrace_test_pop_data *data;
  data = arg;
  stacktrace_pop(data->stacktrace, data->list);
  __atomic_store_n(&data->done, true, __ATOMIC_SEQ_CST);
  return NULL;
}

static void * stacktrace_test_stress_reader (void *arg)
{
  char a[128];
  s_buf buf;
  s_stacktrace_test_stress_data *data;
  uw i;
  p_list list;
  data = arg;
  while (! __atomic_load_n(&data->start, __ATOMIC_SEQ_CST))
    sched_yield();
  for (i = 0; i < STACKTRACE_TEST_ITERATIONS; i++) {
    list = stacktrace_read_begin(data->stacktrace);
    buf_init(&buf, false, sizeof(a), a);
    if ((list && list != &data->base &&
         list_next(list) != &data->base) ||
        buf_inspect_stacktrace_short(&buf, list) < 0)
      __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
    stacktrace_read_end(data->stacktrace);
  }
  return NULL;
}

static void * stacktrace_test_stress_writer (void *arg)
{
  s_stacktrace_test_stress_data *data;
  uw i;
  s_list trace = {0};
  data = arg;
  tag_init_plist(&trace.next, &data->base);
  while (! __atomic_load_n(&data->start, __ATOMIC_SEQ_CST))
    sched_yield();
  for (i = 0; i < STACKTRACE_TEST_ITERATIONS; i++) {
    stacktrace_push(data->stacktrace, &trace);
    stacktrace_pop(data->stacktrace, &data->base);
  }
  return NULL;
}

TEST_CASE(pin)
{
  s_list base = {0};
  s_stacktrace_test_pop_data data = {0};
  p_list list;
  pthread_t thread;
  s_list trace = {0};
  s_stacktrace stacktrace;
  stacktrace_init(&stacktrace);
  tag_init_plist(&trace.next, &base);
  stacktrace_push(&stacktrace, &base);
  stacktrace_push(&stacktrace, &trace);
  list = stacktrace_read_begin(&stacktrace);
  TEST_EQ(list, &trace);
  data.list = &base;
  data.stacktrace = &stacktrace;
  TEST_EQ(pthread_create(&thread, NULL, stacktrace_test_pop_thread,
                         &data), 0);
  while (stacktrace_get(&stacktrace) == &trace)
    sched_yield();
  TEST_ASSERT(! __atomic_load_n(&data.done, __ATOMIC_SEQ_CST));
  TEST_EQ(list_next(list), &base);
  stacktrace_read_end(&stacktrace);
  TEST_EQ(pthread_join(thread, NULL), 0);
  TEST_ASSERT(__atomic_load_n(&data.done, __ATOMIC_SEQ_CST));
  stacktrace_pop(&stacktrace, NULL);
  stacktrace_clean(&stacktrace);
}
TEST_CASE_END(pin)

TEST_CASE(short_format)
{
  char a[128];
  s_buf buf;
  const s_str expected = STR("inner\nouter\n");
  s_list inner = {0};
  s_list inner_call = {0};
  s_list outer = {0};
  s_list outer_call = {0};
  s_str result = {0};
  tag_init_psym(&outer_call.tag, sym_1("outer"));
  tag_init_plist(&outer.tag, &outer_call);
  tag_init_plist(&outer.next, NULL);
  tag_init_psym(&inner_call.tag, sym_1("inner"));
  tag_init_plist(&inner.tag, &inner_call);
  tag_init_plist(&inner.next, &outer);
  buf_init(&buf, false, sizeof(a), a);
  TEST_EQ(buf_inspect_stacktrace_short(&buf, &inner), 12);
  TEST_EQ(buf_read_to_str(&buf, &result), 12);
  TEST_STR_EQ(result, expected);
  str_clean(&result);
}
TEST_CASE_END(short_format)

TEST_CASE(stress)
{
  s_stacktrace_test_stress_data data = {0};
  pthread_t reader;
  s_stacktrace stacktrace;
  pthread_t writer;
  stacktrace_init(&stacktrace);
  data.stacktrace = &stacktrace;
  stacktrace_push(&stacktrace, &data.base);
  TEST_EQ(pthread_create(&reader, NULL, stacktrace_test_stress_reader,
                         &data), 0);
  TEST_EQ(pthread_create(&writer, NULL, stacktrace_test_stress_writer,
                         &data), 0);
  __atomic_store_n(&data.start, true, __ATOMIC_SEQ_CST);
  TEST_EQ(pthread_join(writer, NULL), 0);
  TEST_EQ(pthread_join(reader, NULL), 0);
  TEST_ASSERT(! __atomic_load_n(&data.error, __ATOMIC_SEQ_CST));
  stacktrace_pop(&stacktrace, NULL);
  stacktrace_clean(&stacktrace);
}
TEST_CASE_END(stress)
