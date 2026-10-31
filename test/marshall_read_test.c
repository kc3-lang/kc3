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
#include "../libkc3/array.h"
#include "../libkc3/buf.h"
#include "../libkc3/call.h"
#include "../libkc3/endian.h"
#include "../libkc3/env.h"
#include "../libkc3/env_eval.h"
#include "../libkc3/fact.h"
#include "../libkc3/file.h"
#include "../libkc3/ht.h"
#include "../libkc3/inspect.h"
#include "../libkc3/map.h"
#include "../libkc3/marshall.h"
#include "../libkc3/marshall_read.h"
#include "../libkc3/mutex.h"
#include "../libkc3/pointer.h"
#include "../libkc3/facts.h"
#include "../libkc3/set__tag.h"
#include "../libkc3/str.h"
#include "../libkc3/struct.h"
#include "../libkc3/struct_type.h"
#include "../libkc3/sym.h"
#include "../libkc3/list.h"
#include "../libkc3/tag.h"
#include "../libkc3/tag_init.h"
#include "../libkc3/tuple.h"
#include "../libkc3/var.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "test.h"
#include "tag_test.h"

#define MARSHALL_READ_TEST(type, test, expected)                        \
  do {                                                                  \
    type tmp = {0};                                                     \
    s_str tmp_str = {{0}, sizeof(expected) - 1, {expected}};            \
    s_marshall_read mr = {0};                                           \
    s_str test_str = {0};                                               \
    test_context("marshall_read_" # type "(" # test ") -> "             \
                 # expected);                                           \
    TEST_EQ(marshall_read_init_1(&mr, (test), sizeof(test) - 1), &mr);  \
    TEST_EQ(marshall_read_## type (&mr, false, &tmp), &mr);             \
    marshall_read_clean(&mr);                                           \
    inspect_ ## type(_Generic(tmp,                                      \
                              f32:  tmp,                                \
                              f64:  tmp,                                \
                              s8:   tmp,                                \
                              s16:  tmp,                                \
                              s32:  tmp,                                \
                              s64:  tmp,                                \
                              u8:   tmp,                                \
                              u16:  tmp,                                \
                              u32:  tmp,                                \
                              u64:  tmp,                                \
                              default: &tmp), &test_str);               \
    TEST_STR_EQ(test_str, tmp_str);                                     \
    str_clean(&test_str);                                               \
    str_clean(&tmp_str);                                                \
    test_context(NULL);                                                 \
  } while (0)

