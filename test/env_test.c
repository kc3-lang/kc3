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
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../libkc3/call.h"
#include "../libkc3/callable.h"
#include "../libkc3/cfn.h"
#include "../libkc3/compare.h"
#include "../libkc3/data.h"
#include "../libkc3/env.h"
#include "../libkc3/env_eval.h"
#include "../libkc3/env_eval_equal.h"
#include "../libkc3/facts.h"
#include "../libkc3/file.h"
#include "../libkc3/frame.h"
#include "../libkc3/list.h"
#include "../libkc3/pointer.h"
#include "../libkc3/securelevel.h"
#include "../libkc3/str.h"
#include "../libkc3/sym.h"
#include "../libkc3/tag.h"
#include "test.h"

#if HAVE_PTHREAD
#include <pthread.h>
#endif

static bool g_env_clean_nested;
static bool g_env_clean_scope_ok;

#if HAVE_PTHREAD
static void * env_clean_thread_test (void *arg)
{
  s_env env = {0};
  bool *ok = arg;
  env_global_set(&env);
  *ok = ! env_cleaning(false);
  return NULL;
}
#endif

static void env_clean_data_test (uw *count)
{
  s_env child;
  s_env *parent = env_global();
#if HAVE_PTHREAD
  bool thread_ok = false;
  pthread_t thread;
#endif
  (*count)++;
  if (! g_env_clean_nested)
    return;
  g_env_clean_scope_ok = env_cleaning(false);
#if HAVE_PTHREAD
  if (pthread_create(&thread, NULL, env_clean_thread_test, &thread_ok) ||
      pthread_join(thread, NULL)) {
    g_env_clean_scope_ok = false;
    return;
  }
  g_env_clean_scope_ok &= thread_ok;
#endif
  if (! env_init(&child, NULL, NULL)) {
    g_env_clean_scope_ok = false;
    return;
  }
  g_env_clean_scope_ok &= ! env_cleaning(false);
  env_clean(&child);
  g_env_clean_scope_ok &= env_global() == parent && env_cleaning(false);
}

static bool env_clean_type_test (s_env *env, const s_sym *module)
{
  s_callable *clean;
  s_list *spec;
  s_tag subject = {0};
  s_tag predicate = {0};
  s_tag type = {0};
  bool ok;
  if (! (spec = list_new_ptuple(2, NULL)))
    return false;
  tag_init_psym(spec->tag.data.td_ptuple->tag, sym_1("count"));
  tag_init_uw(spec->tag.data.td_ptuple->tag + 1, 0);
  if (! tag_init_pstruct_type(&type, module, spec)) {
    list_delete_all(spec);
    return false;
  }
  list_delete_all(spec);
  if (! (clean = callable_new())) {
    tag_clean(&type);
    return false;
  }
  clean->type = CALLABLE_CFN;
  cfn_init(&clean->data.cfn, sym_1("env_clean_data_test"),
            list_new_psym(sym_1("Uw*"), NULL), &g_sym_Void);
  clean->data.cfn.ptr.p_f = (void (*)(void)) env_clean_data_test;
  if (! cfn_prep_cif(&clean->data.cfn)) {
    callable_delete(clean);
    tag_clean(&type);
    return false;
  }
  type.data.td_pstruct_type->clean = clean;
  tag_init_psym(&subject, module);
  tag_init_psym(&predicate, &g_sym_struct_type);
  ok = facts_replace_tags(env->facts, &subject, &predicate, &type) != NULL;
  tag_clean(&type);
  return ok;
}

static char env_clean_nested_test (void)
{
  s_env child;
  s_env grandchild;
  s_env *parent = env_global();
  uw i;
  for (i = 0; i < 3; i++) {
    TEST_EQ(env_init(&child, NULL, NULL), &child);
    TEST_EQ(env_init(&grandchild, NULL, NULL), &grandchild);
    env_clean(&grandchild);
    TEST_EQ(env_global(), &child);
    TEST_EQ(env_cleaning(false), false);
    env_clean(&child);
    TEST_EQ(env_global(), parent);
    TEST_EQ(env_cleaning(false), false);
  }
  return 0;
}

