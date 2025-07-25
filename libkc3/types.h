/* kc3
 * Copyright from 2022 to 2025 kmx.io <contact@kmx.io>
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
#ifndef LIBKC3_TYPES_H
#define LIBKC3_TYPES_H

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

#ifdef __cplusplus
extern "C" {
#else

#ifdef bool
# undef bool
#endif
#ifdef false
# undef false
#endif
#ifdef true
# undef true
#endif

#ifndef thread_local
# if __STDC_VERSION__ >= 201112 && !defined __STDC_NO_THREADS__
#  define thread_local _Thread_local
# elif defined _WIN32 && (defined _MSC_VER || \
                          defined __ICL ||    \
                          defined __DMC__ ||  \
                          defined __BORLANDC__ )
#  define thread_local __declspec(thread)
/* note that ICC (linux) and Clang are covered by __GNUC__ */
# elif defined __GNUC__ ||   \
       defined __SUNPRO_C || \
       defined __hpux ||     \
       defined __xlC__
#  define thread_local __thread
# else
#  error "Cannot define thread_local"
# endif
#endif

#endif /* __cplusplus */

/* Basic integer types. */
typedef int8_t             s8;
typedef int16_t            s16;
typedef int32_t            s32;
typedef int64_t            s64;
//XXX (32 bits) typedef long long          s128;
typedef intptr_t           sw;  // signed word
typedef uint8_t            u8;
typedef uint16_t           u16;
typedef uint32_t           u32;
typedef uint64_t           u64;
//XXX (32 bits) typedef unsigned long long u128;
typedef uintptr_t          uw;  // unsigned word

#ifdef SW_MAX
#undef SW_MAX
#endif

#define S8_MAX  ((s8)  (((u8)  1 << (8 * sizeof(s8)  - 1)) - 1))
#define S16_MAX ((s16) (((u16) 1 << (8 * sizeof(s16) - 1)) - 1))
#define S32_MAX ((s32) (((u32) 1 << (8 * sizeof(s32) - 1)) - 1))
#define S64_MAX ((s64) (((u64) 1 << (8 * sizeof(s64) - 1)) - 1))
#define SW_MAX  ((sw)  (((uw)  1 << (8 * sizeof(sw)  - 1)) - 1))

#define S8_MIN  ((s8)  ((u8)  1 << (8 * sizeof(s8)  - 1)))
#define S16_MIN ((s16) ((u16) 1 << (8 * sizeof(s16) - 1)))
#define S32_MIN ((s32) ((u32) 1 << (8 * sizeof(s32) - 1)))
#define S64_MIN ((s64) ((u64) 1 << (8 * sizeof(s64) - 1)))
#define SW_MIN  ((sw)  ((uw)  1 << (8 * sizeof(sw)  - 1)))

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
#ifdef __cplusplus
typedef u8 BOOL;
#else
typedef u8 bool;
#endif

/* enums */

#ifndef __cplusplus

typedef enum {
  false = 0,
  true = 1
} e_bool;

#endif /* __cplusplus */

typedef enum {
  CALLABLE_VOID = 0,
  CALLABLE_CFN,
  CALLABLE_FN
} e_callable_type;

typedef enum {
  FACT_ACTION_ADD,
  FACT_ACTION_REMOVE,
  FACT_ACTION_REPLACE
} e_fact_action;

typedef enum {
  OP_ASSOCIATIVITY_NONE = 0,
  OP_ASSOCIATIVITY_LEFT = 1,
  OP_ASSOCIATIVITY_RIGHT = 2
} e_op_associativity;

typedef enum {
  TAG_VOID         =  0, // \x00
  TAG_ARRAY        =  1, // \x01
  TAG_DO_BLOCK     =  2, // \x02
  TAG_BOOL         =  3, // \x03
  TAG_CALL         =  4, // \x04
  TAG_CHARACTER    =  5, // \x05
  TAG_F32          =  6, // \x06
  TAG_F64          =  7, // \x07
  TAG_F128         =  8, // \x08
  TAG_FACT         =  9, // \x09
  TAG_INTEGER      = 10, // \x0A
  TAG_RATIO        = 11, // \x0B
  TAG_SW           = 12, // \x0C
  TAG_S64          = 13, // \x0D
  TAG_S32          = 14, // \x0E
  TAG_S16          = 15, // \x0F
  TAG_S8           = 16, // \x10
  TAG_U8           = 17, // \x11
  TAG_U16          = 18, // \x12
  TAG_U32          = 19, // \x13
  TAG_U64          = 20, // \x14
  TAG_UW           = 21, // \x15
  TAG_PLIST        = 22, // \x16
  TAG_MAP          = 23, // \x17
  TAG_PCALLABLE    = 24, // \x18
  TAG_PCOMPLEX     = 25, // \x19
  TAG_PCOW         = 26, // \x1A
  TAG_PSTRUCT      = 27, // \x1B
  TAG_PSTRUCT_TYPE = 28, // \x1C
  TAG_PSYM         = 29, // \x1D
  TAG_PTAG         = 30, // \x1E
  TAG_PTR          = 31, // \x1F
  TAG_PTR_FREE     = 32, // \x20
  TAG_PVAR         = 33, // \x21
  TAG_QUOTE        = 34, // \x22
  TAG_STR          = 35, // \x23
  TAG_TIME         = 36, // \x24
  TAG_TUPLE        = 37, // \x25
  TAG_UNQUOTE      = 38, // \x26
  TAG_IDENT        = 39  // \x27
} e_tag_type;


