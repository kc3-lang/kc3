/* c3
 * Copyright 2022,2023 kmx.io <contact@kmx.io>
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
#ifndef TYPES_H
#define TYPES_H

#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <sys/types.h>
#include <ffi.h>
#include <pthread.h>
#include "config.h"
#include "sha1.h"
#include "../libtommath/tommath.h"

/* Basic integer types. */
typedef char          s8;
typedef int16_t       s16;
typedef int32_t       s32;
typedef long          sw;
typedef int64_t       s64;
typedef uint8_t       u8;
typedef uint16_t      u16;
typedef uint32_t      u32;
typedef unsigned long uw;
typedef uint64_t      u64;

#define S8_MAX  ((s8)  (((u8)  1 << (8 * sizeof(s8)  - 1)) - 1))
#define S16_MAX ((s16) (((u16) 1 << (8 * sizeof(s16) - 1)) - 1))
#define S32_MAX ((s32) (((u32) 1 << (8 * sizeof(s32) - 1)) - 1))
#define S64_MAX ((s64) (((u64) 1 << (8 * sizeof(s64) - 1)) - 1))
#define SW_MAX  ((sw)  (((uw)  1 << (8 * sizeof(sw)  - 1)) - 1))

#define S8_MIN  ((s8)  1 << (8 * sizeof(s8)  - 1))
#define S16_MIN ((s16) 1 << (8 * sizeof(s16) - 1))
#define S32_MIN ((s32) 1 << (8 * sizeof(s32) - 1))
#define S64_MIN ((s64) 1 << (8 * sizeof(s64) - 1))
#define SW_MIN  ((sw)  1 << (8 * sizeof(sw)  - 1))

#define U8_MAX  ((u8)  ~ 0)
#define U16_MAX ((u16) ~ 0)
#define U32_MAX ((u32) ~ 0)
#define U64_MAX ((u64) ~ 0)
#define UW_MAX  ((uw)  ~ 0)

/* IEEE 754 floating point numbers. */
typedef float               f32;
typedef double              f64;

/* Boolean : true or false. */
typedef u8 bool;

/* enums */
typedef enum {
  false = 0,
  true = 1
} e_bool;

typedef enum {
  TAG_VOID = 0,
  TAG_ARRAY,
  TAG_BOOL,
  TAG_CALL,
  TAG_CALL_FN,
  TAG_CALL_MACRO,
  TAG_CFN,
  TAG_CHARACTER,
  TAG_F32,
  TAG_F64,
  TAG_FN,
  TAG_IDENT,
  TAG_INTEGER,
  TAG_S64,
  TAG_S32,
  TAG_S16,
  TAG_S8,
  TAG_U8,
  TAG_U16,
  TAG_U32,
  TAG_U64,
  TAG_LIST,
  TAG_PTAG,
  TAG_QUOTE,
  TAG_STR,
  TAG_SYM,
  TAG_TUPLE,
  TAG_VAR
} e_tag_type;

/* structs */
typedef struct arg                     s_arg;
typedef struct array                   s_array;
typedef struct array_dimension         s_array_dimension;
typedef struct binding                 s_binding;
typedef struct buf                     s_buf;
typedef struct buf_save                s_buf_save;
typedef struct call                    s_call;
typedef struct cfn                     s_cfn;
typedef struct env                     s_env;
typedef struct error_handler           s_error_handler;
typedef struct fact                    s_fact;
typedef struct fact_w                  s_fact_w;
typedef struct facts                   s_facts;
typedef struct facts_cursor            s_facts_cursor;
typedef struct facts_spec_cursor       s_facts_spec_cursor;
typedef struct facts_with_cursor       s_facts_with_cursor;
typedef struct facts_with_cursor_level s_facts_with_cursor_level;
typedef struct float_                  s_float;
typedef struct fn                      s_fn;
typedef struct frame                   s_frame;
typedef struct ident                   s_ident;
typedef struct integer                 s_integer;
typedef struct list                    s_list;
typedef struct list                    s_list_map;
typedef struct log                     s_log;
typedef struct module                  s_module;
typedef struct quote                   s_quote;
typedef struct str                     s_str;
typedef struct struct_                 s_struct;
typedef struct sym                     s_sym;
typedef struct sym_list                s_sym_list;
typedef struct tag                     s_tag;
typedef struct tuple                   s_tuple;
typedef struct type_list               s_type_list;
typedef struct unwind_protect          s_unwind_protect;