static char env_clean_destructor_test (void)
{
  s_env child;
  s_env parent;
  const s_sym *module = sym_1("EnvCleanTest");
  uw count = 0;
  TEST_EQ(env_init(&parent, NULL, NULL), &parent);
  TEST_ASSERT(env_clean_type_test(&parent, module));
  TEST_EQ(env_init(&child, NULL, NULL), &child);
  env_clean(&child);
  TEST_EQ(env_cleaning(false), false);
  TEST_ASSERT(data_clean(module, &count));
  TEST_EQ(count, 1);
  env_clean(&parent);
  return 0;
}

static char env_clean_scope_test (void)
{
  uw count = 0;
  s_env env;
  s_env *parent = env_global();
  const s_sym *module = sym_1("EnvCleanTest");
  s_tag value = {0};
  TEST_EQ(env_init(&env, NULL, NULL), &env);
  TEST_ASSERT(env_clean_type_test(&env, module));
  TEST_ASSERT(tag_init_pstruct_with_data(&value, module, &count, false));
  TEST_ASSERT(value.data.td_pstruct->pstruct_type->clean);
  TEST_ASSERT(frame_binding_new(&env.toplevel_frame,
                                sym_1("clean_test"), &value));
  tag_clean(&value);
  g_env_clean_nested = true;
  env_clean(&env);
  TEST_EQ(count, 1);
  TEST_ASSERT(g_env_clean_scope_ok);
  TEST_EQ(env_global(), parent);
  TEST_EQ(env_cleaning(false), false);
  return 0;
}

static char env_clean_securelevel_test (uw action)
{
  s_env child;
  s_env parent;
  s_fact *fact;
  bool removed = false;
  s_tag subject = {0};
  s_tag predicate = {0};
  s_tag object = {0};
  TEST_EQ(env_init(&parent, NULL, NULL), &parent);
  tag_init_psym(&subject, sym_1("env_clean_test"));
  tag_init_psym(&predicate, sym_1("value"));
  tag_init_uw(&object, 1);
  TEST_ASSERT((fact = facts_add_tags(parent.facts, &subject,
                                     &predicate, &object)));
  TEST_EQ(env_init(&child, NULL, NULL), &child);
  env_clean(&child);
  securelevel(2);
  switch (action) {
  case 0: env_clean(&parent); break;
  case 1: facts_remove_all(parent.facts); break;
  case 2: facts_remove_fact(parent.facts, fact, &removed); break;
  case 3: facts_remove_fact_tags(parent.facts, &subject, &predicate,
                                 &object, &removed); break;
  }
  return 0;
}

static int env_clean_child_test (char (*test)(void), uw action)
{
  struct rlimit limit = {0, 0};
  pid_t child;
  pid_t waited;
  int status;
  child = fork();
  if (child < 0)
    return -1;
  if (! child) {
    alarm(30);
    if (setrlimit(RLIMIT_CORE, &limit))
      _exit(1);
    _exit(test ? test() : env_clean_securelevel_test(action));
  }
  do {
    waited = waitpid(child, &status, 0);
  } while (waited < 0 && errno == EINTR);
  if (waited != child)
    return -1;
  if (! test && action)
    return WIFSIGNALED(status) && WTERMSIG(status) == SIGABRT ? 0 : -1;
  return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}

void env_test (void);
TEST_CASE_PROTOTYPE(env_clean_nested);
TEST_CASE_PROTOTYPE(env_clean_destructor);
TEST_CASE_PROTOTYPE(env_clean_scope);
TEST_CASE_PROTOTYPE(env_clean_securelevel);
TEST_CASE_PROTOTYPE(env_eval_call);
TEST_CASE_PROTOTYPE(env_eval_equal_tag);
TEST_CASE_PROTOTYPE(env_eval_ident_qualified);
TEST_CASE_PROTOTYPE(env_eval_ident_callable);
TEST_CASE_PROTOTYPE(env_eval_ident_is_bound);
TEST_CASE_PROTOTYPE(env_eval_tag);
TEST_CASE_PROTOTYPE(env_init_clean);
TEST_CASE_PROTOTYPE(env_module_load);
TEST_CASE_PROTOTYPE(env_facts_transaction_error);
TEST_CASE_PROTOTYPE(env_facts_transaction_unwind);
TEST_CASE_PROTOTYPE(env_module_load_rollback);
TEST_CASE_PROTOTYPE(env_pointer_id);
TEST_CASE_PROTOTYPE(env_dump);
TEST_CASE_PROTOTYPE(env_dump_restore);

