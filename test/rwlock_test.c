#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/rwlock.h"
#include "test.h"

#define RWLOCK_TEST_ITERATIONS 10000
#define RWLOCK_TEST_THREADS 6

#if HAVE_PTHREAD

typedef struct rwlock_test_data {
  s_rwlock lock;
  bool start;
  bool error;
  uw readers;
  uw writers;
  uw value;
  uw mirror;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  bool acquired;
  bool release;
} s_rwlock_test_data;

static int rwlock_test_child (char (*test)(void))
{
  pid_t pid;
  pid_t r;
  int status;
  if ((pid = fork()) < 0)
    return -1;
  if (! pid) {
    alarm(30);
    _exit(test());
  }
  do {
    r = waitpid(pid, &status, 0);
  } while (r < 0 && errno == EINTR);
  if (r != pid || ! WIFEXITED(status))
    return -1;
  return WEXITSTATUS(status);
}

static void * rwlock_test_reader_hold (void *arg)
{
  s_rwlock_test_data *data = arg;
  if (! rwlock_r(&data->lock)) {
    __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
    return NULL;
  }
  pthread_mutex_lock(&data->mutex);
  data->acquired = true;
  pthread_cond_signal(&data->cond);
  while (! data->release)
    pthread_cond_wait(&data->cond, &data->mutex);
  pthread_mutex_unlock(&data->mutex);
  if (! rwlock_unlock_r(&data->lock))
    __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
  return NULL;
}

static void * rwlock_test_writer_once (void *arg)
{
  s_rwlock_test_data *data = arg;
  __atomic_store_n(&data->start, true, __ATOMIC_SEQ_CST);
  if (! rwlock_w(&data->lock)) {
    __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
    return NULL;
  }
  __atomic_add_fetch(&data->writers, 1, __ATOMIC_SEQ_CST);
  data->value++;
  data->mirror = data->value;
  if (! rwlock_unlock_w(&data->lock))
    __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
  return NULL;
}

static void * rwlock_test_stress_reader (void *arg)
{
  s_rwlock_test_data *data = arg;
  uw i;
  while (! __atomic_load_n(&data->start, __ATOMIC_SEQ_CST))
    sched_yield();
  for (i = 0; i < RWLOCK_TEST_ITERATIONS; i++) {
    if (! rwlock_r(&data->lock) || ! rwlock_r(&data->lock)) {
      __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
      return NULL;
    }
    __atomic_add_fetch(&data->readers, 1, __ATOMIC_SEQ_CST);
    if (__atomic_load_n(&data->writers, __ATOMIC_SEQ_CST) ||
        data->value != data->mirror)
      __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
    __atomic_sub_fetch(&data->readers, 1, __ATOMIC_SEQ_CST);
    if (! rwlock_unlock_r(&data->lock) ||
        ! rwlock_unlock_r(&data->lock)) {
      __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
      return NULL;
    }
  }
  return NULL;
}

static void * rwlock_test_stress_writer (void *arg)
{
  s_rwlock_test_data *data = arg;
  uw i;
  while (! __atomic_load_n(&data->start, __ATOMIC_SEQ_CST))
    sched_yield();
  for (i = 0; i < RWLOCK_TEST_ITERATIONS; i++) {
    if (! rwlock_w(&data->lock) || ! rwlock_w(&data->lock) ||
        ! rwlock_r(&data->lock)) {
      __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
      return NULL;
    }
    if (__atomic_add_fetch(&data->writers, 1, __ATOMIC_SEQ_CST) != 1 ||
        __atomic_load_n(&data->readers, __ATOMIC_SEQ_CST))
      __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
    data->value++;
    if (i % 128 == 0)
      sched_yield();
    data->mirror = data->value;
    __atomic_sub_fetch(&data->writers, 1, __ATOMIC_SEQ_CST);
    if (! rwlock_unlock_r(&data->lock) ||
        ! rwlock_unlock_w(&data->lock) ||
        ! rwlock_unlock_w(&data->lock)) {
      __atomic_store_n(&data->error, true, __ATOMIC_SEQ_CST);
      return NULL;
    }
  }
  return NULL;
}