/* structs */
typedef struct arg                     s_arg;
typedef struct array                   s_array;
typedef struct array_dimension         s_array_dimension;
typedef struct binding                 s_binding;
typedef struct block                   s_block;
typedef struct buf                     s_buf;
typedef struct buf_fd                  s_buf_fd;
typedef struct buf_rw                  s_buf_rw;
typedef struct buf_save                s_buf_save;
typedef struct call                    s_call;
typedef struct callable                s_callable;
typedef struct cfn                     s_cfn;
typedef struct complex                 s_complex;
typedef struct cow                     s_cow;
typedef struct do_block                s_do_block;
typedef struct env                     s_env;
typedef struct error_handler           s_error_handler;
typedef struct fact                    s_fact;
typedef struct fact_action             s_fact_action;
typedef struct fact_w                  s_fact_w;
typedef struct facts                   s_facts;
typedef struct facts_transaction       s_facts_transaction;
typedef struct facts_cursor            s_facts_cursor;
typedef struct facts_spec_cursor       s_facts_spec_cursor;
typedef struct facts_with_cursor       s_facts_with_cursor;
typedef struct facts_with_cursor_level s_facts_with_cursor_level;
typedef struct file_stat               s_file_stat;
typedef struct float_s                 s_float;
typedef struct fn                      s_fn;
typedef struct fn_clause               s_fn_clause;
typedef struct frame                   s_frame;
typedef struct ht                      s_ht;
typedef struct ident                   s_ident;
typedef struct integer                 s_integer;
typedef struct integer_fraction        s_integer_fraction;
typedef struct list                    s_list;
typedef struct fact_list               s_fact_list;
typedef struct list                    s_list_map;
typedef struct log                     s_log;
typedef struct loop_context            s_loop_context;
typedef struct map                     s_map;
typedef struct marshall                s_marshall;
typedef struct marshall_header         s_marshall_header;
typedef struct marshall_read           s_marshall_read;
typedef struct mutex                   s_mutex;
typedef struct op                      s_op;
typedef struct ops                     s_ops;
typedef struct pretty                  s_pretty;
typedef struct pretty_save             s_pretty_save;
typedef struct queue                   s_queue;
typedef struct quote                   s_quote;
typedef struct ratio                   s_ratio;
typedef struct rwlock                  s_rwlock;
typedef struct sequence                s_sequence;
typedef struct str                     s_str;
typedef struct struct_                 s_struct;
typedef struct struct_type             s_struct_type;
typedef struct sym                     s_sym;
typedef struct sym_list                s_sym_list;
typedef struct tag                     s_tag;
typedef struct tag_type_list           s_tag_type_list;
typedef struct time                    s_time;
typedef struct timespec                s_timespec;
typedef struct tuple                   s_tuple;
typedef struct type                    s_type;
typedef struct unquote                 s_unquote;
typedef struct unwind_protect          s_unwind_protect;
typedef struct var                     s_var;

/* unions */
typedef union callable_data u_callable_data;
typedef union ptr_          u_ptr;
typedef union ptr_w         u_ptr_w;
typedef union tag_data      u_tag_data;

/* typedefs */
typedef u32            character;
typedef s_tag *        t_facts_spec[];
typedef SHA1_CTX       t_hash;
typedef u64            t_skiplist_height;

/* pointer types */
typedef s_callable *   p_callable;
typedef s_complex *    p_complex;
typedef s_cow *        p_cow;
typedef s_tag **       p_facts_spec;
typedef s_list *       p_list;
typedef s_struct *     p_struct;
typedef s_struct_type *p_struct_type;
typedef const s_sym *  p_sym;
typedef s_tag *        p_tag;
typedef s_var *        p_var;