#define MARSHALL_READ_TEST_TAG(test, expected)                          \
  do {                                                                  \
    s_marshall_read mr = {0};                                           \
    s_tag tag = {0};                                                    \
    s_tag tag_expected = {0};                                           \
    test_context("marshall_read_tag(" # test ") -> " # expected);       \
    TEST_ASSERT(tag_init_1(&tag_expected, (expected)));                 \
    TEST_EQ(marshall_read_init_1(&mr, (test), sizeof(test) - 1), &mr);  \
    TEST_EQ(marshall_read_tag(&mr, false, &tag), &mr);                  \
    TAG_TEST_EQ(&tag, &tag_expected);                                   \
    marshall_read_clean(&mr);                                           \
    tag_clean(&tag);                                                    \
    tag_clean(&tag_expected);                                           \
    test_context(NULL);                                                 \
  } while (0)

void marshal_test (void);

TEST_CASE_PROTOTYPE(marshall_read_array);
TEST_CASE_PROTOTYPE(marshall_read_array_empty);
TEST_CASE_PROTOTYPE(marshall_read_bool);
TEST_CASE_PROTOTYPE(marshall_read_call_cache);
TEST_CASE_PROTOTYPE(marshall_read_cache_types);
TEST_CASE_PROTOTYPE(marshall_read_cache_state);
TEST_CASE_PROTOTYPE(marshall_read_cache_offsets);
TEST_CASE_PROTOTYPE(marshall_read_cache_cycle);
TEST_CASE_PROTOTYPE(marshall_read_cache_skiplist_cycle);
TEST_CASE_PROTOTYPE(marshall_read_cache_shared);
TEST_CASE_PROTOTYPE(marshall_read_cache_pointer);
TEST_CASE_PROTOTYPE(marshall_read_cache_pointer_legacy);
TEST_CASE_PROTOTYPE(marshall_read_character);
TEST_CASE_PROTOTYPE(marshall_read_chunks);
TEST_CASE_PROTOTYPE(marshall_read_container_limits);
TEST_CASE_PROTOTYPE(marshall_read_container_counts);
TEST_CASE_PROTOTYPE(marshall_read_header_limits);
TEST_CASE_PROTOTYPE(marshall_read_struct_layout);
TEST_CASE_PROTOTYPE(marshall_read_struct_roundtrip);
TEST_CASE_PROTOTYPE(marshall_read_struct_destination);
TEST_CASE_PROTOTYPE(marshall_read_struct_nested);
TEST_CASE_PROTOTYPE(marshall_read_struct_legacy_limits);
TEST_CASE_PROTOTYPE(marshall_read_facts);
TEST_CASE_PROTOTYPE(marshall_read_facts_log);
TEST_CASE_PROTOTYPE(marshall_read_init_buf);
TEST_CASE_PROTOTYPE(marshall_read_init_file);
TEST_CASE_PROTOTYPE(marshall_read_init_str);
TEST_CASE_PROTOTYPE(marshall_read_plist);
TEST_CASE_PROTOTYPE(marshall_read_s8);
TEST_CASE_PROTOTYPE(marshall_read_s16);
TEST_CASE_PROTOTYPE(marshall_read_s32);
TEST_CASE_PROTOTYPE(marshall_read_s64);
TEST_CASE_PROTOTYPE(marshall_read_set__tag);
#ifdef NDEBUG
TEST_CASE_PROTOTYPE(marshall_read_array_layout);
TEST_CASE_PROTOTYPE(marshall_read_array_size);
TEST_CASE_PROTOTYPE(marshall_read_set_max_overflow);
TEST_CASE_PROTOTYPE(marshall_read_set_item_error);
TEST_CASE_PROTOTYPE(marshall_read_skiplist_max_height);
#endif
TEST_CASE_PROTOTYPE(marshall_read_sw);
TEST_CASE_PROTOTYPE(marshall_read_tag);
TEST_CASE_PROTOTYPE(marshall_read_unquote);
TEST_CASE_PROTOTYPE(marshall_read_var);
TEST_CASE_PROTOTYPE(marshall_read_u8);
TEST_CASE_PROTOTYPE(marshall_read_u16);
TEST_CASE_PROTOTYPE(marshall_read_u32);
TEST_CASE_PROTOTYPE(marshall_read_u64);
TEST_CASE_PROTOTYPE(marshall_read_uw);

void marshall_read_test (void)
{
  TEST_CASE_RUN(marshall_read_array);
  TEST_CASE_RUN(marshall_read_array_empty);
  TEST_CASE_RUN(marshall_read_bool);
  TEST_CASE_RUN(marshall_read_call_cache);
  TEST_CASE_RUN(marshall_read_cache_types);
  TEST_CASE_RUN(marshall_read_cache_state);
  TEST_CASE_RUN(marshall_read_cache_offsets);
  TEST_CASE_RUN(marshall_read_cache_cycle);
  TEST_CASE_RUN(marshall_read_cache_skiplist_cycle);
  TEST_CASE_RUN(marshall_read_cache_shared);
  TEST_CASE_RUN(marshall_read_cache_pointer);
  TEST_CASE_RUN(marshall_read_cache_pointer_legacy);
  TEST_CASE_RUN(marshall_read_chunks);
  TEST_CASE_RUN(marshall_read_container_limits);
  TEST_CASE_RUN(marshall_read_container_counts);
  TEST_CASE_RUN(marshall_read_header_limits);
  TEST_CASE_RUN(marshall_read_struct_layout);
  TEST_CASE_RUN(marshall_read_struct_roundtrip);
  TEST_CASE_RUN(marshall_read_struct_destination);
  TEST_CASE_RUN(marshall_read_struct_nested);
  TEST_CASE_RUN(marshall_read_struct_legacy_limits);
  TEST_CASE_RUN(marshall_read_facts);
  TEST_CASE_RUN(marshall_read_facts_log);
  TEST_CASE_RUN(marshall_read_tag);
  TEST_CASE_RUN(marshall_read_unquote);
  TEST_CASE_RUN(marshall_read_set__tag);
  TEST_CASE_RUN(marshall_read_var);
#ifdef NDEBUG
  TEST_CASE_RUN(marshall_read_array_layout);
  TEST_CASE_RUN(marshall_read_array_size);
  TEST_CASE_RUN(marshall_read_set_max_overflow);
  TEST_CASE_RUN(marshall_read_set_item_error);
  TEST_CASE_RUN(marshall_read_skiplist_max_height);
#endif
}

static s_marshall * marshall_test_struct_type (s_marshall *m,
                                               s_struct_type *st,
                                               bool legacy)
{
  uw i;
  if (! marshall_1(m, false, legacy ? "_KC3STRUCTTYPE_" :
                    "_KC3STRUCTTYPE1_") ||
      ! marshall_psym(m, false, &st->module) ||
      ! marshall_map(m, false, &st->map))
    return NULL;
  if (! legacy) {
    for (i = 0; i < st->map.count; i++)
      if (! marshall_uw(m, false, st->offset[i]))
        return NULL;
    if (! marshall_u8(m, false, st->align_max) ||
        ! marshall_uw(m, false, st->size))
      return NULL;
  }
  if (! marshall_pcallable(m, false, &st->clean) ||
      (! legacy && ! marshall_bool(m, false, st->must_clean)))
    return NULL;
  return m;
}

TEST_CASE(marshall_read_struct_layout)
{
  uw i;
  s_marshall m;
  s_marshall_read mr;
  uw offsets[2];
  s_struct_type src;
  s_struct_type dest;
  s_struct_type saved;
  s_str str;
  for (i = 0; i < 17; i++) {
    m = (s_marshall) {0};
    mr = (s_marshall_read) {0};
    src = (s_struct_type) {0};
    dest = (s_struct_type) {0};
    dest.size = 123;
    saved = dest;
    str = (s_str) {0};
    src.module = sym_1("MarshallLayoutTest");
    TEST_ASSERT(map_init(&src.map, 2));
    tag_init_psym(src.map.key, sym_1("a"));
    tag_init_psym(src.map.key + 1, sym_1("b"));
    tag_init_u64(src.map.value, 0);
    tag_init_u64(src.map.value + 1, 0);
    offsets[0] = 0;
    offsets[1] = sizeof(u64);
    src.offset = offsets;
    src.align_max = _Alignof(u64);
    src.size = 2 * sizeof(u64);
    switch (i) {
    case 0: offsets[0] = 16; src.size = 8; break;
    case 1: offsets[1] = UW_MAX; break;
    case 2: offsets[1] = UW_MAX - 7; break;
    case 3: src.size--; break;
    case 4: src.size = 0; break;
    case 5: offsets[1] = 0; break;
    case 6: offsets[1] = 1; break;
    case 7: src.align_max = 0; break;
    case 8: src.align_max = 3; break;
    case 9: src.align_max = 1; break;
    case 10: src.module = NULL; break;
    case 11:
      tag_clean(src.map.key);
      tag_init_u8(src.map.key, 1);
      break;
    case 12:
      tag_clean(src.map.value);
      TEST_ASSERT(tag_init_pvar(src.map.value,
                                sym_1("MarshallUnknownFieldType")));
      break;
    case 13: src.size = (uw) SW_MAX + 1; break;
    case 14: src.map.key[0].data.td_psym = NULL; break;
    case 15: src.map.value[0].type = TAG_PVAR; break;
    case 16: src.map.value[0].type = TAG_PSTRUCT; break;
    }
    test_context("reject invalid serialized struct layout");
    TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
    TEST_EQ(marshall_test_struct_type(&m, &src, false), &m);
    TEST_ASSERT(marshall_to_str(&m, &str));
    TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
    TEST_EQ(marshall_read_struct_type(&mr, false, &dest), NULL);
    TEST_MEM_EQ(&dest, sizeof(dest), &saved, sizeof(saved));
    marshall_read_clean(&mr);
    marshall_clean(&m);
    str_clean(&str);
    if (i == 15 || i == 16)
      src.map.value[0].type = TAG_VOID;
    map_clean(&src.map);
  }
  test_context(NULL);
}
TEST_CASE_END(marshall_read_struct_layout)

TEST_CASE(marshall_read_struct_roundtrip)
{
  struct { u8 a; u64 b; s_str c; } data = {0}, inline_data = {0};
  bool copy;
  bool copy_saved = env_global()->pass_by_copy;
  s_struct dest = {0};
  s_struct inline_dest = {0};
  s_struct inline_saved;
  uw j;
  bool legacy;
  s_marshall m;
  s_marshall_read mr;
  uw mode;
  s_list *spec;
  s_struct src = {0};
  s_struct_type *st;
  s_str str;
  s_struct_type type_read;
  s_struct_type empty = {0};
  TEST_ASSERT((spec = list_new_1("[{:a, (U8) 0}, {:b, (U64) 0},"
                                 " {:c, \"\"}]")));
  TEST_ASSERT((st = struct_type_new(sym_1("MarshallRoundtripTest"),
                                    spec)));
  list_delete_all(spec);
  TEST_EQ(st->size, sizeof(data));
  TEST_EQ(st->offset[1], offsetof(__typeof__(data), b));
  TEST_EQ(st->offset[2], offsetof(__typeof__(data), c));
  data.a = 42;
  data.b = 123456;
  TEST_ASSERT(str_init_1(&data.c, NULL, "owned field"));
  src.pstruct_type = st;
  for (mode = 0; mode < 4; mode++) {
    m = (s_marshall) {0};
    mr = (s_marshall_read) {0};
    str = (s_str) {0};
    src.data = mode & 2 ? &data : NULL;
    src.tag = mode & 1 ? st->map.value : NULL;
    TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
    TEST_EQ(marshall_struct(&m, false, &src), &m);
    TEST_ASSERT(marshall_to_str(&m, &str));
    TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
    TEST_EQ(marshall_read_struct(&mr, false, &dest), &mr);
    TEST_EQ(!! dest.data, !! (mode & 2));
    TEST_EQ(!! dest.tag, !! (mode & 1));
    if (dest.data) {
      TEST_EQ(((u8 *) dest.data)[0], data.a);
      TEST_EQ(*(u64 *) ((u8 *) dest.data + st->offset[1]), data.b);
      TEST_STR_EQ(*(s_str *) ((u8 *) dest.data + st->offset[2]), data.c);
    }
    struct_clean(&dest);
    dest = (s_struct) {0};
    marshall_read_clean(&mr);
    marshall_clean(&m);
    str_clean(&str);
  }
  src.data = &data;
  src.tag = NULL;
  for (copy = false; ; copy = true) {
    env_global()->pass_by_copy = copy;
    m = (s_marshall) {0};
    mr = (s_marshall_read) {0};
    str = (s_str) {0};
    inline_dest.pstruct_type = st;
    inline_dest.data = &inline_data;
    inline_saved = inline_dest;
    TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
    for (j = 0; j < 2; j++)
      TEST_EQ(marshall_struct(&m, false, &src), &m);
    TEST_ASSERT(marshall_to_str(&m, &str));
    TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
    for (j = 0; j < 2; j++) {
      TEST_EQ(marshall_read_struct(&mr, false, &inline_dest), &mr);
      TEST_MEM_EQ(&inline_dest, sizeof(inline_dest), &inline_saved,
                  sizeof(inline_saved));
      TEST_EQ(inline_data.a, data.a);
      TEST_EQ(inline_data.b, data.b);
      TEST_STR_EQ(inline_data.c, data.c);
      str_clean(&inline_data.c);
      inline_data = (__typeof__(inline_data)) {0};
    }
    marshall_read_clean(&mr);
    marshall_clean(&m);
    str_clean(&str);
    if (copy)
      break;
  }
  env_global()->pass_by_copy = copy_saved;
  empty.module = sym_1("MarshallEmptyTest");
  empty.align_max = 1;
  for (legacy = false; ; legacy = true) {
    for (j = 0; j < 2; j++) {
      m = (s_marshall) {0};
      mr = (s_marshall_read) {0};
      str = (s_str) {0};
      type_read = (s_struct_type) {0};
      TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
      TEST_EQ(marshall_test_struct_type(&m, j ? &empty : st, legacy),
               &m);
      TEST_ASSERT(marshall_to_str(&m, &str));
      TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
      TEST_EQ(marshall_read_struct_type(&mr, false, &type_read), &mr);
      TEST_EQ(type_read.size, j ? 0 : st->size);
      TEST_EQ(type_read.align_max, j ? 1 : st->align_max);
      TEST_EQ(type_read.must_clean, ! j);
      if (! j)
        TEST_MEM_EQ(type_read.offset, 3 * sizeof(uw), st->offset,
                    3 * sizeof(uw));
      struct_type_clean(&type_read);
      marshall_read_clean(&mr);
      marshall_clean(&m);
      str_clean(&str);
    }
    if (legacy)
      break;
  }
  struct_type_delete(st);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_struct_roundtrip)

TEST_CASE(marshall_read_struct_destination)
{
  u64 data[3];
  u64 saved_data[3];
  s_struct dest;
  s_struct saved;
  uw i;
  s_marshall m;
  s_marshall_read mr;
  uw offset;
  s_struct_type *pwire;
  s_list *spec;
  s_struct_type *st;
  s_str str;
  s_struct_type wire;
  TEST_ASSERT((spec = list_new_1("[{:a, (U64) 0}]")));
  TEST_ASSERT((st = struct_type_new(sym_1("MarshallDestinationTest"),
                                    spec)));
  list_delete_all(spec);
  for (i = 0; i < 9; i++) {
    m = (s_marshall) {0};
    mr = (s_marshall_read) {0};
    str = (s_str) {0};
    TEST_ASSERT(struct_type_init_copy(&wire, st));
    pwire = &wire;
    offset = wire.offset[0];
    switch (i) {
    case 0: wire.module = sym_1("MarshallWrongModule"); break;
    case 1: wire.size *= 2; break;
    case 2: tag_init_u8(wire.map.value, 0); break;
    case 3: wire.offset[0] = 8; wire.size = 16; break;
    case 4: tag_init_psym(wire.map.key, sym_1("wrong")); break;
    case 5: break;
    case 6: break;
    case 7: pwire = NULL; break;
    case 8: wire.offset[0] = 16; break;
    }
    memset(data, 0xA5, sizeof(data));
    memcpy(saved_data, data, sizeof(data));
    dest = (s_struct) {0};
    dest.pstruct_type = st;
    dest.data = data + 1;
    saved = dest;
    TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
    TEST_EQ(marshall_1(&m, false, "_KC3STRUCT_"), &m);
    TEST_EQ(marshall_pstruct_type(&m, false, &pwire), &m);
    TEST_EQ(marshall_bool(&m, false, i == 5), &m);
    TEST_EQ(marshall_bool(&m, false, i != 6), &m);
    TEST_EQ(marshall_u64(&m, false, 42), &m);
    TEST_ASSERT(marshall_to_str(&m, &str));
    TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
    TEST_EQ(marshall_read_struct(&mr, false, &dest), NULL);
    TEST_MEM_EQ(&dest, sizeof(dest), &saved, sizeof(saved));
    TEST_MEM_EQ(data, sizeof(data), saved_data, sizeof(saved_data));
    marshall_read_clean(&mr);
    marshall_clean(&m);
    str_clean(&str);
    wire.offset[0] = offset;
    struct_type_clean(&wire);
  }
  struct_type_delete(st);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_struct_destination)

TEST_CASE(marshall_read_struct_nested)
{
  struct { s_str first; u64 second; u64 canary; } data;
  __typeof__(data) saved_data;
  s_struct dest;
  s_struct saved;
  s_fact *fact;
  uw i;
  s_struct inner = {0};
  u64 inner_data = 42;
  s_struct_type *inner_type;
  s_marshall m;
  s_marshall_read mr;
  s_tag module = {0};
  s_struct_type *outer_type;
  s_tag predicate = {0};
  bool removed;
  s_list *spec;
  s_str str;
  s_tag type = {0};
  const s_str value = STR("decoded before the nested field");
  tag_init_psym(&module, sym_1("MarshallNestedTest"));
  tag_init_psym(&predicate, &g_sym_struct_type);
  TEST_ASSERT((spec = list_new_1("[{:value, (U64) 0}]")));
  TEST_ASSERT(tag_init_pstruct_type(&type, module.data.td_psym, spec));
  list_delete_all(spec);
  TEST_ASSERT((fact = facts_add_tags(env_global()->facts, &module,
                                     &predicate, &type)));
  TEST_ASSERT((spec = list_new_1("[{:first, \"\"},"
                                 " {:second, (MarshallNestedTest) ?}]")));
  TEST_ASSERT((outer_type = struct_type_new(sym_1("MarshallOuterTest"),
                                            spec)));
  list_delete_all(spec);
  TEST_EQ(outer_type->size, offsetof(__typeof__(data), canary));
  TEST_ASSERT((inner_type =
               struct_type_new_copy(type.data.td_pstruct_type)));
  inner.pstruct_type = inner_type;
  inner.data = &inner_data;
  for (i = 0; i < 4; i++) {
    inner_type->module = i & 1 ? sym_1("MarshallWrongNestedType") :
      module.data.td_psym;
    m = (s_marshall) {0};
    mr = (s_marshall_read) {0};
    str = (s_str) {0};
    dest = (s_struct) {0};
    memset(&data, 0xA5, sizeof(data));
    saved_data = data;
    if (i & 2) {
      dest.pstruct_type = outer_type;
      dest.data = &data;
    }
    saved = dest;
    TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
    TEST_EQ(marshall_1(&m, false, "_KC3STRUCT_"), &m);
    TEST_EQ(marshall_pstruct_type(&m, false, &outer_type), &m);
    TEST_EQ(marshall_bool(&m, false, false), &m);
    TEST_EQ(marshall_bool(&m, false, true), &m);
    TEST_EQ(marshall_str(&m, false, &value), &m);
    TEST_EQ(marshall_struct(&m, false, &inner), &m);
    TEST_ASSERT(marshall_to_str(&m, &str));
    TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
    if (i & 1) {
      TEST_EQ(marshall_read_struct(&mr, false, &dest), NULL);
      TEST_MEM_EQ(&dest, sizeof(dest), &saved, sizeof(saved));
      TEST_MEM_EQ(&data, sizeof(data), &saved_data, sizeof(saved_data));
    }
    else {
      TEST_EQ(marshall_read_struct(&mr, false, &dest), &mr);
      TEST_STR_EQ(*((s_str *) dest.data), value);
      TEST_EQ(*(u64 *) ((u8 *) dest.data + outer_type->offset[1]), 42);
      if (i & 2) {
        TEST_EQ(data.canary, saved_data.canary);
        TEST_MEM_EQ(&dest, sizeof(dest), &saved, sizeof(saved));
        str_clean(&data.first);
      }
      else
        struct_clean(&dest);
    }
    marshall_read_clean(&mr);
    marshall_clean(&m);
    str_clean(&str);
  }
  struct_type_delete(inner_type);
  struct_type_delete(outer_type);
  TEST_ASSERT(facts_remove_fact(env_global()->facts, fact, &removed));
  TEST_ASSERT(removed);
  tag_clean(&type);
  tag_clean(&predicate);
  tag_clean(&module);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_struct_nested)

TEST_CASE(marshall_read_struct_legacy_limits)
{
  s_struct_type dest;
  s_fact *fact;
  s_tag huge = {0};
  uw i;
  s_marshall m;
  s_marshall_read mr;
  s_tag module = {0};
  s_tag predicate = {0};
  bool removed;
  s_struct_type saved;
  s_struct_type src;
  s_str str;
  tag_init_psym(&module, sym_1("MarshallHugeFieldTest"));
  tag_init_psym(&predicate, &g_sym_struct_type);
  TEST_ASSERT(tag_init_pstruct_type(&huge, module.data.td_psym, NULL));
  huge.data.td_pstruct_type->size = SW_MAX;
  huge.data.td_pstruct_type->align_max = _Alignof(u64);
  TEST_ASSERT((fact = facts_add_tags(env_global()->facts, &module,
                                     &predicate, &huge)));
  for (i = 0; i < 3; i++) {
    m = (s_marshall) {0};
    mr = (s_marshall_read) {0};
    str = (s_str) {0};
    src = (s_struct_type) {0};
    dest = (s_struct_type) {0};
    dest.size = 123;
    saved = dest;
    src.module = sym_1("MarshallLegacyLimitsTest");
    TEST_ASSERT(map_init(&src.map, i ? 2 : 1));
    tag_init_psym(src.map.key, sym_1("a"));
    TEST_ASSERT(tag_init_pvar(src.map.value, module.data.td_psym));
    if (i) {
      tag_init_psym(src.map.key + 1, sym_1("b"));
      if (i == 1)
        tag_init_u8(src.map.value + 1, 0);
      else
        tag_init_u64(src.map.value + 1, 0);
    }
    TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
    TEST_EQ(marshall_test_struct_type(&m, &src, true), &m);
    TEST_ASSERT(marshall_to_str(&m, &str));
    TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
    TEST_EQ(marshall_read_struct_type(&mr, false, &dest), NULL);
    TEST_MEM_EQ(&dest, sizeof(dest), &saved, sizeof(saved));
    marshall_read_clean(&mr);
    marshall_clean(&m);
    str_clean(&str);
    map_clean(&src.map);
  }
  TEST_ASSERT(facts_remove_fact(env_global()->facts, fact, &removed));
  TEST_ASSERT(removed);
  tag_clean(&huge);
  tag_clean(&predicate);
  tag_clean(&module);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_struct_legacy_limits)

TEST_CASE(marshall_read_container_limits)
{
  s_ht ht = {0};
  s_list **items;
  uw i;
  s_tag *key;
  s_map map = {0};
  s_tuple tuple = {0};
  s_tag *tag;
  s_tag *value;
  const uw ht_counts[] = {0, UW_MAX / sizeof(s_list *) + 1,
                         UW_MAX / sizeof(s_list *) + 2, UW_MAX};
  const uw tag_counts[] = {UW_MAX / sizeof(s_tag) + 1,
                          UW_MAX / sizeof(s_tag) + 2, UW_MAX};
  TEST_EQ(ht_init(&ht, &g_sym_Tag, 4), &ht);
  items = ht.items;
  for (i = 0; i < sizeof(ht_counts) / sizeof(ht_counts[0]); i++) {
    TEST_EQ(ht_init(&ht, &g_sym_Tag, ht_counts[i]), NULL);
    TEST_EQ(ht.items, items);
    TEST_EQ(ht.size, 4);
    TEST_EQ(ht.ref_count, 1);
  }
  ht_clean(&ht);
  ht = (s_ht) {0};
  TEST_EQ(ht_init(&ht, &g_sym_Tag, ht_counts[2]), NULL);
  ht_clean(&ht);
  TEST_EQ(map_init(&map, 2), &map);
  key = map.key;
  value = map.value;
  for (i = 0; i < sizeof(tag_counts) / sizeof(tag_counts[0]); i++) {
    TEST_EQ(map_init(&map, tag_counts[i]), NULL);
    TEST_EQ(map.count, 2);
    TEST_EQ(map.key, key);
    TEST_EQ(map.value, value);
  }
  map_clean(&map);
  map = (s_map) {0};
  TEST_EQ(map_init(&map, tag_counts[1]), NULL);
  map_clean(&map);
  TEST_EQ(map_init(&map, 0), &map);
  TEST_EQ(map.count, 0);
  map_clean(&map);
  TEST_EQ(tuple_init(&tuple, 2), &tuple);
  tag = tuple.tag;
  tag_init_bool(tag, true);
  for (i = 0; i < sizeof(tag_counts) / sizeof(tag_counts[0]); i++) {
    TEST_EQ(tuple_init(&tuple, tag_counts[i]), NULL);
    TEST_EQ(tuple.count, 2);
    TEST_EQ(tuple.tag, tag);
    TEST_EQ(tuple.tag->type, TAG_BOOL);
    TEST_EQ(tuple.tag->data.td_bool_, true);
    TEST_EQ(tuple.ref_count, 1);
  }
  TEST_EQ(tuple_init(&tuple, 0), NULL);
  TEST_EQ(tuple_init(&tuple, 1), NULL);
  TEST_EQ(tuple.count, 2);
  TEST_EQ(tuple.tag, tag);
  tuple_clean(&tuple);
  tuple = (s_tuple) {0};
  TEST_EQ(tuple_init(&tuple, tag_counts[1]), NULL);
  tuple_clean(&tuple);
}
TEST_CASE_END(marshall_read_container_limits)

TEST_CASE(marshall_read_container_counts)
{
  s_buf *buf;
  bool heap;
  uw i;
  uw j;
  s_marshall m = {0};
  s_map map = {0};
  s_marshall_read mr = {0};
  uw pos;
  s_tuple tuple = {0};
  s_tag value = {0};
  const uw counts[] = {UW_MAX / sizeof(s_tag) + 1,
                       UW_MAX / sizeof(s_tag) + 2, UW_MAX, 0, 1};
  tag_init_bool(&value, true);
  for (j = 0; j < 2; j++) {
    heap = j;
    for (i = 0; i < sizeof(counts) / sizeof(counts[0]); i++) {
      TEST_EQ(marshall_init(&m, 256), &m);
      mr.buf = &m.buf;
      mr.heap = &m.heap;
      buf = heap ? mr.heap : mr.buf;
      if (i < 3) {
        TEST_EQ(marshall_1(&m, heap, "_KC3MAP_"), &m);
        TEST_EQ(marshall_uw(&m, heap, counts[i]), &m);
        pos = buf->wpos;
        TEST_EQ(marshall_tag(&m, heap, &value), &m);
        TEST_EQ(marshall_read_map(&mr, heap, &map), NULL);
        TEST_EQ(map.count, 0);
        TEST_EQ(map.key, NULL);
        TEST_EQ(map.value, NULL);
        TEST_EQ(buf->rpos, pos);
        map_clean(&map);
      }
      buf_empty(buf);
      TEST_EQ(marshall_uw(&m, heap, counts[i]), &m);
      pos = buf->wpos;
      TEST_EQ(marshall_tag(&m, heap, &value), &m);
      TEST_EQ(marshall_read_tuple(&mr, heap, &tuple), NULL);
      TEST_EQ(tuple.count, 0);
      TEST_EQ(tuple.tag, NULL);
      TEST_EQ(buf->rpos, pos);
      tuple_clean(&tuple);
      marshall_clean(&m);
    }
  }
}
TEST_CASE_END(marshall_read_container_counts)

TEST_CASE(marshall_read_header_limits)
{
  s_buf buf;
  uw i;
  s_list **items;
  s_marshall_header mh;
  s_marshall_read mr = {0};
  struct {
    u64 offset;
    u64 count;
    u64 heap;
    u64 body;
    bool valid;
    bool limited;
  } cases[] = {
    {0, 0, 0, 0, true, false},
    {0, 0, 0, SW_MAX - sizeof(mh), true, false},
    {0, 1, 1, 0, true, false},
    {0, 0, 0, 32, true, true},
    {0, 4, 16, 32, true, true},
    {0, UW_MAX / sizeof(s_list *) + 1, 0, 0, false, false},
    {0, UW_MAX / sizeof(s_list *) + 2, 0, 0, false, false},
    {0, U64_MAX, 0, 0, false, false},
    {U64_MAX, 0, 0, 0, false, false},
    {0, 1, U64_MAX, 0, false, false},
    {0, 0, 0, U64_MAX, false, false},
    {0, 1, SW_MAX, 0, false, false},
    {0, 1, SW_MAX - sizeof(mh), 1, false, false},
    {SW_MAX, 1, 1, 0, false, false},
    {0, 0, 1, 0, false, false},
    {0, 1, 0, 0, false, false},
    {0, 2, 1, 0, false, false},
    {0, 5, 8, 0, false, true},
    {0, 1, 17, 0, false, true},
    {0, 0, 0, 33, false, true}
  };
  for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    mh = (s_marshall_header) {htole64(MARSHALL_MAGIC),
                            htole64(cases[i].offset),
                            htole64(cases[i].count),
                            htole64(cases[i].heap),
                            htole64(cases[i].body)};
    TEST_EQ(buf_init_const(&buf, sizeof(mh), (const char *) &mh), &buf);
    buf.wpos = sizeof(mh);
    TEST_EQ(marshall_read_init_buf(&mr, &buf), &mr);
    TEST_EQ(ht_init(&mr.ht, &g_sym_Tuple, 4), &mr.ht);
    items = mr.ht.items;
    mr.heap_count = 4;
    mr.heap_size = 16;
    mr.buf_size = 32;
    if (cases[i].limited) {
      TEST_EQ(marshall_read_header_limit(&mr, 4, 16, 32),
              cases[i].valid ? &mr : NULL);
    }
    else {
      TEST_EQ(marshall_read_header(&mr), cases[i].valid ? &mr : NULL);
    }
    TEST_EQ(buf.rpos, sizeof(mh));
    if (cases[i].valid) {
      TEST_EQ(mr.heap_count, cases[i].count);
      TEST_EQ(mr.heap_size, cases[i].heap);
      TEST_EQ(mr.buf_size, cases[i].body);
      TEST_EQ(mr.ht.size, cases[i].count);
    }
    else {
      TEST_EQ(mr.heap_count, 4);
      TEST_EQ(mr.heap_size, 16);
      TEST_EQ(mr.buf_size, 32);
      TEST_EQ(mr.ht.items, items);
      TEST_EQ(mr.ht.size, 4);
    }
    marshall_read_clean(&mr);
    buf_clean(&buf);
  }
  mh = (s_marshall_header) {0};
  TEST_EQ(buf_init_const(&buf, sizeof(mh), (const char *) &mh), &buf);
  buf.wpos = sizeof(mh);
  TEST_EQ(marshall_read_init_buf(&mr, &buf), &mr);
  TEST_EQ(marshall_read_header(&mr), NULL);
  TEST_EQ(mr.ht.items, NULL);
  buf.rpos = 0;
  buf.wpos--;
  TEST_EQ(marshall_read_header(&mr), NULL);
  TEST_EQ(buf.rpos, 0);
  marshall_read_clean(&mr);
  buf_clean(&buf);
}
TEST_CASE_END(marshall_read_header_limits)

