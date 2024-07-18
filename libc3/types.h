/* c3
 * Copyright 2022-2024 kmx.io <contact@kmx.io>
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
#ifndef LIBC3_TYPES_H
#define LIBC3_TYPES_H

#include <ffi.h>
#include <limits.h>
#include <setjmp.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include "config.h"
#include "sha1.h"
#include "../libtommath/tommath.h"

#ifdef bool
# undef bool
#endif
#ifdef false
# undef false
#endif
#ifdef true
# undef true
#endif

#ifdef __APPLE__
# define st_mtim st_mtimespec
#endif

/* Basic integer types. */
typedef int8_t             s8;
typedef int16_t            s16;
typedef int32_t            s32;
typedef int64_t            s64;
typedef long long          s128;
typedef long               sw;
typedef uint8_t            u8;
typedef uint16_t           u16;
typedef uint32_t           u32;
typedef uint64_t           u64;
typedef unsigned long long u128;
typedef unsigned long      uw;

#ifdef SW_MAX
#undef SW_MAX
#endif

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
typedef float       f32;
typedef double      f64;
typedef long double f128;

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
  TAG_BLOCK,
  TAG_BOOL,
  TAG_CALL,
  TAG_CFN,
  TAG_CHARACTER,
  //TAG_COMPLEX,
  TAG_F32,
  TAG_F64,
  TAG_F128,
  TAG_FACT,
  TAG_FN,
  TAG_INTEGER,
  TAG_RATIO,
  TAG_SW,
  TAG_S64,
  TAG_S32,
  TAG_S16,
  TAG_S8,
  TAG_U8,
  TAG_U16,
  TAG_U32,
  TAG_U64,
  TAG_UW,
  TAG_LIST,
  TAG_MAP,
  TAG_PTAG,
  TAG_PTR,
  TAG_PTR_FREE,
  TAG_QUOTE,
  TAG_STR,
  TAG_STRUCT,
  TAG_STRUCT_TYPE,
  TAG_SYM,
  TAG_TUPLE,
  TAG_UNQUOTE,
  TAG_VAR,
  TAG_IDENT
} e_tag_type;

/* structs */
typedef struct arg                     s_arg;
typedef struct array                   s_array;
typedef struct array_dimension         s_array_dimension;
typedef struct binding                 s_binding;
typedef struct block                   s_block;
typedef struct buf                     s_buf;
typedef struct buf_save                s_buf_save;
typedef struct call                    s_call;
typedef struct cfn                     s_cfn;
typedef struct complex                 s_complex;
typedef struct env                     s_env;
typedef struct error_handler           s_error_handler;
typedef struct fact                    s_fact;
typedef struct fact_w                  s_fact_w;
typedef struct facts                   s_facts;
typedef struct facts_cursor            s_facts_cursor;
typedef struct facts_spec_cursor       s_facts_spec_cursor;
typedef struct facts_with_cursor       s_facts_with_cursor;
typedef struct facts_with_cursor_level s_facts_with_cursor_level;
typedef struct float_s                 s_float;
typedef struct fn                      s_fn;
typedef struct fn_clause               s_fn_clause;
typedef struct frame                   s_frame;
typedef struct ident                   s_ident;
typedef struct integer                 s_integer;
typedef struct integer_fraction        s_integer_fraction;
typedef struct list                    s_list;
typedef struct fact_list               s_fact_list;
typedef struct list                    s_list_map;
typedef struct log                     s_log;
typedef struct map                     s_map;
typedef struct queue                   s_queue;
typedef struct quote                   s_quote;
typedef struct ratio                   s_ratio;
typedef struct sequence                s_sequence;
typedef struct str                     s_str;
typedef struct struct_                 s_struct;
typedef struct struct_type             s_struct_type;
typedef struct sym                     s_sym;
typedef struct sym_list                s_sym_list;
typedef struct tag                     s_tag;
typedef struct tag_type_list           s_tag_type_list;
typedef struct timespec                s_time;
typedef struct tuple                   s_tuple;
typedef struct type                    s_type;
typedef struct unquote                 s_unquote;
typedef struct unwind_protect          s_unwind_protect;

/* unions */
typedef union ptr_     u_ptr;
typedef union ptr_w    u_ptr_w;
typedef union tag_data u_tag_data;
typedef union tag_type u_tag_type;

/* typedefs */
typedef u32            character;
typedef s_tag      **p_facts_spec;
typedef s_tag       *t_facts_spec[];
typedef SHA1_CTX     t_hash;
typedef const s_tag *p_tag;
typedef u64          t_skiplist_height;

/* function typedefs */
typedef void (* f_clean) (void *x);
typedef bool (* f_sequence) (s_sequence *seq);
typedef bool (* f_sequence_button) (s_sequence *seq, u8 button, sw x,
                                    sw y);

#define CHARACTER_MAX S32_MAX
#define SKIPLIST_HEIGHT_MAX U64_MAX

/* 1 */

struct array_dimension {
  uw count;
  uw item_size;
};