void env_test (void)
{
  TEST_CASE_RUN(env_clean_nested);
  TEST_CASE_RUN(env_clean_destructor);
  TEST_CASE_RUN(env_clean_scope);
  TEST_CASE_RUN(env_clean_securelevel);
  TEST_CASE_RUN(env_init_clean);
  TEST_CASE_RUN(env_eval_equal_tag);
  TEST_CASE_RUN(env_eval_call);
  TEST_CASE_RUN(env_eval_ident_qualified);
  TEST_CASE_RUN(env_eval_ident_callable);
  TEST_CASE_RUN(env_eval_ident_is_bound);
  TEST_CASE_RUN(env_eval_tag);
  TEST_CASE_RUN(env_module_load);
  TEST_CASE_RUN(env_facts_transaction_error);
  TEST_CASE_RUN(env_facts_transaction_unwind);
  TEST_CASE_RUN(env_module_load_rollback);
  TEST_CASE_RUN(env_pointer_id);
  TEST_CASE_RUN(env_dump);
  TEST_CASE_RUN(env_dump_restore);
}

TEST_CASE(env_clean_nested)
{
  TEST_EQ(env_clean_child_test(env_clean_nested_test, 0), 0);
}
TEST_CASE_END(env_clean_nested)

TEST_CASE(env_clean_destructor)
{
  TEST_EQ(env_clean_child_test(env_clean_destructor_test, 0), 0);
}
TEST_CASE_END(env_clean_destructor)

TEST_CASE(env_clean_scope)
{
  TEST_EQ(env_clean_child_test(env_clean_scope_test, 0), 0);
}
TEST_CASE_END(env_clean_scope)

TEST_CASE(env_clean_securelevel)
{
  uw action;
  for (action = 0; action < 4; action++)
    TEST_EQ(env_clean_child_test(NULL, action), 0);
}
TEST_CASE_END(env_clean_securelevel)

TEST_CASE(env_dump)
{
  s_env env = {0};
  const s_str path = STR("env_test_dump.1.dump");
  test_context("env_dump()");
  TEST_ASSERT(env_init(&env, 0, NULL));
  TEST_ASSERT(env_dump(&env, &path) > 0);
  env_clean(&env);
  file_unlink(&path);
  test_context(NULL);
}
TEST_CASE_END(env_dump)

TEST_CASE(env_pointer_id)
{
  s_pointer address = {0};
  s_pointer cast = {0};
  s_pointer tag_pointer = {0};
  p_sym type = &g_sym_Pointer;
  s_tag value = {0};
  tag_init_uw(&value, 1);
  TEST_EQ(pointer_init_tag(&tag_pointer, &value), &tag_pointer);
  TEST_EQ(pointer_init_cast(&cast, &type, &value), &cast);
  TEST_EQ(env_address_of(env_global(), &value, &address), &address);
  TEST_ASSERT(tag_pointer.id);
  TEST_ASSERT(cast.id);
  TEST_ASSERT(address.id);
  TEST_ASSERT(tag_pointer.id != cast.id);
  TEST_ASSERT(tag_pointer.id != address.id);
  TEST_ASSERT(cast.id != address.id);
  tag_clean(&value);
}
TEST_CASE_END(env_pointer_id)

TEST_CASE(env_dump_restore)
{
  s_env env = {0};
  const s_str path = STR("kc3.dump");
  sw size;
  test_context("env_dump() + env_restore()");
  TEST_ASSERT(env_init(&env, 0, NULL));
  size = env_dump(&env, &path);
  TEST_ASSERT(size > 0);
  env_clean(&env);
  TEST_ASSERT(env_init(&env, 0, NULL));
  TEST_EQ(env_dump(&env, &path), size);
  env_clean(&env);
  TEST_ASSERT(env_init(&env, 0, NULL));
  TEST_EQ(env_dump(&env, &path), size);
  env_clean(&env);
  test_context(NULL);
}
TEST_CASE_END(env_dump)