static char rwlock_test_recursive (void)
{
  s_rwlock lock;
  uw i;
  TEST_EQ(rwlock_init(&lock), &lock);
  for (i = 0; i < 8; i++) {
    TEST_EQ(rwlock_w(&lock), &lock);
    TEST_EQ(lock.count, i + 1);
    TEST_EQ(rwlock_r(&lock), &lock);
    TEST_EQ(rwlock_r(&lock), &lock);
    TEST_EQ(rwlock_unlock_r(&lock), &lock);
    TEST_EQ(rwlock_unlock_r(&lock), &lock);
    TEST_EQ(lock.count, i + 1);
  }
  for (i = 8; i > 0; i--) {
    TEST_EQ(rwlock_unlock_w(&lock), &lock);
    TEST_EQ(lock.count, i - 1);
  }
  TEST_EQ(rwlock_r(&lock), &lock);
  TEST_EQ(rwlock_r(&lock), &lock);
  TEST_EQ(rwlock_unlock_r(&lock), &lock);
  TEST_EQ(rwlock_unlock_r(&lock), &lock);
  TEST_EQ(pthread_rwlock_trywrlock(&lock.rwlock), 0);
  TEST_EQ(pthread_rwlock_unlock(&lock.rwlock), 0);
  rwlock_clean(&lock);
  return 0;
}

static char rwlock_test_readers (void)
{
  s_rwlock_test_data data = {0};
  pthread_t reader;
  pthread_t writer;
  TEST_EQ(rwlock_init(&data.lock), &data.lock);
  TEST_EQ(pthread_mutex_init(&data.mutex, NULL), 0);
  TEST_EQ(pthread_cond_init(&data.cond, NULL), 0);
  TEST_EQ(rwlock_r(&data.lock), &data.lock);
  TEST_EQ(pthread_create(&reader, NULL, rwlock_test_reader_hold, &data), 0);
  TEST_EQ(pthread_mutex_lock(&data.mutex), 0);
  while (! data.acquired)
    TEST_EQ(pthread_cond_wait(&data.cond, &data.mutex), 0);
  TEST_EQ(pthread_mutex_unlock(&data.mutex), 0);
  TEST_EQ(pthread_create(&writer, NULL, rwlock_test_writer_once, &data), 0);
  while (! __atomic_load_n(&data.start, __ATOMIC_SEQ_CST))
    sched_yield();
  TEST_EQ(__atomic_load_n(&data.writers, __ATOMIC_SEQ_CST), 0);
  TEST_EQ(rwlock_unlock_r(&data.lock), &data.lock);
  TEST_EQ(__atomic_load_n(&data.writers, __ATOMIC_SEQ_CST), 0);
  TEST_EQ(pthread_mutex_lock(&data.mutex), 0);
  data.release = true;
  TEST_EQ(pthread_cond_signal(&data.cond), 0);
  TEST_EQ(pthread_mutex_unlock(&data.mutex), 0);
  TEST_EQ(pthread_join(reader, NULL), 0);
  TEST_EQ(pthread_join(writer, NULL), 0);
  TEST_EQ(data.writers, 1);
  TEST_EQ(data.value, 1);
  TEST_EQ(data.error, false);
  TEST_EQ(pthread_cond_destroy(&data.cond), 0);
  TEST_EQ(pthread_mutex_destroy(&data.mutex), 0);
  rwlock_clean(&data.lock);
  return 0;
}

static char rwlock_test_handoff (void)
{
  s_rwlock_test_data data = {0};
  uw i;
  pthread_t writer;
  TEST_EQ(rwlock_init(&data.lock), &data.lock);
  for (i = 0; i < 100; i++) {
    __atomic_store_n(&data.start, false, __ATOMIC_SEQ_CST);
    TEST_EQ(rwlock_w(&data.lock), &data.lock);
    TEST_EQ(rwlock_w(&data.lock), &data.lock);
    TEST_EQ(rwlock_r(&data.lock), &data.lock);
    TEST_EQ(pthread_create(&writer, NULL, rwlock_test_writer_once,
                           &data), 0);
    while (! __atomic_load_n(&data.start, __ATOMIC_SEQ_CST))
      sched_yield();
    TEST_EQ(rwlock_unlock_r(&data.lock), &data.lock);
    TEST_EQ(rwlock_unlock_w(&data.lock), &data.lock);
    TEST_EQ(__atomic_load_n(&data.writers, __ATOMIC_SEQ_CST), i);
    TEST_EQ(rwlock_unlock_w(&data.lock), &data.lock);
    TEST_EQ(pthread_join(writer, NULL), 0);
    TEST_EQ(data.writers, i + 1);
    TEST_EQ(data.lock.count, 0);
  }
  TEST_EQ(data.value, 100);
  TEST_EQ(data.error, false);
  rwlock_clean(&data.lock);
  return 0;
}