TEST_CASE(marshall_read_array)
{
  s_str actual = {0};
  s_tag dest = {0};
  s_tag evaluated = {0};
  s_str expected = {0};
  uw i;
  const char *inputs[] = {
    "(U8[]) {0, 1, 2}",
    "(U64[]) {42}",
    "(U64[]) {{1, 2}, {3, 4}}",
    "(U64[]) {{{1, 2}}, {{3, 4}}}",
    "(Str[]) {\"one\", \"two\"}",
    "(U64[]) {}"
  };
  uw j;
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_tag parsed = {0};
  const s_tag *src;
  s_str str = {0};
  for (i = 0; i < sizeof(inputs) / sizeof(inputs[0]); i++) {
    test_context(inputs[i]);
    TEST_ASSERT(tag_init_1(&parsed, inputs[i]));
    TEST_ASSERT(env_eval_tag(env_global(), &parsed, &evaluated));
    for (j = 0; j < 2; j++) {
      src = j ? &evaluated : &parsed;
      TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
      TEST_EQ(marshall_tag(&m, false, src), &m);
      TEST_EQ(marshall_to_str(&m, &str), &str);
      TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
      TEST_EQ(marshall_read_tag(&mr, false, &dest), &mr);
      TEST_EQ(dest.type, TAG_ARRAY);
      TEST_EQ(dest.data.td_array.array_type,
              src->data.td_array.array_type);
      TEST_EQ(dest.data.td_array.element_type,
              src->data.td_array.element_type);
      TEST_EQ(dest.data.td_array.dimension_count,
              src->data.td_array.dimension_count);
      TEST_EQ(dest.data.td_array.count, src->data.td_array.count);
      TEST_EQ(dest.data.td_array.size, src->data.td_array.size);
      TEST_EQ(! dest.data.td_array.data, ! src->data.td_array.data);
      TEST_EQ(inspect_tag(&dest, &actual), &actual);
      TEST_EQ(inspect_tag(src, &expected), &expected);
      TEST_STR_EQ(actual, expected);
      str_clean(&actual);
      str_clean(&expected);
      TEST_EQ(mr.buf->rpos, mr.buf->wpos);
      tag_clean(&dest);
      marshall_read_clean(&mr);
      marshall_clean(&m);
      str_clean(&str);
    }
    tag_clean(&evaluated);
    tag_clean(&parsed);
  }
  test_context(NULL);
}
TEST_CASE_END(marshall_read_array)