TEST_CASE(env_eval_call)
{
  s_env env = {0};
  s_call call;
  s_tag result = {0};
  env_init(&env, 0, NULL);
  test_context("env_eval_call(1 + 2) -> 3");
  call_init(&call);
  call.ident.module = NULL;
  call.ident.sym = sym_1("+");
  call.arguments = list_new_1("[1, 2]");
  TEST_ASSERT(env_eval_call(&env, &call, &result));
  TEST_EQ(result.type, TAG_U8);
  TEST_EQ(result.data.td_u8, 3);
  call_clean(&call);
  tag_clean(&result);
  env_clean(&env);
  test_context(NULL);
}
TEST_CASE_END(env_eval_call)

TEST_CASE(env_eval_equal_tag)
{
  s_env env;
  s_frame frame;
  s_tag x = {0};
  s_tag y = {0};
  s_tag z = {0};
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame);
  test_context("x = 1");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_init_1(&x, "x"),
                                 tag_init_1(&y, "1"),
                                 &z));
  TEST_ASSERT(frame_get(&frame, x.data.td_ident.sym));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame);
  test_context("x = (1, 2]");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_init_1(&x, "x"),
                                 tag_init_1(&y, "[1, 2]"),
                                 &z));
  TEST_ASSERT(frame_get(&frame, sym_1("x")));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame);
  test_context("[] = []");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_1(&x, "[]"),
                                 tag_1(&y, "[]"),
                                 &z));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame);
  test_context("[a, b] = [1, 2]");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_1(&x, "[a, b]"),
                                 tag_1(&y, "[1, 2]"),
                                 &z));
  TEST_ASSERT(frame_get(&frame, sym_1("a")));
  TEST_ASSERT(frame_get(&frame, sym_1("b")));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  env_init(&env, 0, NULL);
  env.frame = frame_init(&frame, env.frame);
  test_context("x = [1, 2]");
  TEST_ASSERT(env_eval_equal_tag(&env, false,
                                 tag_1(&x, "[a | b]"),
                                 tag_1(&y, "[1, 2]"),
                                 &z));
  TEST_ASSERT(frame_get(&frame, sym_1("a")));
  TEST_ASSERT(frame_get(&frame, sym_1("b")));
  TEST_EQ(compare_tag(&z, &y), 0);
  tag_clean(&z);
  env.frame = frame_clean(&frame);
  env_clean(&env);
  tag_clean(&x);
  tag_clean(&y);
  test_context(NULL);
}
TEST_CASE_END(env_eval_equal_tag)

TEST_CASE(env_eval_ident_qualified)
{
  uw copy;
  uw scope;
  bool copy_saved;
  s_env env;
  s_frame outer;
  s_frame inner;
  s_frame *frames[3];
  s_ident qualified = {0};
  s_ident unqualified = {0};
  s_tag value = {0};
  s_tag shadow = {0};
  s_tag result = {0};
  qualified.module = sym_1("EnvIdentTest");
  qualified.sym = sym_1("qualified_value_test");
  unqualified.sym = qualified.sym;
  for (copy = 0; copy < 2; copy++) {
    TEST_ASSERT(env_init(&env, 0, NULL));
    copy_saved = env.pass_by_copy;
    env.pass_by_copy = copy;
    TEST_ASSERT(tag_init_u8(&value, 1));
    TEST_ASSERT(env_def(&env, &qualified, &value));
    TEST_ASSERT(frame_init(&outer, env.frame));
    TEST_ASSERT(frame_init(&inner, &outer));
    env.frame = &inner;
    frames[0] = env.global_frame;
    frames[1] = &outer;
    frames[2] = &inner;
    for (scope = 0; scope < 3; scope++) {
      TEST_ASSERT(tag_init_u8(&shadow, scope + 2));
      TEST_ASSERT(frame_binding_new(frames[scope], qualified.sym,
                                     &shadow));
      TEST_ASSERT(env_eval_ident(&env, &qualified, &result));
      TEST_EQ(compare_tag(&result, &value), 0);
      tag_clean(&result);
      TEST_ASSERT(env_eval_ident(&env, &unqualified, &result));
      TEST_EQ(compare_tag(&result, &shadow), 0);
      tag_clean(&result);
      TEST_ASSERT(env_eval_ident_is_bound(&env, &qualified));
      TEST_ASSERT(env_eval_ident_is_bound(&env, &unqualified));
      tag_clean(&shadow);
    }
    env.frame = frame_clean(&inner);
    env.frame = frame_clean(&outer);
    TEST_ASSERT(frame_binding_delete(env.global_frame, qualified.sym));
    env.current_defmodule = qualified.module;
    TEST_ASSERT(env_eval_ident(&env, &unqualified, &result));
    TEST_EQ(compare_tag(&result, &value), 0);
    TEST_ASSERT(env_eval_ident_is_bound(&env, &unqualified));
    tag_clean(&result);
    tag_clean(&value);
    env.pass_by_copy = copy_saved;
    env_clean(&env);
  }
}
TEST_CASE_END(env_eval_ident_qualified)

