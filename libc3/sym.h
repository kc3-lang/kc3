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
/**
 * @file sym.h
 * @brief Symbol data structure.
 *
 * Structure to manipulate symbols.
 * Symbols are strings that are deduplicated.
 * They are allocated once and only free'd at the end of the program.
 * Comparing symbols is fast : it's a pointer comparison.
 */
#ifndef LIBC3_SYM_H
#define LIBC3_SYM_H

#include "types.h"

#define SYM_MAX 1024

const s_sym  * sym_1 (const s8 *p);
const s_sym ** sym_init_1 (const s_sym **sym, const s8 *p);
const s_sym ** sym_init_cast (const s_sym **sym, const s_tag *tag);
const s_sym ** sym_init_copy (const s_sym **sym,
                              const s_sym * const *src);
const s_sym ** sym_init_str (const s_sym **sym, const s_str *src);


/* Heap-allocation functions, call sym_delete_all at exit. */
void          sym_delete_all (void);
const s_sym * sym_new (const s_str *src);

/* Observers */
bool          sym_character_is_reserved (character c);
const s_sym * sym_find (const s_str *src);
bool          sym_has_reserved_characters (const s_sym *sym);
s_str *       sym_inspect (const s_sym *sym, s_str *dest);
bool          sym_is_module (const s_sym *sym);
bool          sym_to_ffi_type (const s_sym *sym, ffi_type *result_type,
                               ffi_type **dest);
bool          sym_to_tag_type (const s_sym *sym, e_tag_type *dest);
bool          sym_type_size (const s_sym *type, uw *size);

#endif /* LIBC3_SYM_H */