TEST_CASE(marshall_read_array_empty)
{
  s_array dest = {0};
  uw i;
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_array src = {0};
  s_str str = {0};
  for (i = 0; i < 2; i++) {
    if (i)
      TEST_ASSERT(array_init(&src, sym_1("U64[]"), 2,
                             (uw []) {2, 0}));
    else
      TEST_ASSERT(array_init_void(&src));
    TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
    TEST_EQ(marshall_array(&m, false, &src), &m);
    TEST_EQ(marshall_to_str(&m, &str), &str);
    TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
    TEST_EQ(marshall_read_array(&mr, false, &dest), &mr);
    TEST_EQ(dest.array_type, src.array_type);
    TEST_EQ(dest.element_type, src.element_type);
    TEST_EQ(dest.dimension_count, src.dimension_count);
    TEST_EQ(dest.count, 0);
    TEST_EQ(dest.size, 0);
    TEST_ASSERT(! dest.data && ! dest.tags);
    TEST_EQ(mr.buf->rpos, mr.buf->wpos);
    array_clean(&dest);
    array_clean(&src);
    marshall_read_clean(&mr);
    marshall_clean(&m);
    str_clean(&str);
  }
}
TEST_CASE_END(marshall_read_array_empty)

#ifdef NDEBUG
static char marshall_read_array_invalid_layout (const s_array *src)
{
  s_array dest = {0};
  uw i;
  s_marshall m = {0};
  uw mode;
  s_marshall_read mr = {0};
  uw payload_pos;
  s_str str = {0};
  /* Check data-backed, tag-backed and unallocated arrays. */
  for (mode = 0; mode < 3; mode++) {
    TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
    TEST_ASSERT(marshall_psym(&m, false, &src->array_type));
    TEST_ASSERT(marshall_psym(&m, false, &src->element_type));
    TEST_ASSERT(marshall_uw(&m, false, src->dimension_count));
    for (i = 0; i < src->dimension_count; i++) {
      TEST_ASSERT(marshall_uw(&m, false, src->dimensions[i].count));
      TEST_ASSERT(marshall_uw(&m, false, src->dimensions[i].item_size));
    }
    TEST_ASSERT(marshall_uw(&m, false, src->count));
    TEST_ASSERT(marshall_uw(&m, false, src->size));
    TEST_ASSERT(marshall_bool(&m, false, mode == 0));
    payload_pos = m.buf.wpos;
    if (mode != 0)
      TEST_ASSERT(marshall_bool(&m, false, mode == 1));
    else
      TEST_ASSERT(marshall_u64(&m, false, 42));
    TEST_EQ(marshall_to_str(&m, &str), &str);
    TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
    payload_pos += mr.buf->rpos;
    TEST_ASSERT(! marshall_read_array(&mr, false, &dest));
    TEST_EQ(mr.buf->rpos, payload_pos);
    TEST_ASSERT(! dest.dimensions && ! dest.data && ! dest.tags);
    TEST_EQ(dest.dimension_count, 0);
    TEST_EQ(dest.count, 0);
    marshall_read_clean(&mr);
    marshall_clean(&m);
    str_clean(&str);
  }
  return 0;
}

TEST_CASE(marshall_read_array_layout)
{
  struct {
    const char *context;
    uw dimension_count;
    s_array_dimension dimensions[2];
    uw count;
    uw size;
  } cases[] = {
    {"undersized element stride", 1, {{1, 1}}, 1, 1},
    {"oversized element stride", 1, {{1, 16}}, 1, 16},
    {"zero element stride", 1, {{1, 0}}, 1, 8},
    {"inconsistent outer stride", 2, {{2, 8}, {2, 8}}, 4, 32},
    {"element count below dimension product", 1, {{2, 8}}, 1, 16},
    {"element count above dimension product", 1, {{2, 8}}, 3, 16},
    {"size below dimension product", 1, {{2, 8}}, 2, 8},
    {"size above dimension product", 1, {{2, 8}}, 2, 24},
    {"dimension size overflow", 1, {{UW_MAX / 8 + 1, 8}},
     UW_MAX / 8 + 1, 0},
    {"outer dimension size overflow", 2,
     {{2, UW_MAX / 8 * 8}, {UW_MAX / 8, 8}}, 2 * (UW_MAX / 8),
     2 * (UW_MAX / 8 * 8)}
  };
  uw i;
  s_array src = {0};
  src.array_type = sym_1("U64[]");
  src.element_type = &g_sym_U64;
  for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    test_context(cases[i].context);
    src.dimension_count = cases[i].dimension_count;
    src.dimensions = cases[i].dimensions;
    src.count = cases[i].count;
    src.size = cases[i].size;
    TEST_EQ(marshall_read_array_invalid_layout(&src), 0);
  }
  src.dimension_count = 1;
  src.dimensions = (s_array_dimension []) {{1, 8}};
  src.count = 1;
  src.size = 8;
  test_context("array and element types disagree");
  src.element_type = &g_sym_U8;
  TEST_EQ(marshall_read_array_invalid_layout(&src), 0);
  test_context("zero-sized element type");
  src.array_type = sym_1("Void[]");
  src.element_type = &g_sym_Void;
  TEST_EQ(marshall_read_array_invalid_layout(&src), 0);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_array_layout)
#endif

TEST_CASE(marshall_read_bool)
{
  MARSHALL_READ_TEST(bool,
                     "KC3MARSH"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x0a\x00\x00\x00\x00\x00\x00\x00"
                     "\x5f\x4b\x43\x33\x42\x4f\x4f\x4c"
                     "\x5f\x00",
                     "false");
  MARSHALL_READ_TEST(bool,
                     "KC3MARSH"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x00\x00\x00\x00\x00\x00\x00\x00"
                     "\x0a\x00\x00\x00\x00\x00\x00\x00"
                     "\x5f\x4b\x43\x33\x42\x4f\x4f\x4c"
                     "\x5f\x01",
                     "true");
}
TEST_CASE_END(marshall_read_bool)