TEST_CASE(env_eval_ident_callable)
{
  uw scope;
  s_env env;
  s_ident ident = {0};
  s_tag value = {0};
  s_tag shadow = {0};
  s_tag expr = {0};
  s_tag function = {0};
  s_tag result = {0};
  s_call call = {0};
  for (scope = 0; scope < 2; scope++) {
    TEST_ASSERT(env_init(&env, 0, NULL));
    TEST_ASSERT(tag_init_1(&expr, "EnvIdentTest.qualified_fn_test"));
    TEST_EQ(expr.type, TAG_IDENT);
    ident = expr.data.td_ident;
    TEST_ASSERT(tag_init_1(&value, "fn () { 42 }"));
    TEST_ASSERT(env_def(&env, &ident, &value));
    TEST_ASSERT(tag_init_1(&shadow, "fn () { 99 }"));
    TEST_ASSERT(frame_binding_new(scope ? env.frame : env.global_frame,
                                   ident.sym, &shadow));
    TEST_ASSERT(env_eval_tag(&env, &expr, &function));
    TEST_EQ(function.type, TAG_PCALLABLE);
    TEST_ASSERT(env_eval_call_callable_args
                (&env, function.data.td_pcallable, NULL, &result));
    TEST_EQ(result.type, TAG_U8);
    TEST_EQ(result.data.td_u8, 42);
    tag_clean(&result);
    tag_clean(&function);
    TEST_ASSERT(call_init(&call));
    call.ident = ident;
    TEST_ASSERT(env_eval_call(&env, &call, &result));
    TEST_EQ(result.type, TAG_U8);
    TEST_EQ(result.data.td_u8, 42);
    tag_clean(&result);
    call_clean(&call);
    ident.module = NULL;
    TEST_ASSERT(env_eval_ident(&env, &ident, &function));
    TEST_EQ(function.type, TAG_PCALLABLE);
    TEST_ASSERT(env_eval_call_callable_args
                (&env, function.data.td_pcallable, NULL, &result));
    TEST_EQ(result.type, TAG_U8);
    TEST_EQ(result.data.td_u8, 99);
    tag_clean(&result);
    tag_clean(&function);
    tag_clean(&expr);
    tag_clean(&shadow);
    tag_clean(&value);
    env_clean(&env);
  }
}
TEST_CASE_END(env_eval_ident_callable)

