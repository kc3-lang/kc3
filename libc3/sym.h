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
#ifndef SYM_H
#define SYM_H

#include "types.h"

#define SYM_MAX 1024

/** @brief Make symbol from C string.
 *  @sa str_to_sym
 */
const s_sym * sym_1 (const s8 *p);

bool sym_character_is_reserved (character c);

/** @brief Call when exiting program. */
void sym_delete_all ();

/** @brief Find an existing symbol. */
const s_sym * sym_find (const s_str *src);

bool sym_has_reserved_characters (const s_sym *sym);

s_str * sym_inspect (const s_sym *sym, s_str *dest);

/** @brief True iff sym is a module name (starts with a capital). */
bool sym_is_module (const s_sym *sym);

const s_sym * sym_new (const s_str *src);

ffi_type * sym_to_ffi_type (const s_sym *sym, ffi_type *result_type);

bool sym_to_tag_type (const s_sym *sym, e_tag_type *dest);

#endif /* SYM_H */