#define MARSHALL_READ_CACHE_TYPE_TEST(name, type, magic)                \
  do {                                                                 \
    s_marshall m = {0};                                                \
    s_marshall_read mr = {0};                                          \
    s_str str = {0};                                                   \
    p_sym sym = sym_1("cache_type_test");                               \
    p_sym sym_read = NULL;                                             \
    type dest = (type) &str;                                           \
    test_context(magic);                                              \
    TEST_ASSERT(marshall_init(&m, BUF_SIZE));                           \
    TEST_ASSERT(marshall_psym(&m, false, &sym));                        \
    TEST_ASSERT(marshall_1(&m, false, magic));                          \
    TEST_ASSERT(marshall_offset(&m, false,                            \
                                 sizeof(s_marshall_header)));         \
    TEST_ASSERT(marshall_to_str(&m, &str));                            \
    TEST_ASSERT(marshall_read_init_str(&mr, &str));                    \
    TEST_ASSERT(marshall_read_psym(&mr, false, &sym_read));             \
    TEST_EQ(sym_read, sym);                                           \
    TEST_EQ(marshall_read_ ## name(&mr, false, &dest), NULL);          \
    TEST_EQ(dest, (type) &str);                                       \
    TEST_ASSERT(mr.ht_failed);                                        \
    marshall_read_clean(&mr);                                         \
    str_clean(&str);                                                  \
    marshall_clean(&m);                                               \
    test_context(NULL);                                              \
  } while (0)

TEST_CASE(marshall_read_cache_types)
{
  MARSHALL_READ_CACHE_TYPE_TEST(pcall, p_call,
                                "_KC3PCALL_");
  MARSHALL_READ_CACHE_TYPE_TEST(pcallable, p_callable,
                                "_KC3PCALLABLE_");
  MARSHALL_READ_CACHE_TYPE_TEST(pcomplex, p_complex,
                                "_KC3PCOMPLEX_");
  MARSHALL_READ_CACHE_TYPE_TEST(pfacts, p_facts,
                                "_KC3PFACTS_");
  MARSHALL_READ_CACHE_TYPE_TEST(pframe, p_frame,
                                "_KC3PFRAME_");
  MARSHALL_READ_CACHE_TYPE_TEST(plist, p_list,
                                "_KC3PLIST_");
  MARSHALL_READ_CACHE_TYPE_TEST(pset_item__fact, p_set_item__fact,
                                "_KC3PSETITEMFACT_");
  MARSHALL_READ_CACHE_TYPE_TEST(pset_item__tag, p_set_item__tag,
                                "_KC3PSETITEMTAG_");
  MARSHALL_READ_CACHE_TYPE_TEST(pskiplist_node__fact, p_skiplist_node__fact,
                                "_KC3PSKIPLISTNODEFACT_");
  MARSHALL_READ_CACHE_TYPE_TEST(pskiplist__fact, p_skiplist__fact,
                                "_KC3PSKIPLISTFACT_");
  MARSHALL_READ_CACHE_TYPE_TEST(pstruct, p_struct,
                                "_KC3PSTRUCT_");
  MARSHALL_READ_CACHE_TYPE_TEST(pstruct_type, p_struct_type,
                                "_KC3PSTRUCTTYPE_");
  MARSHALL_READ_CACHE_TYPE_TEST(ptag, p_tag,
                                "_KC3PTAG_");
  MARSHALL_READ_CACHE_TYPE_TEST(pvar, p_var,
                                "_KC3PVAR_");
  MARSHALL_READ_CACHE_TYPE_TEST(ptuple, p_tuple,
                                "_KC3PTUPLE_");
}
TEST_CASE_END(marshall_read_cache_types)

TEST_CASE(marshall_read_cache_state)
{
  const char *types[] = {
    "Call", "Callable", "Complex", "Facts", "Frame", "List",
    "_KC3PSETITEMFACT_", "_KC3PSETITEMTAG_", "Skiplist.Node.Fact",
    "Skiplist.Fact", "Struct", "StructType", "Sym", "Tag", "Var",
    "Tuple", "U8*", "U64*"
  };
  uw i;
  uw j;
  uw state;
  u8 token;
  s_marshall m = {0};
  s_marshall_read mr;
  s_str str = {0};
  u64 offset;
  void *present;
  p_sym type;
  p_sym other;
  TEST_ASSERT(marshall_init(&m, BUF_SIZE));
  TEST_ASSERT(marshall_1(&m, true, "x"));
  m.heap_count = 1;
  for (i = 0; i < 3; i++)
    TEST_ASSERT(marshall_offset(&m, false,
                                sizeof(s_marshall_header)));
  TEST_ASSERT(marshall_to_str(&m, &str));
  for (i = 0; i < sizeof(types) / sizeof(*types); i++) {
    type = sym_1(types[i]);
    for (j = 0; j < sizeof(types) / sizeof(*types); j++) {
      other = sym_1(types[j]);
      mr = (s_marshall_read) {0};
      TEST_ASSERT(marshall_read_init_str(&mr, &str));
      TEST_ASSERT(marshall_read_heap_pointer(&mr, false, &offset,
                                             &present, type));
      TEST_EQ(present, NULL);
      TEST_ASSERT(marshall_read_ht_add(&mr, offset, &token, type));
      present = &str;
      if (i == j) {
        TEST_ASSERT(marshall_read_heap_pointer(&mr, false, &offset,
                                               &present, other));
        TEST_EQ(present, &token);
      }
      else if (sym_is_pointer_type(type, NULL) !=
               sym_is_pointer_type(other, NULL)) {
        TEST_ASSERT(marshall_read_heap_pointer(&mr, false, &offset,
                                               &present, other));
        TEST_EQ(present, NULL);
        TEST_ASSERT(marshall_read_ht_add(&mr, offset, &str, other));
        TEST_ASSERT(marshall_read_heap_pointer(&mr, false, &offset,
                                               &present, type));
        TEST_EQ(present, &token);
      }
      else {
        TEST_EQ(marshall_read_heap_pointer(&mr, false, &offset,
                                           &present, other), NULL);
        TEST_EQ(present, &str);
        TEST_ASSERT(mr.ht_failed);
        TEST_EQ(marshall_read_heap_pointer(&mr, false, &offset,
                                           &present, type), NULL);
      }
      marshall_read_clean(&mr);
    }
  }
  for (state = 0; state < 4; state++) {
    mr = (s_marshall_read) {0};
    TEST_ASSERT(marshall_read_init_str(&mr, &str));
    TEST_ASSERT(marshall_read_heap_pointer(&mr, false, &offset,
                                           &present, &g_sym_Sym));
    switch (state) {
    case 0:
      TEST_EQ(marshall_read_heap_pointer(&mr, false, &offset,
                                         &present, &g_sym_Sym), NULL);
      break;
    case 1:
      TEST_EQ(marshall_read_ht_add(&mr, offset, &token,
                                   &g_sym_Tuple), NULL);
      break;
    case 2:
      TEST_ASSERT(marshall_read_ht_add(&mr, offset, &token, &g_sym_Sym));
      TEST_EQ(marshall_read_ht_add(&mr, offset, &token,
                                   &g_sym_Sym), NULL);
      break;
    case 3:
      TEST_EQ(marshall_read_ht_add(&mr, offset, NULL, &g_sym_Sym), NULL);
      break;
    }
    TEST_ASSERT(mr.ht_failed);
    marshall_read_clean(&mr);
  }
  str_clean(&str);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_read_cache_state)

TEST_CASE(marshall_read_cache_offsets)
{
  uw i;
  const u64 offsets[] = {
    0, 1, sizeof(s_marshall_header) - 1,
    sizeof(s_marshall_header), sizeof(s_marshall_header) + 1,
    sizeof(s_marshall_header) + 2, (u64) SW_MAX, U64_MAX
  };
  s_marshall m;
  s_marshall_read mr;
  s_str str;
  void *present;
  u64 offset;
  for (i = 0; i < sizeof(offsets) / sizeof(*offsets) + 3; i++) {
    m = (s_marshall) {0};
    mr = (s_marshall_read) {0};
    str = (s_str) {0};
    TEST_ASSERT(marshall_init(&m, BUF_SIZE));
    TEST_ASSERT(marshall_1(&m, true, "x"));
    m.heap_count = 1;
    TEST_ASSERT(marshall_offset(&m, false, i < 8 ? offsets[i] :
                                sizeof(s_marshall_header)));
    TEST_ASSERT(marshall_to_str(&m, &str));
    TEST_ASSERT(marshall_read_init_str(&mr, &str));
    switch (i) {
    case 8: mr.heap_start = SW_MAX; break;
    case 9: mr.heap_offset = SW_MAX; break;
    case 10: mr.heap_start = -(s64) sizeof(s_marshall_header) - 1; break;
    }
    present = &str;
    if (i == 0 || i == 3) {
      TEST_ASSERT(marshall_read_heap_pointer(&mr, false, &offset,
                                             &present, &g_sym_Sym));
      TEST_EQ(present, NULL);
      TEST_EQ(mr.ht.count, i == 0 ? 0 : 1);
    }
    else {
      TEST_EQ(marshall_read_heap_pointer(&mr, false, &offset,
                                         &present, &g_sym_Sym), NULL);
      TEST_EQ(present, &str);
      TEST_EQ(mr.ht.count, 0);
      TEST_ASSERT(mr.ht_failed);
    }
    marshall_read_clean(&mr);
    str_clean(&str);
    marshall_clean(&m);
  }
}
TEST_CASE_END(marshall_read_cache_offsets)

TEST_CASE(marshall_read_cache_cycle)
{
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_str str = {0};
  p_frame dest = (p_frame) &str;
  p_list cycle;
  p_list list_dest = (p_list) &str;
  TEST_ASSERT(marshall_init(&m, BUF_SIZE));
  TEST_ASSERT(marshall_1(&m, false, "_KC3PFRAME_"));
  TEST_ASSERT(marshall_offset(&m, false, sizeof(s_marshall_header)));
  TEST_ASSERT(marshall_1(&m, true, "_KC3FRAME_"));
  TEST_ASSERT(marshall_uw(&m, true, 0));
  TEST_ASSERT(marshall_1(&m, true, "_KC3PFRAME_"));
  TEST_ASSERT(marshall_offset(&m, true, sizeof(s_marshall_header)));
  m.heap_count = 1;
  TEST_ASSERT(marshall_to_str(&m, &str));
  TEST_ASSERT(marshall_read_init_str(&mr, &str));
  TEST_EQ(marshall_read_pframe(&mr, false, &dest), NULL);
  TEST_EQ(dest, (p_frame) &str);
  TEST_ASSERT(mr.ht_failed);
  marshall_read_clean(&mr);
  str_clean(&str);
  marshall_clean(&m);
  /* Fail after nested lists have already entered the retention pool. */
  m = (s_marshall) {0};
  mr = (s_marshall_read) {0};
  str = (s_str) {0};
  TEST_ASSERT((cycle = list_new_plist_1("[1, 2]", NULL)));
  cycle->next.data.td_plist = cycle;
  TEST_ASSERT(marshall_init(&m, BUF_SIZE));
  TEST_ASSERT(marshall_plist(&m, false, &cycle));
  TEST_ASSERT(marshall_to_str(&m, &str));
  TEST_ASSERT(marshall_read_init_str(&mr, &str));
  TEST_EQ(marshall_read_plist(&mr, false, &list_dest), NULL);
  TEST_EQ(list_dest, (p_list) &str);
  TEST_ASSERT(mr.ht_failed);
  marshall_read_clean(&mr);
  cycle->next.data.td_plist = NULL;
  list_delete_all(cycle);
  str_clean(&str);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_read_cache_cycle)

TEST_CASE(marshall_read_cache_skiplist_cycle)
{
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_str str = {0};
  p_skiplist_node__fact dest = (p_skiplist_node__fact) &str;
  TEST_ASSERT(marshall_init(&m, BUF_SIZE));
  TEST_ASSERT(marshall_1(&m, false, "_KC3PSKIPLISTNODEFACT_"));
  TEST_ASSERT(marshall_offset(&m, false, sizeof(s_marshall_header)));
  TEST_ASSERT(marshall_1(&m, true, "_KC3SKIPLISTNODEFACT_"));
  TEST_ASSERT(marshall_u8(&m, true, 1));
  TEST_ASSERT(marshall_1(&m, true, "_KC3PSETITEMFACT_"));
  TEST_ASSERT(marshall_offset(&m, true, 0));
  TEST_ASSERT(marshall_1(&m, true, "_KC3PSKIPLISTNODEFACT_"));
  TEST_ASSERT(marshall_offset(&m, true, sizeof(s_marshall_header)));
  m.heap_count = 1;
  TEST_ASSERT(marshall_to_str(&m, &str));
  TEST_ASSERT(marshall_read_init_str(&mr, &str));
  TEST_EQ(marshall_read_pskiplist_node__fact(&mr, false, &dest), NULL);
  TEST_EQ(dest, (p_skiplist_node__fact) &str);
  TEST_ASSERT(mr.ht_failed);
  marshall_read_clean(&mr);
  str_clean(&str);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_read_cache_skiplist_cycle)

TEST_CASE(marshall_read_cache_shared)
{
  const char *values[] = {
    "{1, 2}", "[1, 2]", ":cache_shared", "[]", "[1 | :tail]",
    "[[1, 2], {3, 4}, \"cached string\"]"
  };
  uw copy;
  uw i;
  bool copy_saved = env_global()->pass_by_copy;
  bool list;
  s_marshall m;
  s_marshall_read mr;
  s_str str;
  s_tag src;
  s_tag a;
  s_tag b;
  s_tag tail;
  s_tag again;
  s_tag other;
  s_tag other_result;
  s_list other_node;
  for (copy = 0; copy < 2; copy++) {
    env_global()->pass_by_copy = copy;
    for (i = 0; i < sizeof(values) / sizeof(*values); i++) {
      m = (s_marshall) {0};
      mr = (s_marshall_read) {0};
      str = (s_str) {0};
      src = (s_tag) {0};
      a = (s_tag) {0};
      b = (s_tag) {0};
      tail = (s_tag) {0};
      again = (s_tag) {0};
      other = (s_tag) {0};
      other_result = (s_tag) {0};
      TEST_ASSERT(tag_init_1(&src, values[i]));
      list = src.type == TAG_PLIST && src.data.td_plist;
      TEST_ASSERT(marshall_init(&m, BUF_SIZE));
      TEST_ASSERT(marshall_tag(&m, false, &src));
      TEST_ASSERT(marshall_tag(&m, false, &src));
      TEST_ASSERT(marshall_tag(&m, false, &src));
      TEST_ASSERT(marshall_tag(&m, false, &src));
      if (list) {
        TEST_ASSERT(marshall_tag(&m, false, &src.data.td_plist->next));
        /* A distinct parent shares an already-serialized list suffix. */
        TEST_ASSERT(list_init_u8(&other_node, 77,
                                 list_next(src.data.td_plist)));
        TEST_ASSERT(tag_init_plist(&other, &other_node));
        TEST_ASSERT(marshall_tag(&m, false, &other));
      }
      TEST_ASSERT(marshall_to_str(&m, &str));
      TEST_ASSERT(marshall_read_init_str(&mr, &str));
      TEST_ASSERT(marshall_read_tag(&mr, false, &a));
      TEST_EQ(compare_tag(&a, &src), 0);
      /* List nodes belong to the caller: mutation must not alter the cache. */
      if (list) {
        tag_clean(&a.data.td_plist->tag);
        TEST_ASSERT(tag_init_u8(&a.data.td_plist->tag, 99));
      }
      TEST_ASSERT(marshall_read_tag(&mr, false, &b));
      TEST_EQ(compare_tag(&b, &src), 0);
      /* Release the first result before following later shared offsets. */
      tag_clean(&a);
      a = (s_tag) {0};
      TEST_ASSERT(marshall_read_tag(&mr, false, &a));
      TEST_EQ(compare_tag(&a, &src), 0);
      tag_clean(&b);
      b = (s_tag) {0};
      TEST_ASSERT(marshall_read_tag(&mr, false, &b));
      TEST_EQ(compare_tag(&b, &src), 0);
      if (list) {
        TEST_ASSERT(marshall_read_tag(&mr, false, &tail));
        TEST_EQ(compare_tag(&tail, &src.data.td_plist->next), 0);
        TEST_ASSERT(marshall_read_tag(&mr, false, &other_result));
        TEST_EQ(compare_tag(&other_result, &other), 0);
      }
      TEST_ASSERT(! mr.ht_failed);
      /* A new header discards the old cache while results remain live. */
      mr.buf->rpos = 0;
      TEST_ASSERT(marshall_read_header(&mr));
      mr.buf->rpos += mr.heap_size;
      TEST_ASSERT(marshall_read_tag(&mr, false, &again));
      TEST_EQ(compare_tag(&again, &src), 0);
      marshall_read_clean(&mr);
      /* Returned values must also outlive the decoding context. */
      TEST_EQ(compare_tag(&a, &src), 0);
      TEST_EQ(compare_tag(&b, &src), 0);
      TEST_EQ(compare_tag(&again, &src), 0);
      if (list) {
        TEST_EQ(compare_tag(&tail, &src.data.td_plist->next), 0);
        TEST_EQ(compare_tag(&other_result, &other), 0);
        list_clean(&other_node);
      }
      tag_clean(&other_result);
      tag_clean(&again);
      tag_clean(&tail);
      tag_clean(&b);
      tag_clean(&a);
      tag_clean(&src);
      str_clean(&str);
      marshall_clean(&m);
    }
  }
  env_global()->pass_by_copy = copy_saved;
}
TEST_CASE_END(marshall_read_cache_shared)

TEST_CASE(marshall_read_cache_pointer)
{
#if HAVE_PTHREAD
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_mutex *mutex;
  s_pointer src = {0};
  s_pointer a = {0};
  s_pointer b = {0};
  s_str str = {0};
  TEST_ASSERT((mutex = mutex_new()));
  TEST_ASSERT(pointer_init(&src, NULL, &g_sym_Mutex, mutex));
  TEST_ASSERT(marshall_init(&m, BUF_SIZE));
  TEST_ASSERT(marshall_pointer(&m, false, &src));
  TEST_ASSERT(marshall_pointer(&m, false, &src));
  TEST_ASSERT(marshall_to_str(&m, &str));
  TEST_ASSERT(marshall_read_init_str(&mr, &str));
  TEST_ASSERT(marshall_read_pointer(&mr, false, &a));
  TEST_ASSERT(marshall_read_pointer(&mr, false, &b));
  TEST_EQ(a.target_type, &g_sym_Mutex);
  TEST_EQ(b.target_type, &g_sym_Mutex);
  TEST_ASSERT(a.ptr.p_pvoid);
  TEST_EQ(a.ptr.p_pvoid, b.ptr.p_pvoid);
  TEST_EQ(a.id, b.id);
  marshall_read_clean(&mr);
  mutex_delete(a.ptr.p_pvoid);
  mutex_delete(mutex);
  str_clean(&str);
  marshall_clean(&m);
#endif
}
TEST_CASE_END(marshall_read_cache_pointer)

TEST_CASE(marshall_read_cache_pointer_legacy)
{
#if HAVE_PTHREAD
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_pointer a = {0};
  s_pointer b = {0};
  s_pointer saved;
  s_str str = {0};
  p_sym type = &g_sym_Mutex;
  u64 offset;
  TEST_ASSERT(marshall_init(&m, BUF_SIZE));
  TEST_ASSERT(marshall_1(&m, false, "_KC3POINTER_"));
  TEST_ASSERT(marshall_psym(&m, false, &type));
  offset = sizeof(s_marshall_header) + m.heap_pos;
  TEST_ASSERT(marshall_offset(&m, false, offset));
  TEST_ASSERT(marshall_1(&m, false, "_KC3POINTER_"));
  TEST_ASSERT(marshall_psym(&m, false, &type));
  TEST_ASSERT(marshall_offset(&m, false, offset));
  TEST_ASSERT(marshall_1(&m, false, "_KC3POINTER_"));
  type = &g_sym_U8;
  TEST_ASSERT(marshall_psym(&m, false, &type));
  TEST_ASSERT(marshall_offset(&m, false, offset));
  m.heap_count++;
  TEST_ASSERT(marshall_to_str(&m, &str));
  TEST_ASSERT(marshall_read_init_str(&mr, &str));
  TEST_ASSERT(marshall_read_pointer(&mr, false, &a));
  TEST_ASSERT(marshall_read_pointer(&mr, false, &b));
  TEST_ASSERT(a.ptr.p_pvoid);
  TEST_EQ(a.target_type, &g_sym_Mutex);
  TEST_EQ(b.target_type, &g_sym_Mutex);
  TEST_EQ(a.ptr.p_pvoid, b.ptr.p_pvoid);
  saved = b;
  TEST_EQ(marshall_read_pointer(&mr, false, &b), NULL);
  TEST_MEM_EQ(&b, sizeof(b), &saved, sizeof(saved));
  TEST_ASSERT(mr.ht_failed);
  marshall_read_clean(&mr);
  mutex_delete(a.ptr.p_pvoid);
  str_clean(&str);
  marshall_clean(&m);
#endif
}
TEST_CASE_END(marshall_read_cache_pointer_legacy)

TEST_CASE(marshall_read_call_cache)
{
  s_call call = {0};
  s_call call_read = {0};
  s_env env = {0};
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_tag result = {0};
  s_str str = {0};
  test_context("marshall_read_call cache is transient");
  TEST_ASSERT(env_init(&env, 0, NULL));
  TEST_ASSERT(call_init(&call));
  call.ident.sym = sym_1("+");
  call.arguments = list_new_1("[1, 2]");
  TEST_ASSERT(call.arguments);
  TEST_ASSERT(env_eval_call(&env, &call, &result));
  TEST_ASSERT(call.pcallable);
  TEST_ASSERT(marshall_init(&m, BUF_SIZE));
  TEST_ASSERT(marshall_call(&m, false, &call));
  TEST_ASSERT(marshall_to_str(&m, &str));
  TEST_ASSERT(marshall_read_init_str(&mr, &str));
  TEST_ASSERT(marshall_read_call(&mr, false, &call_read));
  TEST_ASSERT(! call_read.pcallable);
  call_clean(&call);
  call_clean(&call_read);
  env_clean(&env);
  marshall_clean(&m);
  marshall_read_clean(&mr);
  str_clean(&str);
  tag_clean(&result);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_call_cache)

TEST_CASE(marshall_read_chunks)
{
  char data[BUF_SIZE];
  s_buf buf = {0};
  s_marshall m = {0};
  s_marshall_read mr = {0};
  p_sym sym;
  p_sym sym_read = NULL;
  TEST_EQ(buf_init(&buf, false, sizeof(data), data), &buf);
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  sym = sym_1("first");
  TEST_EQ(marshall_psym(&m, false, &sym), &m);
  TEST_ASSERT(marshall_to_buf(&m, &buf) > 0);
  sym = sym_1("second");
  TEST_EQ(marshall_psym(&m, false, &sym), &m);
  TEST_ASSERT(marshall_to_buf(&m, &buf) > 0);
  TEST_EQ(marshall_read_init_buf(&mr, &buf), &mr);
  TEST_EQ(marshall_read_header(&mr), &mr);
  TEST_EQ(marshall_read_chunk(&mr), &mr);
  TEST_EQ(marshall_read_psym(&mr, false, &sym_read), &mr);
  TEST_EQ(sym_read, sym_1("first"));
  TEST_EQ(mr.ht.count, 1);
  TEST_EQ(marshall_read_chunk_reset(&mr), &mr);
  TEST_EQ(marshall_read_header(&mr), &mr);
  TEST_EQ(marshall_read_chunk(&mr), &mr);
  TEST_EQ(marshall_read_psym(&mr, false, &sym_read), &mr);
  TEST_EQ(sym_read, sym_1("second"));
  TEST_EQ(mr.ht.count, 1);
  TEST_EQ(marshall_read_chunk_reset(&mr), &mr);
  marshall_read_clean(&mr);
  marshall_clean(&m);
  buf_clean(&buf);
}
TEST_CASE_END(marshall_read_chunks)

TEST_CASE(marshall_read_facts)
{
  s_facts facts = {0};
  s_facts facts_read = {0};
  uw id;
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_str str = {0};
  test_context("marshall_read_facts() V3 round trip without log");
  TEST_EQ(facts_init(&facts), &facts);
  facts.next_id = 42;
  id = facts.id;
  TEST_EQ(marshall_init(&m, 1024 * 1024), &m);
  TEST_EQ(marshall_facts(&m, false, &facts), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_facts(&mr, false, &facts_read), &mr);
  TEST_EQ(facts_read.id, id);
  TEST_EQ(facts_read.next_id, 42);
  TEST_EQ(facts_read.log, NULL);
  facts_clean(&facts_read);
  marshall_read_clean(&mr);
  str_clean(&str);
  marshall_clean(&m);
  facts_clean(&facts);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_facts)

TEST_CASE(marshall_read_facts_log)
{
  const s_str after_dump_path =
    STR("marshall_read_facts_log.after-dump.facts");
  s_fact fact = {0};
  s_facts facts = {0};
  s_facts facts_read = {0};
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_tag object = {0};
  const s_str path = STR("marshall_read_facts_log.facts");
  s_tag predicate = {0};
  s_str str = {0};
  s_tag subject = {0};
  test_context("marshall_read_facts() V3 round trip with log");
  unlink(path.ptr.p_pchar);
  unlink(after_dump_path.ptr.p_pchar);
  TEST_EQ(facts_init(&facts), &facts);
  TEST_ASSERT(facts_save_binary_file(&facts, &path) >= 0);
  TEST_ASSERT(facts_open_file_after_dump(&facts, &after_dump_path) >= 0);
  TEST_EQ(marshall_init(&m, 1024 * 1024), &m);
  TEST_EQ(marshall_facts(&m, false, &facts), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  TEST_EQ(tag_init_1(&subject, ":after_dump_subject"), &subject);
  TEST_EQ(tag_init_1(&predicate, ":after_dump_predicate"), &predicate);
  TEST_EQ(tag_init_1(&object, ":after_dump_object"), &object);
  TEST_EQ(fact_init(&fact, &subject, &predicate, &object), &fact);
  TEST_ASSERT(facts_add_fact(&facts, &fact));
  tag_clean(&object);
  tag_clean(&predicate);
  tag_clean(&subject);
  facts_clean(&facts);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_facts(&mr, false, &facts_read), &mr);
  TEST_ASSERT(facts_read.log);
  TEST_EQ(facts_read.facts.count, 1);
  TEST_STR_EQ(facts_read.log->path, path);
  TEST_STR_EQ(facts_read.log->after_dump_path, after_dump_path);
  facts_clean(&facts_read);
  marshall_read_clean(&mr);
  str_clean(&str);
  marshall_clean(&m);
  unlink(path.ptr.p_pchar);
  unlink(after_dump_path.ptr.p_pchar);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_facts_log)

TEST_CASE(marshall_read_plist)
{
  s_marshall_read mr = {0};
  s_tag expected = {0};
  s_tag test_tag = {0};
  const char test_str[] = "KC3MARSH"
    "\x02\x00\x00\x00\x00\x00\x00\x00"
    "\x15\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x00\x00\x00\x00\x00\x00\x00\x00"
    "\x10\x00\x10\x00\x00\x00\x00\x00"
    "\x00\x00\x00";
  TEST_EQ(marshall_read_init_1(&mr, test_str, sizeof(test_str) - 1),
          &mr);
  TEST_EQ(marshall_read_plist(&mr, false, &test_tag.data.td_plist), &mr);
  test_tag.type = TAG_PLIST;
  tag_init_plist_1(&expected, "[0, 1]");
  TAG_TEST_EQ(&test_tag, &expected);
  tag_clean(&test_tag);
  tag_clean(&expected);
}
TEST_CASE_END(marshall_read_plist)

TEST_CASE(marshall_read_set__tag)
{
  uw h;
  uw i;
  const s_set_item__tag *item;
  const s_set_item__tag *item2;
  s_marshall m = {0};
  s_marshall_read mr = {0};
  const char *p[] = {
    "false",
    "true",
    "'a'",
    "\"abc\"",
    ":a",
    "A.b(c, d)",
    "[1, 2, 3]",
    "{:a, :b}",
    "0x10000000000000000",
    "-1",
    NULL
  };
  s_set__tag set = {0};
  s_set__tag set2 = {0};
  s_set_item__tag *set_item;
  s_str str = {0};
  s_tag tag = {0};
  TEST_EQ(set_init__tag(&set, 8), &set);
  tag_init(&tag);
  i = 0;
  while (p[i]) {
    TEST_ASSERT(tag_1(&tag, p[i]));
    TEST_ASSERT((set_item = set_add__tag(&set, &tag)));
    set_item->usage = i + 1;
    i++;
  }
  tag_clean(&tag);
  TEST_EQ(set.count, i);
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_set__tag(&m, false, &set), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_set__tag(&mr, false, &set2), &mr);
  TEST_EQ(set2.max, set.max);
  TEST_EQ(set2.count, set.count);
  TEST_EQ(set2.collisions, set.collisions);
  h = 0;
  while (h < set.max) {
    item = set.items[h];
    item2 = set2.items[h];
    while (item && item2) {
      TEST_EQ(item2->hash, item->hash);
      TEST_EQ(item2->usage, item->usage);
      TAG_TEST_EQ(&item2->data, &item->data);
      item = item->next;
      item2 = item2->next;
    }
    TEST_ASSERT(! item);
    TEST_ASSERT(! item2);
    h++;
  }
  marshall_read_clean(&mr);
  marshall_clean(&m);
  set_clean__tag(&set2);
  set_clean__tag(&set);
  str_clean(&str);
}
TEST_CASE_END(marshall_read_set__tag)