TEST_CASE(env_eval_ident_is_bound)
{
  uw scope;
  s_env env;
  s_ident ident = {0};
  s_tag value = {0};
  s_tag result = {0};
  for (scope = 0; scope < 2; scope++) {
    TEST_ASSERT(env_init(&env, 0, NULL));
    ident.module = NULL;
    ident.sym = sym_1("qualified_missing_test");
    TEST_ASSERT(tag_init_u8(&value, 2));
    TEST_ASSERT(frame_binding_new(scope ? env.frame : env.global_frame,
                                   ident.sym, &value));
    TEST_ASSERT(env_eval_ident_is_bound(&env, &ident));
    ident.module = &g_sym_KC3;
    TEST_ASSERT(! env_eval_ident_is_bound(&env, &ident));
    TEST_ASSERT(tag_init_u8(&result, 123));
    TEST_ASSERT(! env_eval_ident(&env, &ident, &result));
    TEST_EQ(result.type, TAG_U8);
    TEST_EQ(result.data.td_u8, 123);
    ident.module = sym_1("EnvIdentMissingModule");
    TEST_ASSERT(! env_eval_ident_is_bound(&env, &ident));
    TEST_ASSERT(! env_eval_ident(&env, &ident, &result));
    TEST_EQ(result.type, TAG_U8);
    TEST_EQ(result.data.td_u8, 123);
    ident.module = NULL;
    ident.sym = sym_1("qualified_absent_test");
    TEST_ASSERT(! env_eval_ident_is_bound(&env, &ident));
    tag_clean(&result);
    tag_clean(&value);
    env_clean(&env);
  }
}
TEST_CASE_END(env_eval_ident_is_bound)

TEST_CASE(env_eval_tag)
{
  s_env env;
  s_tag x = {0};
  s_tag y = {0};
  s_tag expected = {0};
  env_init(&env, 0, NULL);
  test_context("env_eval_tag(1 + 2) -> 3");
  TEST_EQ(tag_init_1(&x, "1 + 2"), &x);
  TEST_EQ(tag_init_1(&expected, "3"), &expected);
  TEST_ASSERT(env_eval_tag(&env, &x, &y));
  TEST_EQ(compare_tag(&y, &expected), 0);
  tag_clean(&x);
  tag_clean(&y);
  tag_clean(&expected);
  env_clean(&env);
  test_context(NULL);
}
TEST_CASE_END(env_eval_tag)

TEST_CASE(env_init_clean)
{
  s_env env;
  TEST_ASSERT(env_init(&env, 0, NULL));
  env_clean(&env);
}
TEST_CASE_END(env_init_clean)

TEST_CASE(env_module_load)
{
  s_env env;
  env_init(&env, 0, NULL);
  TEST_ASSERT(env_module_load(&env, sym_1("KC3.Op")));
  env_clean(&env);
}
TEST_CASE_END(env_module_load)

TEST_CASE(env_facts_transaction_error)
{
  s_tag body = {0};
  s_env env;
  s_tag facts_arg = {0};
  s_fact *found;
  s_tag object = {0};
  s_tag predicate = {0};
  s_tag result = {0};
  s_tag subject = {0};
  TEST_ASSERT(env_init(&env, 0, NULL));
  TEST_ASSERT(env_module_load(&env, sym_1("Facts")));
  TEST_ASSERT(tag_init_1(&facts_arg, "Facts.env_db()"));
  TEST_ASSERT(tag_init_1(&subject, ":f12_transaction"));
  TEST_ASSERT(tag_init_1(&predicate, ":value"));
  TEST_ASSERT(tag_init_1(&object, "1"));
  TEST_ASSERT(tag_init_1(&body,
    "do Facts.with_transaction(Facts.env_db(), do "
    "Facts.add_tags(Facts.env_db(), :f12_transaction, :value, 1) "
    "end); KC3.f12_unbound_transaction_test end"));
  TEST_ASSERT(tag_init_u8(&result, 123));
  TEST_EQ(env_facts_with_transaction(&env, &facts_arg, &body, &result),
          NULL);
  TEST_EQ(result.type, TAG_U8);
  TEST_EQ(result.data.td_u8, 123);
  TEST_EQ(env.facts->transaction, NULL);
  TEST_ASSERT(facts_find_fact_by_tags(env.facts, &subject, &predicate,
                                     &object, &found));
  TEST_EQ(found, NULL);
  tag_clean(&body);
  TEST_ASSERT(tag_init_1(&body,
    "do Facts.add_tags(Facts.env_db(), :f12_transaction, :value, 1); "
    "false end"));
  TEST_EQ(env_facts_with_transaction(&env, &facts_arg, &body, &result),
          &result);
  TEST_EQ(result.type, TAG_BOOL);
  TEST_EQ(result.data.td_bool_, false);
  TEST_ASSERT(facts_find_fact_by_tags(env.facts, &subject, &predicate,
                                     &object, &found));
  TEST_ASSERT(found);
  tag_clean(&body);
  tag_clean(&facts_arg);
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
  tag_clean(&result);
  env_clean(&env);
}
TEST_CASE_END(env_facts_transaction_error)

