/* c3
 * Copyright 2022 kmx.io <contact@kmx.io>
 *
 * Permission is hereby granted to use this software granted
 * the above copyright notice and this permission paragraph
 * are included in all copies and substantial portions of this
 * software.
 *
 * THIS SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY GUARANTEE OF
 * PURPOSE AND PERFORMANCE. IN NO EVENT WHATSOEVER SHALL THE
 * AUTHOR BE CONSIDERED LIABLE FOR THE USE AND PERFORMANCE OF
 * THIS SOFTWARE.
 */
#ifndef TYPES_H
#define TYPES_H

#include <setjmp.h>
#include <stdio.h>
#include "../libtommath/tommath.h"

/* Basic integer types. */
typedef char                s8;
typedef short               s16;
typedef int                 s32;
typedef long                sw;
typedef long long           s64;
typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long       uw;
typedef unsigned long long  u64;

/* IEEE 754 floating point numbers. */
typedef float               f32;
typedef double              f64;

/* Boolean : true or false. */
typedef s8 bool;

typedef enum {
  false = 0,
  true = 1
} e_bool;

/* character */
typedef s32 character;

/* integer */
typedef struct integer {
  mp_int mp_int;
} s_integer;

/* ptr */
typedef union ptr {
  const void *p;
  const s8   *ps8;
  const u8   *pu8;
} u_ptr;

typedef union ptr_w {
  void *p;
  s8   *ps8;
  u8   *pu8;
} u_ptr_w;

/* str */
typedef struct str {
  u_ptr_w free;        /**< Pointer to free or NULL. */
  uw      size;        /**< Size in bytes. */
  u_ptr   ptr;         /**< Pointer to memory. */
} s_str;

/* sym */
typedef struct sym_list s_sym_list;

typedef struct sym {
  s_str str;
} s_sym;

struct sym_list {
  s_sym *sym;
  s_sym_list *next;
};

/* ident */
typedef struct ident {
  const s_sym *module;
  const s_sym *sym;
} s_ident;

/* buf */
typedef struct buf s_buf;
typedef struct buf_save s_buf_save;

struct buf_save {
  s_buf_save *next;
  uw rpos;
  uw wpos;
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
  uw          size;
  void       *user_ptr;
  u64         wpos;
};

/* tag */
typedef struct tag  s_tag;
typedef struct list s_list;

typedef s_tag * p_quote;
typedef const s_tag * p_tag;

typedef struct tuple {
  uw count;
  s_tag *tag;
} s_tuple;

typedef struct call {
  s_ident ident;
  s_list *args;
} s_call;

typedef const s_tag * p_var;

typedef enum tag_type {
  TAG_VOID = 0,
  TAG_BOOL = 1,
  TAG_CHARACTER,
  TAG_F32,
  TAG_F64,
  TAG_CALL,
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

typedef union tag_type_ {
  e_tag_type type;
} u_tag_type;

typedef union tag_data {
  bool         bool;
  character    character;
  f32          f32;
  f64          f64;
  s_call       call;
  s_ident      ident;
  s_integer    integer;
  s_list      *list;
  p_tag        ptag;
  p_quote      quote;
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
  p_var        var;
} u_tag_data;

struct tag {
  u_tag_type type;
  u_tag_data data;
};

/* list */
struct list {
  s_tag tag;
  s_tag next;
};

/* fact */
typedef struct fact {
  const s_tag *subject;
  const s_tag *predicate;
  const s_tag *object;
  uw id;
} s_fact;

/* set */
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

/* skiplist */
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

/* facts */
typedef struct facts {
  s_set__tag        tags;
  s_set__fact       facts;
  s_skiplist__fact *index_spo;
  s_skiplist__fact *index_pos;
  s_skiplist__fact *index_osp;
  s_buf            *log;
} s_facts;

typedef struct facts_cursor {
  s_skiplist__fact *tree;
  s_skiplist_node__fact *node;
  s_fact start;
  s_fact end;
  s_tag *var_subject;
  s_tag *var_predicate;
  s_tag *var_object;
} s_facts_cursor;

typedef s_tag ** p_facts_spec;

typedef struct facts_spec_cursor {
  p_facts_spec spec;
  const s_tag *subject;
  uw pos;
} s_facts_spec_cursor;

typedef struct facts_with_cursor_level {
  s_facts_cursor cursor;
  s_fact *fact;
  p_facts_spec spec;
} s_facts_with_cursor_level;

typedef struct facts_with_cursor {
  const s_facts *facts;
  s_list *bindings;
  size_t facts_count;
  s_facts_with_cursor_level *l;
  size_t level;
  p_facts_spec spec;
} s_facts_with_cursor;

/* module */
typedef struct module {
  const s_sym *name;
  s_facts *facts;
} s_module;

/* error_handler */
typedef struct error_handler s_error_handler;

struct error_handler
{
  jmp_buf jmp_buf;
  s_error_handler *next;
};

/* frame */
typedef struct frame s_frame;

struct frame {
  s_list *bindings;
  s_frame *next;
};

/* env */
typedef struct env {
  s_frame *frame;
  s_error_handler *error_handler;
} s_env;

#endif /* TYPES_H */
