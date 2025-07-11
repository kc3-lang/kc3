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

#ifndef LIBKC3_MARSHALL_H
#define LIBKC3_MARSHALL_H

#include "types.h"

/*
 * This header is written at the beginning of every marshall'd data
 * written on the disk to be recovered when we want to read the file
 * to unmarshall.
*/
typedef struct __attribute__((packed)) marshall_header {
  e_tag_type type;    // Used if someday we want to auto instanciate.
  size_t buf_size;
  size_t buf_offset;  // Byte offset where the buffer bytes starts.
  size_t heap_size;
  size_t heap_offset; // Byte offset where the heap bytes starts.
} marshall_header_t;

#define PROTO_MARSHALL(name, type)                                    \
  s_marshall * marshall_ ## name (s_marshall *marshall, type src)

/*
 * Is a wrapper function really needed ? If so, don't forget endianess.
*/
#define MARSHALL_READ(marshall, type, store_in)                       \
    (buf_read_ ## type (&(marshall)->buf, &(store_in)) > -1           \
      ? (store_in)                                                    \
      : -1)

/* Stack-allocation compatible functions, call marshall_clean
   after use. */
void         marshall_clean (s_marshall *marshall);
s_marshall * marshall_init (s_marshall *marshall);

/* Heap-allocation functions, call marshall_delete after use. */
void         marshall_delete (s_marshall *marshall);
s_marshall * marshall_new (void);

/* Operators. */
PROTO_MARSHALL(array, const s_array *);
PROTO_MARSHALL(bool, bool);
PROTO_MARSHALL(call, const s_call *);
PROTO_MARSHALL(callable, const s_callable *);
PROTO_MARSHALL(character, character);
PROTO_MARSHALL(cow, const s_cow *);
PROTO_MARSHALL(complex, const s_complex *);
PROTO_MARSHALL(do_block, const s_do_block *);
PROTO_MARSHALL(f128, f128);
PROTO_MARSHALL(f32, f32);
PROTO_MARSHALL(f64, f64);
PROTO_MARSHALL(fact, const s_fact *);
PROTO_MARSHALL(list, const s_list *);
PROTO_MARSHALL(ident, const s_ident *);
PROTO_MARSHALL(integer, const s_integer *);
PROTO_MARSHALL(map, const s_map *);
PROTO_MARSHALL(quote, const s_quote *);
PROTO_MARSHALL(ratio, const s_ratio *);
PROTO_MARSHALL(s8, s8);
PROTO_MARSHALL(s16, s16);
PROTO_MARSHALL(s32, s32);
PROTO_MARSHALL(s64, s64);
PROTO_MARSHALL(struct, const s_struct *);
PROTO_MARSHALL(struct_type, const s_struct_type *);
PROTO_MARSHALL(sym, const s_sym *);
PROTO_MARSHALL(str, const s_str *);
PROTO_MARSHALL(sw,  sw);
PROTO_MARSHALL(tag, const s_tag *);
PROTO_MARSHALL(tuple, const s_tuple *);
PROTO_MARSHALL(time, const s_time *);
PROTO_MARSHALL(u8,  u8);
PROTO_MARSHALL(u16, u16);
PROTO_MARSHALL(u32, u32);
PROTO_MARSHALL(u64, u64);
PROTO_MARSHALL(unquote, const s_unquote *);
PROTO_MARSHALL(uw, uw);
PROTO_MARSHALL(var, const s_var *);

/* Export. */
sw           marshall_to_buf (s_marshall *marshall,
                              s_buf *buf);
s_str *      marshall_to_str (s_marshall *marshall,
                              s_str *dest);
/**
 * @brief Write the marshall struct in a file.
 *
 * This function opens a file in wb (binary write) to write marshall'd
 * data. If the file can't be opened, the finction returns -1. After
 * the operation, the file is then closed.
 *
 * @param marshall self argument
 * @param path Where the marshall'd data will be saved.
 *
 * @return Zero in case of success or -1.
 *
 */
ssize_t marshall_save_to_file(s_marshall *marshall, const char *path);

/**
 * @brief Load the marshall'd data from a file.
 *
 * This function reads a file, and parses the file header to retrieve
 * a valid s_marshall set of bytes that can then be used with
 * `marshall_read_<type>(s_marshall, param *) to retrieve the data.
 * The file is read at once in an allocated buffer because the size
 * of the file is not known before (and may be > stack size).
 *
 * @param path Where the marshall'd data is saved.
 *
 * @return A new s_marshall allocated pointer or NULL.
 *
 */
s_marshall *marshall_read_from_file(const char *path);

#endif /* LIBKC3_MARSHALL_H */