TEST_CASE(env_facts_transaction_unwind)
{
  s_tag body = {0};
  s_env env;
  s_fact *found;
  s_tag object = {0};
  s_tag predicate = {0};
  s_tag result = {0};
  s_tag subject = {0};
  TEST_ASSERT(env_init(&env, 0, NULL));
  TEST_ASSERT(env_module_load(&env, sym_1("Facts")));
  TEST_ASSERT(tag_init_1(&subject, ":f12_unwind"));
  TEST_ASSERT(tag_init_1(&predicate, ":value"));
  TEST_ASSERT(tag_init_1(&object, "1"));
  TEST_ASSERT(tag_init_1(&body,
    "block :f12_exit do "
    "Facts.with_transaction(Facts.env_db(), do "
    "Facts.add_tags(Facts.env_db(), :f12_unwind, :value, 1); "
    "return_from :f12_exit 42 end) end"));
  TEST_ASSERT(env_eval_tag(&env, &body, &result));
  TEST_EQ(result.type, TAG_U8);
  TEST_EQ(result.data.td_u8, 42);
  TEST_EQ(env.facts->transaction, NULL);
  TEST_EQ(env.unwind_protect, NULL);
  TEST_ASSERT(facts_find_fact_by_tags(env.facts, &subject, &predicate,
                                     &object, &found));
  TEST_EQ(found, NULL);
  tag_clean(&body);
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
  tag_clean(&result);
  env_clean(&env);
}
TEST_CASE_END(env_facts_transaction_unwind)

TEST_CASE(env_module_load_rollback)
{
  s_str failed = STR("facts_transaction/fail/");
  s_str success = STR("facts_transaction/success/");
  const s_str cache =
    STR("facts_transaction/success/f12rollback.kc3c");
  s_env env;
  s_fact *found;
  bool loading;
  uw n;
  s_tag object = {0};
  s_tag predicate = {0};
  s_tag subject = {0};
  TEST_ASSERT(env_init(&env, 0, NULL));
  TEST_ASSERT(env_module_load(&env, sym_1("Facts")));
  TEST_ASSERT(tag_init_1(&subject, ":f12_module"));
  TEST_ASSERT(tag_init_1(&predicate, ":partial"));
  TEST_ASSERT(tag_init_1(&object, "1"));
  str_clean(env.module_path);
  TEST_ASSERT(str_init_copy(env.module_path, &failed));
  for (n = 0; n < 2; n++) {
    TEST_EQ(env_module_load(&env, sym_1("F12Rollback")), false);
    TEST_EQ(env.facts->transaction, NULL);
    TEST_ASSERT(env_module_is_loading(&env, sym_1("F12Rollback"),
                                      &loading));
    TEST_EQ(loading, false);
    TEST_ASSERT(facts_find_fact_by_tags(env.facts, &subject, &predicate,
                                       &object, &found));
    TEST_EQ(found, NULL);
  }
  str_clean(env.module_path);
  TEST_ASSERT(str_init_copy(env.module_path, &success));
  TEST_ASSERT(env_module_load(&env, sym_1("F12Rollback")));
  TEST_ASSERT(env_module_is_loading(&env, sym_1("F12Rollback"),
                                    &loading));
  TEST_EQ(loading, false);
  TEST_ASSERT(facts_find_fact_by_tags(env.facts, &subject, &predicate,
                                     &object, &found));
  TEST_ASSERT(found);
  tag_clean(&subject);
  tag_clean(&predicate);
  tag_clean(&object);
  env_clean(&env);
  file_unlink(&cache);
}
TEST_CASE_END(env_module_load_rollback)