TEST_CASE(marshall_read_var)
{
  s_env *env;
  uw id;
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_str str = {0};
  s_var var = {0};
  s_var var_read = {0};
  env = env_global();
  TEST_EQ(var_init(&var, &g_sym_U8), &var);
  id = env->next_var_id + 10;
  var.id = id;
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_var(&m, false, &var), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_var(&mr, false, &var_read), &mr);
  TEST_EQ(var_read.id, id);
  TEST_EQ(env->next_var_id, id + 1);
  var_clean(&var_read);
  var_clean(&var);
  str_clean(&str);
  marshall_read_clean(&mr);
  marshall_clean(&m);
}
TEST_CASE_END(marshall_read_var)

  TEST_CASE(marshall_read_tag)
{
  // u8
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x11"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x00",
                         "0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x11"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x01",
                         "1");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x11"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x02",
                         "2");
  // u16
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1a\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x12"
                         "\x5f\x4b\x43\x33\x55\x31\x36\x5f"
                         "\x00\x01",
                         "256");
  // u32
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x13"
                         "\x5f\x4b\x43\x33\x55\x33\x32\x5f"
                         "\x00\x00\x01\x00",
                         "65536");
  // u64
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x14"
                         "\x5f\x4b\x43\x33\x55\x36\x34\x5f"
                         "\xA3\x8D#\xD6\xE2S\x00\x00",
                         "92233720368547");
  // s8:
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x18\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x10"
                         "\x5f\x4b\x43\x33\x53\x38\x5f\xff",
                         "-1");
  // s16:
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1a\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x0f"
                         "\x5f\x4b\x43\x33\x53\x31\x36\x5f"
                         "\x7f\xff",
                         "-129");
  // s32:
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x0e"
                         "\x5f\x4b\x43\x33\x53\x33\x32\x5f"
                         "\x00\x00\x00\x80",
                         "-2147483648");
  // s64
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x0d"
                         "\x5f\x4b\x43\x33\x53\x36\x34\x5f"
                         "\x5d\xf2\xa6\x63\x7e\xd0\xff\xff",
                         "-52233720368547");
  // f32
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x05"
                         "\x5f\x4b\x43\x33\x46\x33\x32\x5f"
                         "\xdb\x0f\x49\x40",
                         "(F32) 3.1415927");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x05"
                         "\x5f\x4b\x43\x33\x46\x33\x32\x5f"
                         "\x00\x00\x80\x3F",
                         "(F32) 1.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1c\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x05"
                         "\x5f\x4b\x43\x33\x46\x33\x32\x5f"
                         "\xFF\xFF\xFF\xFF",
                         "(F32) 4294967295.0");
  // f64
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x06"
                         "\x5f\x4b\x43\x33\x46\x36\x34\x5f"
                         "\x18\x2d\x44\x54\xfb\x21\x09\x40",
                         "(F64) 3.141592653589793");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x06"
                         "\x5f\x4b\x43\x33\x46\x36\x34\x5f"
                         "\x00\x00\x00\x00\x00\x00\xF0\x3F",
                         "(F64) 1.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x06"
                         "\x5f\x4b\x43\x33\x46\x36\x34\x5f"
                         "\x00\x00\x00\x00\x00\x00\xF0\x41",
                         "(F64) 4294967296.0");
