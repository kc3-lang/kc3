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
#include "../libkc3/env.h"
#include "../libkc3/kc3_main.h"
#include "../libkc3/module.h"
#include "../libkc3/str.h"
#include "../libkc3/struct.h"
#include "../libkc3/sym.h"
#include "../http/types.h"
#include "test.h"

#define STRUCT_TEST_OFFSETOF(type, module, key)                        \
  do {                                                                 \
    const s_sym *sym_module;                                           \
    const s_sym *sym_key;                                              \
    uw offset;                                                         \
    test_context("KC3.offsetof(" # module ", " # key ") = offsetof("   \
                 # type ", " # key ")");                               \
    sym_module = sym_1(# module);                                      \
    sym_key = sym_1(# key);                                            \
    TEST_EQ(kc3_offsetof(&sym_module, &sym_key, &offset), &offset);    \
    TEST_EQ(offset, offsetof(type, key));                              \
    test_context(NULL);                                                \
  } while (0)

#define STRUCT_TEST_SIZEOF(type, module)                               \
  do {                                                                 \
    uw size;                                                           \
    const s_sym *sym;                                                  \
    test_context("sym_type_size(" # module ") = sizeof(" # type ")");  \
    sym = sym_1(# module);                                             \
    TEST_EQ(sym_type_size(sym, &size), &size);                        \
    TEST_EQ(size, sizeof(type));                                       \
    test_context(NULL);                                                \
  } while (0)

TEST_CASE_PROTOTYPE(struct_test_fact_w);
TEST_CASE_PROTOTYPE(struct_test_http_request);
TEST_CASE_PROTOTYPE(struct_test_http_response);
TEST_CASE_PROTOTYPE(struct_test_time);

void struct_test (void)
{
  const s_str path = STR("struct_test.kc3");
  env_load(env_global(), &path);
  /*
  TEST_CASE_RUN(struct_test_fact_w);
  TEST_CASE_RUN(struct_test_time);
  TEST_CASE_RUN(struct_test_http_request);
  TEST_CASE_RUN(struct_test_http_response);
  */
}

TEST_CASE(struct_test_fact_w)
{
  STRUCT_TEST_OFFSETOF(s_fact_w, FactW, subject);
  STRUCT_TEST_OFFSETOF(s_fact_w, FactW, predicate);
  STRUCT_TEST_OFFSETOF(s_fact_w, FactW, object);
  STRUCT_TEST_OFFSETOF(s_fact_w, FactW, id);
  STRUCT_TEST_SIZEOF(  s_fact_w, FactW);
}
TEST_CASE_END(struct_test_fact_w)

TEST_CASE(struct_test_http_request)
{
  STRUCT_TEST_OFFSETOF(s_http_request, HTTP.Request, time);
  STRUCT_TEST_OFFSETOF(s_http_request, HTTP.Request, method);
  STRUCT_TEST_OFFSETOF(s_http_request, HTTP.Request, body);
  STRUCT_TEST_OFFSETOF(s_http_request, HTTP.Request, url);
  STRUCT_TEST_OFFSETOF(s_http_request, HTTP.Request, protocol);
  STRUCT_TEST_OFFSETOF(s_http_request, HTTP.Request, headers);
  STRUCT_TEST_SIZEOF(  s_http_request, HTTP.Request);
}
TEST_CASE_END(struct_test_http_request)

TEST_CASE(struct_test_http_response)
{
  STRUCT_TEST_OFFSETOF(s_http_response, HTTP.Response, protocol);
  STRUCT_TEST_OFFSETOF(s_http_response, HTTP.Response, code);
  STRUCT_TEST_OFFSETOF(s_http_response, HTTP.Response, message);
  STRUCT_TEST_OFFSETOF(s_http_response, HTTP.Response, headers);
  STRUCT_TEST_OFFSETOF(s_http_response, HTTP.Response, body);
  STRUCT_TEST_SIZEOF(  s_http_response, HTTP.Response);
}
TEST_CASE_END(struct_test_http_response)

TEST_CASE(struct_test_time)
{
  STRUCT_TEST_OFFSETOF(s_time, Time, tv_sec);
  STRUCT_TEST_OFFSETOF(s_time, Time, tv_nsec);
  STRUCT_TEST_OFFSETOF(s_time, Time, tag);
  STRUCT_TEST_SIZEOF(  s_time, Time);
}
TEST_CASE_END(struct_test_time)