struct block {
  uw count;
  s_tag *tag;
  bool short_form;
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

struct fn_clause {
  uw arity;
  s_list *pattern;
  s_block algo;
  s_fn_clause *next_clause;
};

struct frame {
  s_binding *bindings;
  s_frame *next;
};

struct fact_list {
  s_fact *fact;
  s_fact_list *next;
};

struct map {
  uw count;
  s_tag *key; /* sorted (see tag_compare) */
  s_tag *value;
};

union ptr_ {
  const void *p;
  const char *pchar;
  const s8 *ps8;
  const u8 *pu8;
};

union ptr_w {
  void *p;
  char *pchar;
  s8 *ps8;
  u8 *pu8;
};

struct queue {
  uw count;
  s_list *head;
  s_list *tail;
};

struct quote {
  s_tag *tag;
};

struct struct_ {
  void *data;
  s_tag *tag;
  bool free_data;
  const s_struct_type *type;
};

struct sym_list {
  const s_sym *sym;
  s_sym *free_sym;
  s_sym_list *next;
};

struct tag_type_list {
  e_tag_type type;
  s_tag_type_list *next;
};

struct tuple {
  uw count;
  s_tag *tag;
};

struct unquote {
  s_tag *tag;
};

struct unwind_protect {
  jmp_buf buf;
  jmp_buf *jmp;
  s_unwind_protect *next;
};

/* 2 */

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
  uw          wpos;
};

struct facts_spec_cursor {
  p_facts_spec spec;
  const s_tag *subject;
  uw pos;
};

struct fn {
  s_fn_clause *clauses;
  bool macro;
  bool special_operator;
};

struct ident {
  const s_sym *module;
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

struct struct_type {
  const s_sym *module;
  s_map map;
  bool must_clean;
  uw *offset;
  uw size;
  f_clean clean;
};

/* 3 */

struct call {
  /* key */
  s_ident ident;
  s_list *arguments;
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
  bool macro;
  bool special_operator;
};

struct log {
  s_buf  buf;
  u64    count;
  t_hash hash;
};

struct ratio {
  s_integer numerator;
  s_integer denominator;
};

struct sym {
  s_str str;
};

struct type {
  s_ident ident;
};

/* 4 */

struct array {
  uw count;
  uw dimension;
  s_array_dimension *dimensions;
  void *data;
  void *free_data;
  uw size;
  s_tag *tags;
  const s_sym *array_type;
  const s_sym *element_type;
};

/* 5 */

union tag_data {
  s_array       array;
  s_block       block;
  bool          bool;
  s_call        call;
  s_cfn         cfn;
  character     character;
  s_complex    *complex;
  f32           f32;
  f64           f64;
  f128          f128;
  s_fact        fact;
  s_fn          fn;
  s_ident       ident;
  s_integer     integer;
  s_list       *list;
  s_map         map;
  p_tag         ptag;
  u_ptr_w       ptr;
  u_ptr_w       ptr_free;
  s_quote       quote;
  s_ratio       ratio;
  s_str         str;
  s_struct      struct_;
  s_struct_type struct_type;
  const s_sym  *sym;
  s8            s8;
  s16           s16;
  s32           s32;
  s64           s64;
  sw            sw;
  s_tuple       tuple;
  u8            u8;
  u16           u16;
  u32           u32;
  u64           u64;
  s_unquote     unquote;
  uw            uw;
};

/* 6 */

struct tag {
  e_tag_type type;
  u_tag_data data;
};

/* 7 */

struct arg {
  const s_sym *name;
  s_type type;
  s_arg *next;
};

struct binding {
  const s_sym *name;
  s_tag value;
  s_binding *next;
};

struct complex {
  s_tag x;
  s_tag y;
};

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

struct sequence {
  s_tag tag;
  f64 dt;
  f64 duration;
  u64 frame;
  f64 t;
  s_time t0;
  const char *title;
  void *window;
  f_sequence load;
  f_sequence render;
  f_sequence unload;
  f_sequence_button button;
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

/* 8 */

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

/* 9 */

struct env {
  sw                argc;
  char            **argv;
  s_str             argv0_dir;
  s_list           *backtrace;
  const s_sym      *current_module;
  s_buf             err;
  s_error_handler  *error_handler;
  s_facts           facts;
  s_frame          *frame;
  s_frame           global_frame;
  s_buf             in;
  s_str             module_path;
  s_buf             out;
  s_list           *path;
  uw                quote_level;
  uw                unquote_level;
  s_unwind_protect *unwind_protect;
};

struct facts_with_cursor_level {
  s_facts_cursor cursor;
  s_fact *fact;
  p_facts_spec spec;
};

/* 10 */

struct facts_with_cursor {
  s_facts *facts;
  s_binding *bindings;
  uw facts_count;
  s_facts_with_cursor_level *levels;
  uw level;
  p_facts_spec spec;
  pthread_mutex_t mutex;
};

#endif /* LIBC3_TYPES_H */