/* unions */
typedef union ptr      u_ptr;
typedef union ptr_w    u_ptr_w;
typedef union tag_data u_tag_data;
typedef union tag_type u_tag_type;

/* typedefs */
typedef s32            character;
typedef s_tag      **p_facts_spec;
typedef s_tag       *t_facts_spec[];
typedef SHA1_CTX     t_hash;
typedef const s_tag *p_tag;
typedef u64          t_skiplist_height;

/* function typedefs */
typedef sw (* f_buf_parse) (s_buf *buf, void *dest);

#define CHARACTER_MAX S32_MAX
#define SKIPLIST_HEIGHT_MAX U64_MAX

/* 1 */
struct array_dimension {
  uw count;
  uw item_size;
};

struct buf_save {
  s_buf_save *next;
  uw rpos;
  uw wpos;
};

struct fact {
  const s_tag *subject;
  const s_tag *predicate;
  const s_tag *object;
  uw id; /* serial id */
};

struct fact_w {
  s_tag *subject;
  s_tag *predicate;
  s_tag *object;
  uw id; /* serial id */
};

struct float_ {
  sw integer;
  sw bit_shift;
};

struct frame {
  s_binding *bindings;
  s_frame *next;
};

struct fn {
  uw arity;
  s_list *pattern;
  s_list *algo;
  s_fn *next_clause;
};

struct module {
  const s_sym *name;
  s_facts *facts;
};

union ptr {
  const void *p;
  const s8   *ps8;
  const u8   *pu8;
};

union ptr_w {
  void *p;
  s8   *ps8;
  u8   *pu8;
};

struct quote {
  s_tag *tag;
};

struct struct_ {
  void *data;
  uw count;
  s_module *module;
  s_ident *type;
};

struct sym_list {
  s_sym *sym;
  s_sym_list *next;
};

union tag_type {
  e_tag_type type;
};

struct tuple {
  uw count;
  s_tag *tag;
};

struct type_list {
  e_tag_type type;
  s_type_list *next;
};

struct unwind_protect {
  jmp_buf buf;
  jmp_buf *jmp;
  s_unwind_protect *next;
};

/* 2 */
struct array {
  uw dimension;
  e_tag_type type;
  s_array_dimension *dimensions;
  uw size;
  void *data;
};

struct binding {
  const s_sym *name;
  const s_tag *value;
  s_binding *next;
};

struct buf {
  sw          column;
  sw        (*flush) (s_buf *buf);
  bool        free;
  sw          line;
  u_ptr_w     ptr;
  sw        (*refill) (s_buf *buf);
  uw          rpos;
  s_buf_save *save;
  sw        (*seek) (s_buf *buf, sw offset, u8 whence);
  uw          size;
  void       *user_ptr;
  u64         wpos;
};

struct facts_spec_cursor {
  p_facts_spec spec;
  const s_tag *subject;
  uw pos;
};

struct ident {
  /*const s_module *module;*/
  const s_sym *module_name;
  const s_sym *sym;
};

struct integer {
  mp_int mp_int;
};

struct str {
  u_ptr_w free;        /**< Pointer to free or NULL. */
  uw      size;        /**< Size in bytes. */
  u_ptr   ptr;         /**< Pointer to memory. */
};

/* 3 */
struct arg {
  const s_sym *name;
  s_ident type;
  s_arg *next;
};

struct call {
  /* key */
  s_ident ident;
  s_list *arguments;
  s_list_map *keyword;
  /* value */
  s_cfn *cfn;
  s_fn *fn;
};

struct cfn {
  const s_sym *name;
  union {
    void (*f) (void);
    void *p;
  } ptr;
  u8 arity;
  const s_sym *result_type;
  bool arg_result;
  s_list *arg_types;
  ffi_cif cif;
};

struct log {
  s_buf  buf;
  u64    count;
  t_hash hash;
};

struct sym {
  s_str str;
};