/* function typedefs */
typedef void (* f_clean) (void *x);
typedef bool (* f_sequence) (s_sequence *seq);
typedef bool (* f_sequence_button) (s_sequence *seq, u8 button, s64 x,
                                    s64 y);

#define CHARACTER_MAX S32_MAX
#define SKIPLIST_HEIGHT_MAX U64_MAX

/* 1 */

struct array_dimension {
  uw count;
  uw item_size;
};

struct buf_fd {
  s64 fd;
};

struct buf_save {
  s_buf_save *next;
  uw line;
  uw rpos;
  uw wpos;
};

struct do_block {
  uw count;
  s_tag *tag;
  bool short_form;
};

struct fact {
  s_tag *subject;
  s_tag *predicate;
  s_tag *object;
  uw id; /* serial id */
};

struct facts_transaction {
  s_fact_action *log;
  s_facts_transaction *next;
};

struct fn_clause {
  uw arity;
  s_list *pattern;
  s_do_block algo;
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

struct marshall_header {
  u64 le_magic;
  u64 le_heap_count;
  u64 le_heap_size;
  u64 le_buf_size;
} __attribute((packed));

struct mutex {
  pthread_mutex_t mutex;
};

struct pretty {
  uw base_column;
  sw column;
  bool disable;
};

struct pretty_save {
  uw base_column;
};

union ptr_ {
  const void *p;
  const char *pchar;
  const s8   *ps8;
  const u8   *pu8;
  uw          uw;
};

union ptr_w {
  void *p;
  char *pchar;
  s8   *ps8;
  u8   *pu8;
  uw    uw;
};

struct queue {
  uw count;
  s_list *head;
  s_list *tail;
};

struct quote {
  s_tag *tag;
};

struct rwlock {
  pthread_rwlock_t rwlock;
  sw               count;
  pthread_t        thread;
};

struct struct_ {
  void *data;
  bool free_data;
  s_tag *tag;
  p_struct_type pstruct_type;
  s_mutex mutex;
  bool    mutex_ready;
  sw ref_count;
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

struct time {
  sw tv_sec;
  sw tv_nsec;
  s_tag *tag;
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
  sw              (*flush) (s_buf *buf);
  bool              free;
  sw                line;
  s_pretty          pretty;
  u_ptr_w           ptr;
  bool              read_only;
  sw              (*refill) (s_buf *buf);
  uw                rpos;
  s_rwlock          rwlock;
  s_buf_save       *save;
  sw              (*seek) (s_buf *buf, sw offset, u8 whence);
  uw                size;
  void *            user_ptr;
  uw                wpos;
};

struct cow {
  const s_sym *type;
  s_list *list;
  s_mutex mutex; // TODO
  sw ref_count;
};

struct facts_spec_cursor {
  p_facts_spec spec;
  s_tag *subject;
  uw pos;
};

struct file_stat {
  uw st_dev;      /* ID of device containing file */
  uw st_ino;      /* Inode number */
  s_list * st_mode; /* File type and mode */
  uw st_nlink;    /* Number of hard links */
  uw st_uid;      /* User ID of owner */
  uw st_gid;      /* Group ID of owner */
  uw st_rdev;     /* Device ID (if special file) */
  uw st_size;     /* Total size, in bytes */
  uw st_blksize;  /* Block size for filesystem I/O */
  uw st_blocks;   /* Number of 512B blocks allocated */
  s_time st_atim; /* Time of last access */
  s_time st_ctim; /* Time of last status change */
  s_time st_mtim; /* Time of last modification */
};

struct ht {
  s8        (* compare) (const s_tag *a, const s_tag *b);
  uw           count;
  uw        (* hash) (const s_tag *tag);
  s_list     **items;
  s_rwlock     rwlock;
  uw           size;
  const s_sym *type;
};

struct ident {
  const s_sym *module;
  const s_sym *sym;
};

struct integer {
  mp_int mp_int;
};

struct loop_context {
  jmp_buf continue_buf;
  jmp_buf break_buf;
  s_unwind_protect up;
  s_loop_context *next;
};

struct str {
  u_ptr_w free;        /**< Pointer to free or NULL. */
  u32     size;        /**< Size in bytes. */
  u_ptr   ptr;         /**< Pointer to memory. */
};

struct struct_type {
  f_clean clean;
  s_map map;
  const s_sym *module;
  bool must_clean;
  uw *offset;
  uw size;
  s_mutex mutex;
  sw ref_count;
};

/* 3 */

struct buf_rw {
  s_buf *r;
  s_buf *w;
};

struct call {
  s_ident ident;
  s_list *arguments;
  p_callable pcallable;
};

struct cfn {
  bool macro;
  bool special_operator;
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
  bool ready;
  s_mutex mutex;
};

struct fn {
  bool macro;
  bool special_operator;
  s_ident ident;
  s_fn_clause *clauses;
  const s_sym *module;
  s_frame *frame;
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

struct marshall_read {
  s_buf buf;
  uw    buf_size;
  s_buf heap;
  uw    heap_count;
  uw    heap_size;
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
  s_mutex *mutex;
  sw ref_count;
};

union callable_data {
  s_cfn cfn;
  s_fn fn;
};

struct marshall {
  s_buf buf;
  sw    buf_pos;
  s_buf heap;
  sw    heap_count;
  sw    heap_pos;
  s_ht  ht;
};

/* 5 */

struct callable {
  e_callable_type type;
  u_callable_data data;
  s_mutex mutex;
  sw ref_count;
};

union tag_data {
  s_array       array;
  s_do_block    do_block;
  bool          bool_;
  s_call        call;
  character     character;
  f32           f32;
  f64           f64;
  f128          f128;
  s_fact        fact;
  s_ident       ident;
  s_integer     integer;
  s_map         map;
  p_complex     pcomplex;
  p_cow         pcow;
  p_callable    pcallable;
  p_list        plist;
  p_struct      pstruct;
  p_struct_type pstruct_type;
  const s_sym  *psym;
  p_tag         ptag;
  u_ptr_w       ptr;
  u_ptr_w       ptr_free;
  p_var         pvar;
  s_quote       quote;
  s_ratio       ratio;
  s_str         str;
  s8            s8;
  s16           s16;
  s32           s32;
  s64           s64;
  sw            sw;
  s_time        time;
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

struct block {
  jmp_buf buf;
  const s_sym *name;
  s_tag tag;
  s_block *next;
};

struct complex {
  s_tag x;
  s_tag y;
};

struct error_handler
{
  s_list *stacktrace;
  jmp_buf jmp_buf;
  s_error_handler *next;
  s_tag tag;
};

struct fact_action {
  e_fact_action action;
  s_fact fact;
  s_tag object;
  s_fact_action *next;
};

struct fact_w {
  s_tag subject;
  s_tag predicate;
  s_tag object;
  uw id; /* serial id */
};

struct list {
  s_tag tag;
  s_tag next;
  s_mutex mutex; // TODO
  sw ref_count;
};

struct op {
  const s_sym *sym;
  u8           arity;
  bool         special;
  u8           precedence;
  u8           associativity;
  p_callable   pcallable;
};

struct ops {
  s_ht ht;
};

struct sequence {
  s_tag tag;
  f64 dt;
  f64 duration;
  u64 frame;
  f64 t;
  s_timespec t0;
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

struct var {
  bool         bound;
  s_mutex      mutex;
  bool         ready;
  sw           ref_count;
  s_tag        tag;
  const s_sym *type;
};

/* 8 */

struct facts {
  s_set__tag           tags;
  s_set__fact          facts;
  s_skiplist__fact    *index_spo;
  s_skiplist__fact    *index_pos;
  s_skiplist__fact    *index_osp;
  s_log               *log;
  uw                   next_id;
  s_rwlock             rwlock;
  s_facts_transaction *transaction;
};

struct facts_cursor {
  s_facts *facts;
  s_skiplist__fact *index;
  s_skiplist_node__fact *node;
  s_fact start;
  s_fact end;
  p_var pvar_subject;
  p_var pvar_predicate;
  p_var pvar_object;
  pthread_mutex_t mutex;
};

/* 9 */

struct env {
  sw                argc;
  char            **argv;
  s_str            *argv0_dir;
  s_block          *block;
  s_time            boot_time;
  const s_sym      *current_defmodule;
  s_buf            *err;
  s_error_handler  *error_handler;
  s_facts          *facts;
  s_frame          *frame;
  s_frame          *global_frame;
  s_buf            *in;
  bool              loaded;
  s_loop_context   *loop_context;
  s_str            *module_path;
  s_ops            *ops;
  s_buf            *out;
  s_env            *parent_env;
  bool              pass_by_copy;
  s_list           *path;
  uw                quote_level;
  s_frame          *read_time_frame;
  s_list           *search_modules;
  s_list           *search_modules_default;
  bool              silence_errors;
  s_list           *stacktrace;
  sw                stacktrace_depth;
  s_frame           toplevel_frame;
  bool              trace;
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
  s_mutex mutex;
};

#ifdef __cplusplus
}
#endif

#endif /* LIBKC3_TYPES_H */