static char rwlock_test_independent (void)
{
  s_rwlock a;
  s_rwlock_test_data b = {0};
  pthread_t writer;
  TEST_EQ(rwlock_init(&a), &a);
  TEST_EQ(rwlock_init(&b.lock), &b.lock);
  TEST_EQ(rwlock_w(&a), &a);
  TEST_EQ(pthread_create(&writer, NULL, rwlock_test_writer_once, &b), 0);
  TEST_EQ(pthread_join(writer, NULL), 0);
  TEST_EQ(b.value, 1);
  TEST_EQ(b.error, false);
  TEST_EQ(rwlock_w(&b.lock), &b.lock);
  TEST_EQ(rwlock_w(&a), &a);
  TEST_EQ(rwlock_r(&a), &a);
  TEST_EQ(rwlock_r(&b.lock), &b.lock);
  TEST_EQ(rwlock_unlock_r(&b.lock), &b.lock);
  TEST_EQ(rwlock_unlock_r(&a), &a);
  TEST_EQ(rwlock_unlock_w(&a), &a);
  TEST_EQ(rwlock_unlock_w(&b.lock), &b.lock);
  TEST_EQ(rwlock_unlock_w(&a), &a);
  rwlock_clean(&b.lock);
  rwlock_clean(&a);
  return 0;
}

static char rwlock_test_stress (void)
{
  s_rwlock_test_data data = {0};
  uw i;
  pthread_t threads[RWLOCK_TEST_THREADS];
  TEST_EQ(rwlock_init(&data.lock), &data.lock);
  for (i = 0; i < RWLOCK_TEST_THREADS; i++)
    TEST_EQ(pthread_create(threads + i, NULL,
                           i % 2 ? rwlock_test_stress_reader :
                           rwlock_test_stress_writer, &data), 0);
  __atomic_store_n(&data.start, true, __ATOMIC_SEQ_CST);
  for (i = 0; i < RWLOCK_TEST_THREADS; i++)
    TEST_EQ(pthread_join(threads[i], NULL), 0);
  TEST_EQ(data.error, false);
  TEST_EQ(data.readers, 0);
  TEST_EQ(data.writers, 0);
  TEST_EQ(data.value, RWLOCK_TEST_ITERATIONS * (RWLOCK_TEST_THREADS / 2));
  TEST_EQ(data.mirror, data.value);
  TEST_EQ(data.lock.count, 0);
  rwlock_clean(&data.lock);
  return 0;
}

TEST_CASE(rwlock_recursive)
{
  TEST_EQ(rwlock_test_child(rwlock_test_recursive), 0);
}
TEST_CASE_END(rwlock_recursive)

TEST_CASE(rwlock_readers)
{
  TEST_EQ(rwlock_test_child(rwlock_test_readers), 0);
}
TEST_CASE_END(rwlock_readers)

TEST_CASE(rwlock_handoff)
{
  TEST_EQ(rwlock_test_child(rwlock_test_handoff), 0);
}
TEST_CASE_END(rwlock_handoff)

TEST_CASE(rwlock_independent)
{
  TEST_EQ(rwlock_test_child(rwlock_test_independent), 0);
}
TEST_CASE_END(rwlock_independent)

TEST_CASE(rwlock_stress)
{
  TEST_EQ(rwlock_test_child(rwlock_test_stress), 0);
}
TEST_CASE_END(rwlock_stress)

#endif

void rwlock_test (void);

void rwlock_test (void)
{
#if HAVE_PTHREAD
  TEST_CASE_RUN(rwlock_recursive);
  TEST_CASE_RUN(rwlock_readers);
  TEST_CASE_RUN(rwlock_handoff);
  TEST_CASE_RUN(rwlock_independent);
  TEST_CASE_RUN(rwlock_stress);
#endif
}