union tag_data {
  s_array      array;
  bool         bool;
  s_call       call;
  s_cfn        cfn;
  character    character;
  f32          f32;
  f64          f64;
  s_fn        *fn;
  s_ident      ident;
  s_integer    integer;
  s_list      *list;
  p_tag        ptag;
  s_quote      quote;
  s_str        str;
  const s_sym *sym;
  s8           s8;
  s16          s16;
  s32          s32;
  s64          s64;
  s_tuple      tuple;
  u8           u8;
  u16          u16;
  u32          u32;
  u64          u64;
};

/* 4 */
struct tag {
  u_tag_type type;
  u_tag_data data;
};

/* 5 */
struct error_handler
{
  s_list *backtrace;
  jmp_buf jmp_buf;
  s_error_handler *next;
  s_tag tag;
};

struct list {
  s_tag tag;
  s_tag next;
};

#define TYPEDEF_SET_ITEM(name, type)                                   \
  typedef struct set_item__##name s_set_item__##name;                  \
                                                                       \
  struct set_item__##name {                                            \
    type data;                                                         \
    uw hash;                                                           \
    s_set_item__##name *next;                                          \
    uw usage;                                                          \
  }

TYPEDEF_SET_ITEM(tag, s_tag);
TYPEDEF_SET_ITEM(fact, s_fact);

#define TYPEDEF_SET(name, type)                                        \
  typedef struct set__##name {                                         \
    uw collisions;                                                     \
    uw count;                                                          \
    s_set_item__##name **items;                                        \
    uw max;                                                            \
  } s_set__##name

TYPEDEF_SET(tag, s_tag);
TYPEDEF_SET(fact, s_fact);

#define TYPEDEF_SET_CURSOR(name)                                       \
  typedef struct set_cursor__##name {                                  \
    s_set__##name *set;                                                \
    uw i;                                                          \
    s_set_item__##name *item;                                          \
    uw count;                                                      \
  } s_set_cursor__##name

TYPEDEF_SET_CURSOR(tag);
TYPEDEF_SET_CURSOR(fact);

#define TYPEDEF_SKIPLIST_NODE(name, type)                              \
  typedef struct skiplist_node__##name {                               \
    type name;                                                         \
    u8 height;                                                         \
  } s_skiplist_node__##name

TYPEDEF_SKIPLIST_NODE(fact, s_fact *);

#define TYPEDEF_SKIPLIST(name, type)                                   \
  typedef struct skiplist__##name {                                    \
    s_skiplist_node__##name *head;                                     \
    s8 (*compare) (const type a, const type b);                        \
    uw length;                                                         \
    u8 max_height;                                                     \
  } s_skiplist__##name

TYPEDEF_SKIPLIST(fact, s_fact *);
  
/* 5 */
struct facts {
  s_set__tag        tags;
  s_set__fact       facts;
  s_skiplist__fact *index_spo;
  s_skiplist__fact *index_pos;
  s_skiplist__fact *index_osp;
  s_log            *log;
  pthread_rwlock_t  rwlock;
  sw                rwlock_count;
  pthread_t         rwlock_thread;
  uw                next_id;
};

struct facts_cursor {
  s_facts *facts;
  s_skiplist__fact *index;
  s_skiplist_node__fact *node;
  s_fact start;
  s_fact end;
  s_tag *var_subject;
  s_tag *var_predicate;
  s_tag *var_object;
  pthread_mutex_t mutex;
};

/* 6 */
struct env {
  s_list           *backtrace;
  const s_module   *current_module;
  s_module          c3_module;
  s_buf             err;
  s_error_handler  *error_handler;
  s_facts           facts;
  s_frame          *frame;
  s_buf             in;
  s_str             module_path;
  s_buf             out;
  s_unwind_protect *unwind_protect;
};

struct facts_with_cursor_level {
  s_facts_cursor cursor;
  s_fact *fact;
  p_facts_spec spec;
};

/* 7 */
struct facts_with_cursor {
  s_facts *facts;
  s_binding *bindings;
  size_t facts_count;
  s_facts_with_cursor_level *levels;
  size_t level;
  p_facts_spec spec;
  pthread_mutex_t mutex;
};

#endif /* TYPES_H */