#if HAVE_F80
  // f80
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x29\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x07"
                         "\x5f\x4b\x43\x33\x46\x38\x30\x5f"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00",
                         "(F80) 0.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x29\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x07"
                         "\x5f\x4b\x43\x33\x46\x38\x30\x5f"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x80\xff\x3f\x00\x00\x00\x00\x00"
                         "\x00",
                         "(F80) 1.0");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x29\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x07"
                         "\x5f\x4b\x43\x33\x46\x38\x30\x5f"
                         "\x5f\x32\xc2\x68\x21\xa2\xda\x0f"
                         "\xc9\x00\x40\x00\x00\x00\x00\x00"
                         "\x00",
                         "(F80) 3.1415926535897932384"
                         "28841971");
#endif
  // str
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0D\x00\x00\x00\x00\x00\x00\x00"
                         "\x23\x0B\x68\x65\x6C\x6C\x6F\x20"
                         "\x77\x6F\x72\x6C\x64",
                         "\"hello world\"");
  */

  // list
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x16\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x01\x16\x23\x00\x00\x00\x00"
                         "\x00\x00\x00\x11\x02\x16\x2e\x00"
                         "\x00\x00\x00\x00\x00\x00\x16\x20"
                         "\x00\x00\x00\x00\x00\x00\x00",
                         "[1, 2]");
  */
  // Do block
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x10\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x03\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x11\x01\x11\x02\x11\x03",
                         "do\n"
                         "  1\n"
                         "  2\n"
                         "  3\n"
                         "end");
  */
  // Tuple
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x16\x00\x00\x00\x00\x00\x00\x00"
                         "\x16\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x03\x16\x23\x00\x00\x00\x00"
                         "\x00\x00\x00\x11\x04\x16\x2e\x00"
                         "\x00\x00\x00\x00\x00\x00\x25\x03"
                         "\x00\x00\x00\x00\x00\x00\x00\x11"
                         "\x01\x11\x02\x16\x20\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "{1, 2, [3, 4]}");
  */
  // psym
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0c\x00\x00\x00\x00\x00\x00\x00"
                         "\x1d\x01\x06\x00\x00\x00\x4d\x6f"
                         "\x64\x75\x6c\x65",
                         "Module");
  */
  // Map
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x1B\x00\x00\x00\x00\x00\x00\x00"
                         "\x17\x02\x00\x00\x00\x00\x00\x00"
                         "\x00\x1D\x01\x01\x00\x00\x00\x61"
                         "\x11\x01\x1D\x01\x01\x00\x00\x00"
                         "\x62\x11\x02", "%{a: 1, b: 2}");
  */
  // Cfn
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x07\x00\x00\x00\x00\x00\x00\x00"
                         "\x6b\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x21\x00\x00\x00\x00\x00\x00"
                         "\x00\x29\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x33\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x03\x00\x00\x00"
                         "\x54\x61\x67\x43\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x07\x00\x00\x00"
                         "\x74\x61\x67\x5f\x61\x64\x64\x03"
                         "\x58\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x03\x00\x00\x00\x54\x61\x67"
                         "\x68\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x03\x00\x00\x00\x54\x61\x67"
                         "\x78\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x06\x00\x00\x00\x52\x65\x73"
                         "\x75\x6c\x74\x18\x20\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "cfn Tag \"tag_add\" (Tag, Tag, Result)");
  */
  // Call
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0e\x00\x00\x00\x00\x00\x00\x00"
                         "\x35\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x09\x00\x00\x00\x64\x65\x66"
                         "\x6d\x6f\x64\x75\x6c\x65\x04\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x02"
                         "\x00\x00\x00\x00\x00\x00\x00\x1d"
                         "\x01\x04\x00\x00\x00\x54\x65\x73"
                         "\x74\x02\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x2e\x00\x00\x00\x00"
                         "\x00\x00\x00",
                         "defmodule Test do end");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x15\x00\x00\x00\x00\x00\x00\x00"
                         "\x2a\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x04\x00\x00\x00\x4c\x69\x73"
                         "\x74\x01\x07\x00\x00\x00\x72\x65"
                         "\x76\x65\x72\x73\x65\x04\x20\x00"
                         "\x00\x00\x00\x00\x00\x00\x29\x00"
                         "\x00\x00\x00\x00\x00\x00\x01\x00"
                         "\x00\x00\x00\x00\x00\x00\x16\x35"
                         "\x00\x00\x00\x00\x00\x00\x00\x35"
                         "\x00\x00\x00\x00\x00\x00\x00",
                         "List.reverse([])");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0c\x00\x00\x00\x00\x00\x00\x00"
                         "\x34\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x01\x00\x00\x00\x3d\x01\x01"
                         "\x00\x00\x00\x61\x04\x20\x00\x00"
                         "\x00\x00\x00\x00\x00\x20\x00\x00"
                         "\x00\x00\x00\x00\x00\x02\x00\x00"
                         "\x00\x00\x00\x00\x00\x27\x26\x00"
                         "\x00\x00\x00\x00\x00\x00\x26\x00"
                         "\x00\x00\x00\x00\x00\x00\x11\x01"
                         "\x2c\x00\x00\x00\x00\x00\x00\x00",
                         "a = 1");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x0b\x00\x00\x00\x00\x00\x00\x00"
                         "\x84\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x23\x00\x00\x00\x00"
                         "\x00\x00\x00\x2b\x00\x00\x00\x00"
                         "\x00\x00\x00\x33\x00\x00\x00\x00"
                         "\x00\x00\x00\x27\x3c\x00\x00\x00"
                         "\x00\x00\x00\x00\x44\x00\x00\x00"
                         "\x00\x00\x00\x00\x01\x01\x00\x00"
                         "\x00\x78\x16\x53\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x00\x00\x00\x00"
                         "\x00\x00\x00\x01\x04\x65\x00\x00"
                         "\x00\x00\x00\x00\x00\x6d\x00\x00"
                         "\x00\x00\x00\x00\x00\x01\x01\x00"
                         "\x00\x00\x2a\x02\x00\x00\x00\x00"
                         "\x00\x00\x00\x27\x84\x00\x00\x00"
                         "\x00\x00\x00\x00\x8c\x00\x00\x00"
                         "\x00\x00\x00\x00\x01\x01\x00\x00"
                         "\x00\x78\x11\x02\x9c\x00\x00\x00"
                         "\x00\x00\x00\x00\x18\x20\x00\x00"
                         "\x00\x00\x00\x00\x00",
                         "fn (x) { x * 2 }");
  */
  // Ratio
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x15\x00\x00\x00\x00\x00\x00\x00"
                         "\x0b\x02\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x02\x02\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x06",
                         "2/6");
  */
  // Quote
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x06\x00\x00\x00\x00\x00\x00\x00"
                         "\x26\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x01\x00\x00\x00\x2b\x22\x04"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x01\x11\x01\x26\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "quote 1 + 1");
  */
  // Var
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x01\x02\x00\x00\x00\x55\x38"
                         "\x00\x21\x20\x00\x00\x00\x00\x00"
                         "\x00\x00",
                         "(U8) ?");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0a\x00\x00\x00\x00\x00\x00\x00"
                         "\x09\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x01\x03\x00\x00\x00\x54\x61"
                         "\x67\x00\x21\x20\x00\x00\x00\x00"
                         "\x00\x00\x00",
                         "(Tag) ?");
  // Complex
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x07\x00\x00\x00\x00\x00\x00\x00"
                         "\x25\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x02\x00\x00\x00\x2b\x69\x04"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x20\x00\x00\x00\x00\x00\x00\x00"
                         "\x02\x00\x00\x00\x00\x00\x00\x00"
                         "\x11\x01\x11\x02\x27\x00\x00\x00"
                         "\x00\x00\x00\x00",
                         "1 +i 2");
  // Unquote
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x03\x00\x00\x00\x00\x00\x00\x00"
                         "\x26\x11\x7b",
                         "unquote(123)");
  */
  // Time
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x48\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x27"
                         "\x5f\x4b\x43\x33\x42\x4f\x4f\x4c"
                         "\x5f\x00\x5f\x4b\x43\x33\x53\x57"
                         "\x5f\x5f\x4b\x43\x33\x53\x36\x34"
                         "\x5f\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x5f\x4b\x43\x33\x53\x57\x5f"
                         "\x5f\x4b\x43\x33\x53\x36\x34\x5f"
                         "\x00\x00\x00\x00\x00\x00\x00\x00",
                         "%Time{}");
  /*
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x12\x00\x00\x00\x00\x00\x00\x00"
                         "\x24\x00\xe8\x03\x00\x00\x00\x00"
                         "\x00\x00\xd0\x07\x00\x00\x00\x00"
                         "\x00\x00",
                         "%Time{tv_sec: 1000, tv_nsec: 2000}");
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x0c\x00\x00\x00\x00\x00\x00\x00"
                         "\x4c\x00\x00\x00\x00\x00\x00\x00"
                         "\x01\x01\x00\x00\x00\x2b\x01\x01"
                         "\x00\x00\x00\x2b\x24\x01\x04\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x20"
                         "\x00\x00\x00\x00\x00\x00\x00\x02"
                         "\x00\x00\x00\x00\x00\x00\x00\x11"
                         "\x01\x11\x01\x26\x00\x00\x00\x00"
                         "\x00\x00\x00\x04\x26\x00\x00\x00"
                         "\x00\x00\x00\x00\x26\x00\x00\x00"
                         "\x00\x00\x00\x00\x02\x00\x00\x00"
                         "\x00\x00\x00\x00\x11\x02\x11\x02"
                         "\x2c\x00\x00\x00\x00\x00\x00\x00",
                         "%Time{tv_sec: 1 + 1, tv_nsec: 2 + 2}");
  */
  // Ptr
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x2f\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x22"
                         "\x5f\x4b\x43\x33\x50\x54\x52\x5f"
                         "\x5f\x4b\x43\x33\x55\x57\x5f\x5f"
                         "\x4b\x43\x33\x55\x36\x34\x5f\x00"
                         "\x00\x00\x00\x00\x00\x00\x00",
                         "(Ptr) 0");
  // PtrFree
  MARSHALL_READ_TEST_TAG("KC3MARSH"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x00\x00\x00\x00\x00\x00\x00\x00"
                         "\x33\x00\x00\x00\x00\x00\x00\x00"
                         "\x5f\x4b\x43\x33\x54\x41\x47\x5f"
                         "\x5f\x4b\x43\x33\x55\x38\x5f\x23"
                         "\x5f\x4b\x43\x33\x50\x54\x52\x46"
                         "\x52\x45\x45\x5f\x5f\x4b\x43\x33"
                         "\x55\x57\x5f\x5f\x4b\x43\x33\x55"
                         "\x36\x34\x5f\x00\x00\x00\x00\x00"
                         "\x00\x00\x00",
                         "(PtrFree) 0");

}
TEST_CASE_END(marshall_read_tag)

TEST_CASE(marshall_read_unquote)
{
  s_marshall m = {0};
  s_marshall_read mr = {0};
  s_str str = {0};
  s_tag tag = {0};
  s_tag tag_read = {0};
  test_context("marshall_read unquote(123) roundtrip");
  TEST_ASSERT(tag_init_1(&tag, "unquote 123"));
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_tag(&m, false, &tag), &m);
  TEST_ASSERT(marshall_to_str(&m, &str));
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(marshall_read_tag(&mr, false, &tag_read), &mr);
  TAG_TEST_EQ(&tag_read, &tag);
  marshall_clean(&m);
  marshall_read_clean(&mr);
  str_clean(&str);
  tag_clean(&tag);
  tag_clean(&tag_read);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_unquote)

#ifdef NDEBUG
static uw m1_patch_max_height (char *data, uw size)
{
  uw i;
  static const char magic[] = "_KC3SKIPLISTFACT_";
  uw patched = 0;
  if (size < 384)
    return 0;
  for (i = 0; i + 384 <= size; i++) {
    if (! memcmp(data + i, magic, sizeof(magic) - 1) &&
        ! memcmp(data + i + 40, "_KC3U8_", 7) &&
        (u8) data[i + 47] == 20) {
      data[i + 47] = 21;
      memcpy(data + i + 368, "_KC3U64_", 8);
      memset(data + i + 376, 0x41, 8);
      patched++;
    }
  }
  return patched;
}

TEST_CASE(marshall_read_skiplist_max_height)
{
  s_facts facts = {0};
  s_facts facts_read = {0};
  s_marshall m = {0};
  s_marshall_read mr = {0};
  uw patched;
  s_str str = {0};
  test_context("M1: forged max_height in _KC3SKIPLISTFACT_");
  TEST_EQ(facts_init(&facts), &facts);
  TEST_EQ(marshall_init(&m, 1024 * 1024), &m);
  TEST_EQ(marshall_facts(&m, false, &facts), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  patched = m1_patch_max_height((char *) str.ptr.p_pchar, str.size);
  fprintf(stderr, "M1 repro: patched %lu _KC3SKIPLISTFACT_ sections\n",
          (unsigned long) patched);
  TEST_ASSERT(patched);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_EQ(facts_init(&facts_read), &facts_read);
  TEST_ASSERT(! marshall_read_facts(&mr, false, &facts_read));
  TEST_EQ(facts_read.facts.count, 0);
  facts_clean(&facts_read);
  marshall_read_clean(&mr);
  marshall_clean(&m);
  str_clean(&str);
  facts_clean(&facts);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_skiplist_max_height)

TEST_CASE(marshall_read_set_item_error)
{
  char *data;
  s_set_item__tag dest = {0};
  uw i;
  s_set_item__tag item = {0};
  s_marshall m = {0};
  static const char magic[] = "_KC3PSETITEMTAG_";
  s_marshall_read mr = {0};
  bool patched = false;
  s_str str = {0};
  test_context("marshall_read_set_item__tag() reports read failure");
  TEST_ASSERT(tag_init_1(&item.data, "\"m6\""));
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_set_item__tag(&m, false, &item), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  data = (char *) str.ptr.p_pchar;
  i = 0;
  while (! patched && i + sizeof(magic) - 1 <= str.size) {
    if (! memcmp(data + i, magic, sizeof(magic) - 1)) {
      data[i + 4] = 'X';
      patched = true;
    }
    i++;
  }
  TEST_ASSERT(patched);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_ASSERT(! marshall_read_set_item__tag(&mr, false, &dest));
  marshall_read_clean(&mr);
  marshall_clean(&m);
  str_clean(&str);
  tag_clean(&item.data);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_set_item_error)
#endif

#ifdef NDEBUG
TEST_CASE(marshall_read_set_max_overflow)
{
  char *data;
  s_set__tag dest = {0};
  uw i;
  static const char magic[] = "_KC3SETTAG_";
  s_marshall m = {0};
  s_marshall_read mr = {0};
  bool patched = false;
  s_set__tag set = {0};
  s_str str = {0};
  test_context("M5: forged set max overflows the bucket array");
  TEST_ASSERT(set_init__tag(&set, 4));
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_set__tag(&m, false, &set), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  data = (char *) str.ptr.p_pchar;
  i = 0;
  while (! patched && i + 80 <= str.size) {
    if (! memcmp(data + i, magic, sizeof(magic) - 1)) {
      memcpy(data + i + 72,
             "\x01\x00\x00\x00\x00\x00\x00\x20", 8);
      patched = true;
    }
    i++;
  }
  TEST_ASSERT(patched);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_ASSERT(! marshall_read_set__tag(&mr, false, &dest));
  marshall_read_clean(&mr);
  marshall_clean(&m);
  str_clean(&str);
  set_clean__tag(&set);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_set_max_overflow)
#endif

#ifdef NDEBUG
static bool f15_uw_at (const char *d, uw size, uw off, uw *dest)
{
  if (off + 23 > size)
    return false;
  if (memcmp(d + off, "_KC3UW_", 7))
    return false;
  if (memcmp(d + off + 7, "_KC3U64_", 8))
    return false;
  memcpy(dest, d + off + 15, 8);
  return true;
}

TEST_CASE(marshall_read_array_size)
{
  char *data;
  s_tag dest = {0};
  uw i;
  s_marshall m = {0};
  s_marshall_read mr = {0};
  bool patched = false;
  s_str str = {0};
  s_tag src = {0};
  s_tag tag = {0};
  uw v[5];
  test_context("F15: forged array size overflows the data buffer");
  TEST_ASSERT(tag_init_1(&src, "(U8[]) {0, 1, 2, 3}"));
  TEST_ASSERT(env_eval_tag(env_global(), &src, &tag));
  TEST_EQ(tag.type, TAG_ARRAY);
  TEST_ASSERT(tag.data.td_array.data);
  TEST_EQ(marshall_init(&m, BUF_SIZE), &m);
  TEST_EQ(marshall_tag(&m, false, &tag), &m);
  TEST_EQ(marshall_to_str(&m, &str), &str);
  data = (char *) str.ptr.p_pchar;
  i = 0;
  while (! patched && i + 115 <= str.size) {
    if (f15_uw_at(data, str.size, i +  0, v + 0) && v[0] == 1 &&
        f15_uw_at(data, str.size, i + 23, v + 1) && v[1] == 4 &&
        f15_uw_at(data, str.size, i + 46, v + 2) && v[2] == 1 &&
        f15_uw_at(data, str.size, i + 69, v + 3) && v[3] == 4 &&
        f15_uw_at(data, str.size, i + 92, v + 4) && v[4] == 4) {
      v[4] = 1;
      memcpy(data + i + 92 + 15, v + 4, 8);
      patched = true;
    }
    i++;
  }
  fprintf(stderr, "F15 repro: patched=%d\n", (int) patched);
  TEST_ASSERT(patched);
  TEST_EQ(marshall_read_init_str(&mr, &str), &mr);
  TEST_ASSERT(! marshall_read_tag(&mr, false, &dest));
  tag_clean(&dest);
  marshall_read_clean(&mr);
  marshall_clean(&m);
  str_clean(&str);
  tag_clean(&tag);
  tag_clean(&src);
  test_context(NULL);
}
TEST_CASE_END(marshall_read_array_size)
#endif
